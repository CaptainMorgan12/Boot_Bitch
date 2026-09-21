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
#include <QGridLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QInputDialog>
#include <QImage>
#include <QPaintEvent>
#include <QHash>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QModelIndex>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QPointer>
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
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QStyleHints>
#include <QSyntaxHighlighter>
#include <QTabBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextOption>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextStream>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#ifdef BOOT_REPAIR_HAVE_DBUS
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusVariant>
#endif

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

// ---- File-local helpers -----------------------------------------------------

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

// A single coalescing delay absorbs bursts of invalidations (for example the
// stages of one repair) before the automatic read-only report is regenerated.
// The delay lives in MainWindow::m_evidenceRefreshDelayMs so the UI regression
// tests can shorten it without changing the production default.

// Stable marker for a privileged helper response that ended without the
// protocol DONE record (helper rejected the request, exited early, or the
// response was truncated). Callers use it to distinguish a transport/protocol
// failure from an ordinary non-zero command exit.
const char kHelperProtocolIncompleteMarker[] = "did not return a protocol DONE record";

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

// Keeps the global busy indicator balanced for the complete lifetime of an
// asynchronous operation, including every success, failure and cancel path.
// The indicator is purely informational and never changes the operation.
class BusyOperationScope
{
public:
    BusyOperationScope(MainWindow *window, const QString &label)
        : m_window(window)
        , m_label(label)
    {
        if (m_window) {
            m_window->beginBusyOperation(m_label);
        }
    }

    ~BusyOperationScope()
    {
        if (m_window) {
            m_window->endBusyOperation(m_label);
        }
    }

    BusyOperationScope(const BusyOperationScope &) = delete;
    BusyOperationScope &operator=(const BusyOperationScope &) = delete;

private:
    MainWindow *m_window = nullptr;
    QString m_label;
};

// Marks the complete file system repair flow — the read-only check, the
// device/mode selection and every selected device's sequential repair — as
// owned by one operation. While the flag is set every repair entry point
// refuses a second request with a clear message instead of queueing it behind
// the running flow. The flag is cleared on every exit path, including
// cancellation and failure.
class FilesystemRepairFlowScope
{
public:
    explicit FilesystemRepairFlowScope(bool &flag)
        : m_flag(flag)
    {
        m_flag = true;
    }

    ~FilesystemRepairFlowScope()
    {
        m_flag = false;
    }

    FilesystemRepairFlowScope(const FilesystemRepairFlowScope &) = delete;
    FilesystemRepairFlowScope &operator=(const FilesystemRepairFlowScope &) = delete;

private:
    bool &m_flag;
};

// The busy label is informational and may be wider than the compact header
// slot reserved for it. Paint an elided form while text() still exposes the
// complete label to accessibility, tooltips and tests.
class ElidedLabel final : public QLabel
{
public:
    explicit ElidedLabel(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    QSize sizeHint() const override
    {
        const QSize base = QLabel::sizeHint();
        return QSize(qMin(base.width(), 420), base.height());
    }

    QSize minimumSizeHint() const override
    {
        return QSize(0, QLabel::minimumSizeHint().height());
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setPen(palette().color(foregroundRole()));
        const QString elided = fontMetrics().elidedText(text(), Qt::ElideRight, contentsRect().width());
        painter.drawText(contentsRect(), static_cast<int>(alignment()), elided);
    }
};

// Scope labels on the responsive page headers wrap into the two-line form
// ("Host maintenance:" then "/dev/vda") instead of eliding the value away when
// the header narrows. QLabel's wrapped size hint prefers that form; the
// minimum height additionally reserves the two wrapped lines so a vertical
// squeeze in a scroll-area page cannot clip the second line. The complete text
// stays in text() and in the tooltip set by updateTargetLabels().
class WrappedScopeLabel final : public QLabel
{
public:
    explicit WrappedScopeLabel(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        configure();
    }

    explicit WrappedScopeLabel(const QString &text, QWidget *parent = nullptr)
        : QLabel(text, parent)
    {
        configure();
    }

    QSize sizeHint() const override
    {
        QSize hint = QLabel::sizeHint();
        hint.setHeight(qMax(hint.height(), twoLineHeight()));
        return hint;
    }

    QSize minimumSizeHint() const override
    {
        QSize hint = QLabel::minimumSizeHint();
        hint.setHeight(qMax(hint.height(), twoLineHeight()));
        return hint;
    }

protected:
    void changeEvent(QEvent *event) override
    {
        QLabel::changeEvent(event);
        if (event->type() == QEvent::FontChange) {
            updateMinimumHeight();
        }
    }

private:
    void configure()
    {
        setWordWrap(true);
        setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        setMinimumWidth(0);
        setMaximumWidth(440);
        updateMinimumHeight();
    }

    // An explicit minimum height, unlike minimumSizeHint(), survives the
    // vertical squeeze of a scroll-area page layout, so the second scope line
    // can never be clipped.
    void updateMinimumHeight()
    {
        setMinimumHeight(twoLineHeight());
    }

    int twoLineHeight() const
    {
        const QFontMetrics metrics(font());
        return metrics.lineSpacing() * 2;
    }
};

// The Full Repair plan row shares its width with the stage-count label. At the
// minimum supported window width the two action buttons must shrink below
// their content width instead of colliding. Paint an elided form while text()
// keeps the complete label for accessibility, tooltips and tests.
class ElidedPushButton final : public QPushButton
{
public:
    explicit ElidedPushButton(QWidget *parent = nullptr)
        : QPushButton(parent)
    {
    }

    ElidedPushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr)
        : QPushButton(icon, text, parent)
    {
    }

    explicit ElidedPushButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
    }

    QSize minimumSizeHint() const override
    {
        QSize hint = QPushButton::minimumSizeHint();
        hint.setWidth(qMin(hint.width(), 64));
        return hint;
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        QStylePainter painter(this);
        QStyleOptionButton option;
        initStyleOption(&option);
        option.text = fontMetrics().elidedText(text(), Qt::ElideRight, elidedTextWidth());
        painter.drawControl(QStyle::CE_PushButton, option);
    }

private:
    int elidedTextWidth() const
    {
        int available = contentsRect().width();
        if (!icon().isNull()) {
            const QSize extent = iconSize().isValid()
                ? iconSize()
                : QSize(style()->pixelMetric(QStyle::PM_ButtonIconSize),
                        style()->pixelMetric(QStyle::PM_ButtonIconSize));
            available -= extent.width() + 8;
        }
        return qMax(0, available - 8);
    }
};

// File Copy and Diagnostics share a responsive page-title header. The title
// and help button keep the first row. The wrapping scope label and the primary
// action buttons share one row (scope left of the buttons, buttons
// right-aligned at the content edge) while every item still fits at its
// preferred width; when the row gets too narrow that pair drops to a second
// row together, so the title is never truncated and the scope label keeps its
// two-line form beside the actions instead of eliding the value away.
class ResponsiveHeaderReflow final : public QObject
{
public:
    ResponsiveHeaderReflow(QWidget *host, QLayout *hostLayout, QGridLayout *header,
                           QLabel *title, QToolButton *help, QLabel *scope,
                           const QList<QPushButton *> &actions)
        : QObject(host)
        , m_host(host)
        , m_hostLayout(hostLayout)
        , m_header(header)
        , m_title(title)
        , m_help(help)
        , m_scope(scope)
        , m_actions(actions)
    {
        if (m_host) {
            m_host->installEventFilter(this);
        }
        updatePlacement();
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_host) {
            switch (event->type()) {
            case QEvent::Resize:
            case QEvent::LayoutRequest:
            case QEvent::Show:
                updatePlacement();
                break;
            default:
                break;
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    int headerSpacing() const
    {
        if (!m_header) {
            return 6;
        }
        int spacing = m_header->horizontalSpacing();
        if (spacing < 0) {
            spacing = m_header->spacing();
        }
        return spacing < 0 ? 6 : spacing;
    }

    bool singleRowFits() const
    {
        if (!m_host || !m_header || !m_title || !m_scope) {
            return false;
        }
        const int spacing = headerSpacing();
        int required = m_title->sizeHint().width();
        if (m_help) {
            required += spacing + m_help->sizeHint().width();
        }
        required += spacing + qMin(m_scope->sizeHint().width(), m_scope->maximumWidth());
        for (QPushButton *action : m_actions) {
            if (action) {
                required += spacing + action->sizeHint().width();
            }
        }
        int available = m_host->width();
        if (m_hostLayout) {
            const QMargins margins = m_hostLayout->contentsMargins();
            available -= qMax(0, margins.left()) + qMax(0, margins.right());
        }
        // The item hints do not include the header's own margins or a
        // scrollbar gutter; keep a small safety pad before choosing one row.
        return required + 8 <= available;
    }

    void updatePlacement()
    {
        if (!m_header) {
            return;
        }
        const bool singleRow = singleRowFits();
        if (m_placed && m_stacked == !singleRow) {
            return;
        }
        m_placed = true;
        m_stacked = !singleRow;

        auto place = [this](QWidget *widget, int row, int column, Qt::Alignment alignment) {
            if (!widget) {
                return;
            }
            m_header->removeWidget(widget);
            m_header->addWidget(widget, row, column, alignment);
        };
        place(m_title, 0, 0, Qt::Alignment());
        place(m_help, 0, 1, Qt::AlignVCenter);
        // The scope label always shares the action row: to the left of the
        // first action while the title row has room, otherwise together with
        // the actions on the wrapped second row. It must never be left behind
        // on a row that the actions just left, where it would be squeezed to
        // nothing. On the wrapped row it spans the title/help columns as well,
        // so the two-line form is measured against the full content width
        // instead of the narrow title columns it no longer shares.
        const int actionRow = singleRow ? 0 : 1;
        if (m_scope) {
            m_header->removeWidget(m_scope);
            if (singleRow) {
                m_header->addWidget(m_scope, actionRow, 2, Qt::AlignRight | Qt::AlignVCenter);
            } else {
                m_header->addWidget(m_scope, actionRow, 0, 1, 3,
                                    Qt::AlignRight | Qt::AlignVCenter);
            }
        }
        int column = 3;
        for (QPushButton *action : m_actions) {
            place(action, actionRow, column, Qt::AlignVCenter);
            ++column;
        }
        // The scope column absorbs the slack so the actions stay anchored to
        // the content edge on both rows.
        m_header->setColumnStretch(2, 1);
    }

    QWidget *m_host = nullptr;
    QLayout *m_hostLayout = nullptr;
    QGridLayout *m_header = nullptr;
    QLabel *m_title = nullptr;
    QToolButton *m_help = nullptr;
    QLabel *m_scope = nullptr;
    QList<QPushButton *> m_actions;
    bool m_placed = false;
    bool m_stacked = false;
};

// Diagnostic log sections are tracked by (key, scope) so a newer result can
// replace the previous section instead of accumulating duplicates. The
// separator never appears in a key or scope label.
QString diagnosticEntryIdentity(const QString &key, const QString &scope)
{
    return scope + QLatin1Char('\x1f') + key;
}

// Recurring one-line status entries share the diagnostic replacement model but
// live in their own namespace so a status can never collide with a diagnostic
// section or a repair block. The kind names the status and the optional scope
// keeps entries that legitimately differ per host/target scope, disk/root or
// diagnostic key apart.
QString statusEntryIdentity(const QString &kind, const QString &scope = QString())
{
    return scope.isEmpty()
        ? QStringLiteral("status\x1f%1").arg(kind)
        : QStringLiteral("status\x1f%1\x1f%2").arg(kind, scope);
}

// Recognizes the framing written by appendDiagnosticLog. Other entries that
// merely mention "Diagnostic:" (repair output, chroot transcripts) do not
// carry a following "Scope:" metadata line and are deliberately ignored. This
// avoids splitting potentially huge diagnostic sections into line lists.
bool diagnosticEntryMetadata(const QString &entry, QString *key, QString *scope)
{
    const int keyPos = entry.indexOf(QStringLiteral("\nDiagnostic: "));
    if (keyPos < 0) {
        return false;
    }
    const int keyStart = keyPos + 13;
    int keyEnd = entry.indexOf(QLatin1Char('\n'), keyStart);
    if (keyEnd < 0) {
        keyEnd = entry.size();
    }
    const int scopePos = entry.indexOf(QStringLiteral("\nScope: "), keyEnd);
    if (scopePos < 0) {
        return false;
    }
    const int scopeStart = scopePos + 8;
    int scopeEnd = entry.indexOf(QLatin1Char('\n'), scopeStart);
    if (scopeEnd < 0) {
        scopeEnd = entry.size();
    }
    *key = entry.mid(keyStart, keyEnd - keyStart).trimmed();
    *scope = entry.mid(scopeStart, scopeEnd - scopeStart).trimmed();
    return !key->isEmpty() && !scope->isEmpty();
}

// The visible title of a framed diagnostic section, used by the @section
// search so a term can select a section by name as well as by key.
QString diagnosticEntryTitle(const QString &entry)
{
    const int titlePos = entry.indexOf(QStringLiteral("\nTitle: "));
    if (titlePos < 0) {
        return QString();
    }
    const int titleStart = titlePos + 8;
    int titleEnd = entry.indexOf(QLatin1Char('\n'), titleStart);
    if (titleEnd < 0) {
        titleEnd = entry.size();
    }
    return entry.mid(titleStart, titleEnd - titleStart).trimmed();
}

// Every application-log entry starts with a "──────── CATEGORY ────────"
// header written by appendLog() from the explicit operation kind chosen at
// append time. The category text is a stable, one-to-one projection of that
// kind and drives the Logs filter dropdown (Diagnostics / Repairs and the
// diagnostic sections); it is never derived from the message body.
QString logEntryCategory(const QString &entry)
{
    static const QString marker = QStringLiteral("────────");
    if (!entry.startsWith(marker)) {
        return QString();
    }
    const int end = entry.indexOf(marker, marker.size());
    if (end < 0) {
        return QString();
    }
    return entry.mid(marker.size(), end - marker.size()).trimmed();
}

QString logCategoryForKind(MainWindow::LogEntryKind kind)
{
    switch (kind) {
    case MainWindow::LogEntryKind::Diagnostic:
        return QStringLiteral("DIAGNOSTIC");
    case MainWindow::LogEntryKind::Repair:
        return QStringLiteral("REPAIR");
    case MainWindow::LogEntryKind::Unlock:
        return QStringLiteral("UNLOCK");
    case MainWindow::LogEntryKind::Snapshot:
        return QStringLiteral("SNAPSHOTS");
    case MainWindow::LogEntryKind::FileCopy:
        return QStringLiteral("FILE COPY");
    case MainWindow::LogEntryKind::ChrootShell:
        return QStringLiteral("CHROOT SHELL");
    case MainWindow::LogEntryKind::HostShell:
        return QStringLiteral("HOST SHELL");
    case MainWindow::LogEntryKind::Application:
        break;
    }
    return QStringLiteral("APPLICATION");
}

// Reads back the kind recorded in an entry's header. Entries loaded from a
// session file keep the category written when they were appended, so filtering
// behaves identically for the live register and prior sessions without
// examining the message text. Unknown or legacy headers stay Application.
MainWindow::LogEntryKind logEntryKind(const QString &entry)
{
    const QString category = logEntryCategory(entry);
    if (category == QStringLiteral("DIAGNOSTIC")) {
        return MainWindow::LogEntryKind::Diagnostic;
    }
    if (category == QStringLiteral("REPAIR")) {
        return MainWindow::LogEntryKind::Repair;
    }
    if (category == QStringLiteral("UNLOCK")) {
        return MainWindow::LogEntryKind::Unlock;
    }
    if (category == QStringLiteral("SNAPSHOTS")) {
        return MainWindow::LogEntryKind::Snapshot;
    }
    if (category == QStringLiteral("FILE COPY")) {
        return MainWindow::LogEntryKind::FileCopy;
    }
    if (category == QStringLiteral("CHROOT SHELL")) {
        return MainWindow::LogEntryKind::ChrootShell;
    }
    if (category == QStringLiteral("HOST SHELL")) {
        return MainWindow::LogEntryKind::HostShell;
    }
    return MainWindow::LogEntryKind::Application;
}

static const DiagnosticSpec diagnosticSpecs[] = {
    {"environment", "Environment validation", "Summarizes the selected system, protection state, mounted identity and inspection readiness.", "task-complete"},
    {"backend", "Distribution and boot backend profile", "Identifies the distribution family, package manager, initramfs generator, bootloader, ESP location and current guarded repair capability.", "distribution"},
    {"boot", "Boot diagnostics", "Shows boot mounts, /boot and EFI contents plus storage evidence without changing the selected system.", "system-run"},
    {"boot-evidence", "Boot evidence and selection history", "Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected.", "dialog-information"},
    {"kernel", "Kernel / initramfs", "Reviews kernel files and verifies matching initramfs images through a read-only inspection.", "kernel"},
    {"grub", "GRUB configuration", "Reviews GRUB configuration and /etc/default/grub without changing boot files.", "grub"},
    {"uki", "EFI / UKI boot state", "Inspects the selected ESP, vendor or generic UKI images, systemd-boot loader files, embedded kernel/cmdline data and firmware entries with PARTUUID ownership classification.", "drive-removable-media"},
    {"display", "Graphical login / display manager", "Reviews graphical.target, the configured display manager (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), installed desktop packages, and recent boot/journal evidence without starting the GUI.", "video-display"},
    {"errors", "Boot errors", "Reads recent error-priority entries from the running host or selected repair system's persistent journal when available.", "dialog-warning"},
    {"usage", "Disk usage", "Summarizes filesystem capacity/free space for the running host or read-only repair target.", "drive-harddisk"},
    {"filesystem", "File systems", "Runs the read-only file system check for the running host or selected repair system's root, /boot, ESP and /home filesystems and reports each device's check tool and result without changing anything.", "drive-harddisk"},
    {"fstab", "fstab", "Displays the running host or selected repair system's fstab; repair-system inspection is mounted read-only.", "document"},
    {"btrfs", "Btrfs status", "Shows Btrfs filesystem and subvolume information for the running host or selected repair system.", "drive-harddisk"},
    {"mapper", "Mapper status", "Shows selected mapper ancestry, device-mapper state and cryptsetup status when available.", "lock"},
    {"luks", "LUKS / crypttab", "Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.", "lock"},
    {"report", "Full diagnostic report", "Combines all read-only diagnostics for the selected scope in one privileged inspection session.", "document-preview"}
};

// Repair topics referenced by the log section filters. Each topic is keyed by
// the stable repair-tool key and matched against the recorded repair entries
// through the case-insensitive title fragments below. The entry title always
// carries the tool's user-visible name, so one fragment list also covers the
// start, completion and captured-output entries of a single repair cycle.
struct RepairTopicSpec {
    const char *key;
    const char *titleFragments; // '|'-separated fragments
};

static const RepairTopicSpec repairTopicSpecs[] = {
    {"validate", "Validate repair target|Validate running host"},
    {"filesystem", "File system repair|File system check|Check File Systems"},
    // File-copy operations are their own topic so the dedicated "File copy"
    // workflow can show them alone while the "File system repair" workflow
    // also includes them (copying files is part of repairing a system's
    // filesystems). The fragments cover both directions and the staging
    // notices, so no copy action can silently disappear from the filters.
    {"filecopy", "File Copy|Host → Repair|Repair → Host"},
    {"dpkg", "Complete package configuration|dpkg --configure"},
    {"fixbroken", "Repair broken dependencies|fix-broken"},
    {"aptupdate", "Refresh package metadata|apt-update"},
    {"upgrade", "Upgrade installed packages|apt-upgrade"},
    {"dkms", "DKMS"},
    // The helper's package-backend transcript and package-manager feedback
    // lines are not tied to a single stage title: a combined Full Repair
    // entry carries them for every backend that ran or was skipped. They are
    // their own topic so the "Package repair" workflow can capture that
    // entry without leaking the lines into unrelated filters.
    {"packagefeedback", "BEGIN: package backend|SKIP: package backend|Package kept back|Package skipped|Package manager feedback"},
    {"display", "graphical login manager|display manager|display-manager"},
    {"initramfs", "initramfs"},
    {"efi", "EFI / UKI|EFI bootloader|Make host default boot entry"},
    {"grub", "GRUB"},
    {"extlinux", "extlinux"},
    {"bootstack", "boot stack|boot-stack"}
};

// Single mapping for the Logs section filters: every diagnostic section key
// pairs with the repair workflows that belong to it, so selecting the filter
// shows both the captured evidence and what the related repair tools did.
// Extend this table when a section gains another related workflow.
struct LogSectionFilterSpec {
    const char *sectionKey;
    const char *repairTopics; // space-separated RepairTopicSpec keys
};

static const LogSectionFilterSpec logSectionFilterSpecs[] = {
    {"environment", "dpkg fixbroken aptupdate upgrade"},
    {"backend", "dpkg fixbroken aptupdate upgrade"},
    {"boot", "bootstack grub extlinux efi"},
    {"boot-evidence", "bootstack grub extlinux efi"},
    {"kernel", "initramfs dkms"},
    {"grub", "grub"},
    {"uki", "efi"},
    {"display", "display"},
    {"errors", ""},
    {"usage", ""},
    {"filesystem", "filesystem"},
    {"fstab", "filesystem bootstack initramfs"},
    {"btrfs", ""},
    {"mapper", "initramfs bootstack"},
    {"luks", "initramfs bootstack"},
    {"report", "validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack"}
};

static QString logRepairTopicsForSection(const QString &sectionKey)
{
    for (const LogSectionFilterSpec &spec : logSectionFilterSpecs) {
        if (sectionKey == QLatin1String(spec.sectionKey)) {
            return QString::fromLatin1(spec.repairTopics);
        }
    }
    return QString();
}

// Repair-workflow dropdown filters. A workflow filter is a repair-centric view
// that names the repair topics it shows plus the diagnostic sections carrying
// its evidence. The sections are shown whole exactly like a section filter:
// the dedicated capture when it exists, otherwise the embedded section of a
// combined report. This is what makes "File system repair" coherent instead of
// a flat list of unrelated fragments.
struct LogWorkflowFilterSpec {
    const char *key;
    const char *title;
    const char *repairTopics; // space-separated RepairTopicSpec keys
    const char *sections;     // space-separated diagnostic section keys
};

static const LogWorkflowFilterSpec logWorkflowFilterSpecs[] = {
    // The file system check and repair share one workflow: the check is the
    // read-only pre-stage of the repair, so both entry sets belong to the same
    // filter together with the storage evidence they inspect or invalidate.
    // File-copy operations are part of the same repair workflow (they move
    // files on the repaired filesystems) and are included through the
    // dedicated filecopy topic.
    {"topic:filesystem", "File system repair", "filesystem filecopy", "environment usage btrfs fstab"},
    // Package stages share one workflow: dependency repair, interrupted
    // package configuration, metadata refresh, upgrades and DKMS module
    // rebuilds are all package operations. The stage topics carry the visible
    // titles; packagefeedback adds the helper's per-backend BEGIN/SKIP and
    // held-back/skipped lines, so a combined Full Repair entry is captured as
    // well. The environment and backend evidence sections describe the
    // detected package managers; the file system check/repair keeps its own
    // workflow above instead of absorbing the package stages.
    {"topic:package", "Package repair", "dpkg fixbroken aptupdate upgrade dkms packagefeedback", "environment backend"},
    // Dedicated file-copy view: the copy operations alone, plus the storage
    // evidence a transfer can affect, without the check/repair tool entries.
    {"topic:filecopy", "File copy", "filecopy", "environment usage btrfs fstab"}
};

static const LogWorkflowFilterSpec *logWorkflowFilterFor(const QString &key)
{
    for (const LogWorkflowFilterSpec &spec : logWorkflowFilterSpecs) {
        if (key == QLatin1String(spec.key)) {
            return &spec;
        }
    }
    return nullptr;
}

// Single explicit invalidation table: the diagnostic sections whose cached
// evidence each repair tool/stage makes stale. "all" is the combined-report
// pseudo-section and means the complete set; an empty list means the action is
// read-only and invalidates nothing. Every invalidation decision goes through
// MainWindow::diagnosticSectionsForRepair(); no ad-hoc section lists exist
// elsewhere. Unknown keys (including file copy, shell, snapshot, unlock and
// target/scope changes) deliberately map to "all" (fail safe).
//
// toolKey (repair tool / helper stage)   invalidated diagnostic sections
// ------------------------------------   ---------------------------------
// validate                               (none: read-only)
// filesystem                             environment usage btrfs filesystem
// dpkg, dpkg-configure                   all
// fixbroken, fix-broken                  all
// aptupdate, apt-update                  all
// upgrade, apt-upgrade                   all
// dkms                                   kernel
// display, display-manager               display
// initramfs                              kernel
// efi                                    uki grub boot-evidence
// grub                                   grub
// extlinux                               boot boot-evidence
// bootstack, boot-stack                  all
// host-default                           all
struct RepairDiagnosticSectionSpec {
    const char *toolKey;
    const char *sections; // space-separated; "all" means the complete set
};

static const RepairDiagnosticSectionSpec repairDiagnosticSectionSpecs[] = {
    {"validate", ""},
    // A file system repair changes filesystem metadata and free space; keep
    // the capability evidence and the sections that describe the repaired
    // storage, including the read-only file system check evidence itself.
    {"filesystem", "environment usage btrfs filesystem"},
    {"dpkg", "all"},
    {"dpkg-configure", "all"},
    {"fixbroken", "all"},
    {"fix-broken", "all"},
    {"aptupdate", "all"},
    {"apt-update", "all"},
    {"upgrade", "all"},
    {"apt-upgrade", "all"},
    {"dkms", "kernel"},
    {"display", "display"},
    {"display-manager", "display"},
    {"initramfs", "kernel"},
    // EFI / UKI repair reinstalls the loader, regenerates GRUB afterward and
    // changes firmware entries, so all three boot sections are invalidated.
    {"efi", "uki grub boot-evidence"},
    {"grub", "grub"},
    // extlinux is the Alpine/BIOS bootloader sibling of GRUB: regenerating it
    // changes the boot configuration and boot-chain evidence, not the kernel.
    {"extlinux", "boot boot-evidence"},
    {"bootstack", "all"},
    {"boot-stack", "all"},
    {"host-default", "all"}
};

// The canonical section order used by the combined report, so partial
// regenerations run in the same deterministic sequence.
static QStringList orderedDiagnosticSectionKeys()
{
    QStringList keys;
    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        const QString key = QString::fromLatin1(spec.key);
        if (key != QStringLiteral("report")) {
            keys.append(key);
        }
    }
    return keys;
}

static QStringList orderedDiagnosticSections(const QStringList &sections)
{
    QStringList ordered;
    for (const QString &key : orderedDiagnosticSectionKeys()) {
        if (sections.contains(key)) {
            ordered.append(key);
        }
    }
    return ordered;
}

static bool repairEntryMatchesTopics(const QString &entry, const QSet<QString> &topics)
{
    if (topics.isEmpty()) {
        return false;
    }
    for (const RepairTopicSpec &spec : repairTopicSpecs) {
        if (!topics.contains(QString::fromLatin1(spec.key))) {
            continue;
        }
        const QStringList fragments = QString::fromLatin1(spec.titleFragments)
            .split(QLatin1Char('|'), Qt::SkipEmptyParts);
        for (const QString &fragment : fragments) {
            if (entry.contains(fragment, Qt::CaseInsensitive)) {
                return true;
            }
        }
    }
    return false;
}

// The helper frames every diagnostic section and its title with this exact
// divider. Section extraction relies on the framing to keep the captured
// header and metadata intact.
bool isDiagnosticSectionDivider(const QString &line)
{
    return line == QStringLiteral("========================================");
}

// Line index where the embedded section whose "Diagnostic:" header is at
// diagnosticLine begins. Helper output frames the title between two dividers
// directly above the header, while the GUI framing places the opening divider
// directly above the header instead.
int embeddedSectionStartLine(const QStringList &lines, int diagnosticLine)
{
    if (diagnosticLine >= 3
        && isDiagnosticSectionDivider(lines.at(diagnosticLine - 1))
        && !lines.at(diagnosticLine - 2).isEmpty()
        && !isDiagnosticSectionDivider(lines.at(diagnosticLine - 2))
        && isDiagnosticSectionDivider(lines.at(diagnosticLine - 3))) {
        return diagnosticLine - 3;
    }
    if (diagnosticLine >= 1 && isDiagnosticSectionDivider(lines.at(diagnosticLine - 1))) {
        return diagnosticLine - 1;
    }
    return diagnosticLine;
}

// Extracts the complete embedded sections of the requested keys from a
// combined diagnostic entry (for example a Run All report). Each section is
// returned with its header, metadata and framing exactly as captured, ending
// where the next section's frame begins.
QStringList extractEmbeddedDiagnosticSections(const QString &entry, const QSet<QString> &keys)
{
    if (keys.isEmpty()) {
        return QStringList();
    }
    const QString headerPrefix = QStringLiteral("Diagnostic: ");
    const QStringList lines = entry.split(QLatin1Char('\n'));
    QList<int> headers;
    for (int index = 0; index < lines.size(); ++index) {
        if (lines.at(index).startsWith(headerPrefix)) {
            headers.append(index);
        }
    }
    QStringList sections;
    for (int position = 0; position < headers.size(); ++position) {
        const int header = headers.at(position);
        const QString key = lines.at(header).mid(headerPrefix.size()).trimmed();
        if (!keys.contains(key.toCaseFolded())) {
            continue;
        }
        const int start = embeddedSectionStartLine(lines, header);
        int end = position + 1 < headers.size()
            ? embeddedSectionStartLine(lines, headers.at(position + 1))
            : lines.size();
        while (end > start && lines.at(end - 1).trimmed().isEmpty()) {
            --end;
        }
        if (end > start) {
            sections.append(lines.mid(start, end - start).join(QLatin1Char('\n')));
        }
    }
    return sections;
}

// Single truth log reader: every action gate parses the cached diagnostic
// evidence through this helper, so the `Repair tool <key>` protocol is
// implemented once instead of being re-checked in each action. `hadEvidence`
// reports whether a line for the key was present at all.
static bool cachedRepairToolAvailable(const QMap<QString, QString> &cache, const QString &key,
                                      QString *reason, bool *hadEvidence = nullptr)
{
    if (hadEvidence) {
        *hadEvidence = false;
    }
    const QString prefix = QStringLiteral("Repair tool %1: ").arg(key);
    bool available = false;
    for (const QString &evidence : cache) {
        for (const QString &line : evidence.split(QLatin1Char('\n'))) {
            if (!line.startsWith(prefix)) {
                continue;
            }
            if (hadEvidence) {
                *hadEvidence = true;
            }
            const QString state = line.mid(prefix.size()).trimmed();
            if (state == QStringLiteral("available")) {
                available = true;
            } else if (state.startsWith(QStringLiteral("unavailable|"))) {
                const QString detail = state.mid(QStringLiteral("unavailable|").size()).trimmed();
                if (reason) {
                    *reason = detail.isEmpty()
                        ? QStringLiteral("Repair tool %1 is unavailable.").arg(key)
                        : detail;
                }
                return false;
            } else {
                if (reason) {
                    *reason = QStringLiteral("Repair tool %1 has unknown or unavailable diagnostic evidence.").arg(key);
                }
                return false;
            }
        }
    }
    if (!available) {
        if (reason) {
            *reason = QStringLiteral("No capability evidence for repair tool %1 in the selected scope. Run diagnostics first.").arg(key);
        }
        return false;
    }
    return true;
}

// A soft shadow gradient at the top and bottom edge of a scroll area hints
// that more content is available in that direction; the overlay never takes
// mouse input and repaints with the scroll position and viewport size.
class ScrollFadeOverlay : public QWidget
{
public:
    explicit ScrollFadeOverlay(QScrollArea *area)
        : QWidget(area ? area->viewport() : nullptr), m_area(area)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        if (!m_area) {
            return;
        }
        m_area->viewport()->installEventFilter(this);
        if (QScrollBar *bar = m_area->verticalScrollBar()) {
            connect(bar, &QScrollBar::valueChanged, this, qOverload<>(&QWidget::update));
            connect(bar, &QScrollBar::rangeChanged, this, qOverload<>(&QWidget::update));
        }
        setGeometry(m_area->viewport()->rect());
        raise();
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (m_area && watched == m_area->viewport() && event->type() == QEvent::Resize) {
            setGeometry(m_area->viewport()->rect());
            raise();
        }
        return QWidget::eventFilter(watched, event);
    }

    void paintEvent(QPaintEvent *) override
    {
        if (!m_area) {
            return;
        }
        QScrollBar *bar = m_area->verticalScrollBar();
        if (!bar || bar->maximum() <= bar->minimum()) {
            return;
        }
        constexpr int kFade = 30;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        // The fade follows the effective appearance: a dark shadow on light
        // surfaces, a soft light veil on dark surfaces.
        const QColor shadow = palette().color(QPalette::Window).lightness() < 128
            ? QColor(255, 255, 255, 64)
            : QColor(0, 0, 0, 90);
        const QColor clear(0, 0, 0, 0);
        if (bar->value() < bar->maximum()) {
            QLinearGradient gradient(0, height() - kFade, 0, height());
            gradient.setColorAt(0.0, clear);
            gradient.setColorAt(1.0, shadow);
            painter.fillRect(QRect(0, height() - kFade, width(), kFade), gradient);
            // A small chevron reinforces the fade on high-contrast themes.
            QColor chevron = palette().color(QPalette::WindowText);
            chevron.setAlpha(140);
            painter.setPen(QPen(chevron, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            const int cx = width() / 2;
            const int cy = height() - kFade / 2;
            painter.drawLine(QPointF(cx - 5, cy - 2), QPointF(cx, cy + 3));
            painter.drawLine(QPointF(cx, cy + 3), QPointF(cx + 5, cy - 2));
        }
        if (bar->value() > bar->minimum()) {
            QLinearGradient gradient(0, 0, 0, kFade);
            gradient.setColorAt(0.0, shadow);
            gradient.setColorAt(1.0, clear);
            painter.fillRect(QRect(0, 0, width(), kFade), gradient);
        }
    }

private:
    QScrollArea *m_area = nullptr;
};

// ---- Icon helpers -----------------------------------------------------------

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
    if (name == QStringLiteral("kernel")) {
        atlasName = QStringLiteral("kernel");
    } else if (name == QStringLiteral("initramfs") || name == QStringLiteral("mkinitcpio")) {
        atlasName = QStringLiteral("initramfs");
    } else if (name == QStringLiteral("grub")) {
        atlasName = QStringLiteral("grub");
    } else if (name == QStringLiteral("distribution") || name == QStringLiteral("distro")) {
        atlasName = QStringLiteral("distribution");
    } else if (name == QStringLiteral("task-complete") || name == QStringLiteral("dialog-ok-apply")) {
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
    } else if (name.contains(QStringLiteral("refresh")) || name.contains(QStringLiteral("run"))
               || name.contains(QStringLiteral("software")) || name.contains(QStringLiteral("configure")) || name.contains(QStringLiteral("settings"))
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
    } else if (name.contains(QStringLiteral("open")) || name.contains(QStringLiteral("save"))
               || name.contains(QStringLiteral("edit")) || name.contains(QStringLiteral("list"))) {
        atlasName = QStringLiteral("document");
    } else if (name.contains(QStringLiteral("removable")) || name.contains(QStringLiteral("harddisk"))) {
        atlasName = QStringLiteral("drive");
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
    // Use the application's bundled semantic atlas first.  A host theme can
    // return misleading placeholders (K badges, question marks, blank pages)
    // for valid names, especially on Arch/XFCE.  The atlas is the same
    // repository shipped for every desktop, so controls remain identical on
    // KDE, GNOME and lightweight sessions.  Desktop icons remain a fallback
    // for names that are intentionally outside the atlas.
    QIcon source = bundledIcon(name);
    if (source.isNull()) source = QIcon::fromTheme(name, fallback);
    // Try common freedesktop aliases before drawing a fallback.  Themes often
    // ship a semantically equivalent name rather than every application
    // specific name used by Boot Bitch.
    if (source.isNull()) {
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

// Dense button rows (Logs session controls, File Copy staging controls) must
// stay shrinkable when the window is narrow. normalizeButtonSizing() gives
// every push button a minimum width equal to its content, which makes Qt
// position an over-constrained row at those minimums while still advancing
// by smaller allocated widths, so neighbouring buttons visibly overlap.
// Shrinkable buttons keep a small floor instead and let the row fit.
void makeButtonShrinkable(QPushButton *button, int minimumWidth = 64)
{
    if (!button) {
        return;
    }
    // normalizeButtonSizing() runs after page construction and would restore
    // the full content minimum. Mark the button so that pass leaves it alone.
    button->setProperty("bootRepairShrinkable", true);
    button->setMinimumWidth(minimumWidth);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

// Systems-tab action column: buttons that visually stack on a page's right
// edge share one width (the widest content hint) and one height so their left
// and right edges line up at every window width. The size is fixed rather
// than a minimum so the buttons cannot drift apart when the window narrows;
// runtime text that no longer fits is painted elided by ElidedPushButton.
void standardizeButtonColumn(const QList<QPushButton *> &buttons)
{
    int columnWidth = 0;
    int columnHeight = 0;
    for (QPushButton *button : buttons) {
        if (!button) {
            continue;
        }
        columnWidth = qMax(columnWidth, button->sizeHint().width());
        columnHeight = qMax(columnHeight, qMax(button->minimumHeight(), button->sizeHint().height()));
    }
    for (QPushButton *button : buttons) {
        if (!button) {
            continue;
        }
        button->setFixedSize(columnWidth, columnHeight);
    }
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

    // The Systems target summary may shrink and wrap at narrow widths instead
    // of colliding with the heading. Page-header scope labels use
    // WrappedScopeLabel instead, which reserves the two-line form even under
    // vertical squeeze.
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
    auto *item = new SortableTableItem(cleanText);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setToolTip(cleanText);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    return item;
}

// Natural-order text comparison: embedded digit runs compare by value so
// /dev/sda2 sorts before /dev/sda10 and "snapshot 9" before "snapshot 10".
// Non-digit characters use the locale collation. The comparison is
// deterministic and never falls back to pointer order.
int naturalTextCompare(const QString &left, const QString &right)
{
    int leftIndex = 0;
    int rightIndex = 0;
    while (leftIndex < left.size() && rightIndex < right.size()) {
        const QChar leftChar = left.at(leftIndex);
        const QChar rightChar = right.at(rightIndex);
        if (leftChar.isDigit() && rightChar.isDigit()) {
            int leftEnd = leftIndex;
            int rightEnd = rightIndex;
            while (leftEnd < left.size() && left.at(leftEnd).isDigit()) {
                ++leftEnd;
            }
            while (rightEnd < right.size() && right.at(rightEnd).isDigit()) {
                ++rightEnd;
            }
            // Leading zeros are insignificant, so "02" and "2" compare equal
            // and "10" sorts after "9".
            while (leftIndex + 1 < leftEnd && left.at(leftIndex) == QLatin1Char('0')) {
                ++leftIndex;
            }
            while (rightIndex + 1 < rightEnd && right.at(rightIndex) == QLatin1Char('0')) {
                ++rightIndex;
            }
            const int leftDigits = leftEnd - leftIndex;
            const int rightDigits = rightEnd - rightIndex;
            if (leftDigits != rightDigits) {
                return leftDigits < rightDigits ? -1 : 1;
            }
            const int digits = QStringView(left).mid(leftIndex, leftDigits)
                                   .compare(QStringView(right).mid(rightIndex, rightDigits));
            if (digits != 0) {
                return digits < 0 ? -1 : 1;
            }
            leftIndex = leftEnd;
            rightIndex = rightEnd;
            continue;
        }
        const int characters = QString::localeAwareCompare(QString(leftChar), QString(rightChar));
        if (characters != 0) {
            return characters < 0 ? -1 : 1;
        }
        ++leftIndex;
        ++rightIndex;
    }
    if (leftIndex < left.size()) {
        return 1;
    }
    if (rightIndex < right.size()) {
        return -1;
    }
    return 0;
}

// Compares two explicit sort keys. QDateTime keys compare chronologically;
// every other key compares numerically. Returns false when either side has no
// usable key so the caller can fall back to the natural text comparison.
bool compareExplicitSortKeys(const QVariant &left, const QVariant &right, int *comparison)
{
    if (!comparison || !left.isValid() || !right.isValid()) {
        return false;
    }
    if (left.metaType() == QMetaType::fromType<QDateTime>()
        && right.metaType() == QMetaType::fromType<QDateTime>()) {
        const QDateTime leftDate = left.toDateTime();
        const QDateTime rightDate = right.toDateTime();
        if (!leftDate.isValid() || !rightDate.isValid()) {
            return false;
        }
        *comparison = leftDate < rightDate ? -1 : (rightDate < leftDate ? 1 : 0);
        return true;
    }
    bool leftOk = false;
    bool rightOk = false;
    const qlonglong leftNumber = left.toLongLong(&leftOk);
    const qlonglong rightNumber = right.toLongLong(&rightOk);
    if (!leftOk || !rightOk) {
        return false;
    }
    *comparison = leftNumber < rightNumber ? -1 : (leftNumber > rightNumber ? 1 : 0);
    return true;
}

// Re-applies the header's active sort after a table or tree was repopulated.
// Qt sorts only when asked, so rows added while a sort is active would
// otherwise remain in insertion order. defaultColumn/defaultOrder apply when
// no column has ever been sorted; a negative defaultColumn deliberately leaves
// a view in its curated insertion order until the user clicks a header.
void applyActiveSort(QTableWidget *table, int defaultColumn, Qt::SortOrder defaultOrder)
{
    if (!table) {
        return;
    }
    QHeaderView *header = table->horizontalHeader();
    int column = header->sortIndicatorSection();
    Qt::SortOrder order = header->sortIndicatorOrder();
    if (column < 0 || column >= table->columnCount()) {
        if (defaultColumn < 0 || defaultColumn >= table->columnCount()) {
            return;
        }
        column = defaultColumn;
        order = defaultOrder;
    }
    table->sortByColumn(column, order);
}

void applyActiveSort(QTreeWidget *tree, int defaultColumn, Qt::SortOrder defaultOrder)
{
    if (!tree) {
        return;
    }
    QHeaderView *header = tree->header();
    int column = header->sortIndicatorSection();
    Qt::SortOrder order = header->sortIndicatorOrder();
    if (column < 0 || column >= tree->columnCount()) {
        if (defaultColumn < 0 || defaultColumn >= tree->columnCount()) {
            return;
        }
        column = defaultColumn;
        order = defaultOrder;
    }
    tree->sortByColumn(column, order);
}

// Converts a decimal identifier (snapshot number) into a numeric sort key;
// non-numeric text falls back to the natural text comparison.
QVariant numericSortKey(const QString &text)
{
    bool ok = false;
    const qlonglong value = text.toLongLong(&ok);
    return ok ? QVariant::fromValue(value) : QVariant();
}

// Parses the snapshot creation time emitted by `btrfs subvolume show`
// ("yyyy-MM-dd HH:mm:ss +zzzz") or by stat's fallback ("yyyy-MM-dd HH:mm:ss").
// The helper's offset spelling is not accepted by Qt's ISODate parser, so the
// exact layout is tried first and the offset is preserved for a chronological
// comparison across snapshots taken in different zones.
QDateTime snapshotCreatedDateTime(const QString &created)
{
    QDateTime parsed = QDateTime::fromString(created, QStringLiteral("yyyy-MM-dd HH:mm:ss t"));
    if (!parsed.isValid()) {
        parsed = QDateTime::fromString(created, Qt::ISODate);
    }
    if (!parsed.isValid()) {
        parsed = QDateTime::fromString(created, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    }
    return parsed;
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

// ---- Device-tree inspection helpers -----------------------------------------

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

// Collects the two kinds of mountable Linux repair candidates below a disk:
// mounted installed Linux roots (os-release evidence) and Linux-capable
// filesystems. Swap and ESP-only vfat volumes are never Linux-capable, so
// they cannot enter either list.
void collectLinuxRepairCandidates(const DeviceNode &node,
                                  QList<const DeviceNode *> &installedRoots,
                                  QList<const DeviceNode *> &linuxFilesystems)
{
    if (node.installedLinux) {
        installedRoots.append(&node);
    } else if (node.linuxCapableFileSystem) {
        linuxFilesystems.append(&node);
    }
    for (const DeviceNode &child : node.children) {
        collectLinuxRepairCandidates(child, installedRoots, linuxFilesystems);
    }
}

// Tie-break for equally sized candidates: a node mounted at "/" is decisive,
// root-like labels come next and a bare /boot hint is demoted. These are the
// fstab-like hints a read-only scan can see without mounting anything.
int rootCandidateHintScore(const DeviceNode &node)
{
    int score = 0;
    if (node.mountPoints.contains(QStringLiteral("/"))) {
        score += 4;
    }
    const QString label = (node.partLabel + QLatin1Char(' ') + node.label).toLower();
    if (label.contains(QStringLiteral("root"))) {
        score += 2;
    }
    if (label.contains(QStringLiteral("boot")) && !label.contains(QStringLiteral("root"))) {
        score -= 1;
    }
    return score;
}

// Picks the largest candidate; equal sizes fall back to the root hints and
// then to tree order, so the choice stays deterministic.
const DeviceNode *largestRepairCandidate(const QList<const DeviceNode *> &candidates)
{
    const DeviceNode *best = nullptr;
    for (const DeviceNode *candidate : candidates) {
        if (!best || candidate->sizeBytes > best->sizeBytes
            || (candidate->sizeBytes == best->sizeBytes
                && rootCandidateHintScore(*candidate) > rootCandidateHintScore(*best))) {
            best = candidate;
        }
    }
    return best;
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

// Chooses the most likely repair component of a disk without privileged
// probing: a mounted installed Linux root wins, otherwise the largest
// Linux-capable filesystem. The previous first-match walk committed the
// 300 MiB ext4 /boot partition before the 15.9 GiB ext4 root on the Alpine
// target. Only when no mountable Linux filesystem is visible does an
// already-unlocked Linux filesystem below a LUKS container (or the encrypted
// container itself) become the candidate, and the disk is the last resort.
const DeviceNode *preferredRepairNode(const DeviceNode &disk)
{
    QList<const DeviceNode *> installedRoots;
    QList<const DeviceNode *> linuxFilesystems;
    collectLinuxRepairCandidates(disk, installedRoots, linuxFilesystems);

    if (const DeviceNode *installed = largestRepairCandidate(installedRoots)) {
        return installed;
    }
    if (const DeviceNode *linuxCapable = largestRepairCandidate(linuxFilesystems)) {
        return linuxCapable;
    }
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

// ---- Shell command tokenization and apt retry rewriting ---------------------

// One token of a shell command line. The source span is preserved exactly so a
// rewrite can be spliced back into the original command without re-quoting;
// value is the token with quotes removed for recognition only.
struct ShellCommandToken {
    QString value;
    bool quoted = false;
    QChar quote;
    int start = 0;
    int end = 0;
};

// Decodes one raw token into its unquoted value. A whole-token single or
// double quoted string is recognized as quoted so a shell wrapper's script can
// be re-wrapped with the same quoting. Anything ambiguous fails closed.
bool decodeShellToken(const QString &raw, ShellCommandToken *token)
{
    token->value.clear();
    token->quoted = false;
    token->quote = QChar();

    if (raw.isEmpty()) {
        return false;
    }

    if (raw.size() >= 2 && raw.at(0) == QLatin1Char('\'')) {
        if (raw.at(raw.size() - 1) != QLatin1Char('\'')
            || raw.indexOf(QLatin1Char('\''), 1) != raw.size() - 1) {
            return false;
        }
        token->value = raw.mid(1, raw.size() - 2);
        token->quoted = true;
        token->quote = QLatin1Char('\'');
        return true;
    }

    if (raw.size() >= 2 && raw.at(0) == QLatin1Char('"')) {
        QString value;
        int index = 1;
        while (index < raw.size()) {
            const QChar ch = raw.at(index);
            if (ch == QLatin1Char('\\')) {
                if (index + 1 >= raw.size()) {
                    return false;
                }
                const QChar escaped = raw.at(index + 1);
                if (escaped == QLatin1Char('"') || escaped == QLatin1Char('\\')
                    || escaped == QLatin1Char('$') || escaped == QLatin1Char('`')) {
                    value += escaped;
                } else {
                    value += ch;
                    value += escaped;
                }
                index += 2;
                continue;
            }
            if (ch == QLatin1Char('"')) {
                if (index != raw.size() - 1) {
                    return false;
                }
                token->value = value;
                token->quoted = true;
                token->quote = QLatin1Char('"');
                return true;
            }
            value += ch;
            ++index;
        }
        return false;
    }

    QString value;
    int index = 0;
    while (index < raw.size()) {
        const QChar ch = raw.at(index);
        if (ch == QLatin1Char('\\')) {
            if (index + 1 >= raw.size()) {
                return false;
            }
            value += raw.at(index + 1);
            index += 2;
            continue;
        }
        if (ch == QLatin1Char('\'') || ch == QLatin1Char('"')) {
            const QChar quote = ch;
            const int closing = raw.indexOf(quote, index + 1);
            if (closing < 0) {
                return false;
            }
            value += raw.mid(index + 1, closing - index - 1);
            index = closing + 1;
            continue;
        }
        value += ch;
        ++index;
    }
    token->value = value;
    return true;
}

// Splits a command line into raw tokens without evaluating anything. A quote
// never splits a token; an unterminated quote makes the whole command
// unrecognizable so callers never rewrite a command they cannot re-splice.
bool tokenizeShellCommand(const QString &command, QList<ShellCommandToken> *tokens)
{
    tokens->clear();
    const int length = command.size();
    int index = 0;
    while (index < length) {
        while (index < length && command.at(index).isSpace()) {
            ++index;
        }
        if (index >= length) {
            break;
        }
        ShellCommandToken token;
        token.start = index;
        bool inSingle = false;
        bool inDouble = false;
        while (index < length) {
            const QChar ch = command.at(index);
            if (inSingle) {
                if (ch == QLatin1Char('\'')) {
                    inSingle = false;
                }
                ++index;
                continue;
            }
            if (inDouble) {
                if (ch == QLatin1Char('\\')) {
                    if (index + 1 >= length) {
                        return false;
                    }
                    index += 2;
                    continue;
                }
                if (ch == QLatin1Char('"')) {
                    inDouble = false;
                }
                ++index;
                continue;
            }
            if (ch == QLatin1Char('\\')) {
                if (index + 1 >= length) {
                    return false;
                }
                index += 2;
                continue;
            }
            if (ch == QLatin1Char('\'')) {
                inSingle = true;
                ++index;
                continue;
            }
            if (ch == QLatin1Char('"')) {
                inDouble = true;
                ++index;
                continue;
            }
            if (ch.isSpace()) {
                break;
            }
            ++index;
        }
        if (inSingle || inDouble) {
            return false;
        }
        token.end = index;
        if (!decodeShellToken(command.mid(token.start, token.end - token.start), &token)) {
            return false;
        }
        tokens->append(token);
    }
    return true;
}

QString shellCommandBasename(const QString &value)
{
    const int slash = value.lastIndexOf(QLatin1Char('/'));
    return slash >= 0 ? value.mid(slash + 1) : value;
}

bool isAptFamilyExecutable(const QString &value)
{
    const QString base = shellCommandBasename(value);
    return base == QStringLiteral("apt")
        || base == QStringLiteral("apt-get")
        || base == QStringLiteral("aptitude");
}

bool isShellWrapperExecutable(const QString &value)
{
    const QString base = shellCommandBasename(value);
    return base == QStringLiteral("sh") || base == QStringLiteral("bash")
        || base == QStringLiteral("dash") || base == QStringLiteral("zsh")
        || base == QStringLiteral("ksh") || base == QStringLiteral("mksh");
}

// Skips leading `sudo` options. Returns the index of the first non-option
// token, or -1 when an option takes a separate value or is unknown: those
// shapes are deliberately not rewritten.
int skipSudoOptions(const QList<ShellCommandToken> &tokens, int index)
{
    static const QString valueLessShortFlags = QStringLiteral("nEHkKASbiesvVhl");
    static const QSet<QString> valueLessLongFlags = {
        QStringLiteral("--non-interactive"), QStringLiteral("--preserve-env"),
        QStringLiteral("--set-home"), QStringLiteral("--login"), QStringLiteral("--shell"),
        QStringLiteral("--stdin"), QStringLiteral("--askpass"), QStringLiteral("--reset-timestamp"),
        QStringLiteral("--background"), QStringLiteral("--validate"), QStringLiteral("--help"),
        QStringLiteral("--version"), QStringLiteral("--edit")
    };
    while (index < tokens.size()) {
        const QString value = tokens.at(index).value;
        if (value == QStringLiteral("--")) {
            return index + 1;
        }
        if (value.startsWith(QStringLiteral("--"))) {
            if (!value.contains(QLatin1Char('=')) && !valueLessLongFlags.contains(value)) {
                return -1;
            }
            ++index;
            continue;
        }
        if (value.startsWith(QLatin1Char('-')) && value.size() > 1) {
            for (int i = 1; i < value.size(); ++i) {
                if (!valueLessShortFlags.contains(value.at(i))) {
                    return -1;
                }
            }
            ++index;
            continue;
        }
        return index;
    }
    return -1;
}

// Skips leading `env` options and VAR=value assignments. Returns the index of
// the command token or -1 for an unrecognized/ambiguous shape.
int skipEnvOptions(const QList<ShellCommandToken> &tokens, int index)
{
    static const QRegularExpression assignment(QStringLiteral("^[A-Za-z_][A-Za-z0-9_]*="));
    while (index < tokens.size()) {
        const QString value = tokens.at(index).value;
        if (value == QStringLiteral("--")) {
            return index + 1;
        }
        if (value == QStringLiteral("-i") || value == QStringLiteral("--ignore-environment")
            || value == QStringLiteral("-0") || value == QStringLiteral("--null")
            || value == QStringLiteral("--debug") || value == QStringLiteral("--help")
            || value == QStringLiteral("--version")) {
            ++index;
            continue;
        }
        if (value == QStringLiteral("-u") || value == QStringLiteral("--unset")
            || value == QStringLiteral("-C") || value == QStringLiteral("--chdir")
            || value == QStringLiteral("-S") || value == QStringLiteral("--split-string")) {
            if (index + 1 >= tokens.size()) {
                return -1;
            }
            index += 2;
            continue;
        }
        if (value.startsWith(QStringLiteral("--unset="))
            || value.startsWith(QStringLiteral("--chdir="))
            || value.startsWith(QStringLiteral("--split-string="))) {
            ++index;
            continue;
        }
        if (assignment.match(value).hasMatch()) {
            ++index;
            continue;
        }
        if (value.startsWith(QLatin1Char('-'))) {
            return -1;
        }
        return index;
    }
    return -1;
}

// Skips an optional `nice` adjustment. Returns the index of the command token
// or -1 for an unrecognized shape.
int skipNiceOptions(const QList<ShellCommandToken> &tokens, int index)
{
    if (index >= tokens.size()) {
        return -1;
    }
    const QString value = tokens.at(index).value;
    if (value == QStringLiteral("--")) {
        return index + 1;
    }
    if (value == QStringLiteral("-n") || value == QStringLiteral("--adjustment")) {
        if (index + 1 >= tokens.size()) {
            return -1;
        }
        bool ok = false;
        tokens.at(index + 1).value.toInt(&ok);
        return ok ? index + 2 : -1;
    }
    if (value.startsWith(QStringLiteral("--adjustment="))) {
        bool ok = false;
        value.mid(13).toInt(&ok);
        return ok ? index + 1 : -1;
    }
    static const QRegularExpression shortAdjustment(QStringLiteral("^-\\d+$"));
    if (shortAdjustment.match(value).hasMatch()) {
        return index + 1;
    }
    if (value.startsWith(QLatin1Char('-'))) {
        return -1;
    }
    return index;
}

// Finds the `-c`-style flag of a shell wrapper. Returns its token index or -1.
int findShellScriptFlag(const QList<ShellCommandToken> &tokens, int index)
{
    while (index < tokens.size()) {
        const QString value = tokens.at(index).value;
        if (value == QStringLiteral("--")) {
            return -1;
        }
        if (value == QStringLiteral("--command")) {
            return index;
        }
        if (!value.startsWith(QLatin1Char('-'))) {
            return -1;
        }
        if (!value.startsWith(QStringLiteral("--"))) {
            bool valid = value.size() > 1;
            bool hasCommand = false;
            for (int i = 1; valid && i < value.size(); ++i) {
                const QChar ch = value.at(i);
                if (!ch.isLetter()) {
                    valid = false;
                    break;
                }
                if (ch == QLatin1Char('c')) {
                    hasCommand = true;
                }
            }
            if (valid && hasCommand) {
                return index;
            }
        }
        ++index;
    }
    return -1;
}

// Recursively rewrites one command. Prefixes (sudo/env/nice) and a single
// level of sh/bash-style -c wrapper are recognized conservatively; the option
// is spliced directly after the apt executable so the subcommand and every
// remaining argument stay byte-for-byte unchanged. Returns an empty string
// when the command is not a confidently recognized apt-family command.
QString rewriteAptReleaseInfoChangeCommand(const QString &command, int depth)
{
    if (depth > 3) {
        return QString();
    }
    QList<ShellCommandToken> tokens;
    if (!tokenizeShellCommand(command, &tokens) || tokens.isEmpty()) {
        return QString();
    }

    int index = 0;
    while (index >= 0 && index < tokens.size()) {
        const ShellCommandToken &token = tokens.at(index);
        const QString base = shellCommandBasename(token.value);
        if (base == QStringLiteral("sudo")) {
            index = skipSudoOptions(tokens, index + 1);
            continue;
        }
        if (base == QStringLiteral("env")) {
            index = skipEnvOptions(tokens, index + 1);
            continue;
        }
        if (base == QStringLiteral("nice")) {
            index = skipNiceOptions(tokens, index + 1);
            continue;
        }
        if (isShellWrapperExecutable(token.value)) {
            const int flagIndex = findShellScriptFlag(tokens, index + 1);
            if (flagIndex < 0 || flagIndex + 1 >= tokens.size()) {
                return QString();
            }
            const ShellCommandToken &script = tokens.at(flagIndex + 1);
            if (!script.quoted) {
                return QString();
            }
            const QString rewrittenScript = rewriteAptReleaseInfoChangeCommand(script.value, depth + 1);
            if (rewrittenScript.isEmpty() || rewrittenScript.contains(script.quote)) {
                return QString();
            }
            return command.left(script.start) + script.quote + rewrittenScript
                + script.quote + command.mid(script.end);
        }
        if (isAptFamilyExecutable(token.value)) {
            return command.left(token.end)
                + QStringLiteral(" -o Acquire::AllowReleaseInfoChange=true")
                + command.mid(token.end);
        }
        return QString();
    }
    return QString();
}
} // namespace

// ---- Busy indicator ----------------------------------------------------------

BusyIndicatorWidget::BusyIndicatorWidget(QWidget *parent)
    : QWidget(parent)
    , m_animationTimer(new QTimer(this))
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(8);
    setMinimumWidth(110);
    setMaximumWidth(150);
    m_animationTimer->setInterval(40);
    connect(m_animationTimer, &QTimer::timeout, this, [this] {
        // Fixed phase steps keep the marquee speed independent of the platform
        // style, the theme and the display refresh rate.
        m_phase += 0.05;
        if (m_phase >= 1.0) {
            m_phase -= 1.0;
        }
        update();
    });
}

bool BusyIndicatorWidget::isAnimating() const
{
    return m_animationTimer && m_animationTimer->isActive();
}

qreal BusyIndicatorWidget::animationPhase() const
{
    return m_phase;
}

void BusyIndicatorWidget::setAnimating(bool animating)
{
    m_shouldAnimate = animating;
    if (animating) {
        if (isVisible()) {
            m_animationTimer->start();
        }
    } else {
        m_animationTimer->stop();
    }
    update();
}

void BusyIndicatorWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_shouldAnimate) {
        m_animationTimer->start();
    }
}

void BusyIndicatorWidget::hideEvent(QHideEvent *event)
{
    m_animationTimer->stop();
    QWidget::hideEvent(event);
}

void BusyIndicatorWidget::changeEvent(QEvent *event)
{
    // The marquee derives every color from the current palette, so a theme or
    // palette switch only needs a repaint.
    if (event->type() == QEvent::PaletteChange
        || event->type() == QEvent::ApplicationPaletteChange
        || event->type() == QEvent::StyleChange) {
        update();
    }
    QWidget::changeEvent(event);
}

void BusyIndicatorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const QRectF track = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    if (track.width() <= 1.0 || track.height() <= 1.0) {
        return;
    }

    // Classic progress-bar groove: a recessed palette tone with a thin border,
    // so the widget stays visible on light and dark themes without depending
    // on a platform progress style.
    QColor borderColor = palette().color(QPalette::Mid);
    if (!borderColor.isValid()) {
        borderColor = palette().color(QPalette::WindowText);
    }
    QColor grooveColor = palette().color(QPalette::Base);
    if (!grooveColor.isValid()) {
        grooveColor = palette().color(QPalette::Window);
    }
    painter.setPen(borderColor);
    painter.setBrush(grooveColor);
    painter.drawRoundedRect(track, 2.0, 2.0);

    // Striped marquee: evenly pitched diagonal stripes slide across the groove
    // and wrap around, clipped to the groove outline. Stripe positions depend
    // only on the phase, so every distribution paints the same moving pattern.
    QColor stripeColor = palette().color(QPalette::Highlight);
    if (!stripeColor.isValid() || stripeColor.alpha() == 0) {
        stripeColor = palette().color(QPalette::Link);
    }
    if (!stripeColor.isValid() || stripeColor.alpha() == 0) {
        stripeColor = palette().color(QPalette::WindowText);
    }
    const qreal stripeWidth = 6.0;
    const qreal stripePitch = 12.0;
    const qreal slant = track.height();
    QPainterPath clip;
    clip.addRoundedRect(track, 2.0, 2.0);
    painter.save();
    painter.setClipPath(clip);
    painter.setPen(Qt::NoPen);
    painter.setBrush(stripeColor);
    for (qreal x = track.left() - slant - stripePitch + m_phase * stripePitch;
         x < track.right() + stripePitch; x += stripePitch) {
        QPolygonF stripe;
        stripe << QPointF(x, track.bottom() + 1.0)
               << QPointF(x + stripeWidth, track.bottom() + 1.0)
               << QPointF(x + stripeWidth + slant, track.top() - 1.0)
               << QPointF(x + slant, track.top() - 1.0);
        painter.drawPolygon(stripe);
    }
    painter.restore();
}

// ---- Numeric-aware table/tree item sorting ----------------------------------

SortableTableItem::SortableTableItem(const QString &text, const QVariant &sortKey)
    : QTableWidgetItem(text)
{
    if (sortKey.isValid()) {
        setData(BootRepairSortKeyRole, sortKey);
    }
}

bool SortableTableItem::operator<(const QTableWidgetItem &other) const
{
    int comparison = 0;
    if (compareExplicitSortKeys(data(BootRepairSortKeyRole),
                                other.data(BootRepairSortKeyRole), &comparison)
        && comparison != 0) {
        return comparison < 0;
    }
    return naturalTextCompare(text(), other.text()) < 0;
}

bool SortableTreeWidgetItem::operator<(const QTreeWidgetItem &other) const
{
    const QTreeWidget *view = treeWidget();
    int column = view ? view->sortColumn() : 0;
    if (column < 0 || column >= columnCount()) {
        column = 0;
    }

    int comparison = 0;
    if (compareExplicitSortKeys(data(column, BootRepairSortKeyRole),
                                other.data(column, BootRepairSortKeyRole), &comparison)
        && comparison != 0) {
        return comparison < 0;
    }

    // The first column is the stable row identity (drive/partition name or
    // tool name), so it breaks ties before the clicked column's text and the
    // device path/tool key stored in its UserRole keep the order total and
    // deterministic.
    const int nameComparison = naturalTextCompare(text(0), other.text(0));
    if (nameComparison != 0) {
        return nameComparison < 0;
    }
    const int columnComparison = naturalTextCompare(text(column), other.text(column));
    if (columnComparison != 0) {
        return columnComparison < 0;
    }
    return naturalTextCompare(data(0, Qt::UserRole).toString(),
                              other.data(0, Qt::UserRole).toString()) < 0;
}

// ---- FilesystemRepairDialog -------------------------------------------------

FilesystemRepairDialog::FilesystemRepairDialog(const QString &scopeLabel,
                                               const QList<FilesystemCheckResult> &results,
                                               const QList<QStringList> &modes,
                                               QWidget *parent)
    : QDialog(parent)
    , m_results(results)
{
    setWindowTitle(QStringLiteral("Repair file system errors"));
    resize(820, 480);
    setMinimumSize(520, 360);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(10);

    auto *heading = new QLabel(QStringLiteral(
        "The read-only check found file system errors in the %1. "
        "Select the devices to repair and the mode to use. "
        "Each repair runs separately after the helper repeats its scope, mount-state and tool preflights.")
        .arg(scopeLabel));
    heading->setWordWrap(true);
    layout->addWidget(heading);

    auto *table = new QTableWidget(static_cast<int>(results.size()), 5, this);
    m_table = table;
    table->setObjectName(QStringLiteral("filesystemRepairTable"));
    table->setHorizontalHeaderLabels({
        QStringLiteral("Repair"), QStringLiteral("Device"),
        QStringLiteral("File system"), QStringLiteral("Mode"),
        QStringLiteral("Issue summary")
    });
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setWordWrap(true);
    table->setTextElideMode(Qt::ElideRight);
    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(qMax(24, table->fontMetrics().lineSpacing() + 10));
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setMinimumHeight(160);

    for (int row = 0; row < results.size(); ++row) {
        const FilesystemCheckResult &check = results.at(row);
        auto *checkItem = new QTableWidgetItem;
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        checkItem->setCheckState(Qt::Checked);
        table->setItem(row, 0, checkItem);

        auto *deviceItem = new SortableTableItem(check.device);
        deviceItem->setToolTip(check.uuid.isEmpty()
            ? check.device
            : QStringLiteral("%1\nUUID: %2").arg(check.device, check.uuid));
        table->setItem(row, 1, deviceItem);

        const bool mounted = !check.mount.isEmpty() && check.mount != QStringLiteral("unmounted");
        const QString filesystemLabel = check.fstype.isEmpty() ? QStringLiteral("unknown") : check.fstype;
        auto *filesystemItem = new SortableTableItem(filesystemLabel);
        filesystemItem->setToolTip(QStringLiteral("Device: %1\nFile system: %2\nMount: %3\nCheck tool: %4")
            .arg(check.device, filesystemLabel,
                 mounted ? check.mount : QStringLiteral("unmounted"),
                 check.tool.isEmpty() ? QStringLiteral("unknown") : check.tool));
        table->setItem(row, 2, filesystemItem);

        auto *modeCombo = new QComboBox(table);
        modeCombo->setObjectName(QStringLiteral("filesystemRepairModeCombo"));
        modeCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        modeCombo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        modeCombo->setFixedHeight(qMax(modeCombo->sizeHint().height(),
                                       table->fontMetrics().lineSpacing() + 10));
        const QStringList availableModes = modes.value(row);
        for (const QString &mode : availableModes) {
            modeCombo->addItem(mode);
        }
        modeCombo->setCurrentIndex(0);
        table->setCellWidget(row, 3, modeCombo);

        const QString summary = check.detail.isEmpty()
            ? QStringLiteral("The read-only check reported issues.")
            : check.detail;
        auto *summaryItem = new SortableTableItem(summary);
        summaryItem->setToolTip(summary);
        table->setItem(row, 4, summaryItem);
    }
    // Header-click sorting is enabled only after the rows are complete:
    // QTableWidgetItem insertion while sorting is active can reorder or lose
    // rows mid-fill. The default order is Device ascending, and sorting keeps
    // each device's checkbox and mode combo attached to its row.
    table->setSortingEnabled(true);
    table->sortByColumn(1, Qt::AscendingOrder);
    layout->addWidget(table, 1);

    auto *footer = new QHBoxLayout;
    auto *offlineNotice = new QLabel(QStringLiteral(
        "Offline repair modes refuse mounted filesystems; btrfs scrub and zpool scrub are online modes and require a mounted filesystem. "
        "btrfs check --repair asks for an extra backup warning before it runs."));
    offlineNotice->setWordWrap(true);
    footer->addWidget(offlineNotice, 1);
    layout->addLayout(footer);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    m_repairButton = buttons->addButton(QStringLiteral("Run Selected Repairs"),
                                        QDialogButtonBox::AcceptRole);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem *item) {
        if (!item || item->column() != 0 || !m_table) {
            return;
        }
        if (auto *combo = qobject_cast<QComboBox *>(m_table->cellWidget(item->row(), 3))) {
            combo->setEnabled(item->checkState() == Qt::Checked);
        }
        updateRepairButtonState();
    });
    layout->addWidget(buttons);

    updateRepairButtonState();
}

QList<FilesystemRepairDialog::Selection> FilesystemRepairDialog::selections() const
{
    QList<Selection> selections;
    if (!m_table) {
        return selections;
    }
    for (int row = 0; row < m_table->rowCount(); ++row) {
        QTableWidgetItem *checkItem = m_table->item(row, 0);
        if (!checkItem || checkItem->checkState() != Qt::Checked) {
            continue;
        }
        auto *combo = qobject_cast<QComboBox *>(m_table->cellWidget(row, 3));
        if (!combo || combo->currentIndex() < 0) {
            continue;
        }
        Selection selection;
        selection.device = m_results.at(row).device;
        selection.mode = combo->currentText();
        selections.append(selection);
    }
    return selections;
}

void FilesystemRepairDialog::updateRepairButtonState()
{
    if (!m_repairButton) {
        return;
    }
    bool anyChecked = false;
    if (m_table) {
        for (int row = 0; row < m_table->rowCount(); ++row) {
            QTableWidgetItem *item = m_table->item(row, 0);
            if (item && item->checkState() == Qt::Checked) {
                anyChecked = true;
                break;
            }
        }
    }
    m_repairButton->setEnabled(anyChecked);
}

// ---- MainWindow: construction and lifecycle ---------------------------------

// The process owns at most one active session file per run. It stays empty on
// a fresh launch and is only set once a scope identification creates the file;
// a second MainWindow constructed in the same process adopts it so both
// windows continue the same run instead of starting or adopting a session.
QString MainWindow::s_activeSessionPath;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_scanner(new SystemScanner(this))
    , m_settings(new QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"), this))
{
    setWindowTitle(QStringLiteral("Boot Bitch"));
    setWindowIcon(QApplication::windowIcon());
    setMinimumSize(480, 500);
    resize(1180, 760);

    // Follow the desktop color scheme from the first paint and whenever the
    // desktop switches it at runtime, so a dark GNOME/Fedora session can never
    // render the application light (and vice versa). The portal is read before
    // the first paint because Qt's auto-loaded GTK3 platform theme reports
    // Adwaita light even in a dark GNOME session; the platform palette is only
    // overridden when it does not already match the resolved scheme.
    queryPortalColorScheme();
    applyColorScheme();
    if (QGuiApplication::styleHints()) {
        connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                this, [this](Qt::ColorScheme) { applyColorScheme(); });
    }
    // The platform theme can publish the scheme only after the first paint and
    // does not emit colorSchemeChanged when the desktop was already dark
    // before the launch. Re-resolve once the event loop runs and once more
    // after the portal has had time to answer; both checks are no-ops when the
    // scheme is already known and matches.
    QTimer::singleShot(0, this, [this] { refreshColorScheme(); });
    QTimer::singleShot(250, this, [this] { refreshColorScheme(); });
    // If the portal stayed mute (slow service, no bus) the GNOME preference is
    // still available through the read-only gsettings fallback. The portal
    // reply stays authoritative when it arrives later.
    QTimer::singleShot(600, this, [this] {
        if (m_portalColorScheme == Qt::ColorScheme::Unknown) {
            queryGsettingsColorScheme();
        }
    });
    // A late or changed platform palette carries the desktop scheme with it
    // (and must be re-rendered even when the scheme itself is unchanged). The
    // ApplicationPaletteChange/ThemeChange events are the documented
    // replacement for the deprecated QGuiApplication::paletteChanged signal.
    QApplication::instance()->installEventFilter(this);

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

    // Compact global busy indicator for every asynchronous operation. It lives
    // in a reserved slot on the right edge of the header, below the mode badge,
    // so showing it never moves existing widgets or changes the window height:
    // the slot retains its height while the indicator is hidden.
    m_busyIndicator = new QWidget;
    m_busyIndicator->setObjectName(QStringLiteral("busyIndicator"));
    auto *busyLayout = new QHBoxLayout(m_busyIndicator);
    busyLayout->setContentsMargins(0, 0, 0, 0);
    busyLayout->setSpacing(8);
    // The indicator is a custom-painted striped progress bar instead of a
    // QProgressBar: the platform style renders an indeterminate QProgressBar
    // as a static solid bar on some distributions (for example Fedora/Adwaita)
    // and as different animated shapes on others. The custom widget animates
    // identically everywhere and derives its colors from the current palette.
    m_busyProgress = new BusyIndicatorWidget;
    m_busyProgress->setObjectName(QStringLiteral("busyProgress"));
    m_busyProgress->setAccessibleName(QStringLiteral("Operation in progress"));
    m_busyProgress->setAccessibleDescription(QStringLiteral("A Boot Bitch operation is running in the background."));
    m_busyProgress->setToolTip(QStringLiteral("A Boot Bitch operation is running in the background."));
    m_busyStatusLabel = new ElidedLabel;
    m_busyStatusLabel->setObjectName(QStringLiteral("busyStatusLabel"));
    m_busyStatusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_busyStatusLabel->setToolTip(QStringLiteral("A Boot Bitch operation is running in the background."));
    busyLayout->addWidget(m_busyProgress);
    busyLayout->addWidget(m_busyStatusLabel, 1);
    m_busyIndicator->setVisible(false);
    QSizePolicy busyPolicy = m_busyIndicator->sizePolicy();
    busyPolicy.setRetainSizeWhenHidden(true);
    m_busyIndicator->setSizePolicy(busyPolicy);
    m_busyIndicator->setFixedHeight(m_busyStatusLabel->sizeHint().height());

    auto *busyRow = new QHBoxLayout;
    busyRow->setContentsMargins(0, 0, 0, 0);
    busyRow->addStretch(1);
    busyRow->addWidget(m_busyIndicator);

    auto *headerColumn = new QVBoxLayout;
    headerColumn->setSpacing(4);
    headerColumn->addLayout(headerLayout);
    headerColumn->addLayout(busyRow);
    mainLayout->addLayout(headerColumn);

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
    // Dense Logs and File Copy button rows stay shrinkable so they cannot
    // overlap at the minimum supported window width.
    if (m_sessionLogPanel) {
        const QList<QPushButton *> sessionButtons = m_sessionLogPanel->findChildren<QPushButton *>();
        for (QPushButton *button : sessionButtons) {
            makeButtonShrinkable(button);
        }
    }
    if (m_fileCopySourceBox) {
        const QList<QPushButton *> sourceButtons = m_fileCopySourceBox->findChildren<QPushButton *>();
        for (QPushButton *button : sourceButtons) {
            makeButtonShrinkable(button);
        }
    }
    // Buttons whose label is replaced at runtime may need to elide when a
    // narrow window compresses their row instead of forcing the row wider.
    makeButtonShrinkable(m_unlockTargetButton, 96);
    makeButtonShrinkable(m_runAllDiagnosticsButton);
    makeButtonShrinkable(m_runDiagnosticButton);
    makeButtonShrinkable(m_repairToolButton, 96);
    makeButtonShrinkable(m_chrootShellRunButton);
    // The Systems page presents its actions in one right-hand column: Refresh
    // Devices above the host card's Details, Host Maintenance and Make Default
    // buttons. Standardize the column after the global sizing pass so every
    // member keeps one width and one height at every window width; the
    // enter/exit maintenance labels are bounded by that shared width.
    standardizeButtonColumn({m_refreshDevicesButton, m_hostDetailsButton,
                             m_hostMaintenanceButton, m_hostDefaultButton});
    statusBar()->showMessage(QStringLiteral("Ready — guarded repair mode"));

    // Hint that each page scrolls with a soft edge shadow whenever content
    // continues below or above the viewport.
    for (QScrollArea *area : findChildren<QScrollArea *>()) {
        new ScrollFadeOverlay(area);
    }

    loadSettings();
    // A fresh launch starts with an empty live log; the newest prior session
    // file is never adopted as the current session. Only a second window in
    // this process continues the run's active file. Then enforce the retention
    // bounds before this window writes anything of its own.
    adoptActiveSessionLog();
    pruneSessionLogs();
    refreshSessionLogList();
    updateResponsiveLayout();
    refreshCapabilities();
    refreshDevices();
    // If a target is established by a restored session or an early device
    // selection, begin snapshot discovery after the first event loop turn so
    // Systems remains the initial responsive page.
    QTimer::singleShot(0, this, &MainWindow::scheduleSnapshotPreload);
    // Plan rows wrap against the final viewport width, so measure the plan
    // list once the first layout pass after show() has settled.
    QTimer::singleShot(0, this, &MainWindow::updateFullRepairPlanHeight);
    appendLog(QStringLiteral("Boot Bitch %1 started in guarded repair mode.").arg(QCoreApplication::applicationVersion()));
}

MainWindow::~MainWindow()
{
    closeSessionLogFile();
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
    // Wrapped plan rows change height with the viewport width; re-measure once
    // the layout has settled instead of on every intermediate resize step.
    QTimer::singleShot(0, this, &MainWindow::updateFullRepairPlanHeight);
    QTimer::singleShot(0, this, [this] { resizeCapabilityRows(); });
}

// The platform can apply or replace the desktop palette after construction
// (for example when the GNOME theme finishes loading), so every application
// palette or theme change re-renders the palette-derived custom colors and
// re-resolves the scheme. Our own correction is skipped through the guard so
// setPalette() does not recurse.
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == QApplication::instance()
        && (event->type() == QEvent::ApplicationPaletteChange
            || event->type() == QEvent::ThemeChange)) {
        if (!m_applyingColorScheme) {
            updateThemeDependentColors();
            scheduleColorSchemeRefresh();
        }
    }
    // Keep the shrink-only columns and the stretched Status column in step with
    // the pane width; a wrap-width change also changes the wrapped row heights.
    if (m_deviceTree && watched == m_deviceTree->viewport()
        && event->type() == QEvent::Resize) {
        applyDeviceColumnLayout();
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::beginBusyOperation(const QString &label)
{
    m_activeBusyOperations.append(label);
    updateBusyIndicator();
}

void MainWindow::endBusyOperation(const QString &label)
{
    // Remove one occurrence so overlapping operations that share a label stay
    // balanced: the indicator hides only after the last matching operation.
    const qsizetype index = m_activeBusyOperations.lastIndexOf(label);
    if (index >= 0) {
        m_activeBusyOperations.removeAt(index);
    }
    updateBusyIndicator();
}

void MainWindow::updateBusyIndicator()
{
    if (!m_busyIndicator) {
        return;
    }

    const bool busy = !m_activeBusyOperations.isEmpty();
    const QString label = busy ? m_activeBusyOperations.constLast() : QString();
    if (m_busyStatusLabel) {
        m_busyStatusLabel->setText(label);
        // The visible text may be elided in the compact header slot; the
        // tooltip always carries the complete operation label.
        m_busyStatusLabel->setToolTip(label.isEmpty()
            ? QStringLiteral("A Boot Bitch operation is running in the background.")
            : label);
    }
    m_busyIndicator->setVisible(busy);
    if (m_busyProgress) {
        // The custom striped bar follows the reference-counted busy state
        // exactly: it starts with the first operation and stops with the last.
        m_busyProgress->setAnimating(busy);
    }
    emit busyStateChanged(busy, label);
}

// ---- MainWindow: menu and page construction ---------------------------------

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
    // Pin the page to the viewport width like the Diagnostics, Repair and
    // Snapshots pages: the section frames fit the window and their own scroll
    // areas (device tree, selected-drive details) scroll horizontally when
    // their content is wider, instead of the whole page clipping at the edge.
    content->setMinimumWidth(0);
    content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    auto *topRow = new QHBoxLayout;
    topRow->addWidget(sectionTitle(QStringLiteral("Systems")));
    topRow->addWidget(contextHelpButton(page, QStringLiteral("Systems"),
        QStringLiteral("Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system.")));
    topRow->addStretch(1);
    m_refreshDevicesButton = new ElidedPushButton(themedIcon(QStringLiteral("view-refresh")), QStringLiteral("Refresh Devices"));
    connect(m_refreshDevicesButton, &QPushButton::clicked, this, &MainWindow::refreshDevices);
    topRow->addWidget(m_refreshDevicesButton, 0, Qt::AlignTop);
    layout->addLayout(topRow);

    // Compact, permanently protected running-host card. It is never part of
    // the ordinary candidate list, but its read-only details and explicit
    // maintenance path remains available.
    auto *hostBox = new QFrame;
    hostBox->setFrameShape(QFrame::StyledPanel);
    // The card reflows with the window: identity and actions side by side when
    // wide, identity above the actions when narrow, so the running-system text
    // keeps a readable width instead of wrapping into a sliver beside the
    // buttons.
    auto *hostLayout = new QBoxLayout(QBoxLayout::LeftToRight, hostBox);
    m_hostCardLayout = hostLayout;
    hostLayout->setContentsMargins(10, 8, 10, 8);
    hostLayout->setSpacing(9);

    auto *hostIdentity = new QWidget;
    auto *identityLayout = new QHBoxLayout(hostIdentity);
    identityLayout->setContentsMargins(0, 0, 0, 0);
    identityLayout->setSpacing(9);

    auto *hostIcon = new QLabel;
    // Use the bundled protected-host shield so distro icon themes cannot
    // substitute an unrelated distribution crest (for example Arch's flag).
    QIcon protectedIcon = bundledIcon(QStringLiteral("security-high"));
    if (protectedIcon.isNull()) protectedIcon = themedIcon(QStringLiteral("drive-harddisk"));
    hostIcon->setPixmap(protectedIcon.pixmap(30, 30));
    hostIcon->setFixedSize(34, 34);
    hostIcon->setAlignment(Qt::AlignCenter);
    identityLayout->addWidget(hostIcon, 0, Qt::AlignTop);

    auto *hostText = new QVBoxLayout;
    hostText->setSpacing(2);

    m_hostSystemLabel = new QLabel(QStringLiteral("Detecting running system…"));
    QFont hostFont = m_hostSystemLabel->font();
    hostFont.setBold(true);
    m_hostSystemLabel->setFont(hostFont);
    m_hostSystemLabel->setWordWrap(true);
    hostText->addWidget(m_hostSystemLabel);

    // The protected-storage summary can be long (model, device, size,
    // transport and critical mounts). Keep it wrapped so a narrow window
    // grows the card instead of hiding the tail of the line; subtleLabel()
    // already enables word wrap and these two labels share the pattern.
    m_hostStorageLabel = subtleLabel(QStringLiteral("Detecting protected storage…"));
    m_hostStorageLabel->setMinimumWidth(0);
    m_hostStorageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_hostMountsLabel = subtleLabel(QStringLiteral(""));
    m_hostMountsLabel->setMinimumWidth(0);
    m_hostMountsLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    hostText->addWidget(m_hostStorageLabel);
    hostText->addWidget(m_hostMountsLabel);
    identityLayout->addLayout(hostText, 1);
    hostLayout->addWidget(hostIdentity, 1);

    auto *hostActions = new QWidget;
    // A grid (not a row) so the actions can wrap onto a second line when the
    // window is narrow and every button stays visible.
    auto *actionsLayout = new QGridLayout(hostActions);
    m_hostActionsLayout = actionsLayout;
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setHorizontalSpacing(9);
    actionsLayout->setVerticalSpacing(6);

    m_hostProtectedBadge = new QLabel(QStringLiteral("PROTECTED"));
    QFont protectedFont = m_hostProtectedBadge->font();
    protectedFont.setBold(true);
    m_hostProtectedBadge->setFont(protectedFont);
    m_hostProtectedBadge->setFrameShape(QFrame::StyledPanel);
    m_hostProtectedBadge->setContentsMargins(8, 4, 8, 4);
    m_hostProtectedBadge->setToolTip(QStringLiteral("The running host remains protected from ordinary repair-target operations."));
    actionsLayout->addWidget(m_hostProtectedBadge, 0, 0, Qt::AlignLeft | Qt::AlignTop);

    m_hostDetailsButton = new QPushButton(themedIcon(QStringLiteral("document-preview")), QStringLiteral("Details"));
    m_hostDetailsButton->setEnabled(false);
    m_hostDetailsButton->setToolTip(QStringLiteral("Show read-only details for the protected running host."));
    connect(m_hostDetailsButton, &QPushButton::clicked, this, &MainWindow::showHostDetails);
    actionsLayout->addWidget(m_hostDetailsButton, 0, 1, Qt::AlignLeft | Qt::AlignTop);

    m_hostMaintenanceButton = new ElidedPushButton(themedIcon(QStringLiteral("system-run")), QStringLiteral("Host Maintenance"));
    m_hostMaintenanceButton->setEnabled(false);
    m_hostMaintenanceButton->setToolTip(QStringLiteral(
        "Host Maintenance — select the running host for deliberate guarded maintenance. All supported repair stages run against the active system; running-host Snapper @ snapshots are available in the Snapshots tab, while the chroot shell and file-copy workflows remain separate target tools."));
    connect(m_hostMaintenanceButton, &QPushButton::clicked, this, &MainWindow::selectHostForMaintenance);
    actionsLayout->addWidget(m_hostMaintenanceButton, 0, 2, Qt::AlignLeft | Qt::AlignTop);

    m_hostDefaultButton = new ElidedPushButton(themedIcon(QStringLiteral("preferences-system")), QStringLiteral("Make Default"));
    m_hostDefaultButton->setEnabled(false);
    m_hostDefaultButton->setToolTip(QStringLiteral(
        "Restore the host's uniquely identified TUXEDO UKI entry if needed, then place it first in BootOrder while preserving every other entry."));
    connect(m_hostDefaultButton, &QPushButton::clicked, this, &MainWindow::setHostDefaultBootEntry);
    actionsLayout->addWidget(m_hostDefaultButton, 0, 3, Qt::AlignLeft | Qt::AlignTop);

    hostLayout->addWidget(hostActions, 0);

    // Refresh Devices sits above the host card in the same right-hand action
    // column. Match the card's inner right inset so the stacked buttons share
    // one right edge instead of being offset by the card frame and margin.
    topRow->setContentsMargins(0, 0, hostBox->frameWidth() + hostLayout->contentsMargins().right(), 0);

    layout->addWidget(hostBox);

    auto *candidateRow = new QHBoxLayout;
    candidateRow->addWidget(sectionTitle(QStringLiteral("Available repair targets")));
    candidateRow->addWidget(contextHelpButton(page, QStringLiteral("Repair targets"),
        QStringLiteral("Drives are ranked by visible Linux, EFI, filesystem and encryption evidence. Select the top-level drive; partitions and mapped volumes are informational.")));
    candidateRow->addStretch(1);
    auto *sortHint = new QLabel(QStringLiteral("Most likely first"));
    sortHint->setToolTip(QStringLiteral(
        "Candidates are ranked by visible Linux, EFI, filesystem and encryption evidence. "
        "Click a column header to sort by that column; click it again to reverse the order."));
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
    // The Status column owns its two-line wrap/elide layout and its row
    // heights; the style-independent delegate is what keeps Fusion, Breeze and
    // Adwaita rendering identical.
    m_deviceTree->setItemDelegateForColumn(1, new DeviceStatusDelegate(m_deviceTree, m_deviceTree));
    // The dynamic column policy follows every viewport width change (window
    // resize, splitter drag, scrollbar appearance) instead of only the
    // construction-time defaults.
    m_deviceTree->viewport()->installEventFilter(this);
    applyDeviceColumnLayout();
    // Header-click sorting. The default orders candidates by repair
    // likelihood through the Status column's numeric rank, which preserves the
    // historical "most likely first" ranking; every other column sorts by its
    // real value (Size by bytes, names/devices in natural order) and clicking
    // a header toggles ascending/descending. Protected running-host rows are
    // never rendered in this tree, so no row has to stay pinned.
    m_deviceTree->setSortingEnabled(true);
    m_deviceTree->sortByColumn(1, Qt::AscendingOrder);
    leftLayout->addWidget(m_deviceTree, 0);

    connect(m_deviceTree, &QTreeWidget::itemSelectionChanged, this, &MainWindow::updateDeviceDetails);
    connect(m_deviceTree, &QTreeWidget::itemExpanded, this, [this] { updateDeviceTreeHeight(); });
    connect(m_deviceTree, &QTreeWidget::itemCollapsed, this, [this] { updateDeviceTreeHeight(); });

    auto *buttonRow = new QHBoxLayout;
    m_setTargetButton = new QPushButton(themedIcon(QStringLiteral("dialog-ok-apply")), QStringLiteral("Select Target"));
    m_setTargetButton->setEnabled(false);
    connect(m_setTargetButton, &QPushButton::clicked, this, &MainWindow::setPreviewTarget);

    m_unlockTargetButton = new ElidedPushButton(themedIcon(QStringLiteral("object-unlocked")), QStringLiteral("Unlock"));
    m_unlockTargetButton->setEnabled(false);
    m_unlockTargetButton->setToolTip(QStringLiteral("Select a drive whose detected target is a locked LUKS volume."));
    connect(m_unlockTargetButton, &QPushButton::clicked, this, &MainWindow::unlockSelectedTarget);

    buttonRow->addWidget(m_setTargetButton);
    buttonRow->addWidget(m_unlockTargetButton);

    // Non-modal affordance for a deferred Polkit request: the scope stays
    // selected, but the user can establish the privileged session at any time
    // instead of waiting for the next privileged action.
    m_authorizationStatusLabel = new QLabel;
    m_authorizationStatusLabel->setWordWrap(false);
    m_authorizationStatusLabel->setVisible(false);
    m_authorizationStatusLabel->setToolTip(QStringLiteral(
        "Administrator authorization was deferred for the current scope. Diagnostics and repairs stay available; the next privileged action will request authorization again, or press Authorize to establish the session now."));
    buttonRow->addWidget(m_authorizationStatusLabel, 0, Qt::AlignVCenter);

    m_authorizeNowButton = new QPushButton(themedIcon(QStringLiteral("dialog-password")), QStringLiteral("Authorize"));
    m_authorizeNowButton->setVisible(false);
    m_authorizeNowButton->setToolTip(QStringLiteral(
        "Establish the privileged Boot Bitch helper session for the current scope now instead of waiting for the next privileged action."));
    connect(m_authorizeNowButton, &QPushButton::clicked, this, &MainWindow::authorizePrivilegedSessionNow);
    buttonRow->addWidget(m_authorizeNowButton, 0, Qt::AlignVCenter);

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
    // Long unbreakable values (UUIDs, device paths) scroll inside the frame
    // instead of forcing the Systems page wider than the window.
    detailsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
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
    m_systemSplitter->setHandleWidth(6);
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

    // Responsive title row: title and help keep the first row; the wrapping
    // scope label and Run All share the action row (scope left of the button)
    // while there is room and drop together to a second, right-aligned row
    // when the page gets too narrow.
    auto *heading = new QGridLayout;
    heading->setContentsMargins(0, 0, 0, 0);
    QLabel *diagnosticsHeading = sectionTitle(QStringLiteral("Diagnostics"));
    heading->addWidget(diagnosticsHeading, 0, 0);
    QToolButton *diagnosticsHelp = contextHelpButton(page, QStringLiteral("Diagnostics"),
        QStringLiteral("Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while Host Maintenance is active. Running Host diagnostics are available only inside the explicit Host Maintenance scope: enter Host Maintenance on the protected running-host card in Systems first. Both scopes provide read-only diagnostics; host EFI/UKI checks inspect the active ESP and firmware entries, while repair-drive checks mount only that system read-only."));
    heading->addWidget(diagnosticsHelp, 0, 1);

    // Standard top-right scope line shared with the other tabs: it follows the
    // Systems page live (the committed repair target disk, or the protected
    // running host while Host Maintenance is active) and never offers a second,
    // competing scope selector. It wraps into the two-line form
    // ("Host maintenance:" then the path) and is never elided away.
    m_diagnosticScopeLabel = new WrappedScopeLabel;
    m_diagnosticScopeLabel->setText(QStringLiteral("Target: none selected"));
    heading->addWidget(m_diagnosticScopeLabel, 0, 2, Qt::AlignRight | Qt::AlignVCenter);

    // Run All closes the title row at the content edge: scope label to its
    // left, vertically centered with the Diagnostics title.
    m_runAllDiagnosticsButton = new ElidedPushButton(themedIcon(QStringLiteral("system-run")), QStringLiteral("Run All"));
    m_runAllDiagnosticsButton->setToolTip(QStringLiteral(
        "Run All — run every available read-only diagnostic for the current scope."));
    heading->addWidget(m_runAllDiagnosticsButton, 0, 3, Qt::AlignVCenter);
    heading->setColumnStretch(2, 1);
    m_targetConfigCombo = new QComboBox;
    m_targetConfigCombo->addItem(QStringLiteral("/etc/fstab"), QStringLiteral("fstab"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/crypttab"), QStringLiteral("crypttab"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/default/grub"), QStringLiteral("grub-defaults"));
    m_targetConfigCombo->addItem(QStringLiteral("/boot/grub/grub.cfg"), QStringLiteral("grub-config"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/sddm.conf"), QStringLiteral("sddm"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/gdm3/daemon.conf"), QStringLiteral("gdm3"));
    m_targetConfigCombo->addItem(QStringLiteral("/etc/gdm/custom.conf"), QStringLiteral("gdm"));
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
    new ResponsiveHeaderReflow(page, layout, heading, diagnosticsHeading, diagnosticsHelp,
                               m_diagnosticScopeLabel, {m_runAllDiagnosticsButton});
    auto *configRow = new QHBoxLayout;
    configRow->setContentsMargins(0, 0, 0, 0);
    m_targetConfigLabel = new QLabel(QStringLiteral("Target configuration:"));
    m_targetConfigLabel->setVisible(false);
    configRow->addWidget(m_targetConfigLabel);
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

    // The run/re-run action belongs to the Results area: it shares the heading
    // row (title left, button right, vertically centered) instead of floating
    // between the availability line and the results pane, so the freed row
    // goes to the results text.
    auto *resultsHeader = new QHBoxLayout;
    resultsHeader->setContentsMargins(0, 0, 0, 0);
    m_diagnosticResultsTitle = sectionTitle(QStringLiteral("Results"));
    resultsHeader->addWidget(m_diagnosticResultsTitle);
    resultsHeader->addStretch(1);
    m_runDiagnosticButton = new ElidedPushButton(themedIcon(QStringLiteral("system-run")), QStringLiteral("Run Diagnostic"));
    m_runDiagnosticButton->setToolTip(QStringLiteral(
        "Run Diagnostic — run the selected read-only diagnostic for the current scope; cached results offer a re-run."));
    resultsHeader->addWidget(m_runDiagnosticButton, 0, Qt::AlignVCenter);
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
    m_diagnosticSplitter->setHandleWidth(6);
    m_diagnosticSplitter->setStretchFactor(0, 2);
    m_diagnosticSplitter->setStretchFactor(1, 5);
    m_diagnosticSplitter->setSizes({280, 760});
    layout->addWidget(m_diagnosticSplitter, 1);

    connect(m_diagnosticList, &QListWidget::currentRowChanged, this, &MainWindow::updateDiagnosticDetails);
    connect(m_runDiagnosticButton, &QPushButton::clicked, this, &MainWindow::runSelectedDiagnostic);
    connect(m_runAllDiagnosticsButton, &QPushButton::clicked, this, &MainWindow::runAllDiagnostics);
    connect(m_copyDiagnosticButton, &QPushButton::clicked, this, &MainWindow::copyDiagnosticResults);
    connect(m_saveDiagnosticButton, &QPushButton::clicked, this, &MainWindow::saveDiagnosticResults);
    connect(m_editTargetConfigButton, &QPushButton::clicked, this, &MainWindow::editTargetConfig);
    // The derived scope starts on Repair Target with no committed target, so
    // the standard scope line starts from its no-target text and the target
    // configuration controls stay hidden until Systems commits a target.
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
    m_repairTargetLabel = new WrappedScopeLabel(QStringLiteral("Target: none selected"));
    heading->addWidget(m_repairTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addLayout(heading);

    layout->addWidget(subtleLabel(QStringLiteral(
        "Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.")));

    auto *planBox = new QGroupBox(QStringLiteral("Full Repair plan"));
    m_fullRepairPlanBox = planBox;
    // The plan frame follows its content so the individual-tools pane below it
    // starts directly beneath the last plan row instead of after a gap, while
    // the vertical splitter handle stays draggable to shrink the stage list.
    planBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
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
    // The plan header shares one row with the stage-count label. Both action
    // buttons stay shrinkable (with elided text and full-text tooltips) so the
    // row never overlaps at the minimum supported window width.
    auto *configurePlan = new ElidedPushButton;
    configurePlan->setIcon(themedIcon(QStringLiteral("settings-configure")));
    configurePlan->setText(QStringLiteral("Configure Plan…"));
    configurePlan->setToolTip(QStringLiteral("Open Settings to choose which Full Repair stages are part of the plan."));
    makeButtonShrinkable(configurePlan, 96);
    connect(configurePlan, &QPushButton::clicked, this, [this] {
        if (m_tabs) {
            m_tabs->setCurrentIndex(SettingsTab);
        }
    });
    planHeader->addWidget(configurePlan);

    m_runFullRepairButton = new ElidedPushButton;
    m_runFullRepairButton->setIcon(themedIcon(QStringLiteral("tools-wizard")));
    m_runFullRepairButton->setText(QStringLiteral("Run Full Repair"));
    makeButtonShrinkable(m_runFullRepairButton, 96);
    m_runFullRepairButton->setEnabled(false);
    m_runFullRepairButton->setToolTip(QStringLiteral("Select a repair drive, or choose Host Maintenance on the protected running-host card."));
    planHeader->addWidget(m_runFullRepairButton);
    // Keep the header and readiness message content-sized: the plan frame is
    // sized to these controls plus the plan rows, with no slack below them.
    planLayout->addLayout(planHeader, 0);
    planLayout->setAlignment(planHeader, Qt::AlignTop);
    planLayout->addWidget(m_fullRepairReadinessLabel, 0, Qt::AlignTop);

    m_fullRepairStageList = new QListWidget;
    m_fullRepairStageList->setSelectionMode(QAbstractItemView::NoSelection);
    m_fullRepairStageList->setFocusPolicy(Qt::NoFocus);
    m_fullRepairStageList->setAlternatingRowColors(true);
    m_fullRepairStageList->setWordWrap(true);
    m_fullRepairStageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // The Full Repair plan has at most ten configured rows. Height is
    // recomputed from the visible row count by updateFullRepairPlanHeight():
    // the list grows with its content up to a small row cap and scrolls past
    // it instead of reserving a fixed oversized frame.
    m_fullRepairStageList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // Expanding vertically lets the splitter grow the list when the user
    // drags the divider; the default and minimum heights come from
    // updateFullRepairPlanHeight().
    m_fullRepairStageList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    planLayout->addWidget(m_fullRepairStageList, 0, Qt::AlignTop);
    planLayout->setAlignment(Qt::AlignTop);

    // The handle between the Full Repair plan and the tools panes is
    // deliberately draggable: pull it down to shrink the selected-stage list
    // (down to two visible rows, then the list scrolls) or back up to the
    // content height.
    m_repairVerticalSplitter = new QSplitter(Qt::Vertical);
    m_repairVerticalSplitter->setObjectName(QStringLiteral("repairVerticalSplitter"));
    m_repairVerticalSplitter->setChildrenCollapsible(false);
    m_repairVerticalSplitter->setHandleWidth(6);

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
    // Header-click sorting is available, but the curated Full Repair workflow
    // order stays the default until the user picks a column (the hidden
    // indicator). Clicking a header sorts by that column and toggles
    // ascending/descending exactly like the other tables.
    m_repairToolTree->setSortingEnabled(true);
    m_repairToolTree->header()->setSortIndicator(-1, Qt::AscendingOrder);

    struct ToolSpec {
        const char *key;
        const char *title;
        const char *icon;
    };
    static const ToolSpec tools[] = {
        {"validate", "Validate environment", "task-complete"},
        {"filesystem", "File system repair", "filesystem"},
        {"dpkg", "Complete package configuration", "dialog-ok-apply"},
        {"fixbroken", "Repair broken dependencies", "dialog-ok-apply"},
        {"aptupdate", "Refresh package metadata", "view-refresh"},
        {"upgrade", "Upgrade installed packages", "system-software-update"},
        {"dkms", "DKMS", "applications-development"},
        {"display", "Graphical login / display manager", "video-display"},
        {"initramfs", "Initramfs", "initramfs"},
        {"efi", "EFI / UKI bootloader", "drive-removable-media"},
        {"grub", "GRUB configuration", "grub"},
        {"extlinux", "extlinux configuration", "grub"},
        {"bootstack", "Boot stack reconciliation", "system-run"}
    };

    for (const ToolSpec &spec : tools) {
        auto *item = new SortableTreeWidgetItem(m_repairToolTree);
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

    // The primary action is pinned to the panel header next to the tool title,
    // outside the panel body, so a short window or a long tool description can
    // never scroll the button out of view. The text below the header keeps the
    // remaining space and the panel stays a plain (non-scrolling) container.
    auto *detailHeader = new QHBoxLayout;
    detailHeader->setContentsMargins(0, 0, 0, 0);
    detailHeader->setSpacing(9);

    m_repairToolTitle = sectionTitle(QStringLiteral("Select a repair tool"));
    detailHeader->addWidget(m_repairToolTitle, 0, Qt::AlignVCenter);
    detailHeader->addStretch(1);

    m_repairToolButton = new ElidedPushButton(QStringLiteral("Run Tool"));
    m_repairToolButton->setEnabled(false);
    m_repairToolButton->setToolTip(QStringLiteral("Select a repair drive, or choose Host Maintenance on the protected running-host card."));
    detailHeader->addWidget(m_repairToolButton, 0, Qt::AlignVCenter);
    detailLayout->addLayout(detailHeader);

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

    // Flexible space collapses before any text is compressed.
    detailLayout->addStretch(1);

    m_repairSplitter->addWidget(toolBox);
    m_repairSplitter->addWidget(detailBox);
    m_repairSplitter->setHandleWidth(6);
    // The tools pane is the primary work area: give it roughly 62% of the
    // default width so complete tool names are visible, and let the selected
    // tool description use the remainder. QSplitter keeps this ratio while
    // the window is resized and both panes stay user-adjustable.
    m_repairSplitter->setStretchFactor(0, 3);
    m_repairSplitter->setStretchFactor(1, 2);
    m_repairSplitter->setSizes({650, 390});
    m_repairVerticalSplitter->addWidget(planBox);
    m_repairVerticalSplitter->addWidget(m_repairSplitter);
    m_repairVerticalSplitter->setStretchFactor(0, 0);
    m_repairVerticalSplitter->setStretchFactor(1, 1);
    // The stage list defaults to its minimum height (a few visible rows) so
    // the tools panes keep the slack; the handle is fully adjustable in both
    // directions and updateFullRepairPlanHeight() re-applies the default only
    // until the user moves the divider.
    connect(m_repairVerticalSplitter, &QSplitter::splitterMoved, this, [this] {
        m_repairPlanSplitterUserAdjusted = true;
    });
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
        QStringLiteral("Inspect Btrfs root snapshots and perform a transactional rollback. Rollback keeps the source snapshot unchanged, preserves the current @ root, promotes a writable copy to @, rebuilds the boot stack and automatically restores the old root if post-switch validation fails. In Host Maintenance the same workflow targets the running host through Snapper @ snapshots and Boot Bitch @rollback-before-* undo points; the running host starts the promoted root only after a reboot.")));
    heading->addStretch(1);
    m_snapshotTargetLabel = new WrappedScopeLabel(QStringLiteral("Target: none selected"));
    heading->addWidget(m_snapshotTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addLayout(heading);

    // Persistent reboot-required banner for a staged running-host rollback.
    // Visible only while Host Maintenance is active; the persisted flag keeps
    // the reminder across scope switches until the kernel boot id changes.
    m_hostRebootBanner = new QFrame;
    m_hostRebootBanner->setObjectName(QStringLiteral("hostRebootBanner"));
    m_hostRebootBanner->setFrameShape(QFrame::StyledPanel);
    m_hostRebootBanner->setFrameShadow(QFrame::Plain);
    auto *rebootBannerLayout = new QHBoxLayout(m_hostRebootBanner);
    rebootBannerLayout->setContentsMargins(10, 8, 10, 8);
    rebootBannerLayout->setSpacing(8);
    m_hostRebootBannerLabel = new QLabel;
    m_hostRebootBannerLabel->setWordWrap(true);
    m_hostRebootBannerLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_hostRebootBannerLabel->setAccessibleName(QStringLiteral("Reboot required"));
    rebootBannerLayout->addWidget(m_hostRebootBannerLabel, 1);
    m_hostRebootNowButton = new QPushButton(themedIcon(QStringLiteral("system-reboot")), QStringLiteral("Reboot Now"));
    m_hostRebootNowButton->setToolTip(QStringLiteral("Reboots the running host after a separate confirmation; all users are signed out and unsaved work is lost."));
    m_hostRebootLaterButton = new QPushButton(QStringLiteral("Later"));
    m_hostRebootLaterButton->setToolTip(QStringLiteral("Hides the reboot reminder until Host Maintenance is re-entered; the staged rollback stays in effect."));
    connect(m_hostRebootNowButton, &QPushButton::clicked, this, &MainWindow::confirmAndRebootHost);
    connect(m_hostRebootLaterButton, &QPushButton::clicked, this, [this] {
        m_hostRebootBannerDismissed = true;
        updateHostRebootBanner();
        statusBar()->showMessage(QStringLiteral("Reboot reminder hidden; the running-host rollback stays staged until the host is rebooted."), 5000);
    });
    rebootBannerLayout->addWidget(m_hostRebootNowButton, 0, Qt::AlignTop);
    rebootBannerLayout->addWidget(m_hostRebootLaterButton, 0, Qt::AlignTop);
    m_hostRebootBanner->setVisible(false);
    layout->addWidget(m_hostRebootBanner);

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
    // Header-click sorting: Snapshot and Created carry numeric/date sort keys,
    // and the default presents the newest recovery points first.
    m_snapshotTable->setSortingEnabled(true);
    m_snapshotTable->sortByColumn(1, Qt::DescendingOrder);
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
    // The complete section uses the same framed group-box container as the
    // Diagnostics, Repair and Settings tabs.
    auto *snapshotBox = new QGroupBox(QStringLiteral("Snapshot inventory"));
    auto *snapshotBoxLayout = new QVBoxLayout(snapshotBox);
    snapshotBoxLayout->setContentsMargins(8, 10, 8, 8);
    snapshotBoxLayout->setSpacing(8);

    // The inventory actions live in the panel header, above the splitter, so
    // they stay visible when the window is short and the page scrolls; they
    // are never at the bottom of the scrollable inventory.
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
        "Select a snapshot to enable the actions above, or double-click a row to inspect it read-only."));

    m_snapshotButtonLayout->addWidget(m_snapshotLoadButton);
    m_snapshotButtonLayout->addWidget(m_snapshotInspectButton);
    m_snapshotButtonLayout->addWidget(m_snapshotRollbackButton);
    m_snapshotButtonLayout->addStretch(1);
    snapshotBoxLayout->addLayout(m_snapshotButtonLayout);

    m_snapshotSplitter = new QSplitter(Qt::Vertical);
    m_snapshotSplitter->setObjectName(QStringLiteral("snapshotSplitter"));
    m_snapshotSplitter->setChildrenCollapsible(false);
    m_snapshotSplitter->addWidget(m_snapshotTable);
    m_snapshotSplitter->addWidget(m_snapshotDetails);
    const int rowHeight = qMax(24, fontMetrics().lineSpacing() + 10);
    m_snapshotSplitter->setHandleWidth(6);
    m_snapshotSplitter->setStretchFactor(0, 1);
    m_snapshotSplitter->setStretchFactor(1, 1);
    m_snapshotSplitter->setSizes({13 * rowHeight + 32, 220});
    snapshotBoxLayout->addWidget(m_snapshotSplitter, 1);

    layout->addWidget(snapshotBox, 1);

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
    m_chrootShellHeading = new QLabel(QStringLiteral("Chroot shell"));
    QFont titleFont = m_chrootShellHeading->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() * 1.25);
    m_chrootShellHeading->setFont(titleFont);
    heading->addWidget(m_chrootShellHeading);
    heading->addStretch();
    m_chrootShellTargetLabel = new WrappedScopeLabel(QStringLiteral("Target: none selected"));
    heading->addWidget(m_chrootShellTargetLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addLayout(heading);

    m_chrootShellNotice = new QLabel(QStringLiteral(
        "Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as dnf update -y or apt-get -y upgrade. Output is kept in this window and in the application log."));
    m_chrootShellNotice->setWordWrap(true);
    layout->addWidget(m_chrootShellNotice);

    auto *commandRow = new QHBoxLayout;
    m_chrootShellCommandEdit = new QLineEdit;
    m_chrootShellCommandEdit->setPlaceholderText(QStringLiteral("Command, for example: dnf update -y or update-grub"));
    m_chrootShellCommandEdit->setAccessibleName(QStringLiteral("Chroot shell command"));
    m_chrootShellCommandEdit->setClearButtonEnabled(true);
    commandRow->addWidget(m_chrootShellCommandEdit, 1);
    m_chrootShellRunButton = new ElidedPushButton(themedIcon(QStringLiteral("utilities-terminal")), QStringLiteral("Run Command"));
    m_chrootShellRunButton->setEnabled(false);
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
    m_chrootShellWarning = new QLabel(QStringLiteral("Commands can modify the target system. Review each command before running it."));
    m_chrootShellWarning->setWordWrap(true);
    footer->addWidget(m_chrootShellWarning, 1);
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

    auto *heading = new QGridLayout;
    heading->setContentsMargins(0, 0, 0, 0);
    m_fileCopyHeading = sectionTitle(QStringLiteral("File copy"));
    heading->addWidget(m_fileCopyHeading, 0, 0);
    QToolButton *fileCopyHelp = contextHelpButton(page, QStringLiteral("File Copy"),
        QStringLiteral("Copy and verify files in either direction. Host to Repair remounts only the selected target filesystem read-write after safety checks; Repair to Host keeps the repair target read-only. Browse Target Folders reads the selected repair tree through temporary read-only mounts, while transfers use rsync without --delete, path containment, ownership validation and post-copy verification."));
    heading->addWidget(fileCopyHelp, 0, 1);
    m_fileCopyTargetLabel = new WrappedScopeLabel;
    m_fileCopyTargetLabel->setText(QStringLiteral("Target: none selected"));
    heading->addWidget(m_fileCopyTargetLabel, 0, 2, Qt::AlignRight | Qt::AlignVCenter);

    // The primary File Copy actions share the page-title row with the scope
    // label: the wrapping scope sits to their left and the row ends at the
    // content edge, all vertically centered with the title. The buttons stay
    // shrinkable with elided text as a safety net; ResponsiveHeaderReflow
    // moves the scope label and both actions together to a right-aligned
    // second row as soon as the first row would crowd the title.
    m_fileCopyPreviewButton = new ElidedPushButton(themedIcon(QStringLiteral("document-preview")), QStringLiteral("Preview Changes"));
    m_fileCopyRunButton = new ElidedPushButton(themedIcon(QStringLiteral("edit-copy")), QStringLiteral("Copy and Verify"));
    m_fileCopyPreviewButton->setEnabled(false);
    m_fileCopyRunButton->setEnabled(false);
    m_fileCopyPreviewButton->setToolTip(QStringLiteral("Run an rsync dry-run through the guarded helper. No files are changed."));
    m_fileCopyRunButton->setToolTip(QStringLiteral("Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted."));
    connect(m_fileCopyPreviewButton, &QPushButton::clicked, this, &MainWindow::runFileCopyPreview);
    connect(m_fileCopyRunButton, &QPushButton::clicked, this, &MainWindow::runFileCopy);
    makeButtonShrinkable(m_fileCopyPreviewButton);
    makeButtonShrinkable(m_fileCopyRunButton);
    heading->addWidget(m_fileCopyPreviewButton, 0, 3, Qt::AlignVCenter);
    heading->addWidget(m_fileCopyRunButton, 0, 4, Qt::AlignVCenter);
    heading->setColumnStretch(2, 1);
    layout->addLayout(heading);
    new ResponsiveHeaderReflow(content, layout, heading, m_fileCopyHeading, fileCopyHelp,
                               m_fileCopyTargetLabel, {m_fileCopyPreviewButton, m_fileCopyRunButton});

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
    m_fileCopyAddFilesButton = new ElidedPushButton(themedIcon(QStringLiteral("document-open")), QStringLiteral("Add Files…"));
    m_fileCopyAddFolderButton = new ElidedPushButton(themedIcon(QStringLiteral("folder-open")), QStringLiteral("Add Folder…"));
    auto *remove = new QPushButton(themedIcon(QStringLiteral("list-remove")), QStringLiteral("Remove"));
    remove->setToolTip(QStringLiteral("Remove the selected staged source entries from this list."));
    auto *clear = new QPushButton(QStringLiteral("Clear"));
    clear->setToolTip(QStringLiteral("Clear every staged source entry from this list."));
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
    m_fileCopyBrowseDestinationButton = new ElidedPushButton(themedIcon(QStringLiteral("folder-open")), QStringLiteral("Choose Path…"));
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
        appendLog(QStringLiteral("File Copy direction changed to %1; staged paths were cleared to avoid mixing source/destination namespaces.").arg(direction),
                  QStringLiteral("INFO"), LogEntryKind::FileCopy);
    });
    updateFileCopyDirection();

    // The heading (with the primary actions) and the staged
    // source/destination controls share the page scroll area.
    scroll->setWidget(content);
    outerLayout->addWidget(scroll, 1);

    return page;
}

namespace {

// True when the currently applied application palette is a dark one. The
// custom colors below follow the palette (which applyColorScheme() keeps in
// step with the desktop color scheme), never a hardcoded light-only value.
bool applicationUsesDarkPalette()
{
    return QApplication::palette().color(QPalette::Window).lightness() < 128;
}

// The shared repair-result glyph colors, adapted to the effective color
// scheme so the symbols stay readable on both light and dark backgrounds. The
// same table drives the explicit post-write coloring and the document
// highlighter so the three classes cannot drift apart.
const QList<QPair<QString, QTextCharFormat>> &repairResultSymbolFormats()
{
    static const QList<QPair<QString, QTextCharFormat>> lightFormats = [] {
        QTextCharFormat successFormat;
        successFormat.setForeground(QColor(0x2e, 0xa0, 0x43));
        QTextCharFormat failedFormat;
        failedFormat.setForeground(QColor(0xc0, 0x39, 0x2b));
        QTextCharFormat noRepairFormat;
        noRepairFormat.setForeground(QColor(0x1f, 0x6f, 0xd6));
        return QList<QPair<QString, QTextCharFormat>>{
            {QStringLiteral("✓"), successFormat},
            {QStringLiteral("✗"), failedFormat},
            {QStringLiteral("▪"), noRepairFormat}
        };
    }();
    static const QList<QPair<QString, QTextCharFormat>> darkFormats = [] {
        QTextCharFormat successFormat;
        successFormat.setForeground(QColor(0x56, 0xd3, 0x64));
        QTextCharFormat failedFormat;
        failedFormat.setForeground(QColor(0xff, 0x7b, 0x72));
        QTextCharFormat noRepairFormat;
        noRepairFormat.setForeground(QColor(0x79, 0xc0, 0xff));
        return QList<QPair<QString, QTextCharFormat>>{
            {QStringLiteral("✓"), successFormat},
            {QStringLiteral("✗"), failedFormat},
            {QStringLiteral("▪"), noRepairFormat}
        };
    }();
    return applicationUsesDarkPalette() ? darkFormats : lightFormats;
}

// The lines that carry a repair-result glyph: the aggregate "Full Repair
// results:" line, the single-action "Repair result:" line, and the two-space
// indented per-stage lines of the aggregate.
bool isRepairResultLine(const QString &line)
{
    return line.contains(QStringLiteral("Repair result:"))
        || line.contains(QStringLiteral("Full Repair results:"))
        || line.startsWith(QStringLiteral("  ✓"))
        || line.startsWith(QStringLiteral("  ✗"))
        || line.startsWith(QStringLiteral("  ▪"));
}

// Document-level coloring for the repair-result glyphs. The highlighter is
// attached to the log document, so every block is colored whenever it is
// inserted or changed, no matter which refresh path (full rebuild, incremental
// prepend, filter rebuild, section replacement or an adopted session log)
// produced it. The document itself stays plain text: only the glyph ranges get
// a foreground, so search, copy, Save As and the session file are unaffected.
class RepairResultHighlighter final : public QSyntaxHighlighter
{
public:
    explicit RepairResultHighlighter(QTextDocument *document)
        : QSyntaxHighlighter(document)
    {
    }

protected:
    void highlightBlock(const QString &text) override
    {
        if (!isRepairResultLine(text)) {
            return;
        }
        for (const auto &symbol : repairResultSymbolFormats()) {
            int index = text.indexOf(symbol.first);
            while (index >= 0) {
                setFormat(index, static_cast<int>(symbol.first.size()), symbol.second);
                index = text.indexOf(symbol.first, index + symbol.first.size());
            }
        }
    }
};

} // namespace

QWidget *MainWindow::buildLogsPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(8, 12, 8, 8);
    layout->setSpacing(8);

    auto *top = new QHBoxLayout;
    top->addWidget(sectionTitle(QStringLiteral("Application log")));
    top->addWidget(contextHelpButton(page, QStringLiteral("Logs"),
        QStringLiteral("Shows this application's session activity. Session files are listed on the left; Save a copy when you need to share troubleshooting details.")));
    top->addStretch(1);
    auto *save = new QPushButton(themedIcon(QStringLiteral("document-save")), QStringLiteral("Save As…"));
    auto *clear = new QPushButton(QStringLiteral("Clear Register"));
    connect(save, &QPushButton::clicked, this, &MainWindow::saveLogAs);
    connect(clear, &QPushButton::clicked, this, [this] {
        m_actionLogEntries.clear();
        m_diagnosticLogEntries.clear();
        m_repairLogEntries.clear();
        m_statusLogEntries.clear();
        m_statusEntryIdentities.clear();
        m_activeRepairSection.clear();
        m_activeRepairEntries.clear();
        m_pendingLogEntries.clear();
        m_pendingEntryRemovals.clear();
        m_renderedEntryRanges.clear();
        m_logViewRendered = false;
        m_logView->clear();
        appendLog(QStringLiteral("Action register cleared. No persistent target logs were modified."));
        // Clearing is an explicit user action: show the result immediately
        // instead of waiting for the coalescing timer.
        refreshLogView();
    });
    top->addWidget(save);
    top->addWidget(clear);
    layout->addLayout(top);

    m_sessionLogSplitter = new QSplitter(Qt::Horizontal);
    m_sessionLogSplitter->setObjectName(QStringLiteral("sessionLogSplitter"));
    m_sessionLogSplitter->setChildrenCollapsible(false);

    // The live session is always the first list entry so returning to the
    // current log never requires reopening the newest file from disk.
    m_sessionLogPanel = new QWidget;
    auto *sessionLayout = new QVBoxLayout(m_sessionLogPanel);
    sessionLayout->setContentsMargins(0, 0, 0, 0);
    sessionLayout->setSpacing(6);
    sessionLayout->addWidget(sectionTitle(QStringLiteral("Session logs")));
    m_sessionLogList = new QListWidget;
    m_sessionLogList->setObjectName(QStringLiteral("sessionLogList"));
    // The selection frame starts compact and can shrink further; it must not
    // squeeze the log view into a sliver on a narrow window.
    m_sessionLogList->setMinimumWidth(120);
    m_sessionLogList->setMinimumHeight(70);
    sessionLayout->addWidget(m_sessionLogList, 1);
    // Two compact rows keep the session controls inside the narrow selection
    // frame. A single five-button row cannot fit once the frame is small.
    auto *sessionButtons = new QGridLayout;
    sessionButtons->setContentsMargins(0, 0, 0, 0);
    sessionButtons->setHorizontalSpacing(6);
    sessionButtons->setVerticalSpacing(6);
    m_newSessionLogButton = new QPushButton(QStringLiteral("New Session Log"));
    m_clearSessionLogButton = new QPushButton(QStringLiteral("Clear"));
    m_clearSessionLogButton->setToolTip(QStringLiteral(
        "Truncate the current session log and start it over with a cleared-by-user entry. Prior session files are never modified."));
    m_deleteSessionLogButton = new QPushButton(QStringLiteral("Delete"));
    m_deleteSessionLogButton->setEnabled(false);
    m_refreshSessionLogsButton = new QPushButton(QStringLiteral("Refresh"));
    m_addNoteButton = new QPushButton(QStringLiteral("Add Note"));
    m_addNoteButton->setToolTip(QStringLiteral(
        "Append a timestamped NOTE entry to the current session log."));
    sessionButtons->addWidget(m_newSessionLogButton, 0, 0);
    sessionButtons->addWidget(m_addNoteButton, 0, 1);
    sessionButtons->addWidget(m_clearSessionLogButton, 1, 0);
    sessionButtons->addWidget(m_deleteSessionLogButton, 1, 1);
    sessionButtons->addWidget(m_refreshSessionLogsButton, 1, 2);
    sessionButtons->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 3, 2, 1);
    sessionLayout->addLayout(sessionButtons);
    m_sessionLogSplitter->addWidget(m_sessionLogPanel);

    m_sessionLogViewer = new QWidget;
    auto *viewerLayout = new QVBoxLayout(m_sessionLogViewer);
    viewerLayout->setContentsMargins(0, 0, 0, 0);
    viewerLayout->setSpacing(6);

    auto *filterRow = new QHBoxLayout;
    auto *filterLabel = new QLabel(QStringLiteral("Search log:"));
    m_logSearchEdit = new QLineEdit;
    m_logSearchEdit->setObjectName(QStringLiteral("logSearchEdit"));
    m_logSearchEdit->setClearButtonEnabled(true);
    m_logSearchEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_logSearchEdit->setMinimumHeight(36);
    m_logSearchEdit->setPlaceholderText(QStringLiteral(
        "Filter application log (fuzzy match; use @section, e.g. @errors, for whole diagnostic sections)…"));
    m_logSearchEdit->setToolTip(QStringLiteral(
        "Type any characters to show matching application-log entries. Matching is case-insensitive and fuzzy. "
        "A term beginning with @ selects a complete diagnostic section by key or title (for example @errors, @boot or @all) "
        "together with the repair entries related to that section; other terms keep the fuzzy line match and combine as AND."));
    filterRow->addWidget(filterLabel);
    filterRow->addWidget(m_logSearchEdit, 1);

    m_logFilterCombo = new QComboBox;
    m_logFilterCombo->setObjectName(QStringLiteral("logFilterCombo"));
    m_logFilterCombo->addItem(QStringLiteral("All entries"), QStringLiteral("all"));
    m_logFilterCombo->addItem(QStringLiteral("Diagnostics"), QStringLiteral("diagnostics"));
    m_logFilterCombo->addItem(QStringLiteral("Repairs"), QStringLiteral("repairs"));
    for (const LogWorkflowFilterSpec &spec : logWorkflowFilterSpecs) {
        m_logFilterCombo->addItem(QString::fromLatin1(spec.title),
                                  QString::fromLatin1(spec.key));
    }
    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        m_logFilterCombo->addItem(QString::fromLatin1(spec.title),
                                  QStringLiteral("@%1").arg(QString::fromLatin1(spec.key)));
    }
    m_logFilterCombo->setToolTip(QStringLiteral(
        "Filter the visible log by entry kind. Selecting a diagnostic section shows that complete section "
        "plus the repair entries recorded for its related repair tools; a workflow filter such as "
        "File system repair additionally shows the whole storage evidence sections it works on."));
    filterRow->addWidget(m_logFilterCombo);
    viewerLayout->addLayout(filterRow);

    m_priorLogBanner = new QLabel;
    m_priorLogBanner->setObjectName(QStringLiteral("priorLogBanner"));
    m_priorLogBanner->setWordWrap(true);
    m_priorLogBanner->setFrameShape(QFrame::StyledPanel);
    m_priorLogBanner->setContentsMargins(8, 5, 8, 5);
    m_priorLogBanner->setVisible(false);
    viewerLayout->addWidget(m_priorLogBanner);

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
    // The highlighter is parented to the log document, so the glyph colors are
    // re-applied automatically on every document change instead of depending on
    // a particular refresh path having called the explicit coloring helper.
    new RepairResultHighlighter(m_logView->document());
    viewerLayout->addWidget(m_logView, 1);

    m_sessionLogSplitter->addWidget(m_sessionLogViewer);
    m_sessionLogSplitter->setHandleWidth(6);
    m_sessionLogSplitter->setStretchFactor(0, 0);
    m_sessionLogSplitter->setStretchFactor(1, 1);
    m_sessionLogSplitter->setSizes({280, 800});
    layout->addWidget(m_sessionLogSplitter, 1);

    connect(m_logSearchEdit, &QLineEdit::textChanged, this, [this] { refreshLogView(); });
    connect(m_logFilterCombo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this] { refreshLogView(); });
    connect(m_sessionLogList, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *current, QListWidgetItem *) {
                displaySessionLog(current ? current->data(Qt::UserRole).toString() : QString());
            });
    connect(m_newSessionLogButton, &QPushButton::clicked, this, &MainWindow::startNewSessionLog);
    connect(m_clearSessionLogButton, &QPushButton::clicked, this, &MainWindow::clearCurrentSessionLog);
    connect(m_addNoteButton, &QPushButton::clicked, this, &MainWindow::addSessionNote);
    connect(m_deleteSessionLogButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedSessionLog);
    connect(m_refreshSessionLogsButton, &QPushButton::clicked, this, [this] { refreshSessionLogList(); });

    refreshLogView();
    refreshSessionLogList();

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
    m_fullRepairFilesystem = new QCheckBox(QStringLiteral("Repair file system errors (read-only check first)"));
    m_fullRepairFilesystem->setToolTip(QStringLiteral(
        "Runs a read-only file system check for the root, /boot, ESP and /home filesystems, "
        "then offers an explicit per-device repair for each filesystem that reports errors. "
        "Offline repair tools refuse mounted filesystems; btrfs scrub and zpool scrub are online modes."));
    m_fullRepairDpkg = new QCheckBox(QStringLiteral("Complete interrupted package configuration"));
    m_fullRepairBrokenPackages = new QCheckBox(QStringLiteral("Repair broken package dependencies"));
    m_fullRepairAptUpdate = new QCheckBox(QStringLiteral("Refresh package metadata"));
    m_fullRepairUpgrade = new QCheckBox(QStringLiteral("Upgrade installed packages (adaptive APT simulation)"));
    m_fullRepairDkms = new QCheckBox(QStringLiteral("Rebuild DKMS modules"));
    m_fullRepairDisplayManager = new QCheckBox(QStringLiteral("Restore detected graphical login manager and graphical.target"));
    m_fullRepairInitramfs = new QCheckBox(QStringLiteral("Rebuild initramfs after mapper/crypttab validation"));
    m_fullRepairEfi = new QCheckBox(QStringLiteral("Repair EFI / UKI boot path (explicit target ESP repair)"));
    m_fullRepairGrub = new QCheckBox(QStringLiteral("Update GRUB configuration"));
    m_fullRepairExtlinux = new QCheckBox(QStringLiteral("Update extlinux configuration"));
    repairLayout->addWidget(m_fullRepairFilesystem);
    repairLayout->addWidget(m_fullRepairDpkg);
    repairLayout->addWidget(m_fullRepairBrokenPackages);
    repairLayout->addWidget(m_fullRepairAptUpdate);
    repairLayout->addWidget(m_fullRepairUpgrade);
    repairLayout->addWidget(m_fullRepairDkms);
    repairLayout->addWidget(m_fullRepairDisplayManager);
    repairLayout->addWidget(m_fullRepairInitramfs);
    repairLayout->addWidget(m_fullRepairEfi);
    repairLayout->addWidget(m_fullRepairGrub);
    repairLayout->addWidget(m_fullRepairExtlinux);
    layout->addWidget(repairBox);

    auto *diagnosticsBox = new QGroupBox(QStringLiteral("Diagnostics"));
    auto *diagnosticsLayout = new QVBoxLayout(diagnosticsBox);
    m_autoRefreshDiagnostics = new QCheckBox(QStringLiteral("Automatically regenerate read-only diagnostics after repairs or target changes"));
    m_autoRefreshDiagnostics->setToolTip(QStringLiteral(
        "Regenerates the cached read-only diagnostics for the current Diagnostics scope after a repair, target change, or other evidence invalidation. "
        "The refresh runs only inside an already authorized administrator session; it never triggers a new Polkit prompt."));
    diagnosticsLayout->addWidget(m_autoRefreshDiagnostics);
    layout->addWidget(diagnosticsBox);

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
    // Header-click sorting, defaulting to Feature ascending. The population
    // pass below disables sorting while it inserts rows and re-enables it
    // afterwards; QTableWidgetItem insertion must never happen while a sort is
    // active.
    m_capabilityTable->setSortingEnabled(true);
    m_capabilityTable->sortByColumn(0, Qt::AscendingOrder);
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
        m_fullRepairFilesystem,
        m_fullRepairDpkg, m_fullRepairBrokenPackages, m_fullRepairAptUpdate,
        m_fullRepairUpgrade, m_fullRepairDkms, m_fullRepairDisplayManager, m_fullRepairInitramfs, m_fullRepairEfi, m_fullRepairGrub,
        m_fullRepairExtlinux
    };
    for (QCheckBox *check : repairToggles) {
        connect(check, &QCheckBox::toggled, this, &MainWindow::updateFullRepairSummary);
    }

    connect(m_autoRefreshDiagnostics, &QCheckBox::toggled, this, [this](bool enabled) {
        if (!enabled) {
            m_evidenceRefreshPending = false;
            if (m_evidenceRefreshTimer) {
                m_evidenceRefreshTimer->stop();
            }
            return;
        }
        scheduleEvidenceRefresh(QStringLiteral("automatic diagnostics regeneration enabled"));
    });

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
    m_autoRefreshDiagnostics->setChecked(m_settings->value(QStringLiteral("diagnostics/autoRefreshStale"), true).toBool());

    m_fullRepairFilesystem->setChecked(m_settings->value(QStringLiteral("repair/filesystem"), true).toBool());
    m_fullRepairDpkg->setChecked(m_settings->value(QStringLiteral("repair/dpkgConfigure"), true).toBool());
    m_fullRepairBrokenPackages->setChecked(m_settings->value(QStringLiteral("repair/fixBroken"), true).toBool());
    m_fullRepairAptUpdate->setChecked(m_settings->value(QStringLiteral("repair/refreshMetadata"), true).toBool());
    m_fullRepairUpgrade->setChecked(m_settings->value(QStringLiteral("repair/upgradePackages"), false).toBool());
    m_fullRepairDkms->setChecked(m_settings->value(QStringLiteral("repair/dkms"), true).toBool());
    m_fullRepairDisplayManager->setChecked(m_settings->value(QStringLiteral("repair/displayManager"), false).toBool());
    m_fullRepairInitramfs->setChecked(m_settings->value(QStringLiteral("repair/initramfs"), true).toBool());
    m_fullRepairEfi->setChecked(m_settings->value(QStringLiteral("repair/efiBootloader"), false).toBool());
    m_fullRepairGrub->setChecked(m_settings->value(QStringLiteral("repair/grub"), true).toBool());
    m_fullRepairExtlinux->setChecked(m_settings->value(QStringLiteral("repair/extlinux"), true).toBool());

    const bool wrapLogs = m_settings->value(QStringLiteral("logs/wrapLines"), true).toBool();
    if (m_wrapLogsAction) {
        m_wrapLogsAction->setChecked(wrapLogs);
    }
    setLogWrapEnabled(wrapLogs);

    const QByteArray headerState = m_settings->value(QStringLiteral("systems/headerStateV3")).toByteArray();
    if (!headerState.isEmpty() && m_deviceTree) {
        m_deviceTree->header()->restoreState(headerState);
    }
    // The saved header state also carries a sort indicator and header
    // clickability from older versions that had no header sorting. Column
    // widths persist, but header clicks and the visible indicator are
    // re-enabled and the device sort always starts at the documented default
    // (repair likelihood ascending) instead of an arbitrary restored column.
    if (m_deviceTree) {
        m_deviceTree->header()->setSectionsClickable(true);
        m_deviceTree->header()->setSortIndicatorShown(true);
        m_deviceTree->sortByColumn(1, Qt::AscendingOrder);
        // A restored header state can carry arbitrary section widths from an
        // older layout; re-apply the dynamic policy once the window is shown
        // and the viewport width is final.
        QTimer::singleShot(0, this, [this] {
            applyDeviceColumnLayout();
            updateDeviceTreeHeight();
        });
    }

    const QByteArray splitterState = m_settings->value(QStringLiteral("systems/splitterStateV3")).toByteArray();
    if (!splitterState.isEmpty() && m_systemSplitter) {
        m_systemSplitter->restoreState(splitterState);
    }

    const QByteArray repairSplitterState = m_settings->value(QStringLiteral("repair/splitterStateV3")).toByteArray();
    if (!repairSplitterState.isEmpty() && m_repairSplitter) {
        m_repairSplitter->restoreState(repairSplitterState);
    }
    const QByteArray repairVerticalSplitterState = m_settings->value(QStringLiteral("repair/verticalSplitterStateV2")).toByteArray();
    if (!repairVerticalSplitterState.isEmpty() && m_repairVerticalSplitter) {
        m_repairVerticalSplitter->restoreState(repairVerticalSplitterState);
        // A restored position is the user's own choice: keep it instead of
        // re-applying the minimum default when the plan changes.
        m_repairPlanSplitterUserAdjusted = true;
    }

    const QByteArray diagnosticSplitterState = m_settings->value(QStringLiteral("diagnostics/splitterStateV1")).toByteArray();
    if (!diagnosticSplitterState.isEmpty() && m_diagnosticSplitter) {
        m_diagnosticSplitter->restoreState(diagnosticSplitterState);
    }
    const QByteArray snapshotSplitterState = m_settings->value(QStringLiteral("snapshots/splitterStateV1")).toByteArray();
    if (!snapshotSplitterState.isEmpty() && m_snapshotSplitter) {
        m_snapshotSplitter->restoreState(snapshotSplitterState);
    }

    // Persisted running-host rollback reminder. The kernel boot id decides
    // whether the staged rollback already took effect: a different boot id
    // clears the flag, an equal one keeps it.
    m_hostRebootRequired = m_settings->value(QStringLiteral("host/rebootRequired"), false).toBool();
    m_hostRebootSnapshotId = m_settings->value(QStringLiteral("host/rebootRequiredSnapshot")).toString();
    m_hostRebootRequiredAt = m_settings->value(QStringLiteral("host/rebootRequiredAt")).toString();
    m_hostRebootBootId = m_settings->value(QStringLiteral("host/rebootRequiredBootId")).toString();
    reconcileHostRebootRequired();
    updateHostRebootBanner();

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
    m_settings->setValue(QStringLiteral("diagnostics/autoRefreshStale"), m_autoRefreshDiagnostics->isChecked());
    m_settings->setValue(QStringLiteral("repair/filesystem"), m_fullRepairFilesystem->isChecked());
    m_settings->setValue(QStringLiteral("repair/dpkgConfigure"), m_fullRepairDpkg->isChecked());
    m_settings->setValue(QStringLiteral("repair/fixBroken"), m_fullRepairBrokenPackages->isChecked());
    m_settings->setValue(QStringLiteral("repair/refreshMetadata"), m_fullRepairAptUpdate->isChecked());
    m_settings->setValue(QStringLiteral("repair/upgradePackages"), m_fullRepairUpgrade->isChecked());
    m_settings->setValue(QStringLiteral("repair/dkms"), m_fullRepairDkms->isChecked());
    m_settings->setValue(QStringLiteral("repair/displayManager"), m_fullRepairDisplayManager->isChecked());
    m_settings->setValue(QStringLiteral("repair/initramfs"), m_fullRepairInitramfs->isChecked());
    m_settings->setValue(QStringLiteral("repair/efiBootloader"), m_fullRepairEfi->isChecked());
    m_settings->setValue(QStringLiteral("repair/grub"), m_fullRepairGrub->isChecked());
    m_settings->setValue(QStringLiteral("repair/extlinux"), m_fullRepairExtlinux->isChecked());
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
        m_settings->setValue(QStringLiteral("repair/verticalSplitterStateV2"), m_repairVerticalSplitter->saveState());
    }
    if (m_diagnosticSplitter) {
        m_settings->setValue(QStringLiteral("diagnostics/splitterStateV1"), m_diagnosticSplitter->saveState());
    }
    if (m_snapshotSplitter) {
        m_settings->setValue(QStringLiteral("snapshots/splitterStateV1"), m_snapshotSplitter->saveState());
    }

    m_settings->setValue(QStringLiteral("host/rebootRequired"), m_hostRebootRequired);
    m_settings->setValue(QStringLiteral("host/rebootRequiredSnapshot"), m_hostRebootSnapshotId);
    m_settings->setValue(QStringLiteral("host/rebootRequiredAt"), m_hostRebootRequiredAt);
    m_settings->setValue(QStringLiteral("host/rebootRequiredBootId"), m_hostRebootBootId);

    m_settings->sync();
}

// ---- MainWindow: device discovery and target selection ----------------------

// Scans block devices through SystemScanner and repopulates the Systems page.
// The scan itself is read-only; its recurring summary lines are recorded as
// tracked status entries so a refresh replaces them instead of stacking copies.
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
    // The scan summary lines are status, not history: every refresh replaces
    // the previous scan summary instead of stacking one per repair cycle.
    for (const QString &line : diagnostics) {
        if (line == QStringLiteral("lsblk scan completed successfully.")) {
            appendStatusLog(statusEntryIdentity(QStringLiteral("scan-lsblk")), line);
        } else if (line.startsWith(QStringLiteral("Discovered "))
                   && line.endsWith(QStringLiteral(" top-level physical disk(s)."))) {
            appendStatusLog(statusEntryIdentity(QStringLiteral("scan-discovered")), line);
        } else {
            appendLog(line);
        }
    }
    QString hostIdentity = m_hostPrimaryPath.isEmpty() ? QStringLiteral("unresolved") : m_hostPrimaryPath;
    if (!m_hostPrimaryComponentPath.isEmpty()) {
        hostIdentity += QStringLiteral(" (%1)").arg(m_hostPrimaryComponentPath);
    }
    QString helperResolution;
    const QString helperPath = repairHelperPath(&helperResolution);
    appendStatusLog(statusEntryIdentity(QStringLiteral("launch-detection")),
                    QStringLiteral("Launch detection: %1 device(s) scanned; protected running host %2; repair target %3; privileged helper %4 (%5); pkexec %6.")
        .arg(devices.size())
        .arg(hostIdentity)
        .arg(m_previewTargetPath.isEmpty() ? QStringLiteral("not selected") : m_previewTargetPath)
        .arg(helperPath.isEmpty() ? QStringLiteral("not found") : helperPath,
             helperResolution.isEmpty() ? QStringLiteral("no resolution detail") : helperResolution)
        .arg(QStandardPaths::findExecutable(QStringLiteral("pkexec")).isEmpty() ? QStringLiteral("unavailable") : QStringLiteral("available")));
    statusBar()->showMessage(QStringLiteral("Device scan complete — discovery did not modify storage"), 4000);
    // A refresh can resolve or change the protected host identity that backs
    // an active maintenance scope; keep the session marker in step.
    updateSessionScope();
}

// Applies the Settings filters, ranks candidates by repair likelihood and
// renders the device tree. The protected running-host disk is never a
// candidate; the committed target highlight and control states are reapplied.
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

    // The candidate ranking is applied by the tree's default Status sort
    // (repair-likelihood rank), so the candidates are inserted in scan order
    // and the active header sort orders both the drives and their partitions.
    for (const DeviceNode &device : candidates) {
        addDeviceItem(nullptr, device);
    }
    applyActiveSort(m_deviceTree, 1, Qt::AscendingOrder);
    // Size the shrink-only columns to the freshly rendered content and give
    // the Status column the remainder before the pane height is computed.
    applyDeviceColumnLayout();

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
    // Make Default follows the same single truth log as the repair tools:
    // host identity plus the cached running-host EFI/UKI capability evidence.
    updateHostDefaultButtonState();
}

void MainWindow::addDeviceItem(QTreeWidgetItem *parent, const DeviceNode &node)
{
    const QString nameText = friendlyNodeName(node);
    const QString statusText = parent ? (node.status.isEmpty() ? QStringLiteral("—") : node.status)
                                      : friendlyTopLevelStatus(node);
    const QString connectionText = node.transport.isEmpty() ? QStringLiteral("—") : node.transport.toUpper();
    const QString sizeText = SystemScanner::humanSize(node.sizeBytes);
    // A physical disk has no filesystem of its own. Show the filesystem of the
    // component the details panel resolves as the repair target so the
    // candidate row never stays blank while its details name ext4/…, which is
    // exactly the state of an Alpine system after the unprivileged udev
    // fallback. A node with its own filesystem is unchanged.
    const DeviceNode *preferred = node.fileSystem.isEmpty() ? preferredRepairNode(node) : nullptr;
    const bool derivedFileSystem = preferred && preferred != &node && !preferred->fileSystem.isEmpty();
    const QString fileSystemText = !node.fileSystem.isEmpty()
        ? node.fileSystem
        : (derivedFileSystem ? preferred->fileSystem : QStringLiteral("—"));
    const QString deviceText = node.path.isEmpty() ? node.name : node.path;

    auto *item = parent ? new SortableTreeWidgetItem(parent) : new SortableTreeWidgetItem(m_deviceTree);
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
    // Numeric-aware sort keys: Status orders by the repair-likelihood rank
    // (the historical "most likely first" ordering) and Size by the real byte
    // count instead of the formatted "1.0 GiB" text.
    item->setData(1, BootRepairSortKeyRole, repairLikelihoodScore(node));
    item->setData(3, BootRepairSortKeyRole, QVariant::fromValue<qulonglong>(node.sizeBytes));

    const QStringList fullValues = {
        nameText, statusText, connectionText, sizeText, fileSystemText, deviceText
    };
    for (int column = 0; column < fullValues.size(); ++column) {
        item->setToolTip(column, fullValues.at(column));
    }

    // A derived filesystem cell names the component it came from and keeps
    // its label/UUID reachable even though the tree has no such columns.
    if (derivedFileSystem) {
        QString detail = QStringLiteral("%1 on %2").arg(fileSystemText, preferred->path);
        if (!preferred->label.isEmpty()) {
            detail += QStringLiteral("  •  label %1").arg(preferred->label);
        }
        if (!preferred->uuid.isEmpty()) {
            detail += QStringLiteral("  •  UUID %1").arg(preferred->uuid);
        }
        item->setToolTip(4, detail);
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

// Single exit path for explicit running-host maintenance. Every gesture that
// leaves the host scope (the Host Maintenance button and a repair-target
// selection) goes through here so the button text/tooltip, the scope labels,
// the session scope and the authorization affordance can never drift apart.
// Returns true when maintenance was active and has now been deselected.
bool MainWindow::exitHostMaintenanceMode()
{
    if (!m_hostMaintenanceMode) {
        return false;
    }

    m_hostMaintenanceMode = false;
    if (m_hostMaintenanceButton) {
        m_hostMaintenanceButton->setText(QStringLiteral("Host Maintenance"));
        m_hostMaintenanceButton->setToolTip(QStringLiteral(
            "Host Maintenance — select the running host for deliberate guarded maintenance. All supported repair stages run against the active system; running-host Snapper @ snapshots are available in the Snapshots tab, while the chroot shell and file-copy workflows remain separate target tools."));
    }
    // Leaving host maintenance also leaves the running-host diagnostics scope;
    // cached host evidence stays cached but is no longer displayed or offered.
    // updateTargetLabels() refreshes the standard Diagnostics scope line.
    appendLog(QStringLiteral("Running-host maintenance deselected; ordinary repair-target mode restored. Diagnostics scope returned to Repair Target."));
    updateTargetLabels();
    updateSessionScope();
    updateAuthorizationAffordance();
    // The reboot-required banner is host-scope only; the persisted flag stays
    // and re-entering Host Maintenance shows the banner again.
    updateHostRebootBanner();
    // Leaving host scope restores the ordinary target snapshot preload; the
    // scope identity change must be observed first so the generation guard
    // cannot mistake the target preload for the completed host one.
    updateSnapshotControls();
    scheduleSnapshotPreload();
    return true;
}

void MainWindow::selectHostForMaintenance()
{
    if (m_hostMaintenanceMode) {
        exitHostMaintenanceMode();
        // Put the committed repair target back in the details panel, or clear
        // it when no target is committed.
        if (!m_previewTargetPath.isEmpty() && m_deviceIndex.contains(m_previewTargetPath)) {
            showDeviceDetails(m_deviceIndex.value(m_previewTargetPath), true);
        } else if (m_deviceTree) {
            m_deviceTree->clearSelection();
        }
        return;
    }

    QString reason;
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
    m_hostRebootBannerDismissed = false;
    if (m_hostMaintenanceButton) {
        m_hostMaintenanceButton->setText(QStringLiteral("Exit Host Maintenance"));
        m_hostMaintenanceButton->setToolTip(QStringLiteral(
            "Exit Host Maintenance — leave running-host maintenance and return to ordinary repair-target mode."));
    }
    // Entering maintenance is the only way to enable the Running Host
    // diagnostics scope; the standard scope line follows through
    // updateTargetLabels() below.
    appendLog(QStringLiteral(
        "Running host selected for explicit maintenance: %1 (%2). All supported repair stages are available in this deliberate host scope; running-host Snapper @ snapshots are available in the Snapshots tab, while the chroot shell and file-copy workflows remain separate target tools.")
                  .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath));
    updateTargetLabels();
    updateSessionScope();
    // Show a previously staged rollback reminder again and load the
    // running-host snapshot inventory. The scope identity change must be
    // observed before scheduling so the generation guard serves host scope.
    updateHostRebootBanner();
    updateSnapshotControls();
    scheduleSnapshotPreload();
    // Host Maintenance commits the running host as the active scope, so show
    // the protected host drive in the selected-drive details panel right away
    // instead of leaving the previous selection or placeholders.
    showHostDetails();
    // Schedule the automatic read-only host diagnostics as soon as the host
    // scope is committed. The run itself waits for administrator
    // authorization and then executes asynchronously (no modal dialog); a
    // privileged request started while it is running is refused with a clear
    // message instead of queueing behind it.
    scheduleEvidenceRefresh(QStringLiteral("running-host maintenance selected"));
    // Ask for the privileged session as soon as the host scope is committed so
    // diagnostics and repairs reuse one Polkit prompt. The request is deferred
    // to the next event-loop turn, never blocks the scope switch, and is never
    // repeated automatically for the same scope after a cancellation.
    requestPrivilegedSessionForScope(QStringLiteral("host:%1").arg(m_hostPrimaryPath));
}

void MainWindow::setHostDefaultBootEntry()
{
    QString reason;
    if (!hostDefaultBootReady(&reason)) {
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

    const QString repairSection = repairLogSectionIdentity(QStringLiteral("host-default"));
    beginRepairLogSection(repairSection);
    appendLog(QStringLiteral("Starting privileged host default EFI operation on %1 (%2).")
                  .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath),
              QStringLiteral("INFO"), LogEntryKind::Repair);
    bool succeeded = false;
    const QString output = runPrivilegedRequest(
        QStringLiteral("Make host default boot entry"),
        {QStringLiteral("host-default"), m_hostPrimaryPath, m_hostPrimaryComponentPath},
        QByteArray(), &succeeded, true, LogEntryKind::Repair);
    const QString detail = output.trimmed().isEmpty()
        ? QStringLiteral("The privileged helper returned no diagnostic output.")
        : output.trimmed();
    appendLog(QStringLiteral("Host default EFI output\nDiagnostic: Make host default boot entry\n%1").arg(detail),
              succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"), LogEntryKind::Repair);
    // Categorize the operation exactly like the other repair actions; the
    // helper-proven unchanged case is the no-repair-needed category.
    const RepairResultCategory result = categorizeRepairResult(
        succeeded, {QStringLiteral("host-default")}, output);
    appendRepairResultSummary(result,
                              result == RepairResultCategory::Failed
                                  ? shortRepairFailureReason(output)
                                  : QString(),
                              LogEntryKind::Repair, QStringLiteral("host-default"));
    finishRepairLogSection(repairSection);
    // The host default entry changes firmware state, so the complete cached
    // running-host diagnostic set is invalidated. A helper-proven unchanged
    // operation keeps the cached evidence and schedules no regeneration.
    const bool unchanged = result == RepairResultCategory::NoRepairNeeded;
    if (unchanged) {
        appendStatusLog(statusEntryIdentity(QStringLiteral("repair-unchanged")),
                        QStringLiteral("No system changes were detected; cached diagnostics remain valid."),
                        QStringLiteral("INFO"), LogEntryKind::Repair);
    } else {
        clearHostDiagnosticCache();
        scheduleEvidenceRefresh(QStringLiteral("running-host default boot entry changed"));
    }
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
    // Committing a repair target is a scope change away from the running host:
    // leave host maintenance through the one shared exit path first so the
    // button, scope labels, session scope and authorization affordance all
    // update before the target is committed below.
    if (m_hostMaintenanceMode) {
        exitHostMaintenanceMode();
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
    // preferred is guaranteed non-null and non-empty by the gate above.
    m_previewTargetComponentPath = preferred->path;
    // Selecting a repair drive (or re-confirming it) is a scope change: every
    // cached target diagnostic belongs to the previous selection and the
    // complete set is regenerated automatically.
    invalidateAllTargetDiagnostics();
    updateTargetLabels();

    QString message = QStringLiteral("Repair drive selected: %1").arg(disk.path);
    if (!m_previewTargetComponentPath.isEmpty() && m_previewTargetComponentPath != disk.path) {
        message += QStringLiteral("; best detected system component: %1").arg(m_previewTargetComponentPath);
    }
    message += QStringLiteral(". No mount or repair action was performed.");
    appendLog(message);
    statusBar()->showMessage(QStringLiteral("Repair drive selected: %1").arg(disk.path), 4000);
    updateSessionScope();
    scheduleSnapshotPreload();
    scheduleEvidenceRefresh(QStringLiteral("repair target selection changed"));
    // A committed target is already a decrypted, Linux-capable root: a locked
    // LUKS container can never reach this point because the candidate checks
    // above reject it. Request the session once here so later diagnostics and
    // repairs reuse the same authorization.
    if (preferred && !preferred->encrypted
        && preferred->fileSystem.compare(QStringLiteral("crypto_LUKS"), Qt::CaseInsensitive) != 0) {
        requestPrivilegedSessionForScope(QStringLiteral("target:%1").arg(m_previewTargetPath));
    }
}

// ---- MainWindow: privileged helper session ----------------------------------

namespace {

// pkexec/Polkit reports authorization failures on stderr. Keep the last
// non-empty line so the dialog can name the real cause (a missing session
// authentication agent, a refused prompt, ...) instead of only the generic
// cancellation text. Every line is already mirrored into the session log, so
// the dialog stays concise while the log keeps the complete output.
QString lastAuthorizationOutputLine(const QByteArray &output)
{
    const QList<QByteArray> lines = output.split('\n');
    for (auto line = lines.crbegin(); line != lines.crend(); ++line) {
        const QByteArray trimmed = line->trimmed();
        if (!trimmed.isEmpty()) {
            return QString::fromLocal8Bit(trimmed);
        }
    }
    return QString();
}

// A missing or unregisterable Polkit session agent makes pkexec fall back to
// its textual agent, which cannot prompt without a controlling terminal. The
// known signatures are detected so the failure message can name the
// distribution-specific fix instead of leaving an unexplained cancellation.
bool looksLikePolkitAgentFailure(const QString &detail)
{
    const QString lower = detail.toLower();
    return lower.contains(QStringLiteral("textual authentication agent"))
        || lower.contains(QStringLiteral("/dev/tty"))
        || lower.contains(QStringLiteral("no such device or address"))
        || lower.contains(QStringLiteral("no authentication agent"))
        || lower.contains(QStringLiteral("authentication agent is not available"))
        || lower.contains(QStringLiteral("authentication agent was not found"))
        || lower.contains(QStringLiteral("error registering authentication agent"));
}

// Composes the user-facing authorization failure. The stable prefix is kept
// first so existing callers and messages stay compatible; the last pkexec
// output line and, for the known agent failure, the actionable fix follow.
QString authorizationFailureMessage(const QString &detail)
{
    QString message = QStringLiteral("Administrator authorization session was not established.");
    if (!detail.isEmpty()) {
        message += QStringLiteral(" Last authorization output: %1").arg(detail);
    }
    if (looksLikePolkitAgentFailure(detail)) {
        message += QStringLiteral(
            " No working Polkit authentication agent was found. On Alpine install/start one "
            "(apk add polkit-elogind xfce-polkit) and retry.");
    }
    return message;
}

} // namespace

// Returns true when a usable privileged helper session exists, launching one
// pkexec/Polkit conversation when needed. Concurrent callers coalesce onto the
// single in-flight authorization request instead of opening a second prompt;
// the GUI itself never gains privileges.
bool MainWindow::ensurePrivilegedSession(QString *errorMessage)
{
    auto setError = [errorMessage](const QString &text) {
        if (errorMessage) {
            *errorMessage = text;
        }
    };

    const auto sessionUsable = [this] {
        // The UI-test seam grants the session without a QProcess, so a ready
        // flag alone is sufficient when no process exists.
        return m_privilegedSessionReady
            && (!m_privilegedSession || m_privilegedSession->state() != QProcess::NotRunning);
    };

    if (sessionUsable()) {
        m_authorizationDeferredScope.clear();
        updateAuthorizationAffordance();
        setError(QString());
        return true;
    }

    // At most one authorization conversation may exist at a time. A deferred
    // scope request or another caller that is already waiting for Polkit owns
    // the single prompt; concurrent callers (for example a pending automatic
    // evidence refresh that reaches the helper at the same time) coalesce onto
    // that request instead of launching a second pkexec dialog. The wait ends
    // as soon as the shared request publishes its outcome.
    if (m_authorizationRequestInFlight || m_privilegedSessionRequestInFlight) {
        while (!sessionUsable() && !m_privilegedSessionRequestFailed
               && (m_authorizationRequestInFlight || m_privilegedSessionRequestInFlight)) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        }
        if (sessionUsable()) {
            m_authorizationDeferredScope.clear();
            updateAuthorizationAffordance();
            setError(QString());
            return true;
        }
        setError(authorizationFailureMessage(QString()));
        return false;
    }

    // This call now owns the single authorization request for its duration.
    struct InFlightReset {
        explicit InFlightReset(bool *target) : flag(target) { *flag = true; }
        ~InFlightReset() { *flag = false; }
        bool *flag;
    } inFlightReset(&m_privilegedSessionRequestInFlight);

    BusyOperationScope busy(this, QStringLiteral("Requesting administrator authorization"));

#ifdef BOOT_REPAIR_UI_TEST
    // The production authorization path launches pkexec and waits for a real
    // Polkit conversation, which cannot complete in a headless UI test. Mirror
    // the offscreen diagnostics seam: count the request, honor the test's
    // granted/cancelled outcome, and never spawn a privileged process. This
    // branch is compiled only into boot-repair-ui-tests.
    if (m_privilegedSessionReady) {
        m_authorizationDeferredScope.clear();
        updateAuthorizationAffordance();
        setError(QString());
        return true;
    }
    ++m_privilegedSessionRequestCount;
    if (m_uiTestPrivilegedSessionDelayMs > 0) {
        // Hold the request "in flight" while its outcome stays unknown so
        // tests can exercise concurrent callers and verify that they coalesce
        // onto this single request. The outcome is published from inside the
        // loop, exactly like a real Polkit conversation resolving.
        QEventLoop delayLoop;
        QTimer::singleShot(m_uiTestPrivilegedSessionDelayMs, this, [this, &delayLoop] {
            if (m_uiTestPrivilegedSessionGranted) {
                m_privilegedSessionReady = true;
            } else {
                m_privilegedSessionRequestFailed = true;
            }
            delayLoop.quit();
        });
        delayLoop.exec();
    }
    if (m_uiTestPrivilegedSessionGranted) {
        m_privilegedSessionReady = true;
        m_authorizationDeferredScope.clear();
        if (m_lockAuthorizationAction) {
            m_lockAuthorizationAction->setEnabled(true);
        }
        appendStatusLog(statusEntryIdentity(QStringLiteral("authorization-established")),
                        QStringLiteral("Administrator authorization session established. The GUI remains unprivileged."));
        statusBar()->showMessage(QStringLiteral("Administrator authorization active for this Boot Bitch window"), 5000);
        // The deferred automatic diagnostics refresh becomes schedulable now
        // that the session is active.
        handlePrivilegedSessionEstablished();
        updateAuthorizationAffordance();
        setError(QString());
        return true;
    }
    // The production path reads the last non-empty pkexec output line from the
    // session process and maps it through authorizationFailureMessage(); mirror
    // that for a test-provided, already-finished session process so the
    // actionable Polkit-agent hint stays exercisable offscreen. Tests that use
    // only the granted/cancelled seam attach no process and keep the exact
    // generic message.
    QString failureDetail;
    if (m_privilegedSession && m_privilegedSession->state() == QProcess::NotRunning) {
        QByteArray failureOutput = m_privilegedSession->readAllStandardOutput();
        if (m_privilegedSession->processChannelMode() != QProcess::MergedChannels) {
            failureOutput += m_privilegedSession->readAllStandardError();
        }
        failureDetail = lastAuthorizationOutputLine(failureOutput);
    }
    if (!failureDetail.isEmpty()) {
        // Mirror the production path's session log entry so the complete
        // pkexec detail stays available even though the dialog is concise.
        appendLog(QStringLiteral("Authorization session: %1").arg(failureDetail));
    }
    setError(authorizationFailureMessage(failureDetail));
    return false;
#else

    closePrivilegedSession();
    m_privilegedSessionRequestFailed = false;

    QString helperResolution;
    const QString helper = repairHelperPath(&helperResolution);
    if (helper.isEmpty()) {
        setError(QStringLiteral("The privileged Boot Bitch helper was not found. Rebuild or install this source tree."));
        return false;
    }
    // Log the chosen helper and why it won, so an installed package can never
    // silently fall back to a developer source-tree helper.
    appendLog(QStringLiteral("Privileged helper: %1 (%2).")
                  .arg(helper,
                       helperResolution.isEmpty()
                           ? QStringLiteral("resolution detail unavailable")
                           : helperResolution));

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
            if (!wasReady) {
                // The request ended before it ever became usable (cancelled or
                // failed to start). Publish the outcome so coalescing callers
                // stop waiting even though the wait loop may still be running.
                m_privilegedSessionRequestFailed = true;
            }
            if (m_lockAuthorizationAction) {
                m_lockAuthorizationAction->setEnabled(false);
            }
        }
        if (!wasReady && m_privilegedSessionWaitLoop) {
            m_privilegedSessionWaitLoop->quit();
        }
        if (wasReady) {
            appendLog(QStringLiteral("Administrator authorization session ended (helper exit code %1). The next privileged action will request authorization again.")
                          .arg(exitCode));
        }
    });

    QByteArray startupBuffer;
    QString startupFailureDetail;

    // The authorization wait is a plain event loop with no application-side
    // window: the pkexec/Polkit prompt is the only authorization UI. The
    // session QProcess handlers end the wait as soon as the outcome is known,
    // and the safety timer guarantees the loop cannot hang forever.
    QEventLoop waitLoop;
    QTimer safetyTimer;
    safetyTimer.setSingleShot(true);
    safetyTimer.setInterval(300000);
    connect(&safetyTimer, &QTimer::timeout, &waitLoop, &QEventLoop::quit);

    const auto consumeStartupOutput = [this, session, &startupBuffer, &startupFailureDetail] {
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
                appendStatusLog(statusEntryIdentity(QStringLiteral("authorization-established")),
                                QStringLiteral("Administrator authorization session established. The GUI remains unprivileged."));
                statusBar()->showMessage(QStringLiteral("Administrator authorization active for this Boot Bitch window"), 5000);
                // A deferred automatic diagnostics refresh must not trigger
                // its own Polkit prompt; it becomes schedulable now that
                // the session is active.
                handlePrivilegedSessionEstablished();
                updateAuthorizationAffordance();
                if (m_privilegedSessionWaitLoop) {
                    m_privilegedSessionWaitLoop->quit();
                }
                continue;
            }

            if (!line.isEmpty()) {
                startupFailureDetail = QString::fromLocal8Bit(line);
                appendLog(QStringLiteral("Authorization session: %1")
                              .arg(QString::fromLocal8Bit(line)));
            }
        }
    };

    const QMetaObject::Connection startupReadConnection = connect(
        session, &QProcess::readyReadStandardOutput, this, consumeStartupOutput);

    connect(session, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
        if (!m_privilegedSessionWaitLoop) {
            return;
        }
        m_privilegedSessionRequestFailed = true;
        m_privilegedSessionWaitLoop->quit();
    });

    m_privilegedSessionWaitLoop = &waitLoop;
    safetyTimer.start();
    session->start(program, processArguments);
    ++m_privilegedSessionRequestCount;
    // A synchronous FailedToStart already published its outcome; only wait
    // while the authorization is still undecided.
    if (!m_privilegedSessionRequestFailed && !m_privilegedSessionReady) {
        waitLoop.exec();
    }
    safetyTimer.stop();
    m_privilegedSessionWaitLoop = nullptr;
    // Only the startup protocol belongs to this wait; per-request output is
    // consumed by runPrivilegedRequest() once the session is in use.
    disconnect(startupReadConnection);

    if (m_privilegedSessionRequestFailed || !m_privilegedSessionReady
        || !m_privilegedSession || m_privilegedSession->state() == QProcess::NotRunning) {
        m_privilegedSessionRequestFailed = true;
        // pkexec writes the reason for a refused or failed authorization to
        // stderr (merged into the session channel). Drain output that arrived
        // after the wait loop ended so the message can name the real cause
        // instead of only the generic cancellation text; readiness is never
        // published after the wait. Every drained line is still mirrored into
        // the session log.
        startupBuffer += session->readAllStandardOutput();
        const QString drainedDetail = lastAuthorizationOutputLine(startupBuffer);
        const QList<QByteArray> lateStartupLines = startupBuffer.split('\n');
        startupBuffer.clear();
        for (const QByteArray &lateLine : lateStartupLines) {
            const QByteArray line = lateLine.trimmed();
            if (!line.isEmpty()) {
                appendLog(QStringLiteral("Authorization session: %1")
                              .arg(QString::fromLocal8Bit(line)));
            }
        }
        if (!drainedDetail.isEmpty()) {
            startupFailureDetail = drainedDetail;
        }
        setError(authorizationFailureMessage(startupFailureDetail));
        closePrivilegedSession();
        return false;
    }

    m_authorizationDeferredScope.clear();
    updateAuthorizationAffordance();
    setError(QString());
    return true;
#endif
}

void MainWindow::requestPrivilegedSessionForScope(const QString &scopeKey)
{
    if (scopeKey.isEmpty()) {
        return;
    }
    if (m_privilegedSessionReady) {
        // A session is already active; never prompt again.
        updateAuthorizationAffordance();
        return;
    }
    if (m_authorizationScopeRequested == scopeKey) {
        // This scope already asked for authorization. A cancelled request is
        // not retried automatically; the Authorize affordance stays visible.
        updateAuthorizationAffordance();
        return;
    }

    m_authorizationScopeRequested = scopeKey;
    m_authorizationRequestInFlight = true;
    updateAuthorizationAffordance();

    // Defer to the next event-loop turn so the scope switch completes and the
    // window repaints before the authorization dialog opens. This reuses the
    // exact ensurePrivilegedSession() path that repairs use, so an established
    // session is reused by every later diagnostics and repair action.
    QTimer::singleShot(0, this, [this, scopeKey] {
        m_authorizationRequestInFlight = false;
        if (m_privilegedSessionReady || m_authorizationScopeRequested != scopeKey) {
            updateAuthorizationAffordance();
            return;
        }
        QString error;
        if (ensurePrivilegedSession(&error)) {
            m_authorizationDeferredScope.clear();
            updateAuthorizationAffordance();
            return;
        }
        m_authorizationDeferredScope = scopeKey;
        appendStatusLog(statusEntryIdentity(QStringLiteral("authorization-deferred")),
                        QStringLiteral("Administrator authorization was deferred; the next privileged action will request it again."));
        updateAuthorizationAffordance();
    });
}

void MainWindow::authorizePrivilegedSessionNow()
{
    if (m_privilegedSessionReady) {
        updateAuthorizationAffordance();
        return;
    }

    // An explicit user action may always request authorization again, even
    // after a cancellation for the same scope.
    m_authorizationScopeRequested = currentPrivilegedScopeKey();
    QString error;
    if (ensurePrivilegedSession(&error)) {
        m_authorizationDeferredScope.clear();
    } else {
        m_authorizationDeferredScope = m_authorizationScopeRequested;
        appendStatusLog(statusEntryIdentity(QStringLiteral("authorization-deferred")),
                        QStringLiteral("Administrator authorization was deferred; the next privileged action will request it again."));
    }
    updateAuthorizationAffordance();
}

QString MainWindow::currentPrivilegedScopeKey() const
{
    if (m_hostMaintenanceMode && !m_hostPrimaryPath.isEmpty()) {
        return QStringLiteral("host:%1").arg(m_hostPrimaryPath);
    }
    if (!m_previewTargetPath.isEmpty()) {
        return QStringLiteral("target:%1").arg(m_previewTargetPath);
    }
    return QString();
}

void MainWindow::updateAuthorizationAffordance()
{
    if (!m_authorizationStatusLabel || !m_authorizeNowButton) {
        return;
    }

    if (m_privilegedSessionReady) {
        m_authorizationStatusLabel->setVisible(false);
        m_authorizeNowButton->setVisible(false);
        return;
    }

    const bool deferredForCurrentScope = !m_authorizationDeferredScope.isEmpty()
        && m_authorizationDeferredScope == currentPrivilegedScopeKey();
    if (deferredForCurrentScope) {
        m_authorizationStatusLabel->setText(QStringLiteral(
            "Administrator authorization deferred — diagnostics will regenerate after you authorize."));
        m_authorizationStatusLabel->setVisible(true);
        m_authorizeNowButton->setVisible(true);
        m_authorizeNowButton->setEnabled(true);
        return;
    }

    if (m_authorizationRequestInFlight) {
        m_authorizationStatusLabel->setText(QStringLiteral("Requesting administrator authorization…"));
        m_authorizationStatusLabel->setVisible(true);
        m_authorizeNowButton->setVisible(false);
        return;
    }

    m_authorizationStatusLabel->setVisible(false);
    m_authorizeNowButton->setVisible(false);
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

// Sends one request over the line-oriented privileged session and returns the
// captured helper output. Exactly one request owns the gate at a time; a
// re-entrant request is refused or deferred by its caller instead of being
// queued (queueing on the gate from inside the active request's event loop
// deadlocked the UI). A missing protocol DONE record is reported as a
// transport failure rather than a successful command, and the bounded
// watchdog aborts a request whose helper never answers.
QString MainWindow::runPrivilegedRequest(const QString &title,
                                         const QStringList &arguments,
                                         QByteArray secret,
                                         bool *succeeded,
                                         bool showProgressDialog,
                                         LogEntryKind kind,
                                         const QString &statusIdentity)
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

    // Every helper request names its disk as the second argument; that disk is
    // the request's scope identity for supersession decisions.
    const QString requestScope = arguments.size() > 1 ? arguments.at(1) : QString();

    // The helper session answers one request at a time and this function is
    // synchronous. A call that finds the gate held is therefore necessarily
    // re-entrant: the active request's event loop is on this stack, so the
    // active request cannot clear the gate until this nested handler returns.
    // Blocking here used to deadlock exactly that way (the "Waiting for the
    // running operation" hang). Such a request is never queued: a request for
    // a different disk is dropped as superseded, and a same-scope request is
    // refused with a clear log line. Background work (snapshot preload,
    // automatic regeneration) defers itself at its own call sites instead.
    if (m_privilegedOperationActive) {
        const bool superseded = !requestScope.isEmpty()
            && !m_privilegedOperationScopeKey.isEmpty()
            && requestScope != m_privilegedOperationScopeKey;
        const QString activeTitle = m_privilegedOperationTitle.isEmpty()
            ? QStringLiteral("another privileged operation") : m_privilegedOperationTitle;
        // A same-scope request is refused, never queued, and the refusal names
        // the concrete next step so the user is not left with a dead end.
        const QString detail = superseded
            ? QStringLiteral("superseded by the scope change to %1 and was not queued").arg(requestScope)
            : QStringLiteral("refused because '%1' is still running and was not queued. Wait for it to finish and retry, or cancel it").arg(activeTitle);
        appendLog(QStringLiteral("Privileged request '%1' was %2.").arg(title, detail),
                  superseded ? QStringLiteral("INFO") : QStringLiteral("WARNING"), kind);
        return QStringLiteral("ERROR: Privileged request '%1' was %2.\n").arg(title, detail);
    }

    BusyOperationScope busy(this, title);

    // Own the single-request gate for exactly this request. The local guard
    // clears the busy state on every exit path (success, failure, timeout,
    // session loss) and is the only place the gate is released, so a later
    // request can never observe a stuck busy flag. The gate is claimed before
    // authorization so a re-entrant timer cannot slip a second request into
    // the session while the Polkit conversation is still running.
    struct GateGuard {
        MainWindow *window = nullptr;
        bool armed = false;
        ~GateGuard()
        {
            if (armed && window) {
                window->m_privilegedOperationActive = false;
                window->m_privilegedOperationScopeKey.clear();
                window->m_privilegedOperationTitle.clear();
            }
        }
    } gate{this, false};
    m_privilegedOperationActive = true;
    m_privilegedOperationScopeKey = requestScope;
    m_privilegedOperationTitle = title;
    gate.armed = true;

    QString authorizationError;
    if (!ensurePrivilegedSession(&authorizationError)) {
        if (!authorizationError.isEmpty() && !m_evidenceRefreshInProgress) {
            QMessageBox::warning(this, QStringLiteral("Authorization unavailable"), authorizationError);
        }
        secret.fill('\0');
        secret.clear();
        return QStringLiteral("ERROR: %1\n").arg(authorizationError);
    }

    QPointer<QProcess> session = m_privilegedSession;
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
    bool requestTimedOut = false;
    int requestExitCode = -1;

    // Bounded safety net: a helper that never answers must not leave the UI
    // busy forever. The watchdog aborts the wait, reports a clear error and
    // lets the teardown below close the unresponsive session. The limit text
    // is formatted once for both the log line and the captured error.
    const auto timeoutLimitText = [this] {
        return m_privilegedRequestTimeoutMs >= 60000
            ? QStringLiteral("%1 minute(s)").arg(m_privilegedRequestTimeoutMs / 60000)
            : QStringLiteral("%1 ms").arg(m_privilegedRequestTimeoutMs);
    };
    QEventLoop nonModalWaitLoop;
    QTimer requestWatchdog;
    requestWatchdog.setSingleShot(true);
    if (m_privilegedRequestTimeoutMs > 0) {
        requestWatchdog.setInterval(m_privilegedRequestTimeoutMs);
        connect(&requestWatchdog, &QTimer::timeout, &dialog, [&] {
            if (requestDone || requestTimedOut) {
                return;
            }
            requestTimedOut = true;
            const QString limitText = timeoutLimitText();
            appendLog(QStringLiteral("Privileged request '%1' exceeded the %2 safety limit; aborting the request and closing the unresponsive helper session so the UI cannot stay busy indefinitely.")
                          .arg(title, limitText),
                      QStringLiteral("ERROR"), kind);
            status->setText(QStringLiteral("The privileged operation exceeded the %1 safety limit and was aborted. Review the output before retrying.").arg(limitText));
            closeButton->setEnabled(true);
            dialog.setCloseAllowed(true);
            if (showProgressDialog) {
                dialog.reject();
            } else {
                nonModalWaitLoop.quit();
            }
        });
    }

    // Parse every complete protocol line currently buffered. Called from
    // readyReadStandardOutput and again after the request finishes so a
    // helper that writes DONE immediately before exiting is never
    // misclassified as truncated.
    auto consumeSessionOutput = [&] {
        if (!session) {
            return;
        }
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
                requestWatchdog.stop();
                requestSucceeded = requestExitCode == 0;
                status->setText(requestSucceeded
                    ? QStringLiteral("Privileged operation completed successfully. Administrator authorization remains active for this Boot Bitch window.")
                    : QStringLiteral("Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing."));
                closeButton->setEnabled(true);
                dialog.setCloseAllowed(true);
                continue;
            }
        }
    };

    connect(session, &QProcess::readyReadStandardOutput, &dialog, consumeSessionOutput);

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

    if (m_privilegedRequestTimeoutMs > 0) {
        requestWatchdog.start();
    }
    if (showProgressDialog) {
        dialog.exec();
    } else {
        // Read-only diagnostics and inspections already have a persistent
        // results/details pane. Wait for the authorized helper request without
        // showing a redundant modal progress/output dialog; callers display the
        // captured output in-place when the request completes.
        QTimer pollTimer;
        pollTimer.setInterval(20);
        connect(&pollTimer, &QTimer::timeout, &nonModalWaitLoop, [&] {
            if (requestDone || requestTimedOut || !session || session->state() == QProcess::NotRunning) {
                nonModalWaitLoop.quit();
            }
        });
        pollTimer.start();
        if (!requestDone && !requestTimedOut && session && session->state() != QProcess::NotRunning) {
            nonModalWaitLoop.exec();
        }
        pollTimer.stop();
    }
    requestWatchdog.stop();

    if (requestTimedOut) {
        // Abort the unresponsive helper immediately: a timed-out request must
        // not add a multi-second graceful-close wait to the frozen UI. The
        // close below still resets the session state and authorization.
        if (session && session->state() != QProcess::NotRunning) {
            session->terminate();
            if (!session->waitForFinished(1000)) {
                session->kill();
                session->waitForFinished(1000);
            }
        }
        closePrivilegedSession();
    }

    // Drain any output the helper wrote immediately before exiting: Qt may
    // deliver the final buffered bytes after the process is reported as
    // finished, and a complete DONE record must not look truncated.
    if (!requestDone && !requestTimedOut) {
        consumeSessionOutput();
    }

    if (requestTimedOut) {
        const QString timeoutError = QStringLiteral(
            "ERROR: Privileged request '%1' exceeded the %2 safety limit; the request was aborted and the unresponsive helper session was closed. The operation may not have completed.")
            .arg(title, timeoutLimitText());
        if (!captured.isEmpty() && !captured.endsWith(QLatin1Char('\n'))) {
            captured += QLatin1Char('\n');
        }
        captured += timeoutError;
        captured += QLatin1Char('\n');
    } else if (!requestDone) {
        // The helper rejected the request, exited early, or the response was
        // truncated. Never assume a DONE record: report a clear transport
        // error and treat the request as failed.
        const QString protocolError = QStringLiteral(
            "ERROR: The privileged helper did not return a protocol DONE record for this request; the response was rejected, truncated, or the helper exited before completing it. The request is treated as failed.");
        if (!captured.contains(QString::fromLatin1(kHelperProtocolIncompleteMarker))) {
            if (!captured.isEmpty() && !captured.endsWith(QLatin1Char('\n'))) {
                captured += QLatin1Char('\n');
            }
            captured += protocolError;
            captured += QLatin1Char('\n');
        }
        appendLog(QStringLiteral("%1: the privileged helper did not return a protocol DONE record; treating the request as failed.").arg(title),
                  QStringLiteral("ERROR"), kind);
    }

    if (succeeded) {
        *succeeded = requestDone && requestSucceeded;
    }
    const QString completion = QStringLiteral("%1 finished with exit code %2 (%3).")
        .arg(title)
        .arg(requestExitCode)
        .arg(requestDone && requestSucceeded ? QStringLiteral("success") : QStringLiteral("error"));
    const QString completionLevel = requestDone && requestSucceeded
        ? QStringLiteral("INFO")
        : QStringLiteral("ERROR");
    // Diagnostic requests name a stable identity so the automatic regeneration
    // cycle updates one completion entry per scope/disk/key instead of
    // appending one per run. Unlock, snapshot, file-copy and repair requests
    // pass no identity and keep append-only records.
    if (statusIdentity.isEmpty()) {
        appendLog(completion, completionLevel, kind);
    } else {
        appendStatusLog(statusIdentity, completion, completionLevel, kind);
    }

    // Background work that arrived while this request owned the gate was
    // refused rather than queued. Run it once for the now-current scope on the
    // next event-loop turn, after the gate guard above has cleared the busy
    // state.
    if (m_snapshotPreloadDeferred || m_scopeChangeRefreshPending) {
        QTimer::singleShot(0, this, &MainWindow::runDeferredPrivilegedWork);
    }
    return captured;
}

// ---- MainWindow: LUKS unlock ------------------------------------------------

// Unlocks the selected drive's locked LUKS component through the privileged
// helper. The passphrase travels only over the helper pipe (never in command
// arguments or logs) and a rejected passphrase offers a retry without
// re-authorizing the session.
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

    BusyOperationScope busy(this, QStringLiteral("Unlocking %1").arg(preferred->path));

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
                      .arg(preferred->path, disk.path),
                  QStringLiteral("INFO"), LogEntryKind::Unlock);
        const QString unlockOutput = runPrivilegedRequest(QStringLiteral("Unlock LUKS target"),
                                                           {QStringLiteral("unlock"), disk.path, preferred->path},
                                                           secret,
                                                           &processSucceeded,
                                                           false,
                                                           LogEntryKind::Unlock);
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
                          .arg(disk.path), QStringLiteral("WARNING"), LogEntryKind::Unlock);
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
            appendLog(QStringLiteral("LUKS unlock did not complete for %1.").arg(disk.path),
                      QStringLiteral("ERROR"), LogEntryKind::Unlock);
            return;
        }
    }

    appendLog(QStringLiteral("LUKS volume unlocked for %1; refreshing device topology.").arg(disk.path),
              QStringLiteral("INFO"), LogEntryKind::Unlock);
    refreshDevices();

    for (int i = 0; m_deviceTree && i < m_deviceTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_deviceTree->topLevelItem(i);
        if (item && item->data(0, Qt::UserRole).toString() == diskPath) {
            m_deviceTree->setCurrentItem(item);
            break;
        }
    }

    if (m_previewTargetPath == diskPath) {
        // Unlocking the committed target changes its mapper/topology identity:
        // the complete target diagnostic set is invalidated.
        invalidateAllTargetDiagnostics();
    }
    if (m_previewTargetPath == diskPath && m_deviceIndex.contains(diskPath)) {
        const DeviceNode refreshedDisk = m_deviceIndex.value(diskPath);
        const DeviceNode *refreshedPreferred = preferredRepairNode(refreshedDisk);
        m_previewTargetComponentPath = refreshedPreferred ? refreshedPreferred->path : QString();
        updateTargetLabels();
        updateSessionScope();
    }

    statusBar()->showMessage(QStringLiteral("Encrypted target unlocked — select or reselect the repair target"), 6000);
    scheduleSnapshotPreload();
    scheduleEvidenceRefresh(QStringLiteral("encrypted target unlocked"));
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

    for (QLabel *label : {m_repairTargetLabel, m_snapshotTargetLabel, m_fileCopyTargetLabel,
                          m_chrootShellTargetLabel, m_diagnosticScopeLabel}) {
        if (!label) {
            continue;
        }
        label->setText(text);
        label->setToolTip(tooltip);
    }

    if (m_chrootShellTargetLabel) {
        if (m_hostMaintenanceMode) {
            m_chrootShellTargetLabel->setText(QStringLiteral("Running Host: %1").arg(
                m_hostPrimaryPath.isEmpty() ? QStringLiteral("unresolved") : m_hostPrimaryPath));
            m_chrootShellTargetLabel->setToolTip(QStringLiteral(
                "Running host selected for the Host Shell: %1\nRoot component: %2")
                .arg(m_hostPrimaryPath, m_hostPrimaryComponentPath));
        }
    }

    if (m_systemTargetLabel) {
        // The committed target is the resolved system component, not merely
        // the physical disk: repairs run against e.g. /dev/sda3. Keep the
        // disk model as the identity prefix (model • component path) and fall
        // back to the disk path only when no component is resolved yet.
        const QString committedPath = !m_previewTargetComponentPath.isEmpty()
            ? m_previewTargetComponentPath : m_previewTargetPath;
        QString committedSummary = committedPath;
        if (!m_previewTargetPath.isEmpty() && m_deviceIndex.contains(m_previewTargetPath)) {
            const QString model = combinedModel(m_deviceIndex.value(m_previewTargetPath));
            if (!model.isEmpty()) {
                committedSummary = QStringLiteral("%1  •  %2").arg(model, committedPath);
            }
        }
        m_systemTargetLabel->setText(m_hostMaintenanceMode
            ? QStringLiteral("Host maintenance: %1").arg(m_hostPrimaryPath)
            : (m_previewTargetPath.isEmpty()
                ? QStringLiteral("Committed target: none")
                : QStringLiteral("Committed target: %1").arg(committedSummary)));
        m_systemTargetLabel->setToolTip(m_hostMaintenanceMode
            ? QStringLiteral("Explicit native running-host maintenance is active. Ordinary target repairs, file copy and chroot remain unavailable; running-host Snapper @ snapshot rollback is available in the Snapshots tab.")
            : (m_previewTargetPath.isEmpty()
                ? QStringLiteral("Row selection is inspection only. Press Select Target to commit a repair drive.")
                : QStringLiteral("Committed repair target. Repair, Diagnostics, Snapshots and File Copy target this physical drive until another drive is explicitly selected with Select Target.\n%1").arg(tooltip)));
    }

    updateCommittedTargetVisual();
    updateFileCopyDirection();
    updateSnapshotControls();
    updateChrootShellMode();
    if (m_chrootShellRunButton) {
        QString shellReason;
        m_chrootShellRunButton->setEnabled(shellCommandReady(&shellReason));
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
                "Committed repair target. Clicking another row changes only the inspection highlight; repair actions continue to target %1 until Select Target is pressed on another physical drive.\n%2")
                .arg(path, originalStatus));
        } else {
            item->setToolTip(1, originalStatus);
        }
    }
}

// ---- MainWindow: Btrfs snapshots --------------------------------------------

void MainWindow::clearSnapshotResults()
{
    if (m_snapshotTable) {
        m_snapshotTable->setRowCount(0);
    }
    if (m_snapshotDetails) {
        m_snapshotDetails->clear();
    }
    m_snapshotResultIdentity = snapshotScopeIdentity();
}

void MainWindow::updateSnapshotControls()
{
    if (!m_snapshotLoadButton || !m_snapshotInspectButton || !m_snapshotRollbackButton || !m_snapshotTable) {
        return;
    }

    const bool hostScope = m_hostMaintenanceMode;
    const QString identity = snapshotScopeIdentity();
    if (m_snapshotResultIdentity != identity) {
        // A target/scope change starts a new inventory generation: the next
        // automatic preload is allowed exactly once for it, and any completed
        // load from the previous scope can never satisfy the new one.
        ++m_snapshotScopeGeneration;
        clearSnapshotResults();
    }

    QString reason;
    bool ready = hostScope ? hostMaintenanceReady(&reason) : repairTargetReady(&reason);
    const QString componentPath = snapshotComponentPath();
    if (ready && m_deviceIndex.contains(componentPath)) {
        const DeviceNode component = m_deviceIndex.value(componentPath);
        if (component.fileSystem.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) != 0) {
            ready = false;
            reason = hostScope
                ? QStringLiteral("The running host root is not Btrfs, so Btrfs snapshots are not available.")
                : QStringLiteral("The selected Linux root is not Btrfs, so Btrfs snapshots are not available.");
        }
    }

    m_snapshotLoadButton->setEnabled(ready);
    m_snapshotLoadButton->setToolTip(ready
        ? (hostScope
            ? QStringLiteral("Enumerate running-host root snapshots through a temporary privileged read-only Btrfs mount.")
            : QStringLiteral("Enumerate root snapshots through a temporary privileged read-only Btrfs mount."))
        : reason);

    const bool rowSelected = ready && !m_snapshotTable->selectedItems().isEmpty();
    m_snapshotInspectButton->setEnabled(rowSelected);
    m_snapshotInspectButton->setToolTip(rowSelected
        ? QStringLiteral("Inspect the selected snapshot read-only.")
        : (ready ? QStringLiteral("Select a snapshot row first.") : reason));

    bool rollbackCandidate = rowSelected;
    QString rollbackReason;
    if (rollbackCandidate) {
        const int row = m_snapshotTable->currentRow();
        QTableWidgetItem *statusItem = row >= 0 ? m_snapshotTable->item(row, 4) : nullptr;
        rollbackCandidate = statusItem && statusItem->text().startsWith(QStringLiteral("Linux root snapshot"));
        if (!rollbackCandidate) {
            rollbackReason = QStringLiteral("Select a valid Linux root snapshot first.");
        } else if (hostScope) {
            // Host rollback is enabled only from cached running-host capability
            // evidence and fails closed when the evidence is missing.
            QString capabilityReason;
            if (!hostSnapshotRollbackAvailable(&capabilityReason)) {
                rollbackCandidate = false;
                rollbackReason = capabilityReason;
            }
        }
    } else {
        rollbackReason = ready ? QStringLiteral("Select a valid Linux root snapshot first.") : reason;
    }
    m_snapshotRollbackButton->setEnabled(rollbackCandidate);
    m_snapshotRollbackButton->setToolTip(rollbackCandidate
        ? (hostScope
            ? QStringLiteral("Validate a running-host rollback plan read-only, preserve the running @ as @rollback-before-*, promote a writable snapshot copy and reconcile the boot stack in a scratch chroot. A reboot is required and is never automatic.")
            : QStringLiteral("Validate a rollback plan read-only, preserve the current @ root, promote a writable snapshot copy, reconcile initramfs/UKI/GRUB and auto-restore the old @ if validation fails."))
        : rollbackReason);
}

// ---- MainWindow: snapshot scope accessors -----------------------------------

QString MainWindow::snapshotDiskPath() const
{
    return m_hostMaintenanceMode ? m_hostPrimaryPath : m_previewTargetPath;
}

QString MainWindow::snapshotComponentPath() const
{
    return m_hostMaintenanceMode ? m_hostPrimaryComponentPath : m_previewTargetComponentPath;
}

QString MainWindow::snapshotHelperCommand() const
{
    return m_hostMaintenanceMode ? QStringLiteral("host-snapshots") : QStringLiteral("snapshots");
}

// Scope identity of the Btrfs inventory the preload and request paths serve:
// "host:<disk>" in Host Maintenance, otherwise the committed target disk.
QString MainWindow::snapshotScopeIdentity() const
{
    if (m_hostMaintenanceMode) {
        return m_hostPrimaryPath.isEmpty()
            ? QString()
            : QStringLiteral("host:%1").arg(m_hostPrimaryPath);
    }
    return m_previewTargetPath;
}

// Target/host + resolved component identity of the Btrfs inventory the preload
// and request paths serve. Empty when no scope is committed.
QString MainWindow::snapshotScopeKey() const
{
    const QString identity = snapshotScopeIdentity();
    const QString component = snapshotComponentPath();
    if (identity.isEmpty() || component.isEmpty()) {
        return QString();
    }
    return identity + QLatin1Char('\n') + component;
}

void MainWindow::scheduleSnapshotPreload()
{
    if (!m_snapshotTable) {
        return;
    }
    const QString scopeKey = snapshotScopeKey();
    if (scopeKey.isEmpty()) {
        return;
    }
    // Exactly one automatic preload per scope generation: a completed load
    // for the current scope and an in-flight request for the same
    // target+component both make further scheduling a no-op. This is what
    // prevents the reported duplicate "Loading Btrfs snapshots" pair after a
    // selection/host-maintenance transition.
    if (m_snapshotLoadedGeneration == m_snapshotScopeGeneration) {
        return;
    }
    if (m_snapshotInventoryInFlight && m_snapshotRequestScopeKey == scopeKey) {
        return;
    }
    if (m_privilegedOperationActive) {
        // A privileged request owns the gate. A snapshot preload is background
        // work, so it is never queued behind that request (the nested wait
        // deadlocked the UI). Remember exactly one retry for the then-current
        // scope; the gate release re-runs it.
        if (!m_snapshotPreloadDeferred) {
            appendLog(QStringLiteral("Btrfs snapshot preload for %1 is deferred until the running privileged operation finishes; the request was not queued.")
                          .arg(snapshotDiskPath()),
                      QStringLiteral("INFO"), LogEntryKind::Snapshot);
        }
        m_snapshotPreloadDeferred = true;
        return;
    }
    if (m_snapshotPreloadScheduled) {
        // One pending preload already serves whatever scope is current when it
        // runs; a second queued load for the same or a newer scope is never
        // needed.
        return;
    }
    m_snapshotPreloadScheduled = true;
    if (m_snapshotDetails && m_snapshotTable->rowCount() == 0) {
        m_snapshotDetails->setPlainText(QStringLiteral("Loading Btrfs snapshots in the background…"));
    }
    QTimer::singleShot(0, this, [this] {
        m_snapshotPreloadScheduled = false;
        // The pending preload always serves the scope that is current when it
        // runs, so a transition after it was queued can never load a stale
        // target, and a load that already completed is not repeated.
        if (m_snapshotLoadedGeneration == m_snapshotScopeGeneration) {
            return;
        }
        const QString currentScope = snapshotScopeKey();
        if (currentScope.isEmpty()) {
            return;
        }
        if (m_snapshotInventoryInFlight && m_snapshotRequestScopeKey == currentScope) {
            return;
        }
        // A scope whose filesystem is already known to be non-Btrfs has no
        // snapshot inventory to preload. State the applicability instead of
        // asking the privileged helper for a guaranteed "not applicable"
        // answer.
        const QString componentPath = snapshotComponentPath();
        if (m_deviceIndex.contains(componentPath)) {
            const DeviceNode component = m_deviceIndex.value(componentPath);
            if (!component.fileSystem.isEmpty()
                && component.fileSystem.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) != 0) {
                showSnapshotInventoryNotApplicable(component.fileSystem);
                return;
            }
        }
        QString reason;
        const bool ready = m_hostMaintenanceMode ? hostMaintenanceReady(&reason) : repairTargetReady(&reason);
        if (ready) {
            loadSnapshots();
        }
    });
}

// Informational, never error-styled: a non-Btrfs scope simply has no Btrfs
// snapshot inventory. The page keeps its neutral empty state and the Logs tab
// records an INFO entry that the operation is not applicable.
void MainWindow::showSnapshotInventoryNotApplicable(const QString &fileSystem)
{
    // No inventory request is needed for this scope, so a deferred preload
    // for it is satisfied by this informational state.
    m_snapshotPreloadDeferred = false;
    const QString fs = fileSystem.trimmed().isEmpty()
        ? QStringLiteral("unknown (not Btrfs)") : fileSystem.trimmed();
    const bool hostScope = m_hostMaintenanceMode;
    // Refresh the controls first: a scope change clears the results pane, so
    // the informational text must be written after that reset.
    updateSnapshotControls();
    // The applicability is this scope's complete answer: further automatic
    // preloads are deduped like a completed inventory.
    m_snapshotLoadedGeneration = m_snapshotScopeGeneration;
    if (m_snapshotDetails) {
        m_snapshotDetails->setPlainText(hostScope
            ? QStringLiteral("Btrfs snapshot inventory is not applicable: the running host root filesystem is %1, not Btrfs.\n\nNo snapshot was loaded and the running host was not modified.").arg(fs)
            : QStringLiteral("Btrfs snapshot inventory is not applicable: the selected target filesystem is %1, not Btrfs.\n\nNo snapshot was loaded and the target was not modified.").arg(fs));
    }
    appendLog(hostScope
                  ? QStringLiteral("Btrfs snapshot inventory is not applicable for the running host: the root filesystem is %1, not Btrfs. No snapshots were loaded and nothing was changed.").arg(fs)
                  : QStringLiteral("Btrfs snapshot inventory is not applicable for %1: the selected target filesystem is %2, not Btrfs. No snapshots were loaded and nothing was changed.")
                        .arg(m_previewTargetComponentPath, fs),
              QStringLiteral("INFO"), LogEntryKind::Snapshot);
}

void MainWindow::loadSnapshots()
{
    const bool hostScope = m_hostMaintenanceMode;
    QString reason;
    if (!(hostScope ? hostMaintenanceReady(&reason) : repairTargetReady(&reason))) {
        QMessageBox::warning(this, QStringLiteral("Snapshot inventory unavailable"), reason);
        return;
    }

    // A known non-Btrfs component is resolved before any privileged request.
    const QString scopeComponentPath = snapshotComponentPath();
    if (m_deviceIndex.contains(scopeComponentPath)) {
        const DeviceNode component = m_deviceIndex.value(scopeComponentPath);
        if (!component.fileSystem.isEmpty()
            && component.fileSystem.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) != 0) {
            showSnapshotInventoryNotApplicable(component.fileSystem);
            return;
        }
    }

    // A second request for the same scope+component while one is already in
    // flight is a duplicate (for example a manual click racing the preload);
    // it must never issue another privileged inventory request.
    const QString requestScopeKey = snapshotScopeKey();
    if (m_snapshotInventoryInFlight && m_snapshotRequestScopeKey == requestScopeKey) {
        return;
    }

    if (m_privilegedOperationActive) {
        // The gate is held by another request. Never queue the inventory
        // behind it; remember one retry for the then-current scope and let
        // the gate release re-run it. A manual Load Snapshots click during a
        // read-only request is therefore deferred, not lost.
        if (!m_snapshotPreloadDeferred) {
            appendLog(QStringLiteral("Btrfs snapshot inventory for %1 is deferred until the running privileged operation finishes; the request was not queued.")
                          .arg(snapshotDiskPath()),
                      QStringLiteral("INFO"), LogEntryKind::Snapshot);
        }
        m_snapshotPreloadDeferred = true;
        if (m_snapshotDetails && m_snapshotTable && m_snapshotTable->rowCount() == 0) {
            m_snapshotDetails->setPlainText(QStringLiteral("Loading Btrfs snapshots after the running operation finishes…"));
        }
        return;
    }

    BusyOperationScope busy(this, hostScope
        ? QStringLiteral("Loading running-host Btrfs snapshots")
        : QStringLiteral("Loading Btrfs snapshots"));

    const QString requestTargetPath = snapshotDiskPath();
    const QString requestComponentPath = snapshotComponentPath();
    const QString helperCommand = snapshotHelperCommand();
    bool succeeded = false;
    m_snapshotInventoryInFlight = true;
    m_snapshotRequestScopeKey = requestScopeKey;
    appendLog(hostScope
                  ? QStringLiteral("Loading running-host Btrfs snapshots read-only for %1 (%2).")
                        .arg(requestTargetPath, requestComponentPath)
                  : QStringLiteral("Loading Btrfs snapshots read-only for %1 (%2).")
                        .arg(requestTargetPath, requestComponentPath),
              QStringLiteral("INFO"), LogEntryKind::Snapshot);
    const QString output = runPrivilegedRequest(
        hostScope ? QStringLiteral("Load running-host Btrfs snapshots")
                  : QStringLiteral("Load Btrfs snapshots"),
        {helperCommand, requestTargetPath, requestComponentPath, QStringLiteral("list")},
        QByteArray(),
        &succeeded,
        false,
        LogEntryKind::Snapshot);

    // The request has completed: the in-flight identity no longer blocks a
    // later scope change, and the result below decides whether this scope is
    // satisfied.
    m_snapshotInventoryInFlight = false;
    m_snapshotRequestScopeKey.clear();

    // A scope change while the request was in flight makes this inventory
    // belong to a scope that is no longer selected. Never apply one scope's
    // rows to another: drop the stale result and let the deferred preload
    // serve the new scope.
    if (requestScopeKey != snapshotScopeKey()
        || requestTargetPath != snapshotDiskPath()
        || requestComponentPath != snapshotComponentPath()) {
        appendLog(QStringLiteral("Discarded the Btrfs snapshot inventory for %1 (%2) because the active scope changed while the request was running.")
                      .arg(requestTargetPath, requestComponentPath),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
        m_snapshotPreloadDeferred = true;
        updateSnapshotControls();
        return;
    }
    // The completed request matches the active scope, so any retry that was
    // remembered while it was in flight is now satisfied.
    m_snapshotPreloadDeferred = false;

    if (!succeeded) {
        if (m_snapshotDetails) {
            m_snapshotDetails->setPlainText(output.isEmpty()
                ? QStringLiteral("Snapshot inventory failed. See Logs for details.")
                : output);
        }
        appendLog(QStringLiteral("Btrfs snapshot inventory failed."), QStringLiteral("ERROR"),
                  LogEntryKind::Snapshot);
        updateSnapshotControls();
        return;
    }

    // The helper reports a non-Btrfs scope as an informational outcome (exit
    // code 0) so a filesystem that simply cannot carry Btrfs snapshots never
    // produces a failed state or an error-styled log entry.
    if (output.contains(QStringLiteral("SNAPSHOT_INVENTORY_NOT_APPLICABLE=1"))) {
        showSnapshotInventoryNotApplicable(QString());
        return;
    }

    m_snapshotResultIdentity = snapshotScopeIdentity();

    auto decode = [](const QString &encoded) {
        return QString::fromUtf8(QByteArray::fromBase64(encoded.toLatin1()));
    };

    QList<SnapshotInventoryRow> rows;
    const QStringList lines = output.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (!line.startsWith(QStringLiteral("SNAPSHOT\t"))) {
            continue;
        }
        const QStringList fields = line.split(QLatin1Char('\t'), Qt::KeepEmptyParts);
        if (fields.size() < 7) {
            continue;
        }

        SnapshotInventoryRow snapshot;
        snapshot.id = fields.at(1);
        snapshot.created = decode(fields.at(2));
        snapshot.type = decode(fields.at(3));
        snapshot.description = decode(fields.at(4));
        snapshot.status = decode(fields.at(5));
        snapshot.relativePath = decode(fields.at(6));
        rows.append(snapshot);
    }

    // The inventory renders through populateSnapshotTable(), which stores the
    // numeric/date sort keys and presents the newest recovery points first.
    populateSnapshotTable(rows);

    const int count = rows.size();
    if (m_snapshotDetails) {
        const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        if (count > 0) {
            m_snapshotDetails->setPlainText(hostScope
                ? QStringLiteral("Loaded %1 running-host Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.\n\nNo snapshot or host file was modified.")
                      .arg(count).arg(stamp)
                : QStringLiteral("Loaded %1 Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.\n\nNo snapshot or target file was modified.")
                      .arg(count).arg(stamp));
        } else {
            m_snapshotDetails->setPlainText(hostScope
                ? QStringLiteral("No Snapper-style Btrfs root snapshots or Boot Bitch rollback backups were found on the running host. The scan was read-only.")
                : QStringLiteral("No Snapper-style Btrfs root snapshots were found on the selected target. The scan was read-only."));
        }
    }
    appendLog(hostScope
                  ? QStringLiteral("Loaded %1 running-host Btrfs snapshot(s) read-only.").arg(count)
                  : QStringLiteral("Loaded %1 Btrfs snapshot(s) read-only for the selected repair target.").arg(count),
              QStringLiteral("INFO"), LogEntryKind::Snapshot);
    updateSnapshotControls();
    // The scope is satisfied: automatic preloads for this generation are
    // deduped until the next target/scope change.
    m_snapshotLoadedGeneration = m_snapshotScopeGeneration;
}

void MainWindow::populateSnapshotTable(const QList<SnapshotInventoryRow> &rows)
{
    if (!m_snapshotTable) {
        return;
    }

    // Populate with sorting disabled so a mid-fill sort can never reorder or
    // lose rows, then re-enable and re-apply the active (or default) sort.
    m_snapshotTable->setSortingEnabled(false);
    m_snapshotTable->setRowCount(0);
    for (const SnapshotInventoryRow &snapshot : rows) {
        const int row = m_snapshotTable->rowCount();
        m_snapshotTable->insertRow(row);

        // Snapshot numbers sort numerically and the creation timestamp sorts
        // chronologically; the display text keeps its original formatting.
        auto *idItem = new SortableTableItem(snapshot.id, numericSortKey(snapshot.id));
        idItem->setData(Qt::UserRole, snapshot.id);
        idItem->setData(Qt::UserRole + 1, snapshot.relativePath);
        idItem->setToolTip(snapshot.relativePath);
        m_snapshotTable->setItem(row, 0, idItem);
        m_snapshotTable->setItem(row, 1, new SortableTableItem(
            snapshot.created, QVariant::fromValue(snapshotCreatedDateTime(snapshot.created))));
        m_snapshotTable->setItem(row, 2, new SortableTableItem(snapshot.type));
        m_snapshotTable->setItem(row, 3, new SortableTableItem(snapshot.description));
        m_snapshotTable->setItem(row, 4, new SortableTableItem(snapshot.status));
    }
    m_snapshotTable->setSortingEnabled(true);

    // Compute compact one-line rows after all columns have been populated.
    // This also deliberately shrinks rows that were measured while the tab
    // was hidden at a narrow width.
    applyActiveSort(m_snapshotTable, 1, Qt::DescendingOrder);
    resizeSnapshotRows();
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

    const bool hostScope = m_hostMaintenanceMode;
    BusyOperationScope busy(this, hostScope
        ? QStringLiteral("Inspecting running-host snapshot %1").arg(snapshotId)
        : QStringLiteral("Inspecting snapshot %1").arg(snapshotId));

    bool succeeded = false;
    appendLog(hostScope
                  ? QStringLiteral("Inspecting running-host Btrfs snapshot %1 read-only.").arg(snapshotId)
                  : QStringLiteral("Inspecting Btrfs snapshot %1 read-only.").arg(snapshotId),
              QStringLiteral("INFO"), LogEntryKind::Snapshot);
    const QString output = runPrivilegedRequest(
        hostScope ? QStringLiteral("Inspect running-host Btrfs snapshot %1").arg(snapshotId)
                  : QStringLiteral("Inspect Btrfs snapshot %1").arg(snapshotId),
        {snapshotHelperCommand(), snapshotDiskPath(), snapshotComponentPath(),
         QStringLiteral("inspect"), snapshotId},
        QByteArray(),
        &succeeded,
        false,
        LogEntryKind::Snapshot);

    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Captured: %1\n\n%2").arg(stamp, output));
    appendLog(QStringLiteral("Snapshot %1 read-only inspection %2.")
                  .arg(snapshotId, succeeded ? QStringLiteral("completed") : QStringLiteral("failed")),
              succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"), LogEntryKind::Snapshot);
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

    // Host Maintenance runs the running-host name-preserving transaction; the
    // target branch below stays byte-compatible with the offline flow.
    if (m_hostMaintenanceMode) {
        rollbackHostSnapshot(snapshotId);
        return;
    }

    BusyOperationScope busy(this, QStringLiteral("Rolling back snapshot %1").arg(snapshotId));

    bool planSucceeded = false;
    appendLog(QStringLiteral("Preparing read-only transactional rollback plan for Btrfs snapshot %1.").arg(snapshotId),
              QStringLiteral("INFO"), LogEntryKind::Snapshot);
    const QString plan = runPrivilegedRequest(
        QStringLiteral("Preflight snapshot rollback %1").arg(snapshotId),
        {QStringLiteral("snapshots"), m_previewTargetPath, m_previewTargetComponentPath,
         QStringLiteral("plan"), snapshotId},
        QByteArray(),
        &planSucceeded,
        true,
        LogEntryKind::Snapshot);

    const QString captured = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Rollback preflight captured: %1\n\n%2").arg(captured, plan));
    if (!planSucceeded || !plan.contains(QStringLiteral("PLAN_OK=1"))) {
        appendLog(QStringLiteral("Snapshot %1 rollback preflight failed; no target data was changed.").arg(snapshotId),
                  QStringLiteral("ERROR"), LogEntryKind::Snapshot);
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
        appendLog(QStringLiteral("Snapshot rollback cancelled after preflight; no target data was changed."),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
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
        appendLog(QStringLiteral("Snapshot rollback cancelled because the confirmation text did not match ROLLBACK."),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
        return;
    }

    bool succeeded = false;
    appendLog(QStringLiteral("Starting transactional Btrfs rollback to snapshot %1 on committed target %2.")
                  .arg(snapshotId, m_previewTargetPath),
              QStringLiteral("WARNING"), LogEntryKind::Snapshot);
    const QString output = runPrivilegedRequest(
        QStringLiteral("Roll back to Btrfs snapshot %1").arg(snapshotId),
        {QStringLiteral("snapshots"), m_previewTargetPath, m_previewTargetComponentPath,
         QStringLiteral("rollback"), snapshotId},
        QByteArray(),
        &succeeded,
        true,
        LogEntryKind::Snapshot);

    const QString finished = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Rollback finished: %1\n\n%2").arg(finished, output));
    if (succeeded && output.contains(QStringLiteral("ROLLBACK_RESULT=SUCCESS"))) {
        invalidateAllTargetDiagnostics();
        updateFullRepairSummary();
        if (m_snapshotTable) {
            m_snapshotTable->setRowCount(0);
        }
        m_snapshotResultIdentity = currentTargetDiagnosticCacheIdentity();
        m_snapshotDetails->setPlainText(QStringLiteral("Rollback completed: %1\n\n%2\n\nSnapshot inventory was cleared because the active root changed. Choose Load Snapshots to refresh it.")
                                            .arg(finished, output));
        appendLog(QStringLiteral("STALE DIAGNOSTICS: snapshot %1 rollback changed the active root; snapshot inventory and cached diagnostics were invalidated. Regenerate diagnostics before the next repair.")
                      .arg(snapshotId),
                  QStringLiteral("WARNING"), LogEntryKind::Snapshot);
        refreshDevices();
        updateSnapshotControls();
        scheduleEvidenceRefresh(QStringLiteral("snapshot rollback completed"));
        QMessageBox::information(this, QStringLiteral("Snapshot rollback complete"),
                                 QStringLiteral("Snapshot %1 was promoted to a writable @ root and the boot stack passed reconciliation.\n\nThe previous @ was retained under a timestamped @rollback-before-* name. Reboot using the normal boot path when ready.")
                                     .arg(snapshotId));
    } else {
        invalidateAllTargetDiagnostics();
        updateFullRepairSummary();
        if (m_snapshotTable) {
            m_snapshotTable->setRowCount(0);
        }
        m_snapshotResultIdentity = currentTargetDiagnosticCacheIdentity();
        m_snapshotDetails->setPlainText(QStringLiteral("Rollback attempt finished: %1\n\n%2\n\nCached diagnostics and snapshot inventory were cleared because the rollback request may have changed and/or restored target state.")
                                            .arg(finished, output));
        appendLog(QStringLiteral("STALE DIAGNOSTICS: snapshot %1 rollback did not complete; cached diagnostics and snapshots were invalidated. Regenerate diagnostics and review the operation output before the next repair or reboot.")
                      .arg(snapshotId),
                  QStringLiteral("ERROR"), LogEntryKind::Snapshot);
        refreshDevices();
        updateSnapshotControls();
        scheduleEvidenceRefresh(QStringLiteral("snapshot rollback failed"));
        QMessageBox::critical(this, QStringLiteral("Snapshot rollback failed"),
                              QStringLiteral("The rollback did not complete successfully. Do not reboot until you review the Snapshots output and Logs. The helper attempts to restore the preserved @ automatically when post-switch validation fails."));
    }
}

// Running-host rollback capability from the cached running-host capability
// preamble. The `Host snapshot rollback:` line is evidence, not a 14th
// `Repair tool` key; missing evidence fails closed.
bool MainWindow::hostSnapshotRollbackAvailable(QString *reason) const
{
    auto reject = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
        return false;
    };
    if (!m_hostMaintenanceMode) {
        return reject(QStringLiteral("Running-host snapshot rollback is only available in Host Maintenance."));
    }
    const QString prefix = QStringLiteral("Host snapshot rollback: ");
    bool hadEvidence = false;
    bool available = false;
    for (const QString &evidence : m_hostDiagnosticCache) {
        for (const QString &line : evidence.split(QLatin1Char('\n'))) {
            if (!line.startsWith(prefix)) {
                continue;
            }
            hadEvidence = true;
            const QString state = line.mid(prefix.size()).trimmed();
            if (state == QStringLiteral("available")) {
                available = true;
            } else if (state.startsWith(QStringLiteral("unavailable|"))) {
                const QString detail = state.mid(QStringLiteral("unavailable|").size()).trimmed();
                return reject(detail.isEmpty()
                    ? QStringLiteral("Running-host snapshot rollback is unavailable.")
                    : detail);
            } else {
                return reject(QStringLiteral("Running-host snapshot rollback has unknown capability evidence."));
            }
        }
    }
    if (!available) {
        return reject(hadEvidence
            ? QStringLiteral("Running-host snapshot rollback is unavailable.")
            : QStringLiteral("Run running-host diagnostics in Host Maintenance first."));
    }
    if (reason) {
        *reason = QStringLiteral("Running-host snapshot rollback is available.");
    }
    return true;
}

void MainWindow::rollbackHostSnapshot(const QString &snapshotId)
{
    // A previous rollback staged before the reboot may be replaced only after
    // an explicit warning: the new transaction preserves the currently running
    // root, while the earlier staged root stays on disk but is no longer the
    // recorded undo point.
    if (m_hostRebootRequired) {
        QMessageBox replacement(this);
        replacement.setIcon(QMessageBox::Warning);
        replacement.setWindowTitle(QStringLiteral("Rollback already staged"));
        replacement.setText(QStringLiteral("A running-host rollback to %1 is already staged and takes effect on the next reboot.")
                                .arg(m_hostRebootSnapshotId.isEmpty()
                                         ? QStringLiteral("an earlier snapshot")
                                         : QStringLiteral("snapshot %1").arg(m_hostRebootSnapshotId)));
        replacement.setInformativeText(QStringLiteral(
            "Rolling back again preserves the currently running root as a new @rollback-before-* backup and replaces the staged snapshot. The earlier staged root remains on disk but is no longer the recorded undo point.\n\n"
            "Continue only if you intend to replace the staged rollback."));
        replacement.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
        replacement.setDefaultButton(QMessageBox::Cancel);
        replacement.button(QMessageBox::Yes)->setText(QStringLiteral("Replace Staged Rollback"));
        if (replacement.exec() != QMessageBox::Yes) {
            appendLog(QStringLiteral("Running-host rollback cancelled at the replacement warning; the previously staged rollback remains in effect."),
                      QStringLiteral("INFO"), LogEntryKind::Snapshot);
            return;
        }
    }

    BusyOperationScope busy(this, QStringLiteral("Rolling back running-host snapshot %1").arg(snapshotId));

    bool planSucceeded = false;
    appendLog(QStringLiteral("Preparing read-only running-host rollback plan for snapshot %1.").arg(snapshotId),
              QStringLiteral("INFO"), LogEntryKind::Snapshot);
    const QString plan = runPrivilegedRequest(
        QStringLiteral("Preflight running-host rollback %1").arg(snapshotId),
        {QStringLiteral("host-snapshots"), m_hostPrimaryPath, m_hostPrimaryComponentPath,
         QStringLiteral("plan"), snapshotId},
        QByteArray(),
        &planSucceeded,
        true,
        LogEntryKind::Snapshot);

    const QString captured = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Running-host rollback preflight captured: %1\n\n%2").arg(captured, plan));
    if (!planSucceeded || !plan.contains(QStringLiteral("PLAN_OK=1"))) {
        appendLog(QStringLiteral("Running-host snapshot %1 rollback preflight failed; no host data was changed.").arg(snapshotId),
                  QStringLiteral("ERROR"), LogEntryKind::Snapshot);
        QMessageBox::warning(this, QStringLiteral("Host rollback preflight failed"),
                             QStringLiteral("The selected snapshot did not pass the running-host rollback preflight. No rollback was performed.\n\nReview the Snapshots details pane and Logs."));
        return;
    }

    QMessageBox warning(this);
    warning.setIcon(QMessageBox::Warning);
    warning.setWindowTitle(QStringLiteral("Confirm running-host snapshot rollback"));
    warning.setText(QStringLiteral("Roll the running host back to snapshot %1?").arg(snapshotId));
    warning.setInformativeText(QStringLiteral(
        "Running host: %1\nRoot filesystem: %2\n\n"
        "The running host keeps running the current root until you reboot. On the next reboot it will start the selected snapshot instead.\n\n"
        "Snapper/Boot Bitch take a snapshot of the current system first, so the present state is preserved automatically as an @rollback-before-* undo point.\n\n"
        "Boot Bitch cannot guarantee that no data is lost: changes made after the selected snapshot are not part of the rolled-back root, and separate subvolumes such as /home are not rolled back.\n\n"
        "All users are signed out and unsaved work is lost when the host reboots. The reboot is never automatic."
    ).arg(m_hostPrimaryPath, m_hostPrimaryComponentPath));
    warning.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    warning.setDefaultButton(QMessageBox::Cancel);
    warning.button(QMessageBox::Yes)->setText(QStringLiteral("Continue to Confirmation"));
    if (warning.exec() != QMessageBox::Yes) {
        appendLog(QStringLiteral("Running-host rollback cancelled after preflight; no host data was changed."),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
        return;
    }

    bool ok = false;
    const QString typed = QInputDialog::getText(
        this,
        QStringLiteral("Type ROLLBACK to continue"),
        QStringLiteral("This operation stages the selected snapshot as the running host's next root and rebuilds boot artifacts.\n\nType ROLLBACK exactly to continue:"),
        QLineEdit::Normal,
        QString(),
        &ok).trimmed();
    if (!ok || typed != QStringLiteral("ROLLBACK")) {
        appendLog(QStringLiteral("Running-host rollback cancelled because the confirmation text did not match ROLLBACK."),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
        return;
    }

    bool succeeded = false;
    appendLog(QStringLiteral("Starting running-host Btrfs rollback to snapshot %1 on %2.")
                  .arg(snapshotId, m_hostPrimaryPath),
              QStringLiteral("WARNING"), LogEntryKind::Snapshot);
    const QString output = runPrivilegedRequest(
        QStringLiteral("Roll back the running host to snapshot %1").arg(snapshotId),
        {QStringLiteral("host-snapshots"), m_hostPrimaryPath, m_hostPrimaryComponentPath,
         QStringLiteral("rollback"), snapshotId},
        QByteArray(),
        &succeeded,
        true,
        LogEntryKind::Snapshot);

    const QString finished = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    m_snapshotDetails->setPlainText(QStringLiteral("Running-host rollback finished: %1\n\n%2").arg(finished, output));
    if (succeeded && output.contains(QStringLiteral("HOST_ROLLBACK_RESULT=SUCCESS"))) {
        // The running system still serves the old root; diagnostics describe
        // the old boot and are invalidated. The reboot-required state is
        // persisted and reconciled against the kernel boot id.
        invalidateAllActiveScopeDiagnostics();
        updateFullRepairSummary();
        if (m_snapshotTable) {
            m_snapshotTable->setRowCount(0);
        }
        m_snapshotResultIdentity = snapshotScopeIdentity();
        setHostRebootRequired(snapshotId);
        m_snapshotDetails->setPlainText(QStringLiteral("Running-host rollback staged: %1\n\n%2\n\nSnapshot inventory and cached running-host diagnostics were cleared because the next boot will start the promoted root. Reboot when ready; the reboot is never automatic.")
                                            .arg(finished, output));
        appendLog(QStringLiteral("STALE DIAGNOSTICS: running-host snapshot %1 rollback staged the promoted root for the next reboot; snapshot inventory and cached host diagnostics were invalidated. The running host keeps the current root until reboot.")
                      .arg(snapshotId),
                  QStringLiteral("WARNING"), LogEntryKind::Snapshot);
        updateSnapshotControls();
        updateHostRebootBanner();

        QMessageBox success(this);
        success.setIcon(QMessageBox::Information);
        success.setWindowTitle(QStringLiteral("Running-host rollback staged"));
        success.setText(QStringLiteral("Snapshot %1 is staged as the running host's next root.").arg(snapshotId));
        success.setInformativeText(QStringLiteral(
            "Reboot required — the running host will start snapshot %1 after the next reboot. The previous root was retained as an @rollback-before-* undo point and the boot stack passed reconciliation.\n\n"
            "Use Reboot Now to reboot immediately, or Later to keep working and reboot manually.").arg(snapshotId));
        QPushButton *rebootNow = success.addButton(QStringLiteral("Reboot Now"), QMessageBox::AcceptRole);
        QPushButton *later = success.addButton(QStringLiteral("Later"), QMessageBox::RejectRole);
        success.setDefaultButton(later);
        success.exec();
        if (success.clickedButton() == rebootNow) {
            confirmAndRebootHost();
        }
        return;
    }

    // Failure/partial: never reboot, keep a previously staged rollback's
    // banner state, and surface the helper's recovery evidence verbatim.
    invalidateAllActiveScopeDiagnostics();
    updateFullRepairSummary();
    if (m_snapshotTable) {
        m_snapshotTable->setRowCount(0);
    }
    m_snapshotResultIdentity = snapshotScopeIdentity();
    const bool recoveryFailed = output.contains(QStringLiteral("HOST_ROLLBACK_RECOVERY=failed"))
        || output.contains(QStringLiteral("CRITICAL"));
    m_snapshotDetails->setPlainText(QStringLiteral("Running-host rollback attempt finished: %1\n\n%2\n\nCached running-host diagnostics and snapshot inventory were cleared. Do not reboot until you review the Snapshots output and Logs.")
                                        .arg(finished, output));
    appendLog(QStringLiteral("STALE DIAGNOSTICS: running-host snapshot %1 rollback did not complete; cached host diagnostics and snapshots were invalidated. Review the operation output before the next repair or reboot.")
                  .arg(snapshotId),
              QStringLiteral("ERROR"), LogEntryKind::Snapshot);
    updateSnapshotControls();
    if (recoveryFailed) {
        QMessageBox::critical(this, QStringLiteral("Host rollback failed"),
                              QStringLiteral("The running-host rollback did not complete and automatic recovery could not be proven. DO NOT REBOOT. Review the Snapshots output and Logs, and repair the boot stack from another system before rebooting.\n\nThe helper output carries HOST_ROLLBACK_RECOVERY and any CRITICAL lines verbatim."));
    } else {
        QMessageBox::critical(this, QStringLiteral("Host rollback failed"),
                              QStringLiteral("The running-host rollback did not complete. The helper reports the preserved @ was restored automatically; do not reboot until you review the Snapshots output and Logs."));
    }
}

// ---- MainWindow: running-host reboot-required state -------------------------

QString MainWindow::currentBootId() const
{
    // Read-only test seam: BOOT_REPAIR_BOOT_ID overrides the kernel boot id so
    // the boot-id reconciliation can be exercised without rebooting.
    const QByteArray override = qgetenv("BOOT_REPAIR_BOOT_ID").trimmed();
    if (!override.isEmpty()) {
        return QString::fromUtf8(override);
    }
    QFile bootId(QStringLiteral("/proc/sys/kernel/random/boot_id"));
    if (!bootId.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(bootId.readAll()).trimmed();
}

void MainWindow::setHostRebootRequired(const QString &snapshotId)
{
    m_hostRebootRequired = true;
    m_hostRebootSnapshotId = snapshotId;
    m_hostRebootRequiredAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    m_hostRebootBootId = currentBootId();
    m_hostRebootBannerDismissed = false;
    if (m_settings) {
        m_settings->setValue(QStringLiteral("host/rebootRequired"), true);
        m_settings->setValue(QStringLiteral("host/rebootRequiredSnapshot"), m_hostRebootSnapshotId);
        m_settings->setValue(QStringLiteral("host/rebootRequiredAt"), m_hostRebootRequiredAt);
        m_settings->setValue(QStringLiteral("host/rebootRequiredBootId"), m_hostRebootBootId);
        m_settings->sync();
    }
    updateHostRebootBanner();
}

void MainWindow::clearHostRebootRequired()
{
    m_hostRebootRequired = false;
    m_hostRebootSnapshotId.clear();
    m_hostRebootRequiredAt.clear();
    m_hostRebootBootId.clear();
    m_hostRebootBannerDismissed = false;
    if (m_settings) {
        m_settings->setValue(QStringLiteral("host/rebootRequired"), false);
        m_settings->setValue(QStringLiteral("host/rebootRequiredSnapshot"), QString());
        m_settings->setValue(QStringLiteral("host/rebootRequiredAt"), QString());
        m_settings->setValue(QStringLiteral("host/rebootRequiredBootId"), QString());
        m_settings->sync();
    }
    updateHostRebootBanner();
}

// Clear the persisted reminder only when a real reboot happened: the stored
// boot id differs from the current kernel boot id. A missing stored id (for
// example a settings file from a crash) keeps the reminder and adopts the
// current boot id.
void MainWindow::reconcileHostRebootRequired()
{
    if (!m_hostRebootRequired) {
        return;
    }
    const QString current = currentBootId();
    if (!m_hostRebootBootId.isEmpty() && !current.isEmpty() && m_hostRebootBootId != current) {
        appendLog(QStringLiteral("Previous running-host rollback took effect at boot %1; the reboot reminder was cleared.").arg(current),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
        clearHostRebootRequired();
        return;
    }
    if (m_hostRebootBootId.isEmpty() && !current.isEmpty()) {
        m_hostRebootBootId = current;
        if (m_settings) {
            m_settings->setValue(QStringLiteral("host/rebootRequiredBootId"), m_hostRebootBootId);
            m_settings->sync();
        }
    }
}

void MainWindow::updateHostRebootBanner()
{
    if (!m_hostRebootBanner) {
        return;
    }
    const bool visible = m_hostRebootRequired && m_hostMaintenanceMode && !m_hostRebootBannerDismissed;
    m_hostRebootBanner->setVisible(visible);
    if (!visible) {
        return;
    }
    const QString snapshot = m_hostRebootSnapshotId.isEmpty()
        ? QStringLiteral("the rolled-back snapshot")
        : QStringLiteral("snapshot %1").arg(m_hostRebootSnapshotId);
    const QString text = QStringLiteral("Reboot required — the running host will start %1 after the next reboot. The current root is preserved as an @rollback-before-* undo point.").arg(snapshot);
    m_hostRebootBannerLabel->setText(text);
    m_hostRebootBannerLabel->setAccessibleDescription(text);
    m_hostRebootNowButton->setAccessibleDescription(QStringLiteral("Reboots the running host after a separate confirmation; all users are signed out."));
    m_hostRebootLaterButton->setAccessibleDescription(QStringLiteral("Hides the reboot reminder until Host Maintenance is re-entered; the staged rollback stays in effect."));
}

void MainWindow::confirmAndRebootHost()
{
    if (!m_hostMaintenanceMode || !m_hostRebootRequired) {
        return;
    }

    QMessageBox confirm(this);
    confirm.setIcon(QMessageBox::Warning);
    confirm.setWindowTitle(QStringLiteral("Reboot the running host now?"));
    confirm.setText(QStringLiteral("Reboot the running host now?"));
    confirm.setInformativeText(QStringLiteral(
        "The rolled-back snapshot is already staged and takes effect on the next boot. Rebooting now signs out all users and closes unsaved work.\n\n"
        "The host reboots only after this separate confirmation."));
    confirm.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    confirm.setDefaultButton(QMessageBox::Cancel);
    confirm.button(QMessageBox::Yes)->setText(QStringLiteral("Reboot Now"));
    if (confirm.exec() != QMessageBox::Yes) {
        appendLog(QStringLiteral("Running-host reboot cancelled at the second confirmation; the staged rollback remains in effect."),
                  QStringLiteral("INFO"), LogEntryKind::Snapshot);
        return;
    }

    BusyOperationScope busy(this, QStringLiteral("Rebooting running host"));
    bool succeeded = false;
    appendLog(QStringLiteral("Requesting an explicit running-host reboot after the second confirmation."),
              QStringLiteral("WARNING"), LogEntryKind::Snapshot);
    const QString output = runPrivilegedRequest(
        QStringLiteral("Reboot the running host"),
        {QStringLiteral("host-reboot"), m_hostPrimaryPath, m_hostPrimaryComponentPath},
        QByteArray(),
        &succeeded,
        true,
        LogEntryKind::Snapshot);

    if (succeeded && output.contains(QStringLiteral("HOST_REBOOT_SCHEDULED=1"))) {
        statusBar()->showMessage(QStringLiteral("Running-host reboot scheduled; the host is restarting."), 10000);
        appendLog(QStringLiteral("Running-host reboot scheduled by the privileged helper."),
                  QStringLiteral("WARNING"), LogEntryKind::Snapshot);
        return;
    }
    appendLog(QStringLiteral("Running-host reboot was not scheduled; the staged rollback and its reminder remain in effect."),
              QStringLiteral("ERROR"), LogEntryKind::Snapshot);
    QMessageBox::warning(this, QStringLiteral("Host reboot not scheduled"),
                         output.trimmed().isEmpty()
                             ? QStringLiteral("The running host did not schedule a reboot. The staged rollback and its reminder remain in effect.")
                             : output);
    updateHostRebootBanner();
}

// ---- MainWindow: File Copy --------------------------------------------------

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
            appendLog(QStringLiteral("Staged Repair → Host destination: %1. No copy occurred.").arg(path),
                      QStringLiteral("INFO"), LogEntryKind::FileCopy);
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
        // Let the list yield vertical space to the navigation and confirmation
        // rows at the compact minimum size. A fixed list minimum makes those
        // rows overlap the list frame on short displays.
        folderList->setMinimumHeight(0);
        folderList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        browserLayout->addWidget(folderList, 1);

        auto *navigationWidget = new QWidget;
        navigationWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto *navigation = new QHBoxLayout(navigationWidget);
        navigation->setContentsMargins(0, 0, 0, 0);
        navigation->setSpacing(8);
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
        browserLayout->addWidget(navigationWidget, 0);

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
                QByteArray(), &succeeded, false, LogEntryKind::FileCopy);
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
    appendLog(QStringLiteral("Staged Host → Repair destination: %1. No copy occurred.").arg(cleaned),
              QStringLiteral("INFO"), LogEntryKind::FileCopy);
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
        appendLog(QStringLiteral("Added repaired-system file path to the Repair → Host staging list: %1. No copy occurred.").arg(cleaned),
                  QStringLiteral("INFO"), LogEntryKind::FileCopy);
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
        appendLog(QStringLiteral("Added %1 host file(s) to the Host → Repair staging list. No copy occurred.").arg(paths.size()),
                  QStringLiteral("INFO"), LogEntryKind::FileCopy);
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
        appendLog(QStringLiteral("Added repaired-system folder path to the Repair → Host staging list: %1. No copy occurred.").arg(cleaned),
                  QStringLiteral("INFO"), LogEntryKind::FileCopy);
        updateFileCopyControls();
        return;
    }

    const QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("Select host folder"), QDir::homePath());
    if (!path.isEmpty() && m_sourceList->findItems(path, Qt::MatchExactly).isEmpty()) {
        auto *item = new QListWidgetItem(path, m_sourceList);
        item->setToolTip(path);
        appendLog(QStringLiteral("Added host folder to the Host → Repair staging list: %1. No copy occurred.").arg(path),
                  QStringLiteral("INFO"), LogEntryKind::FileCopy);
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

// ---- MainWindow: chroot and host shell --------------------------------------

void MainWindow::clearChrootShellOutput()
{
    if (m_chrootShellOutput) {
        m_chrootShellOutput->clear();
    }
}

bool MainWindow::shellCommandReady(QString *reason) const
{
    // The Run button and the command field's returnPressed handler share this
    // single gate so an unavailable shell can never be invoked by either path.
    return m_hostMaintenanceMode ? hostMaintenanceReady(reason) : repairTargetReady(reason);
}

void MainWindow::updateChrootShellMode()
{
    const bool hostMode = m_hostMaintenanceMode;

    if (m_chrootShellHeading) {
        m_chrootShellHeading->setText(hostMode
            ? QStringLiteral("Host shell") : QStringLiteral("Chroot shell"));
    }
    if (m_chrootShellNotice) {
        m_chrootShellNotice->setText(hostMode
            ? QStringLiteral("Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.")
            : QStringLiteral("Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as dnf update -y or apt-get -y upgrade. Output is kept in this window and in the application log."));
    }
    if (m_chrootShellCommandEdit) {
        m_chrootShellCommandEdit->setPlaceholderText(hostMode
            ? QStringLiteral("Command to run on the running host, for example: apt update")
            : QStringLiteral("Command, for example: dnf update -y or update-grub"));
        m_chrootShellCommandEdit->setAccessibleName(hostMode
            ? QStringLiteral("Host shell command") : QStringLiteral("Chroot shell command"));
    }
    if (m_chrootShellRunButton) {
        m_chrootShellRunButton->setText(hostMode
            ? QStringLiteral("Run on Host") : QStringLiteral("Run Command"));
        m_chrootShellRunButton->setToolTip(hostMode
            ? QStringLiteral("Execute a command on the running host as root.")
            : QStringLiteral("Execute the command inside the selected repair system as root."));
    }
    if (m_chrootShellWarning) {
        m_chrootShellWarning->setText(hostMode
            ? QStringLiteral("Commands can modify the running host. Review each command before running it.")
            : QStringLiteral("Commands can modify the target system. Review each command before running it."));
    }

    if (m_tabs && m_tabs->count() > ChrootShellTab) {
        // Keep the tab label consistent with the responsive layout: the full
        // label only fits at the wide breakpoint used by updateResponsiveLayout.
        const int availableWidth = centralWidget() ? centralWidget()->width() : width();
        const QString label = availableWidth < 820
            ? QStringLiteral("Shell")
            : (hostMode ? QStringLiteral("Host Shell") : QStringLiteral("Chroot Shell"));
        m_tabs->setTabText(ChrootShellTab, label);
        m_tabs->setTabToolTip(ChrootShellTab, hostMode
            ? QStringLiteral("Execute a command on the running host as root.")
            : QStringLiteral("Execute a command inside the selected repair system as root."));
    }
}

bool MainWindow::outputHasAptReleaseInfoChange(const QString &output)
{
    // The exact error class apt emits when a repository's InRelease metadata
    // changed Origin/Label/Suite/Codename/Version. Case-insensitive because
    // apt and GUI transcripts may differ in capitalization.
    static const QRegularExpression pattern(
        QStringLiteral("changed its '(?:Origin|Label|Suite|Codename|Version)' value"),
        QRegularExpression::CaseInsensitiveOption);
    return output.contains(pattern);
}

QStringList MainWindow::aptReleaseInfoChangedRepositories(const QString &output)
{
    static const QRegularExpression pattern(
        QStringLiteral("Repository\\s+'([^']*)'\\s+changed its\\s+'(?:Origin|Label|Suite|Codename|Version)'\\s+value"),
        QRegularExpression::CaseInsensitiveOption);
    QStringList repositories;
    QRegularExpressionMatchIterator iterator = pattern.globalMatch(output);
    while (iterator.hasNext()) {
        const QString repository = iterator.next().captured(1).trimmed();
        if (!repository.isEmpty() && !repositories.contains(repository)) {
            repositories.append(repository);
        }
    }
    return repositories;
}

QString MainWindow::aptReleaseInfoChangeDetails(const QString &output)
{
    static const QRegularExpression pattern(
        QStringLiteral("Repository\\s+'([^']*)'\\s+changed its\\s+'(Origin|Label|Suite|Codename|Version)'\\s+value\\s+from\\s+'([^']*)'\\s+to\\s+'([^']*)'"),
        QRegularExpression::CaseInsensitiveOption);
    QStringList details;
    QRegularExpressionMatchIterator iterator = pattern.globalMatch(output);
    while (iterator.hasNext()) {
        const QRegularExpressionMatch match = iterator.next();
        details.append(QStringLiteral("• %1: %2 changed from '%3' to '%4'")
                           .arg(match.captured(1), match.captured(2),
                                match.captured(3), match.captured(4)));
    }
    return details.join(QLatin1Char('\n'));
}

QString MainWindow::aptReleaseInfoChangeRetryCommand(const QString &command)
{
    return rewriteAptReleaseInfoChangeCommand(command, 0);
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

    const bool hostMode = m_hostMaintenanceMode;
    const QString unavailableTitle = hostMode
        ? QStringLiteral("Host shell unavailable")
        : QStringLiteral("Chroot shell unavailable");
    QString reason;
    if (!shellCommandReady(&reason)) {
        QMessageBox::warning(this, unavailableTitle, reason);
        return;
    }

    // Never dispatch a request with an incomplete identity. The readiness gate
    // already enforces this, but a future mode change must not be able to send
    // an empty disk/root to the helper (which would reject it with a truncated
    // response rather than a useful error).
    const QString diskPath = hostMode ? m_hostPrimaryPath : m_previewTargetPath;
    const QString componentPath = hostMode ? m_hostPrimaryComponentPath : m_previewTargetComponentPath;
    if (diskPath.isEmpty() || componentPath.isEmpty()) {
        const QString error = hostMode
            ? QStringLiteral("The running-host identity is unresolved; no host shell command was sent.")
            : QStringLiteral("The repair target identity is incomplete; no chroot shell command was sent.");
        appendLog(error, QStringLiteral("ERROR"),
                  hostMode ? LogEntryKind::HostShell : LogEntryKind::ChrootShell);
        QMessageBox::warning(this, unavailableTitle, error);
        return;
    }

    BusyOperationScope busy(this, hostMode
        ? QStringLiteral("Running host shell command")
        : QStringLiteral("Running chroot shell command"));

    const LogEntryKind shellKind = hostMode ? LogEntryKind::HostShell : LogEntryKind::ChrootShell;
    const QString shellTitle = hostMode ? QStringLiteral("Host shell") : QStringLiteral("Chroot shell");
    const QString helperCommand = hostMode ? QStringLiteral("host-shell") : QStringLiteral("shell");

    // One shell attempt. Both the original command and an accepted
    // release-info-change retry keep the complete pane and log transcript of
    // the existing shell flow, so no failure is silently overwritten.
    auto runAttempt = [&](const QString &attemptCommand, bool *attemptSucceeded) {
        m_chrootShellOutput->appendPlainText(QStringLiteral("$ %1").arg(attemptCommand));
        m_chrootShellOutput->appendPlainText(QStringLiteral("[running…]"));
        bool attemptOk = false;
        const QString attemptOutput = runPrivilegedRequest(
            shellTitle,
            {helperCommand, diskPath, componentPath, attemptCommand},
            QByteArray(), &attemptOk, false, shellKind);
        if (!attemptOutput.isEmpty()) {
            m_chrootShellOutput->appendPlainText(attemptOutput.trimmed());
        }
        m_chrootShellOutput->appendPlainText(attemptOk ? QStringLiteral("[exit 0]") : QStringLiteral("[command failed]"));
        const QString shellDetail = attemptOutput.trimmed().isEmpty()
            ? QStringLiteral("(no command output)")
            : attemptOutput.trimmed();
        appendLog(QStringLiteral("%1 output\nDiagnostic: %2\n%3")
                      .arg(shellTitle, attemptCommand, shellDetail),
                  attemptOk ? QStringLiteral("INFO") : QStringLiteral("ERROR"), shellKind);
        appendLog(QStringLiteral("%1 command %2: %3")
                      .arg(shellTitle,
                           attemptOk ? QStringLiteral("completed") : QStringLiteral("failed"), attemptCommand),
                  attemptOk ? QStringLiteral("INFO") : QStringLiteral("ERROR"), shellKind);
        *attemptSucceeded = attemptOk;
        return attemptOutput;
    };

    bool succeeded = false;
    QString output = runAttempt(command, &succeeded);

    // A repository release metadata change must not be a dead end. When apt
    // refused the change and the command is a confidently recognized apt-family
    // command, offer one retry with Acquire::AllowReleaseInfoChange=true; the
    // option is confined to this retry and no signature, key or package check
    // is relaxed. A declined prompt keeps the original failure untouched.
    const QString retryCommand = succeeded ? QString() : aptReleaseInfoChangeRetryCommand(command);
    if (!retryCommand.isEmpty() && outputHasAptReleaseInfoChange(output)) {
        const QStringList repositories = aptReleaseInfoChangedRepositories(output);
        const QString repositoryLabel = repositories.isEmpty()
            ? QStringLiteral("the affected repository")
            : repositories.join(QStringLiteral(", "));
        const QString details = aptReleaseInfoChangeDetails(output);

        QMessageBox confirm(this);
        confirm.setIcon(QMessageBox::Warning);
        confirm.setWindowTitle(QStringLiteral("Repository metadata changed"));
        confirm.setText(QStringLiteral("The repository changed its release metadata:\n%1")
                            .arg(repositoryLabel));
        QString informative = details;
        if (!informative.isEmpty()) {
            informative += QStringLiteral("\n\n");
        }
        informative += QStringLiteral(
            "The package manager refused the change. Allowing it re-runs the command once with "
            "Acquire::AllowReleaseInfoChange=true; the change is accepted for this retry only. "
            "Signature, key and package verification remain enforced.\n\nRetry command:\n%1")
            .arg(retryCommand);
        confirm.setInformativeText(informative);
        confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.button(QMessageBox::Yes)->setText(QStringLiteral("Allow and Retry"));
        if (confirm.exec() == QMessageBox::Yes) {
            appendLog(QStringLiteral("%1 repository metadata change accepted for %2; retrying once with Acquire::AllowReleaseInfoChange=true: %3")
                          .arg(shellTitle, repositoryLabel, retryCommand),
                      QStringLiteral("WARNING"), shellKind);
            output = runAttempt(retryCommand, &succeeded);
        } else {
            appendLog(QStringLiteral("%1 repository metadata change for %2 was not accepted; the original failure stands.")
                          .arg(shellTitle, repositoryLabel),
                      QStringLiteral("WARNING"), shellKind);
        }
    }

    if (output.contains(QString::fromLatin1(kHelperProtocolIncompleteMarker))) {
        // A rejected or truncated helper response is not a normal command
        // failure: surface it explicitly instead of silently assuming the
        // command completed.
        QMessageBox::warning(
            this,
            hostMode ? QStringLiteral("Host shell response incomplete") : QStringLiteral("Chroot shell response incomplete"),
            QStringLiteral("The privileged helper did not return a complete response for this command. The request was treated as failed; review the output and Logs before retrying."));
    }

    // Most shell commands are arbitrary and therefore conservatively stale
    // the active scope's evidence. An apt update that only reports failed
    // fetches did not receive repository data; keep the prior evidence in that
    // one explicitly recognized no-change case. The check uses the command the
    // user asked for, not the retry form with the injected option.
    const QString foldedOutput = output.toCaseFolded();
    const QString foldedCommand = command.toCaseFolded();
    const bool aptRefreshNoChange = QRegularExpression(QStringLiteral("^(sudo\\s+)?apt(-get)?\\s+update$")).match(foldedCommand).hasMatch()
        && (foldedOutput.contains(QStringLiteral("some index files failed"))
            || foldedOutput.contains(QStringLiteral("failed to fetch"))
            || foldedOutput.contains(QStringLiteral("temporary failure resolving")));
    if (aptRefreshNoChange) {
        appendLog(hostMode
                      ? QStringLiteral("Host shell apt update received no repository data; cached host diagnostics remain usable.")
                      : QStringLiteral("Chroot shell apt update received no repository data; cached diagnostics remain usable."),
                  QStringLiteral("WARNING"), shellKind);
    } else {
        // Arbitrary shell commands are deliberately broad: the invalidation
        // mapping sends the shell key to the complete diagnostic set.
        invalidateDiagnosticsForRepair({QStringLiteral("shell")}, false,
                                       hostMode
                                           ? QStringLiteral("host shell command changed running-host files")
                                           : QStringLiteral("chroot shell command changed target files"),
                                       shellKind);
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

    BusyOperationScope busy(this, QStringLiteral("Previewing file copy"));

    QStringList arguments = {QStringLiteral("copy-preview"), m_previewTargetPath, m_previewTargetComponentPath,
                             direction, ownership, QStringLiteral("normal"), m_destinationEdit->text().trimmed()};
    for (int row = 0; row < m_sourceList->count(); ++row) {
        arguments << m_sourceList->item(row)->text();
    }

    appendLog(QStringLiteral("Starting File Copy preview (%1).").arg(repairToHost
        ? QStringLiteral("Repair → Host") : QStringLiteral("Host → Repair")),
              QStringLiteral("INFO"), LogEntryKind::FileCopy);
    runRepairHelper(QStringLiteral("Preview File Copy"), arguments, LogEntryKind::FileCopy);
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

    BusyOperationScope busy(this, QStringLiteral("Copying files"));

    const QString direction = repairToHost ? QStringLiteral("repair-to-host") : QStringLiteral("host-to-repair");
    const QString ownership = m_ownershipCombo && m_ownershipCombo->currentIndex() == 1
        ? QStringLiteral("preserve") : QStringLiteral("smart");
    const QString approval = sensitive ? QStringLiteral("sensitive-ok") : QStringLiteral("normal");

    QStringList arguments = {QStringLiteral("copy"), m_previewTargetPath, m_previewTargetComponentPath,
                             direction, ownership, approval, destination};
    for (int row = 0; row < m_sourceList->count(); ++row) {
        arguments << m_sourceList->item(row)->text();
    }

    appendLog(QStringLiteral("Starting verified File Copy (%1) to %2.").arg(directionLabel, destination),
              QStringLiteral("INFO"), LogEntryKind::FileCopy);
    runRepairHelper(QStringLiteral("File Copy — %1").arg(directionLabel), arguments, LogEntryKind::FileCopy);
}

// ---- MainWindow: host capabilities and diagnostics --------------------------

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

    // Populate with sorting disabled so a mid-fill sort can never reorder or
    // lose rows, then re-enable and re-apply the active (or default) sort.
    m_capabilityTable->setSortingEnabled(false);
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
    m_capabilityTable->setSortingEnabled(true);
    applyActiveSort(m_capabilityTable, 0, Qt::AscendingOrder);

    QTimer::singleShot(0, this, [this] { resizeCapabilityRows(); });

    appendStatusLog(statusEntryIdentity(QStringLiteral("capability-scan")),
                    QStringLiteral("Host capability scan refreshed. Missing optional tools disable only the related future feature."));
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

bool MainWindow::diagnosticHostScope() const
{
    // The Systems page is the single scope authority: entering Host
    // Maintenance switches diagnostics to the protected running host, and
    // committing a repair target (which exits maintenance) switches them back.
    return m_hostMaintenanceMode;
}

bool MainWindow::hostDiagnosticScopeAllowed(QString *reason) const
{
    if (m_hostMaintenanceMode) {
        return true;
    }
    if (reason) {
        *reason = QStringLiteral("Running Host diagnostics require Host Maintenance. Choose Enter Host Maintenance on the protected running-host card in Systems first.");
    }
    return false;
}

void MainWindow::updateDiagnosticDetails()
{
    // The standard scope line is refreshed by updateTargetLabels() on every
    // Systems transition. The target configuration controls are only
    // meaningful for a committed repair target, never for the protected
    // running host, so they follow the derived scope here as well.
    const bool showTargetConfig = !diagnosticHostScope() && !m_previewTargetPath.isEmpty();
    if (m_targetConfigLabel) m_targetConfigLabel->setVisible(showTargetConfig);
    if (m_targetConfigCombo) m_targetConfigCombo->setVisible(showTargetConfig);
    if (m_editTargetConfigButton) m_editTargetConfigButton->setVisible(showTargetConfig);

    if (!m_diagnosticList || !m_diagnosticTitle || !m_diagnosticDescription
        || !m_diagnosticAvailability || !m_runDiagnosticButton || !m_diagnosticScopeLabel) {
        return;
    }

    // The availability label only carries the short state text; the longer
    // scope/freshness explanation from a previous selection is exposed on
    // hover and must not linger once the selection changes.
    m_diagnosticAvailability->setToolTip(QString());

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

    // The scope is derived from the Systems state, so target and host cached
    // results are structurally separated: host evidence is only ever read from
    // m_hostDiagnosticCache while Host Maintenance is active, and the target
    // cache is the only source when maintenance is off.
    const bool targetScope = !diagnosticHostScope();
    const QString targetIdentity = currentTargetDiagnosticCacheIdentity();
    if (targetScope && m_targetDiagnosticCacheIdentity != targetIdentity) {
        // A changed target identity invalidates the cached evidence and every
        // scoped invalidation marker; the complete set will be regenerated.
        clearTargetDiagnosticCache();
        // A changed filesystem identity invalidates repair evidence as well
        // as the Results pane; keep every repair control in sync immediately.
        updateFullRepairSummary();
        scheduleEvidenceRefresh(QStringLiteral("target diagnostic identity changed"));
    }

    const QMap<QString, QString> &cache = targetScope ? m_targetDiagnosticCache : m_hostDiagnosticCache;
    const QMap<QString, QDateTime> &times = targetScope ? m_targetDiagnosticTimes : m_hostDiagnosticTimes;
    const QString cached = cache.value(key);
    const QDateTime cachedAt = times.value(key);
    const bool sectionInvalidated = targetScope
        ? (m_targetDiagnosticsNeedRegeneration || m_targetDiagnosticsStaleSections.contains(key))
        : m_hostDiagnosticsStaleSections.contains(key);
    if (m_diagnosticResults) {
        m_diagnosticResults->setPlainText(cached.isEmpty() && sectionInvalidated
            ? QStringLiteral("The selected system changed after a repair action. Please re-run the %1 diagnostic before reviewing or running this repair action.")
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
        m_diagnosticAvailability->setText(QStringLiteral("Cached: %1 result").arg(stamp));
        // Keep the scope and freshness context the compact label drops.
        m_diagnosticAvailability->setToolTip(targetScope
            ? QStringLiteral("Cached: %1 — read-only selected repair-system result. Selecting another diagnostic and returning here keeps this output; use Re-run only for fresh data.").arg(stamp)
            : QStringLiteral("Cached: %1 — protected running-host result.").arg(stamp));
        m_runDiagnosticButton->setText(QStringLiteral("Re-run Diagnostic"));
    } else {
        if (targetScope) {
            m_diagnosticAvailability->setText(sectionInvalidated
                ? QStringLiteral("Please re-run this diagnostic after the last repair or target configuration change.")
                : (m_privilegedSessionReady
                    ? QStringLiteral("Available — read-only selected repair-system inspection; administrator authorization is already active for this Boot Bitch window.")
                    : QStringLiteral("Available — read-only selected repair-system inspection; the first root action will request administrator authorization.")));
        } else {
            m_diagnosticAvailability->setText(sectionInvalidated
                ? QStringLiteral("Please re-run this diagnostic after the last repair action.")
                : QStringLiteral("Available — protected running host"));
        }
        m_runDiagnosticButton->setText(QStringLiteral("Run Diagnostic"));
    }
    m_runDiagnosticButton->setEnabled(true);
}

// Builds the unprivileged preview text for one diagnostic. The privileged
// helper produces the authoritative captured output; this in-process formatter
// feeds the UI-test seam and the sections that are safe to render locally.
QString MainWindow::diagnosticResultForKey(const QString &key) const
{
    const bool targetScope = !diagnosticHostScope();
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
    } else if (key == QStringLiteral("backend")) {
        stream << (targetScope
            ? QStringLiteral("Selected repair-system backend profiling is collected by the privileged read-only helper.\n")
            : QStringLiteral("Running-host backend profiling is collected by the privileged read-only helper.\n"));
        stream << "The profile reports distribution family, package manager, initramfs generator, bootloader, ESP location, kernel layout, and whether modifying actions are currently enabled.\n";
        stream << "Arch-family package, initramfs, GRUB and conventional EFI repairs require a transaction-specific preflight; unsupported stages remain gated.\n";
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
            const QStringList managers = {QStringLiteral("sddm"), QStringLiteral("gdm"), QStringLiteral("gdm3"), QStringLiteral("lightdm"), QStringLiteral("ly"), QStringLiteral("greetd")};
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
    } else if (key == QStringLiteral("filesystem")) {
        stream << "Read-only file system check for "
               << (targetScope ? QStringLiteral("the selected repair system")
                               : QStringLiteral("the running host"))
               << ".\n";
        stream << "Detected component: " << preferredPath << '\n';
        stream << "The privileged fs-inspect backend checks the scope's root, /boot, ESP and /home filesystems read-only and reports each device's check tool and result.\n";
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
            QStringLiteral("environment"), QStringLiteral("backend"), QStringLiteral("boot"), QStringLiteral("kernel"),
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

QString MainWindow::diagnosticRequestTitle(bool hostScope, const QString &key) const
{
    if (hostScope) {
        return key == QStringLiteral("all") || key == QStringLiteral("report")
            ? QStringLiteral("Run running-host diagnostics")
            : QStringLiteral("Read-only running-host diagnostic");
    }
    return key == QStringLiteral("all")
        ? QStringLiteral("Run All target diagnostics")
        : QStringLiteral("Read-only target diagnostic");
}

QString MainWindow::diagnosticStartStatusIdentity(bool hostScope, const QString &key) const
{
    // The start line names the scope, disk, root and key, so all of them are
    // part of the identity.
    return statusEntryIdentity(
        QStringLiteral("diagnostic-start"),
        QStringLiteral("%1|%2|%3|%4")
            .arg(hostScope ? QStringLiteral("running-host") : QStringLiteral("target"),
                 hostScope ? m_hostPrimaryPath : m_previewTargetPath,
                 hostScope ? m_hostPrimaryComponentPath : m_previewTargetComponentPath,
                 key));
}

QString MainWindow::diagnosticCompletionStatusIdentity(bool hostScope, const QString &key) const
{
    // The completion line names the scope word and key only; the disk/root is
    // intentionally not part of the text, so it is not part of the identity.
    return statusEntryIdentity(
        QStringLiteral("diagnostic-complete"),
        QStringLiteral("%1|%2")
            .arg(hostScope ? QStringLiteral("running-host") : QStringLiteral("target"), key));
}

QString MainWindow::diagnosticRequestStatusIdentity(bool hostScope, const QString &key) const
{
    // The request completion line names only the request title.
    return statusEntryIdentity(QStringLiteral("diagnostic-request"),
                               diagnosticRequestTitle(hostScope, key));
}

void MainWindow::appendDiagnosticRunStartStatus(bool hostScope, const QString &key)
{
    const QString scopeWord = hostScope ? QStringLiteral("running-host") : QStringLiteral("target");
    appendStatusLog(diagnosticStartStatusIdentity(hostScope, key),
                    QStringLiteral("Starting read-only %1 diagnostic '%2' on %3 (%4).")
                        .arg(scopeWord, key,
                             hostScope ? m_hostPrimaryPath : m_previewTargetPath,
                             hostScope ? m_hostPrimaryComponentPath : m_previewTargetComponentPath),
                    QStringLiteral("INFO"), LogEntryKind::Diagnostic);
}

void MainWindow::appendDiagnosticRunCompletionStatus(bool hostScope, const QString &key, bool succeeded)
{
    const QString scopeWord = hostScope ? QStringLiteral("running-host") : QStringLiteral("target");
    appendStatusLog(diagnosticCompletionStatusIdentity(hostScope, key),
                    QStringLiteral("Read-only %1 diagnostic '%2' %3.")
                        .arg(scopeWord, key,
                             succeeded ? QStringLiteral("completed") : QStringLiteral("failed")),
                    succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"),
                    LogEntryKind::Diagnostic);
}

QString MainWindow::runTargetDiagnosticHelper(const QString &key, bool *succeeded,
                                                bool showProgressDialog)
{
    if (succeeded) {
        *succeeded = false;
    }

    // The dedicated "File systems" diagnostic is backed by the read-only
    // fs-inspect helper command instead of the diagnose protocol. It shares
    // the diagnostic status/identity contract and is compiled into both the
    // production and the offscreen builds so the regression suite can capture
    // the real request shape.
    if (key == QStringLiteral("filesystem")) {
        return runFilesystemDiagnostic(false, succeeded, showProgressDialog);
    }

#ifdef BOOT_REPAIR_UI_TEST
    Q_UNUSED(showProgressDialog);
    // The production target diagnostic path uses the privileged helper so it
    // can mount the selected system read-only. A headless UI test cannot
    // complete a Polkit conversation. Use the same in-process read-only
    // formatter that the Running Host UI-test path uses so automatic and
    // manual Run All remain fully exercisable offscreen. This branch is
    // compiled only into boot-repair-ui-tests.
    const QString diagnosticKey = key == QStringLiteral("all")
        ? QStringLiteral("report")
        : key;
    appendDiagnosticRunStartStatus(false, key);
    // Mirror the production request's completion line so the headless suite
    // observes the same stable status entries the privileged path writes.
    appendStatusLog(diagnosticRequestStatusIdentity(false, key),
                    QStringLiteral("%1 finished with exit code 0 (success).")
                        .arg(diagnosticRequestTitle(false, key)),
                    QStringLiteral("INFO"), LogEntryKind::Diagnostic);
    const QString captured = m_uiTestDiagnosticResults.contains(key)
        ? m_uiTestDiagnosticResults.value(key)
        : diagnosticResultForKey(diagnosticKey);
    if (succeeded) {
        *succeeded = !captured.trimmed().isEmpty();
    }
    appendDiagnosticRunCompletionStatus(false, key, succeeded && *succeeded);
    return captured;
#else
    QString reason;
    if (!repairTargetReady(&reason)) {
        if (!m_evidenceRefreshInProgress) {
            QMessageBox::warning(this, QStringLiteral("Target diagnostic unavailable"), reason);
        }
        return QStringLiteral("ERROR: %1\n").arg(reason);
    }

    const QString requestIdentity = currentTargetDiagnosticCacheIdentity();
    appendDiagnosticRunStartStatus(false, key);
    const QString captured = runPrivilegedRequest(
        diagnosticRequestTitle(false, key),
        {QStringLiteral("diagnose"), m_previewTargetPath, m_previewTargetComponentPath, key},
        QByteArray(),
        succeeded,
        showProgressDialog,
        LogEntryKind::Diagnostic,
        diagnosticRequestStatusIdentity(false, key));

    // A scope change while the request was in flight makes this result belong
    // to a target that is no longer selected. Never cache one target's
    // diagnostics as another's; the deferred regeneration serves the new
    // target.
    if (requestIdentity != currentTargetDiagnosticCacheIdentity()) {
        appendLog(QStringLiteral("Discarded the read-only target diagnostic '%1' because the active repair target changed while the request was running.")
                      .arg(key),
                  QStringLiteral("WARNING"), LogEntryKind::Diagnostic);
        if (succeeded) {
            *succeeded = false;
        }
        appendDiagnosticRunCompletionStatus(false, key, false);
        return captured;
    }

    appendDiagnosticRunCompletionStatus(false, key, succeeded && *succeeded);
    return captured;
#endif
}

QString MainWindow::runHostDiagnosticHelper(const QString &key, bool *succeeded,
                                             bool showProgressDialog)
{
    if (succeeded) {
        *succeeded = false;
    }

    // See runTargetDiagnosticHelper: the File systems diagnostic uses the
    // read-only host-fs-inspect command in both builds.
    if (key == QStringLiteral("filesystem")) {
        return runFilesystemDiagnostic(true, succeeded, showProgressDialog);
    }

#ifdef BOOT_REPAIR_UI_TEST
    Q_UNUSED(showProgressDialog);
    // The production host diagnostic path intentionally goes through the
    // privileged helper so it can inspect protected mounts, EFI variables,
    // journals and package state.  A headless UI test cannot complete a
    // Polkit conversation, however.  Keep the test focused on the scope,
    // cache, clipboard and action-register behavior by using the same
    // read-only in-process formatter that supplies the unprivileged preview
    // data.  This branch is compiled only into boot-repair-ui-tests.
    const QString diagnosticKey = key == QStringLiteral("all")
        ? QStringLiteral("report")
        : key;
    appendDiagnosticRunStartStatus(true, key);
    // Mirror the production request's completion line so the headless suite
    // observes the same stable status entries the privileged path writes.
    appendStatusLog(diagnosticRequestStatusIdentity(true, key),
                    QStringLiteral("%1 finished with exit code 0 (success).")
                        .arg(diagnosticRequestTitle(true, key)),
                    QStringLiteral("INFO"), LogEntryKind::Diagnostic);
    const QString captured = m_uiTestDiagnosticResults.contains(key)
        ? m_uiTestDiagnosticResults.value(key)
        : diagnosticResultForKey(diagnosticKey);
    if (succeeded) {
        *succeeded = !captured.trimmed().isEmpty();
    }
    appendDiagnosticRunCompletionStatus(true, key, succeeded && *succeeded);
    return captured;
#else

    QString reason;
    if (!hostBootTargetReady(&reason)) {
        if (!m_evidenceRefreshInProgress) {
            QMessageBox::warning(this, QStringLiteral("Host diagnostic unavailable"), reason);
        }
        return QStringLiteral("ERROR: %1\n").arg(reason);
    }

    appendDiagnosticRunStartStatus(true, key);
    const QString captured = runPrivilegedRequest(
        diagnosticRequestTitle(true, key),
        {QStringLiteral("host-diagnose"), m_hostPrimaryPath, m_hostPrimaryComponentPath, key},
        QByteArray(), succeeded, showProgressDialog, LogEntryKind::Diagnostic,
        diagnosticRequestStatusIdentity(true, key));

    appendDiagnosticRunCompletionStatus(true, key, succeeded && *succeeded);
    return captured;
#endif
}

// Read-only "File systems" diagnostic: runs the same fs-inspect /
// host-fs-inspect helper command the File system repair flow uses as its
// mandatory preflight, without any repair section or confirmation. The helper
// output ("File system check <device>: ...") is the diagnostic evidence; the
// caller caches it under the `filesystem` section key exactly like every other
// diagnostic.
QString MainWindow::runFilesystemDiagnostic(bool hostScope, bool *succeeded,
                                            bool showProgressDialog)
{
    if (succeeded) {
        *succeeded = false;
    }

    QString reason;
    const bool ready = hostScope ? hostMaintenanceReady(&reason) : repairTargetReady(&reason);
    if (!ready) {
        if (!m_evidenceRefreshInProgress) {
            QMessageBox::warning(this,
                                 hostScope ? QStringLiteral("Host diagnostic unavailable")
                                           : QStringLiteral("Target diagnostic unavailable"),
                                 reason);
        }
        return QStringLiteral("ERROR: %1\n").arg(reason);
    }

    const QString key = QStringLiteral("filesystem");
    const QString diskPath = hostScope ? m_hostPrimaryPath : m_previewTargetPath;
    const QString componentPath = hostScope ? m_hostPrimaryComponentPath : m_previewTargetComponentPath;
    appendDiagnosticRunStartStatus(hostScope, key);
    const QString captured = runPrivilegedRequest(
        diagnosticRequestTitle(hostScope, key),
        {hostScope ? QStringLiteral("host-fs-inspect") : QStringLiteral("fs-inspect"),
         diskPath, componentPath},
        QByteArray(), succeeded, showProgressDialog, LogEntryKind::Diagnostic,
        diagnosticRequestStatusIdentity(hostScope, key));
    appendDiagnosticRunCompletionStatus(hostScope, key, succeeded && *succeeded);
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
    m_targetDiagnosticsStaleSections.clear();
    m_targetDiagnosticCacheIdentity = currentTargetDiagnosticCacheIdentity();
}

// Splits the helper's shared capability preamble out of one individual
// diagnostic result. The preamble starts with the stable
// "Repair capability probes (read-only..." header and is a contiguous block of
// `Repair ...` lines plus the running-host `Host snapshot rollback:` /
// `Host reboot:` evidence lines; everything after it is the diagnostic's own
// output. The preamble is never dropped: callers cache it under the dedicated
// capability key so MainWindow::repairToolAvailable() keeps its single source
// of truth and the host-scope accessors can fail closed.
void MainWindow::splitDiagnosticCapabilityPreamble(const QString &captured,
                                                   QString *body,
                                                   QString *preamble)
{
    if (body) {
        *body = captured;
    }
    if (preamble) {
        preamble->clear();
    }
    const QString marker = QStringLiteral("Repair capability probes (read-only");
    if (!captured.contains(marker)) {
        return;
    }

    QStringList lines = captured.split(QLatin1Char('\n'));
    int start = -1;
    for (int index = 0; index < lines.size(); ++index) {
        if (lines.at(index).trimmed().startsWith(marker)) {
            start = index;
            break;
        }
    }
    if (start < 0) {
        return;
    }

    // The preamble is the contiguous run of `Repair ...` lines (plus the
    // running-host `Host ...` evidence lines) that starts at the header and
    // ends at the first blank or unrelated line.
    int end = start;
    while (end < lines.size()) {
        const QString line = lines.at(end).trimmed();
        if (line.isEmpty()
            || (!line.startsWith(QStringLiteral("Repair ")) && !line.startsWith(QStringLiteral("Host ")))) {
            break;
        }
        ++end;
    }
    if (preamble) {
        *preamble = lines.mid(start, end - start).join(QLatin1Char('\n')).trimmed();
    }

    lines.remove(start, end - start);
    // Drop the blank separator line that framed the preamble so the
    // diagnostic body keeps its original single spacing.
    if (start < lines.size() && lines.at(start).trimmed().isEmpty()) {
        lines.removeAt(start);
    }
    if (body) {
        *body = lines.join(QLatin1Char('\n'));
    }
}

// Shared bundle parsing for the host and target diagnostic caches: stores the
// full report and splits out each per-diagnostic section around the stable
// divider/marker format. Callers clear their scope cache before calling this.
static void cacheDiagnosticBundle(QMap<QString, QString> &cache,
                                  QMap<QString, QDateTime> &times,
                                  const QString &bundle)
{
    const QDateTime capturedAt = QDateTime::currentDateTime();
    cache.insert(QStringLiteral("report"), bundle.trimmed());
    times.insert(QStringLiteral("report"), capturedAt);

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
            cache.insert(key, section);
            times.insert(key, capturedAt);
        }
    }
}

void MainWindow::cacheHostDiagnosticBundle(const QString &bundle)
{
    clearHostDiagnosticCache();
    cacheDiagnosticBundle(m_hostDiagnosticCache, m_hostDiagnosticTimes, bundle);
}

bool MainWindow::currentDiagnosticScopeReady(QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    const bool targetScope = !diagnosticHostScope();
    if (targetScope) {
        if (m_previewTargetPath.isEmpty() || m_previewTargetComponentPath.isEmpty()
            || !m_deviceIndex.contains(m_previewTargetPath)
            || !m_deviceIndex.contains(m_previewTargetComponentPath)) {
            setReason(QStringLiteral("No repair target is ready for diagnostics."));
            return false;
        }
        setReason(QStringLiteral("The selected repair target is ready for diagnostics."));
        return true;
    }

    if (!m_hostMaintenanceMode) {
        setReason(QStringLiteral("Running Host diagnostics require Host Maintenance. Choose Host Maintenance on the protected running-host card in Systems first."));
        return false;
    }
    if (m_hostPrimaryPath.isEmpty() || m_hostPrimaryComponentPath.isEmpty()
        || !m_deviceIndex.contains(m_hostPrimaryPath)
        || !m_deviceIndex.contains(m_hostPrimaryComponentPath)) {
        setReason(QStringLiteral("The running host is not resolved for diagnostics."));
        return false;
    }
    setReason(QStringLiteral("The running host is ready for diagnostics."));
    return true;
}

bool MainWindow::currentScopeEvidenceStale(QString *reason) const
{
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    const bool targetScope = !diagnosticHostScope();
    if (targetScope) {
        if (m_targetDiagnosticsNeedRegeneration) {
            setReason(QStringLiteral("Target diagnostics were invalidated by a repair or target change."));
            return true;
        }
        if (!m_targetDiagnosticsStaleSections.isEmpty()) {
            setReason(QStringLiteral("Target diagnostic sections were invalidated by a repair action and await regeneration."));
            return true;
        }
        if (m_targetDiagnosticCacheIdentity != currentTargetDiagnosticCacheIdentity()) {
            setReason(QStringLiteral("No cached diagnostics belong to the selected target."));
            return true;
        }
        if (m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty()) {
            setReason(QStringLiteral("No cached diagnostics exist for the selected target."));
            return true;
        }
        return false;
    }

    if (!m_hostDiagnosticsStaleSections.isEmpty()) {
        setReason(QStringLiteral("Running-host diagnostic sections were invalidated by a repair action and await regeneration."));
        return true;
    }
    if (m_hostDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty()) {
        setReason(QStringLiteral("No cached running-host diagnostics exist."));
        return true;
    }
    return false;
}

bool MainWindow::shouldScheduleEvidenceRefresh(QString *reason) const
{
    auto reject = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
        return false;
    };

    if (!m_autoRefreshDiagnostics || !m_autoRefreshDiagnostics->isChecked()) {
        return reject(QStringLiteral("Automatic diagnostics regeneration is disabled in Settings."));
    }
    if (m_evidenceRefreshInProgress || m_diagnosticsRunInProgress || m_privilegedOperationActive) {
        return reject(QStringLiteral("A diagnostic or repair operation is already in progress."));
    }

    QString scopeReason;
    if (!currentDiagnosticScopeReady(&scopeReason)) {
        return reject(scopeReason);
    }

    QString evidenceReason;
    if (!currentScopeEvidenceStale(&evidenceReason)) {
        return reject(QStringLiteral("Current-scope diagnostic evidence is fresh."));
    }
    if (reason) {
        *reason = evidenceReason;
    }
    return true;
}

bool MainWindow::repairToolAvailable(const QString &key, QString *reason) const
{
    auto reject = [reason](const QString &text) {
        if (reason) *reason = text;
        return false;
    };
    const QStringList keys = {
        QStringLiteral("validate"), QStringLiteral("filesystem"), QStringLiteral("dpkg"),
        QStringLiteral("fixbroken"), QStringLiteral("aptupdate"), QStringLiteral("upgrade"),
        QStringLiteral("dkms"), QStringLiteral("display"), QStringLiteral("initramfs"),
        QStringLiteral("efi"), QStringLiteral("grub"), QStringLiteral("extlinux"),
        QStringLiteral("bootstack")
    };
    if (!keys.contains(key)) {
        return reject(QStringLiteral("Unknown repair tool: %1.").arg(key));
    }
    if (!m_hostMaintenanceMode) {
        if (m_previewTargetPath.isEmpty() || m_previewTargetComponentPath.isEmpty()) {
            return reject(QStringLiteral("Select a repair target in Systems first."));
        }
        if (m_targetDiagnosticCacheIdentity != currentTargetDiagnosticCacheIdentity()) {
            return reject(QStringLiteral("Diagnostic evidence belongs to a different target. Run diagnostics for the selected target."));
        }
    }
    const auto &cache = m_hostMaintenanceMode ? m_hostDiagnosticCache : m_targetDiagnosticCache;
    bool hadEvidence = false;
    if (!cachedRepairToolAvailable(cache, key, reason, &hadEvidence)) {
        if (!hadEvidence && !m_hostMaintenanceMode
            && (m_targetDiagnosticsNeedRegeneration || !m_targetDiagnosticsStaleSections.isEmpty())) {
            return reject(QStringLiteral("Target state changed after the last diagnostic or repair action. Please regenerate diagnostics before starting another repair."));
        }
        return false;
    }
    if (reason) *reason = QStringLiteral("Repair tool %1 is available in the selected scope's cached diagnostics.").arg(key);
    return true;
}

bool MainWindow::hostDefaultBootReady(QString *reason) const
{
    // The host default entry is a host-scope action: it consumes the cached
    // running-host diagnostics and must not be offered outside the explicit
    // host-maintenance scope.
    if (!hostMaintenanceReady(reason)) {
        return false;
    }
    // The cached running-host diagnostics are the single source of truth for
    // the host default boot entry: the EFI/UKI repair capability must be
    // available before the operation is offered.
    bool hadEvidence = false;
    if (!cachedRepairToolAvailable(m_hostDiagnosticCache, QStringLiteral("efi"), reason, &hadEvidence)) {
        if (!hadEvidence && reason) {
            *reason = QStringLiteral("Run read-only running-host diagnostics before restoring the host default boot entry.");
        }
        return false;
    }
    if (reason) {
        *reason = QStringLiteral("The running host is ready to restore its default EFI boot entry.");
    }
    return true;
}

void MainWindow::updateHostDefaultButtonState()
{
    if (!m_hostDefaultButton) {
        return;
    }
    QString reason;
    const bool ready = hostDefaultBootReady(&reason);
    m_hostDefaultButton->setEnabled(ready);
    m_hostDefaultButton->setToolTip(ready
        ? QStringLiteral("Restore and select the running host's default EFI boot entry.")
        : reason);
}

bool MainWindow::repairEvidenceReadyForTool(const QString &toolKey, QString *reason) const
{
    if (!repairToolAvailable(toolKey, reason)) {
        return false;
    }
    auto setReason = [reason](const QString &text) {
        if (reason) {
            *reason = text;
        }
    };

    QString diagnosticKey = QStringLiteral("report");
    if (toolKey == QStringLiteral("validate")) diagnosticKey = QStringLiteral("environment");
    else if (toolKey == QStringLiteral("grub")) diagnosticKey = QStringLiteral("grub");
    // The Alpine extlinux capability is proven by the boot-chain diagnostic.
    else if (toolKey == QStringLiteral("extlinux")) diagnosticKey = QStringLiteral("boot");
    else if (toolKey == QStringLiteral("initramfs")) diagnosticKey = QStringLiteral("kernel");
    else if (toolKey == QStringLiteral("efi")) diagnosticKey = QStringLiteral("uki");
    else if (toolKey == QStringLiteral("display")) diagnosticKey = QStringLiteral("display");
    else if (toolKey == QStringLiteral("dkms")) diagnosticKey = QStringLiteral("kernel");
    else if (toolKey == QStringLiteral("bootstack")) diagnosticKey = QStringLiteral("report");
    else if (toolKey == QStringLiteral("dpkg") || toolKey == QStringLiteral("fixbroken")
             || toolKey == QStringLiteral("aptupdate") || toolKey == QStringLiteral("upgrade")) {
        diagnosticKey = QStringLiteral("environment");
    }

    if (m_hostMaintenanceMode) {
        QString hostReason;
        if (!hostBootTargetReady(&hostReason)) {
            setReason(hostReason);
            return false;
        }
        if (m_hostDiagnosticCache.value(diagnosticKey).trimmed().isEmpty()) {
            const QString diagnosticName = diagnosticKey == QStringLiteral("grub")
                ? QStringLiteral("GRUB configuration")
                : (diagnosticKey == QStringLiteral("boot") ? QStringLiteral("Boot diagnostics") : diagnosticKey);
            setReason(QStringLiteral("Run the %1 diagnostic for the protected running host before starting this maintenance action.").arg(diagnosticName));
            return false;
        }
        setReason(QStringLiteral("Required read-only running-host diagnostics are cached for this maintenance action."));
        return true;
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
            : (diagnosticKey == QStringLiteral("boot") ? QStringLiteral("Boot diagnostics") : diagnosticKey);
        const bool invalidated = m_targetDiagnosticsNeedRegeneration
            || m_targetDiagnosticsStaleSections.contains(diagnosticKey);
        setReason(invalidated
            ? QStringLiteral("Target state changed after the last diagnostic or repair action. Run the %1 diagnostic for this target before starting this repair.").arg(diagnosticName)
            : QStringLiteral("Run the %1 diagnostic for this target before starting this repair.").arg(diagnosticName));
        return false;
    }
    setReason(QStringLiteral("Required read-only diagnostic evidence is cached for this repair tool."));
    return true;
}

QString MainWindow::diagnosticSectionTitle(const QString &key) const
{
    for (const DiagnosticSpec &spec : diagnosticSpecs) {
        if (key == QLatin1String(spec.key)) {
            return QString::fromLatin1(spec.title);
        }
    }
    return key;
}

QStringList MainWindow::diagnosticSectionsForRepair(const QString &toolKey) const
{
    for (const RepairDiagnosticSectionSpec &spec : repairDiagnosticSectionSpecs) {
        if (toolKey == QLatin1String(spec.toolKey)) {
            return QString::fromLatin1(spec.sections).split(QLatin1Char(' '), Qt::SkipEmptyParts);
        }
    }
    // Unknown or deliberately broad actions (file copy, shell commands,
    // snapshot rollback, unlock, config writes, target/scope changes, the Full
    // Repair plan key and any future tool) invalidate the complete set.
    return {QStringLiteral("all")};
}

// The helper stage names and the capability/UI keys differ only for the
// package, display and boot-stack stages. Keeping one normalization point
// guarantees the change-status protocol is matched against the same key the
// invalidation table uses.
QString MainWindow::repairToolKeyForStage(const QString &stage)
{
    static const QMap<QString, QString> stageToTool = {
        {QStringLiteral("dpkg-configure"), QStringLiteral("dpkg")},
        {QStringLiteral("fix-broken"), QStringLiteral("fixbroken")},
        {QStringLiteral("apt-update"), QStringLiteral("aptupdate")},
        {QStringLiteral("apt-upgrade"), QStringLiteral("upgrade")},
        {QStringLiteral("display-manager"), QStringLiteral("display")},
        {QStringLiteral("boot-stack"), QStringLiteral("bootstack")}
    };
    return stageToTool.value(stage, stage);
}

// Reads every stable change-status line from a helper transcript. A later line
// for the same key does not override an earlier "changed": any evidence of a
// write keeps the key invalidating (fail safe).
QMap<QString, QString> MainWindow::parseRepairChangeStatuses(const QString &output)
{
    static const QRegularExpression statusRe(
        QStringLiteral("^Repair change status ([A-Za-z0-9._-]+): (.+)$"));
    QMap<QString, QString> statuses;
    const QStringList lines = output.split(QLatin1Char('\n'));
    for (const QString &rawLine : lines) {
        const QRegularExpressionMatch match = statusRe.match(rawLine.trimmed());
        if (!match.hasMatch()) {
            continue;
        }
        const QString key = match.captured(1);
        const QString state = match.captured(2).trimmed();
        const auto existing = statuses.constFind(key);
        if (existing != statuses.constEnd()
            && !repairChangeStatusIsUnchanged(existing.value())
            && repairChangeStatusIsUnchanged(state)) {
            continue;
        }
        statuses.insert(key, state);
    }
    return statuses;
}

bool MainWindow::repairChangeStatusIsUnchanged(const QString &status)
{
    return status == QStringLiteral("unchanged")
        || status.startsWith(QStringLiteral("unchanged|"));
}

QStringList MainWindow::invalidatingRepairToolKeys(const QStringList &requestedKeys,
                                                   const QString &output)
{
    const QMap<QString, QString> statuses = parseRepairChangeStatuses(output);
    QStringList keys;
    for (const QString &requested : requestedKeys) {
        const QString toolKey = repairToolKeyForStage(requested);
        const auto status = statuses.constFind(toolKey);
        // A requested action without a proven-unchanged status invalidates its
        // mapped sections (fail safe).
        if (status != statuses.constEnd() && repairChangeStatusIsUnchanged(status.value())) {
            continue;
        }
        if (!keys.contains(toolKey)) {
            keys.append(toolKey);
        }
    }
    for (auto it = statuses.constBegin(); it != statuses.constEnd(); ++it) {
        if (repairChangeStatusIsUnchanged(it.value()) || keys.contains(it.key())) {
            continue;
        }
        keys.append(it.key());
    }
    return keys;
}

// The single repair-result categorization. It is deliberately derived from the
// same evidence and the same fail-safe default as the cached-evidence
// invalidation decision: an empty invalidating-key list is a proven no-op (or
// a read-only preflight with nothing to do), a non-empty list means the helper
// changed the system or could not prove otherwise.
MainWindow::RepairResultCategory MainWindow::categorizeRepairResult(bool processSucceeded,
                                                                    const QStringList &requestedKeys,
                                                                    const QString &output)
{
    if (!processSucceeded) {
        return RepairResultCategory::Failed;
    }
    if (invalidatingRepairToolKeys(requestedKeys, output).isEmpty()) {
        return RepairResultCategory::NoRepairNeeded;
    }
    return RepairResultCategory::Success;
}

QString MainWindow::shortRepairFailureReason(const QString &output)
{
    const QStringList lines = output.split(QLatin1Char('\n'));
    for (auto it = lines.crbegin(); it != lines.crend(); ++it) {
        const QString line = it->trimmed();
        if (line.startsWith(QStringLiteral("ERROR:"), Qt::CaseInsensitive)) {
            return line.mid(6).trimmed();
        }
    }
    for (auto it = lines.crbegin(); it != lines.crend(); ++it) {
        const QString line = it->trimmed();
        if (line.contains(QStringLiteral("fail"), Qt::CaseInsensitive)) {
            constexpr int kMaximumReasonLength = 120;
            if (line.size() > kMaximumReasonLength) {
                return line.left(kMaximumReasonLength - 1) + QChar(0x2026);
            }
            return line;
        }
    }
    return QStringLiteral("the privileged helper reported a failure");
}

QString MainWindow::repairFailureStageKey(const QString &output)
{
    static const QRegularExpression stageRe(
        QStringLiteral("ERROR: stage '([^']+)' failed:"),
        QRegularExpression::CaseInsensitiveOption);
    const QStringList lines = output.split(QLatin1Char('\n'));
    QString stage;
    for (const QString &line : lines) {
        const QRegularExpressionMatch match = stageRe.match(line);
        if (match.hasMatch()) {
            // Keep the last named stage: the helper may retry internally and
            // only the final failure line describes where the plan stopped.
            stage = match.captured(1).trimmed();
        }
    }
    if (stage.isEmpty()) {
        return QString();
    }
    // Decorated labels ("grub (EFI follow-up)") belong to their base stage.
    const int decoration = stage.indexOf(QStringLiteral(" ("));
    if (decoration > 0) {
        stage = stage.left(decoration);
    }
    return repairToolKeyForStage(stage);
}

namespace {

// One per-tool repair-result vocabulary. Every phrase is action-accurate: a
// metadata refresh is not a repair, an initramfs regeneration is a rebuild,
// and so on. The same table drives the single-action summary and the plan
// stage lines, so the two can never drift. The empty key is the generic
// fallback for unknown or future tools.
struct RepairResultPhrases {
    QString changed;
    QString unchanged;
    QString failed;
    QString skipped;
    QString notRun;
};

RepairResultPhrases repairResultPhrases(const QString &toolKey)
{
    static const QMap<QString, RepairResultPhrases> vocabulary = {
        {QStringLiteral("aptupdate"), {
            QStringLiteral("package metadata refreshed — changes were applied"),
            QStringLiteral("package metadata already current — no changes"),
            QStringLiteral("package metadata refresh failed"),
            QStringLiteral("package metadata refresh not checked"),
            QStringLiteral("package metadata refresh did not run")}},
        {QStringLiteral("dpkg"), {
            QStringLiteral("package configuration completed — changes were applied"),
            QStringLiteral("nothing to configure"),
            QStringLiteral("package configuration failed"),
            QStringLiteral("package configuration not checked"),
            QStringLiteral("package configuration did not run")}},
        {QStringLiteral("fixbroken"), {
            QStringLiteral("broken dependencies repaired — changes were applied"),
            QStringLiteral("no broken dependencies"),
            QStringLiteral("broken dependency repair failed"),
            QStringLiteral("broken dependency repair not checked"),
            QStringLiteral("broken dependency repair did not run")}},
        {QStringLiteral("upgrade"), {
            QStringLiteral("packages upgraded — changes were applied"),
            QStringLiteral("no packages to upgrade"),
            QStringLiteral("package upgrade failed"),
            QStringLiteral("package upgrade not checked"),
            QStringLiteral("package upgrade did not run")}},
        {QStringLiteral("dkms"), {
            QStringLiteral("DKMS modules rebuilt — changes were applied"),
            QStringLiteral("DKMS modules already current — no changes"),
            QStringLiteral("DKMS rebuild failed"),
            QStringLiteral("DKMS rebuild not checked"),
            QStringLiteral("DKMS rebuild did not run")}},
        {QStringLiteral("display"), {
            QStringLiteral("display manager repaired — changes were applied"),
            QStringLiteral("display manager already correct — no changes"),
            QStringLiteral("display manager repair failed"),
            QStringLiteral("display manager repair not checked"),
            QStringLiteral("display manager repair did not run")}},
        {QStringLiteral("initramfs"), {
            QStringLiteral("initramfs rebuilt — images regenerated"),
            QStringLiteral("initramfs already current — no changes"),
            QStringLiteral("initramfs rebuild failed"),
            QStringLiteral("initramfs rebuild not checked"),
            QStringLiteral("initramfs rebuild did not run")}},
        {QStringLiteral("efi"), {
            QStringLiteral("EFI boot path repaired — changes were applied"),
            QStringLiteral("EFI boot path already correct — no changes"),
            QStringLiteral("EFI boot path repair failed"),
            QStringLiteral("EFI boot path repair not checked"),
            QStringLiteral("EFI boot path repair did not run")}},
        {QStringLiteral("grub"), {
            QStringLiteral("GRUB configuration regenerated — changes were applied"),
            QStringLiteral("GRUB configuration already current — no changes"),
            QStringLiteral("GRUB regeneration failed"),
            QStringLiteral("GRUB regeneration not checked"),
            QStringLiteral("GRUB regeneration did not run")}},
        {QStringLiteral("extlinux"), {
            QStringLiteral("extlinux configuration regenerated — changes were applied"),
            QStringLiteral("extlinux configuration already current — no changes"),
            QStringLiteral("extlinux regeneration failed"),
            QStringLiteral("extlinux regeneration not checked"),
            QStringLiteral("extlinux regeneration did not run")}},
        {QStringLiteral("filesystem"), {
            QStringLiteral("file system repaired — changes were applied"),
            QStringLiteral("no file system errors found — no changes"),
            QStringLiteral("file system repair failed"),
            QStringLiteral("not all filesystems were checked"),
            QStringLiteral("file system repair did not run")}},
        {QStringLiteral("bootstack"), {
            QStringLiteral("boot stack reconciled — changes were applied"),
            QStringLiteral("boot stack already current — no changes"),
            QStringLiteral("boot stack repair failed"),
            QStringLiteral("boot stack not checked"),
            QStringLiteral("boot stack repair did not run")}},
        {QStringLiteral("validate"), {
            QStringLiteral("validation completed — changes were applied"),
            QStringLiteral("validation is read-only — no changes"),
            QStringLiteral("validation failed"),
            QStringLiteral("validation not checked"),
            QStringLiteral("validation did not run")}},
        {QStringLiteral("host-default"), {
            QStringLiteral("host boot default updated — changes were applied"),
            QStringLiteral("host boot default already correct — no changes"),
            QStringLiteral("host boot default update failed"),
            QStringLiteral("host boot default not checked"),
            QStringLiteral("host boot default update did not run")}},
        {QString(), {
            QStringLiteral("repair successful — changes were applied"),
            QStringLiteral("no repair needed — no changes were detected"),
            QStringLiteral("repair failed"),
            QStringLiteral("not checked — no repair was attempted"),
            QStringLiteral("repair did not run")}}
    };
    return vocabulary.value(toolKey, vocabulary.value(QString()));
}

// The helper's proven-unchanged status is "unchanged|<reason>"; the reason is
// the audit evidence that explains why the stage could safely skip its write.
QString repairChangeStatusReason(const QString &status)
{
    const int separator = status.indexOf(QLatin1Char('|'));
    return separator < 0 ? QString() : status.mid(separator + 1).trimmed();
}

} // namespace

// ---- MainWindow: repair-result categorization -------------------------------

QString MainWindow::repairResultSymbol(RepairResultCategory category)
{
    switch (category) {
    case RepairResultCategory::Success:
        return QStringLiteral("✓");
    case RepairResultCategory::Failed:
        return QStringLiteral("✗");
    case RepairResultCategory::NoRepairNeeded:
    case RepairResultCategory::Skipped:
    case RepairResultCategory::NotRun:
        return QStringLiteral("▪");
    }
    return QStringLiteral("▪");
}

QString MainWindow::repairResultSummary(RepairResultCategory category, const QString &toolKey,
                                        const QString &reason, const QString &detail)
{
    const RepairResultPhrases phrases = repairResultPhrases(toolKey);
    QString phrase;
    switch (category) {
    case RepairResultCategory::Success:
        phrase = phrases.changed;
        break;
    case RepairResultCategory::Failed:
        phrase = phrases.failed;
        break;
    case RepairResultCategory::NoRepairNeeded:
        phrase = phrases.unchanged;
        break;
    case RepairResultCategory::Skipped:
        phrase = phrases.skipped;
        break;
    case RepairResultCategory::NotRun:
        phrase = phrases.notRun;
        break;
    }
    QString summary = QStringLiteral("Repair result: %1 %2")
        .arg(repairResultSymbol(category), phrase);
    if (category == RepairResultCategory::Failed) {
        summary += QStringLiteral(" — %1")
            .arg(reason.isEmpty() ? QStringLiteral("the privileged helper reported a failure") : reason);
    } else if (!detail.isEmpty()) {
        // The helper's proven-unchanged reason and the held-back/skipped
        // package-manager feedback appended to a change status stay auditable.
        summary += QStringLiteral(" — %1").arg(detail);
    }
    return summary;
}

QString MainWindow::repairResultStageLine(const RepairStageResult &stage)
{
    const QString name = stage.toolKey.isEmpty() ? QStringLiteral("unknown") : stage.toolKey;
    const RepairResultPhrases phrases = repairResultPhrases(stage.toolKey);
    QString phrase;
    switch (stage.category) {
    case RepairResultCategory::Success:
        phrase = phrases.changed;
        break;
    case RepairResultCategory::Failed:
        phrase = phrases.failed;
        break;
    case RepairResultCategory::NoRepairNeeded:
        phrase = phrases.unchanged;
        break;
    case RepairResultCategory::Skipped:
        phrase = phrases.skipped;
        break;
    case RepairResultCategory::NotRun:
        phrase = phrases.notRun;
        break;
    }
    QString line = QStringLiteral("  %1 %2 — %3")
        .arg(repairResultSymbol(stage.category), name, phrase);
    if (stage.category == RepairResultCategory::Failed && !stage.reason.isEmpty()) {
        line += QStringLiteral(" — %1").arg(stage.reason);
    }
    if (!stage.detail.isEmpty()) {
        line += QStringLiteral(" — %1").arg(stage.detail);
    }
    return line;
}

QString MainWindow::fullRepairPlanSummary(const QList<RepairStageResult> &stages)
{
    int successful = 0;
    int failed = 0;
    int noRepairNeeded = 0;
    int skipped = 0;
    int notRun = 0;
    for (const RepairStageResult &stage : stages) {
        switch (stage.category) {
        case RepairResultCategory::Success:
            ++successful;
            break;
        case RepairResultCategory::Failed:
            ++failed;
            break;
        case RepairResultCategory::NoRepairNeeded:
            ++noRepairNeeded;
            break;
        case RepairResultCategory::Skipped:
            ++skipped;
            break;
        case RepairResultCategory::NotRun:
            ++notRun;
            break;
        }
    }
    QString summary = QStringLiteral("Full Repair results: ✓ %1 successful · ✗ %2 failed · ▪ %3 no repair needed")
        .arg(successful)
        .arg(failed)
        .arg(noRepairNeeded);
    if (skipped > 0) {
        // Name the skipped stage(s) so the aggregate points at the stage line
        // that carries the affected device list; the per-stage line remains
        // the authoritative audit record and is always rendered directly
        // below the aggregate.
        QStringList skippedStages;
        for (const RepairStageResult &stage : stages) {
            if (stage.category == RepairResultCategory::Skipped) {
                skippedStages.append(stage.toolKey.isEmpty() ? QStringLiteral("unknown") : stage.toolKey);
            }
        }
        const QString stageReference = skippedStages.size() == 1
            ? QStringLiteral("see the %1 stage line").arg(skippedStages.first())
            : QStringLiteral("see the stage lines for %1").arg(skippedStages.join(QStringLiteral(", ")));
        summary += QStringLiteral(" · ▪ %1 not checked — %2").arg(skipped).arg(stageReference);
    }
    if (notRun > 0) {
        // Stages the plan never reached are reported as not run, never as
        // failed: the failing stage already owns the failure reason and these
        // stages were simply stopped by it.
        QStringList notRunStages;
        for (const RepairStageResult &stage : stages) {
            if (stage.category == RepairResultCategory::NotRun) {
                notRunStages.append(stage.toolKey.isEmpty() ? QStringLiteral("unknown") : stage.toolKey);
            }
        }
        const QString stageReference = notRunStages.size() == 1
            ? QStringLiteral("see the %1 stage line").arg(notRunStages.first())
            : QStringLiteral("see the stage lines for %1").arg(notRunStages.join(QStringLiteral(", ")));
        summary += QStringLiteral(" · ▪ %1 not run — %2").arg(notRun).arg(stageReference);
    }
    if (failed > 0) {
        summary += QStringLiteral(" — review the failed stages in Logs.");
    } else if (successful == 0 && notRun > 0) {
        summary += QStringLiteral(" — the plan stopped before any stage completed.");
    } else if (successful == 0 && skipped > 0) {
        summary += QStringLiteral(" — no changes were applied; some checks were skipped.");
    } else if (successful == 0 && noRepairNeeded > 0) {
        summary += QStringLiteral(" — no repair was needed; cached diagnostics remain valid.");
    } else {
        summary += QStringLiteral(" — changes were applied.");
    }
    QStringList lines;
    lines.reserve(stages.size() + 1);
    lines.append(summary);
    for (const RepairStageResult &stage : stages) {
        lines.append(repairResultStageLine(stage));
    }
    return lines.join(QLatin1Char('\n'));
}

void MainWindow::recordPlanStageResult(const QString &toolKey, RepairResultCategory category,
                                       const QString &reason, const QString &detail)
{
    if (!m_fullRepairPlanInProgress) {
        return;
    }
    RepairStageResult stage;
    stage.toolKey = toolKey;
    stage.category = category;
    stage.reason = reason;
    stage.detail = detail;
    m_fullRepairPlanStageResults.append(stage);
}

void MainWindow::appendRepairResultSummary(RepairResultCategory category, const QString &reason,
                                           LogEntryKind kind, const QString &toolKey,
                                           const QString &detail)
{
    appendLog(repairResultSummary(category, toolKey, reason, detail),
              category == RepairResultCategory::Failed ? QStringLiteral("ERROR") : QStringLiteral("INFO"),
              kind);
}

bool MainWindow::diagnosticsInvalidationPending() const
{
    if (m_hostMaintenanceMode) {
        return !m_hostDiagnosticsStaleSections.isEmpty();
    }
    return m_targetDiagnosticsNeedRegeneration
        || !m_targetDiagnosticsStaleSections.isEmpty();
}

void MainWindow::clearHostDiagnosticCache()
{
    m_hostDiagnosticCache.clear();
    m_hostDiagnosticTimes.clear();
    m_hostDiagnosticsStaleSections.clear();
}

void MainWindow::invalidateAllTargetDiagnostics()
{
    clearTargetDiagnosticCache();
    m_targetDiagnosticsNeedRegeneration = true;
}

void MainWindow::invalidateAllActiveScopeDiagnostics()
{
    if (m_hostMaintenanceMode) {
        clearHostDiagnosticCache();
    } else {
        invalidateAllTargetDiagnostics();
    }
}

void MainWindow::markDiagnosticSectionsStale(bool hostScope, const QStringList &sections)
{
    auto &cache = hostScope ? m_hostDiagnosticCache : m_targetDiagnosticCache;
    auto &times = hostScope ? m_hostDiagnosticTimes : m_targetDiagnosticTimes;
    auto &stale = hostScope ? m_hostDiagnosticsStaleSections : m_targetDiagnosticsStaleSections;
    for (const QString &section : orderedDiagnosticSections(sections)) {
        cache.remove(section);
        times.remove(section);
        stale.insert(section);
    }
}

void MainWindow::invalidateDiagnosticsForRepair(const QStringList &toolKeys, bool failed,
                                                const QString &reason, LogEntryKind kind)
{
    QSet<QString> sectionSet;
    bool full = failed;
    if (!full) {
        for (const QString &toolKey : toolKeys) {
            const QStringList mapped = diagnosticSectionsForRepair(toolKey);
            if (mapped.contains(QStringLiteral("all"))) {
                full = true;
                break;
            }
            for (const QString &section : mapped) {
                sectionSet.insert(section);
            }
        }
    }
    const QStringList sections = orderedDiagnosticSections(sectionSet.values());
    if (full) {
        invalidateAllActiveScopeDiagnostics();
    } else if (!sections.isEmpty()) {
        markDiagnosticSectionsStale(m_hostMaintenanceMode, sections);
    } else {
        // Read-only action: no cached evidence changed.
        return;
    }

    updateFullRepairSummary();
    updateDiagnosticDetails();
    const QString message = full
        ? (failed
            ? QStringLiteral("STALE DIAGNOSTICS: repair failed after a modifying action. Regenerate diagnostics before the next repair.")
            : QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action. Regenerate diagnostics before the next repair."))
        : QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action. Stale cached diagnostic sections: %1. They will be regenerated automatically before the next repair.")
              .arg(sections.join(QStringLiteral(", ")));
    appendStatusLog(statusEntryIdentity(failed ? QStringLiteral("stale-repair-failed")
                                               : QStringLiteral("stale-repair-modified")),
                    message, failed ? QStringLiteral("ERROR") : QStringLiteral("INFO"), kind);
    scheduleEvidenceRefresh(reason);
}

void MainWindow::accumulatePlanInvalidation(const QStringList &toolKeys, bool failed)
{
    // Every stage proved unchanged: the plan owns no invalidation and must not
    // schedule the end-of-plan regeneration.
    if (!failed && toolKeys.isEmpty()) {
        return;
    }
    m_fullRepairPlanModified = true;
    if (failed) {
        // A failed stage may have left more than its mapped sections in an
        // unknown state; fall back to the complete set (fail safe).
        m_fullRepairPlanRequiresFullInvalidation = true;
        m_fullRepairPlanInvalidatedSections.clear();
        return;
    }
    for (const QString &toolKey : toolKeys) {
        const QStringList mapped = diagnosticSectionsForRepair(toolKey);
        if (mapped.contains(QStringLiteral("all"))) {
            m_fullRepairPlanRequiresFullInvalidation = true;
            m_fullRepairPlanInvalidatedSections.clear();
            return;
        }
        for (const QString &section : mapped) {
            m_fullRepairPlanInvalidatedSections.insert(section);
        }
    }
}

void MainWindow::runDiagnosticSections(const QStringList &sections)
{
    if (!m_diagnosticResults || sections.isEmpty()) {
        return;
    }
    const bool targetScope = !diagnosticHostScope();
    if (!targetScope && !m_hostMaintenanceMode) {
        // Failsafe: automatic regeneration must never run host diagnostics
        // outside the explicit maintenance scope (shouldScheduleEvidenceRefresh
        // already refuses the request before it is queued).
        appendLog(QStringLiteral("Automatic running-host diagnostics regeneration skipped: Host Maintenance is not active."),
                  QStringLiteral("WARNING"), LogEntryKind::Diagnostic);
        return;
    }
    const QStringList ordered = orderedDiagnosticSections(sections);
    if (ordered.isEmpty()) {
        return;
    }

    BusyOperationScope busy(this, targetScope
        ? QStringLiteral("Regenerating target diagnostics")
        : QStringLiteral("Regenerating running-host diagnostics"));

    m_diagnosticsRunInProgress = true;
    if (m_runAllDiagnosticsButton) {
        m_runAllDiagnosticsButton->setText(QStringLiteral("Running…"));
    }

    for (const QString &key : ordered) {
        bool ok = false;
        const QString captured = targetScope
            ? runTargetDiagnosticHelper(key, &ok)
            : runHostDiagnosticHelper(key, &ok);
        if (!ok || captured.trimmed().isEmpty()) {
            // A failed section stays stale and keeps its repair tool gated
            // until a later regeneration succeeds.
            continue;
        }
        // Keep the individual section free of the shared capability preamble;
        // its gating lines live in the dedicated capability cache entry.
        QString body;
        QString capabilityPreamble;
        splitDiagnosticCapabilityPreamble(captured, &body, &capabilityPreamble);
        const QDateTime capturedAt = QDateTime::currentDateTime();
        if (targetScope) {
            m_targetDiagnosticCache.insert(key, body);
            m_targetDiagnosticTimes.insert(key, capturedAt);
            m_targetDiagnosticsStaleSections.remove(key);
            if (!capabilityPreamble.isEmpty()) {
                m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), capabilityPreamble);
                m_targetDiagnosticTimes.insert(QStringLiteral("capabilities"), capturedAt);
            }
        } else {
            m_hostDiagnosticCache.insert(key, body);
            m_hostDiagnosticTimes.insert(key, capturedAt);
            m_hostDiagnosticsStaleSections.remove(key);
            if (!capabilityPreamble.isEmpty()) {
                m_hostDiagnosticCache.insert(QStringLiteral("capabilities"), capabilityPreamble);
                m_hostDiagnosticTimes.insert(QStringLiteral("capabilities"), capturedAt);
            }
        }
        appendDiagnosticLog(key, diagnosticSectionTitle(key),
                            targetScope ? QStringLiteral("Repair Target") : QStringLiteral("Running Host"),
                            body, ok);
        appendLog(QStringLiteral("%1 diagnostic run: %2")
                      .arg(targetScope ? QStringLiteral("Target") : QStringLiteral("Host"),
                           diagnosticSectionTitle(key)),
                  QStringLiteral("INFO"), LogEntryKind::Diagnostic);
    }

    if (m_runAllDiagnosticsButton) {
        m_runAllDiagnosticsButton->setText(QStringLiteral("Run All"));
    }
    m_diagnosticsRunInProgress = false;
    updateDiagnosticDetails();
    updateFullRepairSummary();
    if (m_runAllDiagnosticsQueued) {
        m_runAllDiagnosticsQueued = false;
        QTimer::singleShot(0, this, &MainWindow::runAllDiagnostics);
    }
}

void MainWindow::cacheTargetDiagnosticBundle(const QString &bundle)
{
    clearTargetDiagnosticCache();
    cacheDiagnosticBundle(m_targetDiagnosticCache, m_targetDiagnosticTimes, bundle);
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

    // A single diagnostic request cannot run while another privileged request
    // owns the gate. Refuse before the cache is cleared so a click during a
    // read-only request can never wipe valid evidence or store the refusal as
    // a failed diagnostic.
    if (m_privilegedOperationActive || m_diagnosticsRunInProgress) {
        statusBar()->showMessage(QStringLiteral("A privileged operation is already running; the diagnostic was not started. Try again when it finishes."), 5000);
        return;
    }

    const QString key = item->data(Qt::UserRole).toString();
    const bool targetScope = !diagnosticHostScope();
    if (!targetScope) {
        // Host-scope diagnostics are a deliberate maintenance mode; refuse
        // before any cache is touched or a helper request is sent.
        QString hostReason;
        if (!hostDiagnosticScopeAllowed(&hostReason)) {
            appendLog(QStringLiteral("Host diagnostic refused: %1").arg(hostReason),
                      QStringLiteral("WARNING"), LogEntryKind::Diagnostic);
            statusBar()->showMessage(hostReason, 6000);
            if (!m_evidenceRefreshInProgress) {
                QMessageBox::information(this, QStringLiteral("Host Maintenance required"), hostReason);
            }
            return;
        }
    }
    // A diagnostic whose helper output does not carry the shared capability
    // preamble (the File systems fs-inspect diagnostic) must not disable every
    // repair gate: preserve the last capability evidence across the run.
    const auto capabilityEvidence = [this, targetScope]() -> QString {
        const QMap<QString, QString> &cache = targetScope ? m_targetDiagnosticCache : m_hostDiagnosticCache;
        const QString existing = cache.value(QStringLiteral("capabilities"));
        if (!existing.trimmed().isEmpty()) {
            return existing;
        }
        QString ignoredBody;
        QString preamble;
        splitDiagnosticCapabilityPreamble(cache.value(QStringLiteral("report")), &ignoredBody, &preamble);
        return preamble;
    };
    const QString preservedCapabilities = capabilityEvidence();
    // Per-section cache contract: running one diagnostic replaces only its own
    // cached section (and drops the combined report bundle, which would mix
    // generations). Every other section stays cached for the active scope until
    // an explicit invalidation (scope/target change, repair action,
    // authorization change) marks it stale; selecting another diagnostic must
    // never discard it.
    m_diagnosticResults->clear();
    m_copyDiagnosticButton->setEnabled(false);
    m_saveDiagnosticButton->setEnabled(false);

    BusyOperationScope busy(this, QStringLiteral("Running diagnostic: %1").arg(item->text()));

    bool ok = true;
    const QString result = targetScope
        ? runTargetDiagnosticHelper(key, &ok)
        : runHostDiagnosticHelper(key, &ok);

    // An individual diagnostic must stay purely its own subject. The helper
    // includes the shared capability preamble in every individual run so the
    // `Repair tool <key>` gating contract stays self-contained; split it out
    // into the dedicated capability cache entry instead of showing it inside
    // (for example) the fstab result. The combined report keeps its single
    // preamble at the top by design.
    QString diagnosticBody = result;
    QString capabilityPreamble;
    if (key != QStringLiteral("report")) {
        splitDiagnosticCapabilityPreamble(result, &diagnosticBody, &capabilityPreamble);
    }

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
            m_targetDiagnosticCache.insert(key, diagnosticBody);
            m_targetDiagnosticTimes.insert(key, capturedAt);
            if (ok && !diagnosticBody.trimmed().isEmpty()) {
                // A successful manual re-run satisfies this section's
                // invalidation; the other stale sections stay stale.
                m_targetDiagnosticsStaleSections.remove(key);
            }
            if (!capabilityPreamble.isEmpty()) {
                m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), capabilityPreamble);
                m_targetDiagnosticTimes.insert(QStringLiteral("capabilities"), capturedAt);
            } else if (!preservedCapabilities.isEmpty()) {
                m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), preservedCapabilities);
                m_targetDiagnosticTimes.insert(QStringLiteral("capabilities"), capturedAt);
            }
        }
    } else {
        if (key == QStringLiteral("report")) {
            cacheHostDiagnosticBundle(result);
        } else {
            m_hostDiagnosticCache.remove(QStringLiteral("report"));
            m_hostDiagnosticTimes.remove(QStringLiteral("report"));
            m_hostDiagnosticCache.insert(key, diagnosticBody);
            m_hostDiagnosticTimes.insert(key, capturedAt);
            if (ok && !diagnosticBody.trimmed().isEmpty()) {
                m_hostDiagnosticsStaleSections.remove(key);
            }
            if (!capabilityPreamble.isEmpty()) {
                m_hostDiagnosticCache.insert(QStringLiteral("capabilities"), capabilityPreamble);
                m_hostDiagnosticTimes.insert(QStringLiteral("capabilities"), capturedAt);
            } else if (!preservedCapabilities.isEmpty()) {
                m_hostDiagnosticCache.insert(QStringLiteral("capabilities"), preservedCapabilities);
                m_hostDiagnosticTimes.insert(QStringLiteral("capabilities"), capturedAt);
            }
        }
    }

    updateDiagnosticDetails();
    // Capability evidence can arrive with any individual diagnostic; refresh
    // the repair gates immediately instead of leaving the Settings/Full Repair
    // stages stale until the next unrelated summary update.
    updateFullRepairSummary();
    appendDiagnosticLog(key, item->text(),
                       targetScope ? QStringLiteral("Repair Target") : QStringLiteral("Running Host"),
                       diagnosticBody, ok);
    appendLog(QStringLiteral("%1 diagnostic run: %2").arg(targetScope ? QStringLiteral("Target") : QStringLiteral("Host"), item->text()),
              ok ? QStringLiteral("INFO") : QStringLiteral("ERROR"), LogEntryKind::Diagnostic);

    // An individual run can defer a queued manual Run All.
    if (m_runAllDiagnosticsQueued) {
        m_runAllDiagnosticsQueued = false;
        QTimer::singleShot(0, this, &MainWindow::runAllDiagnostics);
    }
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
    // A target configuration write can change mount, initramfs and boot
    // behavior, so the complete target diagnostic set is invalidated.
    invalidateAllTargetDiagnostics();
    updateFullRepairSummary();
    appendLog(QStringLiteral("STALE DIAGNOSTICS: target configuration edited: %1. Regenerate diagnostics before the next repair.").arg(displayPath),
              QStringLiteral("INFO"), LogEntryKind::Diagnostic);
    scheduleEvidenceRefresh(QStringLiteral("target configuration edited"));
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
    // Manual Run All is always honoured: while the automatic full report is
    // already running the click is satisfied by it, and while an individual
    // diagnostic is running the full report is queued behind it instead of
    // being dropped.
    if (m_diagnosticsRunInProgress) {
        if (m_evidenceRefreshInProgress) {
            statusBar()->showMessage(QStringLiteral(
                "The full diagnostic report is already running; results appear when it completes."), 4000);
        } else if (!m_runAllDiagnosticsQueued) {
            m_runAllDiagnosticsQueued = true;
            statusBar()->showMessage(QStringLiteral(
                "Full diagnostics will run after the current diagnostic finishes."), 4000);
        }
        return;
    }
    m_runAllDiagnosticsQueued = false;

    const bool hostScope = diagnosticHostScope();
    if (hostScope) {
        // Host-scope Run All is a deliberate maintenance mode; refuse before
        // any cache is cleared or a helper request is sent. The derived scope
        // already ties host scope to Host Maintenance; this is the fail-closed
        // runtime gate if that invariant is ever broken.
        QString hostReason;
        if (!hostDiagnosticScopeAllowed(&hostReason)) {
            appendLog(QStringLiteral("Host diagnostic refused: %1").arg(hostReason),
                      QStringLiteral("WARNING"), LogEntryKind::Diagnostic);
            statusBar()->showMessage(hostReason, 6000);
            if (!m_evidenceRefreshInProgress) {
                QMessageBox::information(this, QStringLiteral("Host Maintenance required"), hostReason);
            }
            return;
        }
    }

    if (!diagnosticHostScope() && m_previewTargetPath.isEmpty()) {
        // An automatic refresh never raises a modal dialog; it simply leaves
        // the existing stale-evidence guidance in place.
        if (!m_evidenceRefreshInProgress) {
            QMessageBox::information(this, QStringLiteral("No repair target selected"),
                                     QStringLiteral("Select a repair target in Systems, or enter Host Maintenance for running-host diagnostics."));
        }
        return;
    }

    BusyOperationScope busy(this, m_evidenceRefreshInProgress
        ? QStringLiteral("Regenerating diagnostics automatically")
        : QStringLiteral("Running all diagnostics"));

    m_diagnosticsRunInProgress = true;
    const bool targetScope = !diagnosticHostScope();
    QString combined;
    bool ok = true;

    if (m_runAllDiagnosticsButton) {
        // Keep the button clickable: a click during a run is answered with a
        // status message (or queues the full report) instead of being blocked.
        m_runAllDiagnosticsButton->setText(QStringLiteral("Running…"));
    }

    // Clear stale evidence before collecting a fresh report. If collection
    // fails, repair controls remain blocked instead of reusing old data.
    if (targetScope) {
        clearTargetDiagnosticCache();
    } else {
        clearHostDiagnosticCache();
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
    const QString scopeWord = targetScope ? QStringLiteral("Target") : QStringLiteral("Host");
    appendStatusLog(statusEntryIdentity(QStringLiteral("run-all-start"), scopeWord),
                    QStringLiteral("Starting all available read-only diagnostics (%1 scope).").arg(scopeWord),
                    QStringLiteral("INFO"), LogEntryKind::Diagnostic);
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

    appendStatusLog(statusEntryIdentity(QStringLiteral("run-all-complete")),
                    ok
                        ? QStringLiteral("All available read-only diagnostic summaries generated and cached by diagnostic.")
                        : QStringLiteral("Run All diagnostics failed; failed output was not cached as successful diagnostic data."),
                    ok ? QStringLiteral("INFO") : QStringLiteral("ERROR"), LogEntryKind::Diagnostic);
    if (m_runAllDiagnosticsButton) {
        m_runAllDiagnosticsButton->setText(QStringLiteral("Run All"));
    }
    m_diagnosticsRunInProgress = false;
    if (m_runAllDiagnosticsQueued) {
        m_runAllDiagnosticsQueued = false;
        QTimer::singleShot(0, this, &MainWindow::runAllDiagnostics);
    }
}

void MainWindow::scheduleEvidenceRefresh(const QString &reason)
{
    if (!m_autoRefreshDiagnostics || !m_autoRefreshDiagnostics->isChecked()) {
        return;
    }

    if (m_privilegedOperationActive) {
        // A privileged request owns the gate. Never queue a refresh behind it
        // (the nested wait used to deadlock); remember the invalidation and
        // let the gate release schedule exactly one coalesced regeneration for
        // the then-current scope. A Full Repair plan owns its own single
        // end-of-plan regeneration and must not schedule a second run.
        if (!m_fullRepairPlanInProgress && currentDiagnosticScopeReady()) {
            if (!m_scopeChangeRefreshPending) {
                appendStatusLog(statusEntryIdentity(QStringLiteral("auto-refresh-deferred")),
                                QStringLiteral("Automatic read-only diagnostics regeneration is deferred until the running privileged operation finishes (%1).").arg(reason),
                                QStringLiteral("INFO"), LogEntryKind::Diagnostic);
            }
            m_scopeChangeRefreshPending = true;
            m_scopeChangeRefreshReason = reason;
        }
        return;
    }

    if (!shouldScheduleEvidenceRefresh()) {
        return;
    }

    if (!m_privilegedSessionReady) {
        // Automatic refresh must never raise its own Polkit prompt. Remember
        // the request and let the established-session hook schedule it.
        if (!m_evidenceRefreshPending) {
            m_evidenceRefreshPending = true;
            appendStatusLog(statusEntryIdentity(QStringLiteral("auto-refresh-pending"), reason),
                            QStringLiteral("Automatic read-only diagnostics regeneration is pending until administrator authorization is active (%1).").arg(reason),
                            QStringLiteral("INFO"), LogEntryKind::Diagnostic);
        }
        return;
    }

    if (!m_evidenceRefreshTimer) {
        m_evidenceRefreshTimer = new QTimer(this);
        m_evidenceRefreshTimer->setSingleShot(true);
        connect(m_evidenceRefreshTimer, &QTimer::timeout, this, &MainWindow::runScheduledEvidenceRefresh);
    }

    m_evidenceRefreshPending = false;
    m_evidenceRefreshReason = reason;
    if (!m_evidenceRefreshTimer->isActive()) {
        appendStatusLog(statusEntryIdentity(QStringLiteral("auto-refresh-scheduled"), reason),
                        QStringLiteral("Automatic read-only diagnostics regeneration scheduled after %1.").arg(reason),
                        QStringLiteral("INFO"), LogEntryKind::Diagnostic);
    }
    // Restarting the single-shot timer coalesces bursts of invalidations into
    // one run; the timer callback re-checks scope and evidence freshness.
    m_evidenceRefreshTimer->start(m_evidenceRefreshDelayMs);
}

void MainWindow::handlePrivilegedSessionEstablished()
{
    if (!m_evidenceRefreshPending) {
        return;
    }
    if (!m_autoRefreshDiagnostics || !m_autoRefreshDiagnostics->isChecked()) {
        m_evidenceRefreshPending = false;
        return;
    }
    if (!currentDiagnosticScopeReady()) {
        // Keep the request pending until a scope becomes available.
        return;
    }
    m_evidenceRefreshPending = false;
    scheduleEvidenceRefresh(QStringLiteral("administrator authorization became active"));
}

void MainWindow::runScheduledEvidenceRefresh()
{
    if (m_evidenceRefreshInProgress || m_diagnosticsRunInProgress || m_privilegedOperationActive) {
        // A manual run or privileged operation currently owns the helper.
        // Keep the refresh queued instead of nesting or dropping it.
        if (m_evidenceRefreshTimer) {
            m_evidenceRefreshTimer->start(m_evidenceRefreshDelayMs);
        }
        return;
    }
    if (!shouldScheduleEvidenceRefresh()) {
        m_evidenceRefreshReason.clear();
        return;
    }
    if (!m_privilegedSessionReady) {
        m_evidenceRefreshPending = true;
        m_evidenceRefreshReason.clear();
        return;
    }

    BusyOperationScope busy(this, QStringLiteral("Regenerating diagnostics"));

    m_evidenceRefreshInProgress = true;
    const QString reason = m_evidenceRefreshReason;
    m_evidenceRefreshReason.clear();
    appendStatusLog(statusEntryIdentity(QStringLiteral("auto-refresh-running"), reason),
                    QStringLiteral("Automatically regenerating read-only diagnostics (%1).")
                        .arg(reason.isEmpty() ? QStringLiteral("stale evidence") : reason),
                    QStringLiteral("INFO"), LogEntryKind::Diagnostic);
    statusBar()->showMessage(QStringLiteral("Regenerating read-only diagnostics…"));

    // A complete invalidation regenerates the combined report; a scoped
    // invalidation regenerates exactly the mapped sections, sequentially in
    // the same privileged session. The mapping table decides which case
    // applies; the executor never invents its own section list.
    const bool targetScope = !diagnosticHostScope();
    const bool fullRefresh = targetScope
        ? (m_targetDiagnosticsNeedRegeneration
            || m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty())
        : m_hostDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty();
    if (fullRefresh) {
        runAllDiagnostics();
    } else {
        const QStringList sections = targetScope
            ? m_targetDiagnosticsStaleSections.values()
            : m_hostDiagnosticsStaleSections.values();
        runDiagnosticSections(sections);
    }

    const bool regenerated = !currentScopeEvidenceStale();
    statusBar()->showMessage(regenerated
        ? QStringLiteral("Read-only diagnostics regenerated automatically.")
        : QStringLiteral("Automatic diagnostics regeneration did not complete; repair actions remain disabled until diagnostics are regenerated."),
        6000);
    appendStatusLog(statusEntryIdentity(QStringLiteral("auto-refresh-complete")),
                    regenerated
                        ? QStringLiteral("Automatic read-only diagnostics regeneration completed.")
                        : QStringLiteral("Automatic read-only diagnostics regeneration failed; cached evidence remains stale."),
                    regenerated ? QStringLiteral("INFO") : QStringLiteral("WARNING"),
                    LogEntryKind::Diagnostic);
    m_evidenceRefreshInProgress = false;
}

void MainWindow::runDeferredPrivilegedWork()
{
    if (m_privilegedOperationActive) {
        // A new request claimed the gate before this deferred work ran; its
        // own release schedules the work again.
        return;
    }
    if (m_snapshotPreloadDeferred) {
        m_snapshotPreloadDeferred = false;
        scheduleSnapshotPreload();
    }
    if (m_scopeChangeRefreshPending) {
        m_scopeChangeRefreshPending = false;
        const QString reason = m_scopeChangeRefreshReason;
        m_scopeChangeRefreshReason.clear();
        scheduleEvidenceRefresh(reason);
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
        appendLog(QStringLiteral("Diagnostic results copied to the clipboard."),
                  QStringLiteral("INFO"), LogEntryKind::Diagnostic);
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

// ---- MainWindow: session logs -----------------------------------------------

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
    const QStringList &entries = m_viewingPriorLog ? m_viewedLogEntries : m_actionLogEntries;
    stream << entries.join(QLatin1Char('\n'));
    statusBar()->showMessage(QStringLiteral("Log saved to %1").arg(path), 5000);
}

QString MainWindow::sessionLogDirectory() const
{
    const QByteArray override = qgetenv("BOOT_REPAIR_LOG_DIR");
    if (!override.isEmpty()) {
        return QString::fromLocal8Bit(override);
    }
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/logs");
}

QStringList MainWindow::sessionLogFiles() const
{
    QDir directory(sessionLogDirectory());
    if (!directory.exists()) {
        return {};
    }
    QStringList names = directory.entryList({QStringLiteral("session-*.log")}, QDir::Files, QDir::Name);
    std::reverse(names.begin(), names.end());
    QStringList paths;
    paths.reserve(names.size());
    for (const QString &name : names) {
        paths.append(directory.absoluteFilePath(name));
    }
    return paths;
}

QString MainWindow::sessionLogScopeSummary(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    // Only the leading SCOPE block is relevant; never read a whole large log
    // just to build a one-line list label.
    const QString head = QString::fromUtf8(file.read(128 * 1024));
    const QStringList lines = head.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        const int marker = line.indexOf(QStringLiteral("[SCOPE] "));
        if (marker < 0) {
            continue;
        }
        const QStringList segments = line.mid(marker + 8).split(QStringLiteral(" | "));
        if (segments.isEmpty()) {
            return QString();
        }
        QString summary = segments.first().trimmed();
        for (const QString &segment : segments) {
            if (segment.startsWith(QStringLiteral("Disk: "))) {
                const QString disk = segment.mid(6).trimmed();
                if (!disk.isEmpty() && disk != QStringLiteral("—")) {
                    summary += QStringLiteral(" · %1").arg(disk);
                }
                break;
            }
        }
        return summary;
    }
    return QString();
}

QString MainWindow::sessionLogGenerationTime(const QString &path) const
{
    const QFileInfo info(path);
    const QString base = info.completeBaseName();
    const QString stamp = base.startsWith(QStringLiteral("session-")) ? base.mid(8) : base;
    QDateTime generated = QDateTime::fromString(stamp, QStringLiteral("yyyyMMdd-HHmmss"));
    if (!generated.isValid()) {
        // Duplicate-name files (session-<stamp>-2.log) and hand-copied logs
        // keep the file's own timestamp so the label is still meaningful.
        generated = info.lastModified();
    }
    return generated.isValid() ? generated.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")) : stamp;
}

QString MainWindow::sessionLogLabel(const QString &path) const
{
    const QString readable = sessionLogGenerationTime(path);
    const QString scope = sessionLogScopeSummary(path);
    return scope.isEmpty() ? readable : QStringLiteral("%1 — %2").arg(readable, scope);
}

QString MainWindow::detectedDistributionFamily() const
{
    if (!m_hostMaintenanceMode && m_targetDiagnosticCacheIdentity != currentTargetDiagnosticCacheIdentity()) {
        return QString();
    }
    const auto &cache = m_hostMaintenanceMode ? m_hostDiagnosticCache : m_targetDiagnosticCache;
    for (const QString &evidence : cache) {
        for (const QString &line : evidence.split(QLatin1Char('\n'))) {
            const QString trimmed = line.trimmed();
            if (trimmed.startsWith(QStringLiteral("Distribution family:"), Qt::CaseInsensitive)) {
                return trimmed.section(QLatin1Char(':'), 1).trimmed();
            }
        }
    }
    return QString();
}

// The current scope's cached diagnostic text. Evidence from a different target
// is never used for backend labels (fail closed to generic wording).
QString MainWindow::currentScopeEvidence() const
{
    if (!m_hostMaintenanceMode && m_targetDiagnosticCacheIdentity != currentTargetDiagnosticCacheIdentity()) {
        return QString();
    }
    const auto &cache = m_hostMaintenanceMode ? m_hostDiagnosticCache : m_targetDiagnosticCache;
    return cache.values().join(QLatin1Char('\n'));
}

QStringList MainWindow::detectedPackageManagers() const
{
    QStringList managers;
    const QString evidence = currentScopeEvidence();
    for (const QString &line : evidence.split(QLatin1Char('\n'))) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(QStringLiteral("Package manager backends:"), Qt::CaseInsensitive)) {
            managers = trimmed.section(QLatin1Char(':'), 1).split(QLatin1Char(','), Qt::SkipEmptyParts);
        } else if (managers.isEmpty()
                   && trimmed.startsWith(QStringLiteral("Package manager backend:"), Qt::CaseInsensitive)) {
            managers = QStringList{trimmed.section(QLatin1Char(':'), 1).trimmed()};
        }
    }
    for (QString &manager : managers) {
        manager = manager.trimmed();
    }
    managers.removeAll(QString());
    return managers;
}

QString MainWindow::detectedServiceManager() const
{
    const QString evidence = currentScopeEvidence();
    for (const QString &line : evidence.split(QLatin1Char('\n'))) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(QStringLiteral("Service manager:"), Qt::CaseInsensitive)) {
            return trimmed.section(QLatin1Char(':'), 1).trimmed();
        }
    }
    return QString();
}

QString MainWindow::detectedInitramfsBackend() const
{
    const QString evidence = currentScopeEvidence();
    for (const QString &line : evidence.split(QLatin1Char('\n'))) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(QStringLiteral("Initramfs backend:"), Qt::CaseInsensitive)) {
            return trimmed.section(QLatin1Char(':'), 1).trimmed();
        }
    }
    return QString();
}

QString MainWindow::detectedBootloaderBackend() const
{
    const QString evidence = currentScopeEvidence();
    for (const QString &line : evidence.split(QLatin1Char('\n'))) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(QStringLiteral("Bootloader backend:"), Qt::CaseInsensitive)) {
            return trimmed.section(QLatin1Char(':'), 1).trimmed();
        }
    }
    return QString();
}

bool MainWindow::detectedRpmBackend() const
{
    // The backend profile prints the detected package-manager evidence
    // ("rpm"), and a future helper may print the dnf frontend name directly;
    // either spelling selects the Fedora/RPM wording.  Availability still
    // comes exclusively from the `Repair tool <key>` capability lines.
    for (const QString &manager : detectedPackageManagers()) {
        if (manager.compare(QStringLiteral("rpm"), Qt::CaseInsensitive) == 0
            || manager.compare(QStringLiteral("dnf"), Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

bool MainWindow::detectedGrub2Backend() const
{
    const QString backend = detectedBootloaderBackend();
    if (backend.contains(QStringLiteral("grub2"), Qt::CaseInsensitive)) {
        return true;
    }
    if (!backend.contains(QStringLiteral("grub"), Qt::CaseInsensitive)) {
        return false;
    }
    // Fedora's GRUB is named grub2 and stores its configuration under
    // /boot/grub2.  Accept the tool/path evidence the helper emits (or a
    // future `GRUB tools:` profile line) so the wording follows the probes
    // without a distribution-family gate; no such evidence keeps the generic
    // GRUB wording.
    const QString evidence = currentScopeEvidence();
    return evidence.contains(QStringLiteral("grub2-mkconfig"), Qt::CaseInsensitive)
        || evidence.contains(QStringLiteral("grub2-editenv"), Qt::CaseInsensitive)
        || evidence.contains(QStringLiteral("grub2-install"), Qt::CaseInsensitive)
        || evidence.contains(QStringLiteral("/boot/grub2"), Qt::CaseInsensitive);
}

QString MainWindow::detectedDisplayManagerName() const
{
    const QString evidence = currentScopeEvidence();
    auto mentions = [&evidence](const QString &token) {
        return evidence.contains(token, Qt::CaseInsensitive);
    };
    // GDM3 is checked before GDM so the Debian unit/name is never reported as
    // Fedora's GDM.  Each manager is matched by its unit name or configuration
    // directory, the same evidence the helper's display probes read.
    if (mentions(QStringLiteral("sddm.service")) || mentions(QStringLiteral("/etc/sddm"))) {
        return QStringLiteral("SDDM");
    }
    if (mentions(QStringLiteral("gdm3.service")) || mentions(QStringLiteral("/etc/gdm3"))) {
        return QStringLiteral("GDM3");
    }
    if (mentions(QStringLiteral("lightdm.service")) || mentions(QStringLiteral("/etc/lightdm"))) {
        return QStringLiteral("LightDM");
    }
    if (mentions(QStringLiteral("gdm.service")) || mentions(QStringLiteral("/etc/gdm/"))) {
        return QStringLiteral("GDM");
    }
    if (mentions(QStringLiteral("greetd.service")) || mentions(QStringLiteral("/etc/greetd"))) {
        return QStringLiteral("greetd");
    }
    if (mentions(QStringLiteral("ly.service")) || mentions(QStringLiteral("/etc/ly/"))) {
        return QStringLiteral("Ly");
    }
    return QString();
}

// Adopts the process-wide active session file (a second window continuing the
// same run) and loads its entries into the live register. A missing or
// unreadable file clears the process-wide path so the next scope
// identification starts a fresh session instead.
bool MainWindow::adoptActiveSessionLog()
{
    if (s_activeSessionPath.isEmpty()) {
        return false;
    }
    if (!QFileInfo::exists(s_activeSessionPath)) {
        // The file backing this process's active session is gone (for example
        // a throwaway test directory was removed). A later scope
        // identification starts a fresh session instead of resurrecting it.
        s_activeSessionPath.clear();
        return false;
    }
    QFile reader(s_activeSessionPath);
    if (!reader.open(QIODevice::ReadOnly | QIODevice::Text)) {
        s_activeSessionPath.clear();
        return false;
    }
    const QString text = QString::fromUtf8(reader.readAll());
    reader.close();
    auto *file = new QFile(s_activeSessionPath);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        delete file;
        s_activeSessionPath.clear();
        return false;
    }
    m_sessionLogFile = file;
    m_sessionLogPath = s_activeSessionPath;
    // The live register keeps the newest entry first, matching appendLog();
    // the file itself stays append-only in chronological order.
    m_actionLogEntries = parseSessionLogEntries(text);
    std::reverse(m_actionLogEntries.begin(), m_actionLogEntries.end());
    // The restored scope belongs to the run, not to this window, until the
    // window identifies or re-confirms it itself.
    m_sessionScopeIdentifiedHere = false;
    restoreSessionScopeFromLog(text);
    // Restored status entries stay genuine history; only entries appended by
    // this window are tracked for in-place replacement.
    m_statusLogEntries.clear();
    m_statusEntryIdentities.clear();
    rebuildDiagnosticLogIndex();
    // The live register was replaced wholesale; the next refresh must rebuild
    // the view instead of applying the deferred incremental edits.
    m_logViewRendered = false;
    return true;
}

void MainWindow::restoreSessionScopeFromLog(const QString &logText)
{
    const QStringList lines = logText.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        const int marker = line.indexOf(QStringLiteral("[SCOPE] "));
        if (marker < 0) {
            continue;
        }
        const QStringList segments = line.mid(marker + 8).split(QStringLiteral(" | "));
        if (segments.isEmpty()) {
            continue;
        }
        const QString label = segments.first().trimmed();
        if (label.isEmpty() || label == QStringLiteral("No scope")) {
            m_sessionScopeKey.clear();
            m_sessionScopeLabel.clear();
            m_sessionScopeDisk.clear();
            continue;
        }
        QString disk;
        QString root;
        for (const QString &segment : segments) {
            if (segment.startsWith(QStringLiteral("Disk: "))) {
                disk = segment.mid(6).trimmed();
            } else if (segment.startsWith(QStringLiteral("Root: "))) {
                root = segment.mid(6).trimmed();
            }
        }
        if (disk == QStringLiteral("—")) {
            disk.clear();
        }
        if (root == QStringLiteral("—")) {
            root.clear();
        }
        m_sessionScopeKey = QStringLiteral("%1|%2|%3").arg(label, disk, root);
        m_sessionScopeLabel = label;
        m_sessionScopeDisk = disk;
    }
}

// Resolves the active host/target scope, creates the session file on the first
// usable scope, writes a SCOPE marker when the scope changes and records
// "No scope" when a previously identified scope is lost.
void MainWindow::updateSessionScope()
{
    QString label;
    QString disk;
    QString component;
    if (m_hostMaintenanceMode && !m_hostPrimaryPath.isEmpty()) {
        label = QStringLiteral("Running Host");
        disk = m_hostPrimaryPath;
        component = m_hostPrimaryComponentPath;
    } else if (!m_previewTargetPath.isEmpty() && !m_previewTargetComponentPath.isEmpty()) {
        // A locked LUKS container is not a usable repair scope. Wait until the
        // decrypted Linux root is resolved and committed before opening a file.
        const DeviceNode componentNode = m_deviceIndex.value(m_previewTargetComponentPath);
        if (!componentNode.path.isEmpty() && !componentNode.encrypted
            && (componentNode.linuxCapableFileSystem || componentNode.installedLinux)) {
            label = QStringLiteral("Repair Target");
            disk = m_previewTargetPath;
            component = m_previewTargetComponentPath;
        }
    }

    if (label.isEmpty()) {
        if (m_sessionLogFile && m_sessionScopeIdentifiedHere && !m_sessionScopeKey.isEmpty()) {
            appendSessionScopeMarker(QStringLiteral("No scope"), QString(), QString(), QString());
            m_sessionScopeKey.clear();
            m_sessionScopeLabel.clear();
            m_sessionScopeDisk.clear();
            refreshSessionLogList();
        }
        return;
    }

    const QString key = QStringLiteral("%1|%2|%3").arg(label, disk, component);
    if (key == m_sessionScopeKey) {
        // Re-confirming the restored scope marks it as this window's own, so a
        // later scope loss is recorded as "No scope" in the shared file.
        m_sessionScopeIdentifiedHere = true;
        return;
    }
    const QString family = detectedDistributionFamily();
    if (!m_sessionLogFile) {
        createSessionLogFile(label, disk, component, family);
    } else {
        appendSessionScopeMarker(label, disk, component, family);
    }
    m_sessionScopeKey = key;
    m_sessionScopeLabel = label;
    m_sessionScopeDisk = disk;
    m_sessionScopeIdentifiedHere = true;
    refreshSessionLogList();
}

void MainWindow::createSessionLogFile(const QString &scopeLabel, const QString &disk,
                                      const QString &component, const QString &family)
{
    if (m_sessionLogFile) {
        return;
    }
    // A second window in the same process shares this run's active session
    // file instead of creating another one for the same scope.
    if (adoptActiveSessionLog()) {
        appendSessionScopeMarker(scopeLabel, disk, component, family);
        flushPendingSessionEntries();
        return;
    }
    const QString directory = sessionLogDirectory();
    if (!QDir().mkpath(directory)) {
        return; // The in-memory log keeps working without a file.
    }
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"));
    QString path = QDir(directory).filePath(QStringLiteral("session-%1.log").arg(stamp));
    int suffix = 2;
    while (QFile::exists(path)) {
        path = QDir(directory).filePath(QStringLiteral("session-%1-%2.log").arg(stamp).arg(suffix++));
    }
    auto *file = new QFile(path);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete file;
        return;
    }
    m_sessionLogFile = file;
    m_sessionLogPath = path;
    s_activeSessionPath = path;
    appendSessionScopeMarker(scopeLabel, disk, component, family);
    appendLog(QStringLiteral("Session log %1 generated %2")
                  .arg(QFileInfo(path).fileName(),
                       QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))));
    flushPendingSessionEntries();
}

void MainWindow::appendSessionScopeMarker(const QString &scopeLabel, const QString &disk,
                                          const QString &component, const QString &family)
{
    if (!m_sessionLogFile) {
        return;
    }
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    const QString entry = QStringLiteral(
        "──────── SCOPE ────────\n"
        "[%1] [SCOPE] %2 | Disk: %3 | Root: %4 | OS: %5")
        .arg(timestamp, scopeLabel,
             disk.isEmpty() ? QStringLiteral("—") : disk,
             component.isEmpty() ? QStringLiteral("—") : component,
             family.isEmpty() ? QStringLiteral("unknown") : family);
    writeSessionLogEntry(entry);
}

void MainWindow::writeSessionLogEntry(const QString &entry)
{
    if (!m_sessionLogFile) {
        m_pendingSessionEntries.append(entry);
        return;
    }
    m_sessionLogFile->write(entry.toUtf8());
    m_sessionLogFile->write("\n");
    m_sessionLogFile->flush();
}

void MainWindow::flushPendingSessionEntries()
{
    if (!m_sessionLogFile) {
        return;
    }
    for (const QString &entry : m_pendingSessionEntries) {
        m_sessionLogFile->write(entry.toUtf8());
        m_sessionLogFile->write("\n");
    }
    m_pendingSessionEntries.clear();
    m_sessionLogFile->flush();
}

void MainWindow::closeSessionLogFile()
{
    if (!m_sessionLogFile) {
        return;
    }
    m_sessionLogFile->close();
    delete m_sessionLogFile;
    m_sessionLogFile = nullptr;
}

void MainWindow::refreshSessionLogList()
{
    if (!m_sessionLogList) {
        return;
    }
    const QString wanted = m_viewingPriorLog ? m_viewedLogPath : QString();
    {
        QSignalBlocker blocker(m_sessionLogList);
        m_sessionLogList->clear();
        QString liveLabel = QStringLiteral("Current session");
        if (m_sessionLogPath.isEmpty()) {
            // A fresh launch owns no session file until a scope is identified;
            // the previous run's file is only ever listed as a prior session.
            liveLabel = QStringLiteral("Current session — not started");
        } else if (!m_sessionScopeLabel.isEmpty()) {
            liveLabel += QStringLiteral(" — %1").arg(m_sessionScopeLabel);
            if (!m_sessionScopeDisk.isEmpty()) {
                liveLabel += QStringLiteral(" · %1").arg(m_sessionScopeDisk);
            }
        }
        auto *liveItem = new QListWidgetItem(liveLabel, m_sessionLogList);
        liveItem->setData(Qt::UserRole, QString());
        liveItem->setToolTip(QStringLiteral(
            "Entries from this window. A fresh launch starts empty; a session file is created once a running-host or repair-target scope is identified."));
        for (const QString &path : sessionLogFiles()) {
            if ((!m_sessionLogPath.isEmpty() && path == m_sessionLogPath)
                || (!s_activeSessionPath.isEmpty() && path == s_activeSessionPath)) {
                continue;
            }
            auto *item = new QListWidgetItem(sessionLogLabel(path), m_sessionLogList);
            item->setData(Qt::UserRole, path);
            item->setToolTip(path);
        }
        int row = 0;
        if (!wanted.isEmpty()) {
            for (int index = 0; index < m_sessionLogList->count(); ++index) {
                if (m_sessionLogList->item(index)->data(Qt::UserRole).toString() == wanted) {
                    row = index;
                    break;
                }
            }
        }
        m_sessionLogList->setCurrentRow(row);
    }
    displaySessionLog(wanted);
}

void MainWindow::displaySessionLog(const QString &path)
{
    bool prior = !path.isEmpty() && path != m_sessionLogPath
        && (s_activeSessionPath.isEmpty() || path != s_activeSessionPath);
    if (prior) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            prior = false;
        } else {
            m_viewedLogEntries = parseSessionLogEntries(QString::fromUtf8(file.readAll()));
            m_viewedLogPath = path;
        }
    }
    m_viewingPriorLog = prior;
    if (!prior) {
        m_viewedLogEntries.clear();
        m_viewedLogPath.clear();
    }
    if (m_priorLogBanner) {
        if (prior) {
            m_priorLogBanner->setText(QStringLiteral("Viewing prior session log %1 — generated %2 — read only")
                                          .arg(QFileInfo(path).fileName(), sessionLogGenerationTime(path)));
            m_priorLogBanner->setVisible(true);
        } else {
            m_priorLogBanner->setVisible(false);
        }
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(prior);
    }
    refreshLogView();
}

void MainWindow::startNewSessionLog()
{
    closeSessionLogFile();
    m_sessionLogPath.clear();
    // The closed file becomes a prior session; this process no longer owns an
    // active session until the next scope identification creates one.
    s_activeSessionPath.clear();
    m_pendingSessionEntries.clear();
    m_sessionScopeKey.clear();
    m_sessionScopeLabel.clear();
    m_sessionScopeDisk.clear();
    m_sessionScopeIdentifiedHere = false;
    appendLog(QStringLiteral("Started a new session log; earlier files remain available in the session list."));
    updateSessionScope();
    refreshSessionLogList();
    if (m_sessionLogList) {
        m_sessionLogList->setCurrentRow(0);
    }
    displaySessionLog(QString());
}

void MainWindow::clearCurrentSessionLog()
{
    const bool hasFile = m_sessionLogFile != nullptr && !m_sessionLogPath.isEmpty();
    const QString fileName = hasFile ? QFileInfo(m_sessionLogPath).fileName() : QString();
    const QString question = hasFile
        ? QStringLiteral("Clear the current session log %1? The active file is truncated and cannot be restored. Prior session files are not touched.").arg(fileName)
        : QStringLiteral("No session log file exists yet because no scope has been identified. Clear the in-memory entries only?");
    if (QMessageBox::question(this, QStringLiteral("Clear session log"), question,
                              QMessageBox::Cancel | QMessageBox::Yes,
                              QMessageBox::Cancel) != QMessageBox::Yes) {
        return;
    }

    if (hasFile) {
        if (!m_sessionLogFile->resize(0)) {
            QMessageBox::warning(this, QStringLiteral("Clear session log"),
                                 QStringLiteral("Unable to truncate %1.").arg(fileName));
            return;
        }
        m_sessionLogFile->seek(0);
    }
    m_actionLogEntries.clear();
    m_diagnosticLogEntries.clear();
    m_repairLogEntries.clear();
    m_statusLogEntries.clear();
    m_statusEntryIdentities.clear();
    m_activeRepairSection.clear();
    m_activeRepairEntries.clear();
    m_pendingLogEntries.clear();
    m_pendingEntryRemovals.clear();
    m_renderedEntryRanges.clear();
    m_logViewRendered = false;
    m_pendingSessionEntries.clear();
    if (!hasFile) {
        appendLog(QStringLiteral("Session log clear requested, but no session file exists yet; in-memory entries were cleared."));
    } else {
        appendLog(QStringLiteral("Session log cleared by user at %1")
                      .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))));
    }
    // Clearing is an explicit user action: show the result immediately instead
    // of waiting for the coalescing timer.
    refreshLogView();
    if (hasFile) {
        refreshSessionLogList();
    }
}

void MainWindow::addSessionNote()
{
    bool accepted = false;
    const QString note = QInputDialog::getText(this, QStringLiteral("Add Note"),
                                               QStringLiteral("Note:"),
                                               QLineEdit::Normal, QString(), &accepted);
    if (!accepted) {
        return;
    }
    const QString trimmed = note.trimmed();
    if (trimmed.isEmpty()) {
        return;
    }
    appendLog(QStringLiteral("NOTE: %1").arg(trimmed));
}

void MainWindow::deleteSelectedSessionLog()
{
    if (!m_sessionLogList) {
        return;
    }
    QListWidgetItem *item = m_sessionLogList->currentItem();
    if (!item) {
        return;
    }
    const QString path = item->data(Qt::UserRole).toString();
    if (path.isEmpty() || path == m_sessionLogPath || path == s_activeSessionPath) {
        return;
    }
    const QFileInfo info(path);
    if (!info.fileName().startsWith(QStringLiteral("session-"))
        || !info.fileName().endsWith(QStringLiteral(".log"))) {
        return;
    }
    const QString canonicalDirectory = QFileInfo(sessionLogDirectory()).canonicalFilePath();
    const QString canonicalPath = info.canonicalFilePath();
    if (canonicalDirectory.isEmpty() || canonicalPath.isEmpty()
        || !canonicalPath.startsWith(canonicalDirectory + QLatin1Char('/'))) {
        return;
    }
    if (QMessageBox::question(this, QStringLiteral("Delete session log"),
                              QStringLiteral("Delete %1 permanently? This cannot be undone.").arg(info.fileName()),
                              QMessageBox::Cancel | QMessageBox::Yes,
                              QMessageBox::Cancel) != QMessageBox::Yes) {
        return;
    }
    if (!QFile::remove(canonicalPath)) {
        QMessageBox::warning(this, QStringLiteral("Delete session log"),
                             QStringLiteral("Unable to delete %1.").arg(info.fileName()));
        return;
    }
    appendLog(QStringLiteral("Deleted prior session log %1.").arg(info.fileName()));
    if (m_viewingPriorLog && m_viewedLogPath == path) {
        m_viewingPriorLog = false;
        m_viewedLogPath.clear();
        m_viewedLogEntries.clear();
        if (m_priorLogBanner) {
            m_priorLogBanner->setVisible(false);
        }
    }
    refreshSessionLogList();
}

void MainWindow::pruneSessionLogs()
{
    const QStringList files = sessionLogFiles();
    if (files.isEmpty()) {
        return;
    }
    constexpr int maxFiles = 20;
    constexpr qint64 maxBytes = 10 * 1024 * 1024;
    qint64 totalBytes = 0;
    int kept = 0;
    bool overLimit = false;
    QStringList removed;
    for (const QString &path : files) {
        if ((!m_sessionLogPath.isEmpty() && path == m_sessionLogPath)
            || (!s_activeSessionPath.isEmpty() && path == s_activeSessionPath)) {
            continue;
        }
        const qint64 size = QFileInfo(path).size();
        if (!overLimit && kept < maxFiles && totalBytes + size <= maxBytes) {
            ++kept;
            totalBytes += size;
            continue;
        }
        overLimit = true;
        if (QFile::remove(path)) {
            removed.append(QFileInfo(path).fileName());
        }
    }
    if (!removed.isEmpty()) {
        appendLog(QStringLiteral("Session log retention removed %1 older file(s); at most 20 files / 10 MB are kept.")
                      .arg(removed.size()));
    }
}

// Splits a session file into entries on the "──────── CATEGORY ────────"
// headers, discarding the blank separator lines between entries.
QStringList MainWindow::parseSessionLogEntries(const QString &text)
{
    QStringList entries;
    QStringList current;
    const QStringList lines = text.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        if (line.startsWith(QStringLiteral("────────")) && !current.isEmpty()) {
            entries.append(current.join(QLatin1Char('\n')));
            current.clear();
        }
        if (!current.isEmpty() || !line.trimmed().isEmpty()) {
            current.append(line);
        }
    }
    if (!current.isEmpty()) {
        entries.append(current.join(QLatin1Char('\n')));
    }
    return entries;
}

// ---- MainWindow: log register and view --------------------------------------

void MainWindow::appendLog(const QString &message, const QString &level, LogEntryKind kind)
{
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    // The kind is supplied by the caller and only projected to the display
    // category here; the message text never influences classification.
    const QString category = logCategoryForKind(kind);
    const QString entry = QStringLiteral("──────── %1 ────────\n[%2] [%3] %4")
        .arg(category, timestamp, level, message);
    // Keep the newest event at the front so the Logs tab opens directly on
    // the most recent repair, diagnostic, or shell transcript. The session
    // file is append-only and is not capped; the UI model keeps its 5000 cap.
    m_actionLogEntries.prepend(entry);
    // While a repair section is being captured, every entry appended by that
    // repair cycle (start notice, privileged completion line, captured output
    // and any failure notice) joins the section's replacement group.
    if (!m_activeRepairSection.isEmpty()) {
        m_activeRepairEntries.append(entry);
    }
    m_pendingLogEntries.append(entry);
    while (m_actionLogEntries.size() > 5000) {
        m_actionLogEntries.removeLast();
        // A trimmed tail cannot be removed incrementally without locating the
        // oldest rendered block; force the next refresh to rebuild fully.
        m_logModelTrimmed = true;
    }
    writeSessionLogEntry(entry);
    scheduleLogRefresh();
}

void MainWindow::appendStatusLog(const QString &identity, const QString &message,
                                 const QString &level, LogEntryKind kind)
{
    // Replace the previous entry for this identity. The session file stays
    // append-only, so the on-disk history keeps every occurrence while the
    // live register keeps exactly the newest one.
    const auto previous = m_statusLogEntries.constFind(identity);
    if (previous != m_statusLogEntries.constEnd()) {
        const QString previousEntry = previous.value();
        const int index = m_actionLogEntries.indexOf(previousEntry);
        if (index >= 0) {
            m_actionLogEntries.removeAt(index);
        }
        const int activeIndex = m_activeRepairEntries.lastIndexOf(previousEntry);
        if (activeIndex >= 0) {
            m_activeRepairEntries.removeAt(activeIndex);
        }
        // An entry that was only queued for rendering is dropped from the
        // queue; one already present in the document is queued for block
        // removal, exactly like a replaced diagnostic section.
        const int pendingIndex = m_pendingLogEntries.lastIndexOf(previousEntry);
        if (pendingIndex >= 0) {
            m_pendingLogEntries.removeAt(pendingIndex);
        }
        if (m_renderedEntryRanges.contains(identity)) {
            m_pendingEntryRemovals.insert(identity);
        }
        m_statusEntryIdentities.remove(previousEntry);
        m_statusLogEntries.erase(previous);
    }
    appendLog(message, level, kind);
    const QString entry = m_actionLogEntries.constFirst();
    m_statusLogEntries.insert(identity, entry);
    m_statusEntryIdentities.insert(entry, identity);
}

// The identity under which a rendered entry is tracked for replacement, or an
// empty string for append-only history. Diagnostic sections carry their
// (key, scope) framing; status entries are matched through the identity map
// because the message body alone does not name the status kind.
QString MainWindow::trackedEntryIdentity(const QString &entry) const
{
    QString key;
    QString scope;
    if (diagnosticEntryMetadata(entry, &key, &scope)) {
        return diagnosticEntryIdentity(key, scope);
    }
    const auto it = m_statusEntryIdentities.constFind(entry);
    return it == m_statusEntryIdentities.constEnd() ? QString() : it.value();
}

void MainWindow::scheduleLogRefresh()
{
    if (!m_logView) {
        return;
    }
    if (!m_logRefreshTimer) {
        // A single coalescing delay absorbs bursts of appends (diagnostics can
        // add many large entries in one operation) into one view refresh so
        // the event loop is not saturated while work is running.
        m_logRefreshTimer = new QTimer(this);
        m_logRefreshTimer->setSingleShot(true);
        m_logRefreshTimer->setInterval(100);
        connect(m_logRefreshTimer, &QTimer::timeout, this, &MainWindow::refreshLogView);
    }
    m_logRefreshTimer->start();
}

// Colors the repair-result symbols in place with character formats only. The
// document stays plain text: setPlainText()/insertText() write the same bytes
// and only the ✓/✗/▪ glyphs of summary lines receive a foreground color, so
// search, filters, copy, Save As and the session file are unaffected.
void MainWindow::applyRepairResultColoring(const QString &text, int documentOffset)
{
    if (!m_logView || text.isEmpty()) {
        return;
    }
    QTextDocument *document = m_logView->document();
    if (!document) {
        return;
    }
    const auto colorSymbols = [&](int lineStart, const QString &line) {
        for (const auto &symbol : repairResultSymbolFormats()) {
            int index = line.indexOf(symbol.first);
            while (index >= 0) {
                QTextCursor cursor(document);
                cursor.setPosition(documentOffset + lineStart + index);
                cursor.setPosition(documentOffset + lineStart + index + symbol.first.size(),
                                   QTextCursor::KeepAnchor);
                cursor.mergeCharFormat(symbol.second);
                index = line.indexOf(symbol.first, index + symbol.first.size());
            }
        }
    };

    int lineStart = 0;
    while (lineStart <= text.size()) {
        int lineEnd = text.indexOf(QLatin1Char('\n'), lineStart);
        if (lineEnd < 0) {
            lineEnd = text.size();
        }
        const QString line = text.mid(lineStart, lineEnd - lineStart);
        if (isRepairResultLine(line)) {
            colorSymbols(lineStart, line);
        }
        if (lineEnd >= text.size()) {
            break;
        }
        lineStart = lineEnd + 1;
    }
}

// Renders the live register or the selected prior session into the log view.
// While the live register is shown unfiltered, deferred edits are applied
// incrementally; every other case (filter, search, prior log, cap trim) falls
// back to a full rebuild.
void MainWindow::refreshLogView()
{
    if (!m_logView) {
        return;
    }
    if (m_logRefreshTimer) {
        m_logRefreshTimer->stop();
    }
    ++m_logRefreshCount;

    QScrollBar *scrollBar = m_logView->verticalScrollBar();
    const bool wasAtTop = !scrollBar || scrollBar->value() <= 2;
    QSet<QString> staleSections = m_targetDiagnosticsStaleSections;
    staleSections.unite(m_hostDiagnosticsStaleSections);
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
        const auto entryDiagnosticKey = [](const QString &text) {
            const QRegularExpression header(QStringLiteral("(?:^|\\n)Diagnostic: ([A-Za-z0-9_-]+)\\n"));
            const QRegularExpressionMatch match = header.match(text);
            return match.hasMatch() ? match.captured(1) : QString();
        };
        const auto flushEntry = [&] {
            if (entryBlocks.isEmpty()) return;
            QString entryText;
            for (const QTextBlock &entryBlock : entryBlocks) {
                entryText += entryBlock.text() + QLatin1Char('\n');
            }
            const QString entryKey = entryDiagnosticKey(entryText);
            const bool targetStaleSection = !m_targetDiagnosticsStaleSections.isEmpty()
                && entryText.contains(QStringLiteral("Scope: Repair Target"), Qt::CaseInsensitive)
                && m_targetDiagnosticsStaleSections.contains(entryKey);
            const bool hostStaleSection = !m_hostDiagnosticsStaleSections.isEmpty()
                && entryText.contains(QStringLiteral("Scope: Running Host"), Qt::CaseInsensitive)
                && m_hostDiagnosticsStaleSections.contains(entryKey);
            const bool stale = entryText.contains(QStringLiteral("STALE DIAGNOSTICS"), Qt::CaseInsensitive)
                || (m_targetDiagnosticsNeedRegeneration
                    && entryText.contains(QStringLiteral("Diagnostic:"), Qt::CaseInsensitive)
                    && entryText.contains(QStringLiteral("Scope: Repair Target"), Qt::CaseInsensitive))
                || targetStaleSection
                || hostStaleSection;
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

    // Search and display operate on whichever log is open: the live register
    // or the prior session file selected in the session list.
    const QStringList &sourceEntries = m_viewingPriorLog ? m_viewedLogEntries : m_actionLogEntries;
    const QString query = m_logSearchEdit ? m_logSearchEdit->text().trimmed().toCaseFolded() : QString();
    const QString filterKey = m_logFilterCombo
        ? m_logFilterCombo->currentData().toString()
        : QStringLiteral("all");

    const bool liveUnfiltered = !m_viewingPriorLog && query.isEmpty()
        && filterKey == QStringLiteral("all");
    const bool previousWasLiveUnfiltered = m_logViewRendered && !m_renderedPriorLog
        && m_renderedLogQuery.isEmpty() && m_renderedLogFilter == QStringLiteral("all");

    // Incremental path: while the live register is shown unfiltered, apply the
    // deferred edits locally instead of rebuilding the whole document. New
    // entries are prepended with a QTextCursor at the document start; replaced
    // diagnostic sections remove their old block range first. A cap trim, log
    // switch or search change falls back to a full rebuild.
    if (liveUnfiltered && previousWasLiveUnfiltered && !m_logModelTrimmed) {
        if (!m_pendingEntryRemovals.isEmpty()) {
            QTextDocument *document = m_logView->document();
            for (const QString &identity : m_pendingEntryRemovals) {
                const auto rangeIt = m_renderedEntryRanges.find(identity);
                if (rangeIt == m_renderedEntryRanges.end()) {
                    continue;
                }
                const int rangeStart = rangeIt->first;
                const int rangeEnd = rangeIt->second;
                const int documentLength = document->characterCount() - 1;
                // Remove the section together with one adjacent separator so
                // no empty entry is left behind.
                const bool lastEntry = rangeEnd >= documentLength;
                const int removalStart = lastEntry ? qMax(0, rangeStart - 1) : rangeStart;
                const int removalEnd = lastEntry ? rangeEnd : rangeEnd + 1;
                m_renderedEntryRanges.erase(rangeIt);
                if (removalEnd <= removalStart) {
                    continue;
                }
                QTextCursor cursor(document);
                cursor.setPosition(removalStart);
                cursor.setPosition(removalEnd, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                const int removedCount = removalEnd - removalStart;
                for (auto shiftIt = m_renderedEntryRanges.begin();
                     shiftIt != m_renderedEntryRanges.end(); ) {
                    if (shiftIt->first >= removalEnd) {
                        shiftIt->first -= removedCount;
                        shiftIt->second -= removedCount;
                        ++shiftIt;
                    } else if (shiftIt->second > removalStart) {
                        // Whole-entry ranges cannot partially overlap; drop
                        // the entry defensively rather than keep a stale one.
                        shiftIt = m_renderedEntryRanges.erase(shiftIt);
                    } else {
                        ++shiftIt;
                    }
                }
            }
            m_pendingEntryRemovals.clear();
        }

        if (!m_pendingLogEntries.isEmpty()) {
            // Pending entries are recorded in append order; the document keeps
            // the newest entry first.
            QStringList newestFirst;
            newestFirst.reserve(m_pendingLogEntries.size());
            for (auto it = m_pendingLogEntries.crbegin(); it != m_pendingLogEntries.crend(); ++it) {
                newestFirst.append(*it);
            }
            const bool documentEmpty = m_logView->document()->isEmpty();
            const QString inserted = newestFirst.join(QLatin1Char('\n'))
                + (documentEmpty ? QString() : QStringLiteral("\n"));
            QTextCursor cursor(m_logView->document());
            cursor.movePosition(QTextCursor::Start);
            cursor.insertText(inserted);
            applyRepairResultColoring(inserted);
            for (auto rangeIt = m_renderedEntryRanges.begin();
                 rangeIt != m_renderedEntryRanges.end(); ++rangeIt) {
                rangeIt->first += inserted.size();
                rangeIt->second += inserted.size();
            }
            int offset = 0;
            for (const QString &entry : newestFirst) {
                const QString entryIdentity = trackedEntryIdentity(entry);
                if (!entryIdentity.isEmpty()) {
                    const int entrySize = static_cast<int>(entry.size());
                    m_renderedEntryRanges.insert(
                        entryIdentity, qMakePair(offset, offset + entrySize));
                }
                offset += static_cast<int>(entry.size()) + 1;
            }
            m_pendingLogEntries.clear();
        }

        // Stale highlighting is only recomputed when its input changes or on
        // a full rebuild, never on every incremental append.
        if (!m_staleHighlightValid
            || m_staleHighlightFlag != m_targetDiagnosticsNeedRegeneration
            || m_staleHighlightSections != staleSections) {
            markStaleEntries();
            m_staleHighlightFlag = m_targetDiagnosticsNeedRegeneration;
            m_staleHighlightSections = staleSections;
            m_staleHighlightValid = true;
        }
        if (wasAtTop && scrollBar) {
            scrollBar->setValue(0);
        }
        return;
    }

    if (query.isEmpty() && filterKey == QStringLiteral("all")) {
        const QString rebuilt = sourceEntries.join(QLatin1Char('\n'));
        m_logView->setPlainText(rebuilt);
        applyRepairResultColoring(rebuilt);
        markStaleEntries();
        m_staleHighlightFlag = m_targetDiagnosticsNeedRegeneration;
        m_staleHighlightSections = staleSections;
        m_staleHighlightValid = true;
        m_renderedLogQuery.clear();
        m_renderedLogFilter = filterKey;
        m_renderedPriorLog = m_viewingPriorLog;
        m_logViewRendered = true;
        m_logModelTrimmed = false;
        m_pendingLogEntries.clear();
        m_pendingEntryRemovals.clear();
        m_renderedEntryRanges.clear();
        if (!m_viewingPriorLog) {
            int offset = 0;
            for (const QString &entry : sourceEntries) {
                const QString entryIdentity = trackedEntryIdentity(entry);
                if (!entryIdentity.isEmpty()) {
                    const int entrySize = static_cast<int>(entry.size());
                    m_renderedEntryRanges.insert(
                        entryIdentity, qMakePair(offset, offset + entrySize));
                }
                offset += static_cast<int>(entry.size()) + 1;
            }
        }
        if (wasAtTop && scrollBar) {
            scrollBar->setValue(0);
        }
        return;
    }

    // Match each whitespace-separated query term inside a single log token.
    // Keeping the subsequence within one token makes fuzzy searches forgiving
    // of punctuation (for example, "intrmfs" still finds "initramfs") while
    // preventing unrelated long entries from matching characters scattered
    // across timestamps, messages, and line breaks. A term beginning with '@'
    // is not fuzzy: it selects complete diagnostic sections by key or title.
    const QStringList terms = query.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    QStringList fuzzyTerms;
    QStringList sectionTerms;
    for (const QString &term : terms) {
        if (term.startsWith(QLatin1Char('@')) && term.size() > 1) {
            sectionTerms.append(term.mid(1));
        } else {
            fuzzyTerms.append(term);
        }
    }
    // Selecting a diagnostic section in the filter dropdown behaves exactly
    // like the matching @section search term.
    if (filterKey.startsWith(QLatin1Char('@')) && filterKey.size() > 1) {
        sectionTerms.append(filterKey.mid(1));
    }

    // A repair-workflow filter (for example File system repair) selects a set
    // of diagnostic sections plus the repair topics of that workflow; it is
    // resolved through the single logWorkflowFilterSpecs table above.
    const LogWorkflowFilterSpec *workflowFilter = logWorkflowFilterFor(filterKey);
    const QStringList workflowSections = workflowFilter
        ? QString::fromLatin1(workflowFilter->sections).split(QLatin1Char(' '), Qt::SkipEmptyParts)
        : QStringList();
    const bool sectionsSelected = !sectionTerms.isEmpty() || !workflowSections.isEmpty();

    const auto matchesTerms = [&fuzzyTerms](const QString &text) {
        if (fuzzyTerms.isEmpty()) {
            return true;
        }
        const QStringList tokens = text.toCaseFolded().split(
            QRegularExpression(QStringLiteral("[^\\p{L}\\p{N}]+")), Qt::SkipEmptyParts);
        for (const QString &term : fuzzyTerms) {
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

    const auto sectionMatches = [&sectionTerms](const QString &key, const QString &title) {
        if (sectionTerms.isEmpty()) {
            return true;
        }
        for (const QString &section : sectionTerms) {
            if (section == QStringLiteral("all")) {
                return true;
            }
            if (key.compare(section, Qt::CaseInsensitive) == 0) {
                return true;
            }
            if (!title.isEmpty() && title.contains(section, Qt::CaseInsensitive)) {
                return true;
            }
        }
        return false;
    };

    const auto workflowHasSection = [&workflowSections](const QString &key) {
        for (const QString &section : workflowSections) {
            if (key.compare(section, Qt::CaseInsensitive) == 0) {
                return true;
            }
        }
        return false;
    };

    // True when the entry's diagnostic section belongs to the current filter.
    // Without a section filter every diagnostic entry is shown (whole or as an
    // excerpt); otherwise a section must match the @section terms, the
    // workflow's section set, or both.
    const auto sectionSelected = [&](const QString &key, const QString &title) {
        if (!sectionsSelected) {
            return true;
        }
        if (!sectionTerms.isEmpty() && sectionMatches(key, title)) {
            return true;
        }
        return !workflowSections.isEmpty() && workflowHasSection(key);
    };

    const auto categoryMatches = [&filterKey](const QString &entry) {
        if (filterKey.isEmpty() || filterKey == QStringLiteral("all")
            || filterKey.startsWith(QLatin1Char('@'))) {
            return true;
        }
        const LogEntryKind kind = logEntryKind(entry);
        if (filterKey == QStringLiteral("diagnostics")) {
            return kind == LogEntryKind::Diagnostic;
        }
        if (filterKey == QStringLiteral("repairs")) {
            return kind == LogEntryKind::Repair;
        }
        if (const LogWorkflowFilterSpec *workflow = logWorkflowFilterFor(filterKey)) {
            // A workflow filter shows its diagnostic evidence and its repair
            // entries; unrelated kinds stay hidden. A workflow that owns the
            // filecopy topic additionally shows the tagged file-copy entries,
            // so host→target and target→host copy operations remain visible
            // under both the File system repair and File copy filters.
            if (kind == LogEntryKind::Diagnostic || kind == LogEntryKind::Repair) {
                return true;
            }
            const QStringList topics = QString::fromLatin1(workflow->repairTopics)
                .split(QLatin1Char(' '), Qt::SkipEmptyParts);
            return kind == LogEntryKind::FileCopy && topics.contains(QStringLiteral("filecopy"));
        }
        return true;
    };

    // Expand every selected section (dropdown filter or @section term) into
    // the repair topics that belong to it. The topics are resolved through
    // the single logSectionFilterSpecs table above.
    QSet<QString> selectedRepairTopics;
    if (!sectionTerms.isEmpty()) {
        for (const DiagnosticSpec &spec : diagnosticSpecs) {
            if (!sectionMatches(QString::fromLatin1(spec.key), QString::fromLatin1(spec.title))) {
                continue;
            }
            const QStringList topics = logRepairTopicsForSection(QString::fromLatin1(spec.key))
                .split(QLatin1Char(' '), Qt::SkipEmptyParts);
            for (const QString &topic : topics) {
                selectedRepairTopics.insert(topic);
            }
        }
    }
    if (workflowFilter) {
        const QStringList topics = QString::fromLatin1(workflowFilter->repairTopics)
            .split(QLatin1Char(' '), Qt::SkipEmptyParts);
        for (const QString &topic : topics) {
            selectedRepairTopics.insert(topic);
        }
    }

    // A section can be captured both as a dedicated framed entry and embedded
    // inside a combined Run All report. A dedicated entry wins: embedded
    // sections are only extracted for keys that have none in the current
    // view, so selecting a section never shows the same capture twice.
    QSet<QString> dedicatedSectionKeys;
    QSet<QString> extractionKeys;
    if (sectionsSelected) {
        for (const QString &entry : sourceEntries) {
            if (!categoryMatches(entry)) {
                continue;
            }
            QString entryKey;
            QString entryScope;
            if (diagnosticEntryMetadata(entry, &entryKey, &entryScope)
                && sectionSelected(entryKey, diagnosticEntryTitle(entry))) {
                dedicatedSectionKeys.insert(entryKey.toCaseFolded());
            }
        }
        for (const DiagnosticSpec &spec : diagnosticSpecs) {
            const QString key = QString::fromLatin1(spec.key);
            if (!sectionSelected(key, QString::fromLatin1(spec.title))) {
                continue;
            }
            if (!dedicatedSectionKeys.contains(key.toCaseFolded())) {
                extractionKeys.insert(key.toCaseFolded());
            }
        }
    }

    QStringList visible;
    visible.reserve(sourceEntries.size());

    // Combined reports and other diagnostic captures embed the per-key
    // sections. Show the selected embedded sections whole, exactly as they
    // were captured, and apply the remaining search terms as AND.
    const auto appendExtractedSections = [&](const QString &entry) {
        const QStringList sections = extractEmbeddedDiagnosticSections(entry, extractionKeys);
        for (const QString &section : sections) {
            if (fuzzyTerms.isEmpty() || matchesTerms(section)) {
                visible.append(section);
            }
        }
    };

    const auto appendDiagnosticExcerpt = [&](const QString &entry) {
        const QStringList lines = entry.split(QLatin1Char('\n'));
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
            return;
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

        // A combined report embeds the per-key sections. Showing only the
        // matching lines would produce a flat list of fragments whose section
        // is no longer identifiable. Keep the excerpt coherent: matches before
        // the first embedded section (the shared capability preamble) stay as
        // they are, and every embedded section that contains matches is shown
        // with its title frame and metadata once above its matching lines.
        QList<int> embeddedHeaders;
        for (int index = headerEnd + 1; index < lines.size(); ++index) {
            if (lines.at(index).startsWith(QStringLiteral("Diagnostic: "))) {
                embeddedHeaders.append(index);
            }
        }

        if (embeddedHeaders.isEmpty()) {
            for (const int match : matches) {
                included.insert(match);
            }
        } else {
            const int firstEmbeddedStart =
                embeddedSectionStartLine(lines, embeddedHeaders.constFirst());
            for (const int match : matches) {
                if (match < firstEmbeddedStart) {
                    included.insert(match);
                }
            }
            for (int position = 0; position < embeddedHeaders.size(); ++position) {
                const int header = embeddedHeaders.at(position);
                const int start = embeddedSectionStartLine(lines, header);
                const int end = position + 1 < embeddedHeaders.size()
                    ? embeddedSectionStartLine(lines, embeddedHeaders.at(position + 1))
                    : lines.size();
                // The section metadata is the "Diagnostic:" line and the
                // contiguous header lines that follow it (Scope, Time, ...),
                // ending at the blank line before the body.
                int metadataEnd = header;
                while (metadataEnd + 1 < end
                       && !lines.at(metadataEnd + 1).trimmed().isEmpty()) {
                    ++metadataEnd;
                }
                bool sectionMatched = false;
                for (const int match : matches) {
                    if (match < start || match >= end) {
                        continue;
                    }
                    if (!sectionMatched) {
                        for (int index = start; index <= metadataEnd; ++index) {
                            included.insert(index);
                        }
                        sectionMatched = true;
                    }
                    included.insert(match);
                }
            }
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
    };

    for (const QString &entry : sourceEntries) {
        if (!categoryMatches(entry)) {
            continue;
        }

        QString entryKey;
        QString entryScope;
        if (diagnosticEntryMetadata(entry, &entryKey, &entryScope)) {
            if (!sectionSelected(entryKey, diagnosticEntryTitle(entry))) {
                // A combined report is one framed entry whose key does not
                // match the selected section; its embedded per-key sections
                // are extracted instead.
                if (sectionsSelected) {
                    appendExtractedSections(entry);
                }
                continue;
            }

            // A selected section (or a pure dropdown filter) shows the
            // complete section. Fuzzy terms combine as AND: the section must
            // still match every term somewhere before it is shown whole.
            if (fuzzyTerms.isEmpty() || sectionsSelected) {
                if (matchesTerms(entry)) {
                    visible.append(entry);
                }
                continue;
            }
            appendDiagnosticExcerpt(entry);
            continue;
        }

        // Repair entries record a "Diagnostic: <title>" line without the
        // framed section metadata. A selected section includes the repair
        // entries of its related topics; without a selected section they keep
        // the legacy whole-entry or excerpt matching.
        const bool legacyDiagnosticLike = entry.contains(QStringLiteral("\nDiagnostic: "))
            || entry.contains(QStringLiteral("] Diagnostic: "));
        if (!sectionsSelected) {
            if (fuzzyTerms.isEmpty() || !legacyDiagnosticLike) {
                if (matchesTerms(entry)) {
                    visible.append(entry);
                }
            } else {
                appendDiagnosticExcerpt(entry);
            }
            continue;
        }
        const LogEntryKind entryKind = logEntryKind(entry);
        if (entryKind == LogEntryKind::Diagnostic) {
            // A diagnostic entry without the framed metadata can still carry
            // an embedded combined report.
            appendExtractedSections(entry);
            continue;
        }
        // Repair entries are matched by topic. Tagged file-copy entries join
        // the same path only when the active filter owns the filecopy topic
        // (File system repair and File copy), so copy operations are visible
        // without leaking them into unrelated section filters.
        const bool topicEntry = entryKind == LogEntryKind::Repair
            || (entryKind == LogEntryKind::FileCopy
                && selectedRepairTopics.contains(QStringLiteral("filecopy")));
        if (!topicEntry || !repairEntryMatchesTopics(entry, selectedRepairTopics)) {
            continue;
        }
        if (matchesTerms(entry)) {
            visible.append(entry);
        }
    }

    // A filter that matches nothing must still show a clear notice instead of
    // an empty document.
    if (visible.isEmpty()) {
        visible.append(QStringLiteral("(no matching entries for this filter)"));
    }

    const QString filtered = visible.join(QLatin1Char('\n'));
    m_logView->setPlainText(filtered);
    applyRepairResultColoring(filtered);
    markStaleEntries();
    m_staleHighlightFlag = m_targetDiagnosticsNeedRegeneration;
    m_staleHighlightSections = staleSections;
    m_staleHighlightValid = true;
    m_renderedLogQuery = query;
    m_renderedLogFilter = filterKey;
    m_renderedPriorLog = m_viewingPriorLog;
    m_logViewRendered = true;
    m_logModelTrimmed = false;
    m_pendingLogEntries.clear();
    m_pendingEntryRemovals.clear();
    m_renderedEntryRanges.clear();
    if (wasAtTop && scrollBar) {
        scrollBar->setValue(0);
    }
}

// The privileged helper confirms the installed root by mounting it and
// checking /etc/os-release. When the committed component lacks that evidence
// the helper resolves the real root from the other Linux-capable partitions on
// the same disk and reports the fallback with this stable sentence. Adopt the
// resolved component so the committed target, the log and every later request
// all name the same resolved system. The sentence is deliberately not emitted
// for ordinary mapper/dm-N spelling changes, so reconciliation only follows a
// real component change.
bool MainWindow::reconcileResolvedTargetComponent(const QString &output)
{
    static const QRegularExpression fallbackPattern(QStringLiteral(
        "Root component fallback: selected component (\\S+) \\([^)]*\\) lacks /etc/os-release; "
        "resolved (\\S+) \\(([^)]*)\\)"));
    const QRegularExpressionMatch match = fallbackPattern.match(output);
    if (!match.hasMatch()) {
        return false;
    }

    const QString previous = match.captured(1);
    const QString resolved = match.captured(2);
    if (resolved.isEmpty() || resolved == m_previewTargetComponentPath) {
        return false;
    }
    if (!m_deviceIndex.contains(resolved)) {
        return false;
    }

    m_previewTargetComponentPath = resolved;
    appendLog(QStringLiteral(
                  "Target root component confirmed by privileged evidence: %1 (was %2). The committed target now uses the resolved component.")
                  .arg(resolved, previous));
    updateTargetLabels();
    updateSessionScope();
    return true;
}

void MainWindow::appendDiagnosticLog(const QString &key, const QString &title,
                                     const QString &scope, const QString &result,
                                     bool succeeded)
{
    // A successful target diagnostic is privileged proof of the mounted root.
    // When the helper had to fall back from the committed component, adopt the
    // confirmed component before the captured output is indexed so the
    // committed target and later requests stay consistent.
    if (succeeded && scope == QStringLiteral("Repair Target")) {
        reconcileResolvedTargetComponent(result);
    }

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

    // A newer result supersedes the previous section for the same (key,
    // scope). Run All additionally supersedes every per-key section it
    // regenerates for the same scope, so a fresh report never leaves stale
    // per-key duplicates behind. Non-diagnostic entries are never touched.
    const QString identity = diagnosticEntryIdentity(key, scope);
    const auto removeTrackedEntry = [this](const QString &trackedIdentity) {
        const auto it = m_diagnosticLogEntries.find(trackedIdentity);
        if (it == m_diagnosticLogEntries.end()) {
            return;
        }
        const QString removedEntry = it.value();
        const int index = m_actionLogEntries.indexOf(removedEntry);
        if (index >= 0) {
            m_actionLogEntries.removeAt(index);
        }
        // Keep the deferred incremental view update in sync: an entry that was
        // only queued for rendering is dropped from the queue, while one
        // already present in the document is queued for block removal.
        const int pendingIndex = m_pendingLogEntries.lastIndexOf(removedEntry);
        if (pendingIndex >= 0) {
            m_pendingLogEntries.removeAt(pendingIndex);
        }
        if (m_renderedEntryRanges.contains(trackedIdentity)) {
            m_pendingEntryRemovals.insert(trackedIdentity);
        }
        m_diagnosticLogEntries.erase(it);
    };
    if (key == QStringLiteral("report")) {
        const QString scopePrefix = scope + QLatin1Char('\x1f');
        const QStringList trackedIdentities = m_diagnosticLogEntries.keys();
        for (const QString &trackedIdentity : trackedIdentities) {
            if (trackedIdentity.startsWith(scopePrefix)) {
                removeTrackedEntry(trackedIdentity);
            }
        }
    } else {
        removeTrackedEntry(identity);
    }

    appendLog(section, succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"),
              LogEntryKind::Diagnostic);
    m_diagnosticLogEntries.insert(identity, m_actionLogEntries.constFirst());
}

// The active host/target scope as used by repair sections and scope-sensitive
// status entries. The running host, each repair target and each filesystem
// device stay separate while the same scope re-run replaces.
QString MainWindow::currentLogScopeKey() const
{
    return m_hostMaintenanceMode
        ? QStringLiteral("host:%1:%2").arg(m_hostPrimaryPath, m_hostPrimaryComponentPath)
        : QStringLiteral("target:%1:%2").arg(m_previewTargetPath, m_previewTargetComponentPath);
}

// Repair sections live in their own namespace (the "repair" field) so a repair
// key can never collide with a diagnostic section of the same name; the scope
// keeps the running host, each repair target and each filesystem device/mode
// separate while the same tool/stage re-run on the same scope replaces.
QString MainWindow::repairLogSectionIdentity(const QString &key) const
{
    return QStringLiteral("repair\x1f%1\x1f%2").arg(currentLogScopeKey(), key);
}

void MainWindow::beginRepairLogSection(const QString &section)
{
    m_activeRepairSection = section;
    m_activeRepairEntries.clear();
}

void MainWindow::finishRepairLogSection(const QString &section)
{
    if (m_activeRepairSection != section) {
        return;
    }
    const QStringList newEntries = m_activeRepairEntries;
    m_activeRepairSection.clear();
    m_activeRepairEntries.clear();
    if (newEntries.isEmpty()) {
        return;
    }

    // Drop the previous block for the same tool/stage and scope. A repair
    // cycle's entries are appended back to back, so the old block is a
    // contiguous run in the newest-first register. The search starts after the
    // new entries (identical same-second entries must not remove the fresh
    // copy) and requires the whole run to match in order, so an identical
    // output entry from a different scope cannot be removed by mistake.
    const auto previous = m_repairLogEntries.constFind(section);
    if (previous != m_repairLogEntries.constEnd() && !previous->isEmpty()) {
        const int count = previous->size();
        for (int index = newEntries.size();
             index + count <= m_actionLogEntries.size(); ++index) {
            bool matches = true;
            for (int offset = 0; offset < count; ++offset) {
                if (m_actionLogEntries.at(index + offset)
                    != previous->at(count - 1 - offset)) {
                    matches = false;
                    break;
                }
            }
            if (!matches) {
                continue;
            }
            for (int offset = 0; offset < count; ++offset) {
                m_actionLogEntries.removeAt(index);
            }
            break;
        }
    }
    m_repairLogEntries.insert(section, newEntries);

    // The model changed outside the incremental renderer's view of the
    // document, so the next refresh rebuilds from the register. The updated
    // block keeps the newest-first position it was appended at, matching the
    // diagnostic replacement ordering.
    m_pendingLogEntries.clear();
    m_pendingEntryRemovals.clear();
    m_renderedEntryRanges.clear();
    m_logViewRendered = false;
    scheduleLogRefresh();
}

void MainWindow::rebuildDiagnosticLogIndex()
{
    // Reconstruct the (key, scope) index for the live register, for example
    // after loading a session file. Older duplicate diagnostic sections from
    // previous runs are collapsed to the newest result so the replacement
    // model holds across restarts as well.
    m_diagnosticLogEntries.clear();
    for (int index = 0; index < m_actionLogEntries.size(); ) {
        QString key;
        QString scope;
        if (!diagnosticEntryMetadata(m_actionLogEntries.at(index), &key, &scope)) {
            ++index;
            continue;
        }
        const QString identity = diagnosticEntryIdentity(key, scope);
        if (m_diagnosticLogEntries.contains(identity)) {
            m_actionLogEntries.removeAt(index);
            continue;
        }
        m_diagnosticLogEntries.insert(identity, m_actionLogEntries.at(index));
        ++index;
    }
}

void MainWindow::showUsageHelp()
{
    showCompactHelp(this,
                    QStringLiteral("Using Boot Bitch"),
                    QStringLiteral(
                        "Boot Bitch must run from a different booted Linux environment than the system being repaired. "
                        "Use a Linux live USB or another Linux installation on a different physical drive. "
                        "The running host is protected from ordinary repair-target selection, but it can be explicitly selected through Host Maintenance for guarded native diagnostics and supported maintenance stages. "
                        "Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while it is active. "
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
                           "<p>Arch-family systems expose backend profiling and guarded package, initramfs, GRUB and conventional EFI repairs when their transaction-specific preflights pass. Fedora/RPM-family systems expose guarded dnf package transactions, dracut initramfs rebuilds, GRUB2 configuration and boot-code repair, and the systemd GDM display path from the same probe evidence; other RPM-family systems remain diagnostics-only until their transaction backend is implemented.</p>"
                           "<p>The first privileged action authorizes one narrow helper session for the current Boot Bitch window while the Qt GUI remains unprivileged. It can be ended at any time from File → Lock Administrator Session.</p>"
                           "<p>LUKS target unlock, verified bidirectional File Copy, read-only host/repair diagnostics, transactional Btrfs snapshot rollback, offline graphical login recovery, distribution-aware EFI/UKI repair and boot-stack reconciliation are enabled through the guarded helper. Snapshot rollback preserves the previous @ and automatically restores it if critical post-switch reconciliation fails.</p>")
                           .arg(QCoreApplication::applicationVersion()));
}

// ---- MainWindow: responsive layout ------------------------------------------

// Every splitter pane keeps at least a fifth of the splitter's current size so
// dragging left/right or up/down can never hide a pane behind its neighbour.
// The pixel minimum scales with the window instead of a fixed row count; each
// pane's original minimum stays as its floor.
static void applyFractionalPaneMinimum(QSplitter *splitter, int skipIndex = -1)
{
    if (!splitter) {
        return;
    }
    static QHash<const QWidget *, QSize> baseMinimums;
    constexpr int kMinimumPercent = 20;
    const bool horizontal = splitter->orientation() == Qt::Horizontal;
    const int total = horizontal ? splitter->width() : splitter->height();
    if (total <= 0) {
        return;
    }
    const int minimum = qMax(48, total * kMinimumPercent / 100);
    for (int index = 0; index < splitter->count(); ++index) {
        if (index == skipIndex) {
            continue;
        }
        QWidget *pane = splitter->widget(index);
        if (!pane) {
            continue;
        }
        if (!baseMinimums.contains(pane)) {
            baseMinimums.insert(pane, QSize(pane->minimumWidth(), pane->minimumHeight()));
        }
        const QSize base = baseMinimums.value(pane);
        if (horizontal) {
            const int wanted = qMax(base.width(), minimum);
            if (pane->minimumWidth() != wanted) {
                pane->setMinimumWidth(wanted);
            }
        } else {
            const int wanted = qMax(base.height(), minimum);
            if (pane->minimumHeight() != wanted) {
                pane->setMinimumHeight(wanted);
            }
        }
    }
}

// ---- MainWindow: system color scheme ----------------------------------------

Qt::ColorScheme MainWindow::s_colorSchemeOverride = Qt::ColorScheme::Unknown;

void MainWindow::setColorSchemeOverrideForTests(Qt::ColorScheme scheme)
{
    s_colorSchemeOverride = scheme;
}

Qt::ColorScheme MainWindow::colorSchemeOverrideForTests()
{
    return s_colorSchemeOverride;
}

// The platform/test hint alone. It is not authoritative when the desktop
// evidence disagrees; resolvedColorScheme() applies that precedence.
Qt::ColorScheme MainWindow::effectiveColorScheme()
{
    if (s_colorSchemeOverride != Qt::ColorScheme::Unknown) {
        return s_colorSchemeOverride;
    }
    if (QGuiApplication::styleHints()) {
        return QGuiApplication::styleHints()->colorScheme();
    }
    return Qt::ColorScheme::Unknown;
}

// Maps a portal Read/SettingChanged value to a scheme. GNOME's Settings portal
// wraps the payload in one or two variants depending on the namespace (the
// GNOME namespace returns the string "prefer-dark", the cross-desktop
// appearance key returns 0/1/2), so every variant layer is unwrapped before
// the payload is classified.
Qt::ColorScheme MainWindow::colorSchemeFromPortalValue(const QVariant &value)
{
    QVariant inner = value;
#ifdef BOOT_REPAIR_HAVE_DBUS
    while (inner.metaType() == QMetaType::fromType<QDBusVariant>()) {
        inner = inner.value<QDBusVariant>().variant();
    }
#else
    Q_UNUSED(inner);
    return Qt::ColorScheme::Unknown;
#endif
    if (inner.userType() == QMetaType::QString) {
        const QString text = inner.toString().trimmed().toLower();
        if (text == QStringLiteral("dark") || text == QStringLiteral("prefer-dark")) {
            return Qt::ColorScheme::Dark;
        }
        if (text == QStringLiteral("light") || text == QStringLiteral("prefer-light")) {
            return Qt::ColorScheme::Light;
        }
        return Qt::ColorScheme::Unknown;
    }
    bool ok = false;
    const uint code = inner.toUInt(&ok);
    if (!ok) {
        return Qt::ColorScheme::Unknown;
    }
    if (code == 1) {
        return Qt::ColorScheme::Dark;
    }
    if (code == 2) {
        return Qt::ColorScheme::Light;
    }
    return Qt::ColorScheme::Unknown;
}

// The scheme the application should render. Desktop evidence (the XDG portal,
// then the GNOME gsettings value) wins over QStyleHints when the two disagree:
// Qt auto-loads the GTK3 platform theme on a GNOME session, and that theme
// reports Adwaita's light scheme even when org.gnome.desktop.interface
// color-scheme is prefer-dark (verified on Fedora 44 / GNOME 50). With no
// desktop evidence the platform hint is used; the native palette is the last
// resort before light.
Qt::ColorScheme MainWindow::resolvedColorScheme(ColorSchemeSource *source) const
{
    if (s_colorSchemeOverride != Qt::ColorScheme::Unknown) {
        if (source) {
            *source = ColorSchemeSource::TestOverride;
        }
        return s_colorSchemeOverride;
    }
    if (m_portalColorScheme != Qt::ColorScheme::Unknown) {
        if (source) {
            *source = ColorSchemeSource::Portal;
        }
        return m_portalColorScheme;
    }
    if (m_gsettingsColorScheme != Qt::ColorScheme::Unknown) {
        if (source) {
            *source = ColorSchemeSource::GSettings;
        }
        return m_gsettingsColorScheme;
    }
    const Qt::ColorScheme platform = effectiveColorScheme();
    if (platform != Qt::ColorScheme::Unknown) {
        if (source) {
            *source = ColorSchemeSource::Platform;
        }
        return platform;
    }
    if (source) {
        *source = ColorSchemeSource::Palette;
    }
    return QApplication::palette().color(QPalette::Window).lightness() < 128
        ? Qt::ColorScheme::Dark
        : Qt::ColorScheme::Light;
}

namespace {

// Fallback palettes used only when the platform palette does not match the
// desktop color scheme (for example a GNOME dark session without a Qt
// platform theme that applies it). Roles not named here keep the style's
// standard values, so native widget details stay intact.
QPalette fallbackColorSchemePalette(bool dark)
{
    QPalette palette = QApplication::style() ? QApplication::style()->standardPalette() : QPalette();
    if (!dark) {
        // Explicit light roles so a style whose standard palette is dark can
        // never keep the application dark when the desktop scheme is light.
        const QColor window(0xf0, 0xf0, 0xf0);
        const QColor base(0xff, 0xff, 0xff);
        const QColor alternate(0xe9, 0xe9, 0xe9);
        const QColor text(0x1a, 0x1a, 0x1a);
        const QColor button(0xf0, 0xf0, 0xf0);
        const QColor disabled(0x9f, 0x9f, 0x9f);
        palette.setColor(QPalette::Window, window);
        palette.setColor(QPalette::WindowText, text);
        palette.setColor(QPalette::Base, base);
        palette.setColor(QPalette::AlternateBase, alternate);
        palette.setColor(QPalette::ToolTipBase, QColor(0xff, 0xff, 0xdc));
        palette.setColor(QPalette::ToolTipText, text);
        palette.setColor(QPalette::Text, text);
        palette.setColor(QPalette::Button, button);
        palette.setColor(QPalette::ButtonText, text);
        palette.setColor(QPalette::BrightText, QColor(0xff, 0x00, 0x00));
        palette.setColor(QPalette::Link, QColor(0x00, 0x66, 0xcc));
        palette.setColor(QPalette::LinkVisited, QColor(0x80, 0x00, 0x80));
        palette.setColor(QPalette::Highlight, QColor(0x30, 0x7a, 0xd6));
        palette.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
        palette.setColor(QPalette::PlaceholderText, QColor(0x80, 0x80, 0x80));
        palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
        palette.setColor(QPalette::Disabled, QPalette::Base, window);
        palette.setColor(QPalette::Disabled, QPalette::Button, window);
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(0xbc, 0xbc, 0xbc));
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
        return palette;
    }

    const QColor window(0x2b, 0x2b, 0x2b);
    const QColor base(0x1e, 0x1e, 0x1e);
    const QColor alternate(0x26, 0x26, 0x26);
    const QColor text(0xe6, 0xe6, 0xe6);
    const QColor button(0x35, 0x35, 0x35);
    const QColor disabled(0x80, 0x80, 0x80);

    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::WindowText, text);
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::AlternateBase, alternate);
    palette.setColor(QPalette::ToolTipBase, alternate);
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::ButtonText, text);
    palette.setColor(QPalette::BrightText, QColor(0xff, 0x5c, 0x5c));
    palette.setColor(QPalette::Link, QColor(0x79, 0xc0, 0xff));
    palette.setColor(QPalette::LinkVisited, QColor(0xbc, 0x8c, 0xff));
    palette.setColor(QPalette::Highlight, QColor(0x2a, 0x82, 0xda));
    palette.setColor(QPalette::HighlightedText, QColor(0xff, 0xff, 0xff));
    palette.setColor(QPalette::PlaceholderText, QColor(0x9a, 0x9a, 0x9a));
    palette.setColor(QPalette::Light, QColor(0x3c, 0x3c, 0x3c));
    palette.setColor(QPalette::Midlight, QColor(0x33, 0x33, 0x33));
    palette.setColor(QPalette::Dark, QColor(0x16, 0x16, 0x16));
    palette.setColor(QPalette::Mid, QColor(0x20, 0x20, 0x20));
    palette.setColor(QPalette::Shadow, QColor(0x0f, 0x0f, 0x0f));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    palette.setColor(QPalette::Disabled, QPalette::Base, alternate);
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor(0x30, 0x30, 0x30));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(0x3c, 0x3c, 0x3c));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(0x8c, 0x8c, 0x8c));
    return palette;
}

// Human-readable scheme name used by the startup/change log line.
QString colorSchemeName(Qt::ColorScheme scheme)
{
    switch (scheme) {
    case Qt::ColorScheme::Dark:
        return QStringLiteral("dark");
    case Qt::ColorScheme::Light:
        return QStringLiteral("light");
    default:
        return QStringLiteral("unknown");
    }
}

#ifdef BOOT_REPAIR_HAVE_DBUS
// One synchronous portal read with a short timeout. GNOME's settings namespace
// is tried first and the cross-desktop appearance key second, mirroring the
// asynchronous fallback chain. The timeout only bounds a hung portal; a
// healthy one answers in a few milliseconds.
Qt::ColorScheme readPortalColorScheme()
{
    const QDBusConnection bus = QDBusConnection::sessionBus();
    const auto read = [&bus](const QString &nameSpace) {
        QDBusMessage call = QDBusMessage::createMethodCall(
            QStringLiteral("org.freedesktop.portal.Desktop"),
            QStringLiteral("/org/freedesktop/portal/desktop"),
            QStringLiteral("org.freedesktop.portal.Settings"),
            QStringLiteral("Read"));
        call << nameSpace << QStringLiteral("color-scheme");
        const QDBusMessage reply = bus.call(call, QDBus::Block, 250);
        if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
            return Qt::ColorScheme::Unknown;
        }
        return MainWindow::colorSchemeFromPortalValue(reply.arguments().constFirst());
    };
    Qt::ColorScheme scheme = read(QStringLiteral("org.gnome.desktop.interface"));
    if (scheme == Qt::ColorScheme::Unknown) {
        scheme = read(QStringLiteral("org.freedesktop.appearance"));
    }
    return scheme;
}
#endif

} // namespace

// Applies the resolved desktop color scheme to the application palette
// whenever the current palette does not already match it. A matching native
// palette (for example Breeze dark on TUXEDO OS, Adwaita dark on Fedora, or
// any KDE/GNOME light session) is left untouched, so native styling keeps
// working; only the mismatched light-on-dark / dark-on-light case is
// corrected. Every custom color derived from the palette is re-rendered
// afterwards. Once the palette already matches the resolved scheme the call
// is a no-op, so the deferred startup re-checks never double-apply or flicker.
void MainWindow::applyColorScheme()
{
    ColorSchemeSource source = ColorSchemeSource::Palette;
    const Qt::ColorScheme scheme = resolvedColorScheme(&source);
    const bool wantDark = scheme == Qt::ColorScheme::Dark;
    const bool paletteMatches =
        (QApplication::palette().color(QPalette::Window).lightness() < 128) == wantDark;
    if (!paletteMatches) {
        m_applyingColorScheme = true;
        QApplication::setPalette(fallbackColorSchemePalette(wantDark));
        m_applyingColorScheme = false;
    }
    if (!paletteMatches || scheme != m_lastAppliedColorScheme) {
        m_lastAppliedColorScheme = scheme;
        updateThemeDependentColors();
    }
    logColorSchemeResolution(scheme, source);
}

// One application-log + stderr line per scheme/source change. The line names
// the platform hint and every desktop-evidence value that was consulted, so a
// light launch under a dark desktop can be diagnosed from the log alone.
void MainWindow::logColorSchemeResolution(Qt::ColorScheme scheme, ColorSchemeSource source)
{
    const QString schemeName = colorSchemeName(scheme);
    QString sourceName;
    switch (source) {
    case ColorSchemeSource::TestOverride:
        sourceName = QStringLiteral("test override");
        break;
    case ColorSchemeSource::Platform:
        sourceName = QStringLiteral("QStyleHints");
        break;
    case ColorSchemeSource::Portal:
        sourceName = QStringLiteral("xdg portal");
        break;
    case ColorSchemeSource::GSettings:
        sourceName = QStringLiteral("gsettings");
        break;
    case ColorSchemeSource::Palette:
        sourceName = QStringLiteral("application palette");
        break;
    }
    const QString key = schemeName + QLatin1Char('|') + sourceName;
    if (key == m_lastLoggedColorScheme) {
        return;
    }
    m_lastLoggedColorScheme = key;
    QString detail = QStringLiteral("platform hint: %1").arg(colorSchemeName(effectiveColorScheme()));
    if (m_portalColorScheme != Qt::ColorScheme::Unknown) {
        detail += QStringLiteral(", portal: %1").arg(colorSchemeName(m_portalColorScheme));
    }
    if (m_gsettingsColorScheme != Qt::ColorScheme::Unknown) {
        detail += QStringLiteral(", gsettings: %1").arg(colorSchemeName(m_gsettingsColorScheme));
    }
    const QString message = QStringLiteral("Desktop color scheme: %1 (source: %2; %3)")
                                .arg(schemeName, sourceName, detail);
    appendLog(message, QStringLiteral("INFO"));
    qInfo().noquote() << "[boot-repair]" << message;
}

// Coalesces a deferred scheme re-resolution into one event-loop turn so the
// platform theme has a chance to publish its value without bursts of work.
void MainWindow::scheduleColorSchemeRefresh()
{
    if (m_colorSchemeRefreshScheduled) {
        return;
    }
    m_colorSchemeRefreshScheduled = true;
    QTimer::singleShot(0, this, [this] {
        m_colorSchemeRefreshScheduled = false;
        refreshColorScheme();
    });
}

// Re-resolves the desktop color scheme after startup. The platform palette
// can be replaced after the first paint and the portal reply can arrive late,
// so every deferred check re-applies the best available evidence.
void MainWindow::refreshColorScheme()
{
    queryPortalColorScheme();
    applyColorScheme();
}

// Resolves the desktop scheme through the XDG portal. The initial read is
// synchronous with a short timeout because the platform palette (and therefore
// the first paint) is already fixed before the event loop runs: on GNOME the
// auto-loaded GTK3 platform theme reports Adwaita light in a dark session, so
// waiting for an asynchronous reply would show a light first frame. The
// SettingChanged subscription keeps the value current afterwards.
void MainWindow::queryPortalColorScheme()
{
#ifdef BOOT_REPAIR_HAVE_DBUS
    if (m_portalColorSchemeQueryStarted) {
        return;
    }
    // Offscreen/minimal platforms have no desktop portal: skip so the test
    // suite never depends on the developer's session settings.
    const QString platform = QGuiApplication::platformName();
    if (platform.startsWith(QStringLiteral("offscreen"))
        || platform.startsWith(QStringLiteral("minimal"))) {
        return;
    }
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        return;
    }
    m_portalColorSchemeQueryStarted = true;

    const Qt::ColorScheme scheme = readPortalColorScheme();
    if (scheme != Qt::ColorScheme::Unknown) {
        m_portalColorScheme = scheme;
    } else {
        // A portal that was slow or not yet up still gets an asynchronous
        // chance; the gsettings fallback covers the case where it stays mute.
        requestPortalColorScheme(QStringLiteral("org.gnome.desktop.interface"),
                                 QStringLiteral("color-scheme"));
    }
    bus.connect(QStringLiteral("org.freedesktop.portal.Desktop"),
                QStringLiteral("/org/freedesktop/portal/desktop"),
                QStringLiteral("org.freedesktop.portal.Settings"),
                QStringLiteral("SettingChanged"),
                this, SLOT(portalSettingsChanged()));
#endif
}

// One asynchronous org.freedesktop.portal.Settings.Read call. GNOME's own
// settings namespace is tried first; when it yields no usable value the
// cross-desktop appearance key is asked once as a fallback, and gsettings
// afterwards.
void MainWindow::requestPortalColorScheme(const QString &settingsNamespace, const QString &key)
{
#ifdef BOOT_REPAIR_HAVE_DBUS
    QDBusMessage call = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        QStringLiteral("/org/freedesktop/portal/desktop"),
        QStringLiteral("org.freedesktop.portal.Settings"),
        QStringLiteral("Read"));
    call << settingsNamespace << key;
    auto *watcher = new QDBusPendingCallWatcher(
        QDBusConnection::sessionBus().asyncCall(call), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [this, watcher, settingsNamespace] {
        watcher->deleteLater();
        const QDBusMessage reply = watcher->reply();
        if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
            const Qt::ColorScheme scheme =
                colorSchemeFromPortalValue(reply.arguments().constFirst());
            if (scheme != Qt::ColorScheme::Unknown) {
                m_portalColorScheme = scheme;
                applyColorScheme();
                return;
            }
        }
        if (settingsNamespace == QStringLiteral("org.gnome.desktop.interface")) {
            requestPortalColorScheme(QStringLiteral("org.freedesktop.appearance"),
                                     QStringLiteral("color-scheme"));
        } else {
            queryGsettingsColorScheme();
        }
    });
#else
    Q_UNUSED(settingsNamespace);
    Q_UNUSED(key);
#endif
}

// Runtime appearance switches. GNOME emits SettingChanged for
// org.freedesktop.appearance/color-scheme (and its own namespace); the
// platform theme may emit no ThemeChange at all, so this is the only signal
// that keeps a running window in sync with the desktop preference. The value
// is re-read through the normal asynchronous portal chain instead of being
// trusted from the signal payload.
void MainWindow::portalSettingsChanged()
{
#ifdef BOOT_REPAIR_HAVE_DBUS
    requestPortalColorScheme(QStringLiteral("org.gnome.desktop.interface"),
                             QStringLiteral("color-scheme"));
#endif
}

// Read-only gsettings fallback for sessions whose portal never answers. The
// key is the same one GNOME Settings writes ("prefer-dark"/"prefer-light");
// the child inherits the session environment, so no address is synthesized.
// The short timeout bounds a hung settings daemon, and a missing gsettings
// (KDE, minimal systems) is simply ignored.
void MainWindow::queryGsettingsColorScheme()
{
    if (m_gsettingsColorSchemeQueryStarted) {
        return;
    }
    const QString platform = QGuiApplication::platformName();
    if (platform.startsWith(QStringLiteral("offscreen"))
        || platform.startsWith(QStringLiteral("minimal"))) {
        return;
    }
    m_gsettingsColorSchemeQueryStarted = true;
    QProcess process;
    process.start(QStringLiteral("gsettings"),
                  {QStringLiteral("get"), QStringLiteral("org.gnome.desktop.interface"),
                   QStringLiteral("color-scheme")});
    if (!process.waitForStarted(500)) {
        return;
    }
    if (!process.waitForFinished(1500)) {
        process.kill();
        process.waitForFinished(200);
        return;
    }
    const QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed().toLower();
    Qt::ColorScheme scheme = Qt::ColorScheme::Unknown;
    if (output.contains(QStringLiteral("dark"))) {
        scheme = Qt::ColorScheme::Dark;
    } else if (output.contains(QStringLiteral("light"))) {
        scheme = Qt::ColorScheme::Light;
    }
    if (scheme != Qt::ColorScheme::Unknown) {
        m_gsettingsColorScheme = scheme;
        applyColorScheme();
    }
}

// Re-renders every widget whose appearance is derived from palette colors
// rather than the widget palette itself: log text/selection and stale
// highlighting, repair-result glyph colors, the committed-target highlight
// and the themed plan icons.
void MainWindow::updateThemeDependentColors()
{
    if (m_logView) {
        // Force a full rebuild: the incremental path only colors newly
        // prepended entries, so already-rendered repair-result glyphs would
        // keep the previous scheme's colors.
        m_staleHighlightValid = false;
        m_logViewRendered = false;
        refreshLogView();
    }
    if (m_deviceTree) {
        updateCommittedTargetVisual();
    }
    if (m_fullRepairStageList) {
        updateFullRepairSummary();
    }
}

// Reflows every page and splitter for the current window width: responsive tab
// labels, splitter orientations and pane minimums, and the running-host card
// layout. Called from resizeEvent() and after construction.
void MainWindow::updateResponsiveLayout()
{
    const int availableWidth = centralWidget() ? centralWidget()->width() : width();
    const bool narrowSystems = availableWidth < 900;
    const bool narrowDiagnostics = availableWidth < 820;
    const bool narrowRepair = availableWidth < 820;
    const bool narrowSnapshots = availableWidth < 700;
    const bool narrowLogs = availableWidth < 820;

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
        // The Shell tab follows the active scope (Host Shell vs Chroot Shell)
        // at the wide breakpoint; re-apply it after the responsive labels.
        updateChrootShellMode();
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

    if (m_hostCardLayout) {
        // Stack the running-system identity above its actions on narrow
        // windows so the OS/storage text keeps the full card width instead of
        // wrapping word by word beside the buttons.
        const QBoxLayout::Direction desired = narrowSystems
            ? QBoxLayout::TopToBottom
            : QBoxLayout::LeftToRight;
        if (m_hostCardLayout->direction() != desired) {
            m_hostCardLayout->setDirection(desired);
        }
    }

    if (m_hostActionsLayout && m_hostProtectedBadge && m_hostDetailsButton
        && m_hostMaintenanceButton && m_hostDefaultButton
        && m_hostActionsStacked != narrowSystems) {
        // Narrow: badge and Details on the first action row, the maintenance
        // actions on the second, both rows right-anchored to the card's
        // trailing edge (a leading stretch column absorbs the slack) so the
        // buttons keep the same furthest-right anchor instead of sitting at
        // fixed positions. Wide: one action row beside the identity.
        auto place = [this](QWidget *widget, int row, int column) {
            m_hostActionsLayout->removeWidget(widget);
            m_hostActionsLayout->addWidget(widget, row, column, Qt::AlignRight | Qt::AlignTop);
        };
        if (narrowSystems) {
            m_hostActionsLayout->setColumnStretch(0, 1);
            m_hostActionsLayout->setColumnStretch(1, 0);
            m_hostActionsLayout->setColumnStretch(2, 0);
            place(m_hostProtectedBadge, 0, 1);
            place(m_hostDetailsButton, 0, 2);
            place(m_hostMaintenanceButton, 1, 1);
            place(m_hostDefaultButton, 1, 2);
        } else {
            m_hostActionsLayout->setColumnStretch(0, 0);
            m_hostActionsLayout->setColumnStretch(1, 0);
            m_hostActionsLayout->setColumnStretch(2, 0);
            place(m_hostProtectedBadge, 0, 0);
            place(m_hostDetailsButton, 0, 1);
            place(m_hostMaintenanceButton, 0, 2);
            place(m_hostDefaultButton, 0, 3);
        }
        m_hostActionsStacked = narrowSystems;
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
            m_repairSplitter->setStretchFactor(0, 3);
            m_repairSplitter->setStretchFactor(1, 2);
            if (changed) {
                m_repairSplitter->setSizes({650, 390});
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

    if (m_sessionLogSplitter && m_sessionLogPanel && m_sessionLogViewer) {
        const Qt::Orientation desired = narrowLogs ? Qt::Vertical : Qt::Horizontal;
        const bool changed = m_sessionLogSplitter->orientation() != desired;
        if (changed) {
            m_sessionLogSplitter->setOrientation(desired);
            // Narrow windows stack the log view above the session selection
            // frame so the log stays readable; wider windows keep the
            // selection frame on the left. insertWidget() moves the existing
            // widgets, so the frames stay visible and non-collapsible.
            if (desired == Qt::Vertical) {
                m_sessionLogSplitter->insertWidget(0, m_sessionLogViewer);
                m_sessionLogSplitter->insertWidget(1, m_sessionLogPanel);
            } else {
                m_sessionLogSplitter->insertWidget(0, m_sessionLogPanel);
                m_sessionLogSplitter->insertWidget(1, m_sessionLogViewer);
            }
        }
        if (desired == Qt::Vertical) {
            m_sessionLogSplitter->setStretchFactor(0, 1);
            m_sessionLogSplitter->setStretchFactor(1, 0);
            if (changed) {
                m_sessionLogSplitter->setSizes({260, 160});
            }
        } else {
            m_sessionLogSplitter->setStretchFactor(0, 0);
            m_sessionLogSplitter->setStretchFactor(1, 1);
            if (changed) {
                m_sessionLogSplitter->setSizes({280, 800});
            }
        }
    }

    // Relative pane minimums for every splitter: each pane keeps at least a
    // fifth of the current splitter size, so dragging in any direction never
    // hides a pane. The tools pane of the Repair splitter keeps its explicit
    // minimum height instead.
    applyFractionalPaneMinimum(m_systemSplitter);
    applyFractionalPaneMinimum(m_diagnosticSplitter);
    applyFractionalPaneMinimum(m_repairSplitter);
    applyFractionalPaneMinimum(m_snapshotSplitter);
    applyFractionalPaneMinimum(m_sessionLogSplitter);
    applyFractionalPaneMinimum(m_repairVerticalSplitter, 1);
}

void MainWindow::updateFullRepairPlanHeight()
{
    if (!m_fullRepairStageList) {
        return;
    }

    // The plan list keeps a few stage rows visible at minimum; the vertical
    // splitter owns the current height, so the divider can grow the list up to
    // its content or shrink it back to this minimum and the list scrolls past
    // whatever does not fit.
    constexpr int kMinVisibleRows = 3;

    const int rowCount = m_fullRepairStageList->count();
    const int fallbackRowHeight = qMax(28, m_fullRepairStageList->fontMetrics().lineSpacing() + 12);
    int rowHeight = 0;
    for (int row = 0; row < rowCount; ++row) {
        rowHeight = qMax(rowHeight, m_fullRepairStageList->sizeHintForRow(row));
    }
    if (rowHeight <= 0) {
        rowHeight = fallbackRowHeight;
    }

    // A word-wrapped QLabel reports a size hint measured at a much narrower
    // width than it is actually given, which would keep the plan frame taller
    // than its content. Pin the readiness hint to its real wrapped height at
    // the current width so the frame stays content-sized.
    if (m_fullRepairReadinessLabel && m_fullRepairReadinessLabel->width() > 0) {
        const int readinessHeight = m_fullRepairReadinessLabel->heightForWidth(
            m_fullRepairReadinessLabel->width());
        if (readinessHeight > 0 && m_fullRepairReadinessLabel->minimumHeight() != readinessHeight) {
            m_fullRepairReadinessLabel->setFixedHeight(readinessHeight);
        }
    }

    const int frame = m_fullRepairStageList->frameWidth() * 2;
    const QMargins margins = m_fullRepairStageList->contentsMargins();
    const int minimumHeight = kMinVisibleRows * rowHeight + frame + margins.top() + margins.bottom();
    if (m_fullRepairStageList->minimumHeight() != minimumHeight) {
        m_fullRepairStageList->setMinimumHeight(minimumHeight);
    }

    // Until the user moves the divider, start the plan pane at its minimum so
    // the tools panes keep the slack; afterwards the splitter position wins
    // and no height is forced on the list.
    if (!m_repairPlanSplitterUserAdjusted && m_repairVerticalSplitter && m_fullRepairPlanBox) {
        const int planMinimum = m_fullRepairPlanBox->minimumSizeHint().height();
        if (planMinimum > 0) {
            m_repairVerticalSplitter->setSizes({planMinimum, 100000});
        }
    }
}

// Rebuilds the Full Repair plan list and every repair control from the current
// Settings selection and the cached diagnostic evidence. This is the single
// place where plan stages, individual tools and the Run Full Repair gate are
// refreshed after a selection, evidence or scope change.
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

    QList<StageEntry> stageEntries = {
        {m_fullRepairFilesystem, QStringLiteral("Repair file system errors"), QStringLiteral("filesystem"), QStringLiteral("filesystem")},
        {m_fullRepairDpkg, QStringLiteral("Complete interrupted package configuration"), QStringLiteral("dialog-ok-apply"), QStringLiteral("dpkg")},
        {m_fullRepairBrokenPackages, QStringLiteral("Repair broken package dependencies"), QStringLiteral("dialog-ok-apply"), QStringLiteral("fixbroken")},
        {m_fullRepairAptUpdate, QStringLiteral("Refresh package metadata"), QStringLiteral("view-refresh"), QStringLiteral("aptupdate")},
        {m_fullRepairUpgrade, QStringLiteral("Upgrade installed packages"), QStringLiteral("system-software-update"), QStringLiteral("upgrade")},
        {m_fullRepairDkms, QStringLiteral("Rebuild DKMS modules"), QStringLiteral("applications-development"), QStringLiteral("dkms")},
        {m_fullRepairDisplayManager, QStringLiteral("Restore detected graphical login manager"), QStringLiteral("video-display"), QStringLiteral("display")},
        {m_fullRepairInitramfs, QStringLiteral("Rebuild initramfs after mapper/crypttab validation"), QStringLiteral("initramfs"), QStringLiteral("initramfs")},
        {m_fullRepairEfi, QStringLiteral("Reinstall EFI bootloader"), QStringLiteral("drive-removable-media"), QStringLiteral("efi")},
        {m_fullRepairGrub, QStringLiteral("Update GRUB configuration"), QStringLiteral("grub"), QStringLiteral("grub")},
        {m_fullRepairExtlinux, QStringLiteral("Update extlinux configuration"), QStringLiteral("grub"), QStringLiteral("extlinux")}
    };

    // An Alpine UEFI GRUB target uses Alpine's own guarded EFI path
    // (grub-install --no-nvram plus the ESP fallback copy), so the plan names
    // that backend instead of the generic Debian/Arch wording.  The labels are
    // derived from the same cached probe lines as every other backend label.
    const bool alpineGrubUefi = detectedPackageManagers().contains(QStringLiteral("apk"))
        && detectedBootloaderBackend().contains(QStringLiteral("grub"), Qt::CaseInsensitive);
    if (alpineGrubUefi) {
        for (StageEntry &entry : stageEntries) {
            if (entry.check == m_fullRepairEfi) {
                entry.title = QStringLiteral("Reinstall Alpine GRUB EFI loader");
            } else if (entry.check == m_fullRepairGrub) {
                entry.title = QStringLiteral("Regenerate Alpine GRUB configuration");
            }
        }
    }
    // Fedora's GRUB2/dracut backends name themselves in the plan exactly like
    // the Alpine stages above; the stage order (upgrade 40 -> initramfs 70 ->
    // grub 90) is unchanged.
    if (detectedGrub2Backend()) {
        for (StageEntry &entry : stageEntries) {
            if (entry.check == m_fullRepairGrub) {
                entry.title = QStringLiteral("Update GRUB2 configuration");
            } else if (entry.check == m_fullRepairEfi) {
                entry.title = QStringLiteral("Reinstall GRUB2 bootloader");
            }
        }
    }
    if (detectedInitramfsBackend().compare(QStringLiteral("dracut"), Qt::CaseInsensitive) == 0) {
        for (StageEntry &entry : stageEntries) {
            if (entry.check == m_fullRepairInitramfs) {
                entry.title = QStringLiteral("Rebuild initramfs (dracut)");
            }
        }
    }

    m_fullRepairStageList->clear();
    int selectedCount = 0;
    QString excludedReason;

    for (const StageEntry &entry : stageEntries) {
        if (!entry.check || !entry.check->isChecked()) continue;
        QString availabilityReason;
        // The cached capability evidence gates the stage. The checkbox's own
        // enabled state is presentation only and is refreshed by
        // updateRepairScopeControls() above; relying on it here could exclude
        // a checked stage whose backend became available through an
        // individual diagnostic before the next summary refresh.
        if (!repairToolAvailable(entry.diagnosticKey, &availabilityReason)) {
            if (excludedReason.isEmpty()) excludedReason = availabilityReason;
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

    updateFullRepairPlanHeight();

    if (m_runFullRepairButton) {
        QString reason;
        const bool targetReady = m_hostMaintenanceMode
            ? hostMaintenanceReady(&reason)
            : repairTargetReady(&reason);
        QString diagnosticReason;
        bool diagnosticsReady = targetReady;
        if (diagnosticsReady) {
            for (const StageEntry &entry : stageEntries) {
                if (!entry.check || !entry.check->isChecked()
                    || !repairToolAvailable(entry.diagnosticKey)) continue;
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
                readiness = excludedReason.isEmpty()
                    ? QStringLiteral("No repair stages are selected. Use Configure Plan… to choose the stages Full Repair will run.")
                    : QStringLiteral("No selected stages are available. %1").arg(excludedReason);
            } else if (!targetReady) {
                readiness = reason;
            } else if (!diagnosticsReady) {
                readiness = diagnosticReason;
                if (diagnosticsInvalidationPending()
                    && !readiness.contains(QStringLiteral("regenerate diagnostics"), Qt::CaseInsensitive)) {
                    readiness += QStringLiteral(" Please regenerate diagnostics before starting another repair.");
                }
            } else {
                readiness = QStringLiteral("Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.");
            }
            m_fullRepairReadinessLabel->setText(readiness);
        }
        if (selectedCount == 0) {
            m_runFullRepairButton->setToolTip(excludedReason.isEmpty()
                ? QStringLiteral("No Full Repair stages are selected.") : excludedReason);
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
                if (!check) {
                    return QStringLiteral("Disabled in Settings");
                }
                // The setting decides inclusion, not the widget's enablement:
                // a checked stage whose capability became available is
                // reported as enabled even before the next enablement pass,
                // and an unchecked one names how to include it.
                return check->isChecked()
                    ? QStringLiteral("Enabled in Settings")
                    : QStringLiteral("Disabled in Settings — enable it to include this stage");
            };

            if (key == QStringLiteral("validate")) {
                status = QStringLiteral("Always preflight");
            } else if (key == QStringLiteral("filesystem")) {
                status = planStatus(m_fullRepairFilesystem);
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
            } else if (key == QStringLiteral("extlinux")) {
                status = planStatus(m_fullRepairExtlinux);
            } else if (key == QStringLiteral("bootstack")) {
                status = QStringLiteral("Manual recovery tool");
            }

            QString availabilityReason;
            if (!repairToolAvailable(key, &availabilityReason)) {
                status = QStringLiteral("Unavailable: %1").arg(availabilityReason);
            }
            item->setText(1, status);
            item->setToolTip(1, status);
        }
        // Statuses are part of the sortable Full Repair column; keep an active
        // header sort applied after the text changed. With no active header
        // sort the curated workflow order is left untouched.
        applyActiveSort(m_repairToolTree, -1, Qt::AscendingOrder);
    }

    // Host diagnostics refresh the same evidence the host default entry gate
    // reads, so re-evaluate that action from the single truth log here.
    updateHostDefaultButtonState();

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
    // Tool descriptions follow the same detected backends as the stage
    // labels: the rpm/dnf, dracut, GRUB2 and GDM wording appears only when the
    // cached probe evidence names that backend.
    const bool rpmBackend = detectedRpmBackend();
    const bool dracutBackend = detectedInitramfsBackend().compare(QStringLiteral("dracut"), Qt::CaseInsensitive) == 0;
    const bool grub2Backend = detectedGrub2Backend();
    const QString displayManagerName = detectedDisplayManagerName();

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
            "Repair package dependencies in the running host or selected repair system after the mandatory safety preflight. Debian/Ubuntu uses APT; Arch uses one sandbox-preflighted full pacman transaction; Alpine uses apk fix with a simulation first. This maps directly to Settings → Repair broken package dependencies.");
        if (rpmBackend) {
            description += QStringLiteral(
                " Fedora/RPM systems use rpm verification to find missing or corrupt package files and restore them with one simulated dnf reinstall transaction; dependency problems reported by dnf check are shown but never auto-removed.");
        }
        buttonText = QStringLiteral("Repair Dependencies");
        iconName = QStringLiteral("dialog-ok-apply");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("aptupdate")) {
        title = QStringLiteral("Refresh package metadata");
        description = QStringLiteral(
            "Refresh Debian/Ubuntu APT metadata in the running host or selected repair system without upgrading installed packages. Arch and Alpine deliberately refuse a partial metadata-only transaction; their guarded upgrade refreshes the package index itself. This maps directly to Settings → Refresh package metadata.");
        if (rpmBackend) {
            description += QStringLiteral(
                " Fedora refreshes dnf metadata (dnf makecache) as its standalone metadata stage; the cache write is always reported as changed.");
        }
        buttonText = QStringLiteral("Refresh Metadata");
        iconName = QStringLiteral("view-refresh");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("upgrade")) {
        title = QStringLiteral("Upgrade installed packages");
        description = QStringLiteral(
            "Simulate the distribution's package transaction first, inspect proposed removals, then apply a safe upgrade. Debian/Ubuntu chooses an APT mode; Arch runs one full pacman transaction; Alpine runs one guarded apk upgrade transaction. This maps directly to Settings → Upgrade installed packages.");
        if (rpmBackend) {
            description += QStringLiteral(
                " Fedora runs one guarded dnf upgrade transaction: the simulated transaction must be removal- and downgrade-free, signature-checked and bounded before the exact same command is applied.");
        }
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
            "Restore the display manager identified from the running host or selected repair system's boot evidence and configuration (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), set graphical.target as the default, and repair display-manager.service. ");
        if (displayManagerName == QStringLiteral("GDM")) {
            description += QStringLiteral(
                "The detected backend is GDM (Fedora naming); its configuration lives in /etc/gdm/custom.conf. ");
        } else if (displayManagerName == QStringLiteral("GDM3")) {
            description += QStringLiteral(
                "The detected backend is GDM3; its configuration lives in /etc/gdm3/daemon.conf. ");
        } else if (!displayManagerName.isEmpty()) {
            description += QStringLiteral("The detected backend is %1. ").arg(displayManagerName);
        }
        description += QStringLiteral(
            "When OpenRC is the detected service manager, the same action restores the detected manager's default runlevel symlink without starting it. "
            "Boot Bitch deliberately does not start a graphical session inside a repair chroot or host helper; use Diagnostics to inspect installed packages, service configuration, and recent boot/journal evidence first when graphical boot fails.");
        buttonText = QStringLiteral("Restore Graphical Login");
        iconName = QStringLiteral("video-display");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("initramfs")) {
        title = QStringLiteral("Initramfs");
        description = QStringLiteral(
            "Rebuild initramfs images for the running host or selected repair system only after mapper and crypttab consistency checks pass. The helper uses update-initramfs on Debian/Ubuntu, transaction-specific mkinitcpio trials on Arch, and mkinitfs trials on Alpine.");
        if (dracutBackend) {
            description += QStringLiteral(
                " The detected dracut backend pairs every installed kernel with its /boot/vmlinuz-<kver> and /boot/initramfs-<kver>.img, runs a trial build to a temporary path first, verifies the image with lsinitrd, and backs up each image before the apply so a failed verification restores the previous initramfs.");
        }
        buttonText = QStringLiteral("Rebuild Initramfs");
        iconName = QStringLiteral("initramfs");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("efi")) {
        title = QStringLiteral("EFI / UKI bootloader");
        description = QStringLiteral(
            "Repair the running host or selected repair system's EFI / UKI boot path. On current TUXEDO Debian-base systems with create_boot_uki_base.sh, Boot Bitch uses the vendor UKI builder and preserves every other ESP's firmware entries and BootOrder; TUXEDO Ubuntu layouts without that builder continue to use their GRUB path. On conventional GRUB EFI systems it performs a guarded grub-install on the selected system's validated ESP and restores one verified vendor-loader firmware entry if the guarded installer only writes files. On Alpine UEFI GRUB systems it backs up the ESP loader files, runs grub-install --target=x86_64-efi --bootloader-id=<detected> --boot-directory=/boot --no-nvram, refreshes the EFI/boot/bootx64.efi fallback copy when the layout had one, and reconciles one firmware entry for the detected loader; a failed install restores the ESP backup. An Alpine EFI-stub-only system is detected and reported, and the stage reconciles captured firmware entries only without synthesising kernel command lines. Afterward, decoded entries on each maintained ESP retain their distribution/vendor label and receive that drive's model once; an existing model name is not duplicated. The maintained BootOrder groups each drive's primary loader, fallback and WebFAI destinations and removes only entries that resolve to a duplicate destination, such as a device-path-only UEFI fallback beside BOOTX64.EFI. Unrelated EFI entries on other disks are never removed.");
        buttonText = QStringLiteral("Repair EFI / UKI");
        iconName = QStringLiteral("drive-removable-media");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("grub")) {
        if (grub2Backend) {
            title = QStringLiteral("GRUB2 configuration");
            description = QStringLiteral(
                "Regenerate the running host or selected repair system's GRUB2 configuration after the mandatory safety preflight. Fedora ships grub2-mkconfig and stores /boot/grub2/grub.cfg with /boot/grub2/grubenv and BLS entries under /boot/loader/entries; the stage regenerates the configuration with --no-grubenv-update, an entry-preserving guard and a rollback when a previous menu entry or BLS entry would be lost. "
                "When the read-only boot-code probe finds the MBR or BIOS boot partition broken, the same stage performs a guarded Reinstall GRUB2 bootloader (grub2-install --target=i386-pc --boot-directory=/boot) with MBR and bios_grub backup and rollback; a healthy boot code stays config-only. "
                "This never writes firmware NVRAM and does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.");
            buttonText = QStringLiteral("Regenerate GRUB2");
        } else {
            title = QStringLiteral("GRUB configuration");
            description = QStringLiteral(
                "Regenerate the running host or selected repair system's GRUB menu/configuration after the mandatory safety preflight. Debian/Ubuntu uses update-grub; Arch and Alpine use grub-mkconfig with an isolated trial output, an entry-preserving guard and a rollback when a previous menu entry would be lost. This does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.");
            buttonText = QStringLiteral("Regenerate GRUB");
        }
        iconName = QStringLiteral("grub");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("extlinux")) {
        title = QStringLiteral("extlinux configuration");
        description = QStringLiteral(
            "Regenerate the running host or selected repair system's extlinux bootloader configuration after the mandatory safety preflight. The detected syslinux/extlinux backend uses update-extlinux with an entry-preserving guard and a boot-artifact backup; existing boot entries are never dropped. This regenerates the configuration only and does not reinstall bootloader files.");
        buttonText = QStringLiteral("Regenerate extlinux");
        iconName = QStringLiteral("grub");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("filesystem")) {
        title = QStringLiteral("File system repair");
        description = QStringLiteral(
            "Run a read-only file system check for the selected system's root, /boot, ESP and /home filesystems, "
            "then repair only the devices that report errors. "
            "ext2/3/4 uses e2fsck, XFS xfs_repair, Btrfs check/rescue/scrub, FAT fsck.fat, exFAT fsck.exfat, "
            "NTFS ntfsfix (limited Linux-side repair; Windows chkdsk is still required), F2FS fsck.f2fs (repair only, "
            "no read-only check), JFS jfs_fsck, ReiserFS reiserfsck and ZFS zpool. "
            "Offline tools refuse mounted filesystems; btrfs scrub and zpool scrub are online modes, and "
            "btrfs check --repair requires a separate backup confirmation because upstream flags it as dangerous. "
            "The running host root is never repaired offline, and unsupported filesystems are reported rather than guessed about. "
            "This maps directly to Settings → Repair file system errors (read-only check first).");
        buttonText = QStringLiteral("Check File Systems");
        iconName = QStringLiteral("filesystem");
        planText = QStringLiteral("Full Repair plan: %1").arg(plan);
    } else if (key == QStringLiteral("bootstack")) {
        title = QStringLiteral("Boot stack reconciliation");
        description = QStringLiteral(
            "Reconcile a repaired or restored root with its boot artifacts: validate mapper/crypttab, rebuild installed-kernel initramfs images, rebuild the TUXEDO UKI when the selected system provides its official builder, or use the detected Arch EFI/GRUB path, reconcile one canonical EFI destination per purpose, and regenerate the GRUB fallback. ");
        if (dracutBackend && grub2Backend) {
            description += QStringLiteral(
                "On a Fedora BIOS target the same reconciliation validates mapper/crypttab, rebuilds the dracut initramfs images and regenerates the GRUB2 configuration (config-only; the guarded bootloader reinstall stays in the GRUB stage). ");
        }
        description += QStringLiteral(
            "When the EFI / UKI bootloader repair already ran for the same system in this session, this action reuses it: the second UKI rebuild and the duplicate GRUB regeneration are skipped and logged, while mapper/crypttab validation and initramfs reconciliation still run. "
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
    m_repairToolButton->setText(buttonText);
    m_repairToolButton->setIcon(themedIcon(iconName));
    // The tool name changes with the selection; the button is shrinkable and
    // paints an elided label instead of widening the detail panel.
    const QSize buttonHint = m_repairToolButton->sizeHint();
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
    if (key == QStringLiteral("bootstack") && efiRepairReuseAvailable()) {
        displayedPlanStatus += QStringLiteral(
            "\nReuse: the EFI / UKI repair from this session will be reused (second UKI rebuild and duplicate GRUB regeneration skipped).");
    }
    if (!targetReady || !diagnosticsReady) {
        displayedPlanStatus += QStringLiteral("\nUnavailable: ")
            + (!targetReady ? reason : diagnosticReason);
    }
    m_repairToolPlanStatus->setText(displayedPlanStatus);
    m_repairToolButton->setToolTip(!targetReady
        ? reason
        : (!diagnosticsReady
            ? diagnosticReason
            : (key == QStringLiteral("bootstack") && efiRepairReuseAvailable()
                ? QStringLiteral("Reuses the EFI / UKI repair from this session: the second UKI rebuild and duplicate GRUB regeneration are skipped. Mapper/crypttab validation and initramfs reconciliation still run.")
                : QStringLiteral("Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first."))));
}

// ---- MainWindow: repair gating and execution --------------------------------

// Ordered privileged-helper resolution candidates for one application
// directory. Installed layouts (/usr/bin, /usr/local/bin) must use the
// installed libexec helper instead of a dev/source-tree helper that happens to
// exist on the build machine (the compiled-in BOOT_REPAIR_SOURCE_DIR used to
// win there); build/portable layouts prefer the helper beside or above the
// executable so a source checkout keeps working. An explicit override always
// wins.
QList<MainWindow::HelperPathCandidate> MainWindow::repairHelperCandidates(
    const QString &appDir, const QByteArray &explicitOverride)
{
    QList<HelperPathCandidate> candidates;
    const QString overridePath = QString::fromLocal8Bit(explicitOverride).trimmed();
    if (!overridePath.isEmpty()) {
        candidates.append({overridePath, QStringLiteral("explicit BOOT_REPAIR_HELPER_PATH override")});
    }

    const QString appLibexec = QDir::cleanPath(QDir(appDir).absoluteFilePath(
        QStringLiteral("../libexec/boot-repair/boot-repair-helper")));
    const QString appScripts = QDir::cleanPath(QDir(appDir).absoluteFilePath(
        QStringLiteral("../scripts/boot-repair-helper.sh")));
    const QString appScriptsLocal = QDir::cleanPath(QDir(appDir).absoluteFilePath(
        QStringLiteral("scripts/boot-repair-helper.sh")));
    const QString cwdScripts = QDir::cleanPath(QDir::current().absoluteFilePath(
        QStringLiteral("scripts/boot-repair-helper.sh")));
#ifdef BOOT_REPAIR_SOURCE_DIR
    const QString sourceScripts = QStringLiteral(BOOT_REPAIR_SOURCE_DIR "/scripts/boot-repair-helper.sh");
#else
    const QString sourceScripts;
#endif

    const bool installed = appDir == QStringLiteral("/usr/bin")
        || appDir == QStringLiteral("/usr/local/bin");
    if (installed) {
        candidates.append({appLibexec,
                           QStringLiteral("installed helper beside the executable")});
        candidates.append({QStringLiteral("/usr/libexec/boot-repair/boot-repair-helper"),
                           QStringLiteral("installed /usr/libexec helper")});
        candidates.append({QStringLiteral("/usr/lib/boot-repair/boot-repair-helper"),
                           QStringLiteral("installed /usr/lib helper")});
        // Only when no installed helper is usable: keep a broken install
        // testable from a source/build tree.
        candidates.append({appScripts, QStringLiteral("source-tree helper (installed helper missing)")});
        if (!sourceScripts.isEmpty()) {
            candidates.append({sourceScripts, QStringLiteral("source-tree helper (installed helper missing)")});
        }
    } else {
        // AppImage/portable layout: usr/bin/boot-repair beside
        // usr/libexec/boot-repair/boot-repair-helper.
        candidates.append({appLibexec, QStringLiteral("portable helper beside the executable")});
        candidates.append({appScripts, QStringLiteral("source-tree helper above the executable")});
        candidates.append({appScriptsLocal, QStringLiteral("source-tree helper beside the executable")});
        candidates.append({cwdScripts, QStringLiteral("source-tree helper from the working directory")});
        if (!sourceScripts.isEmpty()) {
            candidates.append({sourceScripts, QStringLiteral("configured source-tree helper")});
        }
        candidates.append({QStringLiteral("/usr/libexec/boot-repair/boot-repair-helper"),
                           QStringLiteral("installed /usr/libexec helper")});
        candidates.append({QStringLiteral("/usr/lib/boot-repair/boot-repair-helper"),
                           QStringLiteral("installed /usr/lib helper")});
    }
    return candidates;
}

QString MainWindow::resolveRepairHelperPath(const QString &appDir,
                                            const QByteArray &explicitOverride,
                                            QString *resolution)
{
    const bool overrideProvided = !QString::fromLocal8Bit(explicitOverride).trimmed().isEmpty();
    for (const HelperPathCandidate &candidate : repairHelperCandidates(appDir, explicitOverride)) {
        if (candidate.path.isEmpty()) {
            continue;
        }
        QFileInfo info(candidate.path);
        // Web-uploaded source trees may lose the executable bit on shell
        // helpers.  Accept a readable .sh here; the session launcher invokes
        // it explicitly through /bin/bash below.
        if (info.exists() && info.isFile()
            && (info.isExecutable() || info.suffix().compare(QStringLiteral("sh"), Qt::CaseInsensitive) == 0)) {
            if (resolution) {
                *resolution = candidate.reason;
                if (overrideProvided
                    && !candidate.reason.startsWith(QStringLiteral("explicit BOOT_REPAIR_HELPER_PATH"))) {
                    // The explicit override existed but was not usable; say so
                    // instead of silently presenting the fallback as the
                    // intended choice.
                    *resolution += QStringLiteral("; the explicit BOOT_REPAIR_HELPER_PATH override was not usable");
                }
            }
            return info.canonicalFilePath().isEmpty() ? info.absoluteFilePath() : info.canonicalFilePath();
        }
    }
    if (resolution) {
        *resolution = QStringLiteral("no usable privileged helper was found in the installed or source locations");
    }
    return QString();
}

QString MainWindow::repairHelperPath(QString *resolution) const
{
    return resolveRepairHelperPath(QCoreApplication::applicationDirPath(),
                                   qgetenv("BOOT_REPAIR_HELPER_PATH"), resolution);
}

QStringList MainWindow::selectedRepairStages() const
{
    QStringList stages;
    // The cached capability evidence is the single gate for a selected stage.
    // The Settings checkbox's own enabled state is presentation only: a stale
    // enablement (for example evidence that arrived through an individual
    // diagnostic before the next summary refresh) must never silently drop a
    // stage whose backend is available and whose setting is on.
    auto selected = [this](QCheckBox *check, const QString &key) {
        return check && check->isChecked() && repairToolAvailable(key);
    };
    if (selected(m_fullRepairFilesystem, QStringLiteral("filesystem"))) stages << QStringLiteral("filesystem");
    if (selected(m_fullRepairDpkg, QStringLiteral("dpkg"))) stages << QStringLiteral("dpkg-configure");
    if (selected(m_fullRepairBrokenPackages, QStringLiteral("fixbroken"))) stages << QStringLiteral("fix-broken");
    if (selected(m_fullRepairAptUpdate, QStringLiteral("aptupdate"))) stages << QStringLiteral("apt-update");
    if (selected(m_fullRepairUpgrade, QStringLiteral("upgrade"))) stages << QStringLiteral("apt-upgrade");
    if (selected(m_fullRepairDkms, QStringLiteral("dkms"))) stages << QStringLiteral("dkms");
    if (selected(m_fullRepairDisplayManager, QStringLiteral("display"))) stages << QStringLiteral("display-manager");
    if (selected(m_fullRepairInitramfs, QStringLiteral("initramfs"))) stages << QStringLiteral("initramfs");
    if (selected(m_fullRepairEfi, QStringLiteral("efi"))) stages << QStringLiteral("efi");
    if (selected(m_fullRepairGrub, QStringLiteral("grub"))) stages << QStringLiteral("grub");
    if (selected(m_fullRepairExtlinux, QStringLiteral("extlinux"))) stages << QStringLiteral("extlinux");
    return stages;
}

void MainWindow::updateRepairScopeControls()
{
    const QList<QPair<QCheckBox *, QString>> allStages = {
        {m_fullRepairFilesystem, QStringLiteral("filesystem")},
        {m_fullRepairDpkg, QStringLiteral("dpkg")},
        {m_fullRepairBrokenPackages, QStringLiteral("fixbroken")},
        {m_fullRepairAptUpdate, QStringLiteral("aptupdate")},
        {m_fullRepairUpgrade, QStringLiteral("upgrade")},
        {m_fullRepairDkms, QStringLiteral("dkms")},
        {m_fullRepairDisplayManager, QStringLiteral("display")},
        {m_fullRepairInitramfs, QStringLiteral("initramfs")},
        {m_fullRepairEfi, QStringLiteral("efi")},
        {m_fullRepairGrub, QStringLiteral("grub")},
        {m_fullRepairExtlinux, QStringLiteral("extlinux")}
    };
    for (const auto &stage : allStages) {
        if (!stage.first) continue;
        QString reason;
        const bool available = repairToolAvailable(stage.second, &reason);
        // Off-by-default stages (upgrade, display manager, EFI/UKI) are only
        // in a Full Repair plan after the user selects them; say so in the
        // tooltip so "not in the plan" is never mistaken for a stale gate.
        const bool offByDefault = stage.second == QStringLiteral("upgrade")
            || stage.second == QStringLiteral("display")
            || stage.second == QStringLiteral("efi");
        if (available && offByDefault) {
            reason += QStringLiteral(" This stage is off by default: select it here to include it in Full Repair.");
        }
        stage.first->setToolTip(reason);
        stage.first->setEnabled(available);
    }
    // Labels derive from the backends the helper detected on the selected
    // scope, never from the distribution family.  A mixed-manager target gets
    // generic wording; a single detected backend keeps its exact wording.
    const QStringList managers = detectedPackageManagers();
    const bool multipleManagers = managers.size() > 1;
    const bool apkBackend = managers.contains(QStringLiteral("apk"));
    const bool pacmanBackend = managers.contains(QStringLiteral("pacman"));
    const bool rpmBackend = detectedRpmBackend();
    const bool openrcBackend = detectedServiceManager().contains(QStringLiteral("openrc"), Qt::CaseInsensitive);
    const QString initramfsBackend = detectedInitramfsBackend();
    const QString bootloaderBackend = detectedBootloaderBackend();
    const bool grubBackend = bootloaderBackend.contains(QStringLiteral("grub"), Qt::CaseInsensitive);
    const bool grub2Backend = detectedGrub2Backend();
    const bool extlinuxBackend = bootloaderBackend.contains(QStringLiteral("syslinux/extlinux"), Qt::CaseInsensitive);
    const QString displayManagerName = detectedDisplayManagerName();
    // Start from the Debian/APT wording so switching the scope or backend can
    // never leave another backend's stage labels behind.
    if (m_fullRepairEfi) {
        m_fullRepairEfi->setText(QStringLiteral("Repair EFI / UKI boot path (explicit target ESP repair)"));
    }
    if (m_fullRepairGrub) {
        m_fullRepairGrub->setText(QStringLiteral("Update GRUB configuration"));
    }
    if (m_fullRepairBrokenPackages) {
        m_fullRepairBrokenPackages->setText(QStringLiteral("Repair broken package dependencies"));
    }
    if (m_fullRepairAptUpdate) {
        m_fullRepairAptUpdate->setText(QStringLiteral("Refresh package metadata"));
    }
    if (m_fullRepairUpgrade) {
        m_fullRepairUpgrade->setText(QStringLiteral("Upgrade installed packages (adaptive APT simulation)"));
    }
    if (m_fullRepairDisplayManager) {
        m_fullRepairDisplayManager->setText(QStringLiteral("Restore detected graphical login manager and graphical.target"));
    }
    if (m_fullRepairInitramfs) {
        m_fullRepairInitramfs->setText(QStringLiteral("Rebuild initramfs after mapper/crypttab validation"));
    }
    if (m_fullRepairExtlinux) {
        m_fullRepairExtlinux->setText(QStringLiteral("Update extlinux configuration"));
    }
    // Fedora's GRUB2 is a distinct backend from the Debian/Arch grub-* naming:
    // the configuration stage regenerates /boot/grub2/grub.cfg with
    // grub2-mkconfig, and the bootloader stage reinstalls the guarded GRUB2
    // boot code.  The Alpine GRUB EFI branch below still wins for apk targets.
    if (grub2Backend) {
        if (m_fullRepairGrub) {
            m_fullRepairGrub->setText(QStringLiteral("Update GRUB2 configuration"));
        }
        if (m_fullRepairEfi) {
            m_fullRepairEfi->setText(QStringLiteral("Reinstall GRUB2 bootloader"));
        }
    }
    if (multipleManagers) {
        // Several package managers are detected: every one of them runs in the
        // stage, so the label stays generic.
        if (m_fullRepairBrokenPackages) {
            m_fullRepairBrokenPackages->setText(QStringLiteral("Repair broken package dependencies (all detected package managers)"));
        }
        if (m_fullRepairUpgrade) {
            m_fullRepairUpgrade->setText(QStringLiteral("Upgrade installed packages (all detected package managers)"));
        }
    } else if (apkBackend) {
        // apk keeps the same capability keys but labels its apk, OpenRC,
        // mkinitfs and extlinux stages. aptupdate and dpkg stay gated by
        // their capability evidence (no standalone metadata refresh, no dpkg).
        if (m_fullRepairBrokenPackages) {
            m_fullRepairBrokenPackages->setText(QStringLiteral("Repair Alpine packages (apk fix)"));
        }
        if (m_fullRepairUpgrade) {
            m_fullRepairUpgrade->setText(QStringLiteral("Upgrade Alpine packages (apk upgrade)"));
        }
        if (grubBackend) {
            if (m_fullRepairEfi) {
                m_fullRepairEfi->setText(QStringLiteral("Reinstall Alpine GRUB EFI loader"));
            }
            if (m_fullRepairGrub) {
                m_fullRepairGrub->setText(QStringLiteral("Regenerate Alpine GRUB configuration"));
            }
        }
    } else if (pacmanBackend) {
        // Arch has no dpkg database or standalone APT metadata transaction.
        // Its guarded package repair/upgrade is one full pacman -Syu
        // transaction, which is exposed through the existing broken-package
        // and upgrade actions and handled by the Arch helper backend.
        if (m_fullRepairBrokenPackages) {
            m_fullRepairBrokenPackages->setText(QStringLiteral("Repair Arch package dependencies (full pacman transaction)"));
        }
        if (m_fullRepairUpgrade) {
            m_fullRepairUpgrade->setText(QStringLiteral("Upgrade Arch packages (full pacman transaction)"));
        }
    } else if (rpmBackend) {
        // Fedora/RPM-family targets use the guarded dnf backend: one
        // simulated dnf transaction for missing files and upgrades, and a
        // dnf makecache metadata stage.  dpkg and the EFI stages stay gated
        // by their own capability evidence.
        if (m_fullRepairBrokenPackages) {
            m_fullRepairBrokenPackages->setText(QStringLiteral("Repair Fedora packages (dnf)"));
        }
        if (m_fullRepairAptUpdate) {
            m_fullRepairAptUpdate->setText(QStringLiteral("Refresh package metadata (dnf makecache)"));
        }
        if (m_fullRepairUpgrade) {
            m_fullRepairUpgrade->setText(QStringLiteral("Upgrade Fedora packages (dnf upgrade)"));
        }
    }
    if (openrcBackend) {
        if (m_fullRepairDisplayManager) {
            m_fullRepairDisplayManager->setText(QStringLiteral("Restore detected graphical login manager (OpenRC runlevel)"));
        }
    } else if (!displayManagerName.isEmpty()) {
        // The display stage names the manager the evidence detected (GDM on
        // Fedora, GDM3 on Debian, SDDM, LightDM, ...) instead of assuming a
        // family default.
        if (m_fullRepairDisplayManager) {
            m_fullRepairDisplayManager->setText(
                QStringLiteral("Restore detected graphical login manager (%1)").arg(displayManagerName));
        }
    }
    if (initramfsBackend.compare(QStringLiteral("dracut"), Qt::CaseInsensitive) == 0) {
        if (m_fullRepairInitramfs) {
            m_fullRepairInitramfs->setText(QStringLiteral("Rebuild initramfs (dracut)"));
        }
    } else if (initramfsBackend.compare(QStringLiteral("mkinitfs"), Qt::CaseInsensitive) == 0) {
        if (m_fullRepairInitramfs) {
            m_fullRepairInitramfs->setText(QStringLiteral("Rebuild initramfs (mkinitfs)"));
        }
    } else if (initramfsBackend.compare(QStringLiteral("mkinitcpio"), Qt::CaseInsensitive) == 0) {
        if (m_fullRepairInitramfs) {
            m_fullRepairInitramfs->setText(QStringLiteral("Rebuild initramfs (mkinitcpio)"));
        }
    }
    if (extlinuxBackend) {
        if (m_fullRepairExtlinux) {
            m_fullRepairExtlinux->setText(QStringLiteral("Regenerate extlinux configuration"));
        }
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

// Runs one repair action through the privileged helper and records the result.
// A Repair-kind action is captured as one replaceable log section, categorized
// from the helper's change-status evidence and, when it modified the system,
// invalidates the mapped cached diagnostics (deferred to the plan's single
// end-of-plan pass while a Full Repair plan is in progress).
void MainWindow::runRepairHelper(const QString &title, const QStringList &arguments, LogEntryKind kind,
                                 const QString &sectionKey)
{
    if (kind == LogEntryKind::Repair && repairBlockedByFilesystemFlow(title)) {
        return;
    }

    QString reason;
    const bool hostMode = m_hostMaintenanceMode;
    if (!(hostMode ? hostMaintenanceReady(&reason) : repairTargetReady(&reason))) {
        QMessageBox::warning(this, QStringLiteral("Repair unavailable"), reason);
        return;
    }

    BusyOperationScope busy(this, title);

    // A repair action is one replaceable section: its start notice, the
    // privileged completion line and the captured helper output are replaced
    // together when the same tool/stage runs again for the same scope. File
    // Copy and other kinds keep append-only history because each of those
    // operations is a distinct record rather than a repeat of one stage.
    // Repair stages only; explicit mode hints such as --post-efi are not part
    // of the section identity.
    QStringList requestedStages;
    if (!arguments.isEmpty()
        && (arguments.first() == QStringLiteral("repair")
            || arguments.first() == QStringLiteral("host-repair"))
        && arguments.size() > 3) {
        for (const QString &argument : arguments.mid(3)) {
            if (!argument.startsWith(QStringLiteral("--"))) {
                requestedStages.append(argument);
            }
        }
    }
    QString repairKey = sectionKey;
    if (repairKey.isEmpty()) {
        if (!requestedStages.isEmpty()) {
            repairKey = requestedStages.join(QLatin1Char('+'));
        } else {
            repairKey = title;
        }
    }
    // The single mapping table decides which cached diagnostics this action
    // invalidates. Unknown modifying workflows (including file copy) map to the
    // complete set through diagnosticSectionsForRepair().
    QStringList invalidationKeys = requestedStages;
    if (invalidationKeys.isEmpty()) {
        invalidationKeys = {sectionKey.isEmpty() ? QStringLiteral("unknown") : sectionKey};
    }
    const QString repairSection = kind == LogEntryKind::Repair
        ? repairLogSectionIdentity(repairKey)
        : QString();
    if (!repairSection.isEmpty()) {
        beginRepairLogSection(repairSection);
    }

    const QString diskPath = hostMode ? m_hostPrimaryPath : m_previewTargetPath;
    const QString componentPath = hostMode ? m_hostPrimaryComponentPath : m_previewTargetComponentPath;
    appendLog(QStringLiteral("Starting privileged action: %1 on %2 (%3).")
                  .arg(title, diskPath, componentPath),
              QStringLiteral("INFO"), kind);

    const bool targetModified = !arguments.isEmpty()
        && (arguments.first() == QStringLiteral("repair")
            || arguments.first() == QStringLiteral("fs-repair")
            || arguments.first() == QStringLiteral("host-fs-repair")
            || (arguments.first() == QStringLiteral("copy")
                && arguments.value(3) == QStringLiteral("host-to-repair")));
    bool processSucceeded = false;
    QStringList helperArguments = arguments;
    if (hostMode && !helperArguments.isEmpty()
        && helperArguments.first() == QStringLiteral("repair")) {
        helperArguments[0] = QStringLiteral("host-repair");
        helperArguments[1] = m_hostPrimaryPath;
        helperArguments[2] = m_hostPrimaryComponentPath;
    } else if (hostMode && !helperArguments.isEmpty()
        && helperArguments.first() == QStringLiteral("fs-repair")) {
        helperArguments[0] = QStringLiteral("host-fs-repair");
        helperArguments[1] = m_hostPrimaryPath;
        helperArguments[2] = m_hostPrimaryComponentPath;
    }
    const QString helperOutput = runPrivilegedRequest(title, helperArguments, QByteArray(), &processSucceeded,
                                                      true, kind);

    // Persist the helper transcript in the action register. The progress
    // dialog is transient, while Logs must retain the complete stage output
    // for both successful and failed repairs and allow it to be searched.
    const QString detail = helperOutput.trimmed().isEmpty()
        ? QStringLiteral("The privileged helper returned no diagnostic output.")
        : helperOutput.trimmed();
    appendLog(QStringLiteral("Repair output\nDiagnostic: %1\n%2").arg(title, detail),
              processSucceeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"), kind);
    // Categorize the action and record the summary inside the replaceable
    // section (newest entry, so it renders above the captured output). A Full
    // Repair request reports one aggregate summary plus one line per stage;
    // every other repair reports its own single result line.
    if (kind == LogEntryKind::Repair && !repairSection.isEmpty()) {
        if (m_fullRepairPlanInProgress && repairKey == QStringLiteral("full-repair")) {
            const QMap<QString, QString> statuses = parseRepairChangeStatuses(helperOutput);
            const QString failureReason = processSucceeded
                ? QString()
                : shortRepairFailureReason(helperOutput);
            // The helper names the stage it stopped in. Stages that completed
            // before it keep their own change status; only the named stage is
            // failed; requested stages after it never ran at all.
            const QString failedStageKey = processSucceeded
                ? QString()
                : repairFailureStageKey(helperOutput);
            QList<RepairStageResult> stageResults = m_fullRepairPlanStageResults;
            QStringList coveredKeys;
            for (const RepairStageResult &recorded : stageResults) {
                coveredKeys.append(recorded.toolKey);
            }
            bool failureAttributed = false;
            for (const QString &requested : requestedStages) {
                const QString toolKey = repairToolKeyForStage(requested);
                if (coveredKeys.contains(toolKey)) {
                    continue;
                }
                const auto status = statuses.constFind(toolKey);
                RepairStageResult stage;
                stage.toolKey = toolKey;
                if (status != statuses.constEnd()) {
                    if (repairChangeStatusIsUnchanged(status.value())) {
                        stage.category = RepairResultCategory::NoRepairNeeded;
                    } else {
                        stage.category = RepairResultCategory::Success;
                    }
                    // Surface the helper's reason so the user can audit why a
                    // stage skipped its write and can see held-back/skipped
                    // package-manager feedback appended to a change status.
                    stage.detail = repairChangeStatusReason(status.value());
                } else if (!processSucceeded
                           && !failureAttributed
                           && (failedStageKey.isEmpty() || toolKey == failedStageKey)) {
                    // The named failing stage (or, when the helper failed
                    // before naming one, the first uncompleted stage) owns the
                    // failure reason.
                    stage.category = RepairResultCategory::Failed;
                    stage.reason = failureReason;
                    failureAttributed = true;
                } else if (!processSucceeded) {
                    // A requested stage without a completed status after the
                    // failure never ran; it is not a failure of its own.
                    stage.category = RepairResultCategory::NotRun;
                } else {
                    // A missing status keeps the same fail-safe default as the
                    // invalidation decision: the stage is reported successful.
                    stage.category = RepairResultCategory::Success;
                }
                stageResults.append(stage);
                coveredKeys.append(toolKey);
            }
            // A failure can belong to a stage the plan did not request
            // directly (for example the GRUB follow-up of an EFI repair) or to
            // the mandatory preflight. Keep that failure visible even when no
            // requested stage matched it.
            if (!processSucceeded && !failureAttributed && !failedStageKey.isEmpty()
                && !coveredKeys.contains(failedStageKey)) {
                RepairStageResult stage;
                stage.toolKey = failedStageKey;
                stage.category = RepairResultCategory::Failed;
                stage.reason = failureReason;
                stageResults.append(stage);
                coveredKeys.append(failedStageKey);
                failureAttributed = true;
            }
            // The helper can report additional changed keys that were not
            // requested (for example the GRUB follow-up of an EFI repair).
            for (auto it = statuses.constBegin(); it != statuses.constEnd(); ++it) {
                if (coveredKeys.contains(it.key())) {
                    continue;
                }
                RepairStageResult stage;
                stage.toolKey = it.key();
                const bool unchanged = repairChangeStatusIsUnchanged(it.value());
                stage.category = unchanged
                    ? RepairResultCategory::NoRepairNeeded
                    : RepairResultCategory::Success;
                stage.detail = repairChangeStatusReason(it.value());
                stageResults.append(stage);
                coveredKeys.append(it.key());
            }
            if (!stageResults.isEmpty()) {
                appendLog(fullRepairPlanSummary(stageResults),
                          processSucceeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"), kind);
                m_fullRepairPlanSummaryLogged = true;
            }
            m_fullRepairPlanStageResults = stageResults;
        } else {
            const RepairResultCategory category =
                categorizeRepairResult(processSucceeded, invalidationKeys, helperOutput);
            // Individual tool runs dispatch exactly one stage; the vocabulary
            // then labels the single-action summary with the same phrase the
            // plan stage line uses. Read-only section actions (validate) fall
            // back to their section key.
            QString toolKey;
            if (requestedStages.size() == 1) {
                toolKey = repairToolKeyForStage(requestedStages.first());
            } else if (requestedStages.isEmpty() && !sectionKey.isEmpty()) {
                toolKey = repairToolKeyForStage(sectionKey);
            }
            // The helper's proven-unchanged reason and the held-back/skipped
            // package-manager feedback appended to a change status stay in the
            // single-action result summary. A bare "changed" carries no reason.
            QString statusDetail;
            const QMap<QString, QString> statuses = parseRepairChangeStatuses(helperOutput);
            if (!toolKey.isEmpty()) {
                statusDetail = repairChangeStatusReason(statuses.value(toolKey));
            } else {
                QStringList reasons;
                for (auto it = statuses.constBegin(); it != statuses.constEnd(); ++it) {
                    const QString reason = repairChangeStatusReason(it.value());
                    if (!reason.isEmpty() && !reasons.contains(reason)) {
                        reasons.append(reason);
                    }
                }
                statusDetail = reasons.join(QStringLiteral("; "));
            }
            appendRepairResultSummary(category,
                                      category == RepairResultCategory::Failed
                                          ? shortRepairFailureReason(helperOutput)
                                          : QString(),
                                      kind, toolKey, statusDetail);
        }
    }
    if (!repairSection.isEmpty()) {
        finishRepairLogSection(repairSection);
    }

    if (!processSucceeded) {
        if (targetModified) {
            if (m_fullRepairPlanInProgress) {
                // The plan owns the single post-plan invalidation and
                // regeneration; a failed stage accumulates the complete set so
                // the plan regenerates once after its last stage.
                accumulatePlanInvalidation(invalidationKeys, /*failed=*/true);
            } else {
                // A failed modifying workflow may have completed an earlier stage
                // before stopping. A failure forces the complete set (fail safe)
                // until a fresh diagnostic proves the resulting state.
                invalidateDiagnosticsForRepair(invalidationKeys, /*failed=*/true,
                                               QStringLiteral("modifying repair failed"), kind);
            }
            // A failed modifying action may have left the boot artifacts in an
            // unknown state; never let Reconcile boot stack reuse a repair
            // claim that this failure could have invalidated.
            m_efiBootloaderRepaired = false;
            m_efiBootloaderRepairedScope.clear();
        }
        QMessageBox::critical(this, QStringLiteral("%1 failed").arg(title),
                              QStringLiteral("The repair action failed. Review the captured helper output in Logs for the failing stage and its reason."));
    }

    if (processSucceeded) {
        if (targetModified) {
            // Evidence-driven invalidation: only actions the helper proved
            // changed invalidate their mapped sections. A missing or malformed
            // status line keeps the fail-safe changed behavior.
            const QStringList invalidatingKeys =
                invalidatingRepairToolKeys(invalidationKeys, helperOutput);
            if (m_fullRepairPlanInProgress) {
                // The plan defers its single invalidation to
                // finishFullRepairPlan(); accumulate only the changed stages.
                m_fullRepairPlanModifyingStageRan = true;
                accumulatePlanInvalidation(invalidatingKeys, /*failed=*/false);
            } else if (invalidatingKeys.isEmpty()) {
                appendStatusLog(statusEntryIdentity(QStringLiteral("repair-unchanged")),
                                QStringLiteral("No system changes were detected; cached diagnostics remain valid."),
                                QStringLiteral("INFO"), kind);
            } else {
                // The repair action may have been launched while the Repair tab
                // was visible. invalidateDiagnosticsForRepair() refreshes both
                // aggregate and individual controls and schedules the one
                // coalesced regeneration for this operation.
                invalidateDiagnosticsForRepair(invalidatingKeys, /*failed=*/false,
                                               QStringLiteral("modifying repair completed"), kind);
            }
            // Reconcile boot stack reuses this repair: the EFI / UKI stage
            // rebuilt and verified the boot artifacts, and a later GRUB-only
            // regeneration does not touch them. Any other modifying stage may
            // have changed kernels or initramfs, so the reuse claim is dropped.
            if (requestedStages.contains(QStringLiteral("efi"))) {
                m_efiBootloaderRepaired = true;
                m_efiBootloaderRepairedScope = currentPrivilegedScopeKey();
            } else if (!requestedStages.contains(QStringLiteral("grub"))) {
                m_efiBootloaderRepaired = false;
                m_efiBootloaderRepairedScope.clear();
            }
        }
        refreshDevices();
    }
}

// Refuses a second repair request while the file system check/repair flow owns
// the privileged session. The running flow is intentionally synchronous and
// can last for hours (large fsck runs, multiple devices); the request must be
// refused instead of queued behind it.
bool MainWindow::repairBlockedByFilesystemFlow(const QString &actionTitle)
{
    if (!m_filesystemRepairFlowActive) {
        return false;
    }
    const QString message = QStringLiteral(
        "A file system check and repair is already running. Wait for it to finish before starting another repair; this request was not queued.");
    appendLog(QStringLiteral("%1 refused: %2").arg(actionTitle, message),
              QStringLiteral("INFO"), LogEntryKind::Repair);
    QMessageBox::warning(this, QStringLiteral("File system repair in progress"), message);
    return true;
}

void MainWindow::runSelectedRepairTool()
{
    if (repairBlockedByFilesystemFlow(QStringLiteral("Run Tool"))) {
        return;
    }

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
    if (!repairToolAvailable(key, &diagnosticReason)) {
        QMessageBox::warning(this, QStringLiteral("Repair tool unavailable"), diagnosticReason);
        return;
    }
    if (key == QStringLiteral("validate")) {
        if (m_hostMaintenanceMode) {
            runRepairHelper(QStringLiteral("Validate running host"),
                            {QStringLiteral("host-validate"), m_hostPrimaryPath, m_hostPrimaryComponentPath},
                            LogEntryKind::Repair, QStringLiteral("validate"));
            return;
        }
        runRepairHelper(QStringLiteral("Validate repair target"),
                        {QStringLiteral("validate"), m_previewTargetPath, m_previewTargetComponentPath},
                        LogEntryKind::Repair, QStringLiteral("validate"));
        return;
    }
    if (key == QStringLiteral("filesystem")) {
        runFilesystemRepairFlow(/*partOfFullRepair=*/false);
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
        operations = {QStringLiteral("Run the distribution-specific transaction preflight, then choose a safe upgrade for the %1").arg(selectedSystemLabel)};
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
        operations = {QStringLiteral("Trial-build and then rebuild all initramfs images for the %1").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("efi")) {
        title = QStringLiteral("Repair EFI / UKI bootloader");
        stages = {QStringLiteral("efi")};
        operations = {QStringLiteral("Use the TUXEDO vendor UKI builder when that layout is detected for the %1, preserving every other firmware entry and BootOrder").arg(selectedSystemLabel),
                      QStringLiteral("Otherwise reinstall GRUB EFI loader files only on the %1's validated ESP").arg(selectedSystemLabel),
                      QStringLiteral("Regenerate GRUB configuration afterward")};
    } else if (key == QStringLiteral("grub")) {
        title = QStringLiteral("Regenerate GRUB configuration");
        stages = {QStringLiteral("grub")};
        operations = {QStringLiteral("Trial-generate and then regenerate the %1's GRUB configuration").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("extlinux")) {
        title = QStringLiteral("Regenerate extlinux configuration");
        stages = {QStringLiteral("extlinux")};
        operations = {QStringLiteral("Regenerate the %1's extlinux bootloader configuration with an entry-preserving guard").arg(selectedSystemLabel)};
    } else if (key == QStringLiteral("bootstack")) {
        title = QStringLiteral("Reconcile boot stack");
        stages = {QStringLiteral("boot-stack")};
        const bool reuseEfiRepair = efiRepairReuseAvailable();
        if (reuseEfiRepair) {
            // Explicit hint: the EFI / UKI stage already rebuilt and verified
            // the boot artifacts in this session, so the helper must not
            // repeat that work or the GRUB regeneration it performed.
            stages.append(QStringLiteral("--post-efi"));
        }
        operations = {QStringLiteral("Validate mapper/crypttab against the %1").arg(selectedSystemLabel),
                      QStringLiteral("Trial-build and rebuild initramfs for installed kernels"),
                      QStringLiteral("Rebuild the TUXEDO UKI or detected Arch EFI path when supported, removing only duplicate EFI destinations"),
                      QStringLiteral("Regenerate the GRUB fallback configuration")};
        if (reuseEfiRepair) {
            operations.prepend(QStringLiteral("Reuse the EFI / UKI bootloader repair already completed for this %1: skip the second UKI rebuild and the duplicate GRUB regeneration").arg(selectedSystemLabel));
        }
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
                  .arg(evidence.toUtf8().size()),
              QStringLiteral("INFO"), LogEntryKind::Repair);
    operations.prepend(QStringLiteral("Read-only diagnostics completed; review the full evidence in Logs before confirming repair."));

    if (!confirmRepairAction(title, operations)) {
        return;
    }

    QStringList arguments = {QStringLiteral("repair"), m_previewTargetPath, m_previewTargetComponentPath};
    arguments.append(stages);
    runRepairHelper(title, arguments, LogEntryKind::Repair, key);
}

// Executes the selected Full Repair stages as one plan. The file system
// pre-stage runs first (inspect, then per-device repair); the remaining stages
// run in one helper request and the plan performs a single invalidation and
// evidence regeneration at the end.
void MainWindow::runFullRepair()
{
    if (repairBlockedByFilesystemFlow(QStringLiteral("Run Full Repair"))) {
        return;
    }

    const QStringList stages = selectedRepairStages();
    if (stages.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Full Repair"), QStringLiteral("No Full Repair stages are selected."));
        return;
    }

    for (const QString &stage : stages) {
        QString diagnosticReason;
        if (!repairEvidenceReadyForTool(repairToolKeyForStage(stage), &diagnosticReason)) {
            QMessageBox::warning(this, QStringLiteral("Repair diagnostics required"), diagnosticReason);
            return;
        }
    }

    QStringList operations;
    const QString selectedSystemLabel = m_hostMaintenanceMode
        ? QStringLiteral("running host")
        : QStringLiteral("selected repair system");
    for (const QString &stage : stages) {
        if (stage == QStringLiteral("filesystem")) operations << QStringLiteral("Run the read-only file system check, then repair the selected devices");
        else if (stage == QStringLiteral("dpkg-configure")) operations << QStringLiteral("Complete interrupted package configuration");
        else if (stage == QStringLiteral("fix-broken")) operations << QStringLiteral("Repair broken APT dependencies");
        else if (stage == QStringLiteral("apt-update")) operations << QStringLiteral("Refresh APT package metadata");
        else if (stage == QStringLiteral("apt-upgrade")) operations << QStringLiteral("Simulate APT first, then choose a safe upgrade/full-upgrade/dist-upgrade transaction");
        else if (stage == QStringLiteral("dkms")) operations << QStringLiteral("Rebuild DKMS modules");
        else if (stage == QStringLiteral("display-manager")) operations << QStringLiteral("Restore the detected display manager and graphical.target without starting the GUI inside chroot");
        else if (stage == QStringLiteral("initramfs")) operations << QStringLiteral("Rebuild all initramfs images");
        else if (stage == QStringLiteral("efi")) operations << QStringLiteral("Repair the %1 EFI / UKI boot path using its validated ESP").arg(selectedSystemLabel);
        else if (stage == QStringLiteral("grub")) operations << QStringLiteral("Regenerate the %1's GRUB configuration").arg(selectedSystemLabel);
        else if (stage == QStringLiteral("extlinux")) operations << QStringLiteral("Regenerate the %1's extlinux configuration").arg(selectedSystemLabel);
    }

    const QString evidence = m_hostMaintenanceMode
        ? m_hostDiagnosticCache.value(QStringLiteral("report"))
        : m_targetDiagnosticCache.value(QStringLiteral("report"));
    appendLog(QStringLiteral("Using cached read-only diagnostic evidence for Full Repair (%1 bytes).")
                  .arg(evidence.toUtf8().size()),
              QStringLiteral("INFO"), LogEntryKind::Repair);
    operations.prepend(QStringLiteral("Read-only diagnostics completed; review the full evidence in Logs before confirming repair."));

    if (!confirmRepairAction(QStringLiteral("Run Full Repair"), operations)) {
        return;
    }

    // From here on the complete plan owns the repair session: stages keep the
    // evidence they were approved with, no per-stage STALE notice or refresh
    // is emitted, and finishFullRepairPlan() invalidates and regenerates once
    // after the last stage (including aborted and failed plans).
    m_fullRepairPlanInProgress = true;
    m_fullRepairPlanModified = false;
    m_fullRepairPlanModifyingStageRan = false;
    m_fullRepairPlanInvalidatedSections.clear();
    m_fullRepairPlanRequiresFullInvalidation = false;
    m_fullRepairPlanStageResults.clear();
    m_fullRepairPlanSummaryLogged = false;

    // File system repair is inspect-first: the read-only fs-inspect run and the
    // per-device confirmation happen before the remaining stages execute.  The
    // stage list sent to the helper excludes it because fs-repair is a
    // separate helper command with its own device/mode arguments.  If the user
    // declines the file system repair, the remaining package/boot stages must
    // not run after a cancellation.
    QStringList helperStages = stages;
    if (helperStages.removeAll(QStringLiteral("filesystem")) > 0) {
        if (!runFilesystemRepairFlow(/*partOfFullRepair=*/true)) {
            finishFullRepairPlan();
            return;
        }
    }
    if (helperStages.isEmpty()) {
        finishFullRepairPlan();
        return;
    }

    QStringList arguments = {QStringLiteral("repair"), m_previewTargetPath, m_previewTargetComponentPath};
    arguments.append(helperStages);
    // Both boot stages in one plan: the boot-stack reconciliation reuses the
    // EFI / UKI repair that already ran earlier in the same plan.
    if (helperStages.contains(QStringLiteral("efi"))
        && helperStages.contains(QStringLiteral("boot-stack"))) {
        arguments.append(QStringLiteral("--post-efi"));
    }
    runRepairHelper(QStringLiteral("Full Repair"), arguments, LogEntryKind::Repair,
                    QStringLiteral("full-repair"));
    finishFullRepairPlan();
}

void MainWindow::finishFullRepairPlan()
{
    if (!m_fullRepairPlanInProgress) {
        return;
    }
    m_fullRepairPlanInProgress = false;
    const bool modifyingStageRan = m_fullRepairPlanModifyingStageRan;
    m_fullRepairPlanModifyingStageRan = false;
    // A plan that never reached the Full Repair helper section (for example a
    // file-system-only plan) still gets its aggregate summary here. The helper
    // path already wrote it inside the section.
    if (!m_fullRepairPlanSummaryLogged && !m_fullRepairPlanStageResults.isEmpty()) {
        bool planFailed = false;
        for (const RepairStageResult &stage : m_fullRepairPlanStageResults) {
            if (stage.category == RepairResultCategory::Failed) {
                planFailed = true;
                break;
            }
        }
        appendLog(fullRepairPlanSummary(m_fullRepairPlanStageResults),
                  planFailed ? QStringLiteral("ERROR") : QStringLiteral("INFO"),
                  LogEntryKind::Repair);
    }
    m_fullRepairPlanStageResults.clear();
    m_fullRepairPlanSummaryLogged = false;
    if (!m_fullRepairPlanModified) {
        m_fullRepairPlanInvalidatedSections.clear();
        m_fullRepairPlanRequiresFullInvalidation = false;
        if (modifyingStageRan) {
            // Every modifying stage proved unchanged: no section is stale and
            // the end-of-plan regeneration is skipped entirely.
            appendStatusLog(statusEntryIdentity(QStringLiteral("repair-unchanged")),
                            QStringLiteral("No system changes were detected; cached diagnostics remain valid."),
                            QStringLiteral("INFO"), LogEntryKind::Repair);
        }
        return;
    }
    // One invalidation and one regeneration for the complete plan: apply the
    // accumulated union of the stages' mapped sections, or the complete set
    // when any stage was broad or failed. The between-run stale gate stays
    // intact: the next manual tool run sees the invalidated evidence until this
    // regeneration completes.
    const QStringList planSections = orderedDiagnosticSections(m_fullRepairPlanInvalidatedSections.values());
    const bool fullInvalidation = m_fullRepairPlanRequiresFullInvalidation || planSections.isEmpty();
    m_fullRepairPlanInvalidatedSections.clear();
    m_fullRepairPlanRequiresFullInvalidation = false;
    if (fullInvalidation) {
        invalidateAllActiveScopeDiagnostics();
    } else {
        markDiagnosticSectionsStale(m_hostMaintenanceMode, planSections);
    }
    refreshDevices();
    updateDiagnosticDetails();
    updateFullRepairSummary();
    appendStatusLog(statusEntryIdentity(QStringLiteral("stale-repair-modified")),
                    fullInvalidation
                        ? QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action. Regenerate diagnostics before the next repair.")
                        : QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action. Stale cached diagnostic sections: %1. They will be regenerated automatically before the next repair.")
                              .arg(planSections.join(QStringLiteral(", "))),
                    QStringLiteral("INFO"), LogEntryKind::Repair);
    scheduleEvidenceRefresh(QStringLiteral("Full Repair plan completed"));
}

bool MainWindow::efiRepairReuseAvailable() const
{
    return m_efiBootloaderRepaired
        && !m_efiBootloaderRepairedScope.isEmpty()
        && m_efiBootloaderRepairedScope == currentPrivilegedScopeKey();
}

// ---- MainWindow: file system check and repair flow --------------------------

QList<FilesystemCheckResult> MainWindow::parseFilesystemCheckOutput(const QString &output)
{
    QList<FilesystemCheckResult> results;
    QMap<QString, int> indexByDevice;
    const QRegularExpression checkRe(QStringLiteral(
        "^File system check (\\S+): (\\S+) uuid=(\\S+) mount=(\\S+) tool=(\\S+) result=(\\S+)\\s*$"));
    const QRegularExpression detailRe(QStringLiteral(
        "^File system check detail (\\S+): tool=(\\S+) output=(.*)$"));
    const QStringList lines = output.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        const QRegularExpressionMatch checkMatch = checkRe.match(line);
        if (checkMatch.hasMatch()) {
            FilesystemCheckResult result;
            result.device = checkMatch.captured(1);
            result.fstype = checkMatch.captured(2);
            result.uuid = checkMatch.captured(3);
            result.mount = checkMatch.captured(4);
            result.tool = checkMatch.captured(5);
            result.result = checkMatch.captured(6);
            indexByDevice.insert(result.device, results.size());
            results.append(result);
            continue;
        }
        const QRegularExpressionMatch detailMatch = detailRe.match(line);
        if (detailMatch.hasMatch()) {
            const auto index = indexByDevice.constFind(detailMatch.captured(1));
            if (index != indexByDevice.constEnd()) {
                results[index.value()].detail = detailMatch.captured(3);
            }
        }
    }
    return results;
}

QString MainWindow::filesystemCheckDetail(const QList<FilesystemCheckResult> &results)
{
    if (results.isEmpty()) {
        return QStringLiteral("no scope filesystems were resolved");
    }
    int clean = 0;
    int issues = 0;
    int skipped = 0;
    int unavailable = 0;
    QStringList skippedDevices;
    QStringList unavailableDevices;
    for (const FilesystemCheckResult &result : results) {
        if (result.result == QStringLiteral("skipped")) {
            ++skipped;
            skippedDevices.append(QStringLiteral("%1 %2").arg(result.device, result.fstype));
        } else if (result.result == QStringLiteral("unsupported")
                   || result.result == QStringLiteral("tool-missing")) {
            ++unavailable;
            unavailableDevices.append(QStringLiteral("%1 %2").arg(result.device, result.fstype));
        } else if (result.result == QStringLiteral("clean")) {
            ++clean;
        } else {
            ++issues;
        }
    }
    QStringList parts;
    if (issues > 0) {
        parts.append(QStringLiteral("%1 file system(s) with issues").arg(issues));
    }
    if (skipped > 0) {
        parts.append(QStringLiteral("%1 skipped (mounted, offline-only check): %2")
                         .arg(skipped)
                         .arg(skippedDevices.join(QStringLiteral(", "))));
    }
    if (unavailable > 0) {
        parts.append(QStringLiteral("%1 not checked (unsupported or no installed check tool): %2")
                         .arg(unavailable)
                         .arg(unavailableDevices.join(QStringLiteral(", "))));
    }
    if (parts.isEmpty()) {
        parts.append(QStringLiteral("%1 checked clean").arg(clean));
    }
    return parts.join(QStringLiteral("; "));
}

QStringList MainWindow::filesystemRepairModes(const QString &fstype, bool mounted)
{
    // The preferred (safest complete) repair mode is listed first so the
    // confirmation dialog defaults to it.  The helper independently validates
    // the requested mode and refuses unsupported or mounted-device cases.
    // This list is the presentation mirror of the helper's
    // filesystem_mode_field() matrix; scripts/test-filesystem-repair-contract.sh
    // asserts the two cannot drift.
    const QString fs = fstype.toLower();
    if (fs == QStringLiteral("btrfs")) {
        if (mounted) {
            // Offline btrfs check/repair must never run against a mounted
            // filesystem; a mounted btrfs filesystem is repaired online by
            // scrub or inspected with the read-only check.
            return {QStringLiteral("scrub"), QStringLiteral("check")};
        }
        return {QStringLiteral("repair"), QStringLiteral("rescue"), QStringLiteral("scrub"), QStringLiteral("check")};
    }
    if (fs == QStringLiteral("zfs")) {
        return {QStringLiteral("scrub"), QStringLiteral("check")};
    }
    if (fs == QStringLiteral("f2fs")) {
        // fsck.f2fs has no read-only check mode and its exit status is not
        // trustworthy, so f2fs is never inspected and is repair-only.
        return mounted ? QStringList{} : QStringList{QStringLiteral("repair")};
    }
    if (mounted) {
        return {QStringLiteral("check")};
    }
    return {QStringLiteral("repair"), QStringLiteral("check")};
}

QString MainWindow::runFilesystemInspect(bool partOfFullRepair)
{
    const bool hostMode = m_hostMaintenanceMode;
    QString reason;
    if (!(hostMode ? hostMaintenanceReady(&reason) : repairTargetReady(&reason))) {
        if (!m_evidenceRefreshInProgress) {
            QMessageBox::warning(this, QStringLiteral("File system check unavailable"), reason);
        }
        return QString();
    }

    const QString diskPath = hostMode ? m_hostPrimaryPath : m_previewTargetPath;
    const QString componentPath = hostMode ? m_hostPrimaryComponentPath : m_previewTargetComponentPath;
    // A standalone check gets its own replaceable section identity: re-running
    // it replaces the previous standalone block, but it can never remove the
    // Full Repair pre-stage block from the live register (and vice versa).
    const QString repairSection = repairLogSectionIdentity(
        partOfFullRepair ? QStringLiteral("filesystem-inspect")
                         : QStringLiteral("filesystem-inspect:manual"));
    beginRepairLogSection(repairSection);
    appendLog(partOfFullRepair
                  ? QStringLiteral("Starting read-only file system check (Full Repair pre-stage) on %1 (%2).")
                        .arg(diskPath, componentPath)
                  : QStringLiteral("Starting manual read-only file system check (standalone, not part of a Full Repair plan) on %1 (%2).")
                        .arg(diskPath, componentPath),
              QStringLiteral("INFO"), LogEntryKind::Repair);

    bool processSucceeded = false;
    const QString output = runPrivilegedRequest(
        partOfFullRepair ? QStringLiteral("Check File Systems (Full Repair pre-stage)")
                         : QStringLiteral("Check File Systems (manual)"),
        {hostMode ? QStringLiteral("host-fs-inspect") : QStringLiteral("fs-inspect"),
         diskPath, componentPath},
        QByteArray(), &processSucceeded, false, LogEntryKind::Repair);

    const QString detail = output.trimmed().isEmpty()
        ? QStringLiteral("The privileged helper returned no file system check output.")
        : output.trimmed();
    appendLog(QStringLiteral("File system check output\n%1").arg(detail),
              processSucceeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"),
              LogEntryKind::Repair);
    finishRepairLogSection(repairSection);
    if (!processSucceeded) {
        QMessageBox::critical(this, QStringLiteral("File system check failed"),
                              QStringLiteral("The read-only file system check failed. Review the captured helper output in Logs for the reason."));
        return QString();
    }
    return output;
}

// Inspects the scope's filesystems read-only, asks the user to select which
// reported issues to repair and in which filesystem-specific mode, then runs
// the selected repairs sequentially. Every repair only ever runs for a device
// the inspection reported as needing one; btrfs check --repair additionally
// requires the dedicated backup warning. Returns true when at least one repair
// completed successfully.
bool MainWindow::runFilesystemRepairFlow(bool partOfFullRepair)
{
    QString availabilityReason;
    if (!repairToolAvailable(QStringLiteral("filesystem"), &availabilityReason)) {
        QMessageBox::warning(this, QStringLiteral("File system repair unavailable"), availabilityReason);
        return false;
    }

    // The complete flow — read-only check, device/mode selection and every
    // selected device's sequential repair — owns one busy scope and one flow
    // guard. The busy indicator therefore stays visible between requests
    // (including while the selection/confirmation dialogs are up) and only
    // hides after the last device completes or the flow is cancelled/fails.
    // While the guard is set every repair entry point refuses a second
    // request with a clear message instead of queueing it.
    FilesystemRepairFlowScope flowScope(m_filesystemRepairFlowActive);
    BusyOperationScope busy(this, partOfFullRepair
        ? QStringLiteral("Checking and repairing file systems (Full Repair)")
        : QStringLiteral("Checking and repairing file systems"));

    const QString scopeLabel = m_hostMaintenanceMode
        ? QStringLiteral("running host")
        : QStringLiteral("selected repair system");
    const QString inspectOutput = runFilesystemInspect(partOfFullRepair);
    if (inspectOutput.trimmed().isEmpty()) {
        return false;
    }

    const QList<FilesystemCheckResult> results = parseFilesystemCheckOutput(inspectOutput);
    QList<FilesystemCheckResult> repairCandidates;
    QList<QStringList> candidateModes;
    int issueCount = 0;
    int unmountedOnlineOnlyCount = 0;
    int skippedCount = 0;
    int unavailableCount = 0;
    for (const FilesystemCheckResult &result : results) {
        // A skipped or unsupported device is not evidence of a clean
        // filesystem; it must never feed the "no errors were detected" path.
        if (result.result == QStringLiteral("skipped")) {
            ++skippedCount;
            continue;
        }
        if (result.result == QStringLiteral("unsupported") || result.result == QStringLiteral("tool-missing")) {
            ++unavailableCount;
            continue;
        }
        if (!result.needsRepair()) {
            continue;
        }
        ++issueCount;
        const bool mounted = result.mount != QStringLiteral("unmounted");
        const QStringList modes = filesystemRepairModes(result.fstype, mounted);
        // A mounted non-btrfs filesystem has no safe repair mode here: the
        // only offline tool must not run against a mounted filesystem and no
        // online repair exists for that filesystem.
        const bool hasRepairMode = std::any_of(modes.cbegin(), modes.cend(),
            [](const QString &mode) { return mode != QStringLiteral("check"); });
        if (!hasRepairMode) {
            ++unmountedOnlineOnlyCount;
            continue;
        }
        repairCandidates.append(result);
        candidateModes.append(modes);
    }

    if (issueCount == 0) {
        QStringList limitations;
        if (skippedCount > 0) {
            limitations << QStringLiteral("%1 mounted filesystem(s) were skipped because their check tools are offline-only")
                               .arg(skippedCount);
        }
        if (unavailableCount > 0) {
            limitations << QStringLiteral("%1 filesystem(s) are unsupported or have no installed check tool")
                               .arg(unavailableCount);
        }
        const QString message = limitations.isEmpty()
            ? QStringLiteral("No file system errors were detected on the %1's root, /boot, ESP or /home filesystems. No repair was run.")
                  .arg(scopeLabel)
            : QStringLiteral("No repairable file system errors were detected on the %1's root, /boot, ESP or /home filesystems, but %2. No repair was run.")
                  .arg(scopeLabel, limitations.join(QStringLiteral("; ")));
        // Inside a Full Repair plan the read-only result is recorded in the
        // pre-stage log and the aggregate plan summary; a modal notice would
        // block the plan for a result that ran no repair. A standalone manual
        // Check File Systems run keeps the dialog.
        appendLog(message, QStringLiteral("INFO"), LogEntryKind::Repair);
        if (!partOfFullRepair) {
            QMessageBox::information(this, QStringLiteral("File system repair"), message);
        }
        // The inspection is read-only and no repair command ran: the cached
        // diagnostics remain valid and nothing needs regeneration. Inside a
        // Full Repair plan the intermediate status is suppressed: later plan
        // stages may still change the system, and the plan's single end-of-plan
        // notice owns the register entry.
        if (!m_fullRepairPlanInProgress) {
            appendStatusLog(statusEntryIdentity(QStringLiteral("repair-unchanged")),
                            QStringLiteral("No system changes were detected; cached diagnostics remain valid."),
                            QStringLiteral("INFO"), LogEntryKind::Repair);
        }
        // A skipped or unsupported filesystem is not evidence of a clean
        // filesystem: the stage is reported as not checked, never as
        // no-repair-needed, and the aggregate counts it in its own category.
        const bool notChecked = skippedCount > 0 || unavailableCount > 0 || results.isEmpty();
        recordPlanStageResult(QStringLiteral("filesystem"),
                              notChecked ? RepairResultCategory::Skipped
                                         : RepairResultCategory::NoRepairNeeded,
                              QString(), filesystemCheckDetail(results));
        return true;
    }

    if (repairCandidates.isEmpty()) {
        const QString message = QStringLiteral("The read-only check found issues on %1 mounted filesystem(s) that have no safe repair mode from this environment. "
                                               "Unmount the filesystem and check again, or repair it from a live system that is not using it.")
                                    .arg(unmountedOnlineOnlyCount);
        // The stage cannot proceed: the notice is shown in every mode, plan or
        // standalone, so the user knows why the flow is ending instead of
        // having to infer it from the aggregate summary.
        appendLog(message, QStringLiteral("INFO"), LogEntryKind::Repair);
        QMessageBox::information(this, QStringLiteral("File system repair"), message);
        // Issues were found that could not be repaired: the failed category,
        // recorded as a standalone result because no repair section ran.
        const QString failureReason =
            QStringLiteral("file system issues were found but no safe repair mode is available");
        appendRepairResultSummary(RepairResultCategory::Failed, failureReason,
                                  LogEntryKind::Repair, QStringLiteral("filesystem"));
        recordPlanStageResult(QStringLiteral("filesystem"), RepairResultCategory::Failed,
                              failureReason, filesystemCheckDetail(results));
        return false;
    }

    FilesystemRepairDialog dialog(scopeLabel, repairCandidates, candidateModes, this);
    if (dialog.exec() != QDialog::Accepted) {
        appendLog(QStringLiteral("File system repair cancelled before any repair was run."),
                  QStringLiteral("INFO"), LogEntryKind::Repair);
        return false;
    }

    const QList<FilesystemRepairDialog::Selection> selections = dialog.selections();
    if (selections.isEmpty()) {
        return false;
    }

    QStringList operations;
    for (const FilesystemRepairDialog::Selection &selection : selections) {
        operations << QStringLiteral("Run %1 repair on %2").arg(selection.mode, selection.device);
    }
    if (!confirmRepairAction(QStringLiteral("Repair file system errors"), operations)) {
        return false;
    }

    // btrfs check --repair is flagged as dangerous upstream: it can make a
    // damaged filesystem worse.  It must never run from the generic
    // confirmation alone, so a dedicated backup warning is required first.
    QStringList btrfsRepairDevices;
    for (const FilesystemRepairDialog::Selection &selection : selections) {
        for (const FilesystemCheckResult &candidate : repairCandidates) {
            if (candidate.device == selection.device
                && candidate.fstype.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) == 0
                && selection.mode == QStringLiteral("repair")) {
                btrfsRepairDevices.append(selection.device);
                break;
            }
        }
    }
    if (!btrfsRepairDevices.isEmpty()) {
        QMessageBox box(this);
        box.setIcon(QMessageBox::Critical);
        box.setWindowTitle(QStringLiteral("Dangerous Btrfs repair"));
        box.setText(QStringLiteral("btrfs check --repair is a last-resort tool"));
        box.setInformativeText(QStringLiteral(
            "Upstream Btrfs documentation warns that check --repair can make a damaged filesystem worse and can lose data. "
            "Back up everything you can reach first. Prefer a scrub, a rescue, or a fresh backup/restore when either is possible.\n\n"
            "Run btrfs check --repair on:\n%1").arg(btrfsRepairDevices.join(QStringLiteral("\n"))));
        box.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
        box.setDefaultButton(QMessageBox::Cancel);
        box.button(QMessageBox::Yes)->setText(QStringLiteral("Run dangerous Btrfs repair"));
        if (box.exec() != QMessageBox::Yes) {
            appendLog(QStringLiteral("Btrfs check --repair cancelled at the extra danger confirmation."),
                      QStringLiteral("INFO"), LogEntryKind::Repair);
            return false;
        }
    }

    bool anyRepairAttempted = false;
    bool anyRepairCompleted = false;
    bool anyRepairFailed = false;
    bool anyRepairChanged = false;
    bool anyRepairStatusMissing = false;
    for (const FilesystemRepairDialog::Selection &selection : selections) {
        // One replaceable block per device and mode: re-repairing the same
        // filesystem replaces its previous block while a different device or
        // mode stays separate. A standalone run lives in its own namespace so
        // it can never overwrite the Full Repair pre-stage's repair block.
        const QString repairSection = repairLogSectionIdentity(
            partOfFullRepair
                ? QStringLiteral("filesystem-repair:%1:%2").arg(selection.device, selection.mode)
                : QStringLiteral("filesystem-repair:manual:%1:%2").arg(selection.device, selection.mode));
        beginRepairLogSection(repairSection);
        const QStringList arguments = {
            m_hostMaintenanceMode ? QStringLiteral("host-fs-repair") : QStringLiteral("fs-repair"),
            m_hostMaintenanceMode ? m_hostPrimaryPath : m_previewTargetPath,
            m_hostMaintenanceMode ? m_hostPrimaryComponentPath : m_previewTargetComponentPath,
            selection.device,
            selection.mode
        };
        bool succeeded = false;
        anyRepairAttempted = true;
        const QString output = runPrivilegedRequest(
            partOfFullRepair
                ? QStringLiteral("File system repair (%1)").arg(selection.mode)
                : QStringLiteral("File system repair (%1, manual)").arg(selection.mode),
            arguments, QByteArray(), &succeeded, false, LogEntryKind::Repair);
        const QString detail = output.trimmed().isEmpty()
            ? QStringLiteral("The privileged helper returned no repair output.")
            : output.trimmed();
        appendLog(QStringLiteral("File system repair output for %1 (%2)\n%3")
                      .arg(selection.device, selection.mode, detail),
                  succeeded ? QStringLiteral("INFO") : QStringLiteral("ERROR"),
                  LogEntryKind::Repair);
        const RepairResultCategory category = categorizeRepairResult(
            succeeded, {QStringLiteral("filesystem")}, output);
        appendRepairResultSummary(category,
                                  category == RepairResultCategory::Failed
                                      ? shortRepairFailureReason(output)
                                      : QString(),
                                  LogEntryKind::Repair, QStringLiteral("filesystem"));
        finishRepairLogSection(repairSection);
        if (succeeded) {
            anyRepairCompleted = true;
            const QMap<QString, QString> statuses = parseRepairChangeStatuses(output);
            const auto status = statuses.constFind(QStringLiteral("filesystem"));
            if (status == statuses.constEnd()) {
                // No proven-unchanged status: fail safe.
                anyRepairStatusMissing = true;
            } else if (!repairChangeStatusIsUnchanged(status.value())) {
                anyRepairChanged = true;
            }
        } else {
            anyRepairFailed = true;
        }
    }

    // A real failure must be visible, not only logged: during a plan the
    // aggregate line is written after the flow ends, so without this notice
    // the stage would end silently. A standalone manual run keeps its
    // existing log-only behavior.
    if (anyRepairFailed && partOfFullRepair) {
        QMessageBox::critical(this, QStringLiteral("File system repair failed"),
                              QStringLiteral("One or more file system repairs failed. Review the captured helper output in Logs for the failing device and its reason."));
    }

    if (anyRepairAttempted) {
        // The single mapping table decides the invalidated sections; a failed
        // repair forces the complete set (fail safe). Only file system repairs
        // the helper proved changed invalidate cached evidence.
        const bool filesystemChanged = anyRepairFailed || anyRepairChanged || anyRepairStatusMissing;
        recordPlanStageResult(QStringLiteral("filesystem"),
                              anyRepairFailed
                                  ? RepairResultCategory::Failed
                                  : (filesystemChanged ? RepairResultCategory::Success
                                                       : RepairResultCategory::NoRepairNeeded),
                              anyRepairFailed
                                  ? QStringLiteral("the privileged helper reported a failure")
                                  : QString(),
                              filesystemCheckDetail(results));
        if (m_fullRepairPlanInProgress) {
            // A plan owns the single post-plan invalidation; accumulate the
            // file system mapping into the plan union.
            m_fullRepairPlanModifyingStageRan = true;
            if (filesystemChanged) {
                accumulatePlanInvalidation({QStringLiteral("filesystem")}, anyRepairFailed);
            }
        } else if (filesystemChanged) {
            refreshDevices();
            invalidateDiagnosticsForRepair({QStringLiteral("filesystem")}, anyRepairFailed,
                                           QStringLiteral("file system repair completed"),
                                           LogEntryKind::Repair);
        } else if (!m_fullRepairPlanInProgress) {
            appendStatusLog(statusEntryIdentity(QStringLiteral("repair-unchanged")),
                            QStringLiteral("No system changes were detected; cached diagnostics remain valid."),
                            QStringLiteral("INFO"), LogEntryKind::Repair);
        }
    }
    return anyRepairCompleted;
}

// ---- MainWindow: layout and device presentation helpers ---------------------

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

// ---- Device tree status delegate and dynamic column policy ------------------

DeviceStatusDelegate::DeviceStatusDelegate(QTreeView *view, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_view(view)
{
}

QStringList DeviceStatusDelegate::wrappedLines(const QString &text, const QFont &font, int width,
                                               int maxLines, int *height)
{
    QStringList lines;
    if (text.isEmpty() || maxLines <= 0) {
        if (height) {
            *height = 0;
        }
        return lines;
    }

    if (width <= 0) {
        // No measurable width yet (before the first layout): keep the text on
        // one line instead of guessing a wrap point.
        lines.append(text);
    } else {
        QTextLayout layout(text, font);
        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        layout.setTextOption(textOption);
        layout.beginLayout();

        QList<QTextLine> textLines;
        while (textLines.size() < maxLines) {
            QTextLine line = layout.createLine();
            if (!line.isValid()) {
                break;
            }
            line.setLineWidth(width);
            textLines.append(line);
        }
        // A third line proves the second one ran out of room and must carry
        // the ellipsis. Asking the layout (instead of comparing text offsets)
        // ignores trailing whitespace that wrapping consumed.
        bool truncated = false;
        if (textLines.size() == maxLines) {
            truncated = layout.createLine().isValid();
        }

        const QFontMetrics metrics(font);
        for (int i = 0; i < textLines.size(); ++i) {
            const QTextLine &line = textLines.at(i);
            if (i == textLines.size() - 1 && truncated) {
                lines.append(metrics.elidedText(text.mid(line.textStart()).trimmed(),
                                                Qt::ElideRight, width));
            } else {
                lines.append(text.mid(line.textStart(), line.textLength()));
            }
        }
        layout.endLayout();
    }

    if (height) {
        *height = QFontMetrics(font).height() * lines.size();
    }
    return lines;
}

QSize DeviceStatusDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // QTreeView computes wrapped row heights with an invalid option width, so
    // fall back to the live column width; this is what makes the wrapped row
    // height independent of the style's own wrap heuristic.
    int width = option.rect.width();
    if (width <= 0 && m_view && index.isValid()) {
        width = m_view->columnWidth(index.column());
    }
    const QWidget *widget = opt.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    const int margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, widget) + 1;
    width -= 2 * margin;
    if (opt.features & QStyleOptionViewItem::HasDecoration) {
        width -= opt.decorationSize.width() + 2 * margin;
    }

    int height = 0;
    const QStringList lines = wrappedLines(opt.text, opt.font, width, 2, &height);
    if (lines.isEmpty()) {
        height = QFontMetrics(opt.font).height();
    }
    return QSize(0, height);
}

void DeviceStatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // The style paints the background, selection, icon and focus ring; only
    // the wrapped text is drawn here so the two-line/elided contract is the
    // same for every style.
    QStyleOptionViewItem background = opt;
    background.text.clear();
    const QWidget *widget = opt.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &background, painter, widget);

    if (opt.text.isEmpty()) {
        return;
    }

    const int margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &opt, widget) + 1;
    QRect textRect = opt.rect.adjusted(margin, 0, -margin, 0);
    if (opt.features & QStyleOptionViewItem::HasDecoration) {
        if (opt.decorationPosition == QStyleOptionViewItem::Left) {
            textRect.setLeft(textRect.left() + opt.decorationSize.width() + 2 * margin);
        } else if (opt.decorationPosition == QStyleOptionViewItem::Right) {
            textRect.setRight(textRect.right() - opt.decorationSize.width() - 2 * margin);
        }
    }

    int wrappedHeight = 0;
    const QStringList lines = wrappedLines(opt.text, opt.font, textRect.width(), 2, &wrappedHeight);
    if (lines.isEmpty()) {
        return;
    }

    QPalette::ColorGroup group = (opt.state & QStyle::State_Enabled)
        ? QPalette::Normal
        : QPalette::Disabled;
    if (group == QPalette::Normal && !(opt.state & QStyle::State_Active)) {
        group = QPalette::Inactive;
    }
    const QPalette::ColorRole role = (opt.state & QStyle::State_Selected)
        ? QPalette::HighlightedText
        : QPalette::Text;

    painter->save();
    painter->setPen(opt.palette.color(group, role));
    painter->setFont(opt.font);
    const Qt::Alignment alignment = QStyle::visualAlignment(opt.direction, opt.displayAlignment);
    const int lineHeight = QFontMetrics(opt.font).height();
    int y = textRect.y() + qMax(0, (textRect.height() - wrappedHeight) / 2);
    for (const QString &line : lines) {
        painter->drawText(QRect(textRect.x(), y, textRect.width(), lineHeight),
                          static_cast<int>(alignment), line);
        y += lineHeight;
    }
    painter->restore();
}

void MainWindow::autoSizeDeviceColumns()
{
    if (!m_deviceTree) {
        return;
    }

    applyDeviceColumnLayout();
    updateDeviceTreeHeight();
    statusBar()->showMessage(QStringLiteral("Device columns auto-sized. Drag headers to fine-tune widths."), 3500);
}

// Applies the dynamic column policy: Model/Label, Connection, Size, Filesystem
// and Device are sized to their content and only ever shrink; the Status column
// stretches to the remaining viewport width. When the pane is too narrow for
// every floor, the shrink-only columns give up their surplus (largest first)
// so Status stays usable instead of the table simply overflowing.
void MainWindow::applyDeviceColumnLayout()
{
    if (!m_deviceTree || m_deviceColumnLayoutInProgress) {
        return;
    }

    QHeaderView *header = m_deviceTree->header();
    const int columnCount = m_deviceTree->columnCount();
    if (!header || columnCount != 6) {
        return;
    }

    constexpr int kStatusColumn = 1;
    static const int minimumWidths[] = {120, 150, 55, 55, 65, 80};
    static const int maximumWidths[] = {360, 0, 120, 110, 150, 260};

    m_deviceColumnLayoutInProgress = true;
    // At most two passes: the first sizes the sections for the current
    // viewport; a second only runs when toggling a scrollbar changed the
    // viewport width while the sections were being resized.
    for (int pass = 0; pass < 2; ++pass) {
        const int available = m_deviceTree->viewport()->width();
        if (available <= 0) {
            break;
        }

        int widths[6] = {0};
        int fixedTotal = 0;
        for (int column = 0; column < columnCount; ++column) {
            if (column == kStatusColumn) {
                continue;
            }
            // resizeColumnToContents() is the public content measurement; the
            // resulting width is then clamped to the policy's floor/ceiling.
            m_deviceTree->resizeColumnToContents(column);
            const int content = m_deviceTree->columnWidth(column);
            widths[column] = qBound(minimumWidths[column], content, maximumWidths[column]);
            fixedTotal += widths[column];
        }

        int deficit = minimumWidths[kStatusColumn] - (available - fixedTotal);
        while (deficit > 0) {
            int donor = -1;
            int donorSurplus = 0;
            for (int column = 0; column < columnCount; ++column) {
                if (column == kStatusColumn) {
                    continue;
                }
                const int surplus = widths[column] - minimumWidths[column];
                if (surplus > donorSurplus) {
                    donorSurplus = surplus;
                    donor = column;
                }
            }
            if (donor < 0) {
                break;
            }
            const int take = qMin(donorSurplus, deficit);
            widths[donor] -= take;
            fixedTotal -= take;
            deficit -= take;
        }
        widths[kStatusColumn] = qMax(minimumWidths[kStatusColumn], available - fixedTotal);

        bool changed = false;
        for (int column = 0; column < columnCount; ++column) {
            if (m_deviceTree->columnWidth(column) != widths[column]) {
                m_deviceTree->setColumnWidth(column, widths[column]);
                changed = true;
            }
        }
        if (changed) {
            // Re-measure the wrapped status rows against the new width.
            m_deviceTree->doItemsLayout();
        }
        if (m_deviceTree->viewport()->width() == available) {
            break;
        }
    }
    m_deviceColumnLayoutInProgress = false;
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

// Gives every push button a consistent minimum size from its content hint.
// Buttons explicitly marked shrinkable by makeButtonShrinkable() are skipped.
void MainWindow::normalizeButtonSizing()
{
    const QList<QPushButton *> pushButtons = findChildren<QPushButton *>();
    for (QPushButton *button : pushButtons) {
        // Dense rows explicitly opted into shrinking keep their smaller
        // floor and Preferred policy so they cannot overlap when narrow.
        if (button->property("bootRepairShrinkable").toBool()) {
            continue;
        }
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

// Applies the Settings "show" filters to one top-level disk. A disk is kept
// when it passes every enabled filter; an encrypted disk stays visible while
// "show encrypted" is enabled even if no Linux candidate is visible yet.
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
