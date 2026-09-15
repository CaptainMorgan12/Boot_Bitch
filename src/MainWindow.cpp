#include "MainWindow.h"

#include <algorithm>
#include <QApplication>
#include <QBoxLayout>
#include <QBrush>
#include <QAbstractItemView>
#include <QClipboard>
#include <QCoreApplication>
#include <QAction>
#include <QDir>
#include <QKeySequence>
#include <QPixmap>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColor>
#include <QPalette>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QImage>
#include <QPainter>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QModelIndex>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QSettings>
#include <QSize>
#include <QSizePolicy>
#include <QStyle>
#include <QSplitter>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QStatusBar>
#include <QTabBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextOption>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

namespace {
enum AppTab {
    SystemsTab = 0,
    DiagnosticsTab,
    RepairTab,
    SnapshotsTab,
    ChrootShellTab,
    FileCopyTab,
    LogsTab,
    SettingsTab
};

struct DiagnosticSpec {
    const char *key;
    const char *title;
    const char *description;
    const char *icon;
};

class RepairProgressDialog final : public QDialog
{
public:
    explicit RepairProgressDialog(QWidget *parent = nullptr)
        : QDialog(parent)
    {
    }

    void setCloseAllowed(bool allowed)
    {
        m_closeAllowed = allowed;
    }

public:
    void reject() override
    {
        if (m_closeAllowed) {
            QDialog::reject();
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_closeAllowed) {
            QDialog::closeEvent(event);
        } else {
            event->ignore();
        }
    }

private:
    bool m_closeAllowed = false;
};

static const DiagnosticSpec diagnosticSpecs[] = {
    {"environment", "Environment validation", "Summarizes the selected system, protection state, mounted identity and inspection readiness.", "task-complete"},
    {"boot", "Boot diagnostics", "Shows boot mounts, /boot and EFI contents plus storage evidence without changing the selected system.", "system-run"},
    {"boot-evidence", "Boot evidence and selection history", "Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected.", "dialog-information"},
    {"kernel", "Kernel / initramfs", "Reviews kernel files and verifies matching initramfs images through a read-only inspection.", "preferences-system"},
    {"grub", "GRUB configuration", "Reviews GRUB configuration and /etc/default/grub without changing boot files.", "preferences-system"},
    {"uki", "EFI / UKI boot state", "Inspects the selected ESP, TUXEDO UKI embedded kernel/cmdline and firmware entries with PARTUUID ownership classification.", "drive-removable-media"},
    {"display", "Graphical login / display manager", "Reviews graphical.target, the configured display manager (for example SDDM, GDM3, LightDM, or another systemd manager), installed desktop packages, and recent boot/journal evidence without starting the GUI.", "video-display"},
    {"errors", "Boot errors", "Reads recent error-priority entries from the running host or selected repair system's persistent journal when available.", "dialog-warning"},
    {"usage", "Disk usage", "Summarizes filesystem capacity/free space for the running host or read-only repair target.", "drive-harddisk"},
    {"fstab", "fstab", "Displays the running host or selected repair system's fstab; repair-system inspection is mounted read-only.", "text-x-generic"},
    {"btrfs", "Btrfs status", "Shows Btrfs filesystem and subvolume information for the running host or selected repair system.", "drive-harddisk"},
    {"mapper", "Mapper status", "Shows selected mapper ancestry, device-mapper state and cryptsetup status when available.", "document-encrypt"},
    {"luks", "LUKS / crypttab", "Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.", "document-encrypt"},
    {"report", "Full diagnostic report", "Combines all read-only diagnostics for the selected scope in one privileged inspection session.", "document-preview"}
};

QIcon terminalIcon(const QColor &foreground)
{
    QIcon icon;
    for (const int size : {16, 20, 22, 24, 32, 48, 64}) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        QPen pen(foreground, qMax(1, size / 10));
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        const QRectF frame(size * 0.12, size * 0.18, size * 0.76, size * 0.62);
        painter.drawRoundedRect(frame, size * 0.08, size * 0.08);
        painter.drawLine(QPointF(size * 0.28, size * 0.39), QPointF(size * 0.40, size * 0.49));
        painter.drawLine(QPointF(size * 0.40, size * 0.49), QPointF(size * 0.28, size * 0.59));
        painter.drawLine(QPointF(size * 0.48, size * 0.59), QPointF(size * 0.68, size * 0.59));
        icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
        icon.addPixmap(pixmap, QIcon::Disabled, QIcon::Off);
    }
    return icon;
}

// AppImages may run without any discoverable host icon theme, and a few Qt
// styles return an empty standard icon in that case.  Keep the interface
// usable by drawing small, high-contrast glyphs ourselves as the final
// fallback.  These are deliberately simple geometric marks so they render
// consistently with both the raster and vector Qt backends.
QIcon glyphIcon(const QString &name, const QColor &requestedForeground)
{
    const QPalette palette = QApplication::palette();
    QColor foreground = requestedForeground;
    if (foreground.lightness() < 120) {
        foreground = palette.color(QPalette::WindowText);
    }
    if (foreground.lightness() < 120) {
        foreground = QColor(235, 235, 235);
    }

    QIcon icon;
    for (const int size : {16, 20, 22, 24, 32, 48, 64}) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        const qreal s = static_cast<qreal>(size);
        const qreal width = qMax<qreal>(1.5, s / 9.0);
        QPen pen(foreground, width);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        if (name.contains(QStringLiteral("warning")) || name.contains(QStringLiteral("error"))) {
            QPolygonF triangle;
            triangle << QPointF(s * 0.50, s * 0.12)
                     << QPointF(s * 0.88, s * 0.83)
                     << QPointF(s * 0.12, s * 0.83);
            painter.drawPolygon(triangle);
            painter.drawLine(QPointF(s * 0.50, s * 0.34), QPointF(s * 0.50, s * 0.58));
            painter.drawPoint(QPointF(s * 0.50, s * 0.70));
        } else if (name.contains(QStringLiteral("task-complete")) || name.contains(QStringLiteral("dialog-ok"))
                   || name == QStringLiteral("ok") || name.contains(QStringLiteral("apply"))) {
            painter.drawEllipse(QRectF(s * 0.14, s * 0.14, s * 0.72, s * 0.72));
            painter.drawLine(QPointF(s * 0.29, s * 0.50), QPointF(s * 0.44, s * 0.65));
            painter.drawLine(QPointF(s * 0.44, s * 0.65), QPointF(s * 0.72, s * 0.34));
        } else if (name.contains(QStringLiteral("refresh"))) {
            painter.drawArc(QRectF(s * 0.18, s * 0.18, s * 0.64, s * 0.64), 40 * 16, 285 * 16);
            QPolygonF arrow;
            arrow << QPointF(s * 0.74, s * 0.16)
                  << QPointF(s * 0.86, s * 0.17)
                  << QPointF(s * 0.82, s * 0.30);
            painter.setBrush(foreground);
            painter.drawPolygon(arrow);
        } else if (name.contains(QStringLiteral("copy"))) {
            painter.drawRoundedRect(QRectF(s * 0.28, s * 0.14, s * 0.52, s * 0.58), s * 0.05, s * 0.05);
            painter.drawRoundedRect(QRectF(s * 0.14, s * 0.30, s * 0.52, s * 0.56), s * 0.05, s * 0.05);
        } else if (name.contains(QStringLiteral("terminal"))) {
            return terminalIcon(foreground);
        } else if (name.contains(QStringLiteral("lock")) || name.contains(QStringLiteral("password"))
                   || name.contains(QStringLiteral("encrypt")) || name.contains(QStringLiteral("unlocked"))) {
            painter.drawRoundedRect(QRectF(s * 0.20, s * 0.42, s * 0.60, s * 0.42), s * 0.05, s * 0.05);
            painter.drawArc(QRectF(s * 0.32, s * 0.16, s * 0.36, s * 0.50), 0, 180 * 16);
            painter.drawLine(QPointF(s * 0.50, s * 0.55), QPointF(s * 0.50, s * 0.69));
        } else if (name.contains(QStringLiteral("drive")) || name == QStringLiteral("computer")
                   || name.contains(QStringLiteral("removable"))) {
            painter.drawRoundedRect(QRectF(s * 0.12, s * 0.22, s * 0.76, s * 0.56), s * 0.06, s * 0.06);
            painter.drawLine(QPointF(s * 0.22, s * 0.62), QPointF(s * 0.78, s * 0.62));
            painter.drawPoint(QPointF(s * 0.70, s * 0.46));
        } else if (name.contains(QStringLiteral("folder"))) {
            QPolygonF folder;
            folder << QPointF(s * 0.12, s * 0.28) << QPointF(s * 0.42, s * 0.28)
                   << QPointF(s * 0.50, s * 0.18) << QPointF(s * 0.86, s * 0.18)
                   << QPointF(s * 0.86, s * 0.78) << QPointF(s * 0.12, s * 0.78);
            painter.drawPolygon(folder);
        } else if (name.contains(QStringLiteral("settings")) || name.contains(QStringLiteral("preferences"))) {
            for (const qreal y : {s * 0.30, s * 0.50, s * 0.70}) {
                painter.drawLine(QPointF(s * 0.16, y), QPointF(s * 0.84, y));
            }
            painter.setBrush(foreground);
            painter.drawEllipse(QPointF(s * 0.34, s * 0.30), width, width);
            painter.drawEllipse(QPointF(s * 0.65, s * 0.50), width, width);
            painter.drawEllipse(QPointF(s * 0.42, s * 0.70), width, width);
        } else if (name.contains(QStringLiteral("run")) || name.contains(QStringLiteral("play"))) {
            QPolygonF play;
            play << QPointF(s * 0.30, s * 0.16) << QPointF(s * 0.76, s * 0.50)
                 << QPointF(s * 0.30, s * 0.84);
            painter.drawPolygon(play);
        } else if (name.contains(QStringLiteral("help")) || name.contains(QStringLiteral("information"))) {
            painter.drawEllipse(QRectF(s * 0.16, s * 0.16, s * 0.68, s * 0.68));
            painter.drawText(QRectF(0, s * 0.16, s, s * 0.68), Qt::AlignCenter, QStringLiteral("i"));
        } else if (name.contains(QStringLiteral("display")) || name.contains(QStringLiteral("video"))) {
            painter.drawRect(QRectF(s * 0.14, s * 0.18, s * 0.72, s * 0.52));
            painter.drawLine(QPointF(s * 0.38, s * 0.82), QPointF(s * 0.62, s * 0.82));
            painter.drawLine(QPointF(s * 0.50, s * 0.70), QPointF(s * 0.50, s * 0.82));
        } else if (name.contains(QStringLiteral("remove")) || name.contains(QStringLiteral("clear"))) {
            painter.drawLine(QPointF(s * 0.22, s * 0.50), QPointF(s * 0.78, s * 0.50));
        } else if (name.contains(QStringLiteral("document")) || name.contains(QStringLiteral("file"))
                   || name.contains(QStringLiteral("text")) || name.contains(QStringLiteral("log"))) {
            painter.drawRect(QRectF(s * 0.22, s * 0.12, s * 0.56, s * 0.76));
            for (const qreal y : {s * 0.36, s * 0.52, s * 0.68}) {
                painter.drawLine(QPointF(s * 0.34, y), QPointF(s * 0.66, y));
            }
        } else {
            painter.drawEllipse(QRectF(s * 0.18, s * 0.18, s * 0.64, s * 0.64));
            painter.drawLine(QPointF(s * 0.34, s * 0.50), QPointF(s * 0.66, s * 0.50));
            painter.drawLine(QPointF(s * 0.50, s * 0.34), QPointF(s * 0.50, s * 0.66));
        }

        icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
        icon.addPixmap(pixmap, QIcon::Disabled, QIcon::Off);
    }
    return icon;
}

QIcon fallbackGlyphIcon(const QString &name, const QColor &foreground)
{
    QString glyph = QStringLiteral("?");
    if (name.contains(QStringLiteral("drive")) || name == QStringLiteral("computer")) glyph = QStringLiteral("D");
    else if (name.contains(QStringLiteral("warning")) || name.contains(QStringLiteral("error"))) glyph = QStringLiteral("!");
    else if (name.contains(QStringLiteral("information")) || name.contains(QStringLiteral("help"))) glyph = QStringLiteral("i");
    else if (name.contains(QStringLiteral("terminal"))) glyph = QStringLiteral(">");
    else if (name.contains(QStringLiteral("copy"))) glyph = QStringLiteral("C");
    else if (name.contains(QStringLiteral("refresh"))) glyph = QStringLiteral("R");
    else if (name.contains(QStringLiteral("settings")) || name.contains(QStringLiteral("configure"))) glyph = QStringLiteral("S");
    else if (name.contains(QStringLiteral("wizard")) || name.contains(QStringLiteral("repair"))) glyph = QStringLiteral("W");
    else if (name.contains(QStringLiteral("revert")) || name.contains(QStringLiteral("snapshot"))) glyph = QStringLiteral("V");
    else if (name.contains(QStringLiteral("log"))) glyph = QStringLiteral("L");
    else if (name.contains(QStringLiteral("lock")) || name.contains(QStringLiteral("encrypt"))) glyph = QStringLiteral("K");

    QIcon icon;
    for (const int size : {16, 20, 22, 24, 32, 48, 64}) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(foreground, qMax(1, size / 12)));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(QRectF(size * 0.12, size * 0.12, size * 0.76, size * 0.76), size * 0.12, size * 0.12);
        QFont font = painter.font();
        font.setBold(true);
        font.setPixelSize(qMax(8, static_cast<int>(size * 0.58)));
        painter.setFont(font);
        painter.drawText(QRectF(0, 0, size, size), Qt::AlignCenter, glyph);
        icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
        icon.addPixmap(pixmap, QIcon::Disabled, QIcon::Off);
    }
    return icon;
}

// A small semantic atlas is compiled into the executable.  It is used only
// when the active desktop theme does not provide a usable icon, which keeps
// AppImages and minimal GTK/Qt installations visually consistent without
// replacing a user's native theme artwork.
QIcon bundledIcon(const QString &name)
{
    QString atlasName = QStringLiteral("document");
    if (name == QStringLiteral("task-complete") || name == QStringLiteral("dialog-ok-apply")) {
        atlasName = QStringLiteral("check");
    } else if (name == QStringLiteral("system-run") || name == QStringLiteral("tools-wizard")) {
        atlasName = QStringLiteral("run");
    } else if (name == QStringLiteral("view-refresh")) {
        atlasName = QStringLiteral("refresh");
    } else if (name == QStringLiteral("document-edit") || name == QStringLiteral("edit-clear")) {
        atlasName = QStringLiteral("edit");
    } else if (name == QStringLiteral("document-save")) {
        atlasName = QStringLiteral("save");
    } else if (name == QStringLiteral("document-open") || name == QStringLiteral("folder-open")) {
        atlasName = QStringLiteral("open");
    } else if (name == QStringLiteral("document-open-recent")) {
        atlasName = QStringLiteral("snapshot");
    } else if (name == QStringLiteral("list-remove") || name == QStringLiteral("application-exit")) {
        atlasName = QStringLiteral("trash");
    } else if (name == QStringLiteral("security-high")) {
        atlasName = QStringLiteral("security");
    } else if (name == QStringLiteral("dialog-information") || name == QStringLiteral("help-contextual")
               || name == QStringLiteral("help-about") || name == QStringLiteral("help-contents")) {
        atlasName = QStringLiteral("info");
    } else if (name == QStringLiteral("dialog-password")) {
        atlasName = QStringLiteral("question");
    } else if (name == QStringLiteral("system-software-install")) {
        atlasName = QStringLiteral("install");
    } else if (name == QStringLiteral("system-software-update")) {
        atlasName = QStringLiteral("update");
    } else if (name == QStringLiteral("applications-development") || name == QStringLiteral("dkms")) {
        atlasName = QStringLiteral("development");
    } else if (name.contains(QStringLiteral("drive")) || name == QStringLiteral("computer")) {
        atlasName = QStringLiteral("drive");
    } else if (name.contains(QStringLiteral("btrfs")) || name.contains(QStringLiteral("filesystem"))
               || name.contains(QStringLiteral("ext4"))) {
        atlasName = QStringLiteral("filesystem");
    } else if (name.contains(QStringLiteral("configure")) || name.contains(QStringLiteral("settings"))
               || name.contains(QStringLiteral("preferences")) || name.contains(QStringLiteral("repair"))) {
        atlasName = QStringLiteral("gear");
    } else if (name.contains(QStringLiteral("terminal"))) {
        atlasName = QStringLiteral("terminal");
    } else if (name.contains(QStringLiteral("lock")) || name.contains(QStringLiteral("encrypt"))
               || name.contains(QStringLiteral("password")) || name.contains(QStringLiteral("unlocked"))) {
        atlasName = QStringLiteral("lock");
    } else if (name.contains(QStringLiteral("warning")) || name.contains(QStringLiteral("error"))
               || name.contains(QStringLiteral("report-bug"))) {
        atlasName = QStringLiteral("warning");
    } else if (name.contains(QStringLiteral("display")) || name.contains(QStringLiteral("video"))) {
        atlasName = QStringLiteral("display");
    } else if (name.contains(QStringLiteral("copy"))) {
        atlasName = QStringLiteral("copy");
    } else if (name.contains(QStringLiteral("folder")) || name.contains(QStringLiteral("log"))) {
        atlasName = QStringLiteral("folder");
    } else if (name.contains(QStringLiteral("snapshot")) || name.contains(QStringLiteral("revert"))) {
        atlasName = QStringLiteral("snapshot");
    } else if (name.contains(QStringLiteral("help")) || name.contains(QStringLiteral("information"))) {
        atlasName = QStringLiteral("info");
    }
    const QString path = QStringLiteral(":/icons/atlas/%1.svg").arg(atlasName);
    return QFile::exists(path) ? QIcon(path) : QIcon();
}

QIcon themedIcon(const QString &name, const QIcon &fallback = QIcon())
{
    QIcon source = QIcon::fromTheme(name, fallback);
    // Try common freedesktop aliases before drawing a fallback.  Themes often
    // ship a semantically equivalent name rather than every application
    // specific name used by Boot Bitch.
    if (source.isNull() || name == QStringLiteral("preferences-system")
        || name == QStringLiteral("dialog-information")) {
        QStringList aliases;
        if (name == QStringLiteral("preferences-system")) aliases << QStringLiteral("applications-system") << QStringLiteral("preferences-desktop");
        else if (name == QStringLiteral("tools-report-bug")) aliases << QStringLiteral("dialog-warning") << QStringLiteral("applications-development");
        else if (name == QStringLiteral("tools-wizard")) aliases << QStringLiteral("applications-utilities") << QStringLiteral("system-run");
        else if (name == QStringLiteral("document-revert")) aliases << QStringLiteral("document-open-recent") << QStringLiteral("view-refresh");
        else if (name == QStringLiteral("text-x-log")) aliases << QStringLiteral("text-x-generic") << QStringLiteral("document-preview");
        else if (name == QStringLiteral("dialog-information")) aliases << QStringLiteral("help-about") << QStringLiteral("dialog-question");
        else if (name == QStringLiteral("document-preview")) aliases << QStringLiteral("document-properties") << QStringLiteral("document-open");
        else if (name == QStringLiteral("filesystem-btrfs")
                 || name == QStringLiteral("filesystem-ext4")
                 || name == QStringLiteral("filesystem-xfs")
                 || name == QStringLiteral("filesystem-ntfs")
                 || name == QStringLiteral("filesystem-vfat")
                 || name == QStringLiteral("filesystem-swap")) {
            // Filesystem-specific names are optional in all three desktop
            // icon ecosystems.  A disk/partition icon still communicates
            // "this is storage" when a theme has no dedicated filesystem
            // artwork.
            aliases << QStringLiteral("filesystem-generic")
                    << QStringLiteral("drive-partition")
                    << QStringLiteral("drive-harddisk");
        } else if (name == QStringLiteral("filesystem-generic")) aliases << QStringLiteral("drive-partition") << QStringLiteral("drive-harddisk");
        for (const QString &alias : aliases) {
            source = QIcon::fromTheme(alias);
            if (!source.isNull()) break;
        }
    }
    // A few desktop themes return a solid placeholder for an unknown name.
    // Treat that as missing so the semantic high-contrast glyph below is used
    // instead of displaying an unreadable white square.
    if (!source.isNull()) {
        const QImage probe = source.pixmap(24, 24, QIcon::Normal, QIcon::Off).toImage().convertToFormat(QImage::Format_ARGB32);
        if (!probe.isNull()) {
            int visible = 0;
            int uniform = 0;
            QColor reference;
            for (int y = 0; y < probe.height(); ++y) {
                for (int x = 0; x < probe.width(); ++x) {
                    const QColor pixel = probe.pixelColor(x, y);
                    if (pixel.alpha() <= 20) continue;
                    ++visible;
                    if (!reference.isValid()) reference = pixel;
                    if (reference.isValid() && std::abs(pixel.red() - reference.red()) < 8
                        && std::abs(pixel.green() - reference.green()) < 8
                        && std::abs(pixel.blue() - reference.blue()) < 8) {
                        ++uniform;
                    }
                }
            }
            if (visible > 180 && uniform * 10 >= visible * 9) {
                source = QIcon();
            }
        }
    }
    if (source.isNull()) {
        source = bundledIcon(name);
    }
    if (source.isNull() || source.pixmap(24, 24).isNull()) {
        // Portable AppImages do not always inherit the host icon-theme search
        // path. Keep every control legible with a native Qt fallback while
        // still preferring the active GNOME/KDE theme when it is available.
        QStyle::StandardPixmap standard = QStyle::SP_FileIcon;
        bool hasStandard = false;
        if (name.contains(QStringLiteral("drive")) || name == QStringLiteral("computer")) {
            standard = QStyle::SP_ComputerIcon;
            hasStandard = true;
        } else if (name.contains(QStringLiteral("warning")) || name.contains(QStringLiteral("error"))) {
            standard = QStyle::SP_MessageBoxWarning;
            hasStandard = true;
        } else if (name.contains(QStringLiteral("ok")) || name == QStringLiteral("task-complete")) {
            standard = QStyle::SP_DialogApplyButton;
            hasStandard = true;
        } else if (name.contains(QStringLiteral("refresh"))) {
            standard = QStyle::SP_BrowserReload;
            hasStandard = true;
        } else if (name.contains(QStringLiteral("copy"))) {
            standard = QStyle::SP_FileDialogDetailedView;
            hasStandard = true;
        } else if (name.contains(QStringLiteral("terminal"))) {
            standard = QStyle::SP_CommandLink;
            hasStandard = true;
        }
        if (hasStandard) {
            source = QApplication::style()->standardIcon(standard);
        }
        if (!hasStandard || source.isNull()) {
            const QPalette palette = QApplication::palette();
            source = fallbackGlyphIcon(name, palette.color(QPalette::WindowText));
        }
    }
    if (source.isNull() || source.pixmap(24, 24).isNull()) {
        const QPalette palette = QApplication::palette();
        QColor foreground = palette.color(QPalette::WindowText);
        if (palette.color(QPalette::Text).lightness() > foreground.lightness()) {
            foreground = palette.color(QPalette::Text);
        }
        return glyphIcon(name, foreground);
    }

    // Some GNOME/GTK icon themes provide dark monochrome glyphs even when the
    // application palette is dark. Tint only monochrome glyphs to the current
    // text colour so they remain legible, while leaving coloured status and
    // device artwork unchanged.
    const QPalette palette = QApplication::palette();
    const QColor background = palette.color(QPalette::Window);
    QColor foreground = palette.color(QPalette::WindowText);
    const QColor buttonText = palette.color(QPalette::ButtonText);
    const QColor text = palette.color(QPalette::Text);
    if (buttonText.lightness() > foreground.lightness()) {
        foreground = buttonText;
    }
    if (text.lightness() > foreground.lightness()) {
        foreground = text;
    }
    if (background.lightness() >= 150 || foreground.lightness() <= background.lightness()) {
        return source;
    }
    if (name == QStringLiteral("utilities-terminal")) {
        return terminalIcon(foreground);
    }

    QIcon tinted;
    const QList<int> sizes{16, 20, 22, 24, 32, 48, 64};
    for (const int size : sizes) {
        const QPixmap pixmap = source.pixmap(size, QIcon::Normal, QIcon::Off);
        if (pixmap.isNull()) {
            continue;
        }
        QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        int colouredPixels = 0;
        int visiblePixels = 0;
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                const QColor pixel = image.pixelColor(x, y);
                if (pixel.alpha() > 20) {
                    ++visiblePixels;
                    if (pixel.saturationF() > 0.18) {
                        ++colouredPixels;
                    }
                }
            }
        }
        const bool mostlyMonochrome = visiblePixels == 0 || colouredPixels * 8 < visiblePixels;
        if (mostlyMonochrome) {
            for (int y = 0; y < image.height(); ++y) {
                for (int x = 0; x < image.width(); ++x) {
                    QColor pixel = image.pixelColor(x, y);
                    if (pixel.alpha() > 0 && pixel.saturationF() <= 0.18) {
                        pixel.setRed(foreground.red());
                        pixel.setGreen(foreground.green());
                        pixel.setBlue(foreground.blue());
                        image.setPixelColor(x, y, pixel);
                    }
                }
            }
            const QPixmap recolored = QPixmap::fromImage(image);
            // GTK platform styles often render disabled tree/list rows with a
            // separate black glyph. Supply an explicit light disabled variant
            // so stage and diagnostic icons remain visible on dark palettes.
            tinted.addPixmap(recolored, QIcon::Normal, QIcon::Off);
            tinted.addPixmap(recolored, QIcon::Disabled, QIcon::Off);
        } else {
            tinted.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
            tinted.addPixmap(pixmap, QIcon::Disabled, QIcon::Off);
        }
    }
    return tinted.isNull() ? source : tinted;
}

QIcon filesystemIcon(const QString &fileSystem)
{
    const QString fs = fileSystem.trimmed().toLower();
    if (fs == QStringLiteral("btrfs")) {
        return themedIcon(QStringLiteral("filesystem-btrfs"));
    }
    if (fs == QStringLiteral("ext4") || fs == QStringLiteral("ext3") || fs == QStringLiteral("ext2")) {
        return themedIcon(QStringLiteral("filesystem-ext4"));
    }
    if (fs == QStringLiteral("xfs")) {
        return themedIcon(QStringLiteral("filesystem-xfs"));
    }
    if (fs == QStringLiteral("ntfs") || fs == QStringLiteral("ntfs3")) {
        return themedIcon(QStringLiteral("filesystem-ntfs"));
    }
    if (fs == QStringLiteral("vfat") || fs == QStringLiteral("fat16") || fs == QStringLiteral("fat32")) {
        return themedIcon(QStringLiteral("filesystem-vfat"));
    }
    if (fs == QStringLiteral("swap")) {
        return themedIcon(QStringLiteral("filesystem-swap"));
    }
    if (!fs.isEmpty() && fs != QStringLiteral("—")) {
        return themedIcon(QStringLiteral("filesystem-generic"));
    }
    return QIcon();
}

QLabel *subtleLabel(const QString &text)
{
    auto *label = new QLabel(text);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    return label;
}

QLabel *sectionTitle(const QString &text)
{
    auto *label = new QLabel(text);
    QFont font = label->font();
    font.setPointSizeF(font.pointSizeF() * 1.2);
    font.setBold(true);
    label->setFont(font);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    return label;
}

// Keep application-owned dialogs aligned with the compact KDE/Qt dialog
// pattern.  Polkit's authentication dialog is provided by the desktop and
// cannot be styled by Boot Bitch, but our password, picker and help dialogs
// should use the same margins, spacing and modal behaviour.
QVBoxLayout *standardDialogLayout(QDialog *dialog, int minimumWidth = 500)
{
    if (!dialog) {
        return nullptr;
    }
    dialog->setModal(true);
    dialog->setMinimumWidth(minimumWidth);
    dialog->setSizeGripEnabled(false);
    auto *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(10);
    return layout;
}

// Informational help is a lightweight, modeless popup.  Keep the explicit
// Close button and Escape handling, while also making a click elsewhere in
// the application dismiss it so it never blocks the page underneath.
class DismissibleHelpDialog final : public QDialog
{
public:
    explicit DismissibleHelpDialog(QWidget *parent)
        : QDialog(parent)
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setModal(false);
        setWindowModality(Qt::NonModal);
        if (auto *application = qApp) {
            application->installEventFilter(this);
        }
    }

    ~DismissibleHelpDialog() override
    {
        if (auto *application = qApp) {
            application->removeEventFilter(this);
        }
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        Q_UNUSED(watched);
        if (event->type() == QEvent::MouseButtonPress && isVisible()) {
            const auto *mouseEvent = static_cast<const QMouseEvent *>(event);
            const QPoint globalPosition = mouseEvent->globalPosition().toPoint();
            if (!frameGeometry().contains(globalPosition)) {
                close();
                return true;
            }
        }
        return QDialog::eventFilter(watched, event);
    }
};

void configureTargetSummaryLabel(QLabel *label)
{
    if (!label) {
        return;
    }

    // Prefer a compact one-line target summary whenever horizontal space is
    // available. If a path or future descriptive target string is unusually
    // long, the label may shrink and wrap rather than colliding with the page
    // title/help controls.
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    label->setMinimumWidth(0);
    label->setMaximumWidth(440);
}

void showCompactHelp(QWidget *parent, const QString &title, const QString &text)
{
    auto *dialog = new DismissibleHelpDialog(parent);
    dialog->setWindowTitle(title);
    auto *layout = standardDialogLayout(dialog, 390);
    // standardDialogLayout is shared with genuinely modal editors and
    // confirmations; help remains explicitly modeless so outside clicks can
    // reach the application and dismiss this popup.
    dialog->setModal(false);
    dialog->setWindowModality(Qt::NonModal);

    layout->addWidget(sectionTitle(title));

    auto *body = subtleLabel(text);
    body->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addWidget(body);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    layout->addWidget(buttons);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

QToolButton *contextHelpButton(QWidget *parent, const QString &title, const QString &text)
{
    auto *button = new QToolButton(parent);
    const QIcon icon = themedIcon(QStringLiteral("help-contextual"));
    if (icon.isNull()) {
        button->setText(QStringLiteral("?"));
    } else {
        button->setIcon(icon);
    }
    button->setAutoRaise(true);
    button->setToolTip(QStringLiteral("About %1").arg(title));
    button->setAccessibleName(QStringLiteral("Help: %1").arg(title));
    QObject::connect(button, &QToolButton::clicked, parent, [parent, title, text] {
        showCompactHelp(parent, title, text);
    });
    return button;
}

QTableWidgetItem *readOnlyItem(const QString &text)
{
    const QString cleanText = text.simplified();
    auto *item = new QTableWidgetItem(cleanText);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setToolTip(cleanText);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    return item;
}

void enableSelectableLabels(QWidget *root)
{
    if (!root) {
        return;
    }

    const QList<QLabel *> labels = root->findChildren<QLabel *>();
    for (QLabel *label : labels) {
        if (!label) {
            continue;
        }
        label->setTextInteractionFlags(label->textInteractionFlags()
            | Qt::TextSelectableByMouse);
    }
}

void installCopyAction(QAbstractItemView *view)
{
    if (!view) {
        return;
    }

    auto *copyAction = new QAction(QStringLiteral("Copy"), view);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    QObject::connect(copyAction, &QAction::triggered, view, [view] {
        if (!view->selectionModel()) {
            return;
        }

        QModelIndexList indexes = view->selectionModel()->selectedIndexes();
        if (indexes.isEmpty() && view->currentIndex().isValid()) {
            indexes.append(view->currentIndex());
        }
        if (indexes.isEmpty()) {
            return;
        }

        std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &left, const QModelIndex &right) {
            if (left.parent() != right.parent()) {
                return left.parent().row() < right.parent().row();
            }
            if (left.row() != right.row()) {
                return left.row() < right.row();
            }
            return left.column() < right.column();
        });

        QString output;
        QModelIndex previous;
        bool first = true;
        for (const QModelIndex &index : indexes) {
            if (!index.isValid()) {
                continue;
            }

            if (!first) {
                if (previous.parent() != index.parent() || previous.row() != index.row()) {
                    output += QLatin1Char('\n');
                } else {
                    output += QLatin1Char('\t');
                }
            }
            output += index.data(Qt::DisplayRole).toString();
            previous = index;
            first = false;
        }

        if (!output.isEmpty()) {
            QApplication::clipboard()->setText(output);
        }
    });

    view->addAction(copyAction);
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
}

bool treeContainsEncryptedNode(const DeviceNode &node)
{
    if (node.encrypted) {
        return true;
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsEncryptedNode(child)) {
            return true;
        }
    }
    return false;
}

bool treeContainsLinuxCandidateNode(const DeviceNode &node)
{
    if (node.installedLinux || node.linuxCapableFileSystem) {
        return true;
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsLinuxCandidateNode(child)) {
            return true;
        }
    }
    return false;
}

bool treeContainsUnlockedLinuxInsideEncrypted(const DeviceNode &node)
{
    if (node.encrypted) {
        for (const DeviceNode &child : node.children) {
            if (treeContainsLinuxCandidateNode(child)) {
                return true;
            }
        }
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsUnlockedLinuxInsideEncrypted(child)) {
            return true;
        }
    }
    return false;
}

bool treeContainsEfiNode(const DeviceNode &node)
{
    const QString fs = node.fileSystem.toLower();
    const QString partLabel = node.partLabel.toLower();
    if (fs == QStringLiteral("vfat") || fs == QStringLiteral("fat") || fs == QStringLiteral("fat32")) {
        if (node.mountPoints.contains(QStringLiteral("/boot/efi"))
            || partLabel.contains(QStringLiteral("efi"))
            || node.sizeBytes <= (2ULL * 1024ULL * 1024ULL * 1024ULL)) {
            return true;
        }
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsEfiNode(child)) {
            return true;
        }
    }
    return false;
}

bool treeContainsInstalledLinuxNode(const DeviceNode &node)
{
    if (node.installedLinux) {
        return true;
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsInstalledLinuxNode(child)) {
            return true;
        }
    }
    return false;
}

int repairLikelihoodScore(const DeviceNode &disk)
{
    if (treeContainsInstalledLinuxNode(disk)) {
        return 0;
    }

    const bool encrypted = treeContainsEncryptedNode(disk);
    const bool linuxCapable = treeContainsLinuxCandidateNode(disk);
    const bool efi = treeContainsEfiNode(disk);

    if (encrypted && efi) {
        return 1;
    }
    if (linuxCapable && efi) {
        return 2;
    }
    if (encrypted) {
        return 3;
    }
    if (linuxCapable) {
        return 4;
    }

    const QString fs = disk.fileSystem.toLower();
    if (fs == QStringLiteral("iso9660") || disk.removable) {
        return 6;
    }

    return 5;
}

QString firstLinuxNameInTree(const DeviceNode &node)
{
    if (!node.osName.isEmpty()) {
        return node.osName;
    }
    for (const DeviceNode &child : node.children) {
        const QString name = firstLinuxNameInTree(child);
        if (!name.isEmpty()) {
            return name;
        }
    }
    return QString();
}

const DeviceNode *firstInstalledLinuxNode(const DeviceNode &node)
{
    if (node.installedLinux) {
        return &node;
    }
    for (const DeviceNode &child : node.children) {
        if (const DeviceNode *match = firstInstalledLinuxNode(child)) {
            return match;
        }
    }
    return nullptr;
}

const DeviceNode *firstEncryptedNode(const DeviceNode &node)
{
    if (node.encrypted) {
        return &node;
    }
    for (const DeviceNode &child : node.children) {
        if (const DeviceNode *match = firstEncryptedNode(child)) {
            return match;
        }
    }
    return nullptr;
}


bool encryptedNodeHasUnlockedLinuxChild(const DeviceNode &node)
{
    if (!node.encrypted) {
        return false;
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsLinuxCandidateNode(child)) {
            return true;
        }
    }
    return false;
}

const DeviceNode *firstLockedEncryptedNode(const DeviceNode &node)
{
    if (node.encrypted && !encryptedNodeHasUnlockedLinuxChild(node)) {
        return &node;
    }
    for (const DeviceNode &child : node.children) {
        if (const DeviceNode *match = firstLockedEncryptedNode(child)) {
            return match;
        }
    }
    return nullptr;
}

const DeviceNode *firstLinuxCapableNode(const DeviceNode &node)
{
    if (node.linuxCapableFileSystem) {
        return &node;
    }
    for (const DeviceNode &child : node.children) {
        if (const DeviceNode *match = firstLinuxCapableNode(child)) {
            return match;
        }
    }
    return nullptr;
}

const DeviceNode *preferredRepairNode(const DeviceNode &disk)
{
    if (const DeviceNode *installed = firstInstalledLinuxNode(disk)) {
        return installed;
    }
    if (const DeviceNode *linuxCapable = firstLinuxCapableNode(disk)) {
        return linuxCapable;
    }
    // Prefer an already-unlocked Linux filesystem below a LUKS container when
    // one is visible. Only fall back to the encrypted container itself when no
    // mountable Linux-capable child has been discovered.
    if (const DeviceNode *encrypted = firstEncryptedNode(disk)) {
        return encrypted;
    }
    return &disk;
}

QString friendlyTopLevelStatus(const DeviceNode &disk)
{
    const QString linuxName = firstLinuxNameInTree(disk);
    if (!linuxName.isEmpty()) {
        return QStringLiteral("Linux detected — %1").arg(linuxName);
    }

    const bool encrypted = treeContainsEncryptedNode(disk);
    const bool linuxCapable = treeContainsLinuxCandidateNode(disk);
    const bool efi = treeContainsEfiNode(disk);
    const bool unlockedInsideEncrypted = treeContainsUnlockedLinuxInsideEncrypted(disk);

    if (unlockedInsideEncrypted) {
        return QStringLiteral("Unlocked Linux filesystem — inspect to confirm");
    }
    if (linuxCapable && efi) {
        return QStringLiteral("Likely Linux — inspect to confirm");
    }
    if (encrypted && efi) {
        return QStringLiteral("Likely Linux — encrypted");
    }
    if (encrypted) {
        return QStringLiteral("Encrypted — unlock to inspect");
    }
    if (linuxCapable) {
        return QStringLiteral("Linux-capable — inspect to confirm");
    }
    return disk.status.isEmpty() ? QStringLiteral("Available for inspection") : disk.status;
}

QString friendlyNodeName(const DeviceNode &node)
{
    QString name;
    if (node.type == QStringLiteral("disk")) {
        name = node.model.trimmed();
        if (!node.vendor.trimmed().isEmpty() && !name.startsWith(node.vendor.trimmed(), Qt::CaseInsensitive)) {
            name = node.vendor.trimmed() + (name.isEmpty() ? QString() : QStringLiteral(" ") + name);
        }
        if (!node.label.isEmpty()) {
            name += name.isEmpty() ? node.label : QStringLiteral(" — ") + node.label;
        }
    } else {
        if (!node.label.isEmpty()) {
            name = node.label;
        } else if (!node.partLabel.isEmpty()) {
            name = node.partLabel;
        } else if (node.encrypted) {
            name = QStringLiteral("Encrypted volume");
        } else if (!node.fileSystem.isEmpty()) {
            name = QStringLiteral("%1 volume").arg(node.fileSystem.toUpper());
        } else if (node.type == QStringLiteral("part")) {
            name = QStringLiteral("Partition");
        } else {
            name = node.type;
        }
    }
    return name.isEmpty() ? QStringLiteral("Unnamed device") : name;
}

void collectCriticalMounts(const DeviceNode &node, QStringList &mounts)
{
    for (const QString &mountPoint : node.mountPoints) {
        if ((mountPoint == QStringLiteral("/")
             || mountPoint == QStringLiteral("/boot")
             || mountPoint == QStringLiteral("/boot/efi"))
            && !mounts.contains(mountPoint)) {
            mounts.append(mountPoint);
        }
    }
    for (const DeviceNode &child : node.children) {
        collectCriticalMounts(child, mounts);
    }
}
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_scanner(new SystemScanner(this))
    , m_settings(new QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"), this))
{
    m_actionLogEntries = m_settings->value(QStringLiteral("actionRegister")).toStringList();
    setWindowTitle(QStringLiteral("Boot Bitch"));
    setWindowIcon(QApplication::windowIcon());
    setMinimumSize(480, 500);
    resize(1180, 760);

    buildMenuBar();

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(18, 12, 18, 14);
    mainLayout->setSpacing(9);

    auto *headerLayout = new QHBoxLayout;
    auto *iconLabel = new QLabel;
    QPixmap pixmap = QApplication::windowIcon().pixmap(46, 46);
    iconLabel->setPixmap(pixmap);
    iconLabel->setFixedSize(50, 50);
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *titleColumn = new QVBoxLayout;
    titleColumn->setSpacing(2);

    auto *title = new QLabel(QStringLiteral("Boot Bitch"));
    QFont titleFont = title->font();
    titleFont.setPointSizeF(titleFont.pointSizeF() * 1.65);
    titleFont.setBold(true);
    title->setFont(titleFont);
    titleColumn->addWidget(title);
    titleColumn->addWidget(subtleLabel(QStringLiteral("Linux recovery and boot-repair utility")));

    auto *modeBadge = new QLabel(QStringLiteral("GUARDED REPAIR  •  %1").arg(QCoreApplication::applicationVersion()));
    QFont badgeFont = modeBadge->font();
    badgeFont.setBold(true);
    modeBadge->setFont(badgeFont);
    modeBadge->setAlignment(Qt::AlignCenter);
    modeBadge->setFrameShape(QFrame::StyledPanel);
    modeBadge->setContentsMargins(9, 5, 9, 5);
    modeBadge->setToolTip(QStringLiteral(
        "Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization."));

    headerLayout->addWidget(iconLabel);
    headerLayout->addLayout(titleColumn, 1);
    headerLayout->addWidget(modeBadge, 0, Qt::AlignTop);
    mainLayout->addLayout(headerLayout);

    m_tabs = new QTabWidget;
    m_tabs->setDocumentMode(true);
    m_tabs->setMovable(false);
    // Responsive labels keep the complete tab set visible at the minimum
    // window width. Disable QTabBar's native overflow pair: on some KDE
    // styles those hidden buttons still paint over the final tab when the
    // bar is resized. The explicit corner controls below provide stable
    // keyboard/pointer navigation without that overlap.
    m_tabs->tabBar()->setUsesScrollButtons(false);
    m_tabs->tabBar()->setElideMode(Qt::ElideRight);
    // Keep explicit navigation controls inside the tab strip.  Qt's native
    // overflow buttons are still used as the scroll implementation, while
    // these corner buttons remain reachable when the native pair would be
    // clipped at the far edge of a narrow window.
    m_tabScrollLeftButton = new QToolButton(m_tabs);
    m_tabScrollLeftButton->setArrowType(Qt::LeftArrow);
    m_tabScrollLeftButton->setAutoRaise(true);
    m_tabScrollLeftButton->setToolTip(QStringLiteral("Show previous tab"));
    m_tabScrollLeftButton->setAccessibleName(QStringLiteral("Show previous tab"));
    m_tabScrollRightButton = new QToolButton(m_tabs);
    m_tabScrollRightButton->setArrowType(Qt::RightArrow);
    m_tabScrollRightButton->setAutoRaise(true);
    m_tabScrollRightButton->setToolTip(QStringLiteral("Show next tab"));
    m_tabScrollRightButton->setAccessibleName(QStringLiteral("Show next tab"));
    // Put both controls in the leading corner so the right edge of a narrow
    // window never clips the navigation affordance.
    auto *tabScrollControls = new QWidget(m_tabs);
    auto *tabScrollLayout = new QHBoxLayout(tabScrollControls);
    tabScrollLayout->setContentsMargins(0, 0, 0, 0);
    tabScrollLayout->setSpacing(0);
    tabScrollLayout->addWidget(m_tabScrollLeftButton);
    tabScrollLayout->addWidget(m_tabScrollRightButton);
    m_tabs->setCornerWidget(tabScrollControls, Qt::TopLeftCorner);
    connect(m_tabScrollLeftButton, &QToolButton::clicked, this, [this] {
        if (!m_tabs || m_tabs->count() == 0) return;
        m_tabs->setCurrentIndex(qMax(0, m_tabs->currentIndex() - 1));
    });
    connect(m_tabScrollRightButton, &QToolButton::clicked, this, [this] {
        if (!m_tabs || m_tabs->count() == 0) return;
        m_tabs->setCurrentIndex(qMin(m_tabs->count() - 1, m_tabs->currentIndex() + 1));
    });
    m_tabs->addTab(buildSystemsPage(), themedIcon(QStringLiteral("drive-harddisk")), QStringLiteral("Systems"));
    m_tabs->addTab(buildDiagnosticsPage(), themedIcon(QStringLiteral("tools-report-bug")), QStringLiteral("Diagnostics"));
    m_tabs->addTab(buildRepairPage(), themedIcon(QStringLiteral("tools-wizard")), QStringLiteral("Repair"));
    m_tabs->addTab(buildSnapshotsPage(), themedIcon(QStringLiteral("document-revert")), QStringLiteral("Snapshots"));
    m_tabs->addTab(buildChrootShellPage(), themedIcon(QStringLiteral("utilities-terminal")), QStringLiteral("Chroot Shell"));
    m_tabs->addTab(buildFileCopyPage(), themedIcon(QStringLiteral("edit-copy")), QStringLiteral("File Copy"));
    m_tabs->addTab(buildLogsPage(), themedIcon(QStringLiteral("text-x-log")), QStringLiteral("Logs"));
    m_tabs->addTab(buildSettingsPage(), themedIcon(QStringLiteral("settings-configure")), QStringLiteral("Settings"));
    const QStringList tabNames = {
        QStringLiteral("Systems"), QStringLiteral("Diagnostics"), QStringLiteral("Repair"),
        QStringLiteral("Snapshots"), QStringLiteral("Chroot Shell"), QStringLiteral("File Copy"), QStringLiteral("Logs"), QStringLiteral("Settings")
    };
    for (int index = 0; index < tabNames.size(); ++index) {
        m_tabs->setTabToolTip(index, tabNames.at(index));
    }
    mainLayout->addWidget(m_tabs, 1);
    // Snapshot discovery may complete while its tab is hidden. Recalculate
    // row heights when the user first opens the tab so measurements use the
    // final visible column widths instead of the hidden page's tiny geometry.
    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == SnapshotsTab) {
            QTimer::singleShot(0, this, [this] { resizeSnapshotRows(); });
        }
    });

    setCentralWidget(central);
    enableSelectableLabels(central);
    normalizeButtonSizing();
    statusBar()->showMessage(QStringLiteral("Ready — guarded repair mode"));

    loadSettings();
    updateResponsiveLayout();
    refreshCapabilities();
    refreshDevices();
    // If a target is established by a restored session or an early device
    // selection, begin snapshot discovery after the first event loop turn so
    // Systems remains the initial responsive page.
    QTimer::singleShot(0, this, &MainWindow::scheduleSnapshotPreload);
    appendLog(QStringLiteral("Boot Bitch %1 started in guarded repair mode.").arg(QCoreApplication::applicationVersion()));
}

MainWindow::~MainWindow()
{
    closePrivilegedSession();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    closePrivilegedSession();
    saveSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateResponsiveLayout();
    QTimer::singleShot(0, this, [this] { resizeCapabilityRows(); });
}

void MainWindow::buildMenuBar()
{
    auto *fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    QAction *refreshAction = fileMenu->addAction(themedIcon(QStringLiteral("view-refresh")), QStringLiteral("Refresh Devices"));
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshDevices);

    m_lockAuthorizationAction = fileMenu->addAction(
        themedIcon(QStringLiteral("system-lock-screen")),
        QStringLiteral("Lock Administrator Session"));
    m_lockAuthorizationAction->setEnabled(false);
    m_lockAuthorizationAction->setToolTip(QStringLiteral(
        "Ends Boot Bitch's current privileged helper session, closes any LUKS mappings opened by Boot Bitch, and requires authorization again for the next root action. Pre-existing external mappings are left alone."));
    connect(m_lockAuthorizationAction, &QAction::triggered, this, [this] {
        closePrivilegedSession();
        appendLog(QStringLiteral("Administrator authorization session was explicitly locked by the user. Boot Bitch-owned temporary mounts were already released after each request and any Boot Bitch-owned LUKS mapper was asked to close."));
        refreshDevices();
        statusBar()->showMessage(QStringLiteral("Administrator session locked — owned target resources released; the next privileged action will request authorization."), 6000);
    });

    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(themedIcon(QStringLiteral("application-exit")), QStringLiteral("Quit"));
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    auto *viewMenu = menuBar()->addMenu(QStringLiteral("&View"));
    QAction *systemsAction = viewMenu->addAction(QStringLiteral("Systems"));
    connect(systemsAction, &QAction::triggered, this, [this] { m_tabs->setCurrentIndex(SystemsTab); });
    QAction *diagnosticsAction = viewMenu->addAction(QStringLiteral("Diagnostics"));
    connect(diagnosticsAction, &QAction::triggered, this, [this] { m_tabs->setCurrentIndex(DiagnosticsTab); });
    QAction *logsAction = viewMenu->addAction(QStringLiteral("Logs"));
    connect(logsAction, &QAction::triggered, this, [this] { m_tabs->setCurrentIndex(LogsTab); });
    QAction *settingsAction = viewMenu->addAction(QStringLiteral("Settings"));
    connect(settingsAction, &QAction::triggered, this, [this] { m_tabs->setCurrentIndex(SettingsTab); });

    viewMenu->addSeparator();
    QAction *autoSizeAction = viewMenu->addAction(QStringLiteral("Auto-size Device Columns"));
    connect(autoSizeAction, &QAction::triggered, this, &MainWindow::autoSizeDeviceColumns);

    m_wrapLogsAction = viewMenu->addAction(QStringLiteral("Wrap Log Lines"));
    m_wrapLogsAction->setCheckable(true);
    m_wrapLogsAction->setChecked(true);
    connect(m_wrapLogsAction, &QAction::toggled, this, &MainWindow::setLogWrapEnabled);

    auto *helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
    QAction *usageAction = helpMenu->addAction(themedIcon(QStringLiteral("help-contents")), QStringLiteral("Using Boot Bitch"));
    connect(usageAction, &QAction::triggered, this, &MainWindow::showUsageHelp);
    helpMenu->addSeparator();
    QAction *aboutAction = helpMenu->addAction(themedIcon(QStringLiteral("help-about")), QStringLiteral("About Boot Bitch"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
}

QWidget *MainWindow::buildSystemsPage()
{
    auto *page = new QWidget;
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(8, 8, 8, 8);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto *content = new QWidget;
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 6, 0);
    layout->setSpacing(8);

    auto *topRow = new QHBoxLayout;
    topRow->addWidget(sectionTitle(QStringLiteral("Systems")));
    topRow->addWidget(contextHelpButton(page, QStringLiteral("Systems"),
        QStringLiteral("Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system.")));
    topRow->addStretch(1);
    m_refreshDevicesButton = new QPushButton(themedIcon(QStringLiteral("view-refresh")), QStringLiteral("Refresh Devices"));
    connect(m_refreshDevicesButton, &QPushButton::clicked, this, &MainWindow::refreshDevices);
    topRow->addWidget(m_refreshDevicesButton, 0, Qt::AlignTop);
    layout->addLayout(topRow);

    // Compact, permanently protected running-host card. It is never part of
    // the ordinary candidate list, but its read-only details and explicit
    // maintenance path remains available.
    auto *hostBox = new QFrame;
    hostBox->setFrameShape(QFrame::StyledPanel);
    auto *hostLayout = new QHBoxLayout(hostBox);
    hostLayout->setContentsMargins(10, 8, 10, 8);
    hostLayout->setSpacing(9);

    auto *hostIcon = new QLabel;
    hostIcon->setPixmap(themedIcon(QStringLiteral("security-high"),
                                        themedIcon(QStringLiteral("drive-harddisk"))).pixmap(30, 30));
    hostIcon->setFixedSize(34, 34);
    hostIcon->setAlignment(Qt::AlignCenter);
    hostLayout->addWidget(hostIcon, 0, Qt::AlignTop);

    auto *hostText = new QVBoxLayout;
    hostText->setSpacing(2);

    m_hostSystemLabel = new QLabel(QStringLiteral("Detecting running system…"));
    QFont hostFont = m_hostSystemLabel->font();
    hostFont.setBold(true);
    m_hostSystemLabel->setFont(hostFont);
    m_hostSystemLabel->setWordWrap(true);
    hostText->addWidget(m_hostSystemLabel);

    m_hostStorageLabel = subtleLabel(QStringLiteral("Detecting protected storage…"));
    m_hostStorageLabel->setWordWrap(false);
    m_hostStorageLabel->setMinimumWidth(0);
    m_hostStorageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    m_hostMountsLabel = subtleLabel(QStringLiteral(""));
    m_hostMountsLabel->setWordWrap(false);
    m_hostMountsLabel->setMinimumWidth(0);
    m_hostMountsLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    hostText->addWidget(m_hostStorageLabel);
    hostText->addWidget(m_hostMountsLabel);
    hostLayout->addLayout(hostText, 1);

    auto *protectedBadge = new QLabel(QStringLiteral("PROTECTED"));
    QFont protectedFont = protectedBadge->font();
    protectedFont.setBold(true);
    protectedBadge->setFont(protectedFont);
    protectedBadge->setFrameShape(QFrame::StyledPanel);
    protectedBadge->setContentsMargins(8, 4, 8, 4);
    protectedBadge->setToolTip(QStringLiteral("The running host remains protected from ordinary repair-target operations."));
    hostLayout->addWidget(protectedBadge, 0, Qt::AlignTop);

    m_hostDetailsButton = new QPushButton(themedIcon(QStringLiteral("document-preview")), QStringLiteral("Details"));
    m_hostDetailsButton->setEnabled(false);
    m_hostDetailsButton->setToolTip(QStringLiteral("Show read-only details for the protected running host."));
    connect(m_hostDetailsButton, &QPushButton::clicked, this, &MainWindow::showHostDetails);
    hostLayout->addWidget(m_hostDetailsButton, 0, Qt::AlignTop);

    m_hostMaintenanceButton = new QPushButton(themedIcon(QStringLiteral("system-run")), QStringLiteral("Host Maintenance"));
    m_hostMaintenanceButton->setEnabled(false);
    m_hostMaintenanceButton->setToolTip(QStringLiteral(
        "Select the running host for deliberate guarded maintenance. All supported repair stages run against the active system; snapshots, shell and file-copy workflows remain separate tools."));
    connect(m_hostMaintenanceButton, &QPushButton::clicked, this, &MainWindow::selectHostForMaintenance);
    hostLayout->addWidget(m_hostMaintenanceButton, 0, Qt::AlignTop);

    m_hostDefaultButton = new QPushButton(themedIcon(QStringLiteral("preferences-system")), QStringLiteral("Make Default"));
    m_hostDefaultButton->setEnabled(false);
    m_hostDefaultButton->setToolTip(QStringLiteral(
        "Restore the host's uniquely identified TUXEDO UKI entry if needed, then place it first in BootOrder while preserving every other entry."));
    connect(m_hostDefaultButton, &QPushButton::clicked, this, &MainWindow::setHostDefaultBootEntry);
    hostLayout->addWidget(m_hostDefaultButton, 0, Qt::AlignTop);

    layout->addWidget(hostBox);

    auto *candidateRow = new QHBoxLayout;
    candidateRow->addWidget(sectionTitle(QStringLiteral("Available repair targets")));
    candidateRow->addWidget(contextHelpButton(page, QStringLiteral("Repair targets"),
        QStringLiteral("Drives are ranked by visible Linux, EFI, filesystem and encryption evidence. Select the top-level drive; partitions and mapped volumes are informational.")));
    candidateRow->addStretch(1);
    auto *sortHint = new QLabel(QStringLiteral("Most likely first"));
    sortHint->setToolTip(QStringLiteral("Candidates are ranked by visible Linux, EFI, filesystem and encryption evidence."));
    candidateRow->addWidget(sortHint);
    layout->addLayout(candidateRow);

    m_systemSplitter = new QSplitter(Qt::Horizontal);
    m_systemSplitter->setChildrenCollapsible(false);
    m_systemSplitter->setMinimumHeight(310);

    auto *left = new QWidget;
    auto *leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(7);

    m_deviceTree = new QTreeWidget;
    m_deviceTree->setColumnCount(6);
    m_deviceTree->setHeaderLabels({
        QStringLiteral("Model / Label"), QStringLiteral("Status"), QStringLiteral("Connection"),
        QStringLiteral("Size"), QStringLiteral("Filesystem"), QStringLiteral("Device")
    });
    m_deviceTree->setAlternatingRowColors(true);
    m_deviceTree->setRootIsDecorated(true);
    m_deviceTree->setUniformRowHeights(false);
    m_deviceTree->setWordWrap(true);
    m_deviceTree->setTextElideMode(Qt::ElideRight);
    m_deviceTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_deviceTree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_deviceTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // A narrow Systems pane may need horizontal scrolling; updateDeviceTree-
    // Height reserves its gutter so the row area remains stable when it
    // appears after device discovery.
    m_deviceTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    installCopyAction(m_deviceTree);
    m_deviceTree->header()->setMinimumSectionSize(55);
    m_deviceTree->header()->setStretchLastSection(false);
    for (int column = 0; column < m_deviceTree->columnCount(); ++column) {
        m_deviceTree->header()->setSectionResizeMode(column, QHeaderView::Interactive);
    }
    applyDeviceColumnDefaults();
    leftLayout->addWidget(m_deviceTree, 0);

    connect(m_deviceTree, &QTreeWidget::itemSelectionChanged, this, &MainWindow::updateDeviceDetails);
    connect(m_deviceTree, &QTreeWidget::itemExpanded, this, [this] { updateDeviceTreeHeight(); });
    connect(m_deviceTree, &QTreeWidget::itemCollapsed, this, [this] { updateDeviceTreeHeight(); });

    auto *buttonRow = new QHBoxLayout;
    m_setTargetButton = new QPushButton(themedIcon(QStringLiteral("dialog-ok-apply")), QStringLiteral("Select Target"));
    m_setTargetButton->setEnabled(false);
    connect(m_setTargetButton, &QPushButton::clicked, this, &MainWindow::setPreviewTarget);

    m_unlockTargetButton = new QPushButton(themedIcon(QStringLiteral("object-unlocked")), QStringLiteral("Unlock"));
    m_unlockTargetButton->setEnabled(false);
    m_unlockTargetButton->setToolTip(QStringLiteral("Select a drive whose detected target is a locked LUKS volume."));
    connect(m_unlockTargetButton, &QPushButton::clicked, this, &MainWindow::unlockSelectedTarget);

    buttonRow->addWidget(m_setTargetButton);
    buttonRow->addWidget(m_unlockTargetButton);
    buttonRow->addStretch(1);
    m_systemTargetLabel = new QLabel(QStringLiteral("Committed target: none"));
    configureTargetSummaryLabel(m_systemTargetLabel);
    m_systemTargetLabel->setToolTip(QStringLiteral("No repair target has been committed yet. Row selection is inspection only until Select Target is pressed."));
    buttonRow->addWidget(m_systemTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    leftLayout->addLayout(buttonRow);

    m_unlockStatusBox = new QGroupBox(QStringLiteral("Unlock status"));
    m_unlockStatusBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *unlockStatusLayout = new QVBoxLayout(m_unlockStatusBox);
    unlockStatusLayout->setContentsMargins(8, 8, 8, 8);
    m_unlockStatusView = new QPlainTextEdit;
    m_unlockStatusView->setReadOnly(true);
    m_unlockStatusView->setUndoRedoEnabled(false);
    m_unlockStatusView->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_unlockStatusView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_unlockStatusView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // Let the status frame occupy the remaining left-pane height.  This keeps
    // its bottom edge aligned with the selected-drive details frame while the
    // read-only text remains compact at the top of the editor.
    m_unlockStatusView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_unlockStatusView->setPlaceholderText(QStringLiteral("Select a drive to see unlock status."));
    m_unlockStatusView->setPlainText(QStringLiteral("No unlock operation recorded for this drive in the current session."));
    unlockStatusLayout->addWidget(m_unlockStatusView);
    leftLayout->addWidget(m_unlockStatusBox, 1);

    m_selectedDriveDetailsBox = new QGroupBox(QStringLiteral("Selected drive details"));
    auto *detailsBoxLayout = new QVBoxLayout(m_selectedDriveDetailsBox);
    detailsBoxLayout->setContentsMargins(6, 8, 6, 6);

    auto *detailsScroll = new QScrollArea;
    detailsScroll->setWidgetResizable(true);
    detailsScroll->setFrameShape(QFrame::NoFrame);
    detailsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    detailsScroll->setMinimumHeight(150);

    auto *detailsContent = new QWidget;
    auto *detailsLayout = new QFormLayout(detailsContent);
    detailsLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    detailsLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    detailsLayout->setFormAlignment(Qt::AlignTop);
    detailsLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    detailsLayout->setVerticalSpacing(7);

    auto makeValue = [] {
        auto *label = new QLabel(QStringLiteral("—"));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setWordWrap(true);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        label->setMinimumHeight(label->fontMetrics().lineSpacing() + 3);
        return label;
    };

    m_detailPath = makeValue();
    m_detailResolvedTarget = makeValue();
    m_detailModel = makeValue();
    m_detailSize = makeValue();
    m_detailTransport = makeValue();
    m_detailFilesystem = makeValue();
    m_detailUuid = makeValue();
    m_detailMounts = makeValue();
    m_detailStatus = makeValue();
    m_detailProtection = makeValue();

    detailsLayout->addRow(QStringLiteral("Drive:"), m_detailPath);
    detailsLayout->addRow(QStringLiteral("Detected target:"), m_detailResolvedTarget);
    detailsLayout->addRow(QStringLiteral("Model / label:"), m_detailModel);
    detailsLayout->addRow(QStringLiteral("Status:"), m_detailStatus);
    detailsLayout->addRow(QStringLiteral("Size:"), m_detailSize);
    detailsLayout->addRow(QStringLiteral("Connection:"), m_detailTransport);
    detailsLayout->addRow(QStringLiteral("Filesystem:"), m_detailFilesystem);
    detailsLayout->addRow(QStringLiteral("UUID:"), m_detailUuid);
    detailsLayout->addRow(QStringLiteral("Mounts:"), m_detailMounts);
    detailsLayout->addRow(QStringLiteral("Protection:"), m_detailProtection);
    m_detailResolvedTarget->setToolTip(QStringLiteral(
        "Best system component visible without privileged probing. Read-only inspection will later resolve closed encryption and Btrfs root subvolumes automatically."));

    detailsScroll->setWidget(detailsContent);
    detailsBoxLayout->addWidget(detailsScroll);

    auto *right = new QWidget;
    auto *rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(4, 0, 0, 0);
    rightLayout->addWidget(m_selectedDriveDetailsBox, 1);

    m_systemSplitter->addWidget(left);
    m_systemSplitter->addWidget(right);
    m_systemSplitter->setStretchFactor(0, 7);
    m_systemSplitter->setStretchFactor(1, 3);
    m_systemSplitter->setSizes({820, 320});
    layout->addWidget(m_systemSplitter, 1);

    scroll->setWidget(content);
    outerLayout->addWidget(scroll, 1);

    return page;
}

QWidget *MainWindow::buildDiagnosticsPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    auto *heading = new QHBoxLayout;
    heading->addWidget(sectionTitle(QStringLiteral("Diagnostics")));
    heading->addWidget(contextHelpButton(page, QStringLiteral("Diagnostics"),
        QStringLiteral("Use Inspect to choose either the protected Running Host or a selected repair drive. Both scopes provide read-only diagnostics; host EFI/UKI checks inspect the active ESP and firmware entries, while repair-drive checks mount only that system read-only.")));
    heading->addStretch(1);
    heading->addWidget(new QLabel(QStringLiteral("Inspect:")));

    m_diagnosticScopeCombo = new QComboBox;
    m_diagnosticScopeCombo->addItem(QStringLiteral("Repair Target"));
    m_diagnosticScopeCombo->addItem(QStringLiteral("Running Host"));
    m_diagnosticScopeCombo->setCurrentIndex(1);
    m_diagnosticScopeCombo->setToolTip(QStringLiteral("Choose the protected Running Host or the selected repair drive for read-only diagnostics."));
    m_diagnosticScopeCombo->setMinimumContentsLength(14);
    heading->addWidget(m_diagnosticScopeCombo);

    m_runAllDiagnosticsButton = new QPushButton(themedIcon(QStringLiteral("system-run")), QStringLiteral("Run All"));
    heading->addWidget(m_runAllDiagnosticsButton);
    m_targetConfigCombo = new QComboBox;
    m_targetConfigCombo->addItem(QStringLiteral("/etc/fstab"), QStringLiteral("fstab"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/crypttab"), QStringLiteral("crypttab"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/default/grub"), QStringLiteral("grub-defaults"));
    m_targetConfigCombo->addItem(QStringLiteral("/boot/grub/grub.cfg"), QStringLiteral("grub-config"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/sddm.conf"), QStringLiteral("sddm"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/gdm3/daemon.conf"), QStringLiteral("gdm3"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/lightdm/lightdm.conf"), QStringLiteral("lightdm"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/greetd/config.toml"), QStringLiteral("greetd"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/ly/config.ini"), QStringLiteral("ly"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/initramfs-tools/initramfs.conf"), QStringLiteral("initramfs"));
    m_targetConfigCombo->setToolTip(QStringLiteral("Select a target configuration file to inspect or edit through the guarded helper."));
    m_targetConfigCombo->setVisible(false);
    m_editTargetConfigButton = new QPushButton(themedIcon(QStringLiteral("document-edit")), QStringLiteral("Edit Target File…"));
    m_editTargetConfigButton->setVisible(false);
    m_editTargetConfigButton->setToolTip(QStringLiteral("Read or edit the selected target configuration file. Changes invalidate cached diagnostics."));
    layout->addLayout(heading);
    auto *configRow = new QHBoxLayout;
    configRow->setContentsMargins(0, 0, 0, 0);
    auto *configLabel = new QLabel(QStringLiteral("Target configuration:"));
    configLabel->setVisible(false);
    configRow->addWidget(configLabel);
    configRow->addWidget(m_targetConfigCombo, 1);
    configRow->addWidget(m_editTargetConfigButton);
    layout->addLayout(configRow);

    m_diagnosticSplitter = new QSplitter(Qt::Horizontal);
    m_diagnosticSplitter->setChildrenCollapsible(false);
    m_diagnosticSplitter->setMinimumHeight(330);

    auto *listBox = new QGroupBox(QStringLiteral("Diagnostic checks"));
    auto *listLayout = new QVBoxLayout(listBox);
    listLayout->setContentsMargins(8, 10, 8, 8);

    m_diagnosticList = new QListWidget;
    m_diagnosticList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_diagnosticList->setAlternatingRowColors(true);
    m_diagnosticList->setTextElideMode(Qt::ElideRight);
    installCopyAction(m_diagnosticList);

    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        auto *item = new QListWidgetItem(themedIcon(QString::fromLatin1(spec.icon)),
                                         QString::fromLatin1(spec.title),
                                         m_diagnosticList);
        item->setData(Qt::UserRole, QString::fromLatin1(spec.key));
        item->setToolTip(QString::fromLatin1(spec.description));
    }
    listLayout->addWidget(m_diagnosticList, 1);

    auto *detailBox = new QGroupBox(QStringLiteral("Selected diagnostic"));
    auto *detailLayout = new QVBoxLayout(detailBox);
    detailLayout->setContentsMargins(10, 10, 10, 10);
    detailLayout->setSpacing(8);

    m_diagnosticTitle = sectionTitle(QStringLiteral("Select a diagnostic"));
    detailLayout->addWidget(m_diagnosticTitle);

    m_diagnosticDescription = subtleLabel(QStringLiteral("Choose a diagnostic from the list."));
    m_diagnosticDescription->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    detailLayout->addWidget(m_diagnosticDescription);

    m_diagnosticAvailability = subtleLabel(QStringLiteral("Ready"));
    QFont availabilityFont = m_diagnosticAvailability->font();
    availabilityFont.setBold(true);
    m_diagnosticAvailability->setFont(availabilityFont);
    m_diagnosticAvailability->setContentsMargins(0, 0, 0, 0);
    detailLayout->addWidget(m_diagnosticAvailability);

    auto *runRow = new QHBoxLayout;
    runRow->setContentsMargins(0, 0, 0, 0);
    runRow->addStretch(1);
    m_runDiagnosticButton = new QPushButton(themedIcon(QStringLiteral("system-run")), QStringLiteral("Run Diagnostic"));
    runRow->addWidget(m_runDiagnosticButton);
    detailLayout->addLayout(runRow);

    auto *resultsHeader = new QHBoxLayout;
    resultsHeader->addWidget(sectionTitle(QStringLiteral("Results")));
    resultsHeader->addStretch(1);
    detailLayout->addLayout(resultsHeader);

    m_diagnosticResults = new QPlainTextEdit;
    m_diagnosticResults->setReadOnly(true);
    m_diagnosticResults->setPlaceholderText(QStringLiteral("Diagnostic results appear here."));
    m_diagnosticResults->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_diagnosticResults->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_diagnosticResults->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QFont diagnosticMono(QStringLiteral("monospace"));
    diagnosticMono.setStyleHint(QFont::Monospace);
    m_diagnosticResults->setFont(diagnosticMono);
    detailLayout->addWidget(m_diagnosticResults, 1);

    auto *resultButtons = new QHBoxLayout;
    resultButtons->setContentsMargins(0, 0, 0, 0);
    resultButtons->addStretch(1);
    m_copyDiagnosticButton = new QPushButton(themedIcon(QStringLiteral("edit-copy")), QStringLiteral("Copy Results"));
    m_saveDiagnosticButton = new QPushButton(themedIcon(QStringLiteral("document-save")), QStringLiteral("Save Results…"));
    m_copyDiagnosticButton->setEnabled(false);
    m_saveDiagnosticButton->setEnabled(false);
    resultButtons->addWidget(m_copyDiagnosticButton);
    resultButtons->addWidget(m_saveDiagnosticButton);
    detailLayout->addLayout(resultButtons);

    m_diagnosticSplitter->addWidget(listBox);
    m_diagnosticSplitter->addWidget(detailBox);
    m_diagnosticSplitter->setStretchFactor(0, 2);
    m_diagnosticSplitter->setStretchFactor(1, 5);
    m_diagnosticSplitter->setSizes({280, 760});
    layout->addWidget(m_diagnosticSplitter, 1);

    connect(m_diagnosticList, &QListWidget::currentRowChanged, this, &MainWindow::updateDiagnosticDetails);
    connect(m_diagnosticScopeCombo, &QComboBox::currentIndexChanged, this, &MainWindow::updateDiagnosticDetails);
    connect(m_runDiagnosticButton, &QPushButton::clicked, this, &MainWindow::runSelectedDiagnostic);
    connect(m_runAllDiagnosticsButton, &QPushButton::clicked, this, &MainWindow::runAllDiagnostics);
    connect(m_copyDiagnosticButton, &QPushButton::clicked, this, &MainWindow::copyDiagnosticResults);
    connect(m_saveDiagnosticButton, &QPushButton::clicked, this, &MainWindow::saveDiagnosticResults);
    connect(m_editTargetConfigButton, &QPushButton::clicked, this, &MainWindow::editTargetConfig);
    connect(m_diagnosticScopeCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, configLabel](int index) {
        const bool target = index == 0;
        // The config controls are only meaningful for a mounted repair target.
        configLabel->setVisible(target);
        if (m_targetConfigCombo) m_targetConfigCombo->setVisible(target);
        if (m_editTargetConfigButton) m_editTargetConfigButton->setVisible(target);
    });

    if (m_diagnosticList->count() > 0) {
        m_diagnosticList->setCurrentRow(0);
    }

    return page;
}

QWidget *MainWindow::buildRepairPage()
{
    auto *page = new QWidget;
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(8, 8, 8, 8);

    auto *scroll = new QScrollArea;
    scroll->setObjectName(QStringLiteral("repairPageScroll"));
    scroll->setWidgetResizable(true);
    // Pin the inner frame to the viewport width. Ignoring its size hint lets
    // it shrink below a child row's preferred width, avoiding a hidden
    // horizontal range and the tiny left/right pan seen on narrow windows.
    scroll->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto *content = new QWidget;
    content->setMinimumWidth(0);
    content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 6, 0);
    layout->setSpacing(8);

    auto *heading = new QHBoxLayout;
    heading->addWidget(sectionTitle(QStringLiteral("Repair")));
    heading->addWidget(contextHelpButton(page, QStringLiteral("Repair"),
        QStringLiteral("Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host.")));
    heading->addStretch(1);
    m_repairTargetLabel = new QLabel(QStringLiteral("Target: none selected"));
    configureTargetSummaryLabel(m_repairTargetLabel);
    heading->addWidget(m_repairTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addLayout(heading);

    layout->addWidget(subtleLabel(QStringLiteral(
        "Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.")));

    auto *planBox = new QGroupBox(QStringLiteral("Full Repair plan"));
    auto *planLayout = new QVBoxLayout(planBox);
    // Keep the plan header, readiness hint and stage list close together so
    // the lower individual-tool pane retains useful space at compact sizes.
    planLayout->setSpacing(4);

    auto *planHeader = new QHBoxLayout;
    m_fullRepairCountLabel = new QLabel(QStringLiteral("No stages selected"));
    QFont countFont = m_fullRepairCountLabel->font();
    countFont.setBold(true);
    m_fullRepairCountLabel->setFont(countFont);
    planHeader->addWidget(m_fullRepairCountLabel);

    m_fullRepairReadinessLabel = subtleLabel(QStringLiteral(
        "Run All diagnostics for the selected target or running host before starting Full Repair. "
        "The report is read-only evidence used to choose and confirm repair stages."));
    m_fullRepairReadinessLabel->setWordWrap(true);
    m_fullRepairReadinessLabel->setObjectName(QStringLiteral("fullRepairReadinessLabel"));
    m_fullRepairReadinessLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    planHeader->addStretch(1);
    auto *configurePlan = new QPushButton(themedIcon(QStringLiteral("settings-configure")), QStringLiteral("Configure Plan…"));
    configurePlan->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    configurePlan->setFixedWidth(configurePlan->sizeHint().width());
    connect(configurePlan, &QPushButton::clicked, this, [this] {
        if (m_tabs) {
            m_tabs->setCurrentIndex(SettingsTab);
        }
    });
    planHeader->addWidget(configurePlan);

    m_runFullRepairButton = new QPushButton(themedIcon(QStringLiteral("tools-wizard")), QStringLiteral("Run Full Repair"));
    m_runFullRepairButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_runFullRepairButton->setFixedWidth(m_runFullRepairButton->sizeHint().width());
    m_runFullRepairButton->setEnabled(false);
    m_runFullRepairButton->setToolTip(QStringLiteral("Select a repair drive, or choose Host Maintenance on the protected running-host card."));
    planHeader->addWidget(m_runFullRepairButton);
    // Keep the header and readiness message content-sized.  The vertical
    // splitter may provide extra room, but it should remain below the plan
    // rows rather than creating large gaps between these controls.
    planLayout->addLayout(planHeader, 0);
    planLayout->setAlignment(planHeader, Qt::AlignTop);
    planLayout->addWidget(m_fullRepairReadinessLabel, 0, Qt::AlignTop);

    m_fullRepairStageList = new QListWidget;
    m_fullRepairStageList->setSelectionMode(QAbstractItemView::NoSelection);
    m_fullRepairStageList->setFocusPolicy(Qt::NoFocus);
    m_fullRepairStageList->setAlternatingRowColors(true);
    m_fullRepairStageList->setWordWrap(true);
    m_fullRepairStageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // The Full Repair plan has at most nine configured rows. Size it to its
    // content and never introduce an inner vertical scrollbar; the Repair
    // page owns scrolling when the whole page becomes too short.
    m_fullRepairStageList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_fullRepairStageList->setMinimumHeight(115);
    m_fullRepairStageList->setMaximumHeight(265);
    m_fullRepairStageList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    planLayout->addWidget(m_fullRepairStageList, 0, Qt::AlignTop);
    planLayout->setAlignment(Qt::AlignTop);

    m_repairVerticalSplitter = new QSplitter(Qt::Vertical);
    m_repairVerticalSplitter->setObjectName(QStringLiteral("repairVerticalSplitter"));
    m_repairVerticalSplitter->setChildrenCollapsible(false);

    m_repairSplitter = new QSplitter(Qt::Horizontal);
    m_repairSplitter->setChildrenCollapsible(false);
    m_repairSplitter->setMinimumHeight(300);

    auto *toolBox = new QGroupBox(QStringLiteral("Individual repair tools"));
    auto *toolLayout = new QVBoxLayout(toolBox);
    toolLayout->setContentsMargins(8, 10, 8, 8);

    m_repairToolTree = new QTreeWidget;
    m_repairToolTree->setColumnCount(2);
    m_repairToolTree->setHeaderLabels({QStringLiteral("Tool"), QStringLiteral("Full Repair")});
    m_repairToolTree->setRootIsDecorated(false);
    m_repairToolTree->setAlternatingRowColors(true);
    m_repairToolTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_repairToolTree->setTextElideMode(Qt::ElideRight);
    m_repairToolTree->setWordWrap(true);
    // Keep the viewport width stable as rows are added or removed.  Reserving
    // the scrollbar gutter prevents the Tool and Full Repair columns from
    // shifting horizontally when the vertical scrollbar becomes visible.
    m_repairToolTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    installCopyAction(m_repairToolTree);
    m_repairToolTree->setMinimumHeight(165);
    m_repairToolTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_repairToolTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    struct ToolSpec {
        const char *key;
        const char *title;
        const char *icon;
    };
    static const ToolSpec tools[] = {
        {"validate", "Validate environment", "task-complete"},
        {"dpkg", "Complete package configuration", "dialog-ok-apply"},
        {"fixbroken", "Repair broken dependencies", "dialog-ok-apply"},
        {"aptupdate", "Refresh package metadata", "view-refresh"},
        {"upgrade", "Upgrade installed packages", "system-software-update"},
        {"dkms", "DKMS", "applications-development"},
        {"display", "Graphical login / display manager", "video-display"},
        {"initramfs", "Initramfs", "view-refresh"},
        {"efi", "EFI / UKI bootloader", "drive-removable-media"},
        {"grub", "GRUB configuration", "preferences-system"},
        {"bootstack", "Boot stack reconciliation", "system-run"}
    };

    for (const ToolSpec &spec : tools) {
        auto *item = new QTreeWidgetItem(m_repairToolTree);
        item->setText(0, QString::fromLatin1(spec.title));
        item->setData(0, Qt::UserRole, QString::fromLatin1(spec.key));
        item->setIcon(0, themedIcon(QString::fromLatin1(spec.icon)));
        item->setToolTip(0, item->text(0));
        item->setToolTip(1, QStringLiteral("Mirrors the corresponding Settings → Full Repair plan checkbox. Individual tools remain runnable independently."));
    }
    toolLayout->addWidget(m_repairToolTree, 1);

    auto *detailBox = new QGroupBox(QStringLiteral("Selected tool"));
    auto *detailLayout = new QVBoxLayout(detailBox);
    detailLayout->setContentsMargins(10, 10, 10, 10);
    detailLayout->setSpacing(9);
    detailBox->setMinimumHeight(180);
    detailBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Keep short tool details directly in the panel instead of nesting another
    // scroll area. The layout consumes spare height first; once the complete
    // Repair page no longer fits, its outer scroll area takes over. This keeps
    // text width stable and lets the action button move naturally with height.
    m_repairToolTitle = sectionTitle(QStringLiteral("Select a repair tool"));
    detailLayout->addWidget(m_repairToolTitle);

    m_repairToolDescription = subtleLabel(QStringLiteral("Select a tool to review its repair action."));
    m_repairToolDescription->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    detailLayout->addWidget(m_repairToolDescription);

    m_repairToolPlanStatus = subtleLabel(QString());
    QFont planStatusFont = m_repairToolPlanStatus->font();
    planStatusFont.setBold(true);
    m_repairToolPlanStatus->setFont(planStatusFont);
    m_repairToolPlanStatus->setWordWrap(true);
    m_repairToolPlanStatus->setContentsMargins(0, 0, 0, 0);
    m_repairToolPlanStatus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    detailLayout->addWidget(m_repairToolPlanStatus);

    // Flexible space collapses before any text or button is compressed.
    detailLayout->addStretch(1);

    auto *actionRow = new QHBoxLayout;
    actionRow->setContentsMargins(0, 0, 0, 0);
    actionRow->addStretch(1);
    m_repairToolButton = new QPushButton(QStringLiteral("Run Tool"));
    m_repairToolButton->setEnabled(false);
    m_repairToolButton->setToolTip(QStringLiteral("Select a repair drive, or choose Host Maintenance on the protected running-host card."));
    actionRow->addWidget(m_repairToolButton);
    detailLayout->addLayout(actionRow);

    m_repairSplitter->addWidget(toolBox);
    m_repairSplitter->addWidget(detailBox);
    m_repairSplitter->setStretchFactor(0, 5);
    m_repairSplitter->setStretchFactor(1, 6);
    m_repairSplitter->setSizes({470, 560});
    m_repairVerticalSplitter->addWidget(planBox);
    m_repairVerticalSplitter->addWidget(m_repairSplitter);
    m_repairVerticalSplitter->setStretchFactor(0, 0);
    m_repairVerticalSplitter->setStretchFactor(1, 1);
    m_repairVerticalSplitter->setSizes({300, 620});
    layout->addWidget(m_repairVerticalSplitter, 1);

    connect(m_repairToolTree, &QTreeWidget::itemSelectionChanged, this, &MainWindow::updateRepairToolDetails);
    connect(m_repairToolButton, &QPushButton::clicked, this, &MainWindow::runSelectedRepairTool);
    connect(m_runFullRepairButton, &QPushButton::clicked, this, &MainWindow::runFullRepair);
    if (m_repairToolTree->topLevelItemCount() > 0) {
        m_repairToolTree->setCurrentItem(m_repairToolTree->topLevelItem(0));
    }

    scroll->setWidget(content);
    outerLayout->addWidget(scroll, 1);

    return page;
}

QWidget *MainWindow::buildSnapshotsPage()
{
    auto *page = new QWidget;
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(8, 8, 8, 8);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto *content = new QWidget;
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 6, 0);
    layout->setSpacing(8);

    auto *heading = new QHBoxLayout;
    heading->addWidget(sectionTitle(QStringLiteral("Btrfs snapshots")));
    heading->addWidget(contextHelpButton(page, QStringLiteral("Snapshots"),
        QStringLiteral("Inspect Btrfs root snapshots and perform a transactional rollback. Rollback keeps the source snapshot unchanged, preserves the current @ root, promotes a writable copy to @, rebuilds the boot stack and automatically restores the old root if post-switch validation fails.")));
    heading->addStretch(1);
    m_snapshotTargetLabel = new QLabel(QStringLiteral("Target: none selected"));
    configureTargetSummaryLabel(m_snapshotTargetLabel);
    heading->addWidget(m_snapshotTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addLayout(heading);

    m_snapshotTable = new QTableWidget(0, 5);
    m_snapshotTable->setHorizontalHeaderLabels({
        QStringLiteral("Snapshot"), QStringLiteral("Created"), QStringLiteral("Type"), QStringLiteral("Description"), QStringLiteral("Status")
    });
    m_snapshotTable->setAlternatingRowColors(true);
    m_snapshotTable->setMinimumHeight(220);
    m_snapshotTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_snapshotTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_snapshotTable->setTextElideMode(Qt::ElideRight);
    // Keep snapshot rows single-line; long descriptions are elided instead
    // of allowing GTK styles to expand every row to a tall wrapped hint.
    m_snapshotTable->setWordWrap(false);
    m_snapshotTable->setFrameShape(QFrame::StyledPanel);
    m_snapshotTable->setFrameShadow(QFrame::Plain);
    m_snapshotTable->setLineWidth(1);
    m_snapshotTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // Rows are sized by resizeSnapshotRows().  Fixed section mode prevents
    // platform styles (notably GNOME/GTK) from stretching each row to fill
    // the viewport while keeping content compact and elided.
    m_snapshotTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // Keep single-line rows compact across Qt/platform font metrics.  A fixed
    // 28px floor made Qt 6.4 builds exceed the intended one-line height.
    m_snapshotTable->verticalHeader()->setMinimumSectionSize(
        m_snapshotTable->fontMetrics().lineSpacing() + 10);
    // Recalculate after the table receives its final width. Snapshot loading
    // may run asynchronously while the page is still hidden; measuring rows
    // at that point otherwise leaves every row at an unnecessarily tall
    // wrapped height after the page is shown.
    connect(m_snapshotTable->horizontalHeader(), &QHeaderView::sectionResized, this, [this] {
        QTimer::singleShot(0, this, [this] { resizeSnapshotRows(); });
    });
    installCopyAction(m_snapshotTable);
    auto *snapshotHeader = m_snapshotTable->horizontalHeader();
    snapshotHeader->setStretchLastSection(true);
    snapshotHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    snapshotHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    // Keep the descriptive columns readable at the normal window width. The
    // table remains interactive, so users can narrow these columns when they
    // need more room for the other content; resizeSnapshotRows() will then
    // wrap only the rows that no longer fit.
    m_snapshotTable->setColumnWidth(2, 92);   // Type
    m_snapshotTable->setColumnWidth(3, 300);  // Description
    m_snapshotTable->setColumnWidth(4, 320);  // Status (stretch target)
    m_snapshotDetails = new QPlainTextEdit;
    m_snapshotDetails->setReadOnly(true);
    m_snapshotDetails->setPlaceholderText(QStringLiteral(
        "Load snapshots to inspect recovery points. Snapshot discovery runs through the guarded read-only helper, so root-owned Snapper metadata does not need to be readable by the desktop user."));
    m_snapshotDetails->setMinimumHeight(120);
    m_snapshotDetails->setMaximumBlockCount(4000);
    m_snapshotDetails->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_snapshotDetails->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    // Keep the inventory and its log/details pane independently resizable.
    // The initial table height is sized for roughly thirteen compact rows so
    // the details remain visible without making the snapshot list unwieldy.
    m_snapshotSplitter = new QSplitter(Qt::Vertical);
    m_snapshotSplitter->setObjectName(QStringLiteral("snapshotSplitter"));
    m_snapshotSplitter->setChildrenCollapsible(false);
    m_snapshotSplitter->addWidget(m_snapshotTable);
    m_snapshotSplitter->addWidget(m_snapshotDetails);
    const int rowHeight = qMax(24, fontMetrics().lineSpacing() + 10);
    m_snapshotSplitter->setStretchFactor(0, 1);
    m_snapshotSplitter->setStretchFactor(1, 1);
    m_snapshotSplitter->setSizes({13 * rowHeight + 32, 220});
    layout->addWidget(m_snapshotSplitter, 1);

    m_snapshotButtonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_snapshotButtonLayout->setContentsMargins(0, 0, 0, 0);
    m_snapshotButtonLayout->setSpacing(8);

    m_snapshotLoadButton = new QPushButton(themedIcon(QStringLiteral("view-refresh")), QStringLiteral("Load Snapshots"));
    m_snapshotInspectButton = new QPushButton(themedIcon(QStringLiteral("document-preview")), QStringLiteral("Inspect Selected"));
    m_snapshotRollbackButton = new QPushButton(themedIcon(QStringLiteral("document-revert")), QStringLiteral("Roll Back to Selected"));

    m_snapshotLoadButton->setEnabled(false);
    m_snapshotInspectButton->setEnabled(false);
    m_snapshotRollbackButton->setEnabled(false);
    m_snapshotLoadButton->setToolTip(QStringLiteral("Load Btrfs root snapshots through the privileged helper using read-only mounts."));
    m_snapshotInspectButton->setToolTip(QStringLiteral("Inspect the selected snapshot read-only, including OS metadata, fstab/crypttab and visible kernel files."));
    m_snapshotRollbackButton->setToolTip(QStringLiteral("Run a read-only rollback preflight, then promote a writable copy of the selected snapshot to @ with automatic root restoration if boot-stack reconciliation fails."));

    connect(m_snapshotLoadButton, &QPushButton::clicked, this, &MainWindow::loadSnapshots);
    connect(m_snapshotInspectButton, &QPushButton::clicked, this, &MainWindow::inspectSelectedSnapshot);
    connect(m_snapshotRollbackButton, &QPushButton::clicked, this, &MainWindow::rollbackSelectedSnapshot);
    connect(m_snapshotTable, &QTableWidget::itemSelectionChanged, this, [this] { updateSnapshotControls(); });
    connect(m_snapshotTable, &QTableWidget::cellDoubleClicked, this,
            [this](int, int) { inspectSelectedSnapshot(); });
    m_snapshotTable->setToolTip(QStringLiteral(
        "Select a snapshot to enable the actions below, or double-click a row to inspect it read-only."));

    m_snapshotButtonLayout->addWidget(m_snapshotLoadButton);
    m_snapshotButtonLayout->addWidget(m_snapshotInspectButton);
    m_snapshotButtonLayout->addWidget(m_snapshotRollbackButton);
    m_snapshotButtonLayout->addStretch(1);
    layout->addLayout(m_snapshotButtonLayout);

    scroll->setWidget(content);
    outerLayout->addWidget(scroll, 1);

    return page;
}

QWidget *MainWindow::buildChrootShellPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *heading = new QHBoxLayout;
    auto *title = new QLabel(QStringLiteral("Chroot shell"));
    QFont titleFont = title->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() * 1.25);
    title->setFont(titleFont);
    heading->addWidget(title);
    heading->addStretch();
    m_chrootShellTargetLabel = new QLabel(QStringLiteral("Target: none selected"));
    m_chrootShellTargetLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_chrootShellTargetLabel->setWordWrap(true);
    heading->addWidget(m_chrootShellTargetLabel);
    layout->addLayout(heading);

    auto *notice = new QLabel(QStringLiteral(
        "Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot; output is kept in this window and in the application log."));
    notice->setWordWrap(true);
    layout->addWidget(notice);

    auto *commandRow = new QHBoxLayout;
    m_chrootShellCommandEdit = new QLineEdit;
    m_chrootShellCommandEdit->setPlaceholderText(QStringLiteral("Command, for example: apt update or update-grub"));
    m_chrootShellCommandEdit->setAccessibleName(QStringLiteral("Chroot shell command"));
    m_chrootShellCommandEdit->setClearButtonEnabled(true);
    commandRow->addWidget(m_chrootShellCommandEdit, 1);
    m_chrootShellRunButton = new QPushButton(themedIcon(QStringLiteral("utilities-terminal")), QStringLiteral("Run Command"));
    m_chrootShellRunButton->setToolTip(QStringLiteral("Execute the command inside the selected repair system as root."));
    commandRow->addWidget(m_chrootShellRunButton);
    layout->addLayout(commandRow);

    m_chrootShellOutput = new QPlainTextEdit;
    m_chrootShellOutput->setReadOnly(true);
    m_chrootShellOutput->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_chrootShellOutput->setPlaceholderText(QStringLiteral("Command output will appear here."));
    QFont mono(QStringLiteral("monospace"));
    mono.setStyleHint(QFont::Monospace);
    m_chrootShellOutput->setFont(mono);
    layout->addWidget(m_chrootShellOutput, 1);

    auto *footer = new QHBoxLayout;
    auto *warning = new QLabel(QStringLiteral("Commands can modify the target system. Review each command before running it."));
    warning->setWordWrap(true);
    footer->addWidget(warning, 1);
    auto *clear = new QPushButton(themedIcon(QStringLiteral("edit-clear")), QStringLiteral("Clear Output"));
    footer->addWidget(clear);
    layout->addLayout(footer);

    connect(m_chrootShellRunButton, &QPushButton::clicked, this, &MainWindow::runChrootShellCommand);
    connect(m_chrootShellCommandEdit, &QLineEdit::returnPressed, this, &MainWindow::runChrootShellCommand);
    connect(clear, &QPushButton::clicked, this, &MainWindow::clearChrootShellOutput);
    return page;
}

QWidget *MainWindow::buildFileCopyPage()
{
    auto *page = new QWidget;
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(8, 12, 8, 8);

    auto *scroll = new QScrollArea;
    scroll->setObjectName(QStringLiteral("fileCopyPageScroll"));
    scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto *content = new QWidget;
    content->setMinimumWidth(0);
    content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 8, 0);
    layout->setSpacing(10);

    auto *heading = new QHBoxLayout;
    m_fileCopyHeading = sectionTitle(QStringLiteral("File copy"));
    heading->addWidget(m_fileCopyHeading);
    heading->addWidget(contextHelpButton(page, QStringLiteral("File Copy"),
        QStringLiteral("Copy and verify files in either direction. Host to Repair remounts only the selected target filesystem read-write after safety checks; Repair to Host keeps the repair target read-only. Browse Target Folders reads the selected repair tree through temporary read-only mounts, while transfers use rsync without --delete, path containment, ownership validation and post-copy verification.")));
    heading->addStretch(1);
    m_fileCopyTargetLabel = new QLabel(QStringLiteral("Target: none selected"));
    configureTargetSummaryLabel(m_fileCopyTargetLabel);
    heading->addWidget(m_fileCopyTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addLayout(heading);

    auto *directionRow = new QHBoxLayout;
    directionRow->addWidget(new QLabel(QStringLiteral("Direction:")));
    m_fileCopyDirectionCombo = new QComboBox;
    m_fileCopyDirectionCombo->addItem(QStringLiteral("Host → Repair"));
    m_fileCopyDirectionCombo->addItem(QStringLiteral("Repair → Host"));
    m_fileCopyDirectionCombo->setToolTip(QStringLiteral("Choose which system supplies the source files and which system receives them."));
    directionRow->addWidget(m_fileCopyDirectionCombo);
    directionRow->addStretch(1);
    layout->addLayout(directionRow);

    m_fileCopySourceBox = new QGroupBox;
    auto *sourceLayout = new QVBoxLayout(m_fileCopySourceBox);
    m_sourceList = new QListWidget;
    m_sourceList->setAlternatingRowColors(true);
    m_sourceList->setMinimumHeight(130);
    m_sourceList->setTextElideMode(Qt::ElideMiddle);
    installCopyAction(m_sourceList);
    sourceLayout->addWidget(m_sourceList);

    auto *sourceButtons = new QHBoxLayout;
    m_fileCopyAddFilesButton = new QPushButton(themedIcon(QStringLiteral("document-open")), QStringLiteral("Add Files…"));
    m_fileCopyAddFolderButton = new QPushButton(themedIcon(QStringLiteral("folder-open")), QStringLiteral("Add Folder…"));
    auto *remove = new QPushButton(themedIcon(QStringLiteral("list-remove")), QStringLiteral("Remove"));
    auto *clear = new QPushButton(QStringLiteral("Clear"));
    connect(m_fileCopyAddFilesButton, &QPushButton::clicked, this, &MainWindow::addSourceFiles);
    connect(m_fileCopyAddFolderButton, &QPushButton::clicked, this, &MainWindow::addSourceFolder);
    connect(remove, &QPushButton::clicked, this, &MainWindow::removeSelectedSources);
    connect(clear, &QPushButton::clicked, this, &MainWindow::clearSourceList);
    sourceButtons->addWidget(m_fileCopyAddFilesButton);
    sourceButtons->addWidget(m_fileCopyAddFolderButton);
    sourceButtons->addWidget(remove);
    sourceButtons->addStretch(1);
    sourceButtons->addWidget(clear);
    sourceLayout->addLayout(sourceButtons);
    layout->addWidget(m_fileCopySourceBox);

    m_fileCopyDestinationBox = new QGroupBox;
    auto *destinationLayout = new QHBoxLayout(m_fileCopyDestinationBox);
    m_destinationEdit = new QLineEdit;
    m_destinationEdit->setReadOnly(true);
    m_fileCopyBrowseDestinationButton = new QPushButton(themedIcon(QStringLiteral("folder-open")), QStringLiteral("Choose Path…"));
    connect(m_fileCopyBrowseDestinationButton, &QPushButton::clicked, this, &MainWindow::browseFileCopyDestination);
    destinationLayout->addWidget(m_destinationEdit, 1);
    destinationLayout->addWidget(m_fileCopyBrowseDestinationButton);
    layout->addWidget(m_fileCopyDestinationBox);

    auto *optionsBox = new QGroupBox(QStringLiteral("3. Ownership and copy policy"));
    auto *optionsLayout = new QVBoxLayout(optionsBox);
    auto *ownershipRow = new QHBoxLayout;
    ownershipRow->addWidget(new QLabel(QStringLiteral("Ownership:")));
    m_ownershipCombo = new QComboBox;
    m_ownershipCombo->addItem(QStringLiteral("Smart destination ownership (recommended)"));
    m_ownershipCombo->addItem(QStringLiteral("Preserve source numeric UID/GID"));
    m_ownershipCombo->setEnabled(true);
    m_ownershipCombo->setToolTip(QStringLiteral("Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account."));
    ownershipRow->addWidget(m_ownershipCombo, 1);
    optionsLayout->addLayout(ownershipRow);

    layout->addWidget(optionsBox);

    auto *copyButtons = new QHBoxLayout;
    m_fileCopyPreviewButton = new QPushButton(themedIcon(QStringLiteral("document-preview")), QStringLiteral("Preview Changes"));
    m_fileCopyRunButton = new QPushButton(themedIcon(QStringLiteral("edit-copy")), QStringLiteral("Copy and Verify"));
    m_fileCopyPreviewButton->setEnabled(false);
    m_fileCopyRunButton->setEnabled(false);
    m_fileCopyPreviewButton->setToolTip(QStringLiteral("Run an rsync dry-run through the guarded helper. No files are changed."));
    m_fileCopyRunButton->setToolTip(QStringLiteral("Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted."));
    connect(m_fileCopyPreviewButton, &QPushButton::clicked, this, &MainWindow::runFileCopyPreview);
    connect(m_fileCopyRunButton, &QPushButton::clicked, this, &MainWindow::runFileCopy);
    copyButtons->addStretch(1);
    copyButtons->addWidget(m_fileCopyPreviewButton);
    copyButtons->addWidget(m_fileCopyRunButton);
    layout->addLayout(copyButtons);
    layout->addStretch(1);

    connect(m_fileCopyDirectionCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) {
        if (m_sourceList) {
            m_sourceList->clear();
        }
        if (m_destinationEdit) {
            m_destinationEdit->clear();
        }
        updateFileCopyDirection();
        const QString direction = m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentIndex() == 1
            ? QStringLiteral("Repair → Host") : QStringLiteral("Host → Repair");
        appendLog(QStringLiteral("File Copy direction changed to %1; staged paths were cleared to avoid mixing source/destination namespaces.").arg(direction));
    });
    updateFileCopyDirection();

    scroll->setWidget(content);
    outerLayout->addWidget(scroll, 1);

    return page;
}

QWidget *MainWindow::buildLogsPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 12, 8, 8);
    layout->setSpacing(8);

    auto *top = new QHBoxLayout;
    top->addWidget(sectionTitle(QStringLiteral("Application log")));
    top->addWidget(contextHelpButton(page, QStringLiteral("Logs"),
        QStringLiteral("Shows this application's session activity. Save a copy when you need to share troubleshooting details.")));
    top->addStretch(1);
    auto *save = new QPushButton(themedIcon(QStringLiteral("document-save")), QStringLiteral("Save As…"));
    auto *clear = new QPushButton(QStringLiteral("Clear Register"));
    connect(save, &QPushButton::clicked, this, &MainWindow::saveLogAs);
    connect(clear, &QPushButton::clicked, this, [this] {
        m_actionLogEntries.clear();
        if (m_settings) m_settings->setValue(QStringLiteral("actionRegister"), m_actionLogEntries);
        m_logView->clear();
        appendLog(QStringLiteral("Action register cleared. No persistent target logs were modified."));
    });
    top->addWidget(save);
    top->addWidget(clear);
    layout->addLayout(top);

    auto *filterRow = new QHBoxLayout;
    auto *filterLabel = new QLabel(QStringLiteral("Search log:"));
    m_logSearchEdit = new QLineEdit;
    m_logSearchEdit->setObjectName(QStringLiteral("logSearchEdit"));
    m_logSearchEdit->setClearButtonEnabled(true);
    m_logSearchEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_logSearchEdit->setMinimumHeight(36);
    m_logSearchEdit->setPlaceholderText(QStringLiteral("Filter application log (fuzzy match)…"));
    m_logSearchEdit->setToolTip(QStringLiteral("Type any characters to show matching application-log entries. Matching is case-insensitive and fuzzy."));
    filterRow->addWidget(filterLabel);
    filterRow->addWidget(m_logSearchEdit, 1);
    layout->addLayout(filterRow);

    m_logView = new QPlainTextEdit;
    m_logView->setReadOnly(true);
    // Logs grow continuously. Reserve the vertical scrollbar gutter from the
    // beginning so wrapped lines do not reflow sideways when scrolling starts.
    m_logView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_logView->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_logView->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    QFont mono = QFont(QStringLiteral("monospace"));
    mono.setStyleHint(QFont::Monospace);
    m_logView->setFont(mono);
    connect(m_logSearchEdit, &QLineEdit::textChanged, this, [this] { refreshLogView(); });
    refreshLogView();
    layout->addWidget(m_logView, 1);

    return page;
}

QWidget *MainWindow::buildSettingsPage()
{
    auto *page = new QWidget;
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(8, 8, 8, 8);
    outerLayout->setSpacing(8);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto *content = new QWidget;
    // Keep the inner frame pinned to the viewport width.  File Copy uses a
    // full-page vertical scroll area and must not expose a hidden horizontal
    // scroll range when button rows are close to the available width.
    content->setMinimumWidth(0);
    content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 8, 0);
    layout->setSpacing(9);

    auto *deviceBox = new QGroupBox(QStringLiteral("Device discovery"));
    auto *deviceLayout = new QVBoxLayout(deviceBox);
    m_showNonLinux = new QCheckBox(QStringLiteral("Show devices without an identified Linux installation"));
    m_showRemovable = new QCheckBox(QStringLiteral("Show removable and USB storage"));
    m_showEncrypted = new QCheckBox(QStringLiteral("Show encrypted devices before unlocking"));
    deviceLayout->addWidget(m_showNonLinux);
    deviceLayout->addWidget(m_showRemovable);
    deviceLayout->addWidget(m_showEncrypted);
    layout->addWidget(deviceBox);

    auto *repairBox = new QGroupBox(QStringLiteral("Full Repair plan"));
    auto *repairLayout = new QVBoxLayout(repairBox);
    m_fullRepairDpkg = new QCheckBox(QStringLiteral("Complete interrupted package configuration"));
    m_fullRepairBrokenPackages = new QCheckBox(QStringLiteral("Repair broken package dependencies"));
    m_fullRepairAptUpdate = new QCheckBox(QStringLiteral("Refresh package metadata"));
    m_fullRepairUpgrade = new QCheckBox(QStringLiteral("Upgrade installed packages (adaptive APT simulation)"));
    m_fullRepairDkms = new QCheckBox(QStringLiteral("Rebuild DKMS modules"));
    m_fullRepairDisplayManager = new QCheckBox(QStringLiteral("Restore detected graphical login manager and graphical.target"));
    m_fullRepairInitramfs = new QCheckBox(QStringLiteral("Rebuild initramfs after mapper/crypttab validation"));
    m_fullRepairEfi = new QCheckBox(QStringLiteral("Repair EFI / UKI boot path (explicit target ESP repair)"));
    m_fullRepairGrub = new QCheckBox(QStringLiteral("Update GRUB configuration"));
    repairLayout->addWidget(m_fullRepairDpkg);
    repairLayout->addWidget(m_fullRepairBrokenPackages);
    repairLayout->addWidget(m_fullRepairAptUpdate);
    repairLayout->addWidget(m_fullRepairUpgrade);
    repairLayout->addWidget(m_fullRepairDkms);
    repairLayout->addWidget(m_fullRepairDisplayManager);
    repairLayout->addWidget(m_fullRepairInitramfs);
    repairLayout->addWidget(m_fullRepairEfi);
    repairLayout->addWidget(m_fullRepairGrub);
    layout->addWidget(repairBox);

    auto *safetyBox = new QGroupBox(QStringLiteral("Mandatory safety controls"));
    auto *safetyLayout = new QVBoxLayout(safetyBox);
    const QStringList safetyItems = {
        QStringLiteral("Protect every physical device backing /, /boot and /boot/efi"),
        QStringLiteral("Require explicit confirmation before package installation or repair actions"),
        QStringLiteral("Require mapper/crypttab consistency before initramfs rebuild"),
        QStringLiteral("Preserve pre-rollback Btrfs root and auto-restore it on validation failure"),
        QStringLiteral("Never log LUKS passphrases or authentication secrets")
    };
    for (const QString &text : safetyItems) {
        auto *check = new QCheckBox(text);
        check->setChecked(true);
        check->setEnabled(false);
        safetyLayout->addWidget(check);
    }
    layout->addWidget(safetyBox);

    auto *capabilityBox = new QGroupBox(QStringLiteral("Host capabilities and dependencies"));
    auto *capabilityLayout = new QVBoxLayout(capabilityBox);
    auto *identityLayout = new QFormLayout;
    m_distributionLabel = new QLabel;
    m_packageManagerLabel = new QLabel;
    m_authBuildLabel = new QLabel;
    identityLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    identityLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    identityLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    identityLayout->setHorizontalSpacing(14);
    identityLayout->setVerticalSpacing(6);
    for (QLabel *value : {m_distributionLabel, m_packageManagerLabel, m_authBuildLabel}) {
        value->setWordWrap(true);
        value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }
    identityLayout->addRow(QStringLiteral("Distribution:"), m_distributionLabel);
    identityLayout->addRow(QStringLiteral("Package manager family:"), m_packageManagerLabel);
    identityLayout->addRow(QStringLiteral("KAuth build support:"), m_authBuildLabel);
    capabilityLayout->addLayout(identityLayout);

    m_capabilityTable = new QTableWidget(0, 6);
    m_capabilityTable->setHorizontalHeaderLabels({
        QStringLiteral("Feature"), QStringLiteral("Command"), QStringLiteral("Scope"),
        QStringLiteral("Status"), QStringLiteral("Suggested package"), QStringLiteral("Notes")
    });
    m_capabilityTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_capabilityTable->setMinimumHeight(230);
    m_capabilityTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_capabilityTable->setAlternatingRowColors(true);
    m_capabilityTable->setWordWrap(true);
    m_capabilityTable->setTextElideMode(Qt::ElideRight);
    m_capabilityTable->setFrameShape(QFrame::StyledPanel);
    m_capabilityTable->setFrameShadow(QFrame::Plain);
    m_capabilityTable->setLineWidth(1);
    installCopyAction(m_capabilityTable);
    m_capabilityTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_capabilityTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_capabilityTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_capabilityTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_capabilityTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_capabilityTable->horizontalHeader()->setStretchLastSection(true);
    // Keep rows content-sized across desktop styles.  Wrapped rows are
    // explicitly enlarged by resizeCapabilityRows(); all others stay at the
    // compact one-line height.
    m_capabilityTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_capabilityTable->verticalHeader()->setMinimumSectionSize(
        qMax(24, m_capabilityTable->fontMetrics().lineSpacing() + 10));
    connect(m_capabilityTable->horizontalHeader(), &QHeaderView::sectionResized, this, [this] {
        QTimer::singleShot(0, this, [this] { resizeCapabilityRows(); });
    });
    capabilityLayout->addWidget(m_capabilityTable);

    auto *capabilityButtons = new QHBoxLayout;
    auto *refresh = new QPushButton(themedIcon(QStringLiteral("view-refresh")), QStringLiteral("Refresh Capabilities"));
    auto *install = new QPushButton(themedIcon(QStringLiteral("system-software-install")), QStringLiteral("Install Missing Support…"));
    install->setEnabled(false);
    install->setToolTip(QStringLiteral("Automatic installation will require explicit package mapping and privilege authorization."));
    connect(refresh, &QPushButton::clicked, this, &MainWindow::refreshCapabilities);
    capabilityButtons->addWidget(refresh);
    capabilityButtons->addStretch(1);
    capabilityButtons->addWidget(install);
    capabilityLayout->addLayout(capabilityButtons);
    layout->addWidget(capabilityBox);
    layout->addStretch(1);

    auto refilterDevices = [this] {
        if (!m_lastDevices.isEmpty()) {
            populateDeviceTree(m_lastDevices);
        }
    };
    connect(m_showNonLinux, &QCheckBox::toggled, this, refilterDevices);
    connect(m_showRemovable, &QCheckBox::toggled, this, refilterDevices);
    connect(m_showEncrypted, &QCheckBox::toggled, this, refilterDevices);

    const QList<QCheckBox *> repairToggles = {
        m_fullRepairDpkg, m_fullRepairBrokenPackages, m_fullRepairAptUpdate,
        m_fullRepairUpgrade, m_fullRepairDkms, m_fullRepairDisplayManager, m_fullRepairInitramfs, m_fullRepairEfi, m_fullRepairGrub
    };
    for (QCheckBox *check : repairToggles) {
        connect(check, &QCheckBox::toggled, this, &MainWindow::updateFullRepairSummary);
    }

    scroll->setWidget(content);
    outerLayout->addWidget(scroll, 1);

    return page;
}

void MainWindow::loadSettings()
{
    const QByteArray geometry = m_settings->value(QStringLiteral("window/geometry")).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

    // Always begin in Systems so the selected/protected drive context is
    // visible before any diagnostic or repair action is considered.
    m_tabs->setCurrentIndex(SystemsTab);

    m_showNonLinux->setChecked(m_settings->value(QStringLiteral("devices/showNonLinux"), true).toBool());
    m_showRemovable->setChecked(m_settings->value(QStringLiteral("devices/showRemovable"), true).toBool());
    m_showEncrypted->setChecked(m_settings->value(QStringLiteral("devices/showEncrypted"), true).toBool());

    m_fullRepairDpkg->setChecked(m_settings->value(QStringLiteral("repair/dpkgConfigure"), true).toBool());
    m_fullRepairBrokenPackages->setChecked(m_settings->value(QStringLiteral("repair/fixBroken"), true).toBool());
    m_fullRepairAptUpdate->setChecked(m_settings->value(QStringLiteral("repair/refreshMetadata"), true).toBool());
    m_fullRepairUpgrade->setChecked(m_settings->value(QStringLiteral("repair/upgradePackages"), false).toBool());
    m_fullRepairDkms->setChecked(m_settings->value(QStringLiteral("repair/dkms"), true).toBool());
    m_fullRepairDisplayManager->setChecked(m_settings->value(QStringLiteral("repair/displayManager"), false).toBool());
    m_fullRepairInitramfs->setChecked(m_settings->value(QStringLiteral("repair/initramfs"), true).toBool());
    m_fullRepairEfi->setChecked(m_settings->value(QStringLiteral("repair/efiBootloader"), false).toBool());
    m_fullRepairGrub->setChecked(m_settings->value(QStringLiteral("repair/grub"), true).toBool());

    const bool wrapLogs = m_settings->value(QStringLiteral("logs/wrapLines"), true).toBool();
    if (m_wrapLogsAction) {
        m_wrapLogsAction->setChecked(wrapLogs);
    }
    setLogWrapEnabled(wrapLogs);

    const QByteArray headerState = m_settings->value(QStringLiteral("systems/headerStateV3")).toByteArray();
    if (!headerState.isEmpty() && m_deviceTree) {
        m_deviceTree->header()->restoreState(headerState);
    }

    const QByteArray splitterState = m_settings->value(QStringLiteral("systems/splitterStateV3")).toByteArray();
    if (!splitterState.isEmpty() && m_systemSplitter) {
        m_systemSplitter->restoreState(splitterState);
    }

    const QByteArray repairSplitterState = m_settings->value(QStringLiteral("repair/splitterStateV3")).toByteArray();
    if (!repairSplitterState.isEmpty() && m_repairSplitter) {
        m_repairSplitter->restoreState(repairSplitterState);
    }
    const QByteArray repairVerticalSplitterState = m_settings->value(QStringLiteral("repair/verticalSplitterStateV1")).toByteArray();
    if (!repairVerticalSplitterState.isEmpty() && m_repairVerticalSplitter) {
        m_repairVerticalSplitter->restoreState(repairVerticalSplitterState);
    }

    const QByteArray diagnosticSplitterState = m_settings->value(QStringLiteral("diagnostics/splitterStateV1")).toByteArray();
    if (!diagnosticSplitterState.isEmpty() && m_diagnosticSplitter) {
        m_diagnosticSplitter->restoreState(diagnosticSplitterState);
    }
    const QByteArray snapshotSplitterState = m_settings->value(QStringLiteral("snapshots/splitterStateV1")).toByteArray();
    if (!snapshotSplitterState.isEmpty() && m_snapshotSplitter) {
        m_snapshotSplitter->restoreState(snapshotSplitterState);
    }

    updateFullRepairSummary();
    updateDiagnosticDetails();
}

void MainWindow::saveSettings()
{
    m_settings->setValue(QStringLiteral("window/geometry"), saveGeometry());
    m_settings->setValue(QStringLiteral("window/tabV4"), m_tabs->currentIndex());
    m_settings->setValue(QStringLiteral("devices/showNonLinux"), m_showNonLinux->isChecked());
    m_settings->setValue(QStringLiteral("devices/showRemovable"), m_showRemovable->isChecked());
    m_settings->setValue(QStringLiteral("devices/showEncrypted"), m_showEncrypted->isChecked());
    m_settings->setValue(QStringLiteral("repair/dpkgConfigure"), m_fullRepairDpkg->isChecked());
    m_settings->setValue(QStringLiteral("repair/fixBroken"), m_fullRepairBrokenPackages->isChecked());
    m_settings->setValue(QStringLiteral("repair/refreshMetadata"), m_fullRepairAptUpdate->isChecked());
    m_settings->setValue(QStringLiteral("repair/upgradePackages"), m_fullRepairUpgrade->isChecked());
    m_settings->setValue(QStringLiteral("repair/dkms"), m_fullRepairDkms->isChecked());
    m_settings->setValue(QStringLiteral("repair/displayManager"), m_fullRepairDisplayManager->isChecked());
    m_settings->setValue(QStringLiteral("repair/initramfs"), m_fullRepairInitramfs->isChecked());
    m_settings->setValue(QStringLiteral("repair/efiBootloader"), m_fullRepairEfi->isChecked());
    m_settings->setValue(QStringLiteral("repair/grub"), m_fullRepairGrub->isChecked());
    m_settings->setValue(QStringLiteral("logs/wrapLines"), m_wrapLogsAction ? m_wrapLogsAction->isChecked() : true);

    if (m_deviceTree) {
        m_settings->setValue(QStringLiteral("systems/headerStateV3"), m_deviceTree->header()->saveState());
    }
    if (m_systemSplitter) {
        m_settings->setValue(QStringLiteral("systems/splitterStateV3"), m_systemSplitter->saveState());
    }
    if (m_repairSplitter) {
        m_settings->setValue(QStringLiteral("repair/splitterStateV3"), m_repairSplitter->saveState());
    }
    if (m_repairVerticalSplitter) {
        m_settings->setValue(QStringLiteral("repair/verticalSplitterStateV1"), m_repairVerticalSplitter->saveState());
    }
    if (m_diagnosticSplitter) {
        m_settings->setValue(QStringLiteral("diagnostics/splitterStateV1"), m_diagnosticSplitter->saveState());
    }
    if (m_snapshotSplitter) {
        m_settings->setValue(QStringLiteral("snapshots/splitterStateV1"), m_snapshotSplitter->saveState());
    }

    m_settings->sync();
}

void MainWindow::refreshDevices()
{
    statusBar()->showMessage(QStringLiteral("Scanning block devices…"));
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString error;
    QStringList diagnostics;
    const QList<DeviceNode> devices = m_scanner->scan(&error, &diagnostics);

    QApplication::restoreOverrideCursor();

    if (!error.isEmpty()) {
        appendLog(error, QStringLiteral("ERROR"));
        QMessageBox::critical(this, QStringLiteral("Device scan failed"), error);
        statusBar()->showMessage(QStringLiteral("Device scan failed"), 5000);
        return;
    }

    m_lastDevices = devices;
    populateDeviceTree(m_lastDevices);
    for (const QString &line : diagnostics) {
        appendLog(line);
    }
    statusBar()->showMessage(QStringLiteral("Device scan complete — discovery did not modify storage"), 4000);
}

void MainWindow::populateDeviceTree(const QList<DeviceNode> &devices)
{
    m_deviceTree->clear();
    m_deviceIndex.clear();

    for (const DeviceNode &device : devices) {
        indexDevice(device);
    }

    updateHostSystemSummary(devices);

    QList<DeviceNode> candidates;
    int repairCandidateTotal = 0;
    int protectedTopLevel = 0;

    for (const DeviceNode &device : devices) {
        if (device.protectedDevice) {
            ++protectedTopLevel;
            continue;
        }

        ++repairCandidateTotal;

        if (devicePassesTopLevelFilters(device)) {
            candidates.append(device);
        }
    }

    std::stable_sort(candidates.begin(), candidates.end(), [](const DeviceNode &left, const DeviceNode &right) {
        const int leftScore = repairLikelihoodScore(left);
        const int rightScore = repairLikelihoodScore(right);
        if (leftScore != rightScore) {
            return leftScore < rightScore;
        }
        return QString::localeAwareCompare(friendlyNodeName(left), friendlyNodeName(right)) < 0;
    });

    for (const DeviceNode &device : candidates) {
        addDeviceItem(nullptr, device);
    }

    updateCommittedTargetVisual();
    m_deviceTree->collapseAll();
    updateDeviceTreeHeight();
    updateDeviceDetails();
    updateSnapshotControls();

    statusBar()->showMessage(
        QStringLiteral("Showing %1 of %2 repair candidate disk(s); %3 running-system disk(s) protected")
            .arg(candidates.size())
            .arg(repairCandidateTotal)
            .arg(protectedTopLevel),
        4500);
}

void MainWindow::updateHostSystemSummary(const QList<DeviceNode> &devices)
{
    if (!m_hostSystemLabel || !m_hostStorageLabel || !m_hostMountsLabel || !m_hostDetailsButton) {
        return;
    }

    QString osName;
    QStringList storageLines;
    QStringList criticalMounts;
    m_hostPrimaryPath.clear();
    m_hostPrimaryComponentPath.clear();

    auto treeHasMount = [](auto &&self, const DeviceNode &node, const QString &mount) -> bool {
        if (node.mountPoints.contains(mount)) {
            return true;
        }
        for (const DeviceNode &child : node.children) {
            if (self(self, child, mount)) {
                return true;
            }
        }
        return false;
    };

    for (const DeviceNode &device : devices) {
        if (!device.protectedDevice) {
            continue;
        }

        // Prefer the protected disk that actually backs `/`; systems with a
        // separate EFI or /boot disk may expose more than one protected tree.
        if ((m_hostPrimaryPath.isEmpty() || treeHasMount(treeHasMount, device, QStringLiteral("/")))
            && !device.path.isEmpty()) {
            m_hostPrimaryPath = device.path;
            const DeviceNode *preferred = preferredRepairNode(device);
            m_hostPrimaryComponentPath = preferred && !preferred->path.isEmpty()
                ? preferred->path : device.path;
        }

        if (osName.isEmpty()) {
            osName = firstLinuxNameInTree(device);
        }

        QStringList description;
        const QString model = combinedModel(device);
        if (!model.isEmpty()) {
            description.append(model);
        }
        if (!device.path.isEmpty()) {
            description.append(device.path);
        }
        if (device.sizeBytes > 0) {
            description.append(SystemScanner::humanSize(device.sizeBytes));
        }
        if (!device.transport.isEmpty()) {
            description.append(device.transport.toUpper());
        }

        storageLines.append(description.join(QStringLiteral("  •  ")));
        collectCriticalMounts(device, criticalMounts);
    }

    if (storageLines.isEmpty()) {
        m_hostSystemLabel->setText(QStringLiteral("Running system protection unresolved"));
        m_hostStorageLabel->setText(QStringLiteral("No protected physical backing disk was identified"));
        m_hostMountsLabel->clear();
        m_hostMountsLabel->setVisible(false);
        m_hostDetailsButton->setEnabled(false);
        m_hostMaintenanceButton->setEnabled(false);
        m_hostDefaultButton->setEnabled(false);
        return;
    }

    m_hostSystemLabel->setText(osName.isEmpty()
        ? QStringLiteral("Current running Linux system")
        : osName);
    criticalMounts.sort();
    const QString mountSummary = criticalMounts.isEmpty()
        ? QStringLiteral("Protected running-system storage")
        : QStringLiteral("Critical mounts: %1").arg(criticalMounts.join(QStringLiteral(", ")));
    const QString storageSummary = storageLines.join(QStringLiteral("  |  "));
    m_hostStorageLabel->setText(QStringLiteral("%1  •  %2").arg(storageSummary, mountSummary));
    m_hostStorageLabel->setToolTip(storageLines.join(QLatin1Char('\n')) + QLatin1Char('\n') + mountSummary);
    m_hostMountsLabel->clear();
    m_hostMountsLabel->setVisible(false);
    m_hostDetailsButton->setEnabled(!m_hostPrimaryPath.isEmpty());
    m_hostMaintenanceButton->setEnabled(!m_hostPrimaryPath.isEmpty() && !m_hostPrimaryComponentPath.isEmpty());
    m_hostDefaultButton->setEnabled(!m_hostPrimaryPath.isEmpty() && !m_hostPrimaryComponentPath.isEmpty());
}

void MainWindow::addDeviceItem(QTreeWidgetItem *parent, const DeviceNode &node)
{
    const QString nameText = friendlyNodeName(node);
    const QString statusText = parent ? (node.status.isEmpty() ? QStringLiteral("—") : node.status)
                                      : friendlyTopLevelStatus(node);
    const QString connectionText = node.transport.isEmpty() ? QStringLiteral("—") : node.transport.toUpper();
    const QString sizeText = SystemScanner::humanSize(node.sizeBytes);
    const QString fileSystemText = node.fileSystem.isEmpty() ? QStringLiteral("—") : node.fileSystem;
    const QString deviceText = node.path.isEmpty() ? node.name : node.path;

    auto *item = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(m_deviceTree);
    item->setIcon(0, iconForDevice(node));
    // Keep the device identity icon in column 0 and show the detected
    // filesystem type alongside its text.  This gives Btrfs/ext4 and other
    // filesystems a useful visual cue without depending on a particular
    // desktop's icon-theme vocabulary.
    if (!node.fileSystem.isEmpty()) {
        item->setIcon(4, filesystemIcon(node.fileSystem));
    }
    item->setText(0, nameText);
    item->setText(1, statusText);
    item->setText(2, connectionText);
    item->setText(3, sizeText);
    item->setText(4, fileSystemText);
    item->setText(5, deviceText);
    item->setData(0, Qt::UserRole, node.path);
    item->setData(1, Qt::UserRole, statusText); // unmodified status, used by persistent committed-target styling

    const QStringList fullValues = {
        nameText, statusText, connectionText, sizeText, fileSystemText, deviceText
    };
    for (int column = 0; column < fullValues.size(); ++column) {
        item->setToolTip(column, fullValues.at(column));
    }

    if (parent) {
        // Child partition/mapper rows are selectable for inspection only.  The
        // Select Target action still resolves to the parent physical disk and
        // its preferred Linux root, so clicking a child cannot bypass the
        // physical-target safety boundary.
        item->setToolTip(0, QStringLiteral("Select to inspect this partition/volume. Select Target still chooses the physical drive and its preferred Linux root.\n%1").arg(nameText));
    } else {
        QFont font = item->font(0);
        font.setBold(true);
        item->setFont(0, font);
        item->setToolTip(0, QStringLiteral("Select this physical drive as the repair target. Expand it only to view technical partition/volume details.\n%1").arg(nameText));
    }

    const bool benefitsFromSecondLine = nameText.size() > 34 || statusText.size() > 38;
    if (benefitsFromSecondLine) {
        const int height = m_deviceTree->fontMetrics().lineSpacing() * 2 + 8;
        item->setSizeHint(0, QSize(0, height));
        item->setSizeHint(1, QSize(0, height));
    }

    for (const DeviceNode &child : node.children) {
        addDeviceItem(item, child);
    }
}

void MainWindow::indexDevice(const DeviceNode &node)
{
    if (!node.path.isEmpty()) {
        m_deviceIndex.insert(node.path, node);
    }
    for (const DeviceNode &child : node.children) {
        indexDevice(child);
    }
}

void MainWindow::updateDeviceDetails()
{
    const QList<QTreeWidgetItem *> selected = m_deviceTree->selectedItems();
    if (selected.isEmpty()) {
        const QList<QLabel *> labels = {
            m_detailPath, m_detailResolvedTarget, m_detailModel, m_detailSize, m_detailTransport,
            m_detailFilesystem, m_detailUuid, m_detailMounts, m_detailStatus, m_detailProtection
        };
        for (QLabel *label : labels) {
            label->setText(QStringLiteral("—"));
        }
        m_setTargetButton->setEnabled(false);
        if (m_unlockTargetButton) {
            m_unlockTargetButton->setText(QStringLiteral("Unlock"));
            m_unlockTargetButton->setEnabled(false);
            m_unlockTargetButton->setToolTip(QStringLiteral("Select a drive whose detected target is a locked LUKS volume."));
        }
        if (m_unlockStatusView) {
            m_unlockStatusView->setPlainText(QStringLiteral("Select a drive to see unlock status."));
        }
        return;
    }

    QTreeWidgetItem *selectedItem = selected.first();
    const QString inspectedPath = selectedItem->data(0, Qt::UserRole).toString();
    if (!m_deviceIndex.contains(inspectedPath)) {
        return;
    }
    const DeviceNode inspectedNode = m_deviceIndex.value(inspectedPath);

    QTreeWidgetItem *topItem = selectedItem;
    while (topItem->parent()) {
        topItem = topItem->parent();
    }

    const QString diskPath = topItem->data(0, Qt::UserRole).toString();
    if (!m_deviceIndex.contains(diskPath)) {
        return;
    }

    const DeviceNode disk = m_deviceIndex.value(diskPath);
    showDeviceDetails(disk, true, &inspectedNode);
}

void MainWindow::showHostDetails()
{
    if (m_hostPrimaryPath.isEmpty() || !m_deviceIndex.contains(m_hostPrimaryPath)) {
        return;
    }

    if (m_deviceTree) {
        m_deviceTree->clearSelection();
    }

    showDeviceDetails(m_deviceIndex.value(m_hostPrimaryPath), false);
    statusBar()->showMessage(QStringLiteral("Showing protected running-host details"), 3000);
}

void MainWindow::selectHostForMaintenance()
{
    QString reason;
    if (m_hostMaintenanceMode) {
        m_hostMaintenanceMode = false;
        if (m_hostMaintenanceButton) {
            m_hostMaintenanceButton->setText(QStringLiteral("Host Maintenance"));
        }
        appendLog(QStringLiteral("Running-host maintenance deselected; ordinary repair-target mode restored."));
        updateTargetLabels();
        return;
    }

    if (!hostBootTargetReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("Host maintenance unavailable"), reason);
        return;
    }

    // Keep the protected host out of the ordinary repair-target state. This
    // prevents snapshots, file copy and chroot controls from inheriting a
    // live-root selection while the explicit host-maintenance mode is active.
    m_previewTargetPath.clear();
    m_previewTargetComponentPath.clear();
    clearTargetDiagnosticCache();
    m_targetDiagnosticsNeedRegeneration = false;
    m_hostMaintenanceMode = true;
    if (m_hostMaintenanceButton) {
        m_hostMaintenanceButton->setText(QStringLiteral("Exit Host Maintenance"));
        m_hostMaintenanceButton->setToolTip(QStringLiteral(
            "Leave running-host maintenance and return to ordinary repair-target mode."));
    }
    if (m_diagnosticScopeCombo) {
        m_diagnosticScopeCombo->setCurrentIndex(1);
    }
    appendLog(QStringLiteral(
        "Running host selected for explicit maintenance: %1 (%2). All supported repair stages are available in this deliberate host scope; snapshots, shell and file-copy workflows remain separate tools.")
                  .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath));
    updateTargetLabels();
}

void MainWindow::setHostDefaultBootEntry()
{
    QString reason;
    if (!hostBootTargetReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("Host default unavailable"), reason);
        return;
    }

    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(QStringLiteral("Make host the default boot entry"));
    box.setText(QStringLiteral("Restore and select the running host's default EFI entry?"));
    box.setInformativeText(QStringLiteral(
        "Host disk: %1\nRoot: %2\n\nBoot Bitch will identify the host ESP by PARTUUID, restore a missing TUXEDO UKI registration when the file is present, and place that one host entry first in BootOrder. Existing entries on this and other disks remain in the firmware inventory.")
        .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath));
    box.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    box.setDefaultButton(QMessageBox::Cancel);
    box.button(QMessageBox::Yes)->setText(QStringLiteral("Make Default"));
    if (box.exec() != QMessageBox::Yes) {
        return;
    }

    appendLog(QStringLiteral("Starting privileged host default EFI operation on %1 (%2).")
                  .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath));
    bool succeeded = false;
    const QString output = runPrivilegedRequest(
        QStringLiteral("Make host default boot entry"),
        {QStringLiteral("host-default"), m_hostPrimaryPath, m_hostPrimaryComponentPath},
        QByteArray(), &succeeded);
    const QString detail = output.trimmed().isEmpty()
        ? QStringLiteral("The privileged helper returned no diagnostic output.")
        : output.trimmed();
    appendLog(QStringLiteral("Host default EFI output\nDiagnostic: Make host default boot entry\n%1").arg(detail),
              succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    m_hostDiagnosticCache.clear();
    m_hostDiagnosticTimes.clear();
    if (succeeded) {
        QMessageBox::information(
            this,
            QStringLiteral("Host default restored"),
            QStringLiteral("The running host's TUXEDO UKI entry was restored and placed first in BootOrder.\n\nFull helper output is available in Logs."));
    } else {
        QMessageBox::critical(this, QStringLiteral("Host default operation failed"),
                              QStringLiteral("The host default operation failed. Review Logs for details."));
    }
}

void MainWindow::showDeviceDetails(const DeviceNode &disk, bool allowRepairTarget, const DeviceNode *inspectedNode)
{
    const DeviceNode *preferred = preferredRepairNode(disk);

    const bool inspectingChild = inspectedNode && !inspectedNode->path.isEmpty() && inspectedNode->path != disk.path;
    const DeviceNode *detailNode = inspectingChild ? inspectedNode : (preferred ? preferred : &disk);

    const QString resolvedPath = detailNode && !detailNode->path.isEmpty() ? detailNode->path : disk.path;
    const QString resolvedFs = detailNode && !detailNode->fileSystem.isEmpty() ? detailNode->fileSystem : QStringLiteral("—");
    const QString resolvedUuid = detailNode && !detailNode->uuid.isEmpty() ? detailNode->uuid : QStringLiteral("—");
    const QString resolvedMounts = detailNode ? SystemScanner::mountPointsText(*detailNode) : QStringLiteral("—");
    const QString detailTransport = detailNode && !detailNode->transport.isEmpty()
        ? detailNode->transport.toUpper()
        : (disk.transport.isEmpty() ? QStringLiteral("—") : disk.transport.toUpper());

    m_detailPath->setText(disk.path.isEmpty() ? QStringLiteral("—") : disk.path);
    m_detailResolvedTarget->setText(resolvedPath.isEmpty() ? QStringLiteral("Pending inspection") : resolvedPath);
    m_detailModel->setText(inspectingChild && detailNode ? friendlyNodeName(*detailNode) : friendlyNodeName(disk));
    m_detailStatus->setText(inspectingChild && detailNode
        ? (detailNode->status.isEmpty() ? QStringLiteral("—") : detailNode->status)
        : friendlyTopLevelStatus(disk));
    m_detailSize->setText(inspectingChild && detailNode
        ? SystemScanner::humanSize(detailNode->sizeBytes)
        : SystemScanner::humanSize(disk.sizeBytes));
    m_detailTransport->setText(detailTransport);
    m_detailFilesystem->setText(resolvedFs);
    m_detailUuid->setText(resolvedUuid);
    m_detailMounts->setText(resolvedMounts);
    m_detailProtection->setText(disk.protectedDevice
        ? QStringLiteral("PROTECTED — running system; read-only details only")
        : QStringLiteral("Eligible repair candidate"));

    // A locked LUKS container is an unlock candidate, not yet a repair
    // target.  Selecting it before a Linux root is visible used to commit an
    // unusable component and could crash later target diagnostics.  Keep
    // Select Target disabled until unlock/refresh exposes a Linux filesystem.
    const bool hasTargetCandidate = treeContainsLinuxCandidateNode(disk);
    const bool canTarget = allowRepairTarget && !disk.protectedDevice
        && !disk.path.isEmpty() && hasTargetCandidate;
    m_setTargetButton->setEnabled(canTarget);
    m_setTargetButton->setToolTip(canTarget
        ? QStringLiteral("Commit this physical drive as the repair target.")
        : (allowRepairTarget && !disk.protectedDevice
            ? (treeContainsEncryptedNode(disk)
                ? QStringLiteral("Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.")
                : QStringLiteral("No Linux filesystem was detected on this drive."))
            : QStringLiteral("The protected running host cannot be selected as a repair target.")));

    const DeviceNode *unlockCandidate = nullptr;
    if (inspectedNode && inspectedNode->encrypted && !encryptedNodeHasUnlockedLinuxChild(*inspectedNode)) {
        unlockCandidate = inspectedNode;
    } else {
        unlockCandidate = firstLockedEncryptedNode(disk);
    }
    const bool canUnlock = allowRepairTarget && !disk.protectedDevice && unlockCandidate
        && !disk.path.isEmpty() && !unlockCandidate->path.isEmpty();
    const bool alreadyUnlocked = treeContainsUnlockedLinuxInsideEncrypted(disk) && !unlockCandidate;
    if (m_unlockStatusView) {
        const QString cached = m_unlockStatusCache.value(disk.path).trimmed();
        if (!cached.isEmpty()) {
            m_unlockStatusView->setPlainText(cached);
        } else if (alreadyUnlocked) {
            m_unlockStatusView->setPlainText(QStringLiteral(
                "Already unlocked before this Boot Bitch session. No unlock operation was performed here; the visible mapper will be reused and will not be closed by Boot Bitch."));
        } else {
            m_unlockStatusView->setPlainText(QStringLiteral("No unlock operation recorded for this drive in the current session."));
        }
        m_unlockStatusView->moveCursor(QTextCursor::End);
    }
    if (m_unlockTargetButton) {
        m_unlockTargetButton->setText(alreadyUnlocked ? QStringLiteral("Already Unlocked") : QStringLiteral("Unlock"));
        m_unlockTargetButton->setEnabled(canUnlock);
        if (canUnlock) {
            m_unlockTargetButton->setToolTip(
                QStringLiteral("Unlock %1 using cryptsetup through the privileged helper. The passphrase is sent on standard input and is never placed in command arguments or logs.")
                    .arg(unlockCandidate->path));
        } else if (alreadyUnlocked && allowRepairTarget && !disk.protectedDevice) {
            m_unlockTargetButton->setToolTip(QStringLiteral(
                "An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by another recovery tool."));
        } else if (allowRepairTarget && !disk.protectedDevice) {
            m_unlockTargetButton->setToolTip(QStringLiteral("No locked LUKS component is currently visible on this selected drive."));
        } else {
            m_unlockTargetButton->setToolTip(QStringLiteral("The protected running host cannot be unlocked or modified by Boot Bitch."));
        }
    }
}

void MainWindow::setPreviewTarget()
{
    const QList<QTreeWidgetItem *> selected = m_deviceTree->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    QTreeWidgetItem *selectedItem = selected.first();
    while (selectedItem->parent()) {
        selectedItem = selectedItem->parent();
    }

    const QString path = selectedItem->data(0, Qt::UserRole).toString();
    // Device rows can disappear while udev is refreshing (and a non-Linux
    // disk may have no usable component at all).  Never continue with a
    // default-constructed node: that used to leave an empty component path
    // behind and could crash downstream target refresh/update code.
    if (path.isEmpty() || !m_deviceIndex.contains(path)) {
        QMessageBox::warning(this, QStringLiteral("Target unavailable"),
                             QStringLiteral("The selected drive is no longer available. Refresh devices and select it again."));
        m_setTargetButton->setEnabled(false);
        return;
    }
    if (m_hostMaintenanceMode) {
        m_hostMaintenanceMode = false;
        if (m_hostMaintenanceButton) {
            m_hostMaintenanceButton->setText(QStringLiteral("Host Maintenance"));
            m_hostMaintenanceButton->setToolTip(QStringLiteral(
                "Select the running host for deliberate guarded maintenance. All supported repair stages run against the active system; snapshots, shell and file-copy workflows remain separate tools."));
        }
        appendLog(QStringLiteral("Repair target selection ended running-host maintenance mode."));
    }
    const DeviceNode disk = m_deviceIndex.value(path);
    if (disk.protectedDevice) {
        QMessageBox::warning(this, QStringLiteral("Protected system"),
                             QStringLiteral("The running system cannot be selected as a repair target."));
        return;
    }

    const DeviceNode *preferred = preferredRepairNode(disk);
    const bool hasLinuxCandidate = treeContainsLinuxCandidateNode(disk);
    if (!preferred || preferred->path.isEmpty() || !hasLinuxCandidate) {
        QMessageBox::information(this, QStringLiteral("Unlock or select a Linux system first"),
                                 treeContainsEncryptedNode(disk)
                                     ? QStringLiteral("This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.")
                                     : QStringLiteral("This drive does not currently expose a Linux filesystem. It cannot be selected as a repair target."));
        return;
    }
    m_previewTargetPath = disk.path;
    m_previewTargetComponentPath = preferred ? preferred->path : QString();
    m_targetDiagnosticsNeedRegeneration = false;
    updateTargetLabels();

    QString message = QStringLiteral("Repair drive selected: %1").arg(disk.path);
    if (!m_previewTargetComponentPath.isEmpty() && m_previewTargetComponentPath != disk.path) {
        message += QStringLiteral("; best detected system component: %1").arg(m_previewTargetComponentPath);
    }
    message += QStringLiteral(". No mount or repair action was performed.");
    appendLog(message);
    statusBar()->showMessage(QStringLiteral("Repair drive selected: %1").arg(disk.path), 4000);
    scheduleSnapshotPreload();
}

bool MainWindow::ensurePrivilegedSession(QString *errorMessage)
{
    auto setError = [errorMessage](const QString &text) {
        if (errorMessage) {
            *errorMessage = text;
        }
    };

    if (m_privilegedSession
        && m_privilegedSessionReady
        && m_privilegedSession->state() != QProcess::NotRunning) {
        setError(QString());
        return true;
    }

    closePrivilegedSession();

    const QString helper = repairHelperPath();
    if (helper.isEmpty()) {
        setError(QStringLiteral("The privileged Boot Bitch helper was not found. Rebuild or install this source tree."));
        return false;
    }

    QString program = helper;
    QStringList processArguments = {QStringLiteral("session")};
    const QFileInfo helperInfo(helper);
    // The AppImage may be mounted with a noexec /tmp policy.  The helper is a
    // shell script even though installation preserves its executable bit, so
    // invoke it through bash explicitly to avoid an EACCES from the mount.
    const bool invokeThroughShell = helperInfo.fileName() == QStringLiteral("boot-repair-helper")
        || helperInfo.suffix().compare(QStringLiteral("sh"), Qt::CaseInsensitive) == 0;
    if (invokeThroughShell) {
        program = QStringLiteral("/bin/bash");
        processArguments.prepend(helper);
    }
#ifdef Q_OS_UNIX
    if (geteuid() != 0) {
        program = QStandardPaths::findExecutable(QStringLiteral("pkexec"));
        if (program.isEmpty()) {
            setError(QStringLiteral("pkexec/Polkit is required to authorize privileged Boot Bitch operations."));
            return false;
        }
        if (processArguments.isEmpty() || processArguments.first() != helper) {
            processArguments.prepend(helper);
        }
        if (invokeThroughShell) {
            processArguments.prepend(QStringLiteral("/bin/bash"));
        }
    }
#endif

    m_privilegedSession = new QProcess(this);
    QProcess *session = m_privilegedSession;
    session->setProcessChannelMode(QProcess::MergedChannels);
    m_privilegedSessionReady = false;

    connect(session, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this,
            [this, session](int exitCode, QProcess::ExitStatus) {
        const bool wasReady = m_privilegedSessionReady;
        if (m_privilegedSession == session) {
            m_privilegedSessionReady = false;
            if (m_lockAuthorizationAction) {
                m_lockAuthorizationAction->setEnabled(false);
            }
        }
        if (wasReady) {
            appendLog(QStringLiteral("Administrator authorization session ended (helper exit code %1). The next privileged action will request authorization again.")
                          .arg(exitCode));
        }
    });

    RepairProgressDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Authorize Boot Bitch session"));
    dialog.resize(650, 300);
    dialog.setModal(true);

    auto *layout = new QVBoxLayout(&dialog);
    auto *status = new QLabel(QStringLiteral(
        "Administrator authorization is required once for this Boot Bitch window. "
        "The GUI remains unprivileged; a narrow root helper stays available only until you lock the session or close Boot Bitch."));
    status->setWordWrap(true);
    layout->addWidget(status);

    auto *output = new QPlainTextEdit;
    output->setReadOnly(true);
    output->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    output->setMaximumBlockCount(200);
    QFont mono(QStringLiteral("monospace"));
    mono.setStyleHint(QFont::Monospace);
    output->setFont(mono);
    layout->addWidget(output, 1);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    QPushButton *closeButton = buttons->button(QDialogButtonBox::Close);
    closeButton->setEnabled(false);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &RepairProgressDialog::reject);

    QByteArray startupBuffer;
    bool startupFailed = false;

    connect(session, &QProcess::readyReadStandardOutput, &dialog,
            [this, session, &startupBuffer, status, output, closeButton, &dialog] {
        startupBuffer += session->readAllStandardOutput();
        while (true) {
            const qsizetype newline = startupBuffer.indexOf('\n');
            if (newline < 0) {
                break;
            }
            QByteArray line = startupBuffer.left(newline);
            startupBuffer.remove(0, newline + 1);
            if (line.endsWith('\r')) {
                line.chop(1);
            }

            if (line == QByteArrayLiteral("SESSION_READY\t1")) {
                m_privilegedSessionReady = true;
                if (m_lockAuthorizationAction) {
                    m_lockAuthorizationAction->setEnabled(true);
                }
                status->setText(QStringLiteral(
                    "Administrator authorization is active for this Boot Bitch window. "
                    "Further diagnostics, validation, file-copy and repair actions will reuse this helper session without another Polkit prompt."));
                closeButton->setEnabled(true);
                dialog.setCloseAllowed(true);
                appendLog(QStringLiteral("Administrator authorization session established. The GUI remains unprivileged."));
                statusBar()->showMessage(QStringLiteral("Administrator authorization active for this Boot Bitch window"), 5000);
                QTimer::singleShot(0, &dialog, [&dialog] { dialog.accept(); });
                continue;
            }

            if (!line.isEmpty()) {
                output->appendPlainText(QString::fromLocal8Bit(line));
            }
        }
    });

    connect(session, &QProcess::errorOccurred, &dialog,
            [status, closeButton, &dialog, &startupFailed](QProcess::ProcessError) {
        startupFailed = true;
        status->setText(QStringLiteral("Unable to start the privileged Boot Bitch session."));
        closeButton->setEnabled(true);
        dialog.setCloseAllowed(true);
    });

    connect(session, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), &dialog,
            [status, closeButton, &dialog, &startupFailed, this](int exitCode, QProcess::ExitStatus) {
        if (!m_privilegedSessionReady) {
            startupFailed = true;
            status->setText(QStringLiteral(
                "Administrator authorization was cancelled or the privileged helper exited before the session became ready (exit code %1).")
                                .arg(exitCode));
            closeButton->setEnabled(true);
            dialog.setCloseAllowed(true);
        }
    });

    session->start(program, processArguments);
    dialog.exec();

    if (startupFailed || !m_privilegedSessionReady
        || !m_privilegedSession || m_privilegedSession->state() == QProcess::NotRunning) {
        setError(QStringLiteral("Administrator authorization session was not established."));
        closePrivilegedSession();
        return false;
    }

    setError(QString());
    return true;
}

void MainWindow::closePrivilegedSession()
{
    QProcess *session = m_privilegedSession;
    m_privilegedSession = nullptr;
    m_privilegedSessionReady = false;
    if (m_lockAuthorizationAction) {
        m_lockAuthorizationAction->setEnabled(false);
    }

    // Unlock status is scoped to the privileged helper session. Clear it as
    // soon as that session ends so the Systems page cannot present stale
    // mapper state after Lock Administrator Session.
    m_unlockStatusCache.clear();
    if (m_unlockStatusView) {
        m_unlockStatusView->setPlainText(QStringLiteral("No unlock operation recorded for this drive in the current session."));
    }

    if (!session) {
        return;
    }

    if (session->state() != QProcess::NotRunning) {
        session->write("QUIT\n");
        session->closeWriteChannel();
        // Give the root helper enough time to unmount request-owned paths and
        // close only dm-crypt mappings that Boot Bitch itself opened.  A
        // short timeout could kill the broker while cryptsetup cleanup was
        // still in progress, leaving a mapper behind after Lock Session.
        if (!session->waitForFinished(5000)) {
            session->terminate();
            if (!session->waitForFinished(1500)) {
                session->kill();
                session->waitForFinished(1000);
            }
        }
    }
    session->deleteLater();
}

QString MainWindow::runPrivilegedRequest(const QString &title,
                                         const QStringList &arguments,
                                         QByteArray secret,
                                         bool *succeeded,
                                         bool showProgressDialog)
{
    if (succeeded) {
        *succeeded = false;
    }
    if (arguments.isEmpty()) {
        return QStringLiteral("ERROR: No privileged helper command was supplied.\n");
    }
    // NUL cannot be represented in the line-oriented privileged-session
    // protocol or in a shell variable. Reject it before any authorization or
    // request is sent so malformed text never reaches the helper broker.
    for (const QString &argument : arguments) {
        if (argument.contains(QChar::Null)) {
            return QStringLiteral("ERROR: Privileged helper arguments cannot contain NUL bytes.\n");
        }
    }

    QString authorizationError;
    if (!ensurePrivilegedSession(&authorizationError)) {
        if (!authorizationError.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Authorization unavailable"), authorizationError);
        }
        secret.fill('\0');
        secret.clear();
        return QStringLiteral("ERROR: %1\n").arg(authorizationError);
    }

    QProcess *session = m_privilegedSession;
    if (!session || session->state() == QProcess::NotRunning) {
        secret.fill('\0');
        secret.clear();
        return QStringLiteral("ERROR: Privileged helper session is not running.\n");
    }

    const QByteArray requestId = QByteArray::number(++m_privilegedRequestCounter);
    const bool hasSecret = !secret.isEmpty();

    RepairProgressDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.resize(840, 540);
    dialog.setModal(true);

    auto *layout = new QVBoxLayout(&dialog);
    auto *status = new QLabel(QStringLiteral(
        "Using the authorized Boot Bitch administrator session. The GUI itself is still running as your normal user."));
    status->setWordWrap(true);
    layout->addWidget(status);

    auto *output = new QPlainTextEdit;
    output->setReadOnly(true);
    output->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    output->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    output->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QFont mono(QStringLiteral("monospace"));
    mono.setStyleHint(QFont::Monospace);
    output->setFont(mono);
    layout->addWidget(output, 1);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    QPushButton *closeButton = buttons->button(QDialogButtonBox::Close);
    closeButton->setEnabled(false);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &RepairProgressDialog::reject);

    QByteArray wireBuffer;
    QString captured;
    bool requestDone = false;
    bool requestSucceeded = false;
    int requestExitCode = -1;

    connect(session, &QProcess::readyReadStandardOutput, &dialog,
            [session, requestId, &wireBuffer, &captured, &requestDone, &requestSucceeded,
             &requestExitCode, output, status, closeButton, &dialog] {
        wireBuffer += session->readAllStandardOutput();
        while (true) {
            const qsizetype newline = wireBuffer.indexOf('\n');
            if (newline < 0) {
                break;
            }
            QByteArray line = wireBuffer.left(newline);
            wireBuffer.remove(0, newline + 1);
            if (line.endsWith('\r')) {
                line.chop(1);
            }

            const qsizetype firstTab = line.indexOf('\t');
            const qsizetype secondTab = firstTab >= 0 ? line.indexOf('\t', firstTab + 1) : -1;
            if (firstTab < 0 || secondTab < 0) {
                continue;
            }
            const QByteArray tag = line.left(firstTab);
            const QByteArray wireId = line.mid(firstTab + 1, secondTab - firstTab - 1);
            if (wireId != requestId) {
                continue;
            }
            const QByteArray payload = line.mid(secondTab + 1);

            if (tag == QByteArrayLiteral("OUT")) {
                const QString text = QString::fromUtf8(payload);
                captured += text;
                captured += QLatin1Char('\n');
                output->moveCursor(QTextCursor::End);
                output->insertPlainText(text + QLatin1Char('\n'));
                output->moveCursor(QTextCursor::End);
                continue;
            }
            if (tag == QByteArrayLiteral("SESSION_ERROR")) {
                const QString text = QString::fromUtf8(payload);
                captured += QStringLiteral("ERROR: %1\n").arg(text);
                output->appendPlainText(QStringLiteral("ERROR: %1").arg(text));
                continue;
            }
            if (tag == QByteArrayLiteral("DONE")) {
                bool converted = false;
                requestExitCode = QString::fromLatin1(payload).toInt(&converted);
                if (!converted) {
                    requestExitCode = 2;
                }
                requestDone = true;
                requestSucceeded = requestExitCode == 0;
                status->setText(requestSucceeded
                    ? QStringLiteral("Privileged operation completed successfully. Administrator authorization remains active for this Boot Bitch window.")
                    : QStringLiteral("Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing."));
                closeButton->setEnabled(true);
                dialog.setCloseAllowed(true);
                continue;
            }
        }
    });

    connect(session, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), &dialog,
            [status, closeButton, &dialog, &requestDone](int, QProcess::ExitStatus) {
        if (!requestDone) {
            status->setText(QStringLiteral("The privileged helper session ended unexpectedly. The next root action will require authorization again."));
            closeButton->setEnabled(true);
            dialog.setCloseAllowed(true);
        }
    });

    QByteArray header = "BEGIN\t" + requestId + "\t" + QByteArray::number(arguments.size())
        + "\t" + (hasSecret ? QByteArrayLiteral("1") : QByteArrayLiteral("0")) + "\n";
    session->write(header);
    header.fill('\0');
    header.clear();

    for (const QString &argument : arguments) {
        QByteArray encoded = argument.toUtf8().toBase64();
        QByteArray record = "ARG\t" + requestId + "\t" + encoded + "\n";
        session->write(record);
        encoded.fill('\0');
        encoded.clear();
        record.fill('\0');
        record.clear();
    }

    if (hasSecret) {
        QByteArray encodedSecret = secret.toBase64();
        QByteArray secretRecord = "SECRET\t" + requestId + "\t" + encodedSecret + "\n";
        session->write(secretRecord);
        encodedSecret.fill('\0');
        encodedSecret.clear();
        secretRecord.fill('\0');
        secretRecord.clear();
    }
    secret.fill('\0');
    secret.clear();

    QByteArray endRecord = "END\t" + requestId + "\n";
    session->write(endRecord);
    endRecord.fill('\0');
    endRecord.clear();
    session->waitForBytesWritten(1000);

    if (showProgressDialog) {
        dialog.exec();
    } else {
        // Read-only diagnostics and inspections already have a persistent
        // results/details pane. Wait for the authorized helper request without
        // showing a redundant modal progress/output dialog; callers display the
        // captured output in-place when the request completes.
        QEventLoop waitLoop;
        QTimer pollTimer;
        pollTimer.setInterval(20);
        connect(&pollTimer, &QTimer::timeout, &waitLoop, [&] {
            if (requestDone || !session || session->state() == QProcess::NotRunning) {
                waitLoop.quit();
            }
        });
        pollTimer.start();
        if (!requestDone && session && session->state() != QProcess::NotRunning) {
            waitLoop.exec();
        }
        pollTimer.stop();
    }

    if (succeeded) {
        *succeeded = requestDone && requestSucceeded;
    }
    appendLog(QStringLiteral("%1 finished with exit code %2 (%3).")
                  .arg(title)
                  .arg(requestExitCode)
                  .arg(requestDone && requestSucceeded ? QStringLiteral("success") : QStringLiteral("error")),
              requestDone && requestSucceeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    return captured;
}

void MainWindow::unlockSelectedTarget()
{
    const QList<QTreeWidgetItem *> selected = m_deviceTree ? m_deviceTree->selectedItems() : QList<QTreeWidgetItem *>();
    if (selected.isEmpty()) {
        return;
    }

    QTreeWidgetItem *selectedItem = selected.first();
    while (selectedItem->parent()) {
        selectedItem = selectedItem->parent();
    }

    const QString diskPath = selectedItem->data(0, Qt::UserRole).toString();
    if (!m_deviceIndex.contains(diskPath)) {
        return;
    }

    const DeviceNode disk = m_deviceIndex.value(diskPath);
    const QString inspectedPath = m_deviceTree->selectedItems().first()->data(0, Qt::UserRole).toString();
    const DeviceNode inspected = m_deviceIndex.value(inspectedPath, disk);
    const DeviceNode *unlockCandidate = nullptr;
    if (inspected.encrypted && !encryptedNodeHasUnlockedLinuxChild(inspected)) {
        unlockCandidate = m_deviceIndex.contains(inspectedPath) ? &m_deviceIndex[inspectedPath] : nullptr;
    }
    if (!unlockCandidate) {
        unlockCandidate = firstLockedEncryptedNode(disk);
    }
    if (disk.protectedDevice || !unlockCandidate || unlockCandidate->path.isEmpty()) {
        const bool alreadyUnlocked = treeContainsUnlockedLinuxInsideEncrypted(disk);
        QMessageBox::information(this, QStringLiteral("Unlock not required"),
                                 alreadyUnlocked
                                     ? QStringLiteral("This drive already has an unlocked Linux filesystem. Boot Bitch will reuse the existing mapper.")
                                     : QStringLiteral("The selected drive does not currently contain a locked LUKS component that needs to be opened."));
        return;
    }
    const DeviceNode *preferred = unlockCandidate;

    const QString helper = repairHelperPath();
    if (helper.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Unlock unavailable"),
                             QStringLiteral("The privileged Boot Bitch helper was not found. Rebuild or install this source tree."));
        return;
    }

#ifdef Q_OS_UNIX
    if (geteuid() != 0 && QStandardPaths::findExecutable(QStringLiteral("pkexec")).isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Unlock unavailable"),
                             QStringLiteral("pkexec/Polkit is required to authorize LUKS unlock operations."));
        return;
    }
#endif

    QMessageBox confirm(this);
    confirm.setIcon(QMessageBox::Warning);
    confirm.setWindowTitle(QStringLiteral("Unlock encrypted repair target"));
    confirm.setText(QStringLiteral("Unlock %1?").arg(preferred->path));
    confirm.setInformativeText(QStringLiteral(
        "Target disk: %1\n\nBoot Bitch will ask Polkit for authorization and open a temporary device-mapper mapping. "
        "A mapping opened by Boot Bitch remains available for this authorized app session so later diagnostics and repairs can reuse it, then closes when you lock the administrator session or exit. "
        "A mapping that was already open before Boot Bitch attached to it is reused but never closed by Boot Bitch.")
        .arg(disk.path));
    confirm.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    confirm.setDefaultButton(QMessageBox::Cancel);
    confirm.button(QMessageBox::Yes)->setText(QStringLiteral("Unlock"));
    if (confirm.exec() != QMessageBox::Yes) {
        return;
    }

    QString authorizationError;
    if (!ensurePrivilegedSession(&authorizationError)) {
        QMessageBox::warning(this, QStringLiteral("Unlock unavailable"), authorizationError);
        return;
    }

    bool processSucceeded = false;
    while (!processSucceeded) {
        QDialog passDialog(this);
        passDialog.setObjectName(QStringLiteral("luksUnlockDialog"));
        passDialog.setWindowTitle(QStringLiteral("Unlock LUKS repair target"));
        passDialog.resize(560, 360);
        passDialog.setMaximumWidth(680);

        auto *passLayout = standardDialogLayout(&passDialog, 500);

        // Match the KDE authentication pattern: one prominent lock icon
        // anchors the complete content column, while the action buttons stay
        // in a conventional right-aligned button row below it.
        auto *passBody = new QHBoxLayout;
        passBody->setSpacing(16);
        auto *passIcon = new QLabel;
        passIcon->setPixmap(themedIcon(QStringLiteral("dialog-password"),
                                              themedIcon(QStringLiteral("document-encrypt"))).pixmap(56, 56));
        passIcon->setFixedSize(60, 60);
        passIcon->setAlignment(Qt::AlignCenter);
        passBody->addWidget(passIcon, 0, Qt::AlignTop);
        auto *passContent = new QVBoxLayout;
        passContent->setContentsMargins(0, 0, 0, 0);
        passContent->setSpacing(10);
        auto *passTitle = sectionTitle(QStringLiteral("Unlock encrypted repair target"));
        passTitle->setWordWrap(true);
        passContent->addWidget(passTitle);

        auto *targetFrame = new QFrame;
        targetFrame->setFrameShape(QFrame::StyledPanel);
        auto *targetLayout = new QHBoxLayout(targetFrame);
        targetLayout->setContentsMargins(10, 4, 10, 4);
        targetLayout->setSpacing(4);
        auto *targetCaption = subtleLabel(QStringLiteral("LUKS volume:"));
        targetLayout->addWidget(targetCaption, 0, Qt::AlignVCenter);
        auto *targetPath = new QLabel(preferred->path);
        targetPath->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        targetPath->setWordWrap(true);
        QFont targetFont = targetPath->font();
        targetFont.setBold(true);
        targetPath->setFont(targetFont);
        targetLayout->addWidget(targetPath, 1, Qt::AlignVCenter);
        passContent->addWidget(targetFrame);

        auto *passLabel = new QLabel(QStringLiteral("Enter the passphrase to unlock this volume."));
        passLabel->setWordWrap(true);
        passContent->addWidget(passLabel);

        auto *passEdit = new QLineEdit;
        passEdit->setEchoMode(QLineEdit::Password);
        passEdit->setClearButtonEnabled(true);
        passEdit->setPlaceholderText(QStringLiteral("LUKS passphrase"));
        passEdit->setAccessibleName(QStringLiteral("LUKS passphrase"));
        passEdit->setMinimumHeight(34);
        passContent->addWidget(passEdit);

        auto *passNote = new QLabel(QStringLiteral(
            "Administrator authorization is already active. The passphrase is sent only to cryptsetup over the privileged helper pipe and is never logged or placed on a command line."));
        passNote->setWordWrap(true);
        QFont noteFont = passNote->font();
        noteFont.setPointSizeF(noteFont.pointSizeF() * 0.92);
        passNote->setFont(noteFont);
        passContent->addWidget(passNote);
        passBody->addLayout(passContent, 1);
        passLayout->addLayout(passBody);

        auto *passButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        passButtons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Unlock"));
        passLayout->addWidget(passButtons);
        connect(passButtons, &QDialogButtonBox::accepted, &passDialog, &QDialog::accept);
        connect(passButtons, &QDialogButtonBox::rejected, &passDialog, &QDialog::reject);
        connect(passEdit, &QLineEdit::returnPressed, &passDialog, &QDialog::accept);
        QTimer::singleShot(0, passEdit, [passEdit] { passEdit->setFocus(Qt::OtherFocusReason); });
        passDialog.adjustSize();

        if (passDialog.exec() != QDialog::Accepted) {
            passEdit->clear();
            return;
        }

        QString passphrase = passEdit->text();
        passEdit->clear();
        if (passphrase.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Passphrase required"),
                                 QStringLiteral("An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel."));
            continue;
        }

        QByteArray secret = passphrase.toUtf8();
        passphrase.fill(QChar(0));
        passphrase.clear();

        appendLog(QStringLiteral("Starting privileged LUKS unlock for %1 on %2. Passphrase is not logged.")
                      .arg(preferred->path, disk.path));
        const QString unlockOutput = runPrivilegedRequest(QStringLiteral("Unlock LUKS target"),
                                                           {QStringLiteral("unlock"), disk.path, preferred->path},
                                                           secret,
                                                           &processSucceeded,
                                                           false);
        secret.fill('\0');
        secret.clear();

        // Keep the helper's safe, passphrase-free result visible in Systems.
        // This replaces the redundant modal output window while preserving
        // exact operation evidence for the selected drive.
        QString statusText = unlockOutput.trimmed();
        if (statusText.isEmpty()) {
            statusText = processSucceeded
                ? QStringLiteral("Unlock completed successfully for %1.").arg(preferred->path)
                : QStringLiteral("Unlock failed for %1.").arg(preferred->path);
        }
        m_unlockStatusCache.insert(disk.path, statusText);
        if (m_unlockStatusView) {
            m_unlockStatusView->setPlainText(statusText);
            m_unlockStatusView->moveCursor(QTextCursor::End);
        }

        if (!processSucceeded && unlockOutput.contains(QStringLiteral("UNLOCK_AUTH_FAILED=1"))) {
            appendLog(QStringLiteral("LUKS passphrase was not accepted for %1; offering retry without re-authorizing the administrator session.")
                          .arg(disk.path), QStringLiteral("WARNING"));
            QMessageBox retry(this);
            retry.setIcon(QMessageBox::Warning);
            retry.setWindowTitle(QStringLiteral("Passphrase not accepted"));
            retry.setText(QStringLiteral("The LUKS passphrase was not accepted."));
            retry.setInformativeText(QStringLiteral("Try again? Administrator authorization remains active, so only the disk passphrase will be requested again."));
            retry.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            retry.setDefaultButton(QMessageBox::Retry);
            if (retry.exec() == QMessageBox::Retry) {
                continue;
            }
            return;
        }

        if (!processSucceeded) {
            appendLog(QStringLiteral("LUKS unlock did not complete for %1.").arg(disk.path), QStringLiteral("ERROR"));
            return;
        }
    }

    appendLog(QStringLiteral("LUKS volume unlocked for %1; refreshing device topology.").arg(disk.path));
    refreshDevices();

    for (int i = 0; m_deviceTree && i < m_deviceTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_deviceTree->topLevelItem(i);
        if (item && item->data(0, Qt::UserRole).toString() == diskPath) {
            m_deviceTree->setCurrentItem(item);
            break;
        }
    }

    if (m_previewTargetPath == diskPath && m_deviceIndex.contains(diskPath)) {
        const DeviceNode refreshedDisk = m_deviceIndex.value(diskPath);
        const DeviceNode *refreshedPreferred = preferredRepairNode(refreshedDisk);
        m_previewTargetComponentPath = refreshedPreferred ? refreshedPreferred->path : QString();
        updateTargetLabels();
    }

    statusBar()->showMessage(QStringLiteral("Encrypted target unlocked — select or reselect the repair target"), 6000);
    scheduleSnapshotPreload();
}

void MainWindow::updateTargetLabels()
{
    QString text = QStringLiteral("Target: none selected");
    QString tooltip;

    if (m_hostMaintenanceMode) {
        text = QStringLiteral("Host maintenance: %1").arg(m_hostPrimaryPath.isEmpty()
            ? QStringLiteral("unresolved") : m_hostPrimaryPath);
        tooltip = QStringLiteral("Running host selected for guarded maintenance: %1\nRoot component: %2")
            .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath);
    } else if (!m_previewTargetPath.isEmpty()) {
        text = QStringLiteral("Target: %1").arg(m_previewTargetPath);
        tooltip = QStringLiteral("Selected physical repair drive: %1").arg(m_previewTargetPath);

        if (!m_previewTargetComponentPath.isEmpty() && m_previewTargetComponentPath != m_previewTargetPath) {
            tooltip += QStringLiteral("\nAutomatically detected system component: %1")
                .arg(m_previewTargetComponentPath);
        }
    }

    for (QLabel *label : {m_repairTargetLabel, m_snapshotTargetLabel, m_fileCopyTargetLabel, m_chrootShellTargetLabel}) {
        if (!label) {
            continue;
        }
        label->setText(text);
        label->setToolTip(tooltip);
    }

    if (m_systemTargetLabel) {
        QString committedSummary = m_previewTargetPath;
        if (!m_previewTargetPath.isEmpty() && m_deviceIndex.contains(m_previewTargetPath)) {
            const QString model = combinedModel(m_deviceIndex.value(m_previewTargetPath));
            if (!model.isEmpty()) {
                committedSummary = QStringLiteral("%1  •  %2").arg(model, m_previewTargetPath);
            }
        }
        m_systemTargetLabel->setText(m_hostMaintenanceMode
            ? QStringLiteral("Host maintenance: %1").arg(m_hostPrimaryPath)
            : (m_previewTargetPath.isEmpty()
                ? QStringLiteral("Committed target: none")
                : QStringLiteral("Committed target: %1").arg(committedSummary)));
        m_systemTargetLabel->setToolTip(m_hostMaintenanceMode
            ? QStringLiteral("Explicit native running-host maintenance is active. Ordinary target repairs, snapshots, file copy and chroot remain unavailable.")
            : (m_previewTargetPath.isEmpty()
                ? QStringLiteral("Row selection is inspection only. Press Select Target to commit a repair drive.")
                : QStringLiteral("Committed repair target. Repair, Diagnostics, Snapshots and File Copy target this physical drive until another drive is explicitly selected with Select Target.\n%1").arg(tooltip)));
    }

    updateCommittedTargetVisual();
    updateFileCopyDirection();
    updateSnapshotControls();
    if (m_chrootShellRunButton) {
        QString shellReason;
        m_chrootShellRunButton->setEnabled(repairTargetReady(&shellReason));
    }

    if (m_diagnosticScopeCombo && !m_previewTargetPath.isEmpty()) {
        m_diagnosticScopeCombo->setCurrentIndex(0);
    }
    updateFullRepairSummary();
    updateDiagnosticDetails();
}


void MainWindow::updateCommittedTargetVisual()
{
    if (!m_deviceTree) {
        return;
    }

    QColor targetColor = m_deviceTree->palette().color(QPalette::Highlight);
    targetColor.setAlpha(55);
    const QBrush targetBrush(targetColor);

    for (int row = 0; row < m_deviceTree->topLevelItemCount(); ++row) {
        QTreeWidgetItem *item = m_deviceTree->topLevelItem(row);
        if (!item) {
            continue;
        }

        const QString path = item->data(0, Qt::UserRole).toString();
        const QString originalStatus = item->data(1, Qt::UserRole).toString();
        const bool committed = !m_previewTargetPath.isEmpty() && path == m_previewTargetPath;

        item->setText(1, committed
            ? QStringLiteral("✓ SELECTED TARGET  •  %1").arg(originalStatus)
            : originalStatus);

        for (int column = 0; column < m_deviceTree->columnCount(); ++column) {
            item->setBackground(column, committed ? targetBrush : QBrush());
            QFont font = item->font(column);
            font.setBold(committed || column == 0);
            item->setFont(column, font);
        }

        if (committed) {
            item->setToolTip(1, QStringLiteral(
                "Committed repair target. Clicking another row changes only the inspection highlight; repair actions continue to target %1 until Select Target is pressed on another physical drive.\\n%2")
                .arg(path, originalStatus));
        } else {
            item->setToolTip(1, originalStatus);
        }
    }
}


void MainWindow::clearSnapshotResults()
{
    if (m_snapshotTable) {
        m_snapshotTable->setRowCount(0);
    }
    if (m_snapshotDetails) {
        m_snapshotDetails->clear();
    }
    m_snapshotResultIdentity = currentTargetDiagnosticCacheIdentity();
}

void MainWindow::updateSnapshotControls()
{
    if (!m_snapshotLoadButton || !m_snapshotInspectButton || !m_snapshotRollbackButton || !m_snapshotTable) {
        return;
    }

    const QString identity = currentTargetDiagnosticCacheIdentity();
    if (m_snapshotResultIdentity != identity) {
        clearSnapshotResults();
    }

    QString reason;
    bool ready = repairTargetReady(&reason);
    if (ready && m_deviceIndex.contains(m_previewTargetComponentPath)) {
        const DeviceNode component = m_deviceIndex.value(m_previewTargetComponentPath);
        if (component.fileSystem.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) != 0) {
            ready = false;
            reason = QStringLiteral("The selected Linux root is not Btrfs, so Btrfs snapshots are not available.");
        }
    }

    m_snapshotLoadButton->setEnabled(ready);
    m_snapshotLoadButton->setToolTip(ready
        ? QStringLiteral("Enumerate root snapshots through a temporary privileged read-only Btrfs mount.")
        : reason);

    const bool rowSelected = ready && !m_snapshotTable->selectedItems().isEmpty();
    m_snapshotInspectButton->setEnabled(rowSelected);
    m_snapshotInspectButton->setToolTip(rowSelected
        ? QStringLiteral("Inspect the selected snapshot read-only.")
        : (ready ? QStringLiteral("Select a snapshot row first.") : reason));

    bool rollbackCandidate = rowSelected;
    if (rollbackCandidate) {
        const int row = m_snapshotTable->currentRow();
        QTableWidgetItem *statusItem = row >= 0 ? m_snapshotTable->item(row, 4) : nullptr;
        rollbackCandidate = statusItem && statusItem->text().startsWith(QStringLiteral("Linux root snapshot"));
    }
    m_snapshotRollbackButton->setEnabled(rollbackCandidate);
    m_snapshotRollbackButton->setToolTip(rollbackCandidate
        ? QStringLiteral("Validate a rollback plan read-only, preserve the current @ root, promote a writable snapshot copy, reconcile initramfs/UKI/GRUB and auto-restore the old @ if validation fails.")
        : (ready ? QStringLiteral("Select a valid Linux root snapshot first.") : reason));
}

void MainWindow::scheduleSnapshotPreload()
{
    if (m_snapshotPreloadScheduled || !m_snapshotTable || m_previewTargetPath.isEmpty()) {
        return;
    }
    m_snapshotPreloadScheduled = true;
    if (m_snapshotDetails && m_snapshotTable->rowCount() == 0) {
        m_snapshotDetails->setPlainText(QStringLiteral("Loading Btrfs snapshots in the background…"));
    }
    QTimer::singleShot(0, this, [this] {
        m_snapshotPreloadScheduled = false;
        if (!m_snapshotTable || m_snapshotTable->rowCount() > 0) {
            return;
        }
        QString reason;
        if (repairTargetReady(&reason)) {
            loadSnapshots();
        }
    });
}

void MainWindow::loadSnapshots()
{
    QString reason;
    if (!repairTargetReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("Snapshot inventory unavailable"), reason);
        return;
    }

    bool succeeded = false;
    appendLog(QStringLiteral("Loading Btrfs snapshots read-only for %1 (%2).")
                  .arg(m_previewTargetPath, m_previewTargetComponentPath));
    const QString output = runPrivilegedRequest(
        QStringLiteral("Load Btrfs snapshots"),
        {QStringLiteral("snapshots"), m_previewTargetPath, m_previewTargetComponentPath, QStringLiteral("list")},
        QByteArray(),
        &succeeded,
        false);

    if (!succeeded) {
        if (m_snapshotDetails) {
            m_snapshotDetails->setPlainText(output.isEmpty()
                ? QStringLiteral("Snapshot inventory failed. See Logs for details.")
                : output);
        }
        appendLog(QStringLiteral("Btrfs snapshot inventory failed."), QStringLiteral("ERROR"));
        updateSnapshotControls();
        return;
    }

    m_snapshotTable->setRowCount(0);
    m_snapshotResultIdentity = currentTargetDiagnosticCacheIdentity();

    auto decode = [](const QString &encoded) {
        return QString::fromUtf8(QByteArray::fromBase64(encoded.toLatin1()));
    };

    struct SnapshotRow {
        QString id;
        QString created;
        QString type;
        QString description;
        QString status;
        QString relativePath;
        bool factory = false;
    };
    QList<SnapshotRow> rows;
    const QStringList lines = output.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (!line.startsWith(QStringLiteral("SNAPSHOT\t"))) {
            continue;
        }
        const QStringList fields = line.split(QLatin1Char('\t'), Qt::KeepEmptyParts);
        if (fields.size() < 7) {
            continue;
        }

        const QString id = fields.at(1);
        const QString created = decode(fields.at(2));
        const QString type = decode(fields.at(3));
        const QString description = decode(fields.at(4));
        const QString status = decode(fields.at(5));
        const QString relativePath = decode(fields.at(6));

        rows.append({id, created, type, description, status, relativePath,
                     id == QStringLiteral("1")
                         || type.compare(QStringLiteral("single"), Qt::CaseInsensitive) == 0
                         || description.compare(QStringLiteral("Factory Image"), Qt::CaseInsensitive) == 0});
    }

    // Keep the immutable factory image at the top, then present the newest
    // recovery points first. The helper's enumeration order is not a stable
    // UI contract, so sort explicitly here using the ISO timestamp text.
    std::stable_sort(rows.begin(), rows.end(), [](const SnapshotRow &left, const SnapshotRow &right) {
        if (left.factory != right.factory) {
            return left.factory;
        }
        return left.created > right.created;
    });

    int count = 0;
    for (const SnapshotRow &snapshot : rows) {
        const int row = m_snapshotTable->rowCount();
        m_snapshotTable->insertRow(row);

        auto *idItem = new QTableWidgetItem(snapshot.id);
        idItem->setData(Qt::UserRole, snapshot.id);
        idItem->setData(Qt::UserRole + 1, snapshot.relativePath);
        idItem->setToolTip(snapshot.relativePath);
        m_snapshotTable->setItem(row, 0, idItem);
        m_snapshotTable->setItem(row, 1, new QTableWidgetItem(snapshot.created));
        m_snapshotTable->setItem(row, 2, new QTableWidgetItem(snapshot.type));
        m_snapshotTable->setItem(row, 3, new QTableWidgetItem(snapshot.description));
        m_snapshotTable->setItem(row, 4, new QTableWidgetItem(snapshot.status));
        ++count;
    }

    // Compute compact one-line rows after all columns have been populated.
    // This also deliberately shrinks rows that were measured while the tab
    // was hidden at a narrow width.
    resizeSnapshotRows();
    if (m_snapshotDetails) {
        m_snapshotDetails->setPlainText(count > 0
            ? QStringLiteral("Loaded %1 Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.\n\nNo snapshot or target file was modified.")
                  .arg(count)
                  .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")))
            : QStringLiteral("No Snapper-style Btrfs root snapshots were found on the selected target. The scan was read-only."));
    }
    appendLog(QStringLiteral("Loaded %1 Btrfs snapshot(s) read-only for the selected repair target.").arg(count));
    updateSnapshotControls();
}

void MainWindow::resizeSnapshotRows()
{
    if (!m_snapshotTable) {
        return;
    }

    const QFontMetrics metrics(m_snapshotTable->font());
    // Keep rows compact across Qt styles and font metrics.  Qt 6.4 on the
    // Ubuntu CI runner uses a smaller line spacing than newer desktop builds;
    // a fixed 28px floor would turn a single-line row into an unnecessary
    // wrapped-looking gap and make the table less dense.
    const int oneLineHeight = qMax(24, metrics.lineSpacing() + 10);
    m_snapshotTable->verticalHeader()->setDefaultSectionSize(oneLineHeight);

    for (int row = 0; row < m_snapshotTable->rowCount(); ++row) {
        m_snapshotTable->setRowHeight(row, oneLineHeight);
    }
}

void MainWindow::inspectSelectedSnapshot()
{
    if (!m_snapshotTable || !m_snapshotDetails) {
        return;
    }
    const int row = m_snapshotTable->currentRow();
    if (row < 0 || !m_snapshotTable->item(row, 0)) {
        return;
    }

    const QString snapshotId = m_snapshotTable->item(row, 0)->data(Qt::UserRole).toString();
    if (snapshotId.isEmpty()) {
        return;
    }

    bool succeeded = false;
    appendLog(QStringLiteral("Inspecting Btrfs snapshot %1 read-only.").arg(snapshotId));
    const QString output = runPrivilegedRequest(
        QStringLiteral("Inspect Btrfs snapshot %1").arg(snapshotId),
        {QStringLiteral("snapshots"), m_previewTargetPath, m_previewTargetComponentPath,
         QStringLiteral("inspect"), snapshotId},
        QByteArray(),
        &succeeded,
        false);

    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Captured: %1\n\n%2").arg(stamp, output));
    appendLog(QStringLiteral("Snapshot %1 read-only inspection %2.")
                  .arg(snapshotId, succeeded ? QStringLiteral("completed") : QStringLiteral("failed")),
              succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
}

void MainWindow::rollbackSelectedSnapshot()
{
    if (!m_snapshotTable || !m_snapshotDetails) {
        return;
    }
    const int row = m_snapshotTable->currentRow();
    if (row < 0 || !m_snapshotTable->item(row, 0)) {
        return;
    }

    const QString snapshotId = m_snapshotTable->item(row, 0)->data(Qt::UserRole).toString();
    if (snapshotId.isEmpty()) {
        return;
    }

    bool planSucceeded = false;
    appendLog(QStringLiteral("Preparing read-only transactional rollback plan for Btrfs snapshot %1.").arg(snapshotId));
    const QString plan = runPrivilegedRequest(
        QStringLiteral("Preflight snapshot rollback %1").arg(snapshotId),
        {QStringLiteral("snapshots"), m_previewTargetPath, m_previewTargetComponentPath,
         QStringLiteral("plan"), snapshotId},
        QByteArray(),
        &planSucceeded);

    const QString captured = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Rollback preflight captured: %1\n\n%2").arg(captured, plan));
    if (!planSucceeded || !plan.contains(QStringLiteral("PLAN_OK=1"))) {
        appendLog(QStringLiteral("Snapshot %1 rollback preflight failed; no target data was changed.").arg(snapshotId),
                  QStringLiteral("ERROR"));
        QMessageBox::warning(this, QStringLiteral("Rollback preflight failed"),
                             QStringLiteral("The selected snapshot did not pass the transactional rollback preflight. No rollback was performed.\n\nReview the Snapshots details pane and Logs."));
        return;
    }

    QMessageBox warning(this);
    warning.setIcon(QMessageBox::Warning);
    warning.setWindowTitle(QStringLiteral("Confirm transactional snapshot rollback"));
    warning.setText(QStringLiteral("Promote snapshot %1 to the normal writable @ root?").arg(snapshotId));
    warning.setInformativeText(QStringLiteral(
        "Target disk: %1\nLinux filesystem: %2\n\n"
        "Boot Bitch will keep the source snapshot unchanged, preserve the current @ under a timestamped rollback backup, set the promoted copy as the Btrfs default, then rebuild/verify initramfs, TUXEDO UKI when present, and GRUB.\n\n"
        "If a critical post-switch validation or boot-stack stage fails, Boot Bitch will automatically restore the preserved @ and reconcile its boot stack.\n\n"
        "Separate Btrfs subvolumes such as /home remain outside the root rollback according to the target fstab."
    ).arg(m_previewTargetPath, m_previewTargetComponentPath));
    warning.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    warning.setDefaultButton(QMessageBox::Cancel);
    warning.button(QMessageBox::Yes)->setText(QStringLiteral("Continue to Confirmation"));
    if (warning.exec() != QMessageBox::Yes) {
        appendLog(QStringLiteral("Snapshot rollback cancelled after preflight; no target data was changed."));
        return;
    }

    bool ok = false;
    const QString typed = QInputDialog::getText(
        this,
        QStringLiteral("Type ROLLBACK to continue"),
        QStringLiteral("This operation changes the active Btrfs root and rebuilds boot artifacts.\n\nType ROLLBACK exactly to continue:"),
        QLineEdit::Normal,
        QString(),
        &ok).trimmed();
    if (!ok || typed != QStringLiteral("ROLLBACK")) {
        appendLog(QStringLiteral("Snapshot rollback cancelled because the confirmation text did not match ROLLBACK."));
        return;
    }

    bool succeeded = false;
    appendLog(QStringLiteral("Starting transactional Btrfs rollback to snapshot %1 on committed target %2.")
                  .arg(snapshotId, m_previewTargetPath),
              QStringLiteral("WARNING"));
    const QString output = runPrivilegedRequest(
        QStringLiteral("Roll back to Btrfs snapshot %1").arg(snapshotId),
        {QStringLiteral("snapshots"), m_previewTargetPath, m_previewTargetComponentPath,
         QStringLiteral("rollback"), snapshotId},
        QByteArray(),
        &succeeded);

    const QString finished = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Rollback finished: %1\n\n%2").arg(finished, output));
    if (succeeded && output.contains(QStringLiteral("ROLLBACK_RESULT=SUCCESS"))) {
        clearTargetDiagnosticCache();
        m_targetDiagnosticsNeedRegeneration = true;
        updateFullRepairSummary();
        if (m_snapshotTable) {
            m_snapshotTable->setRowCount(0);
        }
        m_snapshotResultIdentity = currentTargetDiagnosticCacheIdentity();
        m_snapshotDetails->setPlainText(QStringLiteral("Rollback completed: %1\n\n%2\n\nSnapshot inventory was cleared because the active root changed. Choose Load Snapshots to refresh it.")
                                            .arg(finished, output));
        appendLog(QStringLiteral("STALE DIAGNOSTICS: snapshot %1 rollback changed the active root; snapshot inventory and cached diagnostics were invalidated. Regenerate diagnostics before the next repair.")
                      .arg(snapshotId));
        refreshDevices();
        updateSnapshotControls();
        QMessageBox::information(this, QStringLiteral("Snapshot rollback complete"),
                                 QStringLiteral("Snapshot %1 was promoted to a writable @ root and the boot stack passed reconciliation.\n\nThe previous @ was retained under a timestamped @rollback-before-* name. Reboot using the normal boot path when ready.")
                                     .arg(snapshotId));
    } else {
        clearTargetDiagnosticCache();
        m_targetDiagnosticsNeedRegeneration = true;
        updateFullRepairSummary();
        if (m_snapshotTable) {
            m_snapshotTable->setRowCount(0);
        }
        m_snapshotResultIdentity = currentTargetDiagnosticCacheIdentity();
        m_snapshotDetails->setPlainText(QStringLiteral("Rollback attempt finished: %1\n\n%2\n\nCached diagnostics and snapshot inventory were cleared because the rollback request may have changed and/or restored target state.")
                                            .arg(finished, output));
        appendLog(QStringLiteral("STALE DIAGNOSTICS: snapshot %1 rollback did not complete; cached diagnostics and snapshots were invalidated. Regenerate diagnostics and review the operation output before the next repair or reboot.")
                      .arg(snapshotId),
                  QStringLiteral("ERROR"));
        refreshDevices();
        updateSnapshotControls();
        QMessageBox::critical(this, QStringLiteral("Snapshot rollback failed"),
                              QStringLiteral("The rollback did not complete successfully. Do not reboot until you review the Snapshots output and Logs. The helper attempts to restore the preserved @ automatically when post-switch validation fails."));
    }
}

void MainWindow::updateFileCopyDirection()
{
    if (!m_fileCopyDirectionCombo || !m_fileCopySourceBox || !m_fileCopyDestinationBox
        || !m_fileCopyAddFilesButton || !m_fileCopyAddFolderButton
        || !m_fileCopyBrowseDestinationButton || !m_destinationEdit) {
        return;
    }

    const bool repairToHost = m_fileCopyDirectionCombo->currentIndex() == 1;
    if (m_fileCopyHeading) {
        m_fileCopyHeading->setText(repairToHost
            ? QStringLiteral("Repair → Host file copy")
            : QStringLiteral("Host → Repair file copy"));
    }

    if (repairToHost) {
        m_fileCopySourceBox->setTitle(QStringLiteral("1. Select source paths from repaired system"));
        m_fileCopyDestinationBox->setTitle(QStringLiteral("2. Choose destination on this host"));
        m_fileCopyAddFilesButton->setText(QStringLiteral("Add File Path…"));
        m_fileCopyAddFolderButton->setText(QStringLiteral("Add Folder Path…"));
        m_fileCopyAddFilesButton->setToolTip(QStringLiteral("Enter an absolute file path as it appears inside the repaired system, for example /home/user/document.txt."));
        m_fileCopyAddFolderButton->setToolTip(QStringLiteral("Enter an absolute folder path as it appears inside the repaired system."));
        m_destinationEdit->setPlaceholderText(QStringLiteral("Choose a host destination folder"));
        m_fileCopyBrowseDestinationButton->setText(QStringLiteral("Browse…"));
        m_fileCopyBrowseDestinationButton->setEnabled(true);
    } else {
        m_fileCopySourceBox->setTitle(QStringLiteral("1. Select source files or folders from this host"));
        m_fileCopyDestinationBox->setTitle(QStringLiteral("2. Choose destination in repaired system"));
        m_fileCopyAddFilesButton->setText(QStringLiteral("Add Files…"));
        m_fileCopyAddFolderButton->setText(QStringLiteral("Add Folder…"));
        m_fileCopyAddFilesButton->setToolTip(QStringLiteral("Choose one or more source files from the running host."));
        m_fileCopyAddFolderButton->setToolTip(QStringLiteral("Choose a source folder from the running host."));
        m_destinationEdit->setPlaceholderText(m_previewTargetPath.isEmpty()
            ? QStringLiteral("Select a repair target first")
            : QStringLiteral("Choose an absolute destination path inside the repaired system"));
        m_fileCopyBrowseDestinationButton->setText(QStringLiteral("Browse Target Folders…"));
        m_fileCopyBrowseDestinationButton->setToolTip(QStringLiteral(
            "Browse the selected repair system through temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing."));
        m_fileCopyBrowseDestinationButton->setEnabled(!m_previewTargetPath.isEmpty());
    }

    updateFileCopyControls();
}

void MainWindow::updateFileCopyControls()
{
    if (!m_fileCopyPreviewButton || !m_fileCopyRunButton) {
        return;
    }

    QString reason;
    const bool ready = fileCopyReady(&reason);
    m_fileCopyPreviewButton->setEnabled(ready);
    m_fileCopyRunButton->setEnabled(ready);
    if (!ready) {
        m_fileCopyPreviewButton->setToolTip(reason);
        m_fileCopyRunButton->setToolTip(reason);
    } else {
        m_fileCopyPreviewButton->setToolTip(QStringLiteral("Run a guarded rsync dry-run. The selected repair filesystem remains read-only."));
        m_fileCopyRunButton->setToolTip(QStringLiteral("Copy staged items and verify them. Existing same-name destination content can be overwritten; unrelated destination files are never deleted."));
    }
}

void MainWindow::browseFileCopyDestination()
{
    if (!m_fileCopyDirectionCombo || !m_destinationEdit) {
        return;
    }

    const bool repairToHost = m_fileCopyDirectionCombo->currentIndex() == 1;
    if (repairToHost) {
        const QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("Choose host destination folder"), QDir::homePath());
        if (!path.isEmpty()) {
            m_destinationEdit->setText(path);
            m_destinationEdit->setToolTip(path);
            appendLog(QStringLiteral("Staged Repair → Host destination: %1. No copy occurred.").arg(path));
            updateFileCopyControls();
        }
        return;
    }

    if (m_previewTargetPath.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Select a repair target"),
                                 QStringLiteral("Select the repair drive in Systems before choosing a destination inside it."));
        return;
    }

    // The target is intentionally not left mounted by the GUI.  The folder
    // browser below asks the guarded helper for one read-only directory view
    // at a time, so the dialog shows the repaired system's actual folders
    // without exposing the host filesystem or keeping a target mount open.
    QDialog dialog(this);
    dialog.setObjectName(QStringLiteral("repairDestinationDialog"));
    dialog.setWindowTitle(QStringLiteral("Select repair-system destination"));
    auto *dialogLayout = standardDialogLayout(&dialog, 680);

    auto *headingRow = new QHBoxLayout;
    auto *headingIcon = new QLabel;
    headingIcon->setPixmap(themedIcon(QStringLiteral("folder-open")).pixmap(32, 32));
    headingRow->addWidget(headingIcon, 0, Qt::AlignTop);
    auto *heading = new QLabel(QStringLiteral("Choose a destination folder inside the repaired system"));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    headingFont.setPointSize(headingFont.pointSize() + 2);
    heading->setFont(headingFont);
    headingRow->addWidget(heading, 1, Qt::AlignVCenter);
    dialogLayout->addLayout(headingRow);

    auto *description = new QLabel(QStringLiteral(
        "Browse folders from the selected repaired system through temporary read-only mounts. Nothing is written while browsing, and the guarded copy validates the chosen path again before any write."));
    description->setWordWrap(true);
    description->setTextFormat(Qt::PlainText);
    dialogLayout->addWidget(description);

    auto *pathRow = new QHBoxLayout;
    auto *pathEdit = new QLineEdit(m_destinationEdit->text().isEmpty() ? QStringLiteral("/home") : m_destinationEdit->text());
    pathEdit->setClearButtonEnabled(true);
    pathEdit->setPlaceholderText(QStringLiteral("Absolute path inside repaired system (for example /home/user/Recovered)"));
    pathRow->addWidget(pathEdit, 1);
    auto *selectFolder = new QPushButton(themedIcon(QStringLiteral("folder-open")), QStringLiteral("Browse repair folders…"));
    selectFolder->setToolTip(QStringLiteral("Open a read-only view of the selected repair system's folders. The target is unmounted again after each directory listing."));
    pathRow->addWidget(selectFolder);
    auto *clearPath = new QPushButton(themedIcon(QStringLiteral("edit-clear")), QStringLiteral("Clear"));
    clearPath->setToolTip(QStringLiteral("Clear the destination path."));
    pathRow->addWidget(clearPath);
    dialogLayout->addLayout(pathRow);

    connect(clearPath, &QPushButton::clicked, pathEdit, &QLineEdit::clear);
    connect(selectFolder, &QPushButton::clicked, &dialog, [this, pathEdit, &dialog] {
        QString initialPath = QDir::cleanPath(pathEdit->text().trimmed());
        if (!initialPath.startsWith(QLatin1Char('/'))
            || initialPath == QStringLiteral("/..")
            || initialPath.startsWith(QStringLiteral("/../"))) {
            initialPath = QStringLiteral("/home");
        }

        QDialog browser(&dialog);
        browser.setObjectName(QStringLiteral("repairFolderBrowser"));
        browser.setWindowTitle(QStringLiteral("Browse repaired-system folders"));
        auto *browserLayout = standardDialogLayout(&browser, 620);
        // The folder browser is intentionally compact on first open but may
        // be enlarged freely when a long path or a large directory needs
        // more room.  Keep a usable minimum so the action row never clips.
        browser.setSizeGripEnabled(true);
        browser.setMinimumSize(560, 360);
        browser.resize(700, 500);

        auto *browserHeadingRow = new QHBoxLayout;
        auto *browserHeadingIcon = new QLabel;
        browserHeadingIcon->setPixmap(themedIcon(QStringLiteral("folder-open")).pixmap(32, 32));
        browserHeadingRow->addWidget(browserHeadingIcon, 0, Qt::AlignTop);
        auto *browserHeading = new QLabel(QStringLiteral("Choose a folder from the repaired system"));
        QFont browserHeadingFont = browserHeading->font();
        browserHeadingFont.setBold(true);
        browserHeadingFont.setPointSize(browserHeadingFont.pointSize() + 2);
        browserHeading->setFont(browserHeadingFont);
        browserHeadingRow->addWidget(browserHeading, 1, Qt::AlignVCenter);
        browserLayout->addLayout(browserHeadingRow);

        auto *browserDescription = new QLabel(QStringLiteral(
            "The selected repair filesystem is mounted read-only only for the current directory listing. The mount is removed after the request; the host filesystem is never used as the folder tree."));
        browserDescription->setWordWrap(true);
        browserDescription->setTextFormat(Qt::PlainText);
        browserLayout->addWidget(browserDescription);

        auto *currentPathLabel = new QLabel;
        currentPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        browserLayout->addWidget(currentPathLabel);

        auto *folderList = new QListWidget;
        folderList->setAlternatingRowColors(true);
        folderList->setSelectionMode(QAbstractItemView::SingleSelection);
        folderList->setMinimumHeight(260);
        browserLayout->addWidget(folderList, 1);

        auto *navigation = new QHBoxLayout;
        auto *upButton = new QPushButton(themedIcon(QStringLiteral("go-up")), QStringLiteral("Up"));
        auto *openButton = new QPushButton(themedIcon(QStringLiteral("document-open")), QStringLiteral("Open Selected"));
        auto *refreshButton = new QPushButton(themedIcon(QStringLiteral("view-refresh")), QStringLiteral("Refresh"));
        upButton->setToolTip(QStringLiteral("Show the parent folder in the repaired system."));
        openButton->setToolTip(QStringLiteral("Open the selected repaired-system folder."));
        refreshButton->setToolTip(QStringLiteral("Reload this repaired-system folder through a fresh read-only mount."));
        navigation->addWidget(upButton);
        navigation->addWidget(openButton);
        navigation->addWidget(refreshButton);
        navigation->addStretch(1);
        browserLayout->addLayout(navigation);

        auto *browserStatus = new QLabel(QStringLiteral("Read-only target view; no files are modified."));
        browserStatus->setWordWrap(true);
        browserLayout->addWidget(browserStatus);

        auto *browserButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QPushButton *chooseButton = browserButtons->button(QDialogButtonBox::Ok);
        chooseButton->setText(QStringLiteral("Choose This Folder"));
        browserLayout->addWidget(browserButtons);
        connect(browserButtons, &QDialogButtonBox::accepted, &browser, &QDialog::accept);
        connect(browserButtons, &QDialogButtonBox::rejected, &browser, &QDialog::reject);

        QString currentPath;
        bool pathLoaded = false;
        const auto loadPath = [this, &browser, &currentPath, &pathLoaded, currentPathLabel,
                               folderList, upButton, chooseButton, browserStatus](const QString &requestedPath) {
            const QString candidate = QDir::cleanPath(requestedPath);
            if (!candidate.startsWith(QLatin1Char('/'))
                || candidate == QStringLiteral("/..")
                || candidate.startsWith(QStringLiteral("/../"))) {
                browserStatus->setText(QStringLiteral("Invalid repair-system path."));
                pathLoaded = false;
                chooseButton->setEnabled(false);
                return false;
            }

            bool succeeded = false;
            const QString output = runPrivilegedRequest(
                QStringLiteral("Browse repaired-system folders"),
                {QStringLiteral("browse-target"), m_previewTargetPath, m_previewTargetComponentPath, candidate},
                QByteArray(), &succeeded, false);
            if (!succeeded) {
                browserStatus->setText(QStringLiteral("Unable to read this repaired-system folder. Review Logs for the helper error."));
                pathLoaded = false;
                chooseButton->setEnabled(false);
                return false;
            }

            QStringList childPaths;
            const QString marker = QStringLiteral("BROWSE_ENTRY\t");
            for (const QString &line : output.split(QLatin1Char('\n'))) {
                if (!line.startsWith(marker)) {
                    continue;
                }
                const QByteArray decoded = QByteArray::fromBase64(line.mid(marker.size()).trimmed().toLatin1());
                const QString name = QString::fromUtf8(decoded);
                if (name.isEmpty() || name == QStringLiteral(".") || name == QStringLiteral("..")
                    || name.contains(QLatin1Char('/')) || name.contains(QChar::Null)) {
                    continue;
                }
                childPaths.append(candidate == QStringLiteral("/")
                    ? QStringLiteral("/") + name
                    : candidate + QStringLiteral("/") + name);
            }
            childPaths.sort(Qt::CaseInsensitive);
            folderList->clear();
            for (const QString &childPath : childPaths) {
                auto *item = new QListWidgetItem(QFileInfo(childPath).fileName(), folderList);
                item->setData(Qt::UserRole, childPath);
                item->setToolTip(childPath);
            }
            currentPath = candidate;
            currentPathLabel->setText(QStringLiteral("Current repaired-system folder: %1").arg(currentPath));
            upButton->setEnabled(currentPath != QStringLiteral("/"));
            pathLoaded = true;
            chooseButton->setEnabled(true);
            browserStatus->setText(QStringLiteral("Read-only target view; %1 subfolder(s) found. Nothing was modified.").arg(childPaths.size()));
            return true;
        };

        connect(upButton, &QPushButton::clicked, &browser, [&loadPath, &currentPath] {
            if (currentPath == QStringLiteral("/")) {
                return;
            }
            loadPath(QFileInfo(currentPath).path());
        });
        connect(refreshButton, &QPushButton::clicked, &browser, [&loadPath, &currentPath] {
            loadPath(currentPath);
        });
        connect(openButton, &QPushButton::clicked, &browser, [&loadPath, folderList] {
            QListWidgetItem *item = folderList->currentItem();
            if (item) {
                loadPath(item->data(Qt::UserRole).toString());
            }
        });
        connect(folderList, &QListWidget::itemDoubleClicked, &browser,
                [&loadPath](QListWidgetItem *item) {
            if (item) {
                loadPath(item->data(Qt::UserRole).toString());
            }
        });

        pathLoaded = loadPath(initialPath);
        if (!pathLoaded && initialPath != QStringLiteral("/")) {
            // A destination may be new and therefore absent from the target;
            // show its nearest useful existing namespace instead of falling
            // back to a host QFileDialog.
            pathLoaded = loadPath(QStringLiteral("/"));
        }
        if (browser.exec() == QDialog::Accepted && pathLoaded) {
            pathEdit->setText(currentPath);
        }
    });

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    dialogLayout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    pathEdit->setFocus();
    pathEdit->selectAll();

    QString cleaned;
    while (dialog.exec() == QDialog::Accepted) {
        const QString path = pathEdit->text().trimmed();
        cleaned = QDir::cleanPath(path);
        if (cleaned.startsWith(QLatin1Char('/')) && cleaned != QStringLiteral("/")
            && cleaned != QStringLiteral("/..") && !cleaned.startsWith(QStringLiteral("/../"))) {
            break;
        }
        QMessageBox::warning(&dialog, QStringLiteral("Invalid repair path"),
                             QStringLiteral("Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted."));
    }
    if (cleaned.isEmpty()) {
        return;
    }

    m_destinationEdit->setText(cleaned);
    m_destinationEdit->setToolTip(cleaned);
    appendLog(QStringLiteral("Staged Host → Repair destination: %1. No copy occurred.").arg(cleaned));
    updateFileCopyControls();
}

void MainWindow::addSourceFiles()
{
    const bool repairToHost = m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentIndex() == 1;
    if (repairToHost) {
        bool accepted = false;
        const QString path = QInputDialog::getText(
            this,
            QStringLiteral("Repair-system source file"),
            QStringLiteral("Enter an absolute file path inside the repaired system:"),
            QLineEdit::Normal,
            QStringLiteral("/home/"),
            &accepted).trimmed();
        if (!accepted || path.isEmpty()) {
            return;
        }
        const QString cleaned = QDir::cleanPath(path);
        if (!cleaned.startsWith(QLatin1Char('/')) || cleaned == QStringLiteral("/..") || cleaned.startsWith(QStringLiteral("/../"))) {
            QMessageBox::warning(this, QStringLiteral("Invalid repair path"),
                                 QStringLiteral("Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted."));
            return;
        }
        if (m_sourceList->findItems(cleaned, Qt::MatchExactly).isEmpty()) {
            auto *item = new QListWidgetItem(cleaned, m_sourceList);
            item->setToolTip(cleaned);
        }
        appendLog(QStringLiteral("Added repaired-system file path to the Repair → Host staging list: %1. No copy occurred.").arg(cleaned));
        updateFileCopyControls();
        return;
    }

    const QStringList paths = QFileDialog::getOpenFileNames(this, QStringLiteral("Select host files"), QDir::homePath());
    for (const QString &path : paths) {
        if (m_sourceList->findItems(path, Qt::MatchExactly).isEmpty()) {
            auto *item = new QListWidgetItem(path, m_sourceList);
            item->setToolTip(path);
        }
    }
    if (!paths.isEmpty()) {
        appendLog(QStringLiteral("Added %1 host file(s) to the Host → Repair staging list. No copy occurred.").arg(paths.size()));
        updateFileCopyControls();
    }
}

void MainWindow::addSourceFolder()
{
    const bool repairToHost = m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentIndex() == 1;
    if (repairToHost) {
        bool accepted = false;
        const QString path = QInputDialog::getText(
            this,
            QStringLiteral("Repair-system source folder"),
            QStringLiteral("Enter an absolute folder path inside the repaired system:"),
            QLineEdit::Normal,
            QStringLiteral("/home/"),
            &accepted).trimmed();
        if (!accepted || path.isEmpty()) {
            return;
        }
        const QString cleaned = QDir::cleanPath(path);
        if (!cleaned.startsWith(QLatin1Char('/')) || cleaned == QStringLiteral("/..") || cleaned.startsWith(QStringLiteral("/../"))) {
            QMessageBox::warning(this, QStringLiteral("Invalid repair path"),
                                 QStringLiteral("Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted."));
            return;
        }
        if (m_sourceList->findItems(cleaned, Qt::MatchExactly).isEmpty()) {
            auto *item = new QListWidgetItem(cleaned, m_sourceList);
            item->setToolTip(cleaned);
        }
        appendLog(QStringLiteral("Added repaired-system folder path to the Repair → Host staging list: %1. No copy occurred.").arg(cleaned));
        updateFileCopyControls();
        return;
    }

    const QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("Select host folder"), QDir::homePath());
    if (!path.isEmpty() && m_sourceList->findItems(path, Qt::MatchExactly).isEmpty()) {
        auto *item = new QListWidgetItem(path, m_sourceList);
        item->setToolTip(path);
        appendLog(QStringLiteral("Added host folder to the Host → Repair staging list: %1. No copy occurred.").arg(path));
        updateFileCopyControls();
    }
}

void MainWindow::removeSelectedSources()
{
    const QList<QListWidgetItem *> selected = m_sourceList->selectedItems();
    for (QListWidgetItem *item : selected) {
        delete item;
    }
    updateFileCopyControls();
}

void MainWindow::clearSourceList()
{
    m_sourceList->clear();
    updateFileCopyControls();
}

bool MainWindow::fileCopyReady(QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    QString targetReason;
    if (!repairTargetReady(&targetReason)) {
        setReason(targetReason);
        return false;
    }
    if (!m_sourceList || m_sourceList->count() == 0) {
        setReason(QStringLiteral("Add at least one source file or folder."));
        return false;
    }
    if (!m_destinationEdit || m_destinationEdit->text().trimmed().isEmpty()) {
        setReason(QStringLiteral("Choose a destination folder."));
        return false;
    }
    if (QStandardPaths::findExecutable(QStringLiteral("rsync")).isEmpty()) {
        setReason(QStringLiteral("rsync is required for verified File Copy."));
        return false;
    }

    const bool repairToHost = m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentIndex() == 1;
    const QString destination = m_destinationEdit->text().trimmed();
    if (repairToHost) {
        const QFileInfo info(destination);
        if (!info.exists() || !info.isDir() || info.isSymLink()) {
            setReason(QStringLiteral("Repair → Host requires an existing, non-symlink host destination folder."));
            return false;
        }
    } else {
        if (!destination.startsWith(QLatin1Char('/')) || destination == QStringLiteral("/")) {
            setReason(QStringLiteral("Host → Repair requires a specific absolute path inside the repaired system."));
            return false;
        }
    }

    setReason(QStringLiteral("Ready"));
    return true;
}

void MainWindow::clearChrootShellOutput()
{
    if (m_chrootShellOutput) {
        m_chrootShellOutput->clear();
    }
}

void MainWindow::runChrootShellCommand()
{
    if (!m_chrootShellCommandEdit || !m_chrootShellOutput) {
        return;
    }
    const QString command = m_chrootShellCommandEdit->text().trimmed();
    if (command.isEmpty()) {
        return;
    }
    QString reason;
    if (!repairTargetReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("Chroot shell unavailable"), reason);
        return;
    }
    m_chrootShellOutput->appendPlainText(QStringLiteral("$ %1").arg(command));
    m_chrootShellOutput->appendPlainText(QStringLiteral("[running…]"));
    bool succeeded = false;
    const QString output = runPrivilegedRequest(
        QStringLiteral("Chroot shell"),
        {QStringLiteral("shell"), m_previewTargetPath, m_previewTargetComponentPath, command},
        QByteArray(), &succeeded, false);
    if (!output.isEmpty()) {
        m_chrootShellOutput->appendPlainText(output.trimmed());
    }
    m_chrootShellOutput->appendPlainText(succeeded ? QStringLiteral("[exit 0]") : QStringLiteral("[command failed]"));
    const QString shellDetail = output.trimmed().isEmpty()
        ? QStringLiteral("(no command output)")
        : output.trimmed();
    appendLog(QStringLiteral("Chroot shell output\nDiagnostic: %1\n%2")
                  .arg(command, shellDetail),
              succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    appendLog(QStringLiteral("Chroot shell command %1: %2")
                  .arg(succeeded ? QStringLiteral("completed") : QStringLiteral("failed"), command),
              succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    // Most shell commands are arbitrary and therefore conservatively stale
    // all target evidence. An apt update that only reports failed fetches did
    // not receive repository data; keep the prior evidence in that one
    // explicitly recognized no-change case.
    const QString foldedOutput = output.toCaseFolded();
    const QString foldedCommand = command.toCaseFolded();
    const bool aptRefreshNoChange = QRegularExpression(QStringLiteral("^(sudo\\s+)?apt(-get)?\\s+update$")).match(foldedCommand).hasMatch()
        && (foldedOutput.contains(QStringLiteral("some index files failed"))
            || foldedOutput.contains(QStringLiteral("failed to fetch"))
            || foldedOutput.contains(QStringLiteral("temporary failure resolving")));
    if (aptRefreshNoChange) {
        appendLog(QStringLiteral("Chroot shell apt update received no repository data; cached diagnostics remain usable."),
                  QStringLiteral("WARNING"));
    } else {
        m_targetDiagnosticsNeedRegeneration = true;
        clearTargetDiagnosticCache();
        updateFullRepairSummary();
        appendLog(QStringLiteral("STALE DIAGNOSTICS: chroot shell command may have changed target files. Regenerate diagnostics before the next repair."),
                  QStringLiteral("WARNING"));
    }
    m_chrootShellCommandEdit->clear();
    m_chrootShellOutput->moveCursor(QTextCursor::End);
}

void MainWindow::runFileCopyPreview()
{
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("File Copy unavailable"), reason);
        return;
    }

    const bool repairToHost = m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentIndex() == 1;
    const QString direction = repairToHost ? QStringLiteral("repair-to-host") : QStringLiteral("host-to-repair");
    const QString ownership = m_ownershipCombo && m_ownershipCombo->currentIndex() == 1
        ? QStringLiteral("preserve") : QStringLiteral("smart");

    QStringList arguments = {QStringLiteral("copy-preview"), m_previewTargetPath, m_previewTargetComponentPath,
                             direction, ownership, QStringLiteral("normal"), m_destinationEdit->text().trimmed()};
    for (int row = 0; row < m_sourceList->count(); ++row) {
        arguments << m_sourceList->item(row)->text();
    }

    appendLog(QStringLiteral("Starting File Copy preview (%1).").arg(repairToHost
        ? QStringLiteral("Repair → Host") : QStringLiteral("Host → Repair")));
    runRepairHelper(QStringLiteral("Preview File Copy"), arguments);
}

void MainWindow::runFileCopy()
{
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("File Copy unavailable"), reason);
        return;
    }

    const bool repairToHost = m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentIndex() == 1;
    const QString destination = m_destinationEdit->text().trimmed();
    const QString directionLabel = repairToHost ? QStringLiteral("Repair → Host") : QStringLiteral("Host → Repair");
    const QString ownershipLabel = m_ownershipCombo ? m_ownershipCombo->currentText() : QStringLiteral("Smart destination ownership");

    auto isSensitiveTargetPath = [](const QString &path) {
        static const QStringList prefixes = {
            QStringLiteral("/boot"), QStringLiteral("/etc"), QStringLiteral("/usr"), QStringLiteral("/root"),
            QStringLiteral("/var"), QStringLiteral("/bin"), QStringLiteral("/sbin"), QStringLiteral("/lib"),
            QStringLiteral("/lib64"), QStringLiteral("/opt")
        };
        const QString cleaned = QDir::cleanPath(path);
        for (const QString &prefix : prefixes) {
            if (cleaned == prefix || cleaned.startsWith(prefix + QLatin1Char('/'))) {
                return true;
            }
        }
        return false;
    };

    const bool sensitive = !repairToHost && isSensitiveTargetPath(destination);
    QStringList displayedSources;
    for (int row = 0; row < m_sourceList->count() && row < 8; ++row) {
        displayedSources << m_sourceList->item(row)->text();
    }
    if (m_sourceList->count() > 8) {
        displayedSources << QStringLiteral("… %1 more item(s)").arg(m_sourceList->count() - 8);
    }

    QMessageBox confirm(this);
    confirm.setIcon(sensitive ? QMessageBox::Critical : QMessageBox::Warning);
    confirm.setWindowTitle(QStringLiteral("Confirm File Copy"));
    confirm.setText(QStringLiteral("%1 — copy and verify %2 source item(s)?")
                    .arg(directionLabel).arg(m_sourceList->count()));
    QString details = QStringLiteral("Destination: %1\nOwnership: %2\n\nSources:\n• %3\n\n")
        .arg(destination, ownershipLabel, displayedSources.join(QStringLiteral("\n• ")));
    if (repairToHost) {
        details += QStringLiteral("The repair target stays read-only. Existing same-name files in the host destination may be overwritten. The privileged helper refuses system-critical host destinations.\n\n");
    } else {
        details += QStringLiteral("The selected repair filesystem is promoted read-write only after host/target safety checks. Existing same-name target files may be overwritten.\n\n");
        if (sensitive) {
            details += QStringLiteral("SENSITIVE TARGET PATH: this destination can change boot or operating-system files.\n\n");
        }
    }
    details += QStringLiteral("rsync does not use --delete, so unrelated destination files remain. A second checksum/metadata pass and SHA-256 verification of regular files run after the copy.");
    confirm.setInformativeText(details);
    confirm.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    confirm.setDefaultButton(QMessageBox::Cancel);
    confirm.button(QMessageBox::Yes)->setText(QStringLiteral("Copy and Verify"));
    if (confirm.exec() != QMessageBox::Yes) {
        return;
    }

    const QString direction = repairToHost ? QStringLiteral("repair-to-host") : QStringLiteral("host-to-repair");
    const QString ownership = m_ownershipCombo && m_ownershipCombo->currentIndex() == 1
        ? QStringLiteral("preserve") : QStringLiteral("smart");
    const QString approval = sensitive ? QStringLiteral("sensitive-ok") : QStringLiteral("normal");

    QStringList arguments = {QStringLiteral("copy"), m_previewTargetPath, m_previewTargetComponentPath,
                             direction, ownership, approval, destination};
    for (int row = 0; row < m_sourceList->count(); ++row) {
        arguments << m_sourceList->item(row)->text();
    }

    appendLog(QStringLiteral("Starting verified File Copy (%1) to %2.").arg(directionLabel, destination));
    runRepairHelper(QStringLiteral("File Copy — %1").arg(directionLabel), arguments);
}

void MainWindow::refreshCapabilities()
{
    const QList<Capability> capabilities = CapabilityChecker::scanHost();
    m_distributionLabel->setText(CapabilityChecker::distributionLabel());
    m_packageManagerLabel->setText(CapabilityChecker::packageManagerLabel());
#ifdef BOOT_REPAIR_HAVE_KAUTH
    m_authBuildLabel->setText(QStringLiteral("Available — KF6 KAuth linked"));
#else
    m_authBuildLabel->setText(QStringLiteral("Not compiled — optional integration unavailable"));
#endif

    m_capabilityTable->setRowCount(capabilities.size());
    for (int row = 0; row < capabilities.size(); ++row) {
        const Capability &capability = capabilities.at(row);
        m_capabilityTable->setItem(row, 0, readOnlyItem(capability.feature));
        m_capabilityTable->setItem(row, 1, readOnlyItem(capability.command));
        m_capabilityTable->setItem(row, 2, readOnlyItem(capability.scope));
        m_capabilityTable->setItem(row, 3, readOnlyItem(capability.available ? QStringLiteral("Available") : QStringLiteral("Missing")));
        m_capabilityTable->setItem(row, 4, readOnlyItem(capability.packageName));
        m_capabilityTable->setItem(row, 5, readOnlyItem(capability.note));
        if (!capability.available) {
            for (int column = 0; column < m_capabilityTable->columnCount(); ++column) {
                QTableWidgetItem *item = m_capabilityTable->item(row, column);
                if (item) {
                    item->setIcon(column == 3 ? themedIcon(QStringLiteral("dialog-warning")) : QIcon());
                }
            }
        }
    }

    QTimer::singleShot(0, this, [this] { resizeCapabilityRows(); });

    appendLog(QStringLiteral("Host capability scan refreshed. Missing optional tools disable only the related future feature."));
}

void MainWindow::resizeCapabilityRows()
{
    if (!m_capabilityTable) {
        return;
    }

    const QFontMetrics metrics(m_capabilityTable->font());
    const int oneLineHeight = metrics.lineSpacing() + 10;
    m_capabilityTable->verticalHeader()->setDefaultSectionSize(oneLineHeight);

    for (int row = 0; row < m_capabilityTable->rowCount(); ++row) {
        int rowHeight = oneLineHeight;
        bool needsWrapping = false;

        // Only grow a row when text really exceeds the current cell width.
        // Measuring a word-wrapped rectangle unconditionally can return an
        // over-sized hint on some Qt/KDE combinations even when the text is
        // visibly one line, which is what caused the tall fixed-looking rows.
        for (int column = 0; column < m_capabilityTable->columnCount(); ++column) {
            QTableWidgetItem *item = m_capabilityTable->item(row, column);
            if (!item || item->text().isEmpty()) {
                continue;
            }

            const int availableWidth = qMax(24, m_capabilityTable->columnWidth(column) - 14);
            if (metrics.horizontalAdvance(item->text()) <= availableWidth) {
                continue;
            }

            needsWrapping = true;
            const QRect bounds = metrics.boundingRect(
                QRect(0, 0, availableWidth, 10000),
                Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                item->text());
            rowHeight = qMax(rowHeight, bounds.height() + 10);
        }

        // setRowHeight deliberately shrinks rows as well as expanding them;
        // resizeRowsToContents/section size hints may keep a previous larger
        // interactive height after the columns have been widened.
        m_capabilityTable->setRowHeight(row, needsWrapping ? rowHeight : oneLineHeight);
    }
}

void MainWindow::updateDiagnosticDetails()
{
    if (!m_diagnosticList || !m_diagnosticTitle || !m_diagnosticDescription
        || !m_diagnosticAvailability || !m_runDiagnosticButton || !m_diagnosticScopeCombo) {
        return;
    }

    QListWidgetItem *item = m_diagnosticList->currentItem();
    if (!item) {
        if (m_diagnosticResults) {
            m_diagnosticResults->clear();
        }
        if (m_copyDiagnosticButton) {
            m_copyDiagnosticButton->setEnabled(false);
        }
        if (m_saveDiagnosticButton) {
            m_saveDiagnosticButton->setEnabled(false);
        }
        m_diagnosticTitle->setText(QStringLiteral("Select a diagnostic"));
        m_diagnosticDescription->setText(QStringLiteral("Choose a diagnostic from the list."));
        m_diagnosticAvailability->clear();
        m_runDiagnosticButton->setText(QStringLiteral("Run Diagnostic"));
        m_runDiagnosticButton->setEnabled(false);
        return;
    }

    const QString key = item->data(Qt::UserRole).toString();
    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        if (key == QString::fromLatin1(spec.key)) {
            m_diagnosticTitle->setText(QString::fromLatin1(spec.title));
            m_diagnosticDescription->setText(QString::fromLatin1(spec.description));
            break;
        }
    }

    const bool targetScope = m_diagnosticScopeCombo->currentIndex() == 0;
    const QString targetIdentity = currentTargetDiagnosticCacheIdentity();
    if (targetScope && m_targetDiagnosticCacheIdentity != targetIdentity) {
        m_targetDiagnosticCache.clear();
        m_targetDiagnosticTimes.clear();
        m_targetDiagnosticCacheIdentity = targetIdentity;
        // A changed filesystem identity invalidates repair evidence as well
        // as the Results pane; keep every repair control in sync immediately.
        updateFullRepairSummary();
    }

    const QMap<QString, QString> &cache = targetScope ? m_targetDiagnosticCache : m_hostDiagnosticCache;
    const QMap<QString, QDateTime> &times = targetScope ? m_targetDiagnosticTimes : m_hostDiagnosticTimes;
    const QString cached = cache.value(key);
    const QDateTime cachedAt = times.value(key);
    if (m_diagnosticResults) {
        m_diagnosticResults->setPlainText(cached.isEmpty() && targetScope && m_targetDiagnosticsNeedRegeneration
            ? QStringLiteral("Target state changed after a repair action. Please re-run the %1 diagnostic before reviewing or running this repair action.")
                  .arg(m_diagnosticTitle->text())
            : cached);
    }
    if (m_copyDiagnosticButton) {
        m_copyDiagnosticButton->setEnabled(!cached.isEmpty());
    }
    if (m_saveDiagnosticButton) {
        m_saveDiagnosticButton->setEnabled(!cached.isEmpty());
    }

    if (targetScope && m_previewTargetPath.isEmpty()) {
        m_diagnosticAvailability->setText(QStringLiteral("Select a repair target in Systems."));
        m_runDiagnosticButton->setText(QStringLiteral("Run Diagnostic"));
        m_runDiagnosticButton->setEnabled(false);
        return;
    }

    if (!targetScope) {
        QString hostReason;
        if (!hostBootTargetReady(&hostReason)) {
            m_diagnosticAvailability->setText(hostReason);
            m_runDiagnosticButton->setText(QStringLiteral("Run Diagnostic"));
            m_runDiagnosticButton->setEnabled(false);
            return;
        }
    }

    if (!cached.isEmpty()) {
        const QString stamp = cachedAt.isValid()
            ? cachedAt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
            : QStringLiteral("this session");
        m_diagnosticAvailability->setText(targetScope
            ? QStringLiteral("Cached: %1 — read-only selected repair-system result. Selecting another diagnostic and returning here keeps this output; use Re-run only for fresh data.").arg(stamp)
            : QStringLiteral("Cached: %1 — protected running-host result.").arg(stamp));
        m_runDiagnosticButton->setText(QStringLiteral("Re-run Diagnostic"));
    } else {
        if (targetScope) {
            m_diagnosticAvailability->setText(m_targetDiagnosticsNeedRegeneration
                ? QStringLiteral("Please re-run this diagnostic after the last repair or target configuration change.")
                : (m_privilegedSessionReady
                    ? QStringLiteral("Available — read-only selected repair-system inspection; administrator authorization is already active for this Boot Bitch window.")
                    : QStringLiteral("Available — read-only selected repair-system inspection; the first root action will request administrator authorization.")));
        } else {
            m_diagnosticAvailability->setText(QStringLiteral("Available — protected running host"));
        }
        m_runDiagnosticButton->setText(QStringLiteral("Run Diagnostic"));
    }
    m_runDiagnosticButton->setEnabled(true);
}

QString MainWindow::diagnosticResultForKey(const QString &key) const
{
    const bool targetScope = m_diagnosticScopeCombo && m_diagnosticScopeCombo->currentIndex() == 0;
    const QString scopeName = targetScope ? QStringLiteral("Repair Target") : QStringLiteral("Running Host");
    const QString diskPath = targetScope ? m_previewTargetPath : m_hostPrimaryPath;

    QString output;
    QTextStream stream(&output);
    stream << "Diagnostic: " << key << '\n';
    stream << "Scope: " << scopeName << '\n';
    stream << "Time: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n\n";

    if (diskPath.isEmpty() || !m_deviceIndex.contains(diskPath)) {
        stream << "No system is selected for this diagnostic.\n";
        return output;
    }

    const DeviceNode disk = m_deviceIndex.value(diskPath);
    const DeviceNode *preferred = preferredRepairNode(disk);
    const QString preferredPath = preferred && !preferred->path.isEmpty() ? preferred->path : disk.path;

    auto readFile = [](const QString &path) -> QString {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QStringLiteral("Unavailable or not readable: %1").arg(path);
        }
        QByteArray data = file.read(128 * 1024);
        QString text = QString::fromUtf8(data);
        if (!file.atEnd()) {
            text += QStringLiteral("\n… output truncated …\n");
        }
        return text.trimmed();
    };

    auto readRedactedCrypttab = []() -> QString {
        QFile file(QStringLiteral("/etc/crypttab"));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QStringLiteral("Unavailable or not readable: /etc/crypttab");
        }
        QTextStream input(&file);
        QStringList lines;
        while (!input.atEnd()) {
            const QString line = input.readLine();
            const QString trimmed = line.trimmed();
            if (trimmed.isEmpty() || trimmed.startsWith(QLatin1Char('#'))) {
                lines.append(line);
                continue;
            }
            const QStringList fields = trimmed.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            lines.append(fields.size() >= 2
                ? QStringLiteral("%1 %2 <redacted>").arg(fields.at(0), fields.at(1))
                : QStringLiteral("<redacted>"));
        }
        return lines.join(QLatin1Char('\n')).trimmed();
    };

    auto appendDeviceTree = [&](QTextStream &out) {
        auto walk = [&](auto &&self, const DeviceNode &node, int depth) -> void {
            const QString indent(depth * 2, QLatin1Char(' '));
            out << indent << (node.path.isEmpty() ? node.name : node.path);
            if (!node.fileSystem.isEmpty()) {
                out << "  fs=" << node.fileSystem;
            }
            if (!node.uuid.isEmpty()) {
                out << "  uuid=" << node.uuid;
            }
            if (node.encrypted) {
                out << "  encrypted";
            }
            if (!node.mountPoints.isEmpty()) {
                out << "  mounts=" << node.mountPoints.join(QStringLiteral(", "));
            }
            out << '\n';
            for (const DeviceNode &child : node.children) {
                self(self, child, depth + 1);
            }
        };
        walk(walk, disk, 0);
    };

    if (key == QStringLiteral("environment")) {
        stream << "System: " << (firstLinuxNameInTree(disk).isEmpty()
            ? QStringLiteral("Linux installation not yet identified")
            : firstLinuxNameInTree(disk)) << '\n';
        stream << "Physical drive: " << disk.path << '\n';
        stream << "Detected component: " << preferredPath << '\n';
        stream << "Model / label: " << friendlyNodeName(disk) << '\n';
        stream << "Status: " << friendlyTopLevelStatus(disk) << '\n';
        stream << "Protected: " << (disk.protectedDevice ? QStringLiteral("yes") : QStringLiteral("no")) << '\n';
        stream << "Distribution (host): " << CapabilityChecker::distributionLabel() << '\n';
        stream << "Package manager (host): " << CapabilityChecker::packageManagerLabel() << '\n';
    } else if (key == QStringLiteral("boot")) {
        stream << "Physical drive: " << disk.path << '\n';
        stream << "Detected component: " << preferredPath << '\n';
        stream << "Visible storage tree:\n";
        appendDeviceTree(stream);
        if (targetScope) {
            stream << "\nTarget boot-file inspection becomes available after the target is mounted read-only.\n";
        } else {
            stream << "\nRunning-host critical boot mounts are protected by the Systems scanner.\n";
        }
    } else if (key == QStringLiteral("kernel")) {
        if (targetScope) {
            stream << "Selected drive: " << disk.path << '\n';
            stream << "Kernel/initramfs file inspection requires the target to be mounted read-only.\n";
        } else {
            QDir boot(QStringLiteral("/boot"));
            const QStringList kernels = boot.entryList({QStringLiteral("vmlinuz-*"), QStringLiteral("vmlinuz")}, QDir::Files, QDir::Name);
            const QStringList initrds = boot.entryList({QStringLiteral("initrd.img-*"), QStringLiteral("initrd.img")}, QDir::Files, QDir::Name);
            stream << "Visible host kernels:\n";
            for (const QString &name : kernels) stream << "  " << name << '\n';
            if (kernels.isEmpty()) stream << "  none visible\n";
            stream << "\nVisible host initramfs images:\n";
            for (const QString &name : initrds) stream << "  " << name << '\n';
            if (initrds.isEmpty()) stream << "  none visible\n";
        }
    } else if (key == QStringLiteral("grub")) {
        if (targetScope) {
            stream << "Selected drive: " << disk.path << '\n';
            stream << "Target GRUB configuration requires the target to be mounted read-only.\n";
        } else {
            const QString path = QStringLiteral("/boot/grub/grub.cfg");
            const QString data = readFile(path);
            stream << "GRUB path: " << path << "\n\n" << data << '\n';
        }
    } else if (key == QStringLiteral("uki")) {
        if (targetScope) {
            stream << "Target EFI/UKI inspection requires the target and ESP to be mounted read-only.\n";
        } else {
            stream << "Running-host EFI/UKI inspection is collected by the privileged read-only host diagnostic backend.\n";
        }
    } else if (key == QStringLiteral("display")) {
        if (targetScope) {
            stream << "Target graphical-login/display-manager inspection requires the target to be mounted read-only.\n";
        } else {
            QFileInfo dm(QStringLiteral("/etc/systemd/system/display-manager.service"));
            QFileInfo def(QStringLiteral("/etc/systemd/system/default.target"));
            stream << "display-manager.service: " << (dm.isSymLink() ? dm.symLinkTarget() : QStringLiteral("not enabled / not a symlink")) << '\n';
            stream << "default.target: " << (def.isSymLink() ? def.symLinkTarget() : QStringLiteral("not a symlink")) << '\n';
            const QStringList managers = {QStringLiteral("sddm"), QStringLiteral("gdm3"), QStringLiteral("lightdm"), QStringLiteral("ly"), QStringLiteral("greetd")};
            stream << "Display manager executable candidates:" << '\n';
            for (const QString &manager : managers) {
                const QString executable = QStandardPaths::findExecutable(manager);
                stream << "  " << manager << ": " << (executable.isEmpty() ? QStringLiteral("not found") : executable) << '\n';
            }
        }
    } else if (key == QStringLiteral("errors")) {
        const QString journalctl = QStandardPaths::findExecutable(QStringLiteral("journalctl"));
        stream << "journalctl: " << (journalctl.isEmpty() ? QStringLiteral("not found") : journalctl) << '\n';
        stream << "Boot journal queries will be enabled with the read-only inspection backend.\n";
    } else if (key == QStringLiteral("usage")) {
        if (targetScope) {
            stream << "Drive: " << disk.path << '\n';
            stream << "Capacity: " << SystemScanner::humanSize(disk.sizeBytes) << '\n';
            stream << "Filesystem free-space inspection requires a read-only target mount.\n";
        } else {
            QStorageInfo storage(QStringLiteral("/"));
            stream << "Root filesystem: " << storage.rootPath() << '\n';
            stream << "Device: " << QString::fromUtf8(storage.device()) << '\n';
            stream << "Filesystem: " << QString::fromUtf8(storage.fileSystemType()) << '\n';
            const qint64 total = storage.bytesTotal();
            const qint64 available = storage.bytesAvailable();
            stream << "Total: " << (total >= 0 ? SystemScanner::humanSize(static_cast<quint64>(total)) : QStringLiteral("unknown")) << '\n';
            stream << "Available: " << (available >= 0 ? SystemScanner::humanSize(static_cast<quint64>(available)) : QStringLiteral("unknown")) << '\n';
        }
    } else if (key == QStringLiteral("fstab")) {
        if (targetScope) {
            stream << "Target /etc/fstab becomes available after read-only target mounting.\n";
        } else {
            stream << readFile(QStringLiteral("/etc/fstab")) << '\n';
        }
    } else if (key == QStringLiteral("btrfs")) {
        bool found = false;
        auto walk = [&](auto &&self, const DeviceNode &node) -> void {
            if (node.fileSystem.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) == 0) {
                found = true;
                stream << (node.path.isEmpty() ? node.name : node.path)
                       << "  uuid=" << (node.uuid.isEmpty() ? QStringLiteral("—") : node.uuid)
                       << "  mounts=" << (node.mountPoints.isEmpty() ? QStringLiteral("—") : node.mountPoints.join(QStringLiteral(", ")))
                       << '\n';
            }
            for (const DeviceNode &child : node.children) self(self, child);
        };
        walk(walk, disk);
        if (!found) stream << "No Btrfs filesystem is currently visible in this device tree.\n";
        if (targetScope) stream << "Subvolume inspection becomes available after read-only target mounting.\n";
    } else if (key == QStringLiteral("mapper") || key == QStringLiteral("luks")) {
        bool found = false;
        auto walk = [&](auto &&self, const DeviceNode &node) -> void {
            if (node.encrypted || node.type == QStringLiteral("crypt")) {
                found = true;
                stream << (node.path.isEmpty() ? node.name : node.path)
                       << "  type=" << node.type
                       << "  fs=" << (node.fileSystem.isEmpty() ? QStringLiteral("—") : node.fileSystem)
                       << "  uuid=" << (node.uuid.isEmpty() ? QStringLiteral("—") : node.uuid)
                       << '\n';
            }
            for (const DeviceNode &child : node.children) self(self, child);
        };
        walk(walk, disk);
        if (!found) stream << "No encrypted or mapped volume is currently visible in this device tree.\n";
        if (key == QStringLiteral("luks") && !targetScope) {
            stream << "\n/etc/crypttab (key fields redacted):\n" << readRedactedCrypttab() << '\n';
        } else if (key == QStringLiteral("luks") && targetScope) {
            stream << "\nTarget crypttab inspection becomes available after read-only target mounting.\n";
        }
    } else if (key == QStringLiteral("report")) {
        static const QStringList keys = {
            QStringLiteral("environment"), QStringLiteral("boot"), QStringLiteral("kernel"),
            QStringLiteral("grub"), QStringLiteral("uki"), QStringLiteral("display"), QStringLiteral("errors"), QStringLiteral("usage"),
            QStringLiteral("fstab"), QStringLiteral("btrfs"), QStringLiteral("mapper"), QStringLiteral("luks")
        };
        QString combined;
        QTextStream combinedStream(&combined);
        for (const QString &subKey : keys) {
            combinedStream << "========================================\n";
            combinedStream << subKey.toUpper() << '\n';
            combinedStream << "========================================\n";
            combinedStream << diagnosticResultForKey(subKey) << "\n\n";
        }
        return combined;
    } else {
        stream << "No diagnostic implementation is registered for this item.\n";
    }

    return output;
}

QString MainWindow::runTargetDiagnosticHelper(const QString &key, bool *succeeded,
                                                bool showProgressDialog)
{
    if (succeeded) {
        *succeeded = false;
    }

    QString reason;
    if (!repairTargetReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("Target diagnostic unavailable"), reason);
        return QStringLiteral("ERROR: %1\n").arg(reason);
    }

    appendLog(QStringLiteral("Starting read-only target diagnostic '%1' on %2 (%3).")
                  .arg(key, m_previewTargetPath, m_previewTargetComponentPath));
    const QString captured = runPrivilegedRequest(
        key == QStringLiteral("all")
            ? QStringLiteral("Run All target diagnostics")
            : QStringLiteral("Read-only target diagnostic"),
        {QStringLiteral("diagnose"), m_previewTargetPath, m_previewTargetComponentPath, key},
        QByteArray(),
        succeeded,
        showProgressDialog);

    appendLog(QStringLiteral("Read-only target diagnostic '%1' %2.")
                  .arg(key, (succeeded && *succeeded) ? QStringLiteral("completed") : QStringLiteral("failed")),
              (succeeded && *succeeded) ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    return captured;
}

QString MainWindow::runHostDiagnosticHelper(const QString &key, bool *succeeded,
                                             bool showProgressDialog)
{
    if (succeeded) {
        *succeeded = false;
    }

    QString reason;
    if (!hostBootTargetReady(&reason)) {
        QMessageBox::warning(this, QStringLiteral("Host diagnostic unavailable"), reason);
        return QStringLiteral("ERROR: %1\n").arg(reason);
    }

    appendLog(QStringLiteral("Starting read-only running-host diagnostic '%1' on %2 (%3).")
                  .arg(key, m_hostPrimaryPath, m_hostPrimaryComponentPath));
    const QString captured = runPrivilegedRequest(
        key == QStringLiteral("all") || key == QStringLiteral("report")
            ? QStringLiteral("Run running-host diagnostics")
            : QStringLiteral("Read-only running-host diagnostic"),
        {QStringLiteral("host-diagnose"), m_hostPrimaryPath, m_hostPrimaryComponentPath, key},
        QByteArray(), succeeded, showProgressDialog);

    appendLog(QStringLiteral("Read-only running-host diagnostic '%1' %2.")
                  .arg(key, (succeeded && *succeeded) ? QStringLiteral("completed") : QStringLiteral("failed")),
              (succeeded && *succeeded) ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    return captured;
}

QString MainWindow::currentTargetDiagnosticCacheIdentity() const
{
    // Diagnostics are scoped to the explicitly selected physical drive. The
    // preferred root component may legitimately change spelling after an
    // unlock or topology refresh (for example /dev/mapper/name versus
    // /dev/dm-N); using that transient alias would discard valid evidence
    // immediately after a harmless validation refresh. Mutating repairs and
    // explicit target changes still invalidate this cache explicitly.
    return m_previewTargetPath;
}

void MainWindow::clearTargetDiagnosticCache()
{
    m_targetDiagnosticCache.clear();
    m_targetDiagnosticTimes.clear();
    m_targetDiagnosticCacheIdentity = currentTargetDiagnosticCacheIdentity();
}

void MainWindow::cacheHostDiagnosticBundle(const QString &bundle)
{
    m_hostDiagnosticCache.clear();
    m_hostDiagnosticTimes.clear();
    const QDateTime capturedAt = QDateTime::currentDateTime();
    m_hostDiagnosticCache.insert(QStringLiteral("report"), bundle.trimmed());
    m_hostDiagnosticTimes.insert(QStringLiteral("report"), capturedAt);

    const QString divider = QStringLiteral("========================================\n");
    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        const QString key = QString::fromLatin1(spec.key);
        if (key == QStringLiteral("report")) {
            continue;
        }
        const QString marker = QStringLiteral("Diagnostic: %1\n").arg(key);
        const int markerPos = bundle.indexOf(marker);
        if (markerPos < 0) {
            continue;
        }
        int sectionStart = bundle.lastIndexOf(divider, markerPos);
        if (sectionStart >= 0) {
            const int priorDivider = bundle.lastIndexOf(divider, sectionStart - 1);
            if (priorDivider >= 0) {
                sectionStart = priorDivider;
            }
        } else {
            sectionStart = markerPos;
        }
        int sectionEnd = bundle.indexOf(divider, markerPos + marker.size());
        if (sectionEnd < 0) {
            sectionEnd = bundle.size();
        }
        const QString section = bundle.mid(sectionStart, sectionEnd - sectionStart).trimmed();
        if (!section.isEmpty()) {
            m_hostDiagnosticCache.insert(key, section);
            m_hostDiagnosticTimes.insert(key, capturedAt);
        }
    }
}

bool MainWindow::targetDiagnosticEvidenceReady(QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    if (m_previewTargetPath.isEmpty() || m_previewTargetComponentPath.isEmpty()) {
        setReason(QStringLiteral("Select a repair target in Systems first."));
        return false;
    }
    if (m_targetDiagnosticCacheIdentity != currentTargetDiagnosticCacheIdentity()) {
        setReason(QStringLiteral("Run All diagnostics for the selected target before starting a repair."));
        return false;
    }
    if (m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty()) {
        setReason(m_targetDiagnosticsNeedRegeneration
            ? QStringLiteral("Target state changed after the last diagnostic or repair action. Please regenerate diagnostics in Diagnostics → Repair Target → Run All before starting another repair.")
            : QStringLiteral("Run All diagnostics for the selected target before starting a repair."));
        return false;
    }

    setReason(QStringLiteral("Read-only diagnostics are cached for this target."));
    return true;
}

bool MainWindow::repairEvidenceReadyForTool(const QString &toolKey, QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    if (m_hostMaintenanceMode) {
        QString hostReason;
        if (!hostBootTargetReady(&hostReason)) {
            setReason(hostReason);
            return false;
        }
        if (m_hostDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty()) {
            setReason(QStringLiteral("Run All diagnostics for the protected running host before starting this maintenance action."));
            return false;
        }
        setReason(QStringLiteral("Required read-only running-host diagnostics are cached for this maintenance action."));
        return true;
    }

    QString diagnosticKey = QStringLiteral("report");
    if (toolKey == QStringLiteral("validate")) diagnosticKey = QStringLiteral("environment");
    else if (toolKey == QStringLiteral("grub")) diagnosticKey = QStringLiteral("grub");
    else if (toolKey == QStringLiteral("initramfs")) diagnosticKey = QStringLiteral("kernel");
    else if (toolKey == QStringLiteral("efi")) diagnosticKey = QStringLiteral("uki");
    else if (toolKey == QStringLiteral("display")) diagnosticKey = QStringLiteral("display");
    else if (toolKey == QStringLiteral("dkms")) diagnosticKey = QStringLiteral("kernel");
    else if (toolKey == QStringLiteral("bootstack")) diagnosticKey = QStringLiteral("report");
    else if (toolKey == QStringLiteral("dpkg") || toolKey == QStringLiteral("fixbroken")
             || toolKey == QStringLiteral("aptupdate") || toolKey == QStringLiteral("upgrade")) {
        diagnosticKey = QStringLiteral("environment");
    }

    if (m_previewTargetPath.isEmpty() || m_previewTargetComponentPath.isEmpty()) {
        setReason(QStringLiteral("Select a repair target in Systems first."));
        return false;
    }
    if (m_targetDiagnosticCacheIdentity != currentTargetDiagnosticCacheIdentity()) {
        setReason(QStringLiteral("Run the selected target diagnostic before starting this repair."));
        return false;
    }
    if (m_targetDiagnosticCache.value(diagnosticKey).trimmed().isEmpty()) {
        const QString diagnosticName = diagnosticKey == QStringLiteral("grub")
            ? QStringLiteral("GRUB configuration")
            : diagnosticKey;
        setReason(m_targetDiagnosticsNeedRegeneration
            ? QStringLiteral("Target state changed after the last diagnostic or repair action. Run the %1 diagnostic for this target before starting this repair.").arg(diagnosticName)
            : QStringLiteral("Run the %1 diagnostic for this target before starting this repair.").arg(diagnosticName));
        return false;
    }
    setReason(QStringLiteral("Required read-only diagnostic evidence is cached for this repair tool."));
    return true;
}

void MainWindow::cacheTargetDiagnosticBundle(const QString &bundle)
{
    clearTargetDiagnosticCache();
    const QDateTime capturedAt = QDateTime::currentDateTime();
    m_targetDiagnosticCache.insert(QStringLiteral("report"), bundle.trimmed());
    m_targetDiagnosticTimes.insert(QStringLiteral("report"), capturedAt);

    const QString divider = QStringLiteral("========================================\n");
    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        const QString key = QString::fromLatin1(spec.key);
        if (key == QStringLiteral("report")) {
            continue;
        }

        const QString marker = QStringLiteral("Diagnostic: %1\n").arg(key);
        const int markerPos = bundle.indexOf(marker);
        if (markerPos < 0) {
            continue;
        }

        int sectionStart = bundle.lastIndexOf(divider, markerPos);
        if (sectionStart >= 0) {
            const int priorDivider = bundle.lastIndexOf(divider, sectionStart - 1);
            if (priorDivider >= 0) {
                sectionStart = priorDivider;
            }
        } else {
            sectionStart = markerPos;
        }

        int sectionEnd = bundle.indexOf(divider, markerPos + marker.size());
        if (sectionEnd < 0) {
            sectionEnd = bundle.size();
        }
        const QString section = bundle.mid(sectionStart, sectionEnd - sectionStart).trimmed();
        if (!section.isEmpty()) {
            m_targetDiagnosticCache.insert(key, section);
            m_targetDiagnosticTimes.insert(key, capturedAt);
        }
    }
}

void MainWindow::runSelectedDiagnostic()
{
    if (!m_diagnosticList || !m_diagnosticResults) {
        return;
    }

    QListWidgetItem *item = m_diagnosticList->currentItem();
    if (!item) {
        return;
    }

    const QString key = item->data(Qt::UserRole).toString();
    const bool targetScope = m_diagnosticScopeCombo && m_diagnosticScopeCombo->currentIndex() == 0;
    // A re-run establishes a new source of truth. Discard every older cached
    // result for this scope before starting, including the full report.
    if (targetScope) {
        clearTargetDiagnosticCache();
    } else {
        m_hostDiagnosticCache.clear();
        m_hostDiagnosticTimes.clear();
    }
    m_diagnosticResults->clear();
    m_copyDiagnosticButton->setEnabled(false);
    m_saveDiagnosticButton->setEnabled(false);
    bool ok = true;
    const QString result = targetScope
        ? runTargetDiagnosticHelper(key, &ok)
        : runHostDiagnosticHelper(key, &ok);

    const QDateTime capturedAt = QDateTime::currentDateTime();
    if (targetScope) {
        const QString identity = currentTargetDiagnosticCacheIdentity();
        if (m_targetDiagnosticCacheIdentity != identity) {
            clearTargetDiagnosticCache();
        }
        if (key == QStringLiteral("report")) {
            cacheTargetDiagnosticBundle(result);
            if (ok && !result.trimmed().isEmpty()) {
                m_targetDiagnosticsNeedRegeneration = false;
            }
        } else {
            m_targetDiagnosticCache.remove(QStringLiteral("report"));
            m_targetDiagnosticTimes.remove(QStringLiteral("report"));
            m_targetDiagnosticCache.insert(key, result);
            m_targetDiagnosticTimes.insert(key, capturedAt);
        }
    } else {
        if (key == QStringLiteral("report")) {
            cacheHostDiagnosticBundle(result);
        } else {
            m_hostDiagnosticCache.remove(QStringLiteral("report"));
            m_hostDiagnosticTimes.remove(QStringLiteral("report"));
            m_hostDiagnosticCache.insert(key, result);
            m_hostDiagnosticTimes.insert(key, capturedAt);
        }
    }

    updateDiagnosticDetails();
    appendDiagnosticLog(key, item->text(),
                       targetScope ? QStringLiteral("Repair Target") : QStringLiteral("Running Host"),
                       result, ok);
    appendLog(QStringLiteral("%1 diagnostic run: %2").arg(targetScope ? QStringLiteral("Target") : QStringLiteral("Host"), item->text()),
              ok ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
}

void MainWindow::editTargetConfig()
{
    if (!m_targetConfigCombo || m_previewTargetPath.isEmpty() || m_previewTargetComponentPath.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("No repair target selected"),
                                 QStringLiteral("Select and unlock a repair target in Systems first."));
        return;
    }
    const QString key = m_targetConfigCombo->currentData().toString();
    const QString displayPath = m_targetConfigCombo->currentText();
    bool ok = false;
    const QString output = runPrivilegedRequest(
        QStringLiteral("Read target configuration"),
        {QStringLiteral("config-read"), m_previewTargetPath, m_previewTargetComponentPath, key},
        QByteArray(), &ok, false);
    if (!ok) {
        QMessageBox::warning(this, QStringLiteral("Configuration unavailable"), output.trimmed());
        return;
    }

    QString content = output;
    const QString marker = QStringLiteral("Inspection is read-only.\n\n");
    const int markerPos = content.indexOf(marker);
    if (markerPos >= 0) {
        content = content.mid(markerPos + marker.size());
    }
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Edit target %1").arg(displayPath));
    dialog.resize(900, 650);
    auto *layout = standardDialogLayout(&dialog, 680);
    auto *info = new QLabel(QStringLiteral(
        "Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/grub.cfg may be replaced by the next bootloader update."));
    info->setWordWrap(true);
    layout->addWidget(info);
    auto *editor = new QPlainTextEdit;
    editor->setPlainText(content);
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    QFont mono(QStringLiteral("monospace"));
    mono.setStyleHint(QFont::Monospace);
    editor->setFont(mono);
    layout->addWidget(editor, 1);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Save Target File"));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    if (QMessageBox::question(this, QStringLiteral("Write target configuration"),
                              QStringLiteral("Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.").arg(displayPath),
                              QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Yes) {
        return;
    }
    const QString edited = editor->toPlainText();
    bool saved = false;
    const QString writeOutput = runPrivilegedRequest(
        QStringLiteral("Write target configuration"),
        {QStringLiteral("config-write"), m_previewTargetPath, m_previewTargetComponentPath, key, edited},
        QByteArray(), &saved, false);
    if (!saved) {
        QMessageBox::warning(this, QStringLiteral("Configuration write failed"), writeOutput.trimmed());
        return;
    }
    clearTargetDiagnosticCache();
    m_targetDiagnosticsNeedRegeneration = true;
    updateFullRepairSummary();
    appendLog(QStringLiteral("STALE DIAGNOSTICS: target configuration edited: %1. Regenerate diagnostics before the next repair.").arg(displayPath));
    QString followUp;
    if (key == QStringLiteral("grub-defaults") || key == QStringLiteral("grub-config")) {
        followUp = QStringLiteral("Run the GRUB configuration repair stage (or Full Repair with GRUB enabled), then rerun diagnostics before any further repair.");
    } else if (key == QStringLiteral("crypttab") || key == QStringLiteral("initramfs")) {
        followUp = QStringLiteral("Run the initramfs repair stage (or Full Repair with initramfs enabled), then rerun diagnostics before any further repair.");
    } else if (key == QStringLiteral("fstab")) {
        followUp = QStringLiteral("Rerun diagnostics to validate the edited mount configuration before any further repair.");
    }
    const QString savedText = writeOutput.trimmed()
        + (followUp.isEmpty() ? QString() : QStringLiteral("\n\n") + followUp);
    QMessageBox::information(this, QStringLiteral("Configuration saved"), savedText);
}

void MainWindow::runAllDiagnostics()
{
    if (!m_diagnosticResults) {
        return;
    }

    if (m_diagnosticScopeCombo && m_diagnosticScopeCombo->currentIndex() == 0 && m_previewTargetPath.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("No repair target selected"),
                                 QStringLiteral("Select a repair target in Systems or switch Diagnostics to Running Host."));
        return;
    }

    const bool targetScope = m_diagnosticScopeCombo && m_diagnosticScopeCombo->currentIndex() == 0;
    QString combined;
    bool ok = true;

    if (m_runAllDiagnosticsButton) {
        m_runAllDiagnosticsButton->setEnabled(false);
        m_runAllDiagnosticsButton->setText(QStringLiteral("Running…"));
    }

    // Clear stale evidence before collecting a fresh report. If collection
    // fails, repair controls remain blocked instead of reusing old data.
    if (targetScope) {
        clearTargetDiagnosticCache();
    } else {
        m_hostDiagnosticCache.clear();
        m_hostDiagnosticTimes.clear();
    }
    m_diagnosticResults->clear();
    m_copyDiagnosticButton->setEnabled(false);
    m_saveDiagnosticButton->setEnabled(false);

    if (targetScope) {
        // Keep the combined report in the Diagnostics pane and application log.
        // A modal output window obscures the report users need to review, and
        // individual diagnostics already use the persistent results pane.
        combined = runTargetDiagnosticHelper(QStringLiteral("all"), &ok, false);
        if (ok && !combined.isEmpty()) {
            cacheTargetDiagnosticBundle(combined);
            m_targetDiagnosticsNeedRegeneration = false;
        }
    } else {
        combined = runHostDiagnosticHelper(QStringLiteral("all"), &ok, false);
        if (ok && !combined.isEmpty()) {
            cacheHostDiagnosticBundle(combined);
        }
    }

    m_diagnosticResults->setPlainText(combined);
    m_copyDiagnosticButton->setEnabled(!combined.isEmpty());
    m_saveDiagnosticButton->setEnabled(!combined.isEmpty());
    // The repair page has its own readiness controls. Refresh them immediately
    // after a successful full report so switching tabs is not required to
    // expose the newly cached evidence.
    updateFullRepairSummary();
    appendLog(QStringLiteral("Starting all available read-only diagnostics (%1 scope).")
                  .arg(targetScope ? QStringLiteral("Target") : QStringLiteral("Host")));
    appendDiagnosticLog(QStringLiteral("report"), QStringLiteral("Full diagnostic report"),
                        targetScope ? QStringLiteral("Repair Target") : QStringLiteral("Running Host"),
                        combined, ok);

    // Run All is itself the full report. Select that cached view so the user
    // can immediately browse to any individual diagnostic and back without
    // losing results or triggering another privileged operation.
    if (m_diagnosticList && ok && !combined.isEmpty()) {
        for (int row = 0; row < m_diagnosticList->count(); ++row) {
            QListWidgetItem *candidate = m_diagnosticList->item(row);
            if (candidate && candidate->data(Qt::UserRole).toString() == QStringLiteral("report")) {
                m_diagnosticList->setCurrentItem(candidate);
                break;
            }
        }
    }

    appendLog(ok
                  ? QStringLiteral("All available read-only diagnostic summaries generated and cached by diagnostic.")
                  : QStringLiteral("Run All diagnostics failed; failed output was not cached as successful diagnostic data."),
              ok ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
    if (m_runAllDiagnosticsButton) {
        m_runAllDiagnosticsButton->setEnabled(true);
        m_runAllDiagnosticsButton->setText(QStringLiteral("Run All"));
    }
}

void MainWindow::copyDiagnosticResults()
{
    if (!m_diagnosticResults) {
        return;
    }
    const QString text = m_diagnosticResults->toPlainText();
    if (!text.isEmpty()) {
        QApplication::clipboard()->setText(text);
        statusBar()->showMessage(QStringLiteral("Diagnostic results copied"), 2500);
        appendLog(QStringLiteral("Diagnostic results copied to the clipboard."));
    }
}

void MainWindow::saveDiagnosticResults()
{
    if (!m_diagnosticResults) {
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Save diagnostic results"),
        QDir::homePath() + QStringLiteral("/boot-repair-diagnostics.txt"),
        QStringLiteral("Text files (*.txt *.log);;All files (*)"));

    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, QStringLiteral("Unable to save diagnostics"), file.errorString());
        return;
    }

    QTextStream stream(&file);
    stream << m_diagnosticResults->toPlainText();
    statusBar()->showMessage(QStringLiteral("Diagnostics saved to %1").arg(path), 5000);
}

void MainWindow::saveLogAs()
{
    const QString defaultPath = QDir::homePath() + QStringLiteral("/boot-repair.log");
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Save application log"), defaultPath, QStringLiteral("Log files (*.log *.txt);;All files (*)"));
    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, QStringLiteral("Unable to save log"), file.errorString());
        return;
    }

    QTextStream stream(&file);
    // Export the complete register even when the Logs tab is filtered.
    stream << m_actionLogEntries.join(QLatin1Char('\n'));
    statusBar()->showMessage(QStringLiteral("Log saved to %1").arg(path), 5000);
}

void MainWindow::appendLog(const QString &message, const QString &level)
{
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    QString category = QStringLiteral("APPLICATION");
    const QString folded = message.toCaseFolded();
    if (folded.contains(QStringLiteral("chroot shell"))) category = QStringLiteral("CHROOT SHELL");
    else if (folded.contains(QStringLiteral("repair output")) || folded.contains(QStringLiteral("repair")) || folded.contains(QStringLiteral("grub"))
             || folded.contains(QStringLiteral("initramfs")) || folded.contains(QStringLiteral("sddm"))) category = QStringLiteral("REPAIR");
    else if (folded.contains(QStringLiteral("diagnostic"))) category = QStringLiteral("DIAGNOSTIC");
    else if (folded.contains(QStringLiteral("unlock")) || folded.contains(QStringLiteral("luks"))) category = QStringLiteral("UNLOCK");
    else if (folded.contains(QStringLiteral("file copy")) || folded.contains(QStringLiteral("copied"))) category = QStringLiteral("FILE COPY");
    else if (folded.contains(QStringLiteral("snapshot")) || folded.contains(QStringLiteral("btrfs"))) category = QStringLiteral("SNAPSHOTS");
    const QString entry = QStringLiteral("──────── %1 ────────\n[%2] [%3] %4")
        .arg(category, timestamp, level, message);
    // Keep the newest event at the front so the Logs tab opens directly on
    // the most recent repair, diagnostic, or shell transcript.
    m_actionLogEntries.prepend(entry);
    while (m_actionLogEntries.size() > 5000) m_actionLogEntries.removeLast();
    if (m_settings) { m_settings->setValue(QStringLiteral("actionRegister"), m_actionLogEntries); m_settings->sync(); }
    refreshLogView();
}

void MainWindow::refreshLogView()
{
    if (!m_logView) {
        return;
    }

    QScrollBar *scrollBar = m_logView->verticalScrollBar();
    const bool wasAtTop = !scrollBar || scrollBar->value() <= 2;
    const auto markStaleEntries = [this] {
        QList<QTextEdit::ExtraSelection> selections;
        QTextCharFormat staleFormat;
        staleFormat.setBackground(palette().color(QPalette::AlternateBase));
        // BrightText is commonly white in both KDE dark and GNOME light
        // palettes, which makes stale entries disappear against a light
        // AlternateBase. Use the theme's normal text role for readable
        // contrast in either appearance.
        staleFormat.setForeground(palette().color(QPalette::Text));
        QList<QTextBlock> entryBlocks;
        const auto flushEntry = [&] {
            if (entryBlocks.isEmpty()) return;
            QString entryText;
            for (const QTextBlock &entryBlock : entryBlocks) {
                entryText += entryBlock.text() + QLatin1Char('\n');
            }
            const bool stale = entryText.contains(QStringLiteral("STALE DIAGNOSTICS"), Qt::CaseInsensitive)
                || (m_targetDiagnosticsNeedRegeneration
                    && entryText.contains(QStringLiteral("Diagnostic:"), Qt::CaseInsensitive)
                    && entryText.contains(QStringLiteral("Scope: Repair Target"), Qt::CaseInsensitive));
            if (stale) {
                for (const QTextBlock &entryBlock : entryBlocks) {
                    QTextEdit::ExtraSelection selection;
                    selection.cursor = QTextCursor(entryBlock);
                    selection.cursor.select(QTextCursor::LineUnderCursor);
                    selection.format = staleFormat;
                    selections.append(selection);
                }
            }
            entryBlocks.clear();
        };
        for (QTextBlock block = m_logView->document()->begin(); block.isValid(); block = block.next()) {
            if (!entryBlocks.isEmpty() && block.text().startsWith(QStringLiteral("────────"))) {
                flushEntry();
            }
            entryBlocks.append(block);
        }
        flushEntry();
        m_logView->setExtraSelections(selections);
    };

    const QString query = m_logSearchEdit ? m_logSearchEdit->text().trimmed().toCaseFolded() : QString();
    if (query.isEmpty()) {
        m_logView->setPlainText(m_actionLogEntries.join(QLatin1Char('\n')));
        markStaleEntries();
        if (wasAtTop && scrollBar) {
            scrollBar->setValue(0);
        }
        return;
    }

    // Match each whitespace-separated query term inside a single log token.
    // Keeping the subsequence within one token makes fuzzy searches forgiving
    // of punctuation (for example, "intrmfs" still finds "initramfs") while
    // preventing unrelated long entries from matching characters scattered
    // across timestamps, messages, and line breaks.
    const QStringList terms = query.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    const auto matchesTerms = [&terms](const QString &text) {
        const QStringList tokens = text.toCaseFolded().split(
            QRegularExpression(QStringLiteral("[^\\p{L}\\p{N}]+")), Qt::SkipEmptyParts);
        for (const QString &term : terms) {
            bool termMatched = false;
            for (const QString &token : tokens) {
                if (token.contains(term)) {
                    termMatched = true;
                    break;
                }
                int position = 0;
                bool subsequence = true;
                for (const QChar character : term) {
                    position = token.indexOf(character, position);
                    if (position < 0) {
                        subsequence = false;
                        break;
                    }
                    ++position;
                }
                if (subsequence) {
                    termMatched = true;
                    break;
                }
            }
            if (!termMatched) {
                return false;
            }
        }
        return true;
    };

    QStringList visible;
    visible.reserve(m_actionLogEntries.size());
    for (const QString &entry : m_actionLogEntries) {
        const QStringList lines = entry.split(QLatin1Char('\n'));
        const bool diagnosticEntry = entry.contains(QStringLiteral("\nDiagnostic: "))
            || entry.contains(QStringLiteral("] Diagnostic: "));
        if (!diagnosticEntry) {
            if (matchesTerms(entry)) {
                visible.append(entry);
            }
            continue;
        }

        // Diagnostic entries can contain thousands of lines. Show the
        // framing metadata and only matching lines, so a search for
        // “initramfs” does not reproduce the whole report or unrelated
        // surrounding boot messages.
        QList<int> matches;
        for (int index = 0; index < lines.size(); ++index) {
            const QString line = lines.at(index);
            // Some target inspections intentionally show recursive file
            // evidence. Those path-prefixed or very long JSON/config lines
            // are useful in the complete report, but make search results
            // misleading when a common word (such as “update”) appears in
            // embedded metadata like updateURL.
            if (line.size() > 512
                || line.startsWith(QStringLiteral("/run/boot-repair/"))
                || line.startsWith(QStringLiteral("/var/"))
                || line.startsWith(QStringLiteral("/home/"))) {
                continue;
            }
            if (matchesTerms(lines.at(index))) {
                matches.append(index);
            }
        }
        if (matches.isEmpty()) {
            continue;
        }

        QSet<int> included;
        int headerEnd = qMin(lines.size() - 1, 7);
        for (int index = 0; index < lines.size(); ++index) {
            if (lines.at(index).startsWith(QStringLiteral("Status: "))) {
                headerEnd = qMin(lines.size() - 1, index + 1);
                break;
            }
        }
        for (int index = 0; index <= headerEnd; ++index) {
            included.insert(index);
        }
        for (const int match : matches) {
            included.insert(match);
        }

        QStringList excerpt;
        for (int index = 0; index < lines.size(); ++index) {
            if (included.contains(index)) {
                excerpt.append(lines.at(index));
            }
        }
        if (!excerpt.isEmpty()) {
            visible.append(excerpt.join(QLatin1Char('\n')));
        }
    }

    m_logView->setPlainText(visible.join(QLatin1Char('\n')));
    markStaleEntries();
    if (wasAtTop && scrollBar) {
        scrollBar->setValue(0);
    }
}

void MainWindow::appendDiagnosticLog(const QString &key, const QString &title,
                                     const QString &scope, const QString &result,
                                     bool succeeded)
{
    QString output = result.trimmed();
    if (output.isEmpty()) {
        output = QStringLiteral("(diagnostic produced no output)");
    }

    // Keep a self-contained section in the application log for every
    // diagnostic request. The helper's target output already has its own
    // title and metadata, while host output starts at the key/scope header;
    // embedding either form beneath this framing preserves the complete
    // captured text and records exactly what action produced it.
    const QString requestKey = key == QStringLiteral("report") ? QStringLiteral("all") : key;
    const QString command = scope == QStringLiteral("Repair Target")
        ? QStringLiteral("%1 diagnose %2 %3 %4")
            .arg(repairHelperPath().isEmpty() ? QStringLiteral("boot-repair-helper") : repairHelperPath(),
                 m_previewTargetPath, m_previewTargetComponentPath, requestKey)
        : QStringLiteral("Boot Bitch host diagnostic implementation (read-only): %1").arg(requestKey);

    const QString section = QStringLiteral(
        "========================================\n"
        "Diagnostic: %1\n"
        "Title: %2\n"
        "Scope: %3\n"
        "What was actually run: %4\n"
        "Status: %5\n"
        "========================================\n"
        "%6")
        .arg(key)
        .arg(title)
        .arg(scope)
        .arg(command)
        .arg(succeeded ? QStringLiteral("completed") : QStringLiteral("failed"))
        .arg(output);
    appendLog(section, succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));
}

void MainWindow::showUsageHelp()
{
    showCompactHelp(this,
                    QStringLiteral("Using Boot Bitch"),
                    QStringLiteral(
                        "Boot Bitch must run from a different booted Linux environment than the system being repaired. "
                        "Use a Linux live USB or another Linux installation on a different physical drive. "
                        "The running host is protected from ordinary repair-target selection, but it can be explicitly selected through Host Maintenance for guarded native diagnostics and supported maintenance stages. "
                        "Diagnostics can inspect either Running Host or the selected repair drive; choose the scope in Diagnostics → Inspect. "
                        "The first root action requests administrator authorization once for this Boot Bitch window; "
                        "File → Lock Administrator Session ends that helper session immediately."));
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this,
                       QStringLiteral("About Boot Bitch"),
                       QStringLiteral(
                           "<h3>Boot Bitch %1</h3>"
                           "<p><b>Developer:</b> CaptainMorgan12</p>"
                           "<p>A native Qt 6 Linux recovery and boot-repair utility.</p>"
                           "<p><b>Guarded repair mode:</b> read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. Debian/Ubuntu-family repairs can run through a privileged helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.</p>"
                           "<p>The first privileged action authorizes one narrow helper session for the current Boot Bitch window while the Qt GUI remains unprivileged. It can be ended at any time from File → Lock Administrator Session.</p>"
                           "<p>LUKS target unlock, verified bidirectional File Copy, read-only host/repair diagnostics, transactional Btrfs snapshot rollback, offline graphical login recovery, distribution-aware EFI/UKI repair and boot-stack reconciliation are enabled through the guarded helper. Snapshot rollback preserves the previous @ and automatically restores it if critical post-switch reconciliation fails.</p>")
                           .arg(QCoreApplication::applicationVersion()));
}

void MainWindow::updateResponsiveLayout()
{
    const int availableWidth = centralWidget() ? centralWidget()->width() : width();
    const bool narrowSystems = availableWidth < 900;
    const bool narrowDiagnostics = availableWidth < 820;
    const bool narrowRepair = availableWidth < 820;
    const bool narrowSnapshots = availableWidth < 700;

    if (m_tabs && m_tabs->count() >= 8) {
        QStringList labels;
        if (availableWidth < 620) {
            labels = {QStringLiteral("Sys"), QStringLiteral("Diag"), QStringLiteral("Repair"),
                      QStringLiteral("Snaps"), QStringLiteral("Shell"), QStringLiteral("Files"), QStringLiteral("Logs"), QStringLiteral("Set")};
        } else if (availableWidth < 820) {
            labels = {QStringLiteral("Systems"), QStringLiteral("Diag"), QStringLiteral("Repair"),
                      QStringLiteral("Snaps"), QStringLiteral("Shell"), QStringLiteral("File Copy"), QStringLiteral("Logs"), QStringLiteral("Settings")};
        } else {
            labels = {QStringLiteral("Systems"), QStringLiteral("Diagnostics"), QStringLiteral("Repair"),
                      QStringLiteral("Snapshots"), QStringLiteral("Chroot Shell"), QStringLiteral("File Copy"), QStringLiteral("Logs"), QStringLiteral("Settings")};
        }
        for (int i = 0; i < labels.size() && i < m_tabs->count(); ++i) {
            m_tabs->setTabText(i, labels.at(i));
        }
    }

    if (m_systemSplitter) {
        const Qt::Orientation desired = narrowSystems ? Qt::Vertical : Qt::Horizontal;
        const bool changed = m_systemSplitter->orientation() != desired;
        if (changed) {
            m_systemSplitter->setOrientation(desired);
        }

        if (desired == Qt::Vertical) {
            m_systemSplitter->setMinimumHeight(500);
            m_systemSplitter->setStretchFactor(0, 3);
            m_systemSplitter->setStretchFactor(1, 2);
            if (changed) {
                m_systemSplitter->setSizes({300, 220});
            }
        } else {
            m_systemSplitter->setMinimumHeight(310);
            m_systemSplitter->setStretchFactor(0, 7);
            m_systemSplitter->setStretchFactor(1, 3);
            if (changed) {
                m_systemSplitter->setSizes({820, 320});
            }
        }
    }

    if (m_diagnosticSplitter) {
        const Qt::Orientation desired = narrowDiagnostics ? Qt::Vertical : Qt::Horizontal;
        const bool changed = m_diagnosticSplitter->orientation() != desired;
        if (changed) {
            m_diagnosticSplitter->setOrientation(desired);
        }

        if (desired == Qt::Vertical) {
            m_diagnosticSplitter->setMinimumHeight(350);
            m_diagnosticSplitter->setStretchFactor(0, 2);
            m_diagnosticSplitter->setStretchFactor(1, 4);
            if (changed) {
                m_diagnosticSplitter->setSizes({125, 230});
            }
        } else {
            m_diagnosticSplitter->setMinimumHeight(330);
            m_diagnosticSplitter->setStretchFactor(0, 2);
            m_diagnosticSplitter->setStretchFactor(1, 5);
            if (changed) {
                m_diagnosticSplitter->setSizes({280, 760});
            }
        }
    }

    if (m_repairSplitter) {
        const Qt::Orientation desired = narrowRepair ? Qt::Vertical : Qt::Horizontal;
        const bool changed = m_repairSplitter->orientation() != desired;
        if (changed) {
            m_repairSplitter->setOrientation(desired);
        }

        if (desired == Qt::Vertical) {
            m_repairSplitter->setMinimumHeight(420);
            m_repairSplitter->setStretchFactor(0, 1);
            m_repairSplitter->setStretchFactor(1, 1);
            if (changed) {
                m_repairSplitter->setSizes({205, 205});
            }
        } else {
            m_repairSplitter->setMinimumHeight(270);
            m_repairSplitter->setStretchFactor(0, 5);
            m_repairSplitter->setStretchFactor(1, 6);
            if (changed) {
                m_repairSplitter->setSizes({470, 560});
            }
        }
    }

    if (m_snapshotTable) {
        QTimer::singleShot(0, this, [this] { resizeSnapshotRows(); });
    }

    if (m_snapshotButtonLayout) {
        m_snapshotButtonLayout->setDirection(narrowSnapshots
            ? QBoxLayout::TopToBottom
            : QBoxLayout::LeftToRight);
        if (m_snapshotLoadButton && m_snapshotInspectButton && m_snapshotRollbackButton) {
            const QSizePolicy policy = narrowSnapshots
                ? QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed)
                : QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            m_snapshotLoadButton->setSizePolicy(policy);
            m_snapshotInspectButton->setSizePolicy(policy);
            m_snapshotRollbackButton->setSizePolicy(policy);
        }
    }
}

void MainWindow::updateFullRepairSummary()
{
    if (!m_fullRepairDpkg || !m_fullRepairStageList || !m_fullRepairCountLabel) {
        return;
    }

    updateRepairScopeControls();

    struct StageEntry {
        QCheckBox *check;
        QString title;
        QString icon;
        QString diagnosticKey;
    };

    const QList<StageEntry> stageEntries = {
        {m_fullRepairDpkg, QStringLiteral("Complete interrupted package configuration"), QStringLiteral("dialog-ok-apply"), QStringLiteral("dpkg")},
        {m_fullRepairBrokenPackages, QStringLiteral("Repair broken package dependencies"), QStringLiteral("dialog-ok-apply"), QStringLiteral("fixbroken")},
        {m_fullRepairAptUpdate, QStringLiteral("Refresh package metadata"), QStringLiteral("view-refresh"), QStringLiteral("aptupdate")},
        {m_fullRepairUpgrade, QStringLiteral("Upgrade installed packages"), QStringLiteral("system-software-update"), QStringLiteral("upgrade")},
        {m_fullRepairDkms, QStringLiteral("Rebuild DKMS modules"), QStringLiteral("applications-development"), QStringLiteral("dkms")},
        {m_fullRepairDisplayManager, QStringLiteral("Restore detected graphical login manager"), QStringLiteral("video-display"), QStringLiteral("display")},
        {m_fullRepairInitramfs, QStringLiteral("Rebuild initramfs after mapper/crypttab validation"), QStringLiteral("view-refresh"), QStringLiteral("initramfs")},
        {m_fullRepairEfi, QStringLiteral("Reinstall EFI bootloader"), QStringLiteral("drive-removable-media"), QStringLiteral("efi")},
        {m_fullRepairGrub, QStringLiteral("Update GRUB configuration"), QStringLiteral("preferences-system"), QStringLiteral("grub")}
    };

    m_fullRepairStageList->clear();
    int selectedCount = 0;

    for (const StageEntry &entry : stageEntries) {
        if (!entry.check || !entry.check->isChecked()) {
            continue;
        }
        ++selectedCount;
        const QString orderedTitle = QStringLiteral("%1. %2").arg(selectedCount).arg(entry.title);
        auto *item = new QListWidgetItem(themedIcon(entry.icon), orderedTitle, m_fullRepairStageList);
        item->setToolTip(QStringLiteral("Execution order %1: %2").arg(selectedCount).arg(entry.title));
    }


    if (selectedCount == 0) {
        m_fullRepairCountLabel->setText(QStringLiteral("No stages selected"));
        auto *item = new QListWidgetItem(themedIcon(QStringLiteral("dialog-information")),
                                         QStringLiteral("No Full Repair stages selected — use Configure Plan… or Settings."),
                                         m_fullRepairStageList);
        item->setToolTip(item->text());
    } else {
        m_fullRepairCountLabel->setText(selectedCount == 1
            ? QStringLiteral("1 stage selected")
            : QStringLiteral("%1 stages selected").arg(selectedCount));
    }

    const int planRows = qBound(2, selectedCount > 0 ? selectedCount : 1, 9);
    const int planRowHeight = qMax(28, m_fullRepairStageList->fontMetrics().lineSpacing() + 12);
    // Keep the plan compact enough to leave room for the individual tools;
    // additional stages remain available through the list's own scrollbar.
    m_fullRepairStageList->setFixedHeight(qMin(6 * planRowHeight + 8, planRows * planRowHeight + 8));

    if (m_runFullRepairButton) {
        QString reason;
        const bool targetReady = m_hostMaintenanceMode
            ? hostMaintenanceReady(&reason)
            : repairTargetReady(&reason);
        QString diagnosticReason;
        bool diagnosticsReady = targetReady;
        if (diagnosticsReady) {
            for (const StageEntry &entry : stageEntries) {
                if (!entry.check || !entry.check->isChecked()) continue;
                QString stageReason;
                if (!repairEvidenceReadyForTool(entry.diagnosticKey, &stageReason)) {
                    diagnosticsReady = false;
                    diagnosticReason = stageReason;
                    break;
                }
            }
        }
        m_runFullRepairButton->setEnabled(selectedCount > 0 && targetReady && diagnosticsReady);
        if (m_fullRepairReadinessLabel) {
            QString readiness;
            if (selectedCount == 0) {
                readiness = QStringLiteral("No repair stages are selected. Use Configure Plan… to choose the stages Full Repair will run.");
            } else if (!targetReady) {
                readiness = reason;
            } else if (!diagnosticsReady) {
                readiness = diagnosticReason;
                if (m_targetDiagnosticsNeedRegeneration
                    && !readiness.contains(QStringLiteral("regenerate diagnostics"), Qt::CaseInsensitive)) {
                    readiness += QStringLiteral(" Please regenerate diagnostics before starting another repair.");
                }
            } else {
                readiness = QStringLiteral("Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.");
            }
            m_fullRepairReadinessLabel->setText(readiness);
        }
        if (selectedCount == 0) {
            m_runFullRepairButton->setToolTip(QStringLiteral("No Full Repair stages are selected."));
        } else if (!targetReady) {
            m_runFullRepairButton->setToolTip(reason);
        } else if (!diagnosticsReady) {
            m_runFullRepairButton->setToolTip(diagnosticReason);
        } else {
            m_runFullRepairButton->setToolTip(QStringLiteral(
                "Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation."));
        }
    }

    if (m_repairToolTree) {
        for (int row = 0; row < m_repairToolTree->topLevelItemCount(); ++row) {
            QTreeWidgetItem *item = m_repairToolTree->topLevelItem(row);
            const QString key = item->data(0, Qt::UserRole).toString();
            QString status;

            auto planStatus = [](QCheckBox *check) {
                return check && check->isChecked()
                    ? QStringLiteral("Enabled in Settings")
                    : QStringLiteral("Disabled in Settings");
            };

            if (key == QStringLiteral("validate")) {
                status = QStringLiteral("Always preflight");
            } else if (key == QStringLiteral("dpkg")) {
                status = planStatus(m_fullRepairDpkg);
            } else if (key == QStringLiteral("fixbroken")) {
                status = planStatus(m_fullRepairBrokenPackages);
            } else if (key == QStringLiteral("aptupdate")) {
                status = planStatus(m_fullRepairAptUpdate);
            } else if (key == QStringLiteral("upgrade")) {
                status = planStatus(m_fullRepairUpgrade);
            } else if (key == QStringLiteral("dkms")) {
                status = planStatus(m_fullRepairDkms);
            } else if (key == QStringLiteral("display")) {
                status = planStatus(m_fullRepairDisplayManager);
            } else if (key == QStringLiteral("initramfs")) {
                status = planStatus(m_fullRepairInitramfs);
            } else if (key == QStringLiteral("efi")) {
                status = planStatus(m_fullRepairEfi);
            } else if (key == QStringLiteral("grub")) {
                status = planStatus(m_fullRepairGrub);
            } else if (key == QStringLiteral("bootstack")) {
                status = QStringLiteral("Manual recovery tool");
            }

            item->setText(1, status);
            item->setToolTip(1, status);
        }
    }

    updateRepairToolDetails();
}

void MainWindow::updateRepairToolDetails()
{
    if (!m_repairToolTree || !m_repairToolTitle || !m_repairToolDescription
        || !m_repairToolPlanStatus || !m_repairToolButton) {
        return;
    }

    QTreeWidgetItem *item = m_repairToolTree->currentItem();
    if (!item) {
        m_repairToolTitle->setText(QStringLiteral("Select a repair tool"));
        m_repairToolDescription->setText(QStringLiteral("Select a tool to review its workflow."));
        m_repairToolPlanStatus->clear();
        m_repairToolButton->setText(QStringLiteral("Run Tool"));
        return;
    }

    const QString key = item->data(0, Qt::UserRole).toString();
    const QString plan = item->text(1);
    QString title;
    QString description;
    QString buttonText;
    QString iconName;
    QString planText;

    if (key == QStringLiteral("validate")) {
        title = QStringLiteral("Validate environment");
        description = QStringLiteral(
            "Check the running host or selected repair system's mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. "
            "This is an independent safety preflight rather than an optional Full Repair stage.");
        buttonText = QStringLiteral("Validate");
        iconName = QStringLiteral("task-complete");
        planText = QStringLiteral("Full Repair: automatic safety preflight");
    } else if (key == QStringLiteral("dpkg")) {
        title = QStringLiteral("Complete package configuration");
        description = QStringLiteral(
            "Complete interrupted dpkg package configuration in the running host or selected repair system. This is the same stage controlled by Settings → Full Repair plan → Complete interrupted package configuration, but it can also be run independently here.");
        buttonText = QStringLiteral("Complete Configuration");
        iconName = QStringLiteral("dialog-ok-apply");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("fixbroken")) {
        title = QStringLiteral("Repair broken dependencies");
        description = QStringLiteral(
            "Run APT's broken-dependency repair in the running host or selected repair system after the mandatory safety preflight. This maps directly to Settings → Repair broken package dependencies.");
        buttonText = QStringLiteral("Repair Dependencies");
        iconName = QStringLiteral("dialog-ok-apply");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("aptupdate")) {
        title = QStringLiteral("Refresh package metadata");
        description = QStringLiteral(
            "Refresh the running host or selected repair system's APT package metadata without upgrading installed packages. This maps directly to Settings → Refresh package metadata.");
        buttonText = QStringLiteral("Refresh Metadata");
        iconName = QStringLiteral("view-refresh");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("upgrade")) {
        title = QStringLiteral("Upgrade installed packages");
        description = QStringLiteral(
            "Simulate APT transactions first, inspect distribution feedback and proposed removals, then automatically choose a safe upgrade, full-upgrade, or dist-upgrade operation. This maps directly to Settings → Upgrade installed packages.");
        buttonText = QStringLiteral("Simulate and Upgrade");
        iconName = QStringLiteral("system-software-update");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("dkms")) {
        title = QStringLiteral("DKMS");
        description = QStringLiteral(
            "Rebuild out-of-tree kernel modules for kernels installed in the running host or selected repair system. The helper refuses this action when DKMS is not installed in the selected system.");
        buttonText = QStringLiteral("Rebuild DKMS");
        iconName = QStringLiteral("applications-development");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("display")) {
        title = QStringLiteral("Graphical login / display manager");
        description = QStringLiteral(
            "Restore the display manager identified from the running host or selected repair system's boot evidence and configuration (for example SDDM, GDM3, LightDM, or another systemd manager), set graphical.target as the default, and repair display-manager.service. "
            "Boot Bitch deliberately does not start a graphical session inside a repair chroot or host helper; use Diagnostics to inspect installed packages, service configuration, and recent boot/journal evidence first when graphical boot fails.");
        buttonText = QStringLiteral("Restore Graphical Login");
        iconName = QStringLiteral("video-display");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("initramfs")) {
        title = QStringLiteral("Initramfs");
        description = QStringLiteral(
            "Rebuild initramfs images for the running host or selected repair system only after mapper and crypttab consistency checks pass. The privileged helper enforces this safety gate before running update-initramfs.");
        buttonText = QStringLiteral("Rebuild Initramfs");
        iconName = QStringLiteral("view-refresh");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("efi")) {
        title = QStringLiteral("EFI / UKI bootloader");
        description = QStringLiteral(
            "Repair the running host or selected repair system's EFI / UKI boot path. On current TUXEDO Debian-base systems with create_boot_uki_base.sh, Boot Bitch uses the vendor UKI builder and preserves every other ESP's firmware entries and BootOrder; TUXEDO Ubuntu layouts without that builder continue to use their GRUB path. On conventional GRUB EFI systems it performs a guarded grub-install on the selected system's validated ESP. Afterward, decoded entries on each maintained ESP retain their distribution/vendor label and receive that drive's model once; an existing model name is not duplicated. The maintained BootOrder groups each drive's primary loader, fallback and WebFAI destinations and removes only entries that resolve to a duplicate destination, such as a device-path-only UEFI fallback beside BOOTX64.EFI. Unrelated EFI entries on other disks are never removed.");
        buttonText = QStringLiteral("Repair EFI / UKI");
        iconName = QStringLiteral("drive-removable-media");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("grub")) {
        title = QStringLiteral("GRUB configuration");
        description = QStringLiteral(
            "Regenerate the running host or selected repair system's GRUB menu/configuration after the mandatory safety preflight. "
            "This does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.");
        buttonText = QStringLiteral("Regenerate GRUB");
        iconName = QStringLiteral("preferences-system");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("bootstack")) {
        title = QStringLiteral("Boot stack reconciliation");
        description = QStringLiteral(
            "Reconcile a repaired or restored root with its boot artifacts: validate mapper/crypttab, rebuild installed-kernel initramfs images, rebuild the TUXEDO UKI when the selected system provides its official builder, reconcile one canonical EFI destination per purpose, and regenerate the GRUB fallback. "
            "This is the focused recovery action for a root/EFI mismatch after a partial update or snapshot restore; it does not delete kernels or unrelated ESP entries.");
        buttonText = QStringLiteral("Reconcile Boot Stack");
        iconName = QStringLiteral("system-run");
        planText = QStringLiteral("Full Repair plan: Manual recovery tool");
    } else {
        title = QStringLiteral("Unknown repair tool");
        description = QStringLiteral("No repair workflow is registered for this item.");
        buttonText = QStringLiteral("Unavailable");
        iconName = QStringLiteral("dialog-error");
        planText = QStringLiteral("Full Repair plan: unavailable");
    }

    m_repairToolTitle->setText(title);
    m_repairToolDescription->setText(description);
    m_repairToolPlanStatus->setText(planText);
    m_repairToolButton->setText(buttonText);
    m_repairToolButton->setIcon(themedIcon(iconName));
    const QSize buttonHint = m_repairToolButton->sizeHint();
    m_repairToolButton->setMinimumWidth(buttonHint.width());
    m_repairToolButton->setMinimumHeight(qMax(buttonHint.height(), fontMetrics().height() + 14));
    QString reason;
    const bool targetReady = m_hostMaintenanceMode
        ? hostMaintenanceReady(&reason)
        : repairTargetReady(&reason);
    QString diagnosticReason;
    const bool diagnosticsReady = repairEvidenceReadyForTool(
        m_repairToolTree && m_repairToolTree->currentItem()
            ? m_repairToolTree->currentItem()->data(0, Qt::UserRole).toString()
            : QString(),
        &diagnosticReason);
    m_repairToolButton->setEnabled(targetReady && diagnosticsReady);
    QString displayedPlanStatus = planText;
    if (!targetReady || !diagnosticsReady) {
        displayedPlanStatus += QStringLiteral("\nUnavailable: ")
            + (!targetReady ? reason : diagnosticReason);
    }
    m_repairToolPlanStatus->setText(displayedPlanStatus);
    m_repairToolButton->setToolTip(!targetReady
        ? reason
        : (!diagnosticsReady
            ? diagnosticReason
            : QStringLiteral("Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.")));
}

QString MainWindow::repairHelperPath() const
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        QDir(appDir).absoluteFilePath(QStringLiteral("../scripts/boot-repair-helper.sh")),
        QDir(appDir).absoluteFilePath(QStringLiteral("scripts/boot-repair-helper.sh")),
        // Source-tree builds are often launched from the checkout while the
        // binary lives in a temporary build directory.  Resolve the helper
        // from the working directory as well so guarded UI preflight remains
        // testable and useful before installation.
        QDir::current().absoluteFilePath(QStringLiteral("scripts/boot-repair-helper.sh")),
#ifdef BOOT_REPAIR_SOURCE_DIR
        QStringLiteral(BOOT_REPAIR_SOURCE_DIR "/scripts/boot-repair-helper.sh"),
#endif
        // AppImage/CMake AppDir layout: usr/bin/boot-repair beside
        // usr/libexec/boot-repair/boot-repair-helper.
        QDir(appDir).absoluteFilePath(QStringLiteral("../libexec/boot-repair/boot-repair-helper")),
        QStringLiteral("/usr/libexec/boot-repair/boot-repair-helper"),
        QStringLiteral("/usr/lib/boot-repair/boot-repair-helper")
    };

    for (const QString &candidate : candidates) {
        QFileInfo info(candidate);
        // Web-uploaded source trees may lose the executable bit on shell
        // helpers.  Accept a readable .sh here; the session launcher invokes
        // it explicitly through /bin/bash below.
        if (info.exists() && info.isFile()
            && (info.isExecutable() || info.suffix().compare(QStringLiteral("sh"), Qt::CaseInsensitive) == 0)) {
            return info.canonicalFilePath().isEmpty() ? info.absoluteFilePath() : info.canonicalFilePath();
        }
    }
    return QString();
}

QStringList MainWindow::selectedRepairStages() const
{
    QStringList stages;
    if (m_fullRepairDpkg && m_fullRepairDpkg->isChecked()) stages << QStringLiteral("dpkg-configure");
    if (m_fullRepairBrokenPackages && m_fullRepairBrokenPackages->isChecked()) stages << QStringLiteral("fix-broken");
    if (m_fullRepairAptUpdate && m_fullRepairAptUpdate->isChecked()) stages << QStringLiteral("apt-update");
    if (m_fullRepairUpgrade && m_fullRepairUpgrade->isChecked()) stages << QStringLiteral("apt-upgrade");
    if (m_fullRepairDkms && m_fullRepairDkms->isChecked()) stages << QStringLiteral("dkms");
    if (m_fullRepairDisplayManager && m_fullRepairDisplayManager->isChecked()) stages << QStringLiteral("display-manager");
    if (m_fullRepairInitramfs && m_fullRepairInitramfs->isChecked()) stages << QStringLiteral("initramfs");
    if (m_fullRepairEfi && m_fullRepairEfi->isChecked()) stages << QStringLiteral("efi");
    if (m_fullRepairGrub && m_fullRepairGrub->isChecked()) stages << QStringLiteral("grub");
    return stages;
}

void MainWindow::updateRepairScopeControls()
{
    const QList<QCheckBox *> allStages = {
        m_fullRepairDpkg, m_fullRepairBrokenPackages, m_fullRepairAptUpdate,
        m_fullRepairUpgrade, m_fullRepairDkms, m_fullRepairDisplayManager,
        m_fullRepairInitramfs, m_fullRepairEfi, m_fullRepairGrub
    };
    for (QCheckBox *check : allStages) {
        if (!check) {
            continue;
        }
        check->setEnabled(true);
        check->setToolTip(m_hostMaintenanceMode
            ? QStringLiteral("Allowed for the explicitly selected running host. The helper repeats host identity, mount, package-lock and boot preservation checks.")
            : QString());
    }
}

bool MainWindow::repairTargetReady(QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    if (m_previewTargetPath.isEmpty() || m_previewTargetComponentPath.isEmpty()) {
        setReason(QStringLiteral("Select a repair target in Systems first."));
        return false;
    }

    const DeviceNode disk = m_deviceIndex.value(m_previewTargetPath);
    const DeviceNode component = m_deviceIndex.value(m_previewTargetComponentPath);
    if (disk.protectedDevice || component.protectedDevice) {
        setReason(QStringLiteral("The running host is protected and cannot be repaired from itself."));
        return false;
    }
    if (component.encrypted || component.fileSystem.compare(QStringLiteral("crypto_LUKS"), Qt::CaseInsensitive) == 0) {
        setReason(QStringLiteral("The selected Linux root is still LUKS-encrypted. Unlock it first, refresh devices, then select the mapped filesystem."));
        return false;
    }
    if (!component.linuxCapableFileSystem && !component.installedLinux) {
        setReason(QStringLiteral("No mountable Linux root filesystem has been identified on the selected target."));
        return false;
    }

    if (repairHelperPath().isEmpty()) {
        setReason(QStringLiteral("The privileged Boot Bitch helper was not found. Rebuild or install this source tree."));
        return false;
    }

#ifdef Q_OS_UNIX
    if (geteuid() != 0 && QStandardPaths::findExecutable(QStringLiteral("pkexec")).isEmpty()) {
        setReason(QStringLiteral("pkexec/Polkit is required to authorize repair operations."));
        return false;
    }
#endif

    setReason(QStringLiteral("Ready"));
    return true;
}

bool MainWindow::hostBootTargetReady(QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    if (m_hostPrimaryPath.isEmpty() || m_hostPrimaryComponentPath.isEmpty()
        || !m_deviceIndex.contains(m_hostPrimaryPath)
        || !m_deviceIndex.contains(m_hostPrimaryComponentPath)) {
        setReason(QStringLiteral("The running host disk and root component could not be resolved."));
        return false;
    }

    const DeviceNode disk = m_deviceIndex.value(m_hostPrimaryPath);
    const DeviceNode component = m_deviceIndex.value(m_hostPrimaryComponentPath);
    if (!disk.protectedDevice || !component.protectedDevice) {
        setReason(QStringLiteral("The selected host identity is no longer marked as the protected running system. Refresh devices before retrying."));
        return false;
    }
    if (repairHelperPath().isEmpty()) {
        setReason(QStringLiteral("The privileged Boot Bitch helper was not found. Rebuild or install this source tree."));
        return false;
    }

#ifdef Q_OS_UNIX
    if (geteuid() != 0 && QStandardPaths::findExecutable(QStringLiteral("pkexec")).isEmpty()) {
        setReason(QStringLiteral("pkexec/Polkit is required to authorize host maintenance."));
        return false;
    }
#endif

    setReason(QStringLiteral("Running host is ready for explicit guarded maintenance."));
    return true;
}

bool MainWindow::hostMaintenanceReady(QString *reason) const
{
    if (!m_hostMaintenanceMode) {
        if (reason) {
            *reason = QStringLiteral("Select Host Maintenance on the protected running-host card first.");
        }
        return false;
    }
    return hostBootTargetReady(reason);
}

bool MainWindow::confirmRepairAction(const QString &title, const QStringList &operations)
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(title);
    if (m_hostMaintenanceMode) {
        box.setText(QStringLiteral("Confirm changes to the running host"));
        box.setInformativeText(QStringLiteral(
            "Host disk: %1\nLinux root: %2\n\nThe following guarded changes will run natively on the active system:\n• %3\n\n"
            "The helper independently re-checks that the selected disk backs /, /boot and /boot/efi, and applies the same package, mapper, EFI/NVRAM and GRUB preservation safeguards used for repair targets."
        ).arg(m_hostPrimaryPath,
              m_hostPrimaryComponentPath,
              operations.join(QStringLiteral("\n• "))));
    } else {
        box.setText(QStringLiteral("Confirm changes to the selected repair system"));
        box.setInformativeText(QStringLiteral(
            "Target disk: %1\nLinux root: %2\n\nThe following target-system changes will run:\n• %3\n\n"
            "The running host is independently re-checked and refused by the privileged helper."
        ).arg(m_previewTargetPath,
              m_previewTargetComponentPath,
              operations.join(QStringLiteral("\n• "))));
    }
    box.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    box.setDefaultButton(QMessageBox::Cancel);
    box.button(QMessageBox::Yes)->setText(QStringLiteral("Run Repair"));
    return box.exec() == QMessageBox::Yes;
}

void MainWindow::runRepairHelper(const QString &title, const QStringList &arguments)
{
    QString reason;
    const bool hostMode = m_hostMaintenanceMode;
    if (!(hostMode ? hostMaintenanceReady(&reason) : repairTargetReady(&reason))) {
        QMessageBox::warning(this, QStringLiteral("Repair unavailable"), reason);
        return;
    }

    const QString diskPath = hostMode ? m_hostPrimaryPath : m_previewTargetPath;
    const QString componentPath = hostMode ? m_hostPrimaryComponentPath : m_previewTargetComponentPath;
    appendLog(QStringLiteral("Starting privileged action: %1 on %2 (%3).")
                  .arg(title, diskPath, componentPath));

    const bool targetModified = !arguments.isEmpty()
        && (arguments.first() == QStringLiteral("repair")
            || (arguments.first() == QStringLiteral("copy")
                && arguments.value(3) == QStringLiteral("host-to-repair")));
    bool processSucceeded = false;
    QStringList helperArguments = arguments;
    if (hostMode && !helperArguments.isEmpty()
        && helperArguments.first() == QStringLiteral("repair")) {
        helperArguments[0] = QStringLiteral("host-repair");
        helperArguments[1] = m_hostPrimaryPath;
        helperArguments[2] = m_hostPrimaryComponentPath;
    }
    const QString helperOutput = runPrivilegedRequest(title, helperArguments, QByteArray(), &processSucceeded);

    // Persist the helper transcript in the action register. The progress
    // dialog is transient, while Logs must retain the complete stage output
    // for both successful and failed repairs and allow it to be searched.
    const QString detail = helperOutput.trimmed().isEmpty()
        ? QStringLiteral("The privileged helper returned no diagnostic output.")
        : helperOutput.trimmed();
    appendLog(QStringLiteral("Repair output\nDiagnostic: %1\n%2").arg(title, detail),
              processSucceeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"));

    if (!processSucceeded) {
        if (targetModified) {
            // A failed modifying workflow may have completed an earlier stage
            // before stopping. Treat its evidence as stale until a fresh
            // diagnostic proves the resulting state in the active scope.
            if (hostMode) {
                m_hostDiagnosticCache.clear();
                m_hostDiagnosticTimes.clear();
            } else {
                clearTargetDiagnosticCache();
                m_targetDiagnosticsNeedRegeneration = true;
            }
            updateFullRepairSummary();
            appendLog(QStringLiteral("STALE DIAGNOSTICS: repair failed after a modifying action. Regenerate diagnostics before the next repair."),
                      QStringLiteral("ERROR"));
        }
        QMessageBox::critical(this, QStringLiteral("%1 failed").arg(title),
                              QStringLiteral("The repair action failed. Review the captured helper output in Logs for the failing stage and its reason."));
    }

    if (processSucceeded) {
        if (targetModified) {
            if (hostMode) {
                m_hostDiagnosticCache.clear();
                m_hostDiagnosticTimes.clear();
            } else {
                clearTargetDiagnosticCache();
                m_targetDiagnosticsNeedRegeneration = true;
            }
            appendLog(QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action. Regenerate diagnostics before the next repair."));
        }
        refreshDevices();
        if (targetModified) {
            updateDiagnosticDetails();
            // The repair action may have been launched while the Repair tab
            // was visible. Refresh both aggregate and individual controls so
            // no button can retain an enabled state backed by now-stale
            // evidence; the readiness text explains the required rerun.
            updateFullRepairSummary();
        }
    }
}

void MainWindow::runSelectedRepairTool()
{
    QTreeWidgetItem *item = m_repairToolTree ? m_repairToolTree->currentItem() : nullptr;
    if (!item) {
        return;
    }

    const QString key = item->data(0, Qt::UserRole).toString();
    QString diagnosticReason;
    if (!repairEvidenceReadyForTool(key, &diagnosticReason)) {
        QMessageBox::warning(this, QStringLiteral("Repair diagnostics required"), diagnosticReason);
        return;
    }
    if (key == QStringLiteral("validate")) {
        if (m_hostMaintenanceMode) {
            runRepairHelper(QStringLiteral("Validate running host"),
                            {QStringLiteral("host-validate"), m_hostPrimaryPath, m_hostPrimaryComponentPath});
            return;
        }
        runRepairHelper(QStringLiteral("Validate repair target"),
                        {QStringLiteral("validate"), m_previewTargetPath, m_previewTargetComponentPath});
        return;
    }

    QStringList stages;
    QStringList operations;
    QString title;
    const QString selectedSystemLabel = m_hostMaintenanceMode
        ? QStringLiteral("running host")
        : QStringLiteral("selected repair system");
    if (key == QStringLiteral("dpkg")) {
        title = QStringLiteral("Complete package configuration");
        stages = {QStringLiteral("dpkg-configure")};
        operations = {QStringLiteral("Complete interrupted package configuration in the %1 with dpkg --configure -a").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("fixbroken")) {
        title = QStringLiteral("Repair broken dependencies");
        stages = {QStringLiteral("fix-broken")};
        operations = {QStringLiteral("Repair broken APT package dependencies in the %1").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("aptupdate")) {
        title = QStringLiteral("Refresh package metadata");
        stages = {QStringLiteral("apt-update")};
        operations = {QStringLiteral("Refresh APT package metadata in the %1").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("upgrade")) {
        title = QStringLiteral("Upgrade installed packages");
        stages = {QStringLiteral("apt-upgrade")};
        operations = {QStringLiteral("Simulate upgrade modes first, then choose a safe transaction for the %1").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("dkms")) {
        title = QStringLiteral("Rebuild DKMS");
        stages = {QStringLiteral("dkms")};
        operations = {QStringLiteral("Run DKMS autoinstall in the %1").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("display")) {
        title = QStringLiteral("Restore detected graphical login manager");
        stages = {QStringLiteral("display-manager")};
        operations = {QStringLiteral("Set graphical.target as the %1's default boot target").arg(selectedSystemLabel),
                      QStringLiteral("Enable the detected display manager and repair display-manager.service without starting it inside a chroot or host helper")};
    } else if (key == QStringLiteral("initramfs")) {
        title = QStringLiteral("Rebuild initramfs");
        stages = {QStringLiteral("initramfs")};
        operations = {QStringLiteral("Rebuild all initramfs images for the %1").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("efi")) {
        title = QStringLiteral("Repair EFI / UKI bootloader");
        stages = {QStringLiteral("efi")};
        operations = {QStringLiteral("Use the TUXEDO vendor UKI builder when that layout is detected for the %1, preserving every other firmware entry and BootOrder").arg(selectedSystemLabel),
                      QStringLiteral("Otherwise reinstall GRUB EFI loader files only on the %1's validated ESP").arg(selectedSystemLabel),
                      QStringLiteral("Regenerate GRUB configuration afterward")};
    } else if (key == QStringLiteral("grub")) {
        title = QStringLiteral("Regenerate GRUB configuration");
        stages = {QStringLiteral("grub")};
        operations = {QStringLiteral("Regenerate the %1's GRUB configuration").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("bootstack")) {
        title = QStringLiteral("Reconcile boot stack");
        stages = {QStringLiteral("boot-stack")};
        operations = {QStringLiteral("Validate mapper/crypttab against the %1").arg(selectedSystemLabel),
                      QStringLiteral("Rebuild initramfs for installed kernels"),
                      QStringLiteral("Rebuild the TUXEDO UKI with the selected system's official builder when available and remove only duplicate EFI destinations"),
                      QStringLiteral("Regenerate the GRUB fallback configuration")};
    } else {
        QMessageBox::warning(this, QStringLiteral("Repair tool unavailable"),
                             QStringLiteral("No repair workflow is registered for the selected tool."));
        return;
    }

    const QString evidence = m_hostMaintenanceMode
        ? m_hostDiagnosticCache.value(QStringLiteral("report"))
        : m_targetDiagnosticCache.value(QStringLiteral("report"));
    appendLog(QStringLiteral("Using cached read-only diagnostic evidence for %1 (%2 bytes).")
                  .arg(title)
                  .arg(evidence.toUtf8().size()));
    operations.prepend(QStringLiteral("Read-only diagnostics completed; review the full evidence in Logs before confirming repair."));

    if (!confirmRepairAction(title, operations)) {
        return;
    }

    QStringList arguments = {QStringLiteral("repair"), m_previewTargetPath, m_previewTargetComponentPath};
    arguments.append(stages);
    runRepairHelper(title, arguments);
}

void MainWindow::runFullRepair()
{
    const QStringList stages = selectedRepairStages();
    if (stages.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Full Repair"), QStringLiteral("No Full Repair stages are selected."));
        return;
    }

    QString diagnosticReason;
    if (m_hostMaintenanceMode) {
        QString hostReason;
        if (!hostMaintenanceReady(&hostReason)) {
            QMessageBox::warning(this, QStringLiteral("Host maintenance unavailable"), hostReason);
            return;
        }
        if (m_hostDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Repair diagnostics required"),
                                 QStringLiteral("Run All diagnostics for the protected running host before starting maintenance."));
            return;
        }
    } else if (!targetDiagnosticEvidenceReady(&diagnosticReason)) {
        QMessageBox::warning(this, QStringLiteral("Repair diagnostics required"), diagnosticReason);
        return;
    }

    QStringList operations;
    const QString selectedSystemLabel = m_hostMaintenanceMode
        ? QStringLiteral("running host")
        : QStringLiteral("selected repair system");
    for (const QString &stage : stages) {
        if (stage == QStringLiteral("dpkg-configure")) operations << QStringLiteral("Complete interrupted package configuration");
        else if (stage == QStringLiteral("fix-broken")) operations << QStringLiteral("Repair broken APT dependencies");
        else if (stage == QStringLiteral("apt-update")) operations << QStringLiteral("Refresh APT package metadata");
        else if (stage == QStringLiteral("apt-upgrade")) operations << QStringLiteral("Simulate APT first, then choose a safe upgrade/full-upgrade/dist-upgrade transaction");
        else if (stage == QStringLiteral("dkms")) operations << QStringLiteral("Rebuild DKMS modules");
        else if (stage == QStringLiteral("display-manager")) operations << QStringLiteral("Restore the detected display manager and graphical.target without starting the GUI inside chroot");
        else if (stage == QStringLiteral("initramfs")) operations << QStringLiteral("Rebuild all initramfs images");
        else if (stage == QStringLiteral("efi")) operations << QStringLiteral("Repair the %1 EFI / UKI boot path using its validated ESP").arg(selectedSystemLabel);
        else if (stage == QStringLiteral("grub")) operations << QStringLiteral("Regenerate the %1's GRUB configuration").arg(selectedSystemLabel);
    }

    const QString evidence = m_hostMaintenanceMode
        ? m_hostDiagnosticCache.value(QStringLiteral("report"))
        : m_targetDiagnosticCache.value(QStringLiteral("report"));
    appendLog(QStringLiteral("Using cached read-only diagnostic evidence for Full Repair (%1 bytes).")
                  .arg(evidence.toUtf8().size()));
    operations.prepend(QStringLiteral("Read-only diagnostics completed; review the full evidence in Logs before confirming repair."));

    if (!confirmRepairAction(QStringLiteral("Run Full Repair"), operations)) {
        return;
    }

    QStringList arguments = {QStringLiteral("repair"), m_previewTargetPath, m_previewTargetComponentPath};
    arguments.append(stages);
    runRepairHelper(QStringLiteral("Full Repair"), arguments);
}

void MainWindow::setLogWrapEnabled(bool enabled)
{
    if (!m_logView) {
        return;
    }

    m_logView->setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    m_logView->setWordWrapMode(enabled
        ? QTextOption::WrapAtWordBoundaryOrAnywhere
        : QTextOption::NoWrap);
    m_logView->setHorizontalScrollBarPolicy(enabled
        ? Qt::ScrollBarAlwaysOff
        : Qt::ScrollBarAsNeeded);
}

void MainWindow::autoSizeDeviceColumns()
{
    if (!m_deviceTree) {
        return;
    }

    static const int maximumWidths[] = {360, 420, 120, 110, 150, 260};
    for (int column = 0; column < m_deviceTree->columnCount(); ++column) {
        m_deviceTree->resizeColumnToContents(column);
        if (m_deviceTree->columnWidth(column) > maximumWidths[column]) {
            m_deviceTree->setColumnWidth(column, maximumWidths[column]);
        }
    }
    statusBar()->showMessage(QStringLiteral("Device columns auto-sized. Drag headers to fine-tune widths."), 3500);
}

void MainWindow::applyDeviceColumnDefaults()
{
    if (!m_deviceTree) {
        return;
    }

    const int widths[] = {240, 285, 95, 90, 115, 175};
    for (int column = 0; column < m_deviceTree->columnCount(); ++column) {
        m_deviceTree->setColumnWidth(column, widths[column]);
    }
}

void MainWindow::updateDeviceTreeHeight()
{
    if (!m_deviceTree) {
        return;
    }

    int visibleRows = 0;
    auto countVisibleChildren = [&](auto &&self, QTreeWidgetItem *item) -> int {
        if (!item || !item->isExpanded()) {
            return 0;
        }
        int count = item->childCount();
        for (int i = 0; i < item->childCount(); ++i) {
            count += self(self, item->child(i));
        }
        return count;
    };

    for (int i = 0; i < m_deviceTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_deviceTree->topLevelItem(i);
        ++visibleRows;
        visibleRows += countVisibleChildren(countVisibleChildren, item);
    }

    const int cappedRows = qBound(2, visibleRows, 8);
    const int rowHeight = qMax(34, m_deviceTree->fontMetrics().lineSpacing() * 2 + 8);
    const int headerHeight = qMax(m_deviceTree->header()->sizeHint().height(), m_deviceTree->fontMetrics().height() + 10);
    const int horizontalGutter = m_deviceTree->horizontalScrollBar()
        ? m_deviceTree->horizontalScrollBar()->sizeHint().height()
        : 0;
    const int targetHeight = headerHeight + cappedRows * rowHeight
        + horizontalGutter + m_deviceTree->frameWidth() * 2 + 6;

    m_deviceTree->setFixedHeight(targetHeight);
    m_deviceTree->setVerticalScrollBarPolicy(visibleRows > 8 ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
}

void MainWindow::normalizeButtonSizing()
{
    const QList<QPushButton *> pushButtons = findChildren<QPushButton *>();
    for (QPushButton *button : pushButtons) {
        const QSize hint = button->minimumSizeHint().expandedTo(button->sizeHint());
        button->setMinimumHeight(qMax(hint.height(), fontMetrics().height() + 14));
        button->setMinimumWidth(hint.width());
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }

    const QList<QToolButton *> toolButtons = findChildren<QToolButton *>();
    for (QToolButton *button : toolButtons) {
        // QLineEdit owns its clear button and positions it internally. Global
        // minimum-height normalization makes that child hug the bottom edge
        // of tall fields instead of staying vertically centered.
        if (qobject_cast<QLineEdit *>(button->parentWidget())) {
            continue;
        }
        const QSize hint = button->minimumSizeHint().expandedTo(button->sizeHint());
        button->setMinimumHeight(qMax(hint.height(), fontMetrics().height() + 10));
        button->setMinimumWidth(hint.width());
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
}

bool MainWindow::devicePassesTopLevelFilters(const DeviceNode &device) const
{
    const bool removableOrUsb = device.removable
        || device.transport.compare(QStringLiteral("usb"), Qt::CaseInsensitive) == 0;
    if (m_showRemovable && !m_showRemovable->isChecked() && removableOrUsb) {
        return false;
    }

    const bool encryptedTree = treeContainsEncryptedNode(device);
    if (m_showEncrypted && !m_showEncrypted->isChecked() && encryptedTree) {
        return false;
    }

    const bool linuxCandidate = treeContainsLinuxCandidateNode(device);
    if (m_showNonLinux && !m_showNonLinux->isChecked()
        && !linuxCandidate
        && !(encryptedTree && m_showEncrypted && m_showEncrypted->isChecked())) {
        return false;
    }

    return true;
}

QString MainWindow::combinedModel(const DeviceNode &node)
{
    QStringList parts;
    if (!node.vendor.isEmpty()) {
        parts.append(node.vendor);
    }
    if (!node.model.isEmpty() && !parts.contains(node.model)) {
        parts.append(node.model);
    }
    if (parts.isEmpty() && !node.label.isEmpty()) {
        parts.append(node.label);
    }
    return parts.join(QLatin1Char(' ')).simplified();
}

QString MainWindow::deviceKind(const DeviceNode &node)
{
    if (node.protectedDevice) {
        return QStringLiteral("Protected current system");
    }
    if (node.encrypted) {
        return QStringLiteral("Encrypted volume");
    }
    if (node.type == QStringLiteral("disk")) {
        return QStringLiteral("Physical disk");
    }
    return node.type;
}

QIcon MainWindow::iconForDevice(const DeviceNode &node)
{
    if (node.protectedDevice) {
        return themedIcon(QStringLiteral("security-high"), themedIcon(QStringLiteral("drive-harddisk")));
    }
    if (node.encrypted) {
        return themedIcon(QStringLiteral("document-encrypt"), themedIcon(QStringLiteral("drive-harddisk")));
    }
    if (node.installedLinux) {
        return themedIcon(QStringLiteral("computer"), themedIcon(QStringLiteral("drive-harddisk")));
    }
    if (node.removable || node.transport.compare(QStringLiteral("usb"), Qt::CaseInsensitive) == 0) {
        return themedIcon(QStringLiteral("drive-removable-media"), themedIcon(QStringLiteral("drive-harddisk")));
    }
    if (node.type == QStringLiteral("part")) {
        return themedIcon(QStringLiteral("drive-harddisk"));
    }
    return themedIcon(QStringLiteral("drive-harddisk"));
}
