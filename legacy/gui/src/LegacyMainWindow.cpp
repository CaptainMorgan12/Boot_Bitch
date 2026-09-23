// LegacyMainWindow implementation. Qt3. See LegacyMainWindow.h.

#include "LegacyMainWindow.h"
#include "DeviceInventory.h"
#include "HelperRunner.h"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qglobal.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qimage.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qsettings.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtooltip.h>

#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>

#ifndef LEGACY_VERSION
#define LEGACY_VERSION "unknown"
#endif

namespace legacy {

namespace {

// Layout floors applied by the widget guards and re-checked by the 1024x768
// smoke layout assertion. They are deliberately larger than the raw font
// metrics so KDE's group-box title indent and button/list frame insets cannot
// clip a label on a real desktop.
const int kGroupTitlePadding = 34;
const int kButtonTextPadding = 40;
const int kListHeaderPadding = 26;
const int kLastColumnGutter = 8;
const int kSectionTitlePadding = 10;

// Hidden-input dialogs (administrator authorization, LUKS passphrase) are
// width-constrained and word-wrapped: the long explanatory text used to size
// the modal to its single unwrapped line and produced an unusably wide window.
const int kHiddenInputMaximumWidth = 440;

// The guarded `config-write` verb transports the edited file as one argv
// element; Etch's 2.6.18 kernel caps argv at 128 KiB, so the editor refuses
// anything larger instead of failing the exec opaquely.
const int kConfigEditMaximumBytes = 65536;

// The global header icon is kept compact so the eight tabs still fit at the
// 1024x768 layout contract size.
const int kHeaderIconSize = 32;

// The individual repair tools, in the modern Repair page order. `stage` is the
// legacy helper stage; an empty stage marks a display-only row (the helper
// capability exists but this frontend exposes no action for it, fail closed).
// Every stage maps to the capability key of MainWindow::repairToolKeyForStage;
// the title/button text mirror the modern individual-tool wording.
struct ToolSpec {
    const char *key;
    const char *stage;
    const char *title;
    const char *button;
    const char *description;
    const char *planStage;      // "" when the tool is not a Full Repair stage
    const char *planBase;       // base plan status for non-plan tools
    bool write;
    // Host-scope stages other than the package stages run behind the helper's
    // host-maintenance feature gate (`Legacy feature host-maintenance:`).
    bool hostMaintenance;
    const char *confirm;
};

const ToolSpec toolSpecs[] = {
    { "validate", "validate",
      "Validate environment", "Validate",
      "Check the selected system's mounts, filesystem metadata, boot files, "
      "mapper consistency and dependency readiness before any repair action. "
      "This is an independent safety preflight rather than an optional Full "
      "Repair stage.",
      "", "Always preflight", false, false, 0 },
    { "filesystem", "fs-inspect",
      "File system repair", "Check File Systems",
      "Run the read-only file system check for the selected system's root and "
      "/boot filesystems and report each device's check tool and result "
      "without changing anything. This legacy frontend exposes the read-only "
      "check only; device repair is not wired.",
      "", "Read-only check - not part of the Full Repair plan", false, false, 0 },
    { "dpkg", "dpkg-configure",
      "Complete package configuration", "Complete Configuration",
      "Complete interrupted dpkg package configuration in the selected repair "
      "system. This is the same stage controlled by Settings -> Full Repair "
      "plan -> Complete interrupted package configuration, but it can also be "
      "run independently here.",
      "dpkg-configure", "", true, false,
      "Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights." },
    { "fixbroken", "fix-broken",
      "Repair broken dependencies", "Repair Dependencies",
      "Repair package dependencies in the selected repair system after the "
      "mandatory safety preflight. This maps directly to Settings -> Repair "
      "broken package dependencies.",
      "fix-broken", "", true, false,
      "Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards." },
    { "aptupdate", "apt-update",
      "Refresh package metadata", "Refresh Metadata",
      "Refresh APT metadata in the selected repair system without upgrading "
      "installed packages. This maps directly to Settings -> Refresh package "
      "metadata.",
      "apt-update", "", true, false,
      "Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source." },
    { "upgrade", "apt-upgrade",
      "Upgrade installed packages", "Simulate and Upgrade",
      "Simulate the APT transaction first, inspect proposed removals, then "
      "apply a safe upgrade. This maps directly to Settings -> Upgrade "
      "installed packages.",
      "apt-upgrade", "", true, false,
      "Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards." },
    { "dkms", "",
      "DKMS", "Rebuild DKMS",
      "Rebuild out-of-tree kernel modules for kernels installed in the "
      "selected system. The helper refuses this action when DKMS is not "
      "installed; this legacy frontend exposes no DKMS action.",
      "", "", false, false, 0 },
    { "display", "",
      "Graphical login / display manager", "Restore Graphical Login",
      "Restore the display manager identified from the selected system's boot "
      "evidence and configuration. This legacy frontend exposes no "
      "display-manager repair action.",
      "", "", false, false, 0 },
    { "initramfs", "initramfs",
      "Initramfs", "Rebuild Initramfs",
      "Rebuild initramfs images for the selected repair system only after "
      "mapper and crypttab consistency checks pass. The helper backs up each "
      "image before the apply.",
      "initramfs", "", true, true,
      "Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights." },
    { "efi", "",
      "EFI / UKI bootloader", "Repair EFI / UKI",
      "Repair the selected system's EFI / UKI boot path. This legacy frontend "
      "exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.",
      "", "", false, false, 0 },
    { "grub", "grub",
      "GRUB configuration", "Regenerate GRUB",
      "Regenerate the selected repair system's GRUB menu/configuration after "
      "the mandatory safety preflight. The helper backs up menu.lst, preserves "
      "every existing boot entry and rolls back on any failure.",
      "grub", "", true, true,
      "Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure." },
    { "extlinux", "",
      "extlinux configuration", "Regenerate extlinux",
      "Regenerate the selected system's extlinux bootloader configuration. "
      "This legacy frontend exposes no extlinux action.",
      "", "", false, false, 0 },
    { "bootstack", "",
      "Boot stack reconciliation", "Reconcile Boot Stack",
      "Reconcile a repaired or restored root with its boot artifacts. This "
      "legacy frontend exposes no boot-stack action; use the individual boot "
      "tools above.",
      "", "Manual recovery tool", false, false, 0 }
};
const int toolSpecCount = sizeof(toolSpecs) / sizeof(toolSpecs[0]);

// The Full Repair plan stages supported by the legacy helper, in the order the
// plan runs them (the helper's stage_rank order). The Settings checkboxes and
// the persisted state use the modern-compatible labels and QSettings keys.
struct PlanSpec {
    const char *stage;
    const char *capability;
    const char *title;      // plan list / confirmation title
    const char *checkLabel; // Settings -> Full Repair plan checkbox label
    const char *settingsKey;
    bool defaultChecked;
    bool hostMaintenance;
};
const PlanSpec planSpecs[] = {
    { "dpkg-configure", "dpkg",
      "Complete interrupted package configuration",
      "Complete interrupted package configuration",
      "repair/dpkgConfigure", true, false },
    { "fix-broken", "fixbroken",
      "Repair broken package dependencies",
      "Repair broken package dependencies",
      "repair/fixBroken", true, false },
    { "apt-update", "aptupdate",
      "Refresh package metadata",
      "Refresh package metadata",
      "repair/refreshMetadata", true, false },
    { "apt-upgrade", "upgrade",
      "Upgrade installed packages",
      "Upgrade installed packages (adaptive APT simulation)",
      "repair/upgradePackages", false, false },
    { "initramfs", "initramfs",
      "Rebuild initramfs after mapper/crypttab validation",
      "Rebuild initramfs after mapper/crypttab validation",
      "repair/initramfs", true, true },
    { "grub", "grub",
      "Update GRUB configuration",
      "Update GRUB configuration",
      "repair/grub", true, true }
};
const int planSpecCount = sizeof(planSpecs) / sizeof(planSpecs[0]);

// Index of a legacy stage in planSpecs, or -1.
int planIndexForStage(const char *stage)
{
    if (!stage || !*stage) {
        return -1;
    }
    for (int i = 0; i < planSpecCount; ++i) {
        if (std::strcmp(planSpecs[i].stage, stage) == 0) {
            return i;
        }
    }
    return -1;
}

// The modern Repair page paragraph, verbatim.
const char *const kRepairPlanParagraph =
    "Choose Full Repair stages in Settings. Enabled stages run in the order "
    "shown. Each configurable stage also appears below as an individual tool; "
    "the Full Repair column mirrors its current Settings state. The boot tools "
    "(EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack "
    "reconciliation and Make Default) are independent: run them in any order, "
    "and a later action re-verifies what an earlier one changed and reports "
    "its own result. The active scope is shown beside Repair: selected repair "
    "drive or Running Host maintenance.";

// Modern Full Repair readiness strings.
const char *const kPlanReadinessDefault =
    "Run All diagnostics for the selected target or running host before "
    "starting Full Repair. The report is read-only evidence used to choose and "
    "confirm repair stages.";
const char *const kPlanReadinessReady =
    "Ready: required cached read-only diagnostics are available for the "
    "selected stages. Review them in Diagnostics or Logs before confirming.";

// Read-only diagnostic descriptions for the per-check list; the key list
// itself comes from legacy::diagnosticKeys() so the parser test and the GUI
// cannot drift. The list column carries the stable helper key (the evidence
// name used in the command and the log) while the Selected diagnostic pane
// shows the modern friendly title and description.
struct DiagnosticSpec {
    const char *key;
    const char *title;
    const char *description;
};
const DiagnosticSpec diagnosticSpecs[] = {
    { "environment", "Environment validation",
      "Summarizes the selected system, protection state, mounted identity and inspection readiness." },
    { "backend", "Distribution and boot backend profile",
      "Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability." },
    { "boot", "Boot diagnostics",
      "Shows boot mounts and /boot contents plus storage evidence without changing the selected system." },
    { "boot-evidence", "Boot evidence and selection history",
      "Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence." },
    { "kernel", "Kernel / initramfs",
      "Reviews kernel files and verifies matching initramfs images through a read-only inspection." },
    { "grub", "GRUB configuration",
      "Reviews the GRUB configuration without changing boot files." },
    { "uki", "EFI / UKI boot state",
      "Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper's probe reason." },
    { "display", "Graphical login / display manager",
      "Reviews the configured display manager and recent boot evidence without starting the GUI." },
    { "errors", "Boot errors",
      "Reads recent error-priority entries from the running host or selected repair system when available." },
    { "usage", "Disk usage",
      "Summarizes filesystem capacity and free space for the running host or read-only repair target." },
    { "filesystem", "File systems",
      "Runs the read-only file system check for the selected system's root, /boot and other filesystems." },
    { "fstab", "/etc/fstab review",
      "Displays the running host or selected repair system's fstab; repair-system inspection is mounted read-only." },
    { "btrfs", "Btrfs status",
      "Shows Btrfs filesystem and subvolume information when the target uses Btrfs." },
    { "mapper", "Device-mapper ancestry",
      "Shows selected mapper ancestry and device-mapper state when available." },
    { "luks", "LUKS / crypttab evidence",
      "Shows LUKS/mapped ancestry plus crypttab and fstab mapper references." },
    { "report", "Full diagnostic report",
      "Combines all read-only diagnostics for the selected scope (same as Run All)." }
};
const int diagnosticSpecCount = sizeof(diagnosticSpecs) / sizeof(diagnosticSpecs[0]);

// Etch-era target configuration files offered by the Diagnostics tab's
// "Edit Target File..." control. The keys match the legacy helper's guarded
// `config-read`/`config-write` keys (legacy/overlay.sh); per-target
// availability comes from the helper's read-only `Legacy config <key>:`
// diagnostics probe, so absent files are omitted with the helper's reason.
struct ConfigSpec {
    const char *key;
    const char *path;
};
const ConfigSpec configSpecs[] = {
    { "fstab", "/etc/fstab" },
    { "inittab", "/etc/inittab" },
    { "menu-lst", "/boot/grub/menu.lst" },
    { "crypttab", "/etc/crypttab" },
    { "modules", "/etc/modules" },
    { "interfaces", "/etc/network/interfaces" },
    { "sources-list", "/etc/apt/sources.list" },
    { "apt-conf", "/etc/apt/apt.conf" }
};
const int configSpecCount = sizeof(configSpecs) / sizeof(configSpecs[0]);

// Maps a combo entry (a display path) back to its helper key ("" when the
// path is not one of the Etch configuration files).
QString configKeyForPath(const QString &path)
{
    for (int i = 0; i < configSpecCount; ++i) {
        if (path == QString::fromLatin1(configSpecs[i].path)) {
            return QString::fromLatin1(configSpecs[i].key);
        }
    }
    return QString::null;
}

// Qt3 has no QLabel::setWordWrap (that API is Qt4); `Qt::WordBreak` alignment
// is the Qt3 idiom that makes a QLabel wrap its text, and the explicit
// maximum width keeps the dialog from sizing itself to one unwrapped line.
void enableLabelWordWrap(QLabel *label)
{
    if (label) {
        label->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    }
}

// Modal hidden-input prompt with a width-constrained, word-wrapped label.
// Qt3's QInputDialog sizes itself to the unwrapped text, which made the long
// administrator-authorization wording produce an unusably wide dialog.
QString promptHiddenPassword(QWidget *parent, const QString &title,
                             const QString &text, bool *ok)
{
    QDialog dialog(parent, "legacy-hidden-input", true);
    dialog.setCaption(title);
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *label = new QLabel(text, &dialog);
    enableLabelWordWrap(label);
    label->setMaximumWidth(kHiddenInputMaximumWidth);
    layout->addWidget(label);

    QLineEdit *edit = new QLineEdit(&dialog);
    edit->setEchoMode(QLineEdit::Password);
    edit->setText(QString::null);
    layout->addWidget(edit);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *cancel = new QPushButton(QString::fromLatin1("Cancel"), &dialog);
    QPushButton *accept = new QPushButton(QString::fromLatin1("OK"), &dialog);
    accept->setDefault(true);
    buttons->addWidget(cancel);
    buttons->addWidget(accept);
    QObject::connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(accept, SIGNAL(clicked()), &dialog, SLOT(accept()));
    QObject::connect(edit, SIGNAL(returnPressed()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    dialog.setMaximumWidth(kHiddenInputMaximumWidth + 40);

    if (dialog.exec() != QDialog::Accepted) {
        if (ok) {
            *ok = false;
        }
        return QString::null;
    }
    if (ok) {
        *ok = true;
    }
    return edit->text();
}

QString fromStd(const std::string &text)
{
    return QString::fromLatin1(text.c_str());
}

std::string toStd(const QString &text)
{
    return std::string(text.latin1());
}

bool ensureDirectory(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }
    QString current;
    const QStringList parts = QStringList::split(QChar('/'), path, false);
    if (!path.startsWith(QChar('/'))) {
        current = QString::fromLatin1(".");
    }
    for (QStringList::ConstIterator it = parts.begin(); it != parts.end(); ++it) {
        if ((*it).isEmpty()) {
            continue;
        }
        current += QString::fromLatin1("/") + *it;
        if (::mkdir(current.latin1(), 0755) != 0) {
            struct stat info;
            if (::stat(current.latin1(), &info) != 0 || !S_ISDIR(info.st_mode)) {
                return false;
            }
        }
    }
    return true;
}

bool logLineIsError(const QString &line)
{
    return line.find(QString::fromLatin1("ERROR")) >= 0
        || line.find(QString::fromLatin1("WARNING")) >= 0
        || line.find(QString::fromLatin1("FAIL")) >= 0
        || line.find(QString::fromLatin1("failed")) >= 0;
}

QString mapValue(const QMap<QString, QString> &map, const QString &key)
{
    const QMap<QString, QString>::const_iterator it = map.find(key);
    return it == map.end() ? QString::null : it.data();
}

QString detailOrDash(const QString &text)
{
    return text.stripWhiteSpace().isEmpty() ? QString::fromLatin1("-") : text;
}

QString diagnosticDescription(const QString &key)
{
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
            return QString::fromLatin1(diagnosticSpecs[i].description);
        }
    }
    return QString::fromLatin1("read-only helper diagnostic");
}

QString diagnosticTitle(const QString &key)
{
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
            return QString::fromLatin1(diagnosticSpecs[i].title);
        }
    }
    return key;
}

// The filesystem name shown in the inventory/details: the mounted filesystem
// first, then the read-only udev probe (so an unmounted ext3 or a locked
// crypto_LUKS component is still visible without opening the device).
QString displayFsType(const DeviceRow &row)
{
    if (!row.fstype.empty()) {
        return fromStd(row.fstype);
    }
    return fromStd(row.probedFstype);
}

// Loads the packaged PNG icon with a graceful fallback: the installed hicolor
// path first, then the source tree next to the binary, then no icon at all.
QPixmap legacyHeaderPixmap()
{
    QStringList candidates;
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/32x32/apps/boot-repair-legacy.png"));
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/48x48/apps/boot-repair-legacy.png"));
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/22x22/apps/boot-repair-legacy.png"));
    candidates.append(QString::fromLatin1(
        "/usr/share/pixmaps/boot-repair-legacy.png"));
    if (qApp) {
        const QString appDir = qApp->applicationDirPath();
        candidates.append(appDir
            + QString::fromLatin1("/../share/icons/hicolor/32x32/apps/boot-repair-legacy.png"));
        candidates.append(appDir
            + QString::fromLatin1("/../share/icons/hicolor/48x48/apps/boot-repair-legacy.png"));
        candidates.append(appDir
            + QString::fromLatin1("/data/boot-repair-legacy-32x32.png"));
    }
    for (QStringList::ConstIterator it = candidates.begin(); it != candidates.end(); ++it) {
        if (!QFile::exists(*it)) {
            continue;
        }
        QImage image(*it);
        if (image.isNull()) {
            continue;
        }
        if (image.width() != kHeaderIconSize || image.height() != kHeaderIconSize) {
            image = image.smoothScale(kHeaderIconSize, kHeaderIconSize,
                                      QImage::ScaleMin);
        }
        QPixmap pixmap(image);
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }
    return QPixmap();
}

// True when a failed command transcript actually shows a sudo session problem
// (expired timestamp, no tty for a prompt, rejected password). A mere
// "sudo:" substring in unrelated target log output must not drop the cached
// authorization.
bool transcriptSuggestsAuthFailure(const QString &transcript)
{
    static const char *const markers[] = {
        "a password is required",
        "no tty present",
        "a terminal is required",
        "incorrect password",
        "Sorry, try again",
        "authentication failure",
        "must have a tty",
        "not allowed to execute",
        "is not in the sudoers"
    };
    for (int i = 0; i < static_cast<int>(sizeof(markers) / sizeof(markers[0])); ++i) {
        if (transcript.find(QString::fromLatin1(markers[i])) >= 0) {
            return true;
        }
    }
    return false;
}

// Finds a menu item by its visible text (accelerator ampersands and the
// accelerator shortcut after a tab are ignored; Qt3's text(id) returns
// '&Refresh Devices\tF5'). Qt3 auto-generates NEGATIVE menu ids, so the sign
// cannot indicate validity; the boolean return is the only validity signal.
// Returns the id in *id when requested. Used by the smoke control
// verification.
bool menuItemId(QPopupMenu *menu, const QString &text, int *id)
{
    if (id) {
        *id = -1;
    }
    if (!menu) {
        return false;
    }
    for (uint i = 0; i < menu->count(); ++i) {
        const int itemId = menu->idAt(static_cast<int>(i));
        QString label = menu->text(itemId);
        const int tab = label.find(QChar('\t'));
        if (tab >= 0) {
            label = label.left(tab);
        }
        label.remove(QChar('&'));
        if (label == text) {
            if (id) {
                *id = itemId;
            }
            return true;
        }
    }
    return false;
}

// True when `child` is `ancestor` or a descendant of it (Qt3 QWidget has no
// isAncestorOf).
bool isInsideWidget(const QWidget *child, const QWidget *ancestor)
{
    for (const QWidget *widget = child; widget; widget = widget->parentWidget()) {
        if (widget == ancestor) {
            return true;
        }
    }
    return false;
}

// The list view must be wide enough for every declared column, otherwise the
// Qt3 header clips the last column. `problems` receives one line per issue.
bool listColumnsFit(QListView *list, const QString &name, QString *problems)
{
    if (!list || !list->viewport()) {
        return true;
    }
    int total = 0;
    for (int column = 0; column < list->columns(); ++column) {
        total += list->columnWidth(column);
    }
    if (list->viewport()->width() < total) {
        if (problems) {
            problems->append(QString::fromLatin1("%1 columns need %2px, viewport is %3px")
                                 .arg(name).arg(total).arg(list->viewport()->width()));
        }
        return false;
    }
    return true;
}

// The last column must stretch to the right edge (Qt3 `LastColumn` resize
// mode), leaving no empty gutter after the widest declared column.
bool listLastColumnFills(QListView *list, const QString &name, QString *problems)
{
    if (!list || !list->viewport() || list->columns() == 0) {
        return true;
    }
    int total = 0;
    for (int column = 0; column < list->columns(); ++column) {
        total += list->columnWidth(column);
    }
    const int gutter = list->viewport()->width() - total;
    if (gutter > kLastColumnGutter) {
        if (problems) {
            problems->append(QString::fromLatin1(
                "%1 leaves a %2px gutter after the last column (viewport %3px, columns %4px)")
                .arg(name).arg(gutter).arg(list->viewport()->width()).arg(total));
        }
        return false;
    }
    return true;
}

bool comboTextFits(QComboBox *combo, const QString &name, QString *problems)
{
    if (!combo) {
        return true;
    }
    QFontMetrics metrics(combo->font());
    const int needed = metrics.width(combo->currentText()) + 46;
    if (combo->width() < needed) {
        if (problems) {
            problems->append(QString::fromLatin1("%1 text needs %2px, width is %3px")
                                 .arg(name).arg(needed).arg(combo->width()));
        }
        return false;
    }
    return true;
}

// True when `rootPath` plausibly belongs to `diskName`: a partition on that
// disk, or a mapper whose resolved backing chain leads to one. A mapper with
// an unresolved backing device stays compatible (best effort), exactly like
// the inventory's own resolution.
bool rootBelongsToDisk(const QMap<QString, DeviceRow> &rows,
                       const QString &rootPath, const QString &diskName)
{
    const QMap<QString, DeviceRow>::const_iterator it = rows.find(rootPath);
    if (it == rows.end()) {
        return false;
    }
    const DeviceRow &row = it.data();
    if (row.mapper) {
        if (row.parent.empty()) {
            return true;
        }
        if (row.parent == diskName) {
            return true;
        }
        const QMap<QString, DeviceRow>::const_iterator parent =
            rows.find(QString::fromLatin1("/dev/") + fromStd(row.parent));
        return parent != rows.end() && !parent.data().disk
            && parent.data().parent == diskName;
    }
    return !row.disk && row.parent == diskName;
}

} // namespace

LegacyMainWindow::LegacyMainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name),
      m_tabs(0),
      m_fileMenu(0),
      m_viewMenu(0),
      m_helpMenu(0),
      m_wrapLogsMenuValid(false),
      m_wrapLogsMenuId(-1),
      m_logFilterCombo(0),
      m_configCombo(0),
      m_deviceList(0),
      m_detailList(0),
      m_diagnosticList(0),
      m_planStageList(0),
      m_toolList(0),
      m_sessionLogList(0),
      m_rawView(0),
      m_logView(0),
      m_unlockStatusView(0),
      m_logSearchEdit(0),
      m_shellCommandEdit(0),
      m_shellOutput(0),
      m_logWrapCheck(0),
      m_showNonLinuxCheck(0),
      m_showRemovableCheck(0),
      m_showEncryptedCheck(0),
      m_autoRefreshCheck(0),
      m_priorLogBanner(0),
      m_headerTitle(0),
      m_headerSubtitle(0),
      m_headerBadge(0),
      m_systemsHeading(0),
      m_targetsHeading(0),
      m_repairHeading(0),
      m_repairScopeLabel(0),
      m_diagHeading(0),
      m_scopeLabel(0),
      m_diagTitle(0),
      m_diagDescription(0),
      m_diagAvailability(0),
      m_chrootHeading(0),
      m_chrootScopeLabel(0),
      m_fileCopyHeading(0),
      m_logsHeading(0),
      m_settingsHeading(0),
      m_gateHint(0),
      m_planParagraph(0),
      m_planCountLabel(0),
      m_planReadinessLabel(0),
      m_toolTitle(0),
      m_toolDescription(0),
      m_toolPlanStatus(0),
      m_resultStatus(0),
      m_elevationLabel(0),
      m_targetSummary(0),
      m_configLabel(0),
      m_configReasonLabel(0),
      m_chrootReasonLabel(0),
      m_fileCopyReasonLabel(0),
      m_authStatusLabel(0),
      m_repairAuthStatusLabel(0),
      m_settingsHelperLabel(0),
      m_settingsElevationLabel(0),
      m_settingsLogDirLabel(0),
      m_settingsSessionLabel(0),
      m_settingsVersionLabel(0),
      m_capDistributionLabel(0),
      m_capPackageManagerLabel(0),
      m_capServiceLabel(0),
      m_capDisplayLabel(0),
      m_capInitramfsLabel(0),
      m_capBootloaderLabel(0),
      m_capLoggingLabel(0),
      m_scanButton(0),
      m_diagnosticsButton(0),
      m_runDiagnosticButton(0),
      m_copyResultsButton(0),
      m_saveResultsButton(0),
      m_configButton(0),
      m_unlockButton(0),
      m_elevateButton(0),
      m_setTargetButton(0),
      m_hostMaintenanceButton(0),
      m_authorizeButton(0),
      m_repairAuthorizeButton(0),
      m_configurePlanButton(0),
      m_runFullRepairButton(0),
      m_toolRunButton(0),
      m_resultCloseButton(0),
      m_shellRunButton(0),
      m_shellClearButton(0),
      m_newSessionLogButton(0),
      m_addNoteButton(0),
      m_deleteSessionLogButton(0),
      m_refreshCapabilitiesButton(0),
      m_chrootGroup(0),
      m_fileCopyGroup(0),
      m_chrootTab(0),
      m_settingsContent(0),
      m_repairContent(0),
      m_resultDialog(0),
      m_resultView(0),
      m_chrootCommandHeading(0),
      m_runner(new HelperRunner(this)),
      m_updatingSelection(false),
      m_running(false),
      m_pendingDiagnostic(false),
      m_pendingUnlock(false),
      m_pendingConfig(false),
      m_pendingShell(false),
      m_unlockRetry(false),
      m_targetCommitted(false),
      m_hostMaintenance(false),
      m_viewingPriorLog(false),
      m_logWrapEnabled(true),
      m_autoRefreshEnabled(true),
      m_pendingConfigWrite(false),
      m_smokeMode(false),
      m_smokeStep(0),
      m_smokeDiagnoseOk(false),
      m_smokeValidateOk(false)
{
    setCaption(QString::fromLatin1("Boot Bitch Legacy (Etch / KDE 3.5 era)"));
    // Every section title/group title/button/list column must stay fully
    // visible at the 1024x768 contract size; the layouts only need a modest
    // floor below it.
    setMinimumSize(820, 600);

    // Global header, mirroring the modern Qt6 window: packaged icon, the
    // 1.65x bold "Boot Bitch" title, the subtle recovery subtitle and the
    // right-aligned guarded-repair badge. The icon falls back gracefully when
    // the PNG is not installed (source-tree or headless runs).
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central, 8, 6);

    QHBoxLayout *headerLayout = new QHBoxLayout(mainLayout);
    headerLayout->setSpacing(8);
    QLabel *iconLabel = new QLabel(central);
    const QPixmap headerPixmap = legacyHeaderPixmap();
    if (!headerPixmap.isNull()) {
        iconLabel->setPixmap(headerPixmap);
        iconLabel->setFixedSize(kHeaderIconSize, kHeaderIconSize);
        iconLabel->setAlignment(Qt::AlignCenter);
    } else {
        iconLabel->hide();
    }
    headerLayout->addWidget(iconLabel);

    QVBoxLayout *titleColumn = new QVBoxLayout();
    titleColumn->setSpacing(1);
    m_headerTitle = new QLabel(QString::fromLatin1("Boot Bitch"), central);
    QFont titleFont = m_headerTitle->font();
    titleFont.setPointSizeFloat(titleFont.pointSizeFloat() * 1.65);
    titleFont.setBold(true);
    m_headerTitle->setFont(titleFont);
    titleColumn->addWidget(m_headerTitle);
    m_headerSubtitle = new QLabel(
        QString::fromLatin1("Linux recovery and boot-repair utility"), central);
    titleColumn->addWidget(m_headerSubtitle);
    headerLayout->addLayout(titleColumn, 1);

    m_headerBadge = new QLabel(
        QString::fromLatin1("GUARDED REPAIR  ") + QChar(0x2022)
            + QString::fromLatin1("  ") + QString::fromLatin1(LEGACY_VERSION),
        central);
    QFont badgeFont = m_headerBadge->font();
    badgeFont.setBold(true);
    m_headerBadge->setFont(badgeFont);
    m_headerBadge->setFrameShape(QFrame::StyledPanel);
    m_headerBadge->setMargin(5);
    m_headerBadge->setAlignment(Qt::AlignCenter);
    QToolTip::add(m_headerBadge, QString::fromLatin1(
        "Ordinary repairs require an explicitly selected non-host target. The "
        "protected running host has a separate deliberate maintenance mode with "
        "the same guarded repair stages and requires privilege authorization."));
    headerLayout->addWidget(m_headerBadge, 0, Qt::AlignTop);

    m_tabs = new QTabWidget(central);
    mainLayout->addWidget(m_tabs, 1);
    setCentralWidget(central);
    m_tabs->addTab(buildTargetsTab(), QString::fromLatin1("Systems"));
    m_tabs->addTab(buildDiagnosticsTab(), QString::fromLatin1("Diagnostics"));
    m_tabs->addTab(buildActionsTab(), QString::fromLatin1("Repair"));
    m_chrootTab = buildChrootShellTab();
    m_tabs->addTab(m_chrootTab, QString::fromLatin1("Chroot Shell"));
    m_tabs->addTab(buildFileCopyTab(), QString::fromLatin1("File Copy"));
    m_tabs->addTab(buildLogTab(), QString::fromLatin1("Logs"));
    m_tabs->addTab(buildSettingsTab(), QString::fromLatin1("Settings"));
    m_tabs->addTab(buildAboutTab(), QString::fromLatin1("About"));

    buildMenus();
    // Load the persisted options before the first inventory scan so the
    // device-list filters are applied to the initial population.
    loadLegacySettings();

    connect(m_runner, SIGNAL(outputLine(const QString &)),
            this, SLOT(helperLine(const QString &)));
    connect(m_runner, SIGNAL(finished(bool, int)),
            this, SLOT(helperFinished(bool, int)));

    m_logDirectory = QDir::homeDirPath() + QString::fromLatin1("/.boot-repair-legacy/logs");
    scanDevices();
    autoDetectHostTarget();
    updateElevationLabel();
    updateDriveDetails();
    updateUnlockStatus();
    updateDiagnosticDetails();
    updateCapabilityView();
    refreshSessionLogList();
    appendLog(QString::fromLatin1(
        "Boot Bitch legacy GUI ready. Every privileged command runs through the "
        "ported helper; actions stay disabled until a repair target is "
        "committed (or Host Maintenance is active) and diagnostics report their "
        "capability line."));
    updateStatus();
    updateActionStates();
    resize(1024, 768);
}

void LegacyMainWindow::closeEvent(QCloseEvent *event)
{
    // The Cancel button is gone (modern parity); closing the window is the
    // only remaining path that stops a running helper command.
    if (m_runner && m_runner->isRunning()) {
        appendLog(QString::fromLatin1(
            "Window closed while a helper command was running; cancelling it."));
        m_runner->cancel();
    }
    QMainWindow::closeEvent(event);
}

void LegacyMainWindow::setHelperPath(const QString &path)
{
    m_runner->setHelperPath(path);
    updateElevationLabel();
    updateStatus();
}

void LegacyMainWindow::setElevationOverride(const QString &override)
{
    m_runner->setElevationOverride(override);
    updateElevationLabel();
}

void LegacyMainWindow::setNoElevate(bool noElevate)
{
    m_runner->setNoElevate(noElevate);
    updateElevationLabel();
}

void LegacyMainWindow::setLogDirectory(const QString &path)
{
    m_logDirectory = path;
    // The constructor already opened a session file under the default
    // directory; move to the requested one before the first helper output.
    m_logPath = QString::null;
    if (m_settingsLogDirLabel) {
        m_settingsLogDirLabel->setText(path);
    }
    if (m_settingsSessionLabel) {
        m_settingsSessionLabel->setText(QString::fromLatin1("(not created yet)"));
    }
    refreshSessionLogList();
}

void LegacyMainWindow::setTarget(const QString &disk, const QString &root)
{
    setSelection(disk, QString::null, root);
    selectInventoryRow(disk);
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

// Programmatic selection used by setTarget() and the smoke test. `rootOverride`
// is remembered for `disk` and wins over the inventory auto-resolution until
// the user selects a different row.
void LegacyMainWindow::setSelection(const QString &disk, const QString &component,
                                    const QString &rootOverride)
{
    m_updatingSelection = true;
    m_selectedDisk = disk.stripWhiteSpace();
    m_selectedComponent = component.stripWhiteSpace();
    m_rootOverrideDisk = rootOverride.isEmpty() ? QString::null : m_selectedDisk;
    m_rootOverrideRoot = rootOverride.stripWhiteSpace();
    m_updatingSelection = false;
}

// Selects the inventory row for `disk` (and then the component row when one
// is set) without re-entering the selection handler.
void LegacyMainWindow::selectInventoryRow(const QString &disk)
{
    if (!m_deviceList) {
        return;
    }
    m_updatingSelection = true;
    QListViewItem *match = 0;
    for (QListViewItem *item = m_deviceList->firstChild(); item;
         item = item->nextSibling()) {
        if (item->text(0) == disk) {
            match = item;
            break;
        }
    }
    if (match) {
        m_deviceList->setSelected(match, true);
        m_deviceList->setCurrentItem(match);
    }
    m_updatingSelection = false;
}

QPushButton *LegacyMainWindow::makeButton(const QString &text, QWidget *parent)
{
    QPushButton *button = new QPushButton(text, parent);
    QFontMetrics metrics(button->font());
    button->setMinimumWidth(QMAX(button->minimumWidth(),
                                 metrics.width(text) + kButtonTextPadding));
    button->setMinimumHeight(QMAX(button->minimumHeight(), metrics.height() + 14));
    m_buttons.push_back(button);
    return button;
}

void LegacyMainWindow::updateButtonText(QPushButton *button, const QString &text)
{
    if (!button) {
        return;
    }
    button->setText(text);
    QFontMetrics metrics(button->font());
    button->setMinimumWidth(QMAX(40, metrics.width(text) + kButtonTextPadding));
}

// Modern sectionTitle() parity: 1.2x point size, bold. Qt3 spells the font
// size accessors pointSizeFloat()/setPointSizeFloat() (Qt4's pointSizeF()/
// setPointSizeF() do not exist here). The font-metric floor and the page
// containment check are asserted by the 1024x768 smoke layout gate.
QLabel *LegacyMainWindow::makeSectionTitle(const QString &text, QWidget *parent)
{
    QLabel *label = new QLabel(text, parent);
    QFont font = label->font();
    font.setPointSizeFloat(font.pointSizeFloat() * 1.2);
    font.setBold(true);
    label->setFont(font);
    QFontMetrics metrics(label->font());
    label->setMinimumWidth(metrics.width(text) + kSectionTitlePadding);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_sectionTitles.push_back(label);
    return label;
}

// Qt3 list columns are fixed-width by default, so a header wider than its
// declared column clips. Raise every declared width to the header's font
// metric plus padding; `QListView::LastColumn` then stretches the last column
// to the viewport's right edge.
void LegacyMainWindow::addListViewColumn(QListView *list, const QString &title,
                                         int width)
{
    if (!list) {
        return;
    }
    QFontMetrics metrics(list->font());
    list->addColumn(title, QMAX(width, metrics.width(title) + kListHeaderPadding));
}

void LegacyMainWindow::registerGroupBox(QGroupBox *box)
{
    if (!box) {
        return;
    }
    QFontMetrics metrics(box->font());
    const int titleWidth = metrics.width(box->title())
        + 2 * box->frameWidth() + kGroupTitlePadding;
    box->setMinimumWidth(QMAX(box->minimumWidth(), titleWidth));
    m_groupBoxes.push_back(box);
}

void LegacyMainWindow::buildMenus()
{
    // Modern menu structure (MainWindow::buildMenuBar): File (Refresh
    // Devices, Lock Administrator Session, Quit), View (tab shortcuts,
    // Auto-size Device Columns, checkable Wrap Log Lines) and Help (Using
    // Boot Bitch, About Boot Bitch). The Logs save/clear actions live on the
    // Logs tab only.
    m_fileMenu = new QPopupMenu(this);
    menuBar()->insertItem(QString::fromLatin1("&File"), m_fileMenu);
    m_fileMenu->insertItem(QString::fromLatin1("&Refresh Devices"), this,
                           SLOT(scanDevices()), Key_F5);
    m_fileMenu->insertItem(QString::fromLatin1("&Lock Administrator Session"),
                           this, SLOT(lockAdministratorSession()));
    m_fileMenu->insertSeparator();
    m_fileMenu->insertItem(QString::fromLatin1("&Quit"), qApp, SLOT(quit()),
                           CTRL + Key_Q);

    m_viewMenu = new QPopupMenu(this);
    menuBar()->insertItem(QString::fromLatin1("&View"), m_viewMenu);
    m_viewMenu->insertItem(QString::fromLatin1("&Systems"), this, SLOT(showSystemsTab()));
    m_viewMenu->insertItem(QString::fromLatin1("&Diagnostics"), this, SLOT(showDiagnosticsTab()));
    m_viewMenu->insertItem(QString::fromLatin1("&Logs"), this, SLOT(showLogsTab()));
    m_viewMenu->insertItem(QString::fromLatin1("&Settings"), this, SLOT(showSettingsTab()));
    m_viewMenu->insertSeparator();
    m_viewMenu->insertItem(QString::fromLatin1("&Auto-size Device Columns"),
                           this, SLOT(autoSizeDeviceColumns()));
    m_wrapLogsMenuId = m_viewMenu->insertItem(QString::fromLatin1("&Wrap Log Lines"),
                                              this, SLOT(toggleLogWrapFromMenu()));
    // Qt3 auto-generates negative ids; remember that the item exists instead of
    // testing the id's sign.
    m_wrapLogsMenuValid = true;
    m_viewMenu->setItemChecked(m_wrapLogsMenuId, m_logWrapEnabled);

    m_helpMenu = new QPopupMenu(this);
    menuBar()->insertItem(QString::fromLatin1("&Help"), m_helpMenu);
    m_helpMenu->insertItem(QString::fromLatin1("&Using Boot Bitch"), this,
                           SLOT(showUsageHelp()));
    m_helpMenu->insertSeparator();
    m_helpMenu->insertItem(QString::fromLatin1("&About Boot Bitch"), this,
                           SLOT(showAbout()));
}

void LegacyMainWindow::showSystemsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(0);
    }
}

void LegacyMainWindow::showDiagnosticsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(1);
    }
}

void LegacyMainWindow::showLogsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(5);
    }
}

void LegacyMainWindow::showSettingsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(6);
    }
}

// Modern Auto-size Device Columns: size every column to the widest header or
// cell text plus the header-safe padding floor. The QListView LastColumn
// resize mode keeps stretching the final column to the viewport edge.
void LegacyMainWindow::autoSizeDeviceColumns()
{
    if (!m_deviceList) {
        return;
    }
    const QFontMetrics metrics(m_deviceList->font());
    for (int column = 0; column < m_deviceList->columns(); ++column) {
        int width = metrics.width(m_deviceList->columnText(column)) + kListHeaderPadding;
        for (QListViewItem *item = m_deviceList->firstChild(); item;
             item = item->nextSibling()) {
            width = QMAX(width, metrics.width(item->text(column)) + kListHeaderPadding);
        }
        m_deviceList->setColumnWidth(column, width);
    }
    statusBar()->message(QString::fromLatin1(
        "Device columns auto-sized. Drag headers to fine-tune widths."), 3500);
}

// The View menu item and the Settings checkbox are two views of one state;
// this slot derives the new value from the cached flag (Qt3 QPopupMenu does
// not guarantee an automatic check toggle) and toggleLogWrap() updates both
// controls without re-entering their signals.
void LegacyMainWindow::toggleLogWrapFromMenu()
{
    toggleLogWrap(!m_logWrapEnabled);
}

// Modern File -> Lock Administrator Session: drop the cached elevation
// decision and best-effort clear the sudo timestamp without ever prompting or
// blocking, then refresh the inventory/status so the next privileged action
// asks for the password again.
void LegacyMainWindow::lockAdministratorSession()
{
    if (m_running) {
        statusBar()->message(QString::fromLatin1(
            "A helper command is running; wait for it to finish before locking "
            "the session."), 4000);
        return;
    }
    const bool cleared = m_runner->clearSudoTimestamp();
    m_runner->resetElevation();
    appendLog(QString::fromLatin1(
        "Administrator authorization session locked by the user; the cached "
        "session was dropped and the next privileged action requests the "
        "password again. Any LUKS mapping opened earlier remains open for this "
        "recovery session (the legacy helper exposes no close verb)."));
    if (!cleared) {
        appendLog(QString::fromLatin1(
            "Note: the cached sudo timestamp could not be cleared (sudo is not "
            "installed or is not the elevation method); the cached decision is "
            "still reset."));
    }
    updateElevationLabel();
    updateActionStates();
    scanDevices();
    statusBar()->message(QString::fromLatin1(
        "Administrator session locked; the next privileged action will request "
        "authorization."), 6000);
}

// Modern Help -> Using Boot Bitch, adapted to the legacy frontend. The dialog
// is width-constrained and word-wrapped (Qt3 has no QLabel::setWordWrap).
void LegacyMainWindow::showUsageHelp()
{
    QDialog dialog(this, "legacy-usage-help", true);
    dialog.setCaption(QString::fromLatin1("Using Boot Bitch"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *text = new QLabel(QString::fromLatin1(
        "Boot Bitch must run from a different booted Linux environment than the "
        "system being repaired. Use a Linux live medium or another Linux "
        "installation on a different physical drive.<br><br>"
        "The running host is protected from ordinary repair-target selection, "
        "but it can be explicitly selected through <b>Host Maintenance</b> for "
        "guarded native diagnostics and supported maintenance stages.<br><br>"
        "Diagnostics follow the Systems page: the committed repair drive while "
        "Host Maintenance is off, or the protected running host while it is "
        "active.<br><br>"
        "The first privileged action requests administrator authorization once "
        "for this Boot Bitch window; <b>File - Lock Administrator Session</b> "
        "ends that helper session immediately. Every repair keeps the helper's "
        "own runtime preflights."),
        &dialog);
    text->setTextFormat(Qt::RichText);
    enableLabelWordWrap(text);
    text->setMaximumWidth(560);
    layout->addWidget(text);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->addStretch();
    QPushButton *close = new QPushButton(QString::fromLatin1("Close"), &dialog);
    close->setDefault(true);
    buttons->addWidget(close);
    QObject::connect(close, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    dialog.exec();
}

QWidget *LegacyMainWindow::buildTargetsTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *pageLayout = new QVBoxLayout(page, 8, 6);

    // Modern Systems page heading: title left, Refresh Devices right.
    QHBoxLayout *headingRow = new QHBoxLayout(pageLayout);
    headingRow->setSpacing(6);
    m_systemsHeading = makeSectionTitle(QString::fromLatin1("Systems"), page);
    headingRow->addWidget(m_systemsHeading);
    headingRow->addStretch();
    m_scanButton = makeButton(QString::fromLatin1("Refresh Devices"), page);
    QToolTip::add(m_scanButton, QString::fromLatin1(
        "Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, "
        "/proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev "
        "metadata database). No block device is opened and nothing is written."));
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(scanDevices()));
    headingRow->addWidget(m_scanButton, 0, Qt::AlignTop);

    // "Available repair targets" section title above the drive list (modern
    // candidateRow). The list itself is the read-only kernel inventory.
    QHBoxLayout *targetsRow = new QHBoxLayout(pageLayout);
    targetsRow->setSpacing(6);
    m_targetsHeading = makeSectionTitle(
        QString::fromLatin1("Available repair targets"), page);
    targetsRow->addWidget(m_targetsHeading);
    targetsRow->addStretch();
    QLabel *sortHint = new QLabel(QString::fromLatin1("Most likely first"), page);
    QToolTip::add(sortHint, QString::fromLatin1(
        "Drives are listed by the read-only inventory. Select a row to inspect "
        "it; Select Target commits the selected non-host drive with its "
        "auto-resolved Linux root component."));
    targetsRow->addWidget(sortHint);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    // ---- Left column: inventory, action row, Unlock status ------------------
    QWidget *left = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(left, 0, 6);

    m_deviceList = new QListView(left);
    addListViewColumn(m_deviceList, QString::fromLatin1("Device"), 100);
    addListViewColumn(m_deviceList, QString::fromLatin1("Size"), 60);
    addListViewColumn(m_deviceList, QString::fromLatin1("Type"), 80);
    addListViewColumn(m_deviceList, QString::fromLatin1("Filesystem"), 75);
    addListViewColumn(m_deviceList, QString::fromLatin1("Mount"), 100);
    addListViewColumn(m_deviceList, QString::fromLatin1("Note"), 155);
    m_deviceList->setAllColumnsShowFocus(true);
    m_deviceList->setShowSortIndicator(true);
    m_deviceList->setMultiSelection(false);
    m_deviceList->setResizeMode(QListView::LastColumn);
    m_deviceList->setMinimumHeight(70);
    connect(m_deviceList, SIGNAL(selectionChanged()), this, SLOT(deviceSelectionChanged()));
    leftLayout->addWidget(m_deviceList, 1);

    // Modern button row: Select Target, Unlock, Host Maintenance, the
    // deferred-authorization status + Authorize affordance, then the
    // right-aligned committed-target summary.
    QHBoxLayout *buttonRow = new QHBoxLayout(leftLayout);
    buttonRow->setSpacing(6);
    m_setTargetButton = makeButton(QString::fromLatin1("Select Target"), left);
    m_setTargetButton->setEnabled(false);
    connect(m_setTargetButton, SIGNAL(clicked()), this, SLOT(setRepairTarget()));
    buttonRow->addWidget(m_setTargetButton);

    m_unlockButton = makeButton(QString::fromLatin1("Unlock"), left);
    m_unlockButton->setEnabled(false);
    connect(m_unlockButton, SIGNAL(clicked()), this, SLOT(runUnlock()));
    buttonRow->addWidget(m_unlockButton);

    m_hostMaintenanceButton = makeButton(QString::fromLatin1("Host Maintenance"), left);
    m_hostMaintenanceButton->setEnabled(false);
    connect(m_hostMaintenanceButton, SIGNAL(clicked()), this, SLOT(toggleHostMaintenance()));
    buttonRow->addWidget(m_hostMaintenanceButton);

    m_authStatusLabel = new QLabel(left);
    m_authStatusLabel->setTextFormat(Qt::PlainText);
    m_authStatusLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    buttonRow->addWidget(m_authStatusLabel, 1);

    m_authorizeButton = makeButton(QString::fromLatin1("Authorize"), left);
    m_authorizeButton->setEnabled(false);
    QToolTip::add(m_authorizeButton, QString::fromLatin1(
        "Establish the privileged helper session for the current scope now "
        "instead of waiting for the next privileged action."));
    connect(m_authorizeButton, SIGNAL(clicked()), this, SLOT(authorizeNow()));
    buttonRow->addWidget(m_authorizeButton, 0, Qt::AlignVCenter);

    m_targetSummary = new QLabel(QString::fromLatin1("Committed target: none"), left);
    m_targetSummary->setTextFormat(Qt::PlainText);
    m_targetSummary->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    buttonRow->addWidget(m_targetSummary, 0, Qt::AlignRight | Qt::AlignVCenter);

    // Unlock status sits directly below the button row, exactly like modern.
    // The frame is titleless and the visible title is a sectionTitle label:
    // Qt3's KDE/Etch style clips QGroupBox titles at the top.
    QGroupBox *unlock = new QGroupBox(left);
    QToolTip::add(unlock, QString::fromLatin1(
        "Unlock state for the selected drive; the LUKS passphrase is never logged."));
    QVBoxLayout *unlockLayout = new QVBoxLayout(unlock, 8, 4);
    unlockLayout->addWidget(makeSectionTitle(QString::fromLatin1("Unlock status"), unlock));
    m_unlockStatusView = new QTextEdit(unlock);
    m_unlockStatusView->setReadOnly(true);
    m_unlockStatusView->setTextFormat(Qt::PlainText);
    m_unlockStatusView->setWordWrap(QTextEdit::WidgetWidth);
    m_unlockStatusView->setMinimumHeight(96);
    unlockLayout->addWidget(m_unlockStatusView, 1);
    leftLayout->addWidget(unlock, 1);

    // ---- Right column: selected drive details --------------------------------
    QWidget *right = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(right, 0, 6);

    QGroupBox *details = new QGroupBox(right);
    QToolTip::add(details, QString::fromLatin1(
        "Read-only inventory plus helper-confirmed facts; mirrors the modern "
        "Qt6 Selected drive details panel."));
    QVBoxLayout *detailsLayout = new QVBoxLayout(details, 8, 4);
    detailsLayout->addWidget(makeSectionTitle(QString::fromLatin1("Selected drive details"), details));
    m_detailList = new QListView(details);
    addListViewColumn(m_detailList, QString::fromLatin1("Field"), 120);
    addListViewColumn(m_detailList, QString::fromLatin1("Value"), 220);
    m_detailList->setAllColumnsShowFocus(true);
    m_detailList->setResizeMode(QListView::LastColumn);
    // The documented details field order (Drive, Detected target, Model/label,
    // ...) must not be alphabetized by Qt3's default first-column sorting.
    m_detailList->setSorting(-1);
    m_detailList->setMinimumHeight(120);
    detailsLayout->addWidget(m_detailList, 1);
    rightLayout->addWidget(details, 1);

    registerGroupBox(unlock);
    registerGroupBox(details);

    QValueList<int> sizes;
    sizes.append(620);
    sizes.append(380);
    splitter->setSizes(sizes);
    pageLayout->addWidget(splitter, 1);
    return page;
}

QWidget *LegacyMainWindow::buildDiagnosticsTab()
{
    // Modern parity: heading row (title, scope label, Run All), the
    // target-configuration row (committed target only), then a horizontal
    // splitter with the diagnostic checks list on the left and the Selected
    // diagnostic pane (title, description, availability, Results + Run
    // Diagnostic, results text, Copy/Save Results) on the right.
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_diagHeading = makeSectionTitle(QString::fromLatin1("Diagnostics"), page);
    headingRow->addWidget(m_diagHeading);
    headingRow->addStretch();
    m_scopeLabel = new QLabel(QString::fromLatin1("Target: none selected"), page);
    m_scopeLabel->setTextFormat(Qt::PlainText);
    m_scopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    QToolTip::add(m_scopeLabel, QString::fromLatin1(
        "Diagnostics follow the committed repair target, or the protected "
        "running host while Host Maintenance is active."));
    headingRow->addWidget(m_scopeLabel, 0, Qt::AlignVCenter);
    m_diagnosticsButton = makeButton(QString::fromLatin1("Run All"), page);
    QToolTip::add(m_diagnosticsButton, QString::fromLatin1(
        "Run All - run every available read-only diagnostic for the current scope."));
    connect(m_diagnosticsButton, SIGNAL(clicked()), this, SLOT(runDiagnostics()));
    headingRow->addWidget(m_diagnosticsButton, 0, Qt::AlignVCenter);

    QHBoxLayout *configRow = new QHBoxLayout(layout);
    configRow->setSpacing(6);
    m_configLabel = new QLabel(QString::fromLatin1("Target configuration:"), page);
    configRow->addWidget(m_configLabel);
    m_configCombo = new QComboBox(page);
    m_configCombo->setMinimumWidth(220);
    QToolTip::add(m_configCombo, QString::fromLatin1(
        "Etch-era target configuration files; availability is probed read-only "
        "by the helper's diagnostics."));
    configRow->addWidget(m_configCombo, 1);
    m_configButton = makeButton(QString::fromLatin1("Edit Target File..."), page);
    connect(m_configButton, SIGNAL(clicked()), this, SLOT(editTargetConfigFile()));
    configRow->addWidget(m_configButton);

    m_configReasonLabel = new QLabel(page);
    m_configReasonLabel->setTextFormat(Qt::PlainText);
    m_configReasonLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_configReasonLabel);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    QGroupBox *checks = new QGroupBox(splitter);
    QToolTip::add(checks, QString::fromLatin1(
        "Runs one read-only diagnostic for the selected scope through the "
        "helper (`diagnose <key>` / `host-diagnose <key>`); Run All is the "
        "combined report."));
    QVBoxLayout *checksLayout = new QVBoxLayout(checks, 8, 4);
    checksLayout->addWidget(makeSectionTitle(QString::fromLatin1("Diagnostic checks"), checks));
    m_diagnosticList = new QListView(checks);
    addListViewColumn(m_diagnosticList, QString::fromLatin1("Check"), 130);
    m_diagnosticList->setAllColumnsShowFocus(true);
    m_diagnosticList->setResizeMode(QListView::LastColumn);
    m_diagnosticList->setShowToolTips(true);
    // Qt3 sorts a QListView by the first column by default; disable it so the
    // list keeps the legacy::diagnosticKeys() order (and the smoke's first-
    // entry assertion stays meaningful).
    m_diagnosticList->setSorting(-1);
    m_diagnosticList->setMinimumHeight(150);
    connect(m_diagnosticList, SIGNAL(selectionChanged()),
            this, SLOT(diagnosticSelectionChanged()));
    const std::vector<std::string> diagKeys = legacy::diagnosticKeys();
    m_diagKeyByTitle.clear();
    // Qt3 prepends a plain insertion when sorting is disabled, so every item is
    // chained after the previous one (the after-form constructor) to keep the
    // legacy::diagnosticKeys() order.
    QListViewItem *lastDiagnostic = 0;
    for (std::size_t i = 0; i < diagKeys.size(); ++i) {
        // Modern parity: the list shows the friendly diagnostic title; the
        // stable helper key stays internal (commands and the log use it).
        const QString key = fromStd(diagKeys[i]);
        const QString title = diagnosticTitle(key);
        m_diagKeyByTitle.insert(title, key);
        lastDiagnostic = new QListViewItem(m_diagnosticList, lastDiagnostic, title);
    }
    if (m_diagnosticList->firstChild()) {
        m_diagnosticList->setSelected(m_diagnosticList->firstChild(), true);
        m_diagnosticList->setCurrentItem(m_diagnosticList->firstChild());
    }
    checksLayout->addWidget(m_diagnosticList, 1);

    QGroupBox *detail = new QGroupBox(splitter);
    QVBoxLayout *detailLayout = new QVBoxLayout(detail, 8, 6);
    detailLayout->addWidget(makeSectionTitle(QString::fromLatin1("Selected diagnostic"), detail));
    m_diagTitle = makeSectionTitle(QString::fromLatin1("Select a diagnostic"), detail);
    detailLayout->addWidget(m_diagTitle);
    m_diagDescription = new QLabel(
        QString::fromLatin1("Choose a diagnostic from the list."), detail);
    m_diagDescription->setTextFormat(Qt::PlainText);
    m_diagDescription->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    detailLayout->addWidget(m_diagDescription);
    m_diagAvailability = new QLabel(QString::fromLatin1("Ready"), detail);
    m_diagAvailability->setTextFormat(Qt::PlainText);
    QFont availabilityFont = m_diagAvailability->font();
    availabilityFont.setBold(true);
    m_diagAvailability->setFont(availabilityFont);
    detailLayout->addWidget(m_diagAvailability);

    QHBoxLayout *resultsHeader = new QHBoxLayout(detailLayout);
    resultsHeader->setSpacing(6);
    QLabel *resultsTitle = makeSectionTitle(QString::fromLatin1("Results"), detail);
    resultsHeader->addWidget(resultsTitle);
    resultsHeader->addStretch();
    m_runDiagnosticButton = makeButton(QString::fromLatin1("Run Diagnostic"), detail);
    QToolTip::add(m_runDiagnosticButton, QString::fromLatin1(
        "Run Diagnostic - run the selected read-only diagnostic for the current scope."));
    connect(m_runDiagnosticButton, SIGNAL(clicked()), this, SLOT(runSelectedDiagnostic()));
    resultsHeader->addWidget(m_runDiagnosticButton, 0, Qt::AlignVCenter);

    m_rawView = new QTextEdit(detail);
    m_rawView->setReadOnly(true);
    m_rawView->setTextFormat(Qt::LogText);
    m_rawView->setWordWrap(QTextEdit::NoWrap);
    m_rawView->setMinimumHeight(150);
    detailLayout->addWidget(m_rawView, 1);

    QHBoxLayout *resultButtons = new QHBoxLayout(detailLayout);
    resultButtons->setSpacing(6);
    resultButtons->addStretch();
    m_copyResultsButton = makeButton(QString::fromLatin1("Copy Results"), detail);
    connect(m_copyResultsButton, SIGNAL(clicked()), this, SLOT(copyResults()));
    resultButtons->addWidget(m_copyResultsButton);
    m_saveResultsButton = makeButton(QString::fromLatin1("Save Results..."), detail);
    connect(m_saveResultsButton, SIGNAL(clicked()), this, SLOT(saveResults()));
    resultButtons->addWidget(m_saveResultsButton);

    registerGroupBox(checks);
    registerGroupBox(detail);

    QValueList<int> sizes;
    sizes.append(300);
    sizes.append(700);
    splitter->setSizes(sizes);
    layout->addWidget(splitter, 1);
    return page;
}

QWidget *LegacyMainWindow::buildActionsTab()
{
    // Modern Repair page parity: heading + scope label, the plan paragraph,
    // the "Full Repair plan" section, then a horizontal splitter with the
    // "Individual repair tools" list (Tool | Full Repair) on the left and the
    // "Selected tool" pane (title, run button, description, plan status) on the
    // right, then the legacy-only privilege-elevation frame.
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *pageLayout = new QVBoxLayout(page, 8, 6);

    // The page is scrollable (Qt3 QScrollView, like the Settings tab) because
    // the plan + tools + elevation sections exceed 1024x768.
    QScrollView *scroll = new QScrollView(page);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    scroll->setVScrollBarMode(QScrollView::Auto);
    scroll->setHScrollBarMode(QScrollView::AlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    pageLayout->addWidget(scroll, 1);

    QWidget *content = new QWidget(scroll->viewport());
    scroll->addChild(content);
    m_repairContent = content;
    QVBoxLayout *layout = new QVBoxLayout(content, 6, 6);

    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_repairHeading = makeSectionTitle(QString::fromLatin1("Repair"), content);
    headingRow->addWidget(m_repairHeading);
    headingRow->addStretch();
    m_repairScopeLabel = new QLabel(QString::fromLatin1("Target: none selected"), content);
    m_repairScopeLabel->setTextFormat(Qt::PlainText);
    m_repairScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_repairScopeLabel, 0, Qt::AlignVCenter);

    m_planParagraph = new QLabel(QString::fromLatin1(kRepairPlanParagraph), content);
    m_planParagraph->setTextFormat(Qt::PlainText);
    m_planParagraph->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_planParagraph);

    // ---- Full Repair plan section -------------------------------------------
    QGroupBox *planBox = new QGroupBox(content);
    QVBoxLayout *planLayout = new QVBoxLayout(planBox, 6, 4);
    planLayout->addWidget(makeSectionTitle(QString::fromLatin1("Full Repair plan"), planBox));

    QHBoxLayout *planHeader = new QHBoxLayout(planLayout);
    planHeader->setSpacing(6);
    m_planCountLabel = new QLabel(QString::fromLatin1("No stages selected"), planBox);
    QFont countFont = m_planCountLabel->font();
    countFont.setBold(true);
    m_planCountLabel->setFont(countFont);
    planHeader->addWidget(m_planCountLabel);
    planHeader->addStretch();
    m_configurePlanButton = makeButton(QString::fromLatin1("Configure Plan..."), planBox);
    QToolTip::add(m_configurePlanButton, QString::fromLatin1(
        "Open Settings to choose which Full Repair stages are part of the plan."));
    connect(m_configurePlanButton, SIGNAL(clicked()), this, SLOT(configurePlan()));
    planHeader->addWidget(m_configurePlanButton);
    m_runFullRepairButton = makeButton(QString::fromLatin1("Run Full Repair"), planBox);
    m_runFullRepairButton->setEnabled(false);
    QToolTip::add(m_runFullRepairButton, QString::fromLatin1(
        "Select a repair drive, or choose Host Maintenance on the protected "
        "running-host card."));
    connect(m_runFullRepairButton, SIGNAL(clicked()), this, SLOT(runFullRepair()));
    planHeader->addWidget(m_runFullRepairButton);

    m_planReadinessLabel = new QLabel(QString::fromLatin1(kPlanReadinessDefault), planBox);
    m_planReadinessLabel->setTextFormat(Qt::PlainText);
    m_planReadinessLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    planLayout->addWidget(m_planReadinessLabel);

    m_planStageList = new QListView(planBox);
    addListViewColumn(m_planStageList, QString::fromLatin1("Stage"), 200);
    m_planStageList->setAllColumnsShowFocus(true);
    m_planStageList->setResizeMode(QListView::LastColumn);
    m_planStageList->setSorting(-1);
    m_planStageList->setMinimumHeight(64);
    planLayout->addWidget(m_planStageList, 0);
    layout->addWidget(planBox, 0);

    // ---- Individual tools list + Selected tool pane -------------------------
    QSplitter *splitter = new QSplitter(Qt::Horizontal, content);
    splitter->setChildrenCollapsible(false);

    QGroupBox *tools = new QGroupBox(splitter);
    QVBoxLayout *toolsLayout = new QVBoxLayout(tools, 6, 4);
    toolsLayout->addWidget(makeSectionTitle(QString::fromLatin1("Individual repair tools"), tools));
    m_toolList = new QListView(tools);
    addListViewColumn(m_toolList, QString::fromLatin1("Tool"), 200);
    addListViewColumn(m_toolList, QString::fromLatin1("Full Repair"), 200);
    m_toolList->setAllColumnsShowFocus(true);
    m_toolList->setResizeMode(QListView::LastColumn);
    // Curated workflow order (Qt3's default first-column sorting would reorder
    // it); the after-form insertion keeps that order on the Etch Qt3 style.
    m_toolList->setSorting(-1);
    m_toolList->setMinimumHeight(220);
    connect(m_toolList, SIGNAL(selectionChanged()), this, SLOT(toolSelectionChanged()));
    QListViewItem *lastTool = 0;
    for (int i = 0; i < toolSpecCount; ++i) {
        // The rows stay selectable so the Selected tool pane can explain why a
        // display-only tool cannot run; the run button and the Full Repair
        // column carry the disabled state and the exact reason.
        lastTool = new QListViewItem(m_toolList, lastTool,
                                     QString::fromLatin1(toolSpecs[i].title),
                                     QString::fromLatin1("not reported"));
    }
    if (m_toolList->firstChild()) {
        m_toolList->setSelected(m_toolList->firstChild(), true);
        m_toolList->setCurrentItem(m_toolList->firstChild());
    }
    toolsLayout->addWidget(m_toolList, 1);

    QGroupBox *detail = new QGroupBox(splitter);
    // The Selected tool pane floor keeps its header row usable at 1024x768;
    // long tool titles then wrap (Qt::WordBreak below) instead of forcing the
    // pane wider or clipping like the Qt3 QGroupBox titles did.
    detail->setMinimumWidth(300);
    QVBoxLayout *detailLayout = new QVBoxLayout(detail, 6, 6);
    detailLayout->addWidget(makeSectionTitle(QString::fromLatin1("Selected tool"), detail));
    QHBoxLayout *detailHeader = new QHBoxLayout(detailLayout);
    detailHeader->setSpacing(6);
    m_toolTitle = makeSectionTitle(QString::fromLatin1("Select a repair tool"), detail);
    // Qt3 idiom for a dynamic title that must never clip: Qt::WordBreak lets
    // the label break onto a second line when the pane is too narrow for a
    // long tool name (QLabel has no setWordWrap in Qt 3.3.7).
    m_toolTitle->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    detailHeader->addWidget(m_toolTitle, 0, Qt::AlignVCenter);
    detailHeader->addStretch();
    m_toolRunButton = makeButton(QString::fromLatin1("Run Tool"), detail);
    m_toolRunButton->setEnabled(false);
    QToolTip::add(m_toolRunButton, QString::fromLatin1(
        "Select a repair drive, or choose Host Maintenance on the protected "
        "running-host card."));
    connect(m_toolRunButton, SIGNAL(clicked()), this, SLOT(runSelectedTool()));
    detailHeader->addWidget(m_toolRunButton, 0, Qt::AlignVCenter);
    m_toolDescription = new QLabel(
        QString::fromLatin1("Select a tool to review its repair action."), detail);
    m_toolDescription->setTextFormat(Qt::PlainText);
    m_toolDescription->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    detailLayout->addWidget(m_toolDescription);
    m_toolPlanStatus = new QLabel(detail);
    m_toolPlanStatus->setTextFormat(Qt::PlainText);
    m_toolPlanStatus->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    QFont planStatusFont = m_toolPlanStatus->font();
    planStatusFont.setBold(true);
    m_toolPlanStatus->setFont(planStatusFont);
    detailLayout->addWidget(m_toolPlanStatus);
    detailLayout->addStretch();

    QValueList<int> splitterSizes;
    splitterSizes.append(620);
    splitterSizes.append(380);
    splitter->setSizes(splitterSizes);
    layout->addWidget(splitter, 0);

    // ---- Legacy-only privilege elevation (with the Authorize control) -------
    QGroupBox *elevation = new QGroupBox(content);
    QVBoxLayout *elevationLayout = new QVBoxLayout(elevation, 6, 4);
    elevationLayout->addWidget(makeSectionTitle(QString::fromLatin1("Privilege elevation"), elevation));
    m_elevationLabel = new QLabel(elevation);
    m_elevationLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    elevationLayout->addWidget(m_elevationLabel);
    QHBoxLayout *elevationButtons = new QHBoxLayout(elevationLayout);
    elevationButtons->setSpacing(6);
    m_elevateButton = makeButton(QString::fromLatin1("Re-check elevation"), elevation);
    connect(m_elevateButton, SIGNAL(clicked()), this, SLOT(recheckElevation()));
    elevationButtons->addWidget(m_elevateButton);
    m_repairAuthStatusLabel = new QLabel(elevation);
    m_repairAuthStatusLabel->setTextFormat(Qt::PlainText);
    m_repairAuthStatusLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    elevationButtons->addWidget(m_repairAuthStatusLabel, 1);
    m_repairAuthorizeButton = makeButton(QString::fromLatin1("Authorize"), elevation);
    m_repairAuthorizeButton->setEnabled(false);
    QToolTip::add(m_repairAuthorizeButton, QString::fromLatin1(
        "Re-establish the privileged helper session for the current scope "
        "without leaving the Repair tab."));
    connect(m_repairAuthorizeButton, SIGNAL(clicked()), this, SLOT(authorizeNow()));
    elevationButtons->addWidget(m_repairAuthorizeButton, 0, Qt::AlignVCenter);
    layout->addWidget(elevation, 0);

    m_gateHint = new QLabel(content);
    m_gateHint->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_gateHint);

    QLabel *note = new QLabel(
        QString::fromLatin1(
            "Write actions ask for confirmation and then run the helper's own "
            "runtime preflights; the GUI never weakens them. A repair that is "
            "not proven 'unchanged' invalidates the cached diagnostics and "
            "disables the gated actions until diagnostics run again."),
        content);
    note->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(note);
    layout->addStretch();

    registerGroupBox(planBox);
    registerGroupBox(tools);
    registerGroupBox(detail);
    registerGroupBox(elevation);
    return page;
}

QWidget *LegacyMainWindow::buildChrootShellTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 10, 8);

    // Modern parity: the heading and tab label switch between "Chroot shell"
    // (offline target) and "Host shell" (Host Maintenance) in
    // updateChrootShellMode(); the scope label mirrors the modern page.
    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_chrootHeading = makeSectionTitle(QString::fromLatin1("Chroot shell"), page);
    headingRow->addWidget(m_chrootHeading);
    headingRow->addStretch();
    m_chrootScopeLabel = new QLabel(QString::fromLatin1("Target: none selected"), page);
    m_chrootScopeLabel->setTextFormat(Qt::PlainText);
    m_chrootScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_chrootScopeLabel, 0, Qt::AlignVCenter);

    m_chrootReasonLabel = new QLabel(
        QString::fromLatin1(
            "No 'Legacy feature shell:' line is cached; run diagnostics for "
            "the selected scope to evaluate the helper's chroot/timeout "
            "containment probes (fail closed)."),
        page);
    m_chrootReasonLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_chrootReasonLabel);

    m_chrootGroup = new QGroupBox(page);
    QVBoxLayout *shellLayout = new QVBoxLayout(m_chrootGroup, 8, 6);
    m_chrootCommandHeading = makeSectionTitle(QString::fromLatin1("Command"), m_chrootGroup);
    shellLayout->addWidget(m_chrootCommandHeading);

    QHBoxLayout *commandRow = new QHBoxLayout(shellLayout);
    commandRow->setSpacing(6);
    commandRow->addWidget(new QLabel(QString::fromLatin1("Command:"), m_chrootGroup));
    m_shellCommandEdit = new QLineEdit(m_chrootGroup);
    m_shellCommandEdit->setEnabled(false);
    m_shellCommandEdit->setText(QString::null);
    QToolTip::add(m_shellCommandEdit, QString::fromLatin1(
        "One reviewed command string, passed to the helper as a single "
        "argument (no shell interpolation by the GUI)."));
    commandRow->addWidget(m_shellCommandEdit, 1);
    m_shellRunButton = makeButton(QString::fromLatin1("Run Command"), m_chrootGroup);
    m_shellRunButton->setEnabled(false);
    connect(m_shellRunButton, SIGNAL(clicked()), this, SLOT(runChrootShell()));
    commandRow->addWidget(m_shellRunButton);
    m_shellClearButton = makeButton(QString::fromLatin1("Clear Output"), m_chrootGroup);
    m_shellClearButton->setEnabled(false);
    connect(m_shellClearButton, SIGNAL(clicked()), this, SLOT(clearChrootOutput()));
    commandRow->addWidget(m_shellClearButton);

    m_shellOutput = new QTextEdit(m_chrootGroup);
    m_shellOutput->setReadOnly(true);
    m_shellOutput->setTextFormat(Qt::LogText);
    m_shellOutput->setMinimumHeight(120);
    m_shellOutput->setText(QString::fromLatin1(
        "The helper exposes `shell <disk> <root> <command>` for an offline "
        "target chroot and `host-shell <disk> <root> <command>` for the "
        "running host. Both keep the helper's runtime preflights; this tab "
        "enables the command only when the scope is committed, the session is "
        "authorized and the scope's Legacy feature probe reports available."));
    shellLayout->addWidget(m_shellOutput, 1);

    registerGroupBox(m_chrootGroup);
    layout->addWidget(m_chrootGroup, 1);
    return page;
}

QWidget *LegacyMainWindow::buildFileCopyTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    m_fileCopyHeading = makeSectionTitle(QString::fromLatin1("File copy"), page);
    layout->addWidget(m_fileCopyHeading);

    m_fileCopyGroup = new QGroupBox(page);
    QVBoxLayout *copyLayout = new QVBoxLayout(m_fileCopyGroup, 8, 4);

    m_fileCopyReasonLabel = new QLabel(
        QString::fromLatin1(
            "No 'Legacy feature file-copy:' line is cached; run diagnostics "
            "for the selected scope to evaluate the helper's rsync/containment "
            "probes (fail closed)."),
        m_fileCopyGroup);
    m_fileCopyReasonLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    copyLayout->addWidget(m_fileCopyReasonLabel);

    QHBoxLayout *body = new QHBoxLayout(copyLayout);
    body->setSpacing(8);
    QVBoxLayout *sources = new QVBoxLayout(body);
    sources->addWidget(new QLabel(QString::fromLatin1("Sources:"), m_fileCopyGroup));
    QListView *sourceList = new QListView(m_fileCopyGroup);
    sourceList->addColumn(QString::fromLatin1("Source"), 150);
    sourceList->setEnabled(false);
    sourceList->setMinimumHeight(90);
    sources->addWidget(sourceList, 1);

    QVBoxLayout *controls = new QVBoxLayout(body);
    controls->addWidget(new QLabel(QString::fromLatin1("Destination:"), m_fileCopyGroup));
    QLineEdit *destination = new QLineEdit(m_fileCopyGroup);
    destination->setEnabled(false);
    destination->setText(QString::fromLatin1("unavailable: see the helper's Legacy feature file-copy: probe reason above"));
    controls->addWidget(destination);
    QPushButton *addFiles = makeButton(QString::fromLatin1("Add files..."), m_fileCopyGroup);
    addFiles->setEnabled(false);
    controls->addWidget(addFiles);
    QPushButton *addFolder = makeButton(QString::fromLatin1("Add folder..."), m_fileCopyGroup);
    addFolder->setEnabled(false);
    controls->addWidget(addFolder);
    QPushButton *remove = makeButton(QString::fromLatin1("Remove selected"), m_fileCopyGroup);
    remove->setEnabled(false);
    controls->addWidget(remove);
    QPushButton *runCopy = makeButton(QString::fromLatin1("Copy"), m_fileCopyGroup);
    runCopy->setEnabled(false);
    controls->addWidget(runCopy);
    controls->addStretch();

    registerGroupBox(m_fileCopyGroup);
    layout->addWidget(m_fileCopyGroup, 1);
    return page;
}

QWidget *LegacyMainWindow::buildLogTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 4);

    m_logsHeading = makeSectionTitle(QString::fromLatin1("Logs"), page);
    layout->addWidget(m_logsHeading);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    QGroupBox *sessions = new QGroupBox(splitter);
    QVBoxLayout *sessionLayout = new QVBoxLayout(sessions, 8, 4);
    sessionLayout->addWidget(makeSectionTitle(QString::fromLatin1("Session logs"), sessions));
    m_sessionLogList = new QListView(sessions);
    addListViewColumn(m_sessionLogList, QString::fromLatin1("Session"), 150);
    m_sessionLogList->setAllColumnsShowFocus(true);
    m_sessionLogList->setResizeMode(QListView::LastColumn);
    m_sessionLogList->setMinimumHeight(110);
    QToolTip::add(m_sessionLogList, QString::fromLatin1(
        "The first entry is the live session; earlier files in the log "
        "directory are listed read-only below it."));
    connect(m_sessionLogList, SIGNAL(selectionChanged()), this, SLOT(sessionLogSelectionChanged()));
    sessionLayout->addWidget(m_sessionLogList, 1);

    // Modern session controls (two compact rows so the narrow frame fits):
    // New Session Log, Add Note, Delete (prior files only) and Refresh.
    QGridLayout *sessionButtons = new QGridLayout(sessionLayout, 2, 2, 4);
    sessionButtons->setMargin(4);
    m_newSessionLogButton = makeButton(QString::fromLatin1("New Session Log"), sessions);
    QToolTip::add(m_newSessionLogButton, QString::fromLatin1(
        "Close the active session file; it becomes a prior session and the next "
        "log entry starts a new file."));
    connect(m_newSessionLogButton, SIGNAL(clicked()), this, SLOT(startNewSessionLog()));
    sessionButtons->addWidget(m_newSessionLogButton, 0, 0);
    m_addNoteButton = makeButton(QString::fromLatin1("Add Note"), sessions);
    QToolTip::add(m_addNoteButton, QString::fromLatin1(
        "Append a NOTE entry to the live session register."));
    connect(m_addNoteButton, SIGNAL(clicked()), this, SLOT(addSessionNote()));
    sessionButtons->addWidget(m_addNoteButton, 0, 1);
    m_deleteSessionLogButton = makeButton(QString::fromLatin1("Delete"), sessions);
    m_deleteSessionLogButton->setEnabled(false);
    QToolTip::add(m_deleteSessionLogButton, QString::fromLatin1(
        "Delete the selected prior session file (the live session is never "
        "deleted)."));
    connect(m_deleteSessionLogButton, SIGNAL(clicked()), this, SLOT(deleteSelectedSessionLog()));
    sessionButtons->addWidget(m_deleteSessionLogButton, 1, 0);
    QPushButton *refresh = makeButton(QString::fromLatin1("Refresh"), sessions);
    connect(refresh, SIGNAL(clicked()), this, SLOT(refreshSessionLogs()));
    sessionButtons->addWidget(refresh, 1, 1);

    QGroupBox *applicationLog = new QGroupBox(splitter);
    QVBoxLayout *logLayout = new QVBoxLayout(applicationLog, 8, 4);
    logLayout->addWidget(makeSectionTitle(QString::fromLatin1("Application log"), applicationLog));

    QHBoxLayout *filterRow = new QHBoxLayout(logLayout);
    filterRow->setSpacing(4);
    filterRow->addWidget(new QLabel(QString::fromLatin1("Search log:"), applicationLog));
    m_logSearchEdit = new QLineEdit(applicationLog);
    m_logSearchEdit->setText(QString::null);
    QToolTip::add(m_logSearchEdit, QString::fromLatin1(
        "Type any characters to show matching log entries (case-insensitive). "
        "Save As always writes every entry."));
    connect(m_logSearchEdit, SIGNAL(textChanged(const QString &)), this, SLOT(logSearchChanged()));
    filterRow->addWidget(m_logSearchEdit, 1);
    filterRow->addWidget(new QLabel(QString::fromLatin1("Filter:"), applicationLog));
    m_logFilterCombo = new QComboBox(applicationLog);
    m_logFilterCombo->insertItem(QString::fromLatin1("All entries"));
    m_logFilterCombo->insertItem(QString::fromLatin1("Errors and warnings"));
    QToolTip::add(m_logFilterCombo, QString::fromLatin1(
        "Filter the visible log; Save As always writes every entry."));
    connect(m_logFilterCombo, SIGNAL(activated(int)), this, SLOT(logFilterChanged()));
    filterRow->addWidget(m_logFilterCombo);

    // Modern prior-log banner: shown only while a prior session file is
    // selected (read-only).
    m_priorLogBanner = new QLabel(applicationLog);
    m_priorLogBanner->setFrameShape(QFrame::StyledPanel);
    m_priorLogBanner->setMargin(4);
    m_priorLogBanner->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    m_priorLogBanner->hide();
    logLayout->addWidget(m_priorLogBanner);

    m_logView = new QTextEdit(applicationLog);
    m_logView->setReadOnly(true);
    m_logView->setTextFormat(Qt::LogText);
    m_logView->setWordWrap(m_logWrapEnabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    m_logView->setMinimumHeight(110);
    logLayout->addWidget(m_logView, 1);

    QHBoxLayout *buttons = new QHBoxLayout(logLayout);
    buttons->setSpacing(4);
    QPushButton *save = makeButton(QString::fromLatin1("Save As..."), applicationLog);
    QToolTip::add(save, QString::fromLatin1("Save the complete session log (all entries, not just the current filter)."));
    connect(save, SIGNAL(clicked()), this, SLOT(saveLog()));
    buttons->addWidget(save);
    QPushButton *clear = makeButton(QString::fromLatin1("Clear Register"), applicationLog);
    QToolTip::add(clear, QString::fromLatin1(
        "Clear the live register and view; prior session files are never "
        "modified."));
    connect(clear, SIGNAL(clicked()), this, SLOT(clearLog()));
    buttons->addWidget(clear);
    buttons->addStretch();

    registerGroupBox(sessions);
    registerGroupBox(applicationLog);
    QValueList<int> sizes;
    sizes.append(220);
    sizes.append(760);
    splitter->setSizes(sizes);
    layout->addWidget(splitter, 1);
    return page;
}

QWidget *LegacyMainWindow::buildSettingsTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *pageLayout = new QVBoxLayout(page, 8, 6);

    m_settingsHeading = makeSectionTitle(QString::fromLatin1("Settings"), page);
    pageLayout->addWidget(m_settingsHeading);

    // Modern Settings is a scrollable page. Qt3 has no QScrollArea, so the
    // Qt3 QScrollView hosts the content; AutoOneFit keeps the content at the
    // viewport width and only scrolls vertically when the groups exceed the
    // 1024x768 contract height.
    QScrollView *scroll = new QScrollView(page);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    scroll->setVScrollBarMode(QScrollView::Auto);
    scroll->setHScrollBarMode(QScrollView::AlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    pageLayout->addWidget(scroll, 1);

    QWidget *content = new QWidget(scroll->viewport());
    scroll->addChild(content);
    m_settingsContent = content;
    QVBoxLayout *layout = new QVBoxLayout(content, 6, 6);

    // Modern parity: device discovery, the Full Repair plan, diagnostics and
    // the mandatory safety list in the modern order, then the host
    // capabilities section; the legacy-only read-only application
    // configuration closes the page. Every frame is titleless with a
    // sectionTitle label inside: Qt3's KDE/Etch style clips QGroupBox titles
    // at the top.
    QGroupBox *discovery = new QGroupBox(content);
    QVBoxLayout *discoveryLayout = new QVBoxLayout(discovery, 6, 3);
    discoveryLayout->addWidget(makeSectionTitle(QString::fromLatin1("Device discovery"), discovery));
    m_showNonLinuxCheck = new QCheckBox(
        QString::fromLatin1("Show devices without an identified Linux installation"), discovery);
    m_showRemovableCheck = new QCheckBox(
        QString::fromLatin1("Show removable and USB storage"), discovery);
    m_showEncryptedCheck = new QCheckBox(
        QString::fromLatin1("Show encrypted devices before unlocking"), discovery);
    QToolTip::add(m_showNonLinuxCheck, QString::fromLatin1(
        "When off, drives without a visible Linux filesystem are hidden unless "
        "they still contain an encrypted device and encrypted devices are shown."));
    QToolTip::add(m_showRemovableCheck, QString::fromLatin1(
        "When off, removable and USB drives are hidden from the repair-target list."));
    QToolTip::add(m_showEncryptedCheck, QString::fromLatin1(
        "When off, drives with an encrypted device are hidden until the volume "
        "is unlocked."));
    discoveryLayout->addWidget(m_showNonLinuxCheck);
    discoveryLayout->addWidget(m_showRemovableCheck);
    discoveryLayout->addWidget(m_showEncryptedCheck);
    connect(m_showNonLinuxCheck, SIGNAL(toggled(bool)), this, SLOT(deviceFilterChanged()));
    connect(m_showRemovableCheck, SIGNAL(toggled(bool)), this, SLOT(deviceFilterChanged()));
    connect(m_showEncryptedCheck, SIGNAL(toggled(bool)), this, SLOT(deviceFilterChanged()));
    layout->addWidget(discovery);

    // Modern Settings -> Full Repair plan. The checkboxes drive the Repair
    // plan section, the tools "Full Repair" column and Run Full Repair.
    QGroupBox *planGroup = new QGroupBox(content);
    QVBoxLayout *planGroupLayout = new QVBoxLayout(planGroup, 6, 3);
    planGroupLayout->addWidget(makeSectionTitle(QString::fromLatin1("Full Repair plan"), planGroup));
    m_planChecks.clear();
    for (int i = 0; i < planSpecCount; ++i) {
        QCheckBox *check = new QCheckBox(QString::fromLatin1(planSpecs[i].checkLabel), planGroup);
        check->setChecked(planSpecs[i].defaultChecked);
        QToolTip::add(check, QString::fromLatin1(
            "Include the %1 stage in the Full Repair plan. The stage runs in "
            "the plan order shown on the Repair tab.").arg(QString::fromLatin1(planSpecs[i].title)));
        connect(check, SIGNAL(toggled(bool)), this, SLOT(planCheckboxChanged()));
        planGroupLayout->addWidget(check);
        m_planChecks.push_back(check);
    }
    layout->addWidget(planGroup);

    QGroupBox *diagnostics = new QGroupBox(content);
    QVBoxLayout *diagnosticsLayout = new QVBoxLayout(diagnostics, 6, 3);
    diagnosticsLayout->addWidget(makeSectionTitle(QString::fromLatin1("Diagnostics"), diagnostics));
    m_autoRefreshCheck = new QCheckBox(
        QString::fromLatin1("Automatically regenerate read-only diagnostics after repairs or target changes"),
        diagnostics);
    QToolTip::add(m_autoRefreshCheck, QString::fromLatin1(
        "Regenerates the cached read-only diagnostics for the current scope "
        "after an operation that invalidates them (LUKS unlock, target "
        "configuration edit). It runs only inside an already authorized "
        "administrator session and never opens an authorization prompt by "
        "itself."));
    connect(m_autoRefreshCheck, SIGNAL(toggled(bool)), this, SLOT(autoRefreshToggled(bool)));
    diagnosticsLayout->addWidget(m_autoRefreshCheck);
    layout->addWidget(diagnostics);

    QGroupBox *logs = new QGroupBox(content);
    QVBoxLayout *logsLayout = new QVBoxLayout(logs, 6, 3);
    logsLayout->addWidget(makeSectionTitle(QString::fromLatin1("Logs"), logs));
    m_logWrapCheck = new QCheckBox(QString::fromLatin1("Wrap long log lines"), logs);
    connect(m_logWrapCheck, SIGNAL(toggled(bool)), this, SLOT(toggleLogWrap(bool)));
    logsLayout->addWidget(m_logWrapCheck);
    layout->addWidget(logs);

    QGroupBox *safety = new QGroupBox(content);
    QVBoxLayout *safetyLayout = new QVBoxLayout(safety, 6, 3);
    safetyLayout->addWidget(makeSectionTitle(QString::fromLatin1("Mandatory safety controls"), safety));
    const char *safetyItems[] = {
        "Protect every physical device backing the running host root",
        "Require explicit confirmation before package installation or repair actions",
        "Require mapper/crypttab consistency before initramfs rebuild",
        "Never log LUKS passphrases or authentication secrets",
        "Keep every helper runtime preflight active"
    };
    for (int i = 0; i < 5; ++i) {
        QCheckBox *check = new QCheckBox(QString::fromLatin1(safetyItems[i]), safety);
        check->setChecked(true);
        check->setEnabled(false);
        safetyLayout->addWidget(check);
    }
    layout->addWidget(safety);

    // Modern Host capabilities and dependencies (read-only): the identity and
    // the helper's backend-profile lines parsed from the cached diagnostics.
    QGroupBox *capability = new QGroupBox(content);
    QGridLayout *capabilityGrid = new QGridLayout(capability, 10, 2, 6, 4);
    capabilityGrid->setColStretch(1, 1);
    capabilityGrid->addMultiCellWidget(
        makeSectionTitle(QString::fromLatin1("Host capabilities and dependencies"), capability),
        0, 0, 0, 1, Qt::AlignLeft);
    m_capDistributionLabel = new QLabel(capability);
    m_capPackageManagerLabel = new QLabel(capability);
    m_capServiceLabel = new QLabel(capability);
    m_capDisplayLabel = new QLabel(capability);
    m_capInitramfsLabel = new QLabel(capability);
    m_capBootloaderLabel = new QLabel(capability);
    m_capLoggingLabel = new QLabel(capability);
    QLabel *capValues[] = {
        m_capDistributionLabel, m_capPackageManagerLabel, m_capServiceLabel,
        m_capDisplayLabel, m_capInitramfsLabel, m_capBootloaderLabel,
        m_capLoggingLabel
    };
    const char *capNames[] = {
        "Distribution:", "Package manager family:", "Service manager:",
        "Display manager backend:", "Initramfs backend(s):",
        "Bootloader backend:", "Logging backend:"
    };
    for (int i = 0; i < 7; ++i) {
        capValues[i]->setTextFormat(Qt::PlainText);
        capValues[i]->setAlignment(Qt::WordBreak | Qt::AlignLeft);
        capabilityGrid->addWidget(new QLabel(QString::fromLatin1(capNames[i]), capability), i + 1, 0);
        capabilityGrid->addWidget(capValues[i], i + 1, 1);
    }
    m_refreshCapabilitiesButton = makeButton(
        QString::fromLatin1("Refresh Capabilities"), capability);
    connect(m_refreshCapabilitiesButton, SIGNAL(clicked()), this, SLOT(runDiagnostics()));
    capabilityGrid->addMultiCellWidget(m_refreshCapabilitiesButton, 8, 8, 0, 0,
                                       Qt::AlignLeft);
    QLabel *installNote = new QLabel(
        QString::fromLatin1(
            "Package installation is not offered: host installs are forbidden "
            "in this frontend. Missing tools stay greyed with the helper's own "
            "probe reason."),
        capability);
    installNote->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    capabilityGrid->addMultiCellWidget(installNote, 8, 8, 1, 1);
    layout->addWidget(capability);

    QGroupBox *application = new QGroupBox(content);
    QGridLayout *appGrid = new QGridLayout(application, 6, 2, 6, 4);
    appGrid->setColStretch(1, 1);
    appGrid->addMultiCellWidget(
        makeSectionTitle(QString::fromLatin1("Application configuration"), application),
        0, 0, 0, 1, Qt::AlignLeft);
    m_settingsHelperLabel = new QLabel(application);
    m_settingsElevationLabel = new QLabel(application);
    m_settingsLogDirLabel = new QLabel(application);
    m_settingsSessionLabel = new QLabel(application);
    m_settingsVersionLabel = new QLabel(QString::fromLatin1(LEGACY_VERSION), application);
    QLabel *valueLabels[] = { m_settingsHelperLabel, m_settingsElevationLabel,
                              m_settingsLogDirLabel, m_settingsSessionLabel,
                              m_settingsVersionLabel };
    const char *fieldNames[] = { "Helper:", "Elevation:", "Log directory:",
                                 "Current session log:", "Version:" };
    for (int i = 0; i < 5; ++i) {
        valueLabels[i]->setTextFormat(Qt::PlainText);
        valueLabels[i]->setAlignment(Qt::WordBreak | Qt::AlignLeft);
        appGrid->addWidget(new QLabel(QString::fromLatin1(fieldNames[i]), application), i + 1, 0);
        appGrid->addWidget(valueLabels[i], i + 1, 1);
    }
    layout->addWidget(application);
    layout->addStretch();

    registerGroupBox(discovery);
    registerGroupBox(planGroup);
    registerGroupBox(diagnostics);
    registerGroupBox(logs);
    registerGroupBox(safety);
    registerGroupBox(capability);
    registerGroupBox(application);
    return page;
}

QWidget *LegacyMainWindow::buildAboutTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 12, 8);

    QLabel *about = new QLabel(
        QString::fromLatin1(
            "<b>Boot Bitch Legacy</b> - Qt3 frontend for Debian Etch / KDE 3.5-era systems.<br><br>"
            "This GUI is the package's entry point and a thin client for the "
            "ported privileged helper "
            "(<tt>/usr/sbin/boot-repair-legacy-helper</tt>). It renders the "
            "helper's read-only diagnostics, greys unavailable tools with the "
            "helper's own probe reason, and only enables commands whose "
            "capability line says <tt>available</tt>. The LUKS passphrase "
            "travels only over the helper's standard input; a plain sudo "
            "password is requested in a modal hidden-input dialog and fed to "
            "<tt>sudo -S -v</tt> over a pipe.<br><br>"
            "<b>Modern GUI parity:</b> Systems (Available repair targets, "
            "Select Target with an auto-resolved Linux root, Unlock, Host "
            "Maintenance, Authorize, selected drive details, unlock status), "
            "Diagnostics (Run All + per-key checks, Selected diagnostic pane, "
            "Results with Copy/Save, and the target-only "
            "Edit Target File control with the Etch-era configuration files), "
            "Repair (gated tools, including the guarded GRUB-legacy "
            "regeneration, + elevation state and Authorize), Chroot Shell / "
            "Host Shell and File Copy "
            "(greyed with the helper's <tt>Legacy feature</tt> probe reasons), "
            "Logs (search, all/errors filter, session management and prior-log "
            "banner) and Settings (functional device filters, diagnostics "
            "auto-refresh, read-only host capabilities and configuration) "
            "mirror the Qt6 "
            "hierarchy, and the File/View/Help menus follow the modern "
            "structure. Snapshots, host default/reboot and the Full Repair "
            "plan stay deliberately omitted."),
        page);
    about->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(about);

    QLabel *licence = new QLabel(
        QString::fromLatin1(
            "Project code is MIT. This binary links Qt 3.3.x, which is "
            "distributed under the GPL-2; the legacy package carries that "
            "notice. The artifact is experimental and separate from the modern "
            "Qt6 boot-repair release."),
        page);
    licence->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(licence);
    layout->addStretch();
    return page;
}

void LegacyMainWindow::scanDevices()
{
    // The member slot shadows the namespace-level scanDevices(), so the
    // inventory function is called with its explicit namespace qualification.
    // The complete inventory is cached in m_rows/m_inventory; the visible list
    // is a filtered projection so a hidden row (device-discovery filter) can
    // never invalidate the selected/committed target state.
    m_inventory = legacy::scanDevices();
    m_rows.clear();
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        m_rows.insert(fromStd(m_inventory[i].path), m_inventory[i]);
    }
    rebuildDeviceList();
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

// Rebuilds the visible device list from the cached inventory, applying the
// Settings device-discovery filters. The selected disk is re-selected when it
// is still visible; when its row is hidden the selection/commit state stays
// intact (the details pane, target summary and gated actions keep working).
void LegacyMainWindow::rebuildDeviceList()
{
    if (!m_deviceList) {
        return;
    }
    const QString previousDisk = m_selectedDisk;
    m_deviceList->clear();
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        const DeviceRow &row = m_inventory[i];
        if (!deviceRowVisible(row)) {
            continue;
        }
        QString type;
        QString note = fromStd(row.model);
        if (row.disk) {
            type = row.optical ? QString::fromLatin1("optical") : QString::fromLatin1("disk");
        } else if (row.mapper) {
            type = QString::fromLatin1("mapper");
            if (!row.parent.empty()) {
                note = QString::fromLatin1("backed by ") + fromStd(row.parent);
            }
        } else {
            type = QString::fromLatin1("partition");
            if (!row.parent.empty()) {
                note = QString::fromLatin1("on ") + fromStd(row.parent);
            }
        }
        if (row.encrypted) {
            note = QString::fromLatin1("LUKS container") +
                (note.isEmpty() ? QString::fromLatin1("")
                                : QString::fromLatin1("; ") + note);
        }
        new QListViewItem(m_deviceList, fromStd(row.path), fromStd(row.size),
                          type, displayFsType(row), fromStd(row.mountpoint),
                          note);
    }
    if (!previousDisk.isEmpty()) {
        selectInventoryRow(previousDisk);
    }
}

// Modern devicePassesTopLevelFilters() applied to the flat legacy list: the
// filters act on the owning drive, so a hidden drive hides its partitions and
// mappers too. A row whose owning drive cannot be resolved stays visible
// (fail open for inspection only; gating is unaffected).
bool LegacyMainWindow::deviceRowVisible(const DeviceRow &row) const
{
    if (!m_showNonLinuxCheck || !m_showRemovableCheck || !m_showEncryptedCheck) {
        return true;
    }
    const QString diskPath = row.disk ? fromStd(row.path) : owningDiskFor(row);
    if (diskPath.isEmpty()) {
        return true;
    }
    const QMap<QString, DeviceRow>::const_iterator diskIt = m_rows.find(diskPath);
    if (diskIt == m_rows.end()) {
        return true;
    }
    const DeviceRow &disk = diskIt.data();
    const bool removableOrUsb = disk.transport == std::string("removable")
        || disk.transport == std::string("USB");
    if (!m_showRemovableCheck->isChecked() && removableOrUsb) {
        return false;
    }
    const bool encryptedTree = diskHasEncryptedRow(diskPath);
    if (!m_showEncryptedCheck->isChecked() && encryptedTree) {
        return false;
    }
    const bool linuxCandidate = diskHasLinuxCandidate(diskPath);
    if (!m_showNonLinuxCheck->isChecked() && !linuxCandidate
        && !(encryptedTree && m_showEncryptedCheck->isChecked())) {
        return false;
    }
    return true;
}

bool LegacyMainWindow::rowBelongsToDisk(const DeviceRow &row, const QString &diskPath) const
{
    if (row.disk) {
        return fromStd(row.path) == diskPath;
    }
    if (row.mapper) {
        return owningDiskFor(row) == diskPath;
    }
    const QMap<QString, DeviceRow>::const_iterator diskIt = m_rows.find(diskPath);
    if (diskIt == m_rows.end()) {
        return false;
    }
    return row.parent == diskIt.data().name;
}

bool LegacyMainWindow::diskHasEncryptedRow(const QString &diskPath) const
{
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        if (it.data().encrypted && rowBelongsToDisk(it.data(), diskPath)) {
            return true;
        }
    }
    return false;
}

bool LegacyMainWindow::diskHasLinuxCandidate(const QString &diskPath) const
{
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!rowBelongsToDisk(row, diskPath)) {
            continue;
        }
        if (legacy::isLinuxFileSystemName(row.fstype)
            || legacy::isLinuxFileSystemName(row.probedFstype)) {
            return true;
        }
    }
    return false;
}

// Modern parity: selecting a drive row inspects the drive and resolves its
// best Linux root automatically; selecting a partition/mapper row inspects
// that component while keeping its owning drive as the target candidate.
void LegacyMainWindow::deviceSelectionChanged()
{
    if (m_updatingSelection) {
        return;
    }
    QListViewItem *item = m_deviceList->currentItem();
    if (!item) {
        return;
    }
    const QString path = item->text(0);
    QMap<QString, DeviceRow>::const_iterator it = m_rows.find(path);
    if (it == m_rows.end()) {
        return;
    }
    const DeviceRow &row = it.data();
    if (row.disk) {
        setSelection(path, QString::null, QString::null);
    } else {
        const QString disk = owningDiskFor(row);
        setSelection(disk, path, QString::null);
    }
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::setRepairTarget()
{
    const QString disk = selectedDisk();
    if (disk.isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Select a physical drive in the Available repair targets list first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!runningHostDisk().isEmpty() && disk == runningHostDisk()) {
        QMessageBox::warning(this, QString::fromLatin1("Protected system"),
                             QString::fromLatin1(
                                 "The running system cannot be selected as a repair "
                                 "target. Use Host Maintenance for the protected "
                                 "running host or choose another disk."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString root = selectedRoot();
    if (root.isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("No Linux root component"),
                             QString::fromLatin1(
                                 "No Linux root component was detected on %1. "
                                 "Unlock the encrypted volume first; Select Target "
                                 "becomes available after a Linux filesystem is "
                                 "detected.").arg(disk),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Never resolve the protected running host as a target, even through an
    // inherited component path.
    const QString hostRoot = runningHostRoot();
    if (!hostRoot.isEmpty() && root == hostRoot) {
        QMessageBox::warning(this, QString::fromLatin1("Protected system"),
                             QString::fromLatin1(
                                 "The selected root component (%1) belongs to the "
                                 "running system and cannot be committed as a "
                                 "repair target. Use Host Maintenance for the "
                                 "protected running host.").arg(root),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Committing a repair target is an authorization point: the modal
    // hidden-input dialog is requested here once and the cached session is
    // reused by every later command (never on Run All).
    if (!ensureAdministratorSession(QString::fromLatin1("repair target commit"))) {
        return;
    }
    if (m_hostMaintenance) {
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton, QString::fromLatin1("Host Maintenance"));
        }
        appendLog(QString::fromLatin1(
            "Host Maintenance left; the committed repair target is now the active scope."));
    }
    m_targetCommitted = true;
    m_committedDisk = disk;
    m_committedRoot = root;
    appendLog(QString::fromLatin1("Repair target committed: %1 + %2. Diagnostics and gated repairs now target this scope.")
                  .arg(m_committedDisk).arg(m_committedRoot));
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::toggleHostMaintenance()
{
    if (m_hostMaintenance) {
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton, QString::fromLatin1("Host Maintenance"));
        }
        appendLog(QString::fromLatin1(
            "Running-host maintenance deselected; commit an offline repair target to run diagnostics."));
        updateStatus();
        updateActionStates();
        updateDriveDetails();
        updateUnlockStatus();
        return;
    }

    // Host Maintenance always targets the detected running host, whatever the
    // current selection is; entering it is the authorization point for the
    // running-host session.
    std::string root;
    std::string disk;
    if (!detectRunningHostTarget(&root, &disk)) {
        QMessageBox::warning(this, QString::fromLatin1("Host Maintenance unavailable"),
                             QString::fromLatin1(
                                 "The running host target could not be detected; "
                                 "diagnostics need a committed repair target or a "
                                 "detected running host."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_hostDetectedDisk = fromStd(disk);
    m_hostDetectedRoot = fromStd(root);
    setSelection(m_hostDetectedDisk, QString::null, m_hostDetectedRoot);
    selectInventoryRow(m_hostDetectedDisk);
    if (!ensureAdministratorSession(QString::fromLatin1("Host Maintenance"))) {
        return;
    }
    m_targetCommitted = false;
    m_hostMaintenance = true;
    if (m_hostMaintenanceButton) {
        updateButtonText(m_hostMaintenanceButton, QString::fromLatin1("Exit Host Maintenance"));
    }
    appendLog(QString::fromLatin1(
        "Running-host maintenance activated; diagnostics and gated repairs now "
        "target the protected running host (%1 + %2).")
        .arg(selectedDisk()).arg(selectedRoot()));
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

// Best Linux root component for a drive, resolved from the read-only
// inventory plus the helper-confirmed facts. Order mirrors the modern page:
// a dm/mapper device on the disk with a Linux filesystem first, then a
// partition with a Linux filesystem, then the disk itself. An explicit
// override (main.cpp --host-root or the smoke test) and the helper-confirmed
// component win over the inventory resolution.
QString LegacyMainWindow::autoResolvedRoot(const QString &disk) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    if (m_rootOverrideDisk == disk && !m_rootOverrideRoot.isEmpty()
        && m_rootOverrideRoot.startsWith(QString::fromLatin1("/dev/"))) {
        return m_rootOverrideRoot;
    }
    if (m_helperDisk == disk && !m_helperComponent.isEmpty()
        && m_helperComponent.startsWith(QString::fromLatin1("/dev/"))) {
        return m_helperComponent;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;

    QString partitionCandidate;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        const bool linuxFs = legacy::isLinuxFileSystemName(row.fstype)
            || legacy::isLinuxFileSystemName(row.probedFstype);
        if (!linuxFs) {
            continue;
        }
        if (row.mapper && rootBelongsToDisk(m_rows, it.key(), diskName)) {
            return it.key();
        }
        if (!row.disk && !row.mapper && row.parent == diskName
            && partitionCandidate.isEmpty()) {
            partitionCandidate = it.key();
        }
    }
    if (!partitionCandidate.isEmpty()) {
        return partitionCandidate;
    }
    // A locked LUKS container has no visible Linux filesystem yet: do not
    // fall back to the disk itself, because committing it would store an
    // unusable component (modern "Unlock required before selection").
    if (!autoResolvedLuks(disk).isEmpty()) {
        return QString::null;
    }
    // Whole-disk filesystem or blank/data disk: the disk itself is the only
    // candidate, exactly like the modern fallback.
    return disk;
}

// First locked LUKS component visible on `disk` (a partition or whole-device
// crypto_LUKS row that has no unlocked mapper child yet), or "" when none is
// known. Detection uses the world-readable udev metadata; it fails closed
// (no candidate) when the probe data is unavailable.
QString LegacyMainWindow::autoResolvedLuks(const QString &disk) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!row.encrypted) {
            continue;
        }
        const bool onDisk = row.disk ? (row.name == diskName)
                                     : (row.parent == diskName);
        if (!onDisk) {
            continue;
        }
        // An encrypted container with a visible Linux mapper child is already
        // unlocked; Boot Bitch reuses that mapping and does not reopen it.
        bool unlockedChild = false;
        for (QMap<QString, DeviceRow>::const_iterator child = m_rows.begin();
             child != m_rows.end(); ++child) {
            const DeviceRow &candidate = child.data();
            if (candidate.mapper && candidate.parent == row.name
                && (legacy::isLinuxFileSystemName(candidate.fstype)
                    || legacy::isLinuxFileSystemName(candidate.probedFstype))) {
                unlockedChild = true;
                break;
            }
        }
        if (!unlockedChild) {
            return it.key();
        }
    }
    return QString::null;
}

// The component the Unlock action should open: the inspected row when it is
// itself a locked encrypted container (modern inspectedNode preference),
// otherwise the first locked LUKS component on the drive.
QString LegacyMainWindow::unlockCandidateFor(const QString &disk) const
{
    const QString inspected = selectedComponent();
    if (!inspected.isEmpty()) {
        const QMap<QString, DeviceRow>::const_iterator row = m_rows.find(inspected);
        if (row != m_rows.end() && row.data().encrypted) {
            bool unlockedChild = false;
            for (QMap<QString, DeviceRow>::const_iterator child = m_rows.begin();
                 child != m_rows.end(); ++child) {
                const DeviceRow &candidate = child.data();
                if (candidate.mapper && candidate.parent == row.data().name
                    && (legacy::isLinuxFileSystemName(candidate.fstype)
                        || legacy::isLinuxFileSystemName(candidate.probedFstype))) {
                    unlockedChild = true;
                    break;
                }
            }
            if (!unlockedChild) {
                return inspected;
            }
        }
    }
    return autoResolvedLuks(disk);
}

// The physical disk that owns an inspected partition/mapper row (walking the
// resolved dm chain for mappers).
QString LegacyMainWindow::owningDiskFor(const DeviceRow &row) const
{
    if (row.disk) {
        return QString::fromLatin1("/dev/") + fromStd(row.name);
    }
    QString parent = fromStd(row.parent);
    for (int depth = 0; depth < 8 && !parent.isEmpty(); ++depth) {
        const QMap<QString, DeviceRow>::const_iterator candidate =
            m_rows.find(QString::fromLatin1("/dev/") + parent);
        if (candidate == m_rows.end()) {
            break;
        }
        if (candidate.data().disk) {
            return candidate.key();
        }
        parent = fromStd(candidate.data().parent);
    }
    return QString::null;
}

void LegacyMainWindow::autoDetectHostTarget()
{
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        m_hostDetectedDisk = fromStd(disk);
        m_hostDetectedRoot = fromStd(root);
    }
    if (selectionComplete()) {
        return;
    }
    if (!root.empty() && !disk.empty()) {
        setTarget(fromStd(disk), fromStd(root));
    }
}

QString LegacyMainWindow::identity() const
{
    return QString(hostScope() ? QString::fromLatin1("host") : QString::fromLatin1("target"))
        + QString::fromLatin1("|") + selectedDisk()
        + QString::fromLatin1("|") + selectedRoot();
}

bool LegacyMainWindow::hostScope() const
{
    return m_hostMaintenance;
}

QString LegacyMainWindow::selectedDisk() const
{
    return m_selectedDisk.stripWhiteSpace();
}

QString LegacyMainWindow::selectedComponent() const
{
    return m_selectedComponent.stripWhiteSpace();
}

QString LegacyMainWindow::selectedRoot() const
{
    return autoResolvedRoot(selectedDisk());
}

bool LegacyMainWindow::selectionComplete() const
{
    return !selectedDisk().isEmpty() && !selectedRoot().isEmpty();
}

QString LegacyMainWindow::runningHostDisk() const
{
    if (!m_hostDetectedDisk.isEmpty()) {
        return m_hostDetectedDisk;
    }
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        return fromStd(disk);
    }
    return QString::null;
}

QString LegacyMainWindow::runningHostRoot() const
{
    if (!m_hostDetectedRoot.isEmpty()) {
        return m_hostDetectedRoot;
    }
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        return fromStd(root);
    }
    return QString::null;
}

bool LegacyMainWindow::targetCommitted() const
{
    return m_targetCommitted
        && m_committedDisk == selectedDisk()
        && m_committedRoot == selectedRoot();
}

bool LegacyMainWindow::hostMaintenanceActive() const
{
    return hostScope() && m_hostMaintenance;
}

bool LegacyMainWindow::diagnosticsScopeReady() const
{
    return hostMaintenanceActive() || targetCommitted();
}

QString LegacyMainWindow::scopeFeatureKey() const
{
    return QString::fromLatin1(hostScope() ? "host-shell" : "shell");
}

QString LegacyMainWindow::scopeReadyReason() const
{
    if (hostMaintenanceActive()) {
        return QString::fromLatin1(
            "Host Maintenance is active; diagnostics and gated repairs target "
            "the protected running host.");
    }
    if (selectedDisk().isEmpty()) {
        return QString::fromLatin1(
            "Select a physical drive in the Available repair targets list on the "
            "Systems tab first, or use Host Maintenance for the protected "
            "running host.");
    }
    if (!runningHostDisk().isEmpty() && selectedDisk() == runningHostDisk()) {
        return QString::fromLatin1(
            "The selected drive (%1) is the protected running host. Choose Host "
            "Maintenance on the Systems tab to run read-only host diagnostics "
            "and guarded host repairs; ordinary target repairs stay disabled.")
            .arg(selectedDisk());
    }
    if (!m_targetCommitted) {
        return QString::fromLatin1(
            "No repair target is committed. Choose Select Target on the Systems "
            "tab (or Host Maintenance for the protected running host) first.");
    }
    return QString::fromLatin1(
        "The selection changed after the target was committed. Choose Select "
        "Target again.");
}

void LegacyMainWindow::runDiagnostics()
{
    runDiagnosticsInternal(false);
}

// `quiet` is used by the automatic regeneration: it never opens a dialog and
// never prompts for authorization, it only logs the skip reason.
void LegacyMainWindow::runDiagnosticsInternal(bool quiet)
{
    if (!diagnosticsScopeReady()) {
        if (quiet) {
            appendLog(QString::fromLatin1(
                "Automatic read-only diagnostics regeneration skipped: %1")
                .arg(scopeReadyReason()));
        } else {
            QMessageBox::information(this, QString::fromLatin1("Diagnostics scope required"),
                                     scopeReadyReason(),
                                     QMessageBox::Ok, QMessageBox::NoButton);
        }
        return;
    }
    if (!selectionComplete()) {
        if (quiet) {
            appendLog(QString::fromLatin1(
                "Automatic read-only diagnostics regeneration skipped: the "
                "running host target could not be resolved."));
        } else {
            QMessageBox::warning(
                this, QString::fromLatin1("Diagnostics scope unresolved"),
                QString::fromLatin1(
                    "The running host target could not be resolved; use Refresh "
                    "Devices and commit a repair target or re-enter Host "
                    "Maintenance."),
                QMessageBox::Ok, QMessageBox::NoButton);
        }
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-diagnose") : QString::fromLatin1("diagnose"))
         << selectedDisk() << selectedRoot() << QString::fromLatin1("all");
    startCommand(args, true,
                 host ? QString::fromLatin1("host-diagnose all")
                      : QString::fromLatin1("diagnose all"),
                 false, false, false, quiet);
}

void LegacyMainWindow::runSelectedDiagnostic()
{
    if (m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, QString::fromLatin1("Diagnostics scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!selectionComplete()) {
        QMessageBox::warning(
            this, QString::fromLatin1("Diagnostics scope unresolved"),
            QString::fromLatin1(
                "The running host target could not be resolved; use Refresh "
                "Devices and commit a repair target or re-enter Host "
                "Maintenance."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QListViewItem *item = m_diagnosticList ? m_diagnosticList->currentItem() : 0;
    if (!item) {
        QMessageBox::information(this, QString::fromLatin1("Diagnostic check required"),
                                 QString::fromLatin1(
                                     "Select a diagnostic check in the list first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString key = mapValue(m_diagKeyByTitle,
                                 item->text(0).stripWhiteSpace());
    if (key.isEmpty()) {
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-diagnose") : QString::fromLatin1("diagnose"))
         << selectedDisk() << selectedRoot() << key;
    startCommand(args, true,
                 (host ? QString::fromLatin1("host-diagnose ") : QString::fromLatin1("diagnose "))
                     + key);
}

void LegacyMainWindow::diagnosticSelectionChanged()
{
    updateDiagnosticDetails();
    updateActionStates();
}

// Modern parity: the target configuration row is target-only. Host
// Maintenance never shows it and never enables target-file editing, and the
// helper refuses to read or write a running-host configuration path.
void LegacyMainWindow::editTargetConfigFile()
{
    if (m_running) {
        return;
    }
    if (!targetCommitted() || hostMaintenanceActive()) {
        QMessageBox::information(
            this, QString::fromLatin1("Repair target required"),
            targetCommitted() ? scopeReadyReason()
                              : QString::fromLatin1(
                                    "Target file editing needs a committed repair "
                                    "target. Running-host maintenance has no "
                                    "target-file editing; commit an offline target "
                                    "first."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString path = m_configCombo ? m_configCombo->currentText().stripWhiteSpace()
                                       : QString::null;
    const QString key = configKeyForPath(path);
    if (key.isEmpty()) {
        QMessageBox::information(this, QString::fromLatin1("Configuration file required"),
                                 QString::fromLatin1(
                                     "Select a target configuration file first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_pendingConfigKey = key;
    m_pendingConfigPath = path;
    QStringList args;
    args << QString::fromLatin1("config-read") << selectedDisk() << selectedRoot() << key;
    startCommand(args, false, QString::fromLatin1("config-read %1").arg(path), false, true);
}

// Opens the modal editor with the helper's read-only `config-read` content.
// Saving writes through the guarded `config-write` verb and invalidates the
// cached diagnostics, exactly like the modern Edit Target File dialog.
void LegacyMainWindow::openConfigEditor(const QString &content, const QString &key,
                                        const QString &path)
{
    if (key.isEmpty()) {
        return;
    }
    QDialog dialog(this, "legacy-config-editor", true);
    dialog.setCaption(QString::fromLatin1("Edit target %1").arg(path));
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *info = new QLabel(
        QString::fromLatin1(
            "Edit this target file through the guarded administrator helper. A "
            "successful save invalidates cached diagnostics; rerun diagnostics "
            "before repair. Generated files such as /boot/grub/menu.lst may be "
            "replaced by the next bootloader update."),
        &dialog);
    enableLabelWordWrap(info);
    info->setMaximumWidth(640);
    layout->addWidget(info);

    QTextEdit *editor = new QTextEdit(&dialog);
    editor->setTextFormat(Qt::PlainText);
    editor->setText(content);
    editor->setWordWrap(QTextEdit::NoWrap);
    QFont mono(QString::fromLatin1("monospace"));
    mono.setStyleHint(QFont::TypeWriter);
    editor->setFont(mono);
    layout->addWidget(editor, 1);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *cancel = new QPushButton(QString::fromLatin1("Cancel"), &dialog);
    QPushButton *save = new QPushButton(QString::fromLatin1("Save Target File"), &dialog);
    save->setDefault(true);
    buttons->addWidget(cancel);
    buttons->addWidget(save);
    QObject::connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(save, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.resize(760, 520);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    const QString edited = editor->text();
    if (edited == content) {
        statusBar()->message(QString::fromLatin1("No changes to %1.").arg(path), 3000);
        return;
    }
    if (static_cast<int>(edited.local8Bit().size()) > kConfigEditMaximumBytes) {
        QMessageBox::warning(
            this, QString::fromLatin1("File too large"),
            QString::fromLatin1(
                "The edited file is larger than %1 KiB. The guarded write "
                "transports the content on the command line, which Etch's "
                "kernel cannot accept; edit the file from a console instead.")
                .arg(kConfigEditMaximumBytes / 1024),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const int answer = QMessageBox::question(
        this, QString::fromLatin1("Write target configuration"),
        QString::fromLatin1(
            "Write the edited contents to %1? This modifies the repair target "
            "and invalidates cached diagnostics.").arg(path),
        QMessageBox::Yes, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) {
        return;
    }

    m_pendingConfigKey = key;
    m_pendingConfigPath = path;
    m_pendingConfigWrite = true;
    QStringList args;
    args << QString::fromLatin1("config-write") << selectedDisk() << selectedRoot()
         << key << edited;
    startCommand(args, false, QString::fromLatin1("config-write %1").arg(path), false, true);
}

void LegacyMainWindow::copyResults()
{
    if (!m_rawView) {
        return;
    }
    const QString text = m_rawView->text();
    if (text.stripWhiteSpace().isEmpty()) {
        statusBar()->message(QString::fromLatin1("No diagnostic results to copy yet."), 3000);
        return;
    }
    QApplication::clipboard()->setText(text);
    statusBar()->message(QString::fromLatin1("Diagnostic results copied to the clipboard."), 3000);
}

// Modern parity: Save Results... writes the current read-only results pane to
// a user-chosen file (no helper interaction, no secret can be present).
void LegacyMainWindow::saveResults()
{
    if (!m_rawView) {
        return;
    }
    const QString text = m_rawView->text();
    if (text.stripWhiteSpace().isEmpty()) {
        statusBar()->message(QString::fromLatin1("No diagnostic results to save yet."), 3000);
        return;
    }
    const QString path = QFileDialog::getSaveFileName(
        QString::null,
        QString::fromLatin1("Text files (*.txt);;All files (*)"),
        this, "save-results", QString::fromLatin1("Save diagnostic results"));
    if (path.isEmpty()) {
        return;
    }
    QFile file(path);
    if (!file.open(IO_WriteOnly)) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Could not write %1.").arg(path),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QTextStream stream(&file);
    stream << text;
    if (!text.endsWith(QString::fromLatin1("\n"))) {
        stream << "\n";
    }
    file.close();
    statusBar()->message(QString::fromLatin1("Diagnostic results saved to %1").arg(path), 4000);
}

void LegacyMainWindow::runUnlock()
{
    if (m_running) {
        return;
    }
    if (hostScope()
        || (!runningHostDisk().isEmpty() && selectedDisk() == runningHostDisk())) {
        QMessageBox::warning(this, QString::fromLatin1("Unlock not available"),
                             QString::fromLatin1(
                                 "The protected running host cannot be unlocked. "
                                 "Use Host Maintenance for the protected running "
                                 "host, or select an offline repair target."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (selectedDisk().isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Select a physical drive in the Available repair targets list first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Auto-resolve the locked LUKS component from the read-only inventory; the
    // helper's own cryptsetup preflights verify it again before opening it.
    const QString luks = unlockCandidateFor(selectedDisk());
    if (luks.isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("Unlock not available"),
                             QString::fromLatin1(
                                 "No locked LUKS component is currently visible on "
                                 "this selected drive."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    if (!m_unlockRetry) {
        const int answer = QMessageBox::question(
            this, QString::fromLatin1("Confirm LUKS unlock"),
            QString::fromLatin1(
                "Unlock %1 on %2?\n\n"
                "The helper opens a temporary device-mapper mapping with "
                "cryptsetup and keeps it open for this recovery session. The "
                "passphrase is sent only over the helper's standard input, "
                "never in command arguments or logs.")
                .arg(luks).arg(selectedDisk()),
            QMessageBox::Yes, QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return;
        }
    }

    // Establish the elevation session before collecting the disk passphrase,
    // so the two secrets are never held in memory at the same time. The cached
    // session from the target commit is reused; only an expired session
    // prompts here.
    if (!ensureAdministratorSession(QString::fromLatin1("LUKS unlock"))) {
        return;
    }

    bool ok = false;
    QString passphrase = promptHiddenPassword(
        this, QString::fromLatin1("Unlock LUKS repair target"),
        QString::fromLatin1("Enter the passphrase for %1.\n\n"
                            "It is sent only to cryptsetup over the helper's "
                            "standard input and is never logged or placed on a "
                            "command line.").arg(luks),
        &ok);
    if (!ok) {
        return;
    }
    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("Passphrase required"),
                             QString::fromLatin1(
                                 "An empty passphrase was not submitted. Enter "
                                 "the LUKS passphrase or choose Cancel."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QByteArray secret = passphrase.local8Bit();
    passphrase.fill(QChar(0));
    passphrase = QString::null;

    m_unlockRetry = false;
    m_unlockDevice = luks;
    m_unlockDisk = selectedDisk();
    QStringList args;
    args << QString::fromLatin1("unlock") << selectedDisk() << luks;
    m_runner->setInputData(secret);
    secret.fill('\0');
    secret = QByteArray();
    startCommand(args, false, QString::fromLatin1("unlock %1").arg(luks), true);
}

void LegacyMainWindow::recheckElevation()
{
    if (m_running) {
        return;
    }
    m_runner->resetElevation();
    QString description;
    const bool ok = m_runner->resolveElevation(&description);
    m_lastHelperDescription = description;
    updateElevationLabel();
    appendLog(QString::fromLatin1("Elevation re-check: %1")
                  .arg(ok ? description
                          : QString::fromLatin1("unavailable - %1").arg(description)));
    if (!ok) {
        QMessageBox::warning(this, QString::fromLatin1("Elevation unavailable"),
                             QString::fromLatin1(
                                 "No elevation method could be resolved: %1\n\n"
                                 "Privileged commands will fail until gksu, gksudo "
                                 "or sudo is available (or the GUI is started as "
                                 "root).").arg(description),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }
}

void LegacyMainWindow::logFilterChanged()
{
    refreshLogView();
}

void LegacyMainWindow::logSearchChanged()
{
    refreshLogView();
}

void LegacyMainWindow::sessionLogSelectionChanged()
{
    if (!m_sessionLogList) {
        return;
    }
    QListViewItem *item = m_sessionLogList->currentItem();
    if (!item) {
        return;
    }
    if (item == m_sessionLogList->firstChild()) {
        m_viewingPriorLog = false;
        m_priorLogPath = QString::null;
        m_priorLogLines.clear();
        if (m_priorLogBanner) {
            m_priorLogBanner->hide();
        }
        if (m_deleteSessionLogButton) {
            m_deleteSessionLogButton->setEnabled(false);
        }
        refreshLogView();
        return;
    }
    const QString path = m_logDirectory + QString::fromLatin1("/") + item->text(0);
    QFile file(path);
    if (!file.open(IO_ReadOnly)) {
        m_logView->append(QString::fromLatin1("ERROR: cannot read the session log %1").arg(path));
        return;
    }
    const QByteArray fileData = file.readAll();
    file.close();
    const QString text = QString::fromLocal8Bit(fileData.data(), fileData.size());
    m_priorLogLines = QStringList::split(QChar('\n'), text, false);
    while (!m_priorLogLines.isEmpty() && m_priorLogLines.last().isEmpty()) {
        m_priorLogLines.pop_back();
    }
    m_priorLogPath = path;
    m_viewingPriorLog = true;
    if (m_priorLogBanner) {
        m_priorLogBanner->setText(QString::fromLatin1(
            "Viewing a prior session log (read-only): %1").arg(item->text(0)));
        m_priorLogBanner->show();
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(true);
    }
    refreshLogView();
}

void LegacyMainWindow::refreshSessionLogs()
{
    refreshSessionLogList();
    statusBar()->message(QString::fromLatin1("Session log list refreshed."), 3000);
}

void LegacyMainWindow::toggleLogWrap(bool enabled)
{
    m_logWrapEnabled = enabled;
    if (m_logView) {
        m_logView->setWordWrap(enabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    }
    // Keep the Settings checkbox and the View menu item in sync without
    // re-entering their signals.
    if (m_logWrapCheck && m_logWrapCheck->isChecked() != enabled) {
        m_logWrapCheck->blockSignals(true);
        m_logWrapCheck->setChecked(enabled);
        m_logWrapCheck->blockSignals(false);
    }
    if (m_viewMenu && m_wrapLogsMenuValid
        && m_viewMenu->isItemChecked(m_wrapLogsMenuId) != enabled) {
        m_viewMenu->setItemChecked(m_wrapLogsMenuId, enabled);
    }
    saveLegacySettings();
}

// Modern New Session Log: close the active session file. It becomes a prior
// session in the list; the next log entry creates a new timestamped file (the
// legacy logger has no separate scope-identification step).
void LegacyMainWindow::startNewSessionLog()
{
    const QString closed = m_logPath;
    m_logPath = QString::null;
    m_logLines.clear();
    m_viewingPriorLog = false;
    m_priorLogPath = QString::null;
    m_priorLogLines.clear();
    if (m_priorLogBanner) {
        m_priorLogBanner->hide();
    }
    if (m_settingsSessionLabel) {
        m_settingsSessionLabel->setText(QString::fromLatin1("(not created yet)"));
    }
    appendLog(QString::fromLatin1(
        "Started a new session log; earlier files remain available in the "
        "session list."));
    if (!closed.isEmpty()) {
        appendLog(QString::fromLatin1("Closed session log: %1").arg(closed));
    }
    refreshSessionLogList();
    if (m_sessionLogList && m_sessionLogList->firstChild()) {
        m_sessionLogList->setSelected(m_sessionLogList->firstChild(), true);
        m_sessionLogList->setCurrentItem(m_sessionLogList->firstChild());
    }
    refreshLogView();
}

// Modern Add Note: a free-text NOTE entry in the live register.
void LegacyMainWindow::addSessionNote()
{
    bool accepted = false;
    const QString note = QInputDialog::getText(
        QString::fromLatin1("Add Note"), QString::fromLatin1("Note:"),
        QLineEdit::Normal, QString::null, &accepted, this);
    if (!accepted) {
        return;
    }
    const QString trimmed = note.stripWhiteSpace();
    if (trimmed.isEmpty()) {
        return;
    }
    appendLog(QString::fromLatin1("NOTE: %1").arg(trimmed));
}

// Modern Delete: only a selected prior session file inside the configured log
// directory, never the live file, after a confirmation. The file-name and
// canonical-path guards mirror the modern deleteSelectedSessionLog().
void LegacyMainWindow::deleteSelectedSessionLog()
{
    if (!m_sessionLogList) {
        return;
    }
    QListViewItem *item = m_sessionLogList->currentItem();
    if (!item || item == m_sessionLogList->firstChild()) {
        return;
    }
    const QString fileName = item->text(0);
    if (fileName.isEmpty() || !fileName.endsWith(QString::fromLatin1(".log"))) {
        return;
    }
    const QString path = m_logDirectory + QString::fromLatin1("/") + fileName;
    if (m_logDirectory.isEmpty() || !path.startsWith(m_logDirectory)) {
        return;
    }
    if (!m_logPath.isEmpty() && path == m_logPath) {
        return;
    }
    // Canonical containment: never follow a symlink out of the log directory.
    // Qt3 QFileInfo has no canonicalFilePath(), so canonicalize the directory
    // and rebuild the candidate path from the plain file name.
    const QString canonicalDirectory = QDir(m_logDirectory).canonicalPath();
    if (canonicalDirectory.isEmpty()) {
        return;
    }
    const QString canonicalPath = canonicalDirectory + QString::fromLatin1("/") + fileName;
    QFileInfo fileInfo(canonicalPath);
    if (!fileInfo.exists() || !fileInfo.isFile() || fileInfo.isSymLink()) {
        return;
    }
    const int answer = QMessageBox::question(
        this, QString::fromLatin1("Delete session log"),
        QString::fromLatin1("Delete %1 permanently? This cannot be undone.")
            .arg(fileName),
        QMessageBox::Yes, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) {
        return;
    }
    if (!QFile::remove(canonicalPath)) {
        QMessageBox::warning(this, QString::fromLatin1("Delete session log"),
                             QString::fromLatin1("Unable to delete %1.").arg(fileName),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    appendLog(QString::fromLatin1("Deleted prior session log %1.").arg(fileName));
    if (m_viewingPriorLog && m_priorLogPath == path) {
        m_viewingPriorLog = false;
        m_priorLogPath = QString::null;
        m_priorLogLines.clear();
        if (m_priorLogBanner) {
            m_priorLogBanner->hide();
        }
    }
    refreshSessionLogList();
    refreshLogView();
}

// ---- persisted options (modern-compatible QSettings key names) -------------

void LegacyMainWindow::loadLegacySettings()
{
    QSettings settings;
    settings.setPath(QString::fromLatin1("boot-bitch.local"),
                     QString::fromLatin1("boot-bitch-legacy"), QSettings::User);
    const bool nonLinux = settings.readBoolEntry(
        QString::fromLatin1("/devices/showNonLinux"), true);
    const bool removable = settings.readBoolEntry(
        QString::fromLatin1("/devices/showRemovable"), true);
    const bool encrypted = settings.readBoolEntry(
        QString::fromLatin1("/devices/showEncrypted"), true);
    m_logWrapEnabled = settings.readBoolEntry(
        QString::fromLatin1("/logs/wrapLines"), true);
    m_autoRefreshEnabled = settings.readBoolEntry(
        QString::fromLatin1("/diagnostics/autoRefreshStale"), true);

    QCheckBox *checks[] = { m_showNonLinuxCheck, m_showRemovableCheck,
                            m_showEncryptedCheck, m_autoRefreshCheck,
                            m_logWrapCheck };
    const bool values[] = { nonLinux, removable, encrypted,
                            m_autoRefreshEnabled, m_logWrapEnabled };
    for (int i = 0; i < 5; ++i) {
        if (!checks[i]) {
            continue;
        }
        checks[i]->blockSignals(true);
        checks[i]->setChecked(values[i]);
        checks[i]->blockSignals(false);
    }
    if (m_logView) {
        m_logView->setWordWrap(m_logWrapEnabled ? QTextEdit::WidgetWidth
                                                : QTextEdit::NoWrap);
    }
    if (m_viewMenu && m_wrapLogsMenuValid) {
        m_viewMenu->setItemChecked(m_wrapLogsMenuId, m_logWrapEnabled);
    }
    // Full Repair plan checkboxes (modern-compatible key names).
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        const bool checked = settings.readBoolEntry(
            QString::fromLatin1(planSpecs[i].settingsKey),
            planSpecs[i].defaultChecked);
        m_planChecks[i]->blockSignals(true);
        m_planChecks[i]->setChecked(checked);
        m_planChecks[i]->blockSignals(false);
    }
}

void LegacyMainWindow::saveLegacySettings()
{
    QSettings settings;
    settings.setPath(QString::fromLatin1("boot-bitch.local"),
                     QString::fromLatin1("boot-bitch-legacy"), QSettings::User);
    if (m_showNonLinuxCheck) {
        settings.writeEntry(QString::fromLatin1("/devices/showNonLinux"),
                            m_showNonLinuxCheck->isChecked());
    }
    if (m_showRemovableCheck) {
        settings.writeEntry(QString::fromLatin1("/devices/showRemovable"),
                            m_showRemovableCheck->isChecked());
    }
    if (m_showEncryptedCheck) {
        settings.writeEntry(QString::fromLatin1("/devices/showEncrypted"),
                            m_showEncryptedCheck->isChecked());
    }
    settings.writeEntry(QString::fromLatin1("/logs/wrapLines"), m_logWrapEnabled);
    settings.writeEntry(QString::fromLatin1("/diagnostics/autoRefreshStale"),
                        m_autoRefreshEnabled);
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (m_planChecks[i]) {
            settings.writeEntry(QString::fromLatin1(planSpecs[i].settingsKey),
                                m_planChecks[i]->isChecked());
        }
    }
}

void LegacyMainWindow::deviceFilterChanged()
{
    saveLegacySettings();
    rebuildDeviceList();
    updateActionStates();
    statusBar()->message(QString::fromLatin1("Device discovery filters updated."), 2500);
}

void LegacyMainWindow::autoRefreshToggled(bool enabled)
{
    m_autoRefreshEnabled = enabled;
    saveLegacySettings();
    updateActionStates();
}

// Modern Host capabilities and dependencies: read-only labels fed by the
// helper backend-profile facts parsed from the cached diagnostics.
void LegacyMainWindow::updateCapabilityView()
{
    if (!m_capDistributionLabel) {
        return;
    }
    const QString unavailable = QString::fromLatin1(
        "not reported (run diagnostics for the selected scope)");
    QString distribution = mapValue(m_factMap, QString::fromLatin1("System:"));
    if (distribution.isEmpty()) {
        distribution = mapValue(m_factMap, QString::fromLatin1("Detected target OS:"));
    }
    if (distribution.isEmpty()) {
        distribution = mapValue(m_factMap, QString::fromLatin1("OS:"));
    }
    QString packageManager = mapValue(m_factMap, QString::fromLatin1("Package manager backends:"));
    if (packageManager.isEmpty()) {
        packageManager = mapValue(m_factMap, QString::fromLatin1("Package manager backend:"));
    }
    QString initramfs = mapValue(m_factMap, QString::fromLatin1("Initramfs backends:"));
    if (initramfs.isEmpty()) {
        initramfs = mapValue(m_factMap, QString::fromLatin1("Initramfs backend:"));
    }
    m_capDistributionLabel->setText(distribution.isEmpty() ? unavailable : distribution);
    m_capPackageManagerLabel->setText(packageManager.isEmpty() ? unavailable : packageManager);
    m_capServiceLabel->setText(detailOrDash(mapValue(m_factMap, QString::fromLatin1("Service manager:"))));
    m_capDisplayLabel->setText(detailOrDash(mapValue(m_factMap, QString::fromLatin1("Display manager backend:"))));
    m_capInitramfsLabel->setText(initramfs.isEmpty() ? unavailable : initramfs);
    m_capBootloaderLabel->setText(detailOrDash(mapValue(m_factMap, QString::fromLatin1("Bootloader backend:"))));
    m_capLoggingLabel->setText(detailOrDash(mapValue(m_factMap, QString::fromLatin1("Logging backend:"))));
}

// ---- Individual repair tools and the Full Repair plan -----------------------

int LegacyMainWindow::selectedToolIndex() const
{
    if (!m_toolList) {
        return -1;
    }
    QListViewItem *item = m_toolList->currentItem();
    if (!item) {
        return -1;
    }
    int index = 0;
    for (QListViewItem *row = m_toolList->firstChild(); row;
         row = row->nextSibling(), ++index) {
        if (row == item) {
            return index;
        }
    }
    return -1;
}

void LegacyMainWindow::toolSelectionChanged()
{
    updateToolDetails();
    updateActionStates();
}

// True when the selected tool may run now; `reason` always receives the exact
// explanation. Fail closed: scope, session and the cached capability line
// (plus the host-maintenance probe for the host initramfs/grub stages) must
// all agree. Display-only tools never run.
bool LegacyMainWindow::toolRunReady(int toolIndex, QString *reason) const
{
    if (toolIndex < 0 || toolIndex >= toolSpecCount) {
        if (reason) {
            *reason = QString::fromLatin1("No repair tool is selected.");
        }
        return false;
    }
    const ToolSpec &spec = toolSpecs[toolIndex];
    if (m_running) {
        if (reason) {
            *reason = QString::fromLatin1("A helper command is already running.");
        }
        return false;
    }
    if (!diagnosticsScopeReady()) {
        if (reason) {
            *reason = scopeReadyReason();
        }
        return false;
    }
    if (!selectionComplete()) {
        if (reason) {
            *reason = QString::fromLatin1(
                "The selected scope has no resolved root component; use "
                "Refresh Devices and commit the repair target again.");
        }
        return false;
    }
    if (!spec.stage || !*spec.stage) {
        // Display-only tool: prefer the helper's cached capability reason; when
        // the helper reports the capability as available, name the frontend
        // omission instead (still fail closed, never runnable).
        std::string capabilityReason;
        if (!m_model.isAvailable(spec.key, toStd(identity()), &capabilityReason)) {
            if (reason) {
                *reason = fromStd(capabilityReason);
            }
        } else if (reason) {
            *reason = QString::fromLatin1(
                "This legacy frontend exposes no %1 action; the helper reports "
                "the capability as available.").arg(QString::fromLatin1(spec.title));
        }
        return false;
    }
    std::string capabilityReason;
    if (!m_model.isAvailable(spec.key, toStd(identity()), &capabilityReason)) {
        if (reason) {
            *reason = fromStd(capabilityReason);
        }
        return false;
    }
    if (spec.hostMaintenance && hostScope()) {
        std::string featureReason;
        if (!m_model.legacyFeatureAvailable("host-maintenance", toStd(identity()),
                                            &featureReason)) {
            if (reason) {
                *reason = fromStd(featureReason);
            }
            return false;
        }
    }
    if (reason) {
        *reason = QString::fromLatin1(
            "Run this guarded repair action using the cached read-only "
            "diagnostic evidence. A confirmation is shown first.");
    }
    return true;
}

// Selected tool pane: modern title, per-tool button text, description and
// plan status. The "Unavailable: <reason>" suffix is appended exactly like the
// modern page; the tools list's "Full Repair" column mirrors the Settings plan
// state and the cached capability line.
void LegacyMainWindow::updateToolDetails()
{
    if (!m_toolTitle || !m_toolDescription || !m_toolPlanStatus || !m_toolRunButton) {
        return;
    }

    // 1. Full Repair column for every tool row.
    if (m_toolList) {
        int index = 0;
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling(), ++index) {
            if (index >= toolSpecCount) {
                break;
            }
            const ToolSpec &spec = toolSpecs[index];
            QString status;
            if (spec.planStage && *spec.planStage) {
                const int planIndex = planIndexForStage(spec.planStage);
                status = (planIndex >= 0 && planStageSelected(planIndex))
                    ? QString::fromLatin1("Enabled in Settings")
                    : QString::fromLatin1("Disabled in Settings - enable it to include this stage");
            } else {
                status = QString::fromLatin1(spec.planBase);
            }
            QString reason;
            if (!toolRunReady(index, &reason)) {
                status = QString::fromLatin1("Unavailable: %1").arg(reason);
            }
            row->setText(1, status);
        }
    }

    // 2. Selected tool pane.
    const int index = selectedToolIndex();
    if (index < 0) {
        m_toolTitle->setText(QString::fromLatin1("Select a repair tool"));
        m_toolDescription->setText(QString::fromLatin1("Select a tool to review its repair action."));
        m_toolPlanStatus->setText(QString::null);
        updateButtonText(m_toolRunButton, QString::fromLatin1("Run Tool"));
        m_toolRunButton->setEnabled(false);
        return;
    }
    const ToolSpec &spec = toolSpecs[index];
    m_toolTitle->setText(QString::fromLatin1(spec.title));
    m_toolDescription->setText(QString::fromLatin1(spec.description));
    updateButtonText(m_toolRunButton, QString::fromLatin1(spec.button));

    QString planStatus;
    if (spec.planStage && *spec.planStage) {
        const int planIndex = planIndexForStage(spec.planStage);
        planStatus = (planIndex >= 0 && planStageSelected(planIndex))
            ? QString::fromLatin1("Enabled in Settings")
            : QString::fromLatin1("Disabled in Settings - enable it to include this stage");
    } else {
        planStatus = QString::fromLatin1(spec.planBase);
    }
    QString reason;
    const bool ready = toolRunReady(index, &reason);
    if (!ready) {
        // Display-only tools have an empty plan text: the unavailable reason
        // becomes the whole status, with no cosmetic leading blank line.
        if (planStatus.isEmpty()) {
            planStatus = QString::fromLatin1("Unavailable: %1").arg(reason);
        } else {
            planStatus += QString::fromLatin1("\nUnavailable: %1").arg(reason);
        }
    }
    m_toolPlanStatus->setText(planStatus);
    m_toolRunButton->setEnabled(ready);
    QToolTip::add(m_toolRunButton, reason);
}

void LegacyMainWindow::runSelectedTool()
{
    const int index = selectedToolIndex();
    if (index < 0) {
        return;
    }
    const ToolSpec &spec = toolSpecs[index];
    QString reason;
    if (!toolRunReady(index, &reason)) {
        QMessageBox::information(this, QString::fromLatin1("Repair tool unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (spec.write && spec.confirm) {
        const int answer = QMessageBox::question(
            this, QString::fromLatin1("Confirm repair"),
            QString::fromLatin1(spec.confirm),
            QMessageBox::Yes, QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return;
        }
    }
    const bool host = hostScope();
    const QString stage = QString::fromLatin1(spec.stage);
    QStringList args;
    if (stage == QString::fromLatin1("validate")) {
        args << (host ? QString::fromLatin1("host-validate") : QString::fromLatin1("validate"));
    } else if (stage == QString::fromLatin1("fs-inspect")) {
        args << (host ? QString::fromLatin1("host-fs-inspect") : QString::fromLatin1("fs-inspect"));
    } else {
        args << (host ? QString::fromLatin1("host-repair") : QString::fromLatin1("repair"));
    }
    args << selectedDisk() << selectedRoot();
    if (stage != QString::fromLatin1("validate")
        && stage != QString::fromLatin1("fs-inspect")) {
        args << stage;
    }
    const bool started = startCommand(args, false, QString::fromLatin1(spec.title));
    if (started && !m_smokeMode) {
        showRepairResultDialog(QString::fromLatin1(spec.title));
    }
}

void LegacyMainWindow::configurePlan()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(6);
    }
}

void LegacyMainWindow::planCheckboxChanged()
{
    saveLegacySettings();
    updateActionStates();
}

bool LegacyMainWindow::planStageSelected(int planIndex) const
{
    if (planIndex < 0 || planIndex >= static_cast<int>(m_planChecks.size())) {
        return false;
    }
    return m_planChecks[planIndex] && m_planChecks[planIndex]->isChecked();
}

// Fail-closed availability for one plan stage: the cached capability line plus
// the host-maintenance probe for the host initramfs/grub stages. A missing
// diagnostic run keeps the stage unavailable (m_model.isAvailable).
bool LegacyMainWindow::planStageAvailable(int planIndex, QString *reason) const
{
    if (planIndex < 0 || planIndex >= planSpecCount) {
        if (reason) {
            *reason = QString::fromLatin1("Unknown Full Repair stage.");
        }
        return false;
    }
    const PlanSpec &spec = planSpecs[planIndex];
    std::string capabilityReason;
    if (!m_model.isAvailable(spec.capability, toStd(identity()), &capabilityReason)) {
        if (reason) {
            *reason = fromStd(capabilityReason);
        }
        return false;
    }
    if (spec.hostMaintenance && hostScope()) {
        std::string featureReason;
        if (!m_model.legacyFeatureAvailable("host-maintenance", toStd(identity()),
                                            &featureReason)) {
            if (reason) {
                *reason = fromStd(featureReason);
            }
            return false;
        }
    }
    if (reason) {
        *reason = QString::null;
    }
    return true;
}

// The selected plan stages in execution order, filtered to the ones whose
// capability is available (the modern plan list excludes unavailable stages).
QStringList LegacyMainWindow::selectedPlanStages() const
{
    QStringList stages;
    for (int i = 0; i < planSpecCount; ++i) {
        if (!planStageSelected(i)) {
            continue;
        }
        if (!planStageAvailable(i, 0)) {
            continue;
        }
        stages.append(QString::fromLatin1(planSpecs[i].stage));
    }
    return stages;
}

QStringList LegacyMainWindow::selectedPlanTitles() const
{
    QStringList titles;
    for (int i = 0; i < planSpecCount; ++i) {
        if (!planStageSelected(i)) {
            continue;
        }
        if (!planStageAvailable(i, 0)) {
            continue;
        }
        titles.append(QString::fromLatin1(planSpecs[i].title));
    }
    return titles;
}

bool LegacyMainWindow::planRunReady(QString *reason) const
{
    if (m_running) {
        if (reason) {
            *reason = QString::fromLatin1("A helper command is already running.");
        }
        return false;
    }
    if (!diagnosticsScopeReady()) {
        if (reason) {
            *reason = scopeReadyReason();
        }
        return false;
    }
    if (!selectionComplete()) {
        if (reason) {
            *reason = QString::fromLatin1(
                "The selected scope has no resolved root component; use "
                "Refresh Devices and commit the repair target again.");
        }
        return false;
    }
    if (selectedPlanStages().isEmpty()) {
        if (reason) {
            *reason = QString::fromLatin1(
                "No Full Repair stages are selected or available; use "
                "Configure Plan... to choose the stages.");
        }
        return false;
    }
    if (!m_model.hasDiagnostics(toStd(identity())) || m_model.diagnosticsStale()) {
        if (reason) {
            *reason = QString::fromLatin1(kPlanReadinessDefault);
        }
        return false;
    }
    if (!administratorSessionActive()) {
        if (reason) {
            *reason = QString::fromLatin1(
                "Administrator authorization is required; press Authorize on "
                "the Systems or Repair tab to establish the session.");
        }
        return false;
    }
    if (reason) {
        *reason = QString::fromLatin1(
            "Runs the selected stages using the cached read-only diagnostic "
            "evidence after privilege confirmation.");
    }
    return true;
}

// Plan section: the numbered list of selected+available stages, the count
// label and the readiness text, exactly like the modern Full Repair plan.
void LegacyMainWindow::updatePlanView()
{
    if (!m_planStageList || !m_planCountLabel || !m_planReadinessLabel
        || !m_runFullRepairButton) {
        return;
    }
    m_planStageList->clear();
    int availableCount = 0;
    QString excludedReason;
    QListViewItem *last = 0;
    for (int i = 0; i < planSpecCount; ++i) {
        if (!planStageSelected(i)) {
            continue;
        }
        QString reason;
        if (!planStageAvailable(i, &reason)) {
            if (excludedReason.isEmpty()) {
                excludedReason = reason;
            }
            continue;
        }
        ++availableCount;
        last = new QListViewItem(
            m_planStageList, last,
            QString::fromLatin1("%1. %2").arg(availableCount)
                .arg(QString::fromLatin1(planSpecs[i].title)));
    }
    if (availableCount == 0) {
        m_planCountLabel->setText(QString::fromLatin1("No stages selected"));
        new QListViewItem(m_planStageList,
                          QString::fromLatin1(
                              "No Full Repair stages selected - use Configure "
                              "Plan... or Settings."));
    } else {
        m_planCountLabel->setText(availableCount == 1
            ? QString::fromLatin1("1 stage selected")
            : QString::fromLatin1("%1 stages selected").arg(availableCount));
    }

    QString readiness;
    if (availableCount == 0) {
        readiness = excludedReason.isEmpty()
            ? QString::fromLatin1(
                  "No repair stages are selected. Use Configure Plan... to "
                  "choose the stages Full Repair will run.")
            : QString::fromLatin1("No selected stages are available. %1").arg(excludedReason);
    } else if (!diagnosticsScopeReady()) {
        readiness = scopeReadyReason();
    } else if (!selectionComplete()) {
        readiness = QString::fromLatin1(
            "The selected scope has no resolved root component; use Refresh "
            "Devices and commit the repair target again.");
    } else if (!m_model.hasDiagnostics(toStd(identity())) || m_model.diagnosticsStale()) {
        readiness = QString::fromLatin1(kPlanReadinessDefault);
    } else if (!administratorSessionActive()) {
        readiness = QString::fromLatin1(
            "Administrator authorization is required; press Authorize on the "
            "Systems or Repair tab to establish the session.");
    } else {
        readiness = QString::fromLatin1(kPlanReadinessReady);
    }
    m_planReadinessLabel->setText(readiness);

    QString runReason;
    const bool canRun = planRunReady(&runReason);
    m_runFullRepairButton->setEnabled(canRun);
    QToolTip::add(m_runFullRepairButton, runReason);
}

void LegacyMainWindow::runFullRepair()
{
    const QStringList stages = selectedPlanStages();
    if (stages.isEmpty()) {
        QMessageBox::information(
            this, QString::fromLatin1("Full Repair"),
            QString::fromLatin1(
                "No Full Repair stages are selected or available; use "
                "Configure Plan... to choose the stages."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QString reason;
    if (!planRunReady(&reason)) {
        QMessageBox::warning(this, QString::fromLatin1("Full Repair unavailable"),
                             reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QStringList titles = selectedPlanTitles();
    QString text = QString::fromLatin1(
        "Run the Full Repair plan?\n\nThe selected stages run in order through "
        "the helper's guarded repair command:\n\n");
    for (int i = 0; i < static_cast<int>(titles.size()); ++i) {
        text += QString::fromLatin1("  %1. %2\n").arg(i + 1).arg(titles[i]);
    }
    text += QString::fromLatin1(
        "\nThe helper keeps every runtime preflight; a stage that fails stops "
        "the plan.");
    const int answer = QMessageBox::question(
        this, QString::fromLatin1("Run Full Repair"), text,
        QMessageBox::Yes, QMessageBox::No);
    if (answer != QMessageBox::Yes) {
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-repair") : QString::fromLatin1("repair"))
         << selectedDisk() << selectedRoot();
    args += stages;
    const bool started = startCommand(args, false, QString::fromLatin1("Full Repair"));
    if (started && !m_smokeMode) {
        showRepairResultDialog(QString::fromLatin1("Full Repair"));
    }
}

// Modern-style repair result popup: tool title, bold status line, the streamed
// helper transcript and a Close button that enables on finish. Never opened in
// --smoke-test; diagnostics keep their in-tab Results behavior.
void LegacyMainWindow::showRepairResultDialog(const QString &title)
{
    if (m_smokeMode || m_resultDialog) {
        return;
    }
    QDialog *dialog = new QDialog(this, "legacy-repair-result", true);
    dialog->setCaption(title);
    dialog->resize(760, 460);
    QVBoxLayout *layout = new QVBoxLayout(dialog, 10, 8);
    m_resultStatus = new QLabel(
        QString::fromLatin1(
            "Running %1 through the privileged helper... The Logs tab keeps "
            "the complete transcript.").arg(title),
        dialog);
    QFont statusFont = m_resultStatus->font();
    statusFont.setBold(true);
    m_resultStatus->setFont(statusFont);
    m_resultStatus->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_resultStatus);
    m_resultView = new QTextEdit(dialog);
    m_resultView->setReadOnly(true);
    m_resultView->setTextFormat(Qt::LogText);
    m_resultView->setWordWrap(QTextEdit::NoWrap);
    QFont mono(QString::fromLatin1("monospace"));
    mono.setStyleHint(QFont::TypeWriter);
    m_resultView->setFont(mono);
    m_resultView->setText(m_transcript);
    layout->addWidget(m_resultView, 1);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->addStretch();
    m_resultCloseButton = new QPushButton(QString::fromLatin1("Close"), dialog);
    m_resultCloseButton->setDefault(true);
    m_resultCloseButton->setEnabled(false);
    buttons->addWidget(m_resultCloseButton);
    QObject::connect(m_resultCloseButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    m_resultDialog = dialog;
    dialog->exec();
    // exec() returns on Close, Esc or the window close. The helper command may
    // still be running, so clear the streaming targets before deleting the
    // dialog; helperLine/helperFinished then skip the missing view.
    m_resultDialog = 0;
    m_resultStatus = 0;
    m_resultView = 0;
    m_resultCloseButton = 0;
    delete dialog;
}

// The helper's `shell`/`host-shell` verbs run one reviewed command string as
// root (inside a fresh target chroot, or directly on the running host). The
// GUI passes the string as a single argument (no shell interpolation), gates
// the controls on the scope's cached `Legacy feature` probe plus the cached
// administrator session, and never weakens the helper's runtime preflights.
void LegacyMainWindow::runChrootShell()
{
    if (m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, QString::fromLatin1("Shell scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!administratorSessionActive()) {
        QMessageBox::warning(
            this, QString::fromLatin1("Administrator authorization required"),
            QString::fromLatin1(
                "No administrator session is active.\n\n"
                "Press Authorize on the Systems or Repair tab to establish the "
                "session, or enter Host Maintenance / commit a repair target on "
                "the Systems tab; the chroot shell then reuses the cached "
                "authorization."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString command = m_shellCommandEdit
        ? m_shellCommandEdit->text() : QString::null;
    if (command.stripWhiteSpace().isEmpty()) {
        QMessageBox::information(this, QString::fromLatin1("Shell command required"),
                                 QString::fromLatin1(
                                     "Enter the command to run first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool host = hostScope();
    if (host) {
        const int answer = QMessageBox::question(
            this, QString::fromLatin1("Confirm running-host command"),
            QString::fromLatin1(
                "Run this command as root on the protected running host?\n\n"
                "%1\n\n"
                "The helper keeps its runtime preflights; the command is passed "
                "as one argument and is never interpreted by the GUI.").arg(command),
            QMessageBox::Yes, QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return;
        }
    }
    QStringList args;
    args << (host ? QString::fromLatin1("host-shell") : QString::fromLatin1("shell"))
         << selectedDisk() << selectedRoot() << command;
    if (m_shellOutput) {
        m_shellOutput->setText(QString::fromLatin1("Running %1...")
                                   .arg(host ? QString::fromLatin1("host-shell")
                                             : QString::fromLatin1("shell")));
    }
    startCommand(args, false,
                 host ? QString::fromLatin1("host-shell")
                      : QString::fromLatin1("shell"),
                 false, false, true);
}

void LegacyMainWindow::clearChrootOutput()
{
    if (m_shellOutput) {
        m_shellOutput->setText(QString::null);
    }
    updateActionStates();
}

bool LegacyMainWindow::administratorSessionActive() const
{
    // True when no modal password is pending: root/direct execution, an
    // authenticated `sudo` session (including Etch's timestamp-validated plain
    // sudo), `sudo -n`, or an elevation method that prompts on its own
    // (gksu/gksudo).
    return !m_runner->elevationNeedsPassword(0);
}

// The cached decision plus a live, non-blocking timestamp probe. A plain-sudo
// timestamp can expire between commands; probing here (stdin closed, so an
// expired sudo gets EOF instead of blocking on a password read) lets the GUI
// fail closed with the Authorize remedy instead of hanging.
bool LegacyMainWindow::sessionStillCurrent()
{
    if (!administratorSessionActive()) {
        return false;
    }
    if (m_runner->sessionIsCurrent()) {
        return true;
    }
    m_runner->resetElevation();
    appendLog(QString::fromLatin1(
        "The cached administrator authorization expired or was refused; press "
        "Authorize (Systems or Repair tab) to re-establish the session."));
    updateElevationLabel();
    updateActionStates();
    return false;
}

void LegacyMainWindow::authorizeNow()
{
    if (m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, QString::fromLatin1("Authorization scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (ensureAdministratorSession(QString::fromLatin1("administrator authorization"))) {
        appendLog(QString::fromLatin1(
            "Administrator session authorized from the Authorize control."));
    }
    updateActionStates();
}

// Authorization is requested exactly once per scope: on entering Host
// Maintenance or committing a repair target. Every privileged command reuses
// the cached session afterwards, so Run All never opens the password dialog.
// The hidden-input modal is kept and the password is fed to `sudo -S -v` over
// a pipe only; it is never logged or placed on a command line.
bool LegacyMainWindow::ensureAdministratorSession(const QString &context)
{
    QString description;
    if (!m_runner->elevationNeedsPassword(&description)) {
        return true;
    }
    if (m_smokeMode) {
        // The smoke test must never block on a modal dialog: the Etch
        // validation runs as root or after `sudo -S -v` has cached the
        // timestamp, exactly like the launcher's sudo elevation.
        appendLog(QString::fromLatin1(
            "ERROR: elevation requires an interactive password; the smoke test "
            "runs as root or after sudo authentication."));
        return false;
    }
    const char *userEnv = ::getenv("USER");
    const QString user = userEnv ? QString::fromLocal8Bit(userEnv) : QString::null;
    bool ok = false;
    QString password = promptHiddenPassword(
        this, QString::fromLatin1("Administrator authorization"),
        QString::fromLatin1(
            "Administrator authorization is required for %1.\n\n"
            "Enter the password for %2 (sudo). It is used only for this sudo "
            "authentication, is sent over a pipe and is never logged or placed "
            "on a command line. The authorization is cached for this session "
            "and reused by diagnostics and repairs.")
            .arg(context)
            .arg(user.isEmpty() ? QString::fromLatin1("your account") : user),
        &ok);
    if (!ok) {
        appendLog(QString::fromLatin1(
            "Administrator authorization was cancelled; the command was not started."));
        return false;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("Password required"),
                             QString::fromLatin1(
                                 "An empty password was not submitted. Enter the "
                                 "sudo password or choose Cancel."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    QByteArray secret = password.local8Bit();
    password.fill(QChar(0));
    password = QString::null;
    QString error;
    const bool authenticated = m_runner->authenticateElevation(secret, &error);
    secret.fill('\0');
    secret = QByteArray();
    if (!authenticated) {
        appendLog(QString::fromLatin1("Administrator authorization failed: %1").arg(error));
        QMessageBox::warning(this, QString::fromLatin1("Administrator authorization failed"),
                             QString::fromLatin1(
                                 "sudo did not accept the password: %1\n\n"
                                 "The command was not started.").arg(error),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    appendLog(QString::fromLatin1(
        "Administrator authorization established with sudo for this session (%1).")
        .arg(context));
    updateElevationLabel();
    return true;
}

bool LegacyMainWindow::startCommand(const QStringList &args,
                                     bool diagnostic, const QString &label,
                                     bool unlock, bool config, bool shell,
                                     bool quiet)
{
    if (m_running) {
        return false;
    }
    if (m_smokeMode && m_runner->elevationNeedsPassword(0)) {
        // A modal password prompt would hang the headless smoke: fail with the
        // exact remedy instead.
        m_smokeMode = false;
        m_pendingConfigWrite = false;
        emit smokeFinished(false, QString::fromLatin1(
            "elevation requires an interactive sudo password; run the smoke as "
            "root or after `sudo -S -v` with --elevate 'sudo -n'"));
        return false;
    }
    if (!administratorSessionActive()) {
        // Authorization is established on Host Maintenance entry, target
        // commit or the explicit Authorize control, never here: fail closed
        // with the exact remedy instead of opening a password prompt on Run All.
        appendLog(QString::fromLatin1(
            "ERROR: administrator authorization is required for %1; press "
            "Authorize (Systems or Repair tab), enter Host Maintenance or commit "
            "a repair target to authorize this session.")
            .arg(label));
        if (!m_smokeMode && !quiet) {
            QMessageBox::warning(
                this, QString::fromLatin1("Administrator authorization required"),
                QString::fromLatin1(
                    "No administrator session is active for %1.\n\n"
                    "Press Authorize on the Systems or Repair tab to establish "
                    "the session now, or enter Host Maintenance / commit a repair "
                    "target on the Systems tab; diagnostics and repairs then "
                    "reuse the cached authorization.").arg(label),
                QMessageBox::Ok, QMessageBox::NoButton);
        }
        // Never leave a collected secret behind when authorization is missing.
        m_runner->setInputData(QByteArray());
        m_pendingConfigWrite = false;
        m_pendingConfigKey = QString::null;
        m_pendingConfigPath = QString::null;
        return false;
    }
    // A cached plain-sudo timestamp can expire between commands. Probe it
    // without blocking and fail closed with the Authorize remedy when it did;
    // otherwise the GUI would either prompt mid-command or hang on sudo's
    // password read.
    if (!sessionStillCurrent()) {
        appendLog(QString::fromLatin1(
            "ERROR: the administrator session for %1 is no longer valid; the "
            "command was not started.").arg(label));
        if (!m_smokeMode && !quiet) {
            QMessageBox::warning(
                this, QString::fromLatin1("Administrator authorization expired"),
                QString::fromLatin1(
                    "The cached administrator authorization for %1 expired or "
                    "was refused.\n\nPress Authorize on the Systems or Repair "
                    "tab to re-establish the session, then run the command "
                    "again. No command was started.").arg(label),
                QMessageBox::Ok, QMessageBox::NoButton);
        }
        m_runner->setInputData(QByteArray());
        m_pendingConfigWrite = false;
        m_pendingConfigKey = QString::null;
        m_pendingConfigPath = QString::null;
        return false;
    }
    m_transcript = QString::null;
    m_pendingDiagnostic = diagnostic;
    m_pendingUnlock = unlock;
    m_pendingConfig = config;
    m_pendingShell = shell;
    m_pendingIdentity = identity();
    m_pendingLabel = label;
    m_running = true;
    updateActionStates();

    QString elevation;
    m_runner->resolveElevation(&elevation);
    m_lastHelperDescription = elevation;
    updateElevationLabel();
    appendLog(QString::null);
    appendLog(QString::fromLatin1("=== %1 ===").arg(label));
    appendLog(QString::fromLatin1("helper: %1 (%2)").arg(m_runner->helperPath()).arg(elevation));
    appendLog(QString::fromLatin1("command: %1 %2").arg(m_runner->helperPath()).arg(args.join(QString::fromLatin1(" "))));
    if (!m_runner->run(args)) {
        // HelperRunner emitted finished(false, -1) synchronously.
        return false;
    }
    updateStatus();
    return true;
}

void LegacyMainWindow::helperLine(const QString &line)
{
    m_transcript += line;
    m_transcript += QString::fromLatin1("\n");
    appendLog(line);
    // Stream into the modern-style repair result popup when it is open.
    if (m_resultView) {
        m_resultView->append(line);
    }
}

void LegacyMainWindow::helperFinished(bool ok, int exitCode)
{
    if (m_pendingLabel.isEmpty() && !m_running) {
        return;
    }
    appendLog(QString::fromLatin1("--- %1 %2 (exit %3) ---")
                  .arg(m_pendingLabel)
                  .arg(ok ? QString::fromLatin1("finished") : QString::fromLatin1("failed"))
                  .arg(exitCode));

    // Modern-style result popup: final status line and Close enablement.
    if (m_resultStatus && m_resultCloseButton) {
        m_resultStatus->setText(ok
            ? QString::fromLatin1(
                  "Privileged operation completed successfully. Administrator "
                  "authorization remains active for this session.")
            : QString::fromLatin1(
                  "Privileged operation stopped with an error. Administrator "
                  "authorization remains active; review the output before "
                  "closing."));
        m_resultCloseButton->setEnabled(true);
    }

    // A cached sudo authorization can expire or be refused between commands:
    // drop the cached decision so the explicit Authorize control re-establishes
    // it. Only a transcript that actually shows a sudo session problem resets
    // it (a mere "sudo:" substring in unrelated target log output must not).
    if (!ok && transcriptSuggestsAuthFailure(m_transcript)) {
        m_runner->resetElevation();
        appendLog(QString::fromLatin1(
            "The sudo authorization expired or was refused; press Authorize "
            "(Systems or Repair tab) to re-establish the session."));
        updateElevationLabel();
    }

    const std::string transcript = toStd(m_transcript);
    const ParsedTranscript parsed = parseTranscript(transcript);
    const bool wasConfig = m_pendingConfig;
    const bool wasConfigWrite = m_pendingConfigWrite;
    const QString configKey = m_pendingConfigKey;
    const QString configPath = m_pendingConfigPath;
    const bool wasShell = m_pendingShell;
    QString configReadContent;
    if (m_pendingDiagnostic) {
        m_model.applyDiagnosticTranscript(toStd(m_pendingIdentity), transcript, ok);
        m_rawView->setText(m_transcript);
        if (ok) {
            updateFactView(parsed);
            mergeHelperDevices(parsed);
        }
    } else {
        m_model.applyCommandTranscript(transcript);
        reportChangeStatuses(parsed);
        if (wasConfig && !wasConfigWrite && ok) {
            // The helper's `config-read` output carries a small read-only
            // preamble; the editor receives the file content only.
            configReadContent = m_transcript;
            const QString marker = QString::fromLatin1("Inspection is read-only.\n\n");
            const int markerPos = configReadContent.find(marker);
            if (markerPos >= 0) {
                configReadContent = configReadContent.mid(markerPos + marker.length());
            }
        }
        if (wasConfig && wasConfigWrite && ok) {
            // A target configuration write can change mount, initramfs and
            // boot behavior: the complete cached target diagnostics are
            // invalidated, exactly like the modern Edit Target File dialog.
            m_model.reset();
            appendLog(QString::fromLatin1(
                "STALE DIAGNOSTICS: target configuration edited: %1. "
                "Regenerate diagnostics before the next repair.").arg(configPath));
        }
        if (wasShell && m_shellOutput) {
            // The shell command's transcript is the tab's only content; the
            // helper keeps its own containment/timeout preflights.
            m_shellOutput->setText(m_transcript);
        }
    }

    const bool wasSmoke = m_smokeMode;
    const bool wasDiagnostic = m_pendingDiagnostic;
    const bool wasUnlock = m_pendingUnlock;
    const QString unlockDevice = m_unlockDevice;
    const QString unlockDisk = m_unlockDisk;
    m_running = false;
    m_pendingDiagnostic = false;
    m_pendingUnlock = false;
    m_pendingConfig = false;
    m_pendingConfigWrite = false;
    m_pendingConfigKey = QString::null;
    m_pendingConfigPath = QString::null;
    m_pendingShell = false;
    m_pendingLabel = QString::null;
    updateActionStates();
    updateStatus();
    updateDriveDetails();

    // Config read/write follow-ups run after the run state is reset, so the
    // editor can start the guarded write without hitting the m_running guard.
    if (wasConfig && !wasConfigWrite) {
        if (ok) {
            openConfigEditor(configReadContent, configKey, configPath);
        } else {
            std::string error = unlockErrorLine(transcript);
            if (error.empty()) {
                error = toStd(m_transcript).substr(0, 400);
            }
            appendLog(QString::fromLatin1("Configuration read failed: %1")
                          .arg(fromStd(error)));
            QMessageBox::warning(this, QString::fromLatin1("Configuration unavailable"),
                                 QString::fromLatin1(
                                     "The helper could not read %1:\n\n%2")
                                     .arg(configPath).arg(fromStd(error)),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        }
    } else if (wasConfig && wasConfigWrite && !ok) {
        std::string error = unlockErrorLine(transcript);
        if (error.empty()) {
            error = toStd(m_transcript).substr(0, 400);
        }
        QMessageBox::warning(this, QString::fromLatin1("Configuration write failed"),
                             QString::fromLatin1("%1").arg(fromStd(error)),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }

    if (wasUnlock) {
        handleUnlockFinished(ok, transcript, unlockDevice, unlockDisk);
    }

    if (wasConfig && wasConfigWrite && ok) {
        maybeAutoRefreshDiagnostics(QString::fromLatin1(
            "the target configuration was edited"));
    }

    if (wasSmoke) {
        if (!ok) {
            m_smokeMode = false;
            emit smokeFinished(false, QString::fromLatin1("helper command failed (exit %1)").arg(exitCode));
            return;
        }
        if (wasDiagnostic) {
            int reported = 0;
            const std::vector<std::string> keys = capabilityKeys();
            for (std::size_t i = 0; i < keys.size(); ++i) {
                if (!m_model.state(keys[i]).empty()) {
                    ++reported;
                }
            }
            m_smokeDiagnoseOk = (reported == static_cast<int>(keys.size()));
        } else {
            m_smokeValidateOk = ok;
        }
        ++m_smokeStep;
        runSmokeStep();
    }
}

void LegacyMainWindow::handleUnlockFinished(bool ok, const std::string &transcript,
                                            const QString &device,
                                            const QString &disk)
{
    const std::string mapper = unlockMapper(transcript);
    const bool authFailed = unlockAuthFailed(transcript);
    const QString targetDisk = disk.isEmpty() ? selectedDisk() : disk;
    QString status;
    if (ok && !mapper.empty()) {
        status = QString::fromLatin1(
            "State: unlocked\nComponent: %1\nMapper: %2\n"
            "Method: helper unlock (cryptsetup; passphrase on stdin only)\n"
            "Result: mapping opened for this recovery session.")
            .arg(device).arg(fromStd(mapper));
        // Opening the mapper changes the target topology: cached diagnostics
        // for the old identity no longer describe the selected scope.
        m_model.reset();
        appendLog(QString::fromLatin1(
            "LUKS volume unlocked; the target topology changed, so cached "
            "diagnostics were invalidated. Run diagnostics again."));
        scanDevices();
        maybeAutoRefreshDiagnostics(QString::fromLatin1(
            "the LUKS unlock changed the target topology"));
    } else if (authFailed) {
        status = QString::fromLatin1(
            "State: locked\nComponent: %1\nMethod: helper unlock (cryptsetup)\n"
            "Error: the passphrase was not accepted; retry offered.")
            .arg(device);
        appendLog(QString::fromLatin1(
            "LUKS passphrase was not accepted for %1; offering retry.")
            .arg(device));
    } else {
        std::string error = unlockErrorLine(transcript);
        if (error.empty()) {
            error = "unlock failed (exit 1)";
        }
        status = QString::fromLatin1(
            "State: locked\nComponent: %1\nMethod: helper unlock (cryptsetup)\n"
            "Error: %2")
            .arg(device).arg(fromStd(error));
        appendLog(status);
    }
    m_unlockStatusCache.insert(targetDisk, status);
    updateUnlockStatus();

    if (authFailed) {
        const int answer = QMessageBox::warning(
            this, QString::fromLatin1("Passphrase not accepted"),
            QString::fromLatin1(
                "The LUKS passphrase was not accepted.\n\nTry again?"),
            QMessageBox::Retry, QMessageBox::Cancel);
        if (answer == QMessageBox::Retry) {
            m_unlockRetry = true;
            QTimer::singleShot(0, this, SLOT(runUnlock()));
        }
    }
}

void LegacyMainWindow::reportChangeStatuses(const ParsedTranscript &parsed)
{
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        const std::string &key = parsed.changeStatuses[i].first;
        const std::string &state = parsed.changeStatuses[i].second;
        if (!changeStatusIsUnchanged(state)) {
            appendLog(QString::fromLatin1("NOTE: repair status for %1 is not proven unchanged; cached diagnostics are invalidated.")
                          .arg(fromStd(key)));
        }
    }
}

// Modern scheduleEvidenceRefresh() semantics, reduced to the legacy session
// model: after an operation that invalidated the cached diagnostics, schedule
// one Run All for the current scope only when the setting is on, the scope is
// ready, no command is running and the administrator session is already
// active. It never opens an authorization prompt by itself; a missing session
// leaves the manual stale hint in place.
void LegacyMainWindow::maybeAutoRefreshDiagnostics(const QString &reason)
{
    if (!m_autoRefreshEnabled) {
        return;
    }
    if (m_smokeMode || m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        appendLog(QString::fromLatin1(
            "Automatic read-only diagnostics regeneration is pending until a "
            "scope is ready (%1).").arg(reason));
        return;
    }
    if (!administratorSessionActive()) {
        appendLog(QString::fromLatin1(
            "Automatic read-only diagnostics regeneration is pending until "
            "administrator authorization is active (%1); it never opens an "
            "authorization prompt by itself.").arg(reason));
        return;
    }
    appendLog(QString::fromLatin1(
        "Automatic read-only diagnostics regeneration scheduled after %1.")
        .arg(reason));
    QTimer::singleShot(0, this, SLOT(runScheduledAutoRefresh()));
}

void LegacyMainWindow::runScheduledAutoRefresh()
{
    if (!m_autoRefreshEnabled || m_running || !diagnosticsScopeReady()) {
        return;
    }
    if (!administratorSessionActive()) {
        return;
    }
    runDiagnosticsInternal(true);
}

void LegacyMainWindow::runSmokeStep()
{
    if (!m_smokeMode) {
        return;
    }
    if (m_smokeStep == 0) {
        if (!selectionComplete()) {
            autoDetectHostTarget();
        }
        if (!selectionComplete()) {
            m_smokeMode = false;
            emit smokeFinished(false, QString::fromLatin1("no running-host target could be detected"));
            return;
        }
        m_hostMaintenance = true;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton, QString::fromLatin1("Exit Host Maintenance"));
        }
        QStringList args;
        args << QString::fromLatin1("host-diagnose") << selectedDisk() << selectedRoot()
             << QString::fromLatin1("all");
        startCommand(args, true,
                     QString::fromLatin1("smoke host-diagnose all"));
    } else if (m_smokeStep == 1) {
        QStringList args;
        args << QString::fromLatin1("host-validate") << selectedDisk() << selectedRoot();
        startCommand(args, false,
                     QString::fromLatin1("smoke host-validate"));
    } else if (m_smokeStep == 2) {
        QString problems;
        const bool controlsOk = verifySmokeControls(&problems);
        int layoutChecked = 0;
        const bool layoutOk = verifyLayout(&problems, &layoutChecked);
        int reported = 0;
        const std::vector<std::string> keys = capabilityKeys();
        for (std::size_t i = 0; i < keys.size(); ++i) {
            if (!m_model.state(keys[i]).empty()) {
                ++reported;
            }
        }
        // A pass without checked widgets would be vacuous: every tab has
        // titles, buttons, lists and combos, so the floor stays well above 0.
        if (layoutChecked < 20) {
            problems.append(QString::fromLatin1(
                "layout verification checked only %1 widgets").arg(layoutChecked));
        }
        const bool ok = m_smokeDiagnoseOk && m_smokeValidateOk
            && reported == static_cast<int>(keys.size()) && controlsOk
            && layoutOk && layoutChecked >= 20;
        if (!problems.isEmpty()) {
            appendLog(QString::fromLatin1("smoke layout/control problems:\n%1").arg(problems));
        }
        const QString summary = QString::fromLatin1(
            "host-diagnose: %1 capability lines parsed; host-validate: %2; "
            "controls: %3; layout: %4 (%5 widgets checked)")
            .arg(reported)
            .arg(m_smokeValidateOk ? QString::fromLatin1("ok") : QString::fromLatin1("incomplete"))
            .arg(controlsOk ? QString::fromLatin1("ok") : QString::fromLatin1("FAIL"))
            .arg(layoutOk ? QString::fromLatin1("ok") : QString::fromLatin1("FAIL"))
            .arg(layoutChecked);
        m_smokeMode = false;
        emit smokeFinished(ok, summary);
    }
}

void LegacyMainWindow::startSmokeTest()
{
    m_smokeMode = true;
    m_smokeStep = 0;
    m_smokeDiagnoseOk = false;
    m_smokeValidateOk = false;
    // The layout contract is asserted at 1024x768: resize before the first
    // event-loop turn so the verification step sees the final geometry.
    resize(1024, 768);
    QTimer::singleShot(0, this, SLOT(runSmokeStep()));
}

bool LegacyMainWindow::verifySmokeControls(QString *problems)
{
    bool ok = true;
    if (!m_tabs || m_tabs->count() != 8) {
        problems->append(QString::fromLatin1("tab set missing (expected 8 tabs)"));
        ok = false;
    } else {
        // The Chroot Shell tab label switches to "Host Shell" in host mode
        // (modern updateChrootShellMode); the smoke runs with Host Maintenance
        // active, so expect the host label here.
        const char *expectedTabs[] = {
            "Systems", "Diagnostics", "Repair", "Host Shell", "File Copy",
            "Logs", "Settings", "About"
        };
        for (int i = 0; i < 8; ++i) {
            if (m_tabs->label(i) != QString::fromLatin1(expectedTabs[i])) {
                problems->append(QString::fromLatin1("tab %1 is '%2', expected '%3'")
                                     .arg(i).arg(m_tabs->label(i))
                                     .arg(QString::fromLatin1(expectedTabs[i])));
                ok = false;
            }
        }
    }
    // Global header (modern parity): title, subtitle and guarded-repair badge.
    if (!m_headerTitle || m_headerTitle->text() != QString::fromLatin1("Boot Bitch")) {
        problems->append(QString::fromLatin1("global header title missing"));
        ok = false;
    }
    if (!m_headerSubtitle
        || m_headerSubtitle->text() != QString::fromLatin1("Linux recovery and boot-repair utility")) {
        problems->append(QString::fromLatin1("global header subtitle missing"));
        ok = false;
    }
    if (!m_headerBadge
        || !m_headerBadge->text().contains(QString::fromLatin1("GUARDED REPAIR"))
        || !m_headerBadge->text().contains(QString::fromLatin1(LEGACY_VERSION))) {
        problems->append(QString::fromLatin1("guarded-repair version badge missing"));
        ok = false;
    }
    // Section titles: the page headings plus the sectionTitle label inside
    // every titleless group frame (Qt3 clips QGroupBox titles).
    if (m_sectionTitles.size() < 24) {
        problems->append(QString::fromLatin1("section titles missing (found %1)")
                             .arg(static_cast<int>(m_sectionTitles.size())));
        ok = false;
    }
    if (!m_detailList || m_detailList->childCount() < 10) {
        problems->append(QString::fromLatin1("selected-drive details panel missing or incomplete"));
        ok = false;
    }
    if (!m_unlockStatusView || !m_unlockStatusView->text().contains(QString::fromLatin1("State:"))) {
        problems->append(QString::fromLatin1("unlock status panel missing or not showing unlock state"));
        ok = false;
    }
    if (!m_logSearchEdit) {
        problems->append(QString::fromLatin1("log search control missing"));
        ok = false;
    }
    if (!m_sessionLogList || m_sessionLogList->childCount() < 1) {
        problems->append(QString::fromLatin1("session log list missing or empty"));
        ok = false;
    }
    if (!m_logWrapCheck) {
        problems->append(QString::fromLatin1("log wrap setting missing"));
        ok = false;
    }
    if (!m_unlockButton || !m_setTargetButton || !m_hostMaintenanceButton) {
        problems->append(QString::fromLatin1("target/unlock/host-maintenance controls missing"));
        ok = false;
    }
    if (!m_authorizeButton || !m_authStatusLabel
        || !m_repairAuthorizeButton || !m_repairAuthStatusLabel) {
        problems->append(QString::fromLatin1("deferred-authorization controls missing"));
        ok = false;
    }
    if (!m_elevationLabel || m_elevationLabel->text().isEmpty() || !m_elevateButton) {
        problems->append(QString::fromLatin1("elevation control missing or empty"));
        ok = false;
    }
    if (!m_logFilterCombo || m_logFilterCombo->count() != 2) {
        problems->append(QString::fromLatin1("log filter missing (all/errors)"));
        ok = false;
    }
    // Repair parity: the Full Repair plan section and the tools list/Selected
    // tool pane replace the old button grid and the removed unsupported list.
    if (!m_planParagraph || !m_planCountLabel || !m_planReadinessLabel
        || !m_planStageList || !m_configurePlanButton || !m_runFullRepairButton) {
        problems->append(QString::fromLatin1("Full Repair plan controls missing"));
        ok = false;
    }
    if (m_configurePlanButton
        && m_configurePlanButton->text() != QString::fromLatin1("Configure Plan...")) {
        problems->append(QString::fromLatin1("Configure Plan... text changed: '%1'")
                             .arg(m_configurePlanButton->text()));
        ok = false;
    }
    if (m_runFullRepairButton
        && m_runFullRepairButton->text() != QString::fromLatin1("Run Full Repair")) {
        problems->append(QString::fromLatin1("Run Full Repair text changed: '%1'")
                             .arg(m_runFullRepairButton->text()));
        ok = false;
    }
    if (!m_toolList || m_toolList->childCount() != toolSpecCount) {
        problems->append(QString::fromLatin1(
            "individual repair tools list missing or incomplete (count=%1)")
            .arg(m_toolList ? m_toolList->childCount() : -1));
        ok = false;
    }
    if (m_toolList && m_toolList->columns() != 2) {
        problems->append(QString::fromLatin1(
            "tools list must carry the Tool and Full Repair columns"));
        ok = false;
    }
    if (m_toolList && m_toolList->firstChild()
        && m_toolList->firstChild()->text(0) != QString::fromLatin1("Validate environment")) {
        problems->append(QString::fromLatin1(
            "tools list does not start with Validate environment (first='%1')")
            .arg(m_toolList->firstChild()->text(0)));
        ok = false;
    }
    if (!m_toolTitle || !m_toolDescription || !m_toolPlanStatus || !m_toolRunButton) {
        problems->append(QString::fromLatin1("Selected tool pane missing"));
        ok = false;
    }
    if (m_planChecks.size() != static_cast<std::size_t>(planSpecCount)) {
        problems->append(QString::fromLatin1(
            "Settings Full Repair plan checkboxes missing (found %1)")
            .arg(static_cast<int>(m_planChecks.size())));
        ok = false;
    } else {
        for (int i = 0; i < planSpecCount; ++i) {
            if (!m_planChecks[i] || !m_planChecks[i]->isEnabled()) {
                problems->append(QString::fromLatin1(
                    "Full Repair plan checkbox missing or disabled: %1")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel)));
                ok = false;
            }
        }
    }
    // Plan/tools behavior: selecting a legacy-runnable tool fills the Selected
    // tool pane, a display-only tool disables the run button with the exact
    // reason, and a Settings plan checkbox toggles the tool's Full Repair
    // column.
    if (m_toolList && m_planChecks.size() == static_cast<std::size_t>(planSpecCount)
        && m_toolTitle && m_toolRunButton && m_toolPlanStatus) {
        int dpkgRow = -1;
        int dkmsRow = -1;
        int rowIndex = 0;
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling(), ++rowIndex) {
            if (row->text(0) == QString::fromLatin1("Complete package configuration")) {
                dpkgRow = rowIndex;
            } else if (row->text(0) == QString::fromLatin1("DKMS")) {
                dkmsRow = rowIndex;
            }
        }
        if (dpkgRow < 0 || dkmsRow < 0) {
            problems->append(QString::fromLatin1("tool list rows missing (dpkg/dkms)"));
            ok = false;
        } else {
            QListViewItem *dpkgItem = m_toolList->firstChild();
            for (int step = 0; dpkgItem && step < dpkgRow; ++step) {
                dpkgItem = dpkgItem->nextSibling();
            }
            m_toolList->setSelected(dpkgItem, true);
            m_toolList->setCurrentItem(dpkgItem);
            updateToolDetails();
            if (m_toolTitle->text() != QString::fromLatin1("Complete package configuration")
                || m_toolRunButton->text() != QString::fromLatin1("Complete Configuration")) {
                problems->append(QString::fromLatin1(
                    "Selected tool pane does not follow the selected tool (title='%1', button='%2')")
                    .arg(m_toolTitle->text()).arg(m_toolRunButton->text()));
                ok = false;
            }
            // The Full Repair column mirrors the Settings checkbox when the
            // stage's capability is available (otherwise the exact
            // "Unavailable: ..." reason correctly overrides it).
            const int dpkgPlan = planIndexForStage("dpkg-configure");
            if (dpkgPlan >= 0 && planStageAvailable(dpkgPlan, 0)) {
                const bool original = m_planChecks[dpkgPlan]->isChecked();
                m_planChecks[dpkgPlan]->setChecked(!original);
                const QString offText = dpkgItem->text(1);
                m_planChecks[dpkgPlan]->setChecked(original);
                const QString onText = dpkgItem->text(1);
                if (offText.find(QString::fromLatin1("Disabled in Settings")) < 0
                    || onText.find(QString::fromLatin1("Enabled in Settings")) < 0) {
                    problems->append(QString::fromLatin1(
                        "Full Repair column does not mirror the Settings checkbox"));
                    ok = false;
                }
            }
            // A display-only tool is disabled with the exact reason.
            QListViewItem *dkmsItem = m_toolList->firstChild();
            for (int step = 0; dkmsItem && step < dkmsRow; ++step) {
                dkmsItem = dkmsItem->nextSibling();
            }
            m_toolList->setSelected(dkmsItem, true);
            m_toolList->setCurrentItem(dkmsItem);
            updateToolDetails();
            if (m_toolRunButton->isEnabled()) {
                problems->append(QString::fromLatin1(
                    "display-only DKMS tool has an enabled run button"));
                ok = false;
            }
            if (m_toolPlanStatus->text().find(QString::fromLatin1("Unavailable")) < 0) {
                problems->append(QString::fromLatin1(
                    "display-only DKMS tool does not show its unavailable reason"));
                ok = false;
            }
            // The DKMS plan status must carry the reason directly, without a
            // cosmetic leading blank line (its plan text is empty).
            if (m_toolPlanStatus->text().startsWith(QString::fromLatin1("\n"))) {
                problems->append(QString::fromLatin1(
                    "display-only tool plan status starts with a blank line"));
                ok = false;
            }
            // Every tool title plus the placeholder must fit the Selected tool
            // pane at 1024x768: the title wraps (Qt::WordBreak) and its rect
            // must stay inside the pane, re-verified per title so no long
            // tool name can clip.
            if (!(m_toolTitle->alignment() & Qt::WordBreak)) {
                problems->append(QString::fromLatin1(
                    "Selected tool title does not wrap (Qt::WordBreak)"));
                ok = false;
            }
            QStringList titleTexts;
            titleTexts.append(QString::fromLatin1("Select a repair tool"));
            for (int t = 0; t < toolSpecCount; ++t) {
                titleTexts.append(QString::fromLatin1(toolSpecs[t].title));
            }
            QWidget *titlePane = m_toolTitle->parentWidget();
            const QFontMetrics titleMetrics(m_toolTitle->font());
            for (QStringList::ConstIterator it = titleTexts.begin();
                 it != titleTexts.end(); ++it) {
                m_toolTitle->setText(*it);
                // Two passes: the first delivers the layout invalidation, the
                // second the resize the wrap-height measurement needs.
                qApp->processEvents();
                qApp->processEvents();
                const QRect mapped(
                    m_toolTitle->mapTo(titlePane, QPoint(0, 0)),
                    m_toolTitle->size());
                if (!titlePane->rect().contains(mapped)) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' overflows the Selected tool pane")
                        .arg(*it));
                    ok = false;
                }
                if (m_toolTitle->width() < 40) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' has no usable width").arg(*it));
                    ok = false;
                }
                // When the text is wider than the label, the WordBreak label
                // must have grown to at least two lines, or the wrapped text
                // clips vertically inside a one-line label. The Etch Qt 3.3.7
                // font renders a wrapped two-line label at one pixel less
                // than 2 * lineSpacing() (43px vs 44px), so the threshold
                // tolerates that rounding; a one-line label (~22px) still
                // fails.
                if (titleMetrics.width(*it) > m_toolTitle->width()
                    && m_toolTitle->height() < 2 * titleMetrics.lineSpacing() - 1) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' wraps without growing to two lines "
                        "(%2px wide, %3px tall)")
                        .arg(*it).arg(m_toolTitle->width())
                        .arg(m_toolTitle->height()));
                    ok = false;
                }
            }
            // Restore the first tool selection.
            if (m_toolList->firstChild()) {
                m_toolList->setSelected(m_toolList->firstChild(), true);
                m_toolList->setCurrentItem(m_toolList->firstChild());
                updateToolDetails();
            }
        }
    }
    if (!m_diagnosticList
        || m_diagnosticList->childCount() != static_cast<int>(legacy::diagnosticKeys().size())) {
        problems->append(QString::fromLatin1(
            "per-diagnostic check list missing or incomplete (count=%1)")
            .arg(m_diagnosticList ? m_diagnosticList->childCount() : -1));
        ok = false;
    }
    if (m_diagnosticList && m_diagnosticList->columns() != 1) {
        problems->append(QString::fromLatin1(
            "diagnostic check list must carry the single Check column"));
        ok = false;
    }
    if (!m_configCombo || !m_configButton || !m_configLabel || !m_configReasonLabel) {
        problems->append(QString::fromLatin1("target configuration controls missing"));
        ok = false;
    }
    if (!m_runDiagnosticButton || !m_copyResultsButton || !m_saveResultsButton) {
        problems->append(QString::fromLatin1("diagnostic run/copy/save controls missing"));
        ok = false;
    }
    if (!m_scopeLabel || !m_diagTitle || !m_diagDescription || !m_diagAvailability) {
        problems->append(QString::fromLatin1("selected-diagnostic pane missing"));
        ok = false;
    }
    if (m_runDiagnosticButton
        && m_runDiagnosticButton->text() != QString::fromLatin1("Run Diagnostic")) {
        problems->append(QString::fromLatin1("Run Diagnostic button text changed: '%1'")
                             .arg(m_runDiagnosticButton->text()));
        ok = false;
    }
    if (m_diagnosticsButton
        && m_diagnosticsButton->text() != QString::fromLatin1("Run All")) {
        problems->append(QString::fromLatin1("Run All button text changed: '%1'")
                             .arg(m_diagnosticsButton->text()));
        ok = false;
    }
    if (m_setTargetButton
        && m_setTargetButton->text() != QString::fromLatin1("Select Target")) {
        problems->append(QString::fromLatin1("Select Target button text changed: '%1'")
                             .arg(m_setTargetButton->text()));
        ok = false;
    }
    if (m_scanButton
        && m_scanButton->text() != QString::fromLatin1("Refresh Devices")) {
        problems->append(QString::fromLatin1("Refresh Devices button text changed: '%1'")
                             .arg(m_scanButton->text()));
        ok = false;
    }

    // B2 menus: File (Refresh Devices, Lock Administrator Session, Quit),
    // View (tab shortcuts, Auto-size Device Columns, checkable Wrap Log Lines)
    // and Help (Using Boot Bitch, About Boot Bitch). Qt3's auto-generated menu
    // ids are NEGATIVE, so validity is tracked separately.
    if (!m_fileMenu || !m_viewMenu || !m_helpMenu || !m_wrapLogsMenuValid) {
        problems->append(QString::fromLatin1("modern menu structure missing"));
        ok = false;
    } else {
        struct MenuItemCheck {
            QPopupMenu *menu;
            const char *label;
        };
        const MenuItemCheck menuChecks[] = {
            { m_fileMenu, "Refresh Devices" },
            { m_fileMenu, "Lock Administrator Session" },
            { m_fileMenu, "Quit" },
            { m_viewMenu, "Systems" },
            { m_viewMenu, "Diagnostics" },
            { m_viewMenu, "Logs" },
            { m_viewMenu, "Settings" },
            { m_viewMenu, "Auto-size Device Columns" },
            { m_viewMenu, "Wrap Log Lines" },
            { m_helpMenu, "Using Boot Bitch" },
            { m_helpMenu, "About Boot Bitch" }
        };
        for (int i = 0; i < static_cast<int>(sizeof(menuChecks) / sizeof(menuChecks[0])); ++i) {
            if (!menuItemId(menuChecks[i].menu,
                            QString::fromLatin1(menuChecks[i].label), 0)) {
                problems->append(QString::fromLatin1("menu item missing: %1")
                                     .arg(QString::fromLatin1(menuChecks[i].label)));
                ok = false;
            }
        }
        if (m_viewMenu->isItemChecked(m_wrapLogsMenuId) != m_logWrapEnabled) {
            problems->append(QString::fromLatin1("Wrap Log Lines menu check is out of sync"));
            ok = false;
        }
    }
    // View tab shortcuts must switch the pages.
    if (m_tabs) {
        const int originalPage = m_tabs->currentPageIndex();
        showLogsTab();
        if (m_tabs->currentPageIndex() != 5) {
            problems->append(QString::fromLatin1("View > Logs did not select the Logs tab"));
            ok = false;
        }
        showSettingsTab();
        if (m_tabs->currentPageIndex() != 6) {
            problems->append(QString::fromLatin1("View > Settings did not select the Settings tab"));
            ok = false;
        }
        showSystemsTab();
        if (m_tabs->currentPageIndex() != 0) {
            problems->append(QString::fromLatin1("View > Systems did not select the Systems tab"));
            ok = false;
        }
        showDiagnosticsTab();
        if (m_tabs->currentPageIndex() != 1) {
            problems->append(QString::fromLatin1("View > Diagnostics did not select the Diagnostics tab"));
            ok = false;
        }
        m_tabs->setCurrentPage(originalPage);
    }
    // Wrap Log Lines sync both ways: toggling the Settings checkbox updates
    // the menu check mark, and activating the View menu item updates the
    // checkbox and persists through saveLegacySettings().
    if (m_logWrapCheck && m_viewMenu && m_wrapLogsMenuValid) {
        const bool originalWrap = m_logWrapEnabled;
        // Settings checkbox -> state + menu check mark.
        m_logWrapCheck->setChecked(!originalWrap);
        const bool afterCheckbox = m_logWrapEnabled;
        if (afterCheckbox == originalWrap) {
            problems->append(QString::fromLatin1(
                "Settings wrap checkbox did not change the wrap state"));
            ok = false;
        }
        if (m_viewMenu->isItemChecked(m_wrapLogsMenuId) != afterCheckbox) {
            problems->append(QString::fromLatin1(
                "Wrap Log Lines menu check did not follow the Settings checkbox"));
            ok = false;
        }
        // View menu item -> state + checkbox (this also restores the state).
        toggleLogWrapFromMenu();
        if (m_logWrapEnabled != originalWrap) {
            problems->append(QString::fromLatin1(
                "Wrap Log Lines menu toggle did not restore the state"));
            ok = false;
        }
        if (m_logWrapCheck->isChecked() != m_logWrapEnabled) {
            problems->append(QString::fromLatin1(
                "Settings wrap checkbox did not follow the menu item"));
            ok = false;
        }
        if (m_viewMenu->isItemChecked(m_wrapLogsMenuId) != m_logWrapEnabled) {
            problems->append(QString::fromLatin1(
                "Wrap Log Lines menu check is out of sync after the menu toggle"));
            ok = false;
        }
        // Persistence: only asserted when the environment can write settings
        // (a read-only home must not fail a correct GUI).
        QSettings probe;
        probe.setPath(QString::fromLatin1("boot-bitch.local"),
                      QString::fromLatin1("boot-bitch-legacy"), QSettings::User);
        const bool settingsWritable = probe.writeEntry(
            QString::fromLatin1("/logs/wrapLines"), m_logWrapEnabled);
        if (settingsWritable) {
            QSettings readBack;
            readBack.setPath(QString::fromLatin1("boot-bitch.local"),
                             QString::fromLatin1("boot-bitch-legacy"), QSettings::User);
            if (readBack.readBoolEntry(QString::fromLatin1("/logs/wrapLines"),
                                       !m_logWrapEnabled) != m_logWrapEnabled) {
                problems->append(QString::fromLatin1(
                    "Wrap Log Lines state was not persisted through QSettings"));
                ok = false;
            }
        }
    }
    // Auto-size Device Columns must keep every column visible and the last
    // column stretched (verifyLayout re-checks both afterwards).
    autoSizeDeviceColumns();

    // Logs parity controls: New Session Log / Add Note / Delete / Refresh and
    // the prior-log banner (hidden while the live register is shown).
    if (!m_newSessionLogButton || !m_addNoteButton || !m_deleteSessionLogButton) {
        problems->append(QString::fromLatin1("session log management controls missing"));
        ok = false;
    }
    if (!m_priorLogBanner || m_priorLogBanner->isVisible()) {
        problems->append(QString::fromLatin1("prior-log banner missing or shown for the live register"));
        ok = false;
    }

    // Settings parity: functional device-discovery filters, functional
    // diagnostics auto-refresh and the read-only host capabilities group.
    if (!m_showNonLinuxCheck || !m_showRemovableCheck || !m_showEncryptedCheck
        || !m_autoRefreshCheck) {
        problems->append(QString::fromLatin1("Settings filter/auto-refresh controls missing"));
        ok = false;
    } else if (!m_showNonLinuxCheck->isEnabled() || !m_showRemovableCheck->isEnabled()
               || !m_showEncryptedCheck->isEnabled() || !m_autoRefreshCheck->isEnabled()) {
        problems->append(QString::fromLatin1("Settings filter/auto-refresh controls are disabled"));
        ok = false;
    }
    if (!m_capDistributionLabel || !m_capPackageManagerLabel || !m_capServiceLabel
        || !m_capDisplayLabel || !m_capInitramfsLabel || !m_capBootloaderLabel
        || !m_capLoggingLabel || !m_refreshCapabilitiesButton) {
        problems->append(QString::fromLatin1("host capabilities group missing"));
        ok = false;
    }

    // Diagnostics list parity: the friendly titles are shown, the stable keys
    // stay internal.
    if (m_diagnosticList && m_diagnosticList->firstChild()
        && m_diagnosticList->firstChild()->text(0) != QString::fromLatin1("Environment validation")) {
        problems->append(QString::fromLatin1(
            "diagnostic list does not show the friendly titles (first='%1')")
            .arg(m_diagnosticList->firstChild()->text(0)));
        ok = false;
    }

    // Read-only udev metadata probe: a locked LUKS container must be visible in
    // the inventory with its probed type (the GUI never opens the device).
    if (m_deviceList) {
        for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
             it != m_rows.end(); ++it) {
            const DeviceRow &row = it.data();
            if (!row.encrypted || !row.fstype.empty()) {
                continue;
            }
            for (QListViewItem *item = m_deviceList->firstChild(); item;
                 item = item->nextSibling()) {
                if (item->text(0) != it.key()) {
                    continue;
                }
                if (item->text(3) != QString::fromLatin1("crypto_LUKS")) {
                    problems->append(QString::fromLatin1(
                        "locked LUKS device %1 is not shown with its probed type (row: '%2')")
                        .arg(it.key()).arg(item->text(3)));
                    ok = false;
                }
                break;
            }
        }
    }

    // Device-discovery filters: unchecking "Show encrypted devices before
    // unlocking" hides the encrypted drive's rows (and never loses the
    // selected/committed target state); restoring shows them again.
    if (m_showEncryptedCheck && m_showEncryptedCheck->isChecked() && m_deviceList) {
        const QString selectedBefore = selectedDisk();
        const int rowsBefore = m_deviceList->childCount();
        m_showEncryptedCheck->setChecked(false);
        const int rowsFiltered = m_deviceList->childCount();
        if (rowsFiltered > rowsBefore) {
            problems->append(QString::fromLatin1(
                "device-discovery filter added rows instead of hiding them"));
            ok = false;
        }
        if (!selectedBefore.isEmpty() && selectedDisk() != selectedBefore) {
            problems->append(QString::fromLatin1(
                "hiding the selected drive's row lost the selected target state"));
            ok = false;
        }
        m_showEncryptedCheck->setChecked(true);
        if (m_deviceList->childCount() != rowsBefore) {
            problems->append(QString::fromLatin1(
                "restoring the device-discovery filter did not restore the rows"));
            ok = false;
        }
    }

    // Fail closed for the protected running host: Unlock and Select Target must
    // stay disabled and their tooltips must name Host Maintenance as the path.
    const QString hostDisk = runningHostDisk();
    m_hostMaintenance = false;
    if (m_hostMaintenanceButton) {
        updateButtonText(m_hostMaintenanceButton, QString::fromLatin1("Host Maintenance"));
    }
    if (!hostDisk.isEmpty()) {
        setTarget(hostDisk, selectedRoot());
    }
    updateActionStates();
    if (m_unlockButton && m_unlockButton->isEnabled()) {
        problems->append(QString::fromLatin1("unlock button enabled for the running host scope"));
        ok = false;
    }
    if (!hostDisk.isEmpty() && m_unlockButton
        && QToolTip::textFor(m_unlockButton).find(QString::fromLatin1("Host Maintenance")) < 0) {
        problems->append(QString::fromLatin1(
            "unlock tooltip for the protected host does not name Host Maintenance"));
        ok = false;
    }
    if (!hostDisk.isEmpty() && m_setTargetButton
        && QToolTip::textFor(m_setTargetButton).find(QString::fromLatin1("Host Maintenance")) < 0) {
        problems->append(QString::fromLatin1(
            "Select Target tooltip for the protected host does not name Host Maintenance"));
        ok = false;
    }
    if (!hostDisk.isEmpty() && m_setTargetButton && m_setTargetButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Select Target enabled for the protected running-host disk"));
        ok = false;
    }
    // The protected host must be shown as protected in the details pane.
    if (!hostDisk.isEmpty() && m_detailList) {
        bool protectedShown = false;
        for (QListViewItem *item = m_detailList->firstChild(); item;
             item = item->nextSibling()) {
            if (item->text(0) == QString::fromLatin1("Protection:")
                && item->text(1).contains(QString::fromLatin1("PROTECTED"))) {
                protectedShown = true;
                break;
            }
        }
        if (!protectedShown) {
            problems->append(QString::fromLatin1(
                "protected running host is not shown as PROTECTED in the details pane"));
            ok = false;
        }
    }

    // Modern gating: Run All needs a committed repair target or active Host
    // Maintenance, never just an auto-filled selection.
    if (m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All enabled without a committed target or Host Maintenance"));
        ok = false;
    }
    if (m_configButton && m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1("configuration viewer enabled for the running host scope"));
        ok = false;
    }
    if (m_scopeLabel
        && m_scopeLabel->text() != QString::fromLatin1("Target: none selected")) {
        problems->append(QString::fromLatin1(
            "scope label is '%1', expected 'Target: none selected'")
            .arg(m_scopeLabel->text()));
        ok = false;
    }

    // Target eligibility without a scope selector: any selectable
    // non-running-host disk can be committed, the protected running-host disk
    // stays excluded.
    const QString previousDisk = selectedDisk();
    const QString previousRoot = selectedRoot();
    setTarget(QString::fromLatin1("/dev/bootrepair-smoke-target"),
              QString::fromLatin1("/dev/bootrepair-smoke-target1"));
    updateActionStates();
    if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Select Target disabled for a selectable non-host disk"));
        ok = false;
    }
    if (m_unlockButton && m_unlockButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Unlock enabled for a disk without a locked LUKS component"));
        ok = false;
    }
    if (administratorSessionActive()) {
        // Exercise the real commit path (the smoke runs as root, with
        // --no-elevate or with an authenticated `sudo -n`).
        setRepairTarget();
        if (!targetCommitted() || !m_diagnosticsButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "committing a selectable non-host disk did not enable diagnostics"));
            ok = false;
        }
        if (m_scopeLabel
            && !m_scopeLabel->text().contains(QString::fromLatin1("Target: /dev/bootrepair-smoke-target"))) {
            problems->append(QString::fromLatin1(
                "committed scope label is '%1'").arg(m_scopeLabel->text()));
            ok = false;
        }
        m_targetCommitted = false;
    }
    if (!previousDisk.isEmpty()) {
        setTarget(previousDisk, previousRoot);
    }
    updateActionStates();

    m_hostMaintenance = true;
    updateActionStates();
    if (!m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All disabled while Host Maintenance is active"));
        ok = false;
    }
    if (!m_runDiagnosticButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run Diagnostic disabled while Host Maintenance is active"));
        ok = false;
    }
    if (m_scopeLabel
        && !m_scopeLabel->text().startsWith(QString::fromLatin1("Host maintenance: "))) {
        problems->append(QString::fromLatin1(
            "host scope label is '%1'").arg(m_scopeLabel->text()));
        ok = false;
    }
    // Modern parity: the Chroot Shell heading/tab/button switch to host mode.
    if (m_chrootHeading && m_chrootHeading->text() != QString::fromLatin1("Host shell")) {
        problems->append(QString::fromLatin1("Chroot Shell heading did not switch to Host shell"));
        ok = false;
    }
    if (m_shellRunButton && m_shellRunButton->text() != QString::fromLatin1("Run on Host")) {
        problems->append(QString::fromLatin1("shell run button did not switch to Run on Host"));
        ok = false;
    }
    if (m_tabs && m_tabs->label(3) != QString::fromLatin1("Host Shell")) {
        problems->append(QString::fromLatin1("Chroot Shell tab label did not switch to Host Shell"));
        ok = false;
    }
    // Modern parity: the target configuration row (and Edit Target File) is
    // never shown or enabled for Host Maintenance.
    if (m_configCombo && !m_configCombo->isHidden()) {
        problems->append(QString::fromLatin1(
            "target configuration row shown for Host Maintenance"));
        ok = false;
    }
    if (m_configButton && m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Edit Target File enabled for Host Maintenance"));
        ok = false;
    }
    m_hostMaintenance = false;
    m_targetCommitted = true;
    m_committedDisk = selectedDisk();
    m_committedRoot = selectedRoot();
    updateActionStates();
    if (!m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All disabled for a committed repair target"));
        ok = false;
    }
    if (m_configCombo && m_configCombo->isHidden()) {
        problems->append(QString::fromLatin1(
            "target configuration row hidden for a committed repair target"));
        ok = false;
    }
    if (m_configCombo && m_configCombo->count() != configSpecCount) {
        problems->append(QString::fromLatin1(
            "target configuration list incomplete for an unprobed target (count=%1)")
            .arg(m_configCombo->count()));
        ok = false;
    }
    if (m_configButton && !m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1("Edit Target File disabled for a committed repair target"));
        ok = false;
    }
    if (m_configCombo && !m_configCombo->isHidden()
        && !comboTextFits(m_configCombo, QString::fromLatin1("configuration combo"), problems)) {
        ok = false;
    }
    if (m_scopeLabel
        && !m_scopeLabel->text().contains(QString::fromLatin1("Target: "))) {
        problems->append(QString::fromLatin1(
            "committed scope label is '%1'").arg(m_scopeLabel->text()));
        ok = false;
    }
    m_targetCommitted = false;
    updateActionStates();
    if (m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All enabled for an uncommitted target"));
        ok = false;
    }
    if (m_configCombo && !m_configCombo->isHidden()) {
        problems->append(QString::fromLatin1(
            "target configuration row shown for an uncommitted target"));
        ok = false;
    }
    if (m_configButton && m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Edit Target File enabled for an uncommitted target"));
        ok = false;
    }
    // Restore the running-host scope the smoke's helper commands used.
    m_hostMaintenance = true;
    if (m_hostMaintenanceButton) {
        updateButtonText(m_hostMaintenanceButton, QString::fromLatin1("Exit Host Maintenance"));
    }
    updateActionStates();

    // The helper commands already ran, so no modal authorization may be
    // pending: the smoke requires root, --no-elevate or `sudo -n`.
    if (!administratorSessionActive()) {
        problems->append(QString::fromLatin1(
            "smoke session is not authorized although helper commands ran"));
        ok = false;
    }
    if (m_authStatusLabel
        && m_authStatusLabel->text().find(QString::fromLatin1("Authorization")) < 0) {
        problems->append(QString::fromLatin1("authorization status label is empty"));
        ok = false;
    }

    // Probe-based legacy feature gating (fail closed): the chroot/file-copy
    // probe reasons and the host-maintenance-gated tools must follow the
    // cached `Legacy feature` lines, never hardcoded text.
    const QString shellFeature = scopeFeatureKey();
    if (m_chrootReasonLabel
        && !m_chrootReasonLabel->text().contains(
               QString::fromLatin1("Legacy feature %1:").arg(shellFeature))) {
        problems->append(QString::fromLatin1(
            "Chroot Shell reason does not name the helper probe line (%1)")
            .arg(shellFeature));
        ok = false;
    }
    // Chroot Shell gating: the controls must be exactly as enabled as the
    // scope's cached feature probe plus the committed scope and the cached
    // administrator session allow (fail closed).
    {
        std::string shellFeatureReason;
        const bool shellFeatureAvailable = m_model.legacyFeatureAvailable(
            toStd(shellFeature), toStd(identity()), &shellFeatureReason);
        const bool shellExpected = shellFeatureAvailable
            && diagnosticsScopeReady() && administratorSessionActive();
        if (m_shellRunButton && m_shellRunButton->isEnabled() != shellExpected) {
            problems->append(QString::fromLatin1(
                "chroot shell enabled=%1 but feature=%2 scope-ready=%3 authorized=%4")
                .arg(m_shellRunButton->isEnabled()
                         ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                .arg(shellFeatureAvailable
                         ? QString::fromLatin1("available") : QString::fromLatin1("unavailable"))
                .arg(diagnosticsScopeReady()
                         ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                .arg(administratorSessionActive()
                         ? QString::fromLatin1("yes") : QString::fromLatin1("no")));
            ok = false;
        }
        if (m_shellCommandEdit
            && m_shellCommandEdit->isEnabled() != shellExpected) {
            problems->append(QString::fromLatin1(
                "chroot shell command field does not follow the shell gating"));
            ok = false;
        }
    }
    if (m_fileCopyReasonLabel
        && !m_fileCopyReasonLabel->text().contains(QString::fromLatin1("Legacy feature file-copy:"))) {
        problems->append(QString::fromLatin1(
            "File Copy reason does not name the helper probe line"));
        ok = false;
    }
    // Host-maintenance feature gate: the host initramfs/grub tools must be
    // exactly as runnable as the cached `Legacy feature host-maintenance:` line
    // allows (both also need their capability line).
    std::string maintenanceReason;
    const bool maintenanceAvailable = m_model.legacyFeatureAvailable(
        "host-maintenance", toStd(identity()), &maintenanceReason);
    static const char *const gatedKeys[] = { "initramfs", "grub" };
    for (int i = 0; i < 2; ++i) {
        int toolIndex = -1;
        for (int t = 0; t < toolSpecCount; ++t) {
            if (QString::fromLatin1(toolSpecs[t].key) == QString::fromLatin1(gatedKeys[i])) {
                toolIndex = t;
                break;
            }
        }
        if (toolIndex < 0) {
            problems->append(QString::fromLatin1("host-maintenance-gated tool missing: %1")
                                 .arg(QString::fromLatin1(gatedKeys[i])));
            ok = false;
            continue;
        }
        QString runReason;
        const bool runnable = toolRunReady(toolIndex, &runReason);
        std::string capabilityReason;
        const bool capabilityAvailable = m_model.isAvailable(
            gatedKeys[i], toStd(identity()), &capabilityReason);
        const bool expected = capabilityAvailable && maintenanceAvailable;
        if (runnable != expected) {
            problems->append(QString::fromLatin1(
                "host %1 tool runnable=%2 but capability=%3 host-maintenance=%4")
                .arg(QString::fromLatin1(gatedKeys[i]))
                .arg(runnable ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                .arg(capabilityAvailable ? QString::fromLatin1("available") : QString::fromLatin1("unavailable"))
                .arg(maintenanceAvailable ? QString::fromLatin1("available") : QString::fromLatin1("unavailable")));
            ok = false;
        }
    }

    // Target configuration probe: after the running-host diagnostics the
    // target identity still has no cached `Legacy config` lines, so the row
    // lists every Etch key and the reason label asks for diagnostics; a
    // committed target with a cached unavailable line must omit that file and
    // show the helper's exact reason instead.
    if (m_configCombo && m_configReasonLabel) {
        static const char *const expectedKeys[] = {
            "fstab", "inittab", "menu-lst", "crypttab", "modules", "interfaces",
            "sources-list", "apt-conf"
        };
        const std::vector<std::string> keys = legacy::configFileKeys();
        bool keysMatch = keys.size() == 8;
        for (std::size_t i = 0; keysMatch && i < keys.size(); ++i) {
            keysMatch = keys[i] == expectedKeys[i];
        }
        if (!keysMatch) {
            problems->append(QString::fromLatin1(
                "Etch configuration key list changed (expected 8 fixed keys)"));
            ok = false;
        }
        m_hostMaintenance = false;
        m_targetCommitted = true;
        m_committedDisk = selectedDisk();
        m_committedRoot = selectedRoot();
        updateActionStates();
        if (m_configCombo->count() != static_cast<int>(keys.size())) {
            problems->append(QString::fromLatin1(
                "unprobed target configuration list is incomplete (count=%1)")
                .arg(m_configCombo->count()));
            ok = false;
        }
        if (m_configReasonLabel->text().find(QString::fromLatin1("Run diagnostics")) < 0) {
            problems->append(QString::fromLatin1(
                "unprobed target configuration reason does not ask for diagnostics"));
            ok = false;
        }
        m_targetCommitted = false;
        m_hostMaintenance = true;
        updateActionStates();
    }

    // Log filter: only error/warning lines under the errors filter, and every
    // entry back under All. The probe lines stay in the session log.
    if (m_logFilterCombo && m_logView) {
        m_viewingPriorLog = false;
        m_logFilterCombo->setCurrentItem(0);
        refreshLogView();
        appendLog(QString::fromLatin1("SMOKE-FILTER informational line"));
        appendLog(QString::fromLatin1("ERROR: SMOKE-FILTER error line"));
        m_logFilterCombo->setCurrentItem(1);
        refreshLogView();
        const QString filtered = m_logView->text();
        if (!filtered.contains(QString::fromLatin1("ERROR: SMOKE-FILTER"))
            || filtered.contains(QString::fromLatin1("informational line"))) {
            problems->append(QString::fromLatin1("log errors filter did not select only error lines"));
            ok = false;
        }
        m_logFilterCombo->setCurrentItem(0);
        refreshLogView();
        if (!m_logView->text().contains(QString::fromLatin1("informational line"))) {
            problems->append(QString::fromLatin1("log filter did not restore all entries"));
            ok = false;
        }
    }

    // Log search: matching entries only, restoring every entry when cleared.
    if (m_logSearchEdit && m_logView) {
        m_logSearchEdit->setText(QString::fromLatin1("SMOKE-SEARCH"));
        appendLog(QString::fromLatin1("SMOKE-SEARCH searchable line"));
        if (!m_logView->text().contains(QString::fromLatin1("SMOKE-SEARCH searchable line"))) {
            problems->append(QString::fromLatin1("log search did not show a matching line"));
            ok = false;
        }
        m_logSearchEdit->setText(QString::fromLatin1("NO-SUCH-SMOKE-TEXT"));
        if (m_logView->text().contains(QString::fromLatin1("SMOKE-SEARCH searchable line"))) {
            problems->append(QString::fromLatin1("log search did not hide non-matching lines"));
            ok = false;
        }
        m_logSearchEdit->setText(QString::null);
        if (!m_logView->text().contains(QString::fromLatin1("SMOKE-SEARCH searchable line"))) {
            problems->append(QString::fromLatin1("log search did not restore every entry when cleared"));
            ok = false;
        }
    }

    // Session list: the live session must be listed and selectable.
    ensureLogFile();
    refreshSessionLogList();
    if (!m_sessionLogList || m_sessionLogList->childCount() < 1
        || !m_sessionLogList->firstChild()
        || m_sessionLogList->firstChild()->text(0) != QString::fromLatin1("Current session")) {
        problems->append(QString::fromLatin1(
            "session log list does not lead with the live session (count=%1 first='%2')")
            .arg(m_sessionLogList ? m_sessionLogList->childCount() : -1)
            .arg(m_sessionLogList && m_sessionLogList->firstChild()
                     ? m_sessionLogList->firstChild()->text(0)
                     : QString::fromLatin1("<none>")));
        ok = false;
    }
    return ok;
}

bool LegacyMainWindow::verifyLayout(QString *problems, int *checked)
{
    bool ok = true;
    int checkedCount = 0;
    if (width() < 1024 || height() < 768) {
        problems->append(QString::fromLatin1(
            "window is %1x%2, smaller than the 1024x768 layout contract")
            .arg(width()).arg(height()));
        ok = false;
    }

    const int originalPage = m_tabs->currentPageIndex();
    for (int page = 0; page < m_tabs->count(); ++page) {
        m_tabs->setCurrentPage(page);
        qApp->processEvents();
        QWidget *pageWidget = m_tabs->page(page);

        for (std::size_t i = 0; i < m_groupBoxes.size(); ++i) {
            QGroupBox *box = m_groupBoxes[i];
            if (!box->isVisibleTo(m_tabs)) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(box->font());
            const int needed = metrics.width(box->title())
                + 2 * box->frameWidth() + kGroupTitlePadding;
            if (box->width() < needed) {
                problems->append(QString::fromLatin1(
                    "group title clipped in '%1' (%2px needed, %3px available)")
                    .arg(box->title()).arg(needed).arg(box->width()));
                ok = false;
            }
            QWidget *bound = layoutBoundFor(box, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(box->mapTo(bound, QPoint(0, 0)), box->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "group box '%1' is clipped by the page (at %2,%3 %4x%5, bound %6x%7)")
                    .arg(box->title())
                    .arg(mapped.x()).arg(mapped.y())
                    .arg(mapped.width()).arg(mapped.height())
                    .arg(boundRect.width()).arg(boundRect.height()));
                ok = false;
            }
            // Regression guard for the Qt3 KDE/Etch group-title clipping: a
            // non-empty QGroupBox title is drawn on the upper frame line with
            // its top half above the frame, so any future titled frame needs
            // at least half a line of clearance inside its bound. The legacy
            // frames use a sectionTitle label instead and have empty titles.
            if (!box->title().isEmpty()) {
                const int clearance = metrics.height() / 2 + 2;
                if (mapped.y() < clearance) {
                    problems->append(QString::fromLatin1(
                        "group title '%1' has no top clearance (%2px, needs %3px)")
                        .arg(box->title()).arg(mapped.y()).arg(clearance));
                    ok = false;
                }
            }
        }

        for (std::size_t i = 0; i < m_buttons.size(); ++i) {
            QPushButton *button = m_buttons[i];
            if (!button->isVisibleTo(m_tabs)) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(button->font());
            const int needed = metrics.width(button->text()) + 20;
            if (button->width() < needed) {
                problems->append(QString::fromLatin1(
                    "button text clipped in '%1' (%2px needed, %3px available)")
                    .arg(button->text()).arg(needed).arg(button->width()));
                ok = false;
            }
            QWidget *bound = layoutBoundFor(button, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(button->mapTo(bound, QPoint(0, 0)), button->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "button '%1' is clipped by the page (at %2,%3 %4x%5, bound %6x%7)")
                    .arg(button->text())
                    .arg(mapped.x()).arg(mapped.y())
                    .arg(mapped.width()).arg(mapped.height())
                    .arg(boundRect.width()).arg(boundRect.height()));
                ok = false;
            }
        }

        if (m_deviceList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_deviceList, QString::fromLatin1("device list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_deviceList, QString::fromLatin1("device list"), problems)) {
                ok = false;
            }
        }
        if (m_detailList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_detailList, QString::fromLatin1("details list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_detailList, QString::fromLatin1("details list"), problems)) {
                ok = false;
            }
        }
        if (m_diagnosticList && m_diagnosticList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_diagnosticList, QString::fromLatin1("diagnostic list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_diagnosticList, QString::fromLatin1("diagnostic list"), problems)) {
                ok = false;
            }
        }
        if (m_planStageList && m_planStageList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_planStageList, QString::fromLatin1("plan stage list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_planStageList, QString::fromLatin1("plan stage list"), problems)) {
                ok = false;
            }
        }
        if (m_toolList && m_toolList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_toolList, QString::fromLatin1("tools list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_toolList, QString::fromLatin1("tools list"), problems)) {
                ok = false;
            }
        }
        if (m_sessionLogList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_sessionLogList, QString::fromLatin1("session log list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_sessionLogList, QString::fromLatin1("session log list"), problems)) {
                ok = false;
            }
        }
        if (m_logSearchEdit && m_logSearchEdit->isVisibleTo(m_tabs)) {
            ++checkedCount;
            QFontMetrics metrics(m_logSearchEdit->font());
            if (m_logSearchEdit->width() < metrics.width(m_logSearchEdit->text()) + 24) {
                problems->append(QString::fromLatin1("log search field is too narrow"));
                ok = false;
            }
        }
        if (m_configCombo && m_configCombo->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!comboTextFits(m_configCombo, QString::fromLatin1("configuration combo"), problems)) {
                ok = false;
            }
        }

        // Section titles on THIS page (modern sectionTitle parity): every
        // tab's titles are checked, not only the active one, so a clip on an
        // unvisited page cannot pass the smoke. Wrapping titles
        // (Qt::WordBreak) are exempt from the single-line width check: they
        // break onto a second line instead of clipping, and the containment
        // check below still applies.
        for (std::size_t i = 0; i < m_sectionTitles.size(); ++i) {
            QLabel *title = m_sectionTitles[i];
            if (!title) {
                continue;
            }
            QWidget *titlePage = title->parentWidget();
            while (titlePage && titlePage != m_tabs
                   && m_tabs->indexOf(titlePage) < 0) {
                titlePage = titlePage->parentWidget();
            }
            if (titlePage != pageWidget) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(title->font());
            const bool wraps = (title->alignment() & Qt::WordBreak) != 0;
            if (!wraps && title->width() < metrics.width(title->text()) + 8) {
                problems->append(QString::fromLatin1(
                    "section title clipped: '%1' (%2px needed, %3px available)")
                    .arg(title->text()).arg(metrics.width(title->text()) + 8)
                    .arg(title->width()));
                ok = false;
            }
            QWidget *bound = layoutBoundFor(title, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(title->mapTo(bound, QPoint(0, 0)), title->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "section title '%1' is clipped by its page").arg(title->text()));
                ok = false;
            }
        }
    }
    m_tabs->setCurrentPage(originalPage);
    // Global header: the title, subtitle and badge must fit the 1024x768
    // contract without clipping.
    if (m_headerTitle && m_headerTitle->isVisibleTo(this)) {
        ++checkedCount;
        QFontMetrics metrics(m_headerTitle->font());
        if (m_headerTitle->width() < metrics.width(m_headerTitle->text()) + 4) {
            problems->append(QString::fromLatin1("global header title is clipped"));
            ok = false;
        }
    }
    if (m_headerBadge && m_headerBadge->isVisibleTo(this)) {
        ++checkedCount;
        QFontMetrics metrics(m_headerBadge->font());
        if (m_headerBadge->width() < metrics.width(m_headerBadge->text()) + 12) {
            problems->append(QString::fromLatin1(
                "guarded-repair badge is clipped (%1px needed, %2px available)")
                .arg(metrics.width(m_headerBadge->text()) + 12)
                .arg(m_headerBadge->width()));
            ok = false;
        }
    }
    if (checked) {
        *checked = checkedCount;
    }
    return ok;
}

// Rebuilds the target configuration row from the helper's read-only
// `Legacy config <key>:` probe. The row is only visible for a committed
// repair target (never Host Maintenance); files the probe reports absent are
// omitted from the combo and their exact reason is shown below it, while an
// unprobed identity lists every Etch key so the guarded read can decide.
void LegacyMainWindow::updateConfigView()
{
    const bool show = targetCommitted() && !hostMaintenanceActive();
    // Qt3 QWidget has no setVisible(bool); show()/hide() are the Qt3 API.
    if (m_configLabel) {
        if (show) { m_configLabel->show(); } else { m_configLabel->hide(); }
    }
    if (m_configCombo) {
        if (show) { m_configCombo->show(); } else { m_configCombo->hide(); }
    }
    if (m_configButton) {
        if (show) { m_configButton->show(); } else { m_configButton->hide(); }
    }
    if (m_configReasonLabel) {
        if (show) { m_configReasonLabel->show(); } else { m_configReasonLabel->hide(); }
    }
    if (!show || !m_configCombo) {
        return;
    }

    const QString previousPath = m_configCombo->currentText().stripWhiteSpace();
    const bool probed = m_model.hasDiagnostics(toStd(identity()))
        && !m_model.diagnosticsStale();
    QStringList absent;
    m_configCombo->clear();
    for (int i = 0; i < configSpecCount; ++i) {
        const ConfigSpec &spec = configSpecs[i];
        const std::string state = probed
            ? m_model.configFileState(spec.key) : std::string();
        if (state == "available") {
            m_configCombo->insertItem(QString::fromLatin1(spec.path));
        } else if (state.size() >= 12 && state.compare(0, 12, "unavailable|") == 0) {
            absent.append(QString::fromLatin1("%1 - %2")
                              .arg(QString::fromLatin1(spec.path))
                              .arg(fromStd(state.substr(12))));
        } else {
            // Not probed yet (or an unrecognised state): list the file and let
            // the helper's guarded read decide (fail closed at the helper).
            m_configCombo->insertItem(QString::fromLatin1(spec.path));
        }
    }
    if (!previousPath.isEmpty()) {
        for (int i = 0; i < m_configCombo->count(); ++i) {
            if (m_configCombo->text(i) == previousPath) {
                m_configCombo->setCurrentItem(i);
                break;
            }
        }
    }
    if (m_configReasonLabel) {
        if (m_configCombo->count() == 0) {
            m_configReasonLabel->setText(
                QString::fromLatin1(
                    "The read-only probe found no editable target configuration "
                    "file in this target. %1")
                    .arg(absent.isEmpty() ? QString::fromLatin1("")
                                          : absent.join(QString::fromLatin1("; "))));
        } else if (!absent.isEmpty()) {
            m_configReasonLabel->setText(
                QString::fromLatin1("Not present in the selected target (omitted): %1")
                    .arg(absent.join(QString::fromLatin1("; "))));
        } else if (!probed) {
            m_configReasonLabel->setText(QString::fromLatin1(
                "Run diagnostics to probe which target configuration files "
                "exist; the helper's read-only probe decides the list."));
        } else {
            m_configReasonLabel->setText(QString::fromLatin1(
                "Probed read-only by the helper; a saved edit invalidates the "
                "cached diagnostics."));
        }
    }
}

// Fill the state/reason pair for a legacy feature row. Fail closed: when no
// diagnostics for the current identity are cached, or the helper emitted no
// `Legacy feature <feature>:` line (or an unrecognised state), the state reads
// "not reported"/"unrecognised" and the reason is the model's fail-closed
// explanation.
void LegacyMainWindow::legacyFeatureDisplay(const char *feature, QString *state,
                                            QString *reason) const
{
    const QString id = identity();
    std::string modelReason;
    const bool available =
        m_model.legacyFeatureAvailable(feature, toStd(id), &modelReason);
    const bool haveDiagnostics =
        m_model.hasDiagnostics(toStd(id)) && !m_model.diagnosticsStale();
    const std::string raw =
        haveDiagnostics ? m_model.legacyFeatureState(feature) : std::string();
    QString stateText;
    if (available) {
        stateText = QString::fromLatin1("available");
    } else if (raw.empty()) {
        stateText = QString::fromLatin1("not reported");
    } else if (raw.size() >= 12 && raw.compare(0, 12, "unavailable|") == 0) {
        stateText = QString::fromLatin1("unavailable");
    } else {
        stateText = QString::fromLatin1("unrecognised");
    }
    if (state) {
        *state = stateText;
    }
    if (reason) {
        *reason = fromStd(modelReason);
    }
}

void LegacyMainWindow::updateFeatureTab(QLabel *label, const char *feature)
{
    if (!label) {
        return;
    }
    QString state;
    QString reason;
    legacyFeatureDisplay(feature, &state, &reason);
    QString text;
    if (state == QString::fromLatin1("available")) {
        text = QString::fromLatin1(
                   "Legacy feature %1: available. The helper command exists, but "
                   "this legacy frontend keeps the workflow greyed and never "
                   "weakens the helper's runtime preflights.")
                   .arg(QString::fromLatin1(feature));
    } else {
        text = QString::fromLatin1("Legacy feature %1: %2\n%3")
                   .arg(QString::fromLatin1(feature))
                   .arg(state == QString::fromLatin1("not reported")
                            ? QString::fromLatin1("no probe line cached")
                            : state)
                   .arg(reason);
    }
    label->setText(text);
}

// Modern updateChrootShellMode() parity: the heading, tab label, notice,
// command button and warning follow the active scope (Host Shell while Host
// Maintenance is active, Chroot Shell for a committed offline target).
void LegacyMainWindow::updateChrootShellMode()
{
    const bool host = hostScope();
    if (m_chrootHeading) {
        m_chrootHeading->setText(host
            ? QString::fromLatin1("Host shell")
            : QString::fromLatin1("Chroot shell"));
    }
    if (m_chrootScopeLabel) {
        m_chrootScopeLabel->setText(host
            ? QString::fromLatin1("Host maintenance: %1")
                  .arg(selectedDisk().isEmpty() ? QString::fromLatin1("unresolved")
                                                : selectedDisk())
            : (targetCommitted()
                ? QString::fromLatin1("Target: %1 + %2")
                      .arg(m_committedDisk).arg(m_committedRoot)
                : QString::fromLatin1("Target: none selected")));
    }
    if (m_shellRunButton) {
        updateButtonText(m_shellRunButton, host
            ? QString::fromLatin1("Run on Host")
            : QString::fromLatin1("Run Command"));
    }
    if (m_chrootCommandHeading) {
        m_chrootCommandHeading->setText(host
            ? QString::fromLatin1("Host command")
            : QString::fromLatin1("Command"));
    }
    if (m_tabs && m_chrootTab) {
        m_tabs->setTabLabel(m_chrootTab,
                            host ? QString::fromLatin1("Host Shell")
                                 : QString::fromLatin1("Chroot Shell"));
        m_tabs->setTabToolTip(m_chrootTab, host
            ? QString::fromLatin1("Execute a command on the running host as root.")
            : QString::fromLatin1("Execute a command inside the selected repair system as root."));
    }
}

// Chroot Shell follows the active scope's own helper probe (`host-shell` for
// Host Maintenance, `shell` for a committed repair target) plus the cached
// administrator session. When any prerequisite is missing every control stays
// greyed with the exact reason; the helper keeps its runtime preflights.
void LegacyMainWindow::updateChrootShellState()
{
    if (!m_chrootGroup || !m_chrootReasonLabel) {
        return;
    }
    updateChrootShellMode();
    const bool host = hostScope();
    const char *feature = host ? "host-shell" : "shell";
    QString state;
    QString probeReason;
    legacyFeatureDisplay(feature, &state, &probeReason);

    QString reason;
    bool enabled = false;
    if (m_running) {
        reason = QString::fromLatin1("A helper command is already running.");
    } else if (!diagnosticsScopeReady()) {
        reason = scopeReadyReason();
    } else if (!administratorSessionActive()) {
        reason = QString::fromLatin1(
            "Administrator authorization is required; press Authorize on the "
            "Systems or Repair tab (or re-enter Host Maintenance / re-commit the "
            "repair target) to authorize this session.");
    } else {
        std::string featureReason;
        if (!m_model.legacyFeatureAvailable(feature, toStd(identity()),
                                            &featureReason)) {
            reason = fromStd(featureReason);
        } else {
            enabled = true;
            reason = host
                ? QString::fromLatin1(
                      "Run one reviewed command as root on the running host "
                      "through the helper's guarded host-shell verb.")
                : QString::fromLatin1(
                      "Run one reviewed command as root inside the target "
                      "chroot through the helper's guarded shell verb.");
        }
    }

    QString text;
    if (enabled) {
        text = QString::fromLatin1(
            "Legacy feature %1: available. The helper command is wired and the "
            "session is authorized; the helper keeps its runtime preflights.")
            .arg(QString::fromLatin1(feature));
    } else {
        text = QString::fromLatin1("Legacy feature %1: %2\n%3")
            .arg(QString::fromLatin1(feature))
            .arg(state == QString::fromLatin1("not reported")
                     ? QString::fromLatin1("no probe line cached")
                     : state)
            .arg(reason);
    }
    m_chrootReasonLabel->setText(text);

    if (m_shellCommandEdit) {
        m_shellCommandEdit->setEnabled(enabled);
    }
    if (m_shellRunButton) {
        m_shellRunButton->setEnabled(enabled);
        QToolTip::add(m_shellRunButton, reason);
    }
    if (m_shellClearButton) {
        m_shellClearButton->setEnabled(enabled
            || (m_shellOutput && !m_shellOutput->text().isEmpty()));
    }
    if (m_shellOutput) {
        m_shellOutput->setEnabled(enabled);
    }
}

void LegacyMainWindow::updateLegacyFeatureView()
{
    updateChrootShellState();
    updateFeatureTab(m_fileCopyReasonLabel, "file-copy");
}

void LegacyMainWindow::updateFactView(const ParsedTranscript &parsed)
{
    m_factMap.clear();
    for (std::size_t i = 0; i < parsed.targetFacts.size(); ++i) {
        const std::string &line = parsed.targetFacts[i];
        const std::string::size_type colon = line.find(": ");
        if (colon == std::string::npos) {
            continue;
        }
        m_factMap.insert(fromStd(line.substr(0, colon + 1)),
                         fromStd(line.substr(colon + 2)));
    }
    m_helperDisk = mapValue(m_factMap, QString::fromLatin1("Physical drive:"));
    m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Detected component:"));
    if (m_helperComponent.isEmpty()) {
        m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Running host root mount source:"));
    }
    if (m_helperComponent.isEmpty()) {
        m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Target root mount:"));
    }
    if (m_helperComponent.isEmpty()) {
        m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Root mount source:"));
    }
    updateDriveDetails();
    updateUnlockStatus();
    updateCapabilityView();
}

void LegacyMainWindow::updateDriveDetails()
{
    if (!m_detailList) {
        return;
    }
    m_detailList->clear();
    const QString disk = selectedDisk();
    QMap<QString, DeviceRow>::const_iterator row = m_rows.find(disk);
    const DeviceRow *selected = row == m_rows.end() ? 0 : &row.data();
    const bool helperConfirmed = !disk.isEmpty() && m_helperDisk == disk;

    // Modern parity: the inspected row (a child component when one is
    // selected, otherwise the drive) supplies the details; the Drive field
    // always names the top-level drive.
    QString inspectedPath = selectedComponent();
    if (inspectedPath.isEmpty()) {
        inspectedPath = disk;
    }
    QMap<QString, DeviceRow>::const_iterator inspectedRow = m_rows.find(inspectedPath);
    const DeviceRow *inspected = inspectedRow == m_rows.end() ? selected : &inspectedRow.data();
    const bool inspectingChild = !selectedComponent().isEmpty()
        && selectedComponent() != disk;

    QString model;
    QString size;
    QString transport;
    QString fstype;
    QString uuid;
    QString mounts;
    if (inspected) {
        model = fromStd(inspected->model);
        if (model.isEmpty()) {
            model = fromStd(inspected->label);
        } else if (!inspected->label.empty()) {
            model += QString::fromLatin1(" (") + fromStd(inspected->label) + QString::fromLatin1(")");
        }
        size = fromStd(inspected->size);
        transport = fromStd(inspected->transport);
        fstype = displayFsType(*inspected);
        uuid = fromStd(inspected->uuid);
        mounts = fromStd(inspected->mountpoint);
    }
    if (fstype.isEmpty() && helperConfirmed) {
        fstype = mapValue(m_factMap, QString::fromLatin1("Filesystem:"));
    }

    QString component = selectedComponent();
    if (component.isEmpty()) {
        component = autoResolvedRoot(disk);
    }
    if (component.isEmpty() && helperConfirmed) {
        component = m_helperComponent;
    }
    QString status;
    if (disk.isEmpty()) {
        status = QString::fromLatin1("Select a drive to see its details.");
    } else if (inspectingChild) {
        status = QString::fromLatin1("Inspecting the selected component.");
    } else if (helperConfirmed) {
        status = QString::fromLatin1("Helper-confirmed by the last read-only diagnostics.");
    } else {
        status = QString::fromLatin1("Read-only inventory only; run diagnostics to confirm.");
    }
    QString protection;
    if (disk.isEmpty()) {
        protection = QString::fromLatin1("-");
    } else if (!runningHostDisk().isEmpty() && disk == runningHostDisk()) {
        protection = QString::fromLatin1("PROTECTED - running system; read-only details only");
    } else if (selected && selected->optical) {
        protection = QString::fromLatin1("Live / installer media - not selectable");
    } else if (hostScope()) {
        protection = QString::fromLatin1("PROTECTED - running host scope");
    } else if (!autoResolvedLuks(disk).isEmpty() && component.isEmpty()) {
        protection = QString::fromLatin1("Unlock required before selection");
    } else {
        protection = QString::fromLatin1("Eligible repair candidate");
    }

    // Qt3 prepends a plain insertion when sorting is disabled; chain the rows
    // with the after-form constructor to keep the documented field order
    // (Drive, Detected target, Model/label, ...).
    QListViewItem *lastDetail = 0;
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Drive:"), detailOrDash(disk));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Detected target:"),
                                   component.isEmpty() ? QString::fromLatin1("Pending inspection")
                                                       : component);
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Model / label:"), detailOrDash(model));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Status:"), status);
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Size:"), detailOrDash(size));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Connection:"), detailOrDash(transport));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Filesystem:"), detailOrDash(fstype));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("UUID:"), detailOrDash(uuid));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Mounts:"), detailOrDash(mounts));
    lastDetail = new QListViewItem(m_detailList, lastDetail,
                                   QString::fromLatin1("Protection:"), protection);
}

void LegacyMainWindow::updateUnlockStatus()
{
    if (!m_unlockStatusView) {
        return;
    }
    const QString disk = selectedDisk();
    if (disk.isEmpty()) {
        m_unlockStatusView->setText(QString::fromLatin1("Select a drive to see unlock status."));
        return;
    }
    const QString component = unlockCandidateFor(disk);
    if (hostScope() || (!runningHostDisk().isEmpty() && disk == runningHostDisk())) {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: protected\n"
            "Component: %1\n"
            "The protected running host cannot be unlocked or modified; unlock "
            "is available for an offline repair target only. Use Host "
            "Maintenance for the protected running host.")
            .arg(component.isEmpty() ? QString::fromLatin1("(none detected)")
                                     : component));
        return;
    }
    const QMap<QString, QString>::const_iterator cached = m_unlockStatusCache.find(disk);
    if (cached != m_unlockStatusCache.end() && !cached.data().stripWhiteSpace().isEmpty()) {
        m_unlockStatusView->setText(cached.data());
        return;
    }
    const QString mapper = visibleMapperForDisk(disk);
    const bool helperUnlocked = !m_helperComponent.isEmpty()
        && m_helperComponent.startsWith(QString::fromLatin1("/dev/mapper/"))
        && !m_helperDisk.isEmpty() && m_helperDisk == disk;
    if (!component.isEmpty()) {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: locked\n"
            "Component: %1\n"
            "Method: helper unlock (cryptsetup; passphrase on stdin only)\n"
            "A locked LUKS container is visible on this drive; press Unlock to "
            "open it for this recovery session.")
            .arg(component));
    } else if (!mapper.isEmpty()) {
        const QString shownComponent = selectedComponent().isEmpty()
            ? (m_helperComponent.isEmpty() ? QString::fromLatin1("(visible mapper)")
                                           : m_helperComponent)
            : selectedComponent();
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: unlocked\n"
            "Component: %1\n"
            "Mapper: %2\n"
            "Method: already open before this session (visible mapper; Boot "
            "Bitch will reuse it and will not close it).")
            .arg(shownComponent)
            .arg(mapper));
    } else if (helperUnlocked) {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: unlocked\n"
            "Component: %1\n"
            "Mapper: %2\n"
            "Method: helper-confirmed mapping from the last read-only diagnostics.")
            .arg(m_helperComponent.startsWith(QString::fromLatin1("/dev/mapper/"))
                     ? QString::fromLatin1("(mapper)")
                     : m_helperComponent)
            .arg(m_helperComponent));
    } else {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: locked or no encrypted component detected\n"
            "Component: (none detected)\n"
            "Method: helper unlock (cryptsetup; passphrase on stdin only)\n"
            "No locked LUKS component and no unlock operation were recorded for "
            "this drive in the current session."));
    }
}

QString LegacyMainWindow::visibleMapperForDisk(const QString &disk) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    const QString kernel = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!row.mapper || row.parent.empty()) {
            continue;
        }
        const QString parent = fromStd(row.parent);
        if (parent == kernel) {
            return it.key();
        }
        if (parent.startsWith(kernel) && parent.length() > kernel.length()
            && parent[kernel.length()] >= QChar('0')
            && parent[kernel.length()] <= QChar('9')) {
            return it.key();
        }
    }
    return QString::null;
}

void LegacyMainWindow::mergeHelperDevices(const ParsedTranscript &parsed)
{
    for (std::size_t i = 0; i < parsed.devicePaths.size(); ++i) {
        const QString path = fromStd(parsed.devicePaths[i]);
        if (m_rows.contains(path)) {
            continue;
        }
        DeviceRow row;
        row.name = toStd(path);
        row.path = toStd(path);
        row.mapper = path.startsWith(QString::fromLatin1("/dev/mapper/"));
        m_rows.insert(path, row);
        // Keep the helper-reported row in the cached inventory too, so a
        // device-discovery refilter cannot drop it from the list.
        m_inventory.push_back(row);
        new QListViewItem(m_deviceList, path, QString::fromLatin1("?"),
                          QString::fromLatin1("helper-reported"),
                          QString::fromLatin1(""), QString::fromLatin1(""),
                          QString::fromLatin1("referenced by helper diagnostics"));
    }
}

void LegacyMainWindow::updateElevationLabel()
{
    QString description;
    const bool ok = m_runner->resolveElevation(&description);
    m_lastHelperDescription = description;
    if (m_elevationLabel) {
        if (ok) {
            m_elevationLabel->setText(QString::fromLatin1("Elevation: %1 | helper: %2")
                                          .arg(description)
                                          .arg(m_runner->helperPath()));
        } else {
            m_elevationLabel->setText(QString::fromLatin1("Elevation unavailable: %1")
                                          .arg(description));
        }
        QToolTip::add(m_elevationLabel, ok
            ? QString::fromLatin1("The helper runs through this elevation method; "
                                  "a plain sudo asks for its password in a modal "
                                  "hidden-input dialog before each command.")
            : QString::fromLatin1("No elevation method is available. Install gksu, "
                                  "gksudo or sudo, or start the GUI as root."));
    }
    if (m_settingsHelperLabel) {
        m_settingsHelperLabel->setText(m_runner->helperPath());
    }
    if (m_settingsElevationLabel) {
        m_settingsElevationLabel->setText(ok
            ? description
            : QString::fromLatin1("unavailable - %1").arg(description));
    }
    if (m_settingsLogDirLabel) {
        m_settingsLogDirLabel->setText(m_logDirectory);
    }
}

// Modern scope label parity: the Diagnostics/Repair/Shell pages show the
// committed target, the protected running host while Host Maintenance is
// active, or "Target: none selected".
void LegacyMainWindow::updateScopeLabel()
{
    QString text;
    if (hostMaintenanceActive()) {
        text = QString::fromLatin1("Host maintenance: %1")
                   .arg(selectedDisk().isEmpty() ? QString::fromLatin1("unresolved")
                                                 : selectedDisk());
    } else if (targetCommitted()) {
        text = QString::fromLatin1("Target: %1 + %2")
                   .arg(m_committedDisk).arg(m_committedRoot);
    } else {
        text = QString::fromLatin1("Target: none selected");
    }
    if (m_scopeLabel) {
        m_scopeLabel->setText(text);
    }
    if (m_repairScopeLabel) {
        m_repairScopeLabel->setText(text);
    }
    if (m_chrootScopeLabel) {
        m_chrootScopeLabel->setText(text);
    }
}

// Deferred-authorization affordance (modern m_authorizationStatusLabel /
// Authorize): the status text and the button state on the Systems and Repair
// pages. The modal hidden-input dialog is opened by authorizeNow().
void LegacyMainWindow::updateAuthorizationAffordance()
{
    const bool ready = diagnosticsScopeReady();
    const bool active = administratorSessionActive();
    const bool idle = !m_running;

    QString text;
    bool canAuthorize = false;
    if (!ready) {
        text = QString::fromLatin1(
            "Authorization: select a repair target or Host Maintenance first.");
    } else if (active) {
        QString description;
        m_runner->resolveElevation(&description);
        if (description.find(QString::fromLatin1("gksu")) >= 0
            || description.find(QString::fromLatin1("gksudo")) >= 0) {
            text = QString::fromLatin1("Authorization: %1 provides it per command.")
                       .arg(description);
        } else {
            text = QString::fromLatin1("Authorization: administrator session active (%1).")
                       .arg(description);
        }
    } else if (!idle) {
        text = QString::fromLatin1("Authorization: a helper command is running.");
    } else {
        text = QString::fromLatin1(
            "Authorization required: diagnostics and repairs fail closed until "
            "you press Authorize.");
        canAuthorize = true;
    }
    if (m_authStatusLabel) {
        m_authStatusLabel->setText(text);
    }
    if (m_authorizeButton) {
        m_authorizeButton->setEnabled(canAuthorize);
        QToolTip::add(m_authorizeButton, canAuthorize
            ? QString::fromLatin1(
                  "Re-establish the privileged helper session for the current "
                  "scope now. The password is requested in the hidden-input "
                  "modal and is never logged.")
            : text);
    }
    if (m_repairAuthStatusLabel) {
        m_repairAuthStatusLabel->setText(text);
    }
    if (m_repairAuthorizeButton) {
        m_repairAuthorizeButton->setEnabled(canAuthorize);
        QToolTip::add(m_repairAuthorizeButton, canAuthorize
            ? QString::fromLatin1(
                  "Re-establish the privileged helper session without leaving "
                  "the Repair tab. The password is requested in the "
                  "hidden-input modal and is never logged.")
            : text);
    }
}

// Selected-diagnostic pane: modern title/description/availability text plus
// the list tooltip. The list shows the friendly title; the stable helper key
// is mapped back internally for the command and the description.
void LegacyMainWindow::updateDiagnosticDetails()
{
    if (!m_diagTitle || !m_diagDescription || !m_diagAvailability) {
        return;
    }
    QListViewItem *item = m_diagnosticList ? m_diagnosticList->currentItem() : 0;
    if (!item) {
        m_diagTitle->setText(QString::fromLatin1("Select a diagnostic"));
        m_diagDescription->setText(QString::fromLatin1("Choose a diagnostic from the list."));
        m_diagAvailability->setText(QString::fromLatin1("Unavailable"));
        return;
    }
    const QString key = mapValue(m_diagKeyByTitle,
                                 item->text(0).stripWhiteSpace());
    const QString description = diagnosticDescription(key);
    m_diagTitle->setText(key.isEmpty() ? item->text(0) : diagnosticTitle(key));
    m_diagDescription->setText(description);
    QToolTip::add(m_diagnosticList, description);
    if (m_running) {
        m_diagAvailability->setText(QString::fromLatin1("Running..."));
    } else if (!diagnosticsScopeReady()) {
        m_diagAvailability->setText(QString::fromLatin1("Scope required"));
    } else {
        m_diagAvailability->setText(QString::fromLatin1("Ready"));
    }
}

void LegacyMainWindow::updateActionStates()
{
    const bool complete = selectionComplete();
    const bool idle = !m_running;
    const bool ready = diagnosticsScopeReady();
    const QString id = identity();

    // The individual tools list and the Full Repair plan follow the same
    // fail-closed capability/feature probes; updateToolDetails() and
    // updatePlanView() recompute their buttons and reasons.
    updateToolDetails();
    updatePlanView();

    const bool diagnosticsEnabled = complete && idle && ready;
    if (m_diagnosticsButton) {
        m_diagnosticsButton->setEnabled(diagnosticsEnabled);
        QToolTip::add(m_diagnosticsButton,
                      diagnosticsEnabled
                          ? QString::fromLatin1("Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.")
                          : (ready ? QString::fromLatin1("Select a target and wait for any running command first.")
                                   : scopeReadyReason()));
    }
    if (m_runDiagnosticButton) {
        const bool hasCheck = m_diagnosticList && m_diagnosticList->currentItem() != 0;
        m_runDiagnosticButton->setEnabled(diagnosticsEnabled && hasCheck);
        QToolTip::add(m_runDiagnosticButton, !hasCheck
            ? QString::fromLatin1("Select a diagnostic check in the list first.")
            : (diagnosticsEnabled
                ? QString::fromLatin1("Run Diagnostic - run the selected read-only diagnostic through the helper.")
                : (ready ? QString::fromLatin1("Select a target and wait for any running command first.")
                         : scopeReadyReason())));
    }
    if (m_refreshCapabilitiesButton) {
        // Modern Refresh Capabilities: run the current scope's diagnostics and
        // rebuild the read-only backend profile.
        m_refreshCapabilitiesButton->setEnabled(diagnosticsEnabled);
        QToolTip::add(m_refreshCapabilitiesButton, diagnosticsEnabled
            ? QString::fromLatin1("Run All for the current scope and refresh the read-only backend profile.")
            : (ready ? QString::fromLatin1("Select a target and wait for any running command first.")
                     : scopeReadyReason()));
    }
    if (m_configButton) {
        // Modern parity: Edit Target File is target-only (never Host
        // Maintenance) and needs a probed, selectable file in the combo.
        const bool configShown = targetCommitted() && !hostMaintenanceActive();
        const bool configEnabled = configShown && idle
            && m_configCombo && m_configCombo->count() > 0;
        m_configButton->setEnabled(configEnabled);
        QToolTip::add(m_configButton, configEnabled
            ? QString::fromLatin1(
                  "Read or edit the selected target configuration file through "
                  "the guarded helper; a saved edit invalidates cached diagnostics.")
            : (hostMaintenanceActive()
                ? QString::fromLatin1(
                      "Host Maintenance has no target-file editing; commit an "
                      "offline repair target first.")
                : (!targetCommitted()
                    ? QString::fromLatin1("Commit an offline repair target first.")
                    : (!idle
                        ? QString::fromLatin1("A helper command is already running.")
                        : QString::fromLatin1(
                              "No target configuration file is available for this "
                              "target; run diagnostics to probe the list.")))));
    }
    if (m_scanButton) {
        m_scanButton->setEnabled(idle);
    }

    // Unlock mirrors the modern GUI: it is offered only for an offline target
    // (never the protected running host, whether or not Host Maintenance is
    // active) and only when a locked LUKS component is visible on the selected
    // drive; the helper's own cryptsetup preflights verify it at runtime.
    if (m_unlockButton) {
        const bool onRunningHost = !runningHostDisk().isEmpty()
            && selectedDisk() == runningHostDisk();
        const QString luks = onRunningHost ? QString::null
                                           : unlockCandidateFor(selectedDisk());
        const bool unlockEnabled = idle && !hostScope() && !onRunningHost
            && !selectedDisk().isEmpty() && !luks.isEmpty();
        m_unlockButton->setEnabled(unlockEnabled);
        if (hostScope() || onRunningHost) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "The protected running host cannot be unlocked; use Host "
                "Maintenance for the protected running host."));
        } else if (selectedDisk().isEmpty()) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "Select a physical drive in the Available repair targets list first."));
        } else if (!idle) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "A helper command is already running."));
        } else if (luks.isEmpty()) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "No locked LUKS component is currently visible on this selected "
                "drive."));
        } else {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "Unlock %1 using cryptsetup through the privileged helper. The "
                "passphrase is sent on standard input and is never placed in "
                "command arguments or logs.").arg(luks));
        }
    }
    if (m_elevateButton) {
        m_elevateButton->setEnabled(idle);
    }
    if (m_setTargetButton) {
        const bool onRunningHost = !runningHostDisk().isEmpty()
            && selectedDisk() == runningHostDisk();
        QMap<QString, DeviceRow>::const_iterator selectedRow =
            m_rows.find(selectedDisk());
        const bool optical = selectedRow != m_rows.end()
            && selectedRow.data().optical;
        const QString root = autoResolvedRoot(selectedDisk());
        const bool canCommit = idle && !selectedDisk().isEmpty() && !onRunningHost
            && !optical && !root.isEmpty();
        m_setTargetButton->setEnabled(canCommit);
        if (selectedDisk().isEmpty()) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Select a physical drive in the Available repair targets list first."));
        } else if (onRunningHost) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "The protected running system cannot be selected as a repair "
                "target; use Host Maintenance for the protected running host."));
        } else if (optical) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Live / installer media is read-only boot media and cannot be "
                "selected as a repair target."));
        } else if (root.isEmpty()) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Unlock the encrypted volume first; Select Target becomes "
                "available after a Linux filesystem is detected."));
        } else if (!idle) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "A helper command is already running."));
        } else {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Commit %1 + %2 as the repair target for diagnostics and gated "
                "repairs; the administrator authorization is requested here once "
                "and cached for the session.").arg(selectedDisk()).arg(root));
        }
    }
    if (m_hostMaintenanceButton) {
        const bool canMaintain = idle && !runningHostDisk().isEmpty();
        m_hostMaintenanceButton->setEnabled(canMaintain);
        QToolTip::add(m_hostMaintenanceButton, canMaintain
            ? (m_hostMaintenance
                ? QString::fromLatin1("Leave Host Maintenance and return to repair-target mode.")
                : QString::fromLatin1("Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session."))
            : QString::fromLatin1("The running host target could not be detected; diagnostics need a repair target."));
    }
    if (m_targetSummary) {
        if (hostMaintenanceActive()) {
            m_targetSummary->setText(QString::fromLatin1("Host maintenance: %1")
                                         .arg(selectedDisk().isEmpty()
                                                  ? QString::fromLatin1("unresolved")
                                                  : selectedDisk()));
        } else if (targetCommitted()) {
            m_targetSummary->setText(QString::fromLatin1("Committed target: %1 + %2")
                                         .arg(m_committedDisk).arg(m_committedRoot));
        } else if (m_targetCommitted && !hostScope()) {
            m_targetSummary->setText(QString::fromLatin1("Committed target: none (selection changed)"));
        } else {
            m_targetSummary->setText(QString::fromLatin1("Committed target: none"));
        }
    }

    if (m_gateHint) {
        if (!ready) {
            m_gateHint->setText(scopeReadyReason());
        } else if (!complete) {
            m_gateHint->setText(QString::fromLatin1(
                "The selected scope has no resolved Linux root component; "
                "Refresh Devices and commit the repair target again."));
        } else if (!administratorSessionActive()) {
            m_gateHint->setText(QString::fromLatin1(
                "No administrator session is active; press Authorize on the "
                "Systems or Repair tab to re-establish it. Run All reuses the "
                "cached authorization and never prompts by itself."));
        } else if (!m_model.hasDiagnostics(toStd(id))) {
            m_gateHint->setText(QString::fromLatin1(
                "Run diagnostics for this scope to unlock the gated actions. "
                "Diagnostics are read-only and the only evidence source."));
        } else if (m_model.diagnosticsStale()) {
            m_gateHint->setText(QString::fromLatin1(
                "A repair was not proven unchanged, so the cached diagnostics are invalidated. "
                "Run diagnostics again before another gated action."));
        } else {
            m_gateHint->setText(QString::fromLatin1(
                "Gated actions reflect the cached capability lines; the helper still "
                "runs every runtime preflight when a command starts."));
        }
    }

    updateScopeLabel();
    updateAuthorizationAffordance();
    updateDiagnosticDetails();
    updateCapabilityView();
    updateConfigView();
    updateLegacyFeatureView();
}

// The containment bound for a widget: the scroll content widget when the
// widget lives inside a scrollable page (Settings/Repair use a Qt3
// QScrollView), otherwise the tab page itself.
QWidget *LegacyMainWindow::layoutBoundFor(QWidget *widget, QWidget *pageWidget) const
{
    if (m_settingsContent && isInsideWidget(widget, m_settingsContent)) {
        return m_settingsContent;
    }
    if (m_repairContent && isInsideWidget(widget, m_repairContent)) {
        return m_repairContent;
    }
    return pageWidget;
}

void LegacyMainWindow::updateStatus()
{
    QString scope = hostScope()
        ? QString::fromLatin1("running host")
        : QString::fromLatin1("offline target");
    QString readiness;
    if (hostMaintenanceActive()) {
        readiness = QString::fromLatin1("host maintenance active");
    } else if (targetCommitted()) {
        readiness = QString::fromLatin1("repair target committed");
    } else {
        readiness = QString::fromLatin1("no committed scope");
    }
    QString freshness;
    const QString id = identity();
    if (m_model.hasDiagnostics(toStd(id))) {
        freshness = m_model.diagnosticsStale()
            ? QString::fromLatin1("diagnostics stale")
            : QString::fromLatin1("diagnostics cached");
    } else {
        freshness = QString::fromLatin1("no diagnostics");
    }
    statusBar()->message(QString::fromLatin1("%1 | disk: %2 | root: %3 | %4 | %5")
                             .arg(scope)
                             .arg(selectedDisk().isEmpty() ? QString::fromLatin1("(none)") : selectedDisk())
                             .arg(selectedRoot().isEmpty() ? QString::fromLatin1("(none)") : selectedRoot())
                             .arg(readiness)
                             .arg(freshness));
}

void LegacyMainWindow::appendLog(const QString &line)
{
    const QString text = line.isNull() ? QString::fromLatin1("") : line;
    m_logLines.append(text);
    ensureLogFile();
    if (!m_logPath.isEmpty()) {
        appendToLogFile(text);
    }
    if (!m_viewingPriorLog && logLinePassesFilter(text)) {
        m_logView->append(text);
    }
}

void LegacyMainWindow::ensureLogFile()
{
    if (!m_logPath.isEmpty()) {
        return;
    }
    if (m_logDirectory.isEmpty()) {
        m_logDirectory = QDir::homeDirPath() + QString::fromLatin1("/.boot-repair-legacy/logs");
    }
    ensureDirectory(m_logDirectory);
    m_logPath = m_logDirectory + QString::fromLatin1("/")
        + QDateTime::currentDateTime().toString(QString::fromLatin1("yyyyMMdd-hhmmss"))
        + QString::fromLatin1(".log");
    if (m_settingsSessionLabel) {
        m_settingsSessionLabel->setText(m_logPath);
    }
    refreshSessionLogList();
}

void LegacyMainWindow::appendToLogFile(const QString &line)
{
    ensureLogFile();
    if (m_logPath.isEmpty()) {
        return;
    }
    QFile file(m_logPath);
    if (!file.open(IO_WriteOnly | IO_Append)) {
        return;
    }
    QTextStream stream(&file);
    stream << line << "\n";
    file.close();
}

bool LegacyMainWindow::logLinePassesFilter(const QString &line) const
{
    if (m_logFilterCombo && m_logFilterCombo->currentItem() == 1
        && !logLineIsError(line)) {
        return false;
    }
    if (m_logSearchEdit) {
        const QString search = m_logSearchEdit->text();
        if (!search.isEmpty() && line.find(search, 0, false) < 0) {
            return false;
        }
    }
    return true;
}

void LegacyMainWindow::refreshLogView()
{
    if (!m_logView) {
        return;
    }
    const QStringList &source = m_viewingPriorLog ? m_priorLogLines : m_logLines;
    m_logView->setText(QString::null);
    for (QStringList::ConstIterator it = source.begin(); it != source.end(); ++it) {
        if (logLinePassesFilter(*it)) {
            m_logView->append(*it);
        }
    }
}

QStringList LegacyMainWindow::sessionLogFiles() const
{
    QStringList paths;
    if (m_logDirectory.isEmpty()) {
        return paths;
    }
    QDir dir(m_logDirectory);
    const QStringList names = dir.entryList(QString::fromLatin1("*.log"),
                                            QDir::Files,
                                            QDir::Name | QDir::Reversed);
    for (QStringList::ConstIterator it = names.begin(); it != names.end(); ++it) {
        paths.append(m_logDirectory + QString::fromLatin1("/") + *it);
    }
    return paths;
}

void LegacyMainWindow::refreshSessionLogList()
{
    if (!m_sessionLogList) {
        return;
    }
    const QString wanted = m_viewingPriorLog ? m_priorLogPath : QString::fromLatin1("current");
    m_sessionLogList->clear();
    // Qt3's QListViewItem has no tooltip support; the file names are the
    // selectable labels and the full paths stay in the Settings tab. Sorting is
    // explicitly disabled and every item is chained after the previous one so
    // the live session is always the first entry.
    m_sessionLogList->setSorting(-1);
    QListViewItem *last = new QListViewItem(
        m_sessionLogList, QString::fromLatin1("Current session"));
    const QStringList files = sessionLogFiles();
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
        if (!m_logPath.isEmpty() && *it == m_logPath) {
            continue;
        }
        last = new QListViewItem(m_sessionLogList, last, QFileInfo(*it).fileName());
    }
    // Restore the selection so a refresh never changes the displayed log.
    bool selectedPrior = false;
    for (QListViewItem *item = m_sessionLogList->firstChild(); item;
         item = item->nextSibling()) {
        const bool matches = wanted == QString::fromLatin1("current")
            ? item == m_sessionLogList->firstChild()
            : item->text(0) == QFileInfo(wanted).fileName();
        if (matches) {
            m_sessionLogList->setSelected(item, true);
            m_sessionLogList->setCurrentItem(item);
            selectedPrior = item != m_sessionLogList->firstChild();
            break;
        }
    }
    if (!selectedPrior && m_sessionLogList->firstChild()) {
        m_sessionLogList->setSelected(m_sessionLogList->firstChild(), true);
        m_sessionLogList->setCurrentItem(m_sessionLogList->firstChild());
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(selectedPrior);
    }
}

void LegacyMainWindow::saveLog()
{
    const QString path = QFileDialog::getSaveFileName(
        QString::null, QString::fromLatin1("Log files (*.log);;All files (*)"),
        this, "save-log", QString::fromLatin1("Save log as"));
    if (path.isEmpty()) {
        return;
    }
    QFile file(path);
    if (!file.open(IO_WriteOnly)) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Could not write %1.").arg(path),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QTextStream stream(&file);
    // The complete live register is saved even while a filter hides entries.
    stream << m_logLines.join(QString::fromLatin1("\n"));
    if (!m_logLines.isEmpty()) {
        stream << "\n";
    }
    file.close();
}

void LegacyMainWindow::clearLog()
{
    // Clear Register: the live register and view only; prior session files are
    // never modified.
    m_logLines.clear();
    m_viewingPriorLog = false;
    m_priorLogPath = QString::null;
    m_priorLogLines.clear();
    if (m_priorLogBanner) {
        m_priorLogBanner->hide();
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(false);
    }
    refreshSessionLogList();
    refreshLogView();
}

void LegacyMainWindow::showAbout()
{
    QMessageBox::about(
        this, QString::fromLatin1("About Boot Bitch"),
        QString::fromLatin1(
            "<b>Boot Bitch Legacy</b><br>"
            "Qt3 frontend for Debian Etch / KDE 3.5-era systems.<br><br>"
            "This GUI is the package's entry point and a thin client for the "
            "ported privileged helper "
            "<tt>/usr/sbin/boot-repair-legacy-helper</tt>. Unavailable tools are "
            "greyed with the helper's own probe reason; every repair keeps the "
            "helper's runtime preflights."));
}

} // namespace legacy
