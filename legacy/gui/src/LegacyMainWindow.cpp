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
#include <qtable.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qsignalmapper.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#ifndef LEGACY_VERSION
#define LEGACY_VERSION "unknown"
#endif

namespace legacy {

// Cycle 9 loop 2: --smoke-test settings isolation (set by main() before the
// window is constructed). The flag lives in the legacy namespace so the
// accessors resolve identically everywhere.
static bool g_smokeSettingsIsolation = false;

void legacySetSmokeSettingsIsolation(bool enabled)
{
    g_smokeSettingsIsolation = enabled;
}

bool legacySmokeSettingsIsolation()
{
    return g_smokeSettingsIsolation;
}

// Cycle 16: an initial log directory applied BEFORE the window is
// constructed. The constructor appends its first readiness lines before
// main() can call setLogDirectory(), and a root run whose HOME is preserved
// (sudo env) would otherwise drop those lines into the invoking user's
// ~/.boot-repair-legacy/logs tree as root-owned files the user cannot
// delete. main() publishes the parsed --log-dir here first, mirroring the
// pre-construction smoke settings isolation.
static QString g_initialLogDirectory = QString::null;

void legacySetInitialLogDirectory(const QString &dir)
{
    g_initialLogDirectory = dir;
}

QString legacyInitialLogDirectory()
{
    return g_initialLogDirectory;
}

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
// The section-title left-edge probe tolerance: a title's mapped x must sit at
// its container's content-left plus at most the frame and layout margins
// (never centered inside the row).
const int kSectionTitleLeftTolerance = 18;

// Hidden-input dialogs (administrator authorization, LUKS passphrase) are
// width-constrained and word-wrapped: the long explanatory text used to size
// the modal to its single unwrapped line and produced an unusably wide window.
const int kHiddenInputMaximumWidth = 440;

// B5: the guarded `config-write` verb transports the edited content through a
// private mode-600 content file (--content-file <path> --content-owner <uid>,
// never argv), so the editor's cap is the helper's 1 MiB file bound (the old
// 64 KiB cap only guarded Etch's 128 KiB argv limit, which no longer applies).
const int kConfigEditMaximumBytes = 1048576;

// The global header icon is kept compact so the eight tabs still fit at the
// 1024x768 layout contract size.
const int kHeaderIconSize = 32;

// The global busy indicator (modern parity) sits in a reserved row below the
// guarded-repair badge and animates by cycling 0..3 trailing dots with a
// plain QTimer while a helper command runs (no threads; Qt 3.3.7 has no
// animated-widget or style-dependent busy rendering to lean on).
const int kBusyAnimationIntervalMs = 400;
// The busy indicator's right edge must be flush with the badge's right edge
// (both are right-aligned in zero-margin rows); layout rounding keeps this
// small tolerance from failing the smoke geometry gate.
const int kBusyBadgeEdgeTolerance = 3;

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
    // Host-only stages (the legacy display-manager repair) stay disabled for
    // the offline target scope with an exact reason.
    bool hostOnly;
    const char *confirm;
};

const ToolSpec toolSpecs[] = {
    { "validate", "validate",
      "Validate environment", "Validate",
      "Check the selected system's mounts, filesystem metadata, boot files, "
      "mapper consistency and dependency readiness before any repair action. "
      "This is an independent safety preflight rather than an optional Full "
      "Repair stage.",
      "", "Always preflight", false, false, false, 0 },
    { "filesystem", "fs-inspect",
      "File system repair", "Check File Systems",
      "Run the read-only file system check for the selected system's root and "
      "/boot filesystems and report each device's check tool and result "
      "without changing anything. This legacy frontend exposes the read-only "
      "check only; device repair is not wired.",
      "", "Full Repair plan: unavailable on this frontend - read-only check only", false, false, false, 0 },
    { "dpkg", "dpkg-configure",
      "Complete package configuration", "Complete Configuration",
      "Complete interrupted dpkg package configuration in the selected repair "
      "system. This is the same stage controlled by Settings -> Full Repair "
      "plan -> Complete interrupted package configuration, but it can also be "
      "run independently here.",
      "dpkg-configure", "", true, false, false,
      "Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights." },
    { "fixbroken", "fix-broken",
      "Repair broken dependencies", "Repair Dependencies",
      "Repair package dependencies in the selected repair system after the "
      "mandatory safety preflight. This maps directly to Settings -> Repair "
      "broken package dependencies.",
      "fix-broken", "", true, false, false,
      "Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards." },
    { "aptupdate", "apt-update",
      "Refresh package metadata", "Refresh Metadata",
      "Refresh APT metadata in the selected repair system without upgrading "
      "installed packages. This maps directly to Settings -> Refresh package "
      "metadata.",
      "apt-update", "", true, false, false,
      "Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source." },
    { "upgrade", "apt-upgrade",
      "Upgrade installed packages", "Simulate and Upgrade",
      "Simulate the APT transaction first, inspect proposed removals, then "
      "apply a safe upgrade. This maps directly to Settings -> Upgrade "
      "installed packages.",
      "apt-upgrade", "", true, false, false,
      "Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards." },
    { "dkms", "",
      "DKMS", "Rebuild DKMS",
      "Rebuild out-of-tree kernel modules for kernels installed in the "
      "selected system. The helper refuses this action when DKMS is not "
      "installed; this legacy frontend exposes no DKMS action.",
      "", "", false, false, false, 0 },
    { "display", "display-manager",
      "Graphical login / display manager", "Restore Graphical Login",
      "Restore the legacy SysV display manager configured for the running "
      "host: the /etc/X11/default-display-manager entry and the missing "
      "runlevel S-symlink, with a backup and rollback, never starting the "
      "GUI. This is a host-scope stage on this legacy frontend.",
      "display-manager", "", true, true, true,
      "Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager." },
    { "initramfs", "initramfs",
      "Initramfs", "Rebuild Initramfs",
      "Rebuild initramfs images for the selected repair system only after "
      "mapper and crypttab consistency checks pass. The helper backs up each "
      "image before the apply. On Etch the stage runs through the guarded "
      "plain-chroot fallback (no unshare required).",
      "initramfs", "", true, true, false,
      "Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights." },
    { "efi", "",
      "EFI / UKI bootloader", "Repair EFI / UKI",
      "Repair the selected system's EFI / UKI boot path. This legacy frontend "
      "exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.",
      "", "", false, false, false, 0 },
    { "grub", "grub",
      "GRUB configuration", "Regenerate GRUB",
      "Regenerate the selected repair system's GRUB menu/configuration after "
      "the mandatory safety preflight. The helper backs up menu.lst, preserves "
      "every existing boot entry and rolls back on any failure.",
      "grub", "", true, true, false,
      "Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure." },
    { "extlinux", "",
      "extlinux configuration", "Regenerate extlinux",
      "Regenerate the selected system's extlinux bootloader configuration. "
      "This legacy frontend exposes no extlinux action.",
      "", "", false, false, false, 0 },
    { "bootstack", "boot-stack",
      "Boot stack reconciliation", "Reconcile Boot Stack",
      "Reconcile the selected repair system's boot stack in one guarded "
      "legacy pass: mapper/crypttab validation, initramfs rebuild and "
      "GRUB-legacy configuration regeneration, with the component backups "
      "and preflights unchanged. This is the Etch equivalent of the modern "
      "boot-stack reconciliation and stays out of the Full Repair plan.",
      "", "Full Repair plan: Manual recovery tool", true, true, false,
      "Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup." }
};
const int toolSpecCount = sizeof(toolSpecs) / sizeof(toolSpecs[0]);

// B7-4: the change-status keys the helper emits, mapped to the friendly stage
// titles used by the repair summary block.
struct RepairStageTitle {
    const char *key;
    const char *title;
};
const RepairStageTitle repairStageTitles[] = {
    { "validate", "Validate" },
    { "filesystem", "File system repair" },
    { "dpkg", "Complete interrupted package configuration" },
    { "fixbroken", "Repair broken package dependencies" },
    { "aptupdate", "Refresh package metadata" },
    { "upgrade", "Upgrade installed packages" },
    { "dkms", "DKMS" },
    { "display", "Graphical login / display manager" },
    { "initramfs", "Initramfs" },
    { "efi", "EFI / UKI bootloader" },
    { "grub", "GRUB configuration" },
    { "extlinux", "extlinux configuration" },
    { "bootstack", "Boot stack reconciliation" },
    { "host-default", "Make Default" }
};
const int repairStageTitleCount = sizeof(repairStageTitles) / sizeof(repairStageTitles[0]);

QString repairStageDisplayTitle(const QString &key)
{
    for (int i = 0; i < repairStageTitleCount; ++i) {
        if (key == QString::fromLatin1(repairStageTitles[i].key)) {
            return QString::fromLatin1(repairStageTitles[i].title);
        }
    }
    return key;
}

// The Full Repair plan stages supported by the legacy helper, in the order the
// plan runs them (the helper's stage_rank order). The Settings checkboxes and
// the persisted state use the modern-compatible labels and QSettings keys.
// Plan-list parity fix: the row set is the modern Settings plan's capability
// key set (filesystem, dpkg, fixbroken, aptupdate, upgrade, dkms, display,
// initramfs, efi, grub, extlinux); a row is never hidden. Availability follows
// the modern semantics: available = checkable with the saved/default
// preference, unavailable = shown, disabled and unchecked (the reason names
// the exact capability/feature probe outcome). The filesystem row is the
// documented frontend gap: this legacy frontend exposes the read-only
// fs-inspect check only, so the row carries no helper stage and is never
// checkable (fail closed), with the exact reason in its tooltip.
struct PlanSpec {
    const char *stage;
    const char *capability;
    const char *title;      // plan list / confirmation title
    const char *checkLabel; // Settings -> Full Repair plan checkbox label
    const char *settingsKey;
    bool defaultChecked;
    bool hostMaintenance;
    // Non-empty when the frontend exposes no plan action for the capability:
    // the row is shown disabled+unchecked with this exact reason even when
    // the helper reports the capability as available (fail closed).
    const char *frontendGap;
};
const PlanSpec planSpecs[] = {
    { "", "filesystem",
      "Repair file system errors (read-only check first)",
      "Repair file system errors (read-only check first)",
      "repair/filesystem", true, false,
      "the legacy frontend exposes the read-only file system check only; "
      "per-device repair is not wired on this frontend (fail closed)" },
    { "dpkg-configure", "dpkg",
      "Complete interrupted package configuration",
      "Complete interrupted package configuration",
      "repair/dpkgConfigure", true, false, "" },
    { "fix-broken", "fixbroken",
      "Repair broken package dependencies",
      "Repair broken package dependencies",
      "repair/fixBroken", true, false, "" },
    { "apt-update", "aptupdate",
      "Refresh package metadata",
      "Refresh package metadata",
      "repair/refreshMetadata", true, false, "" },
    { "apt-upgrade", "upgrade",
      "Upgrade installed packages",
      "Upgrade installed packages (adaptive APT simulation)",
      "repair/upgradePackages", false, false, "" },
    { "dkms", "dkms",
      "Rebuild DKMS modules",
      "Rebuild DKMS modules",
      "repair/dkms", true, true, "" },
    { "display-manager", "display",
      "Restore graphical login manager",
      "Restore graphical login manager",
      "repair/displayManager", false, true, "" },
    { "initramfs", "initramfs",
      "Rebuild initramfs after mapper/crypttab validation",
      "Rebuild initramfs after mapper/crypttab validation",
      "repair/initramfs", true, true, "" },
    { "efi", "efi",
      "Repair EFI / UKI boot path",
      "Repair EFI / UKI boot path",
      "repair/efiBootloader", false, true, "" },
    { "grub", "grub",
      "Update GRUB configuration",
      "Update GRUB configuration",
      "repair/grub", true, true, "" },
    { "extlinux", "extlinux",
      "Update extlinux configuration",
      "Update extlinux configuration",
      "repair/extlinux", true, true, "" }
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

// Index of a plan capability key in planSpecs, or -1 (the row set must stay
// 1:1 with the modern plan key set; the smoke test and the contract use this
// to pin the parity).
int planIndexForCapability(const char *capability)
{
    if (!capability || !*capability) {
        return -1;
    }
    for (int i = 0; i < planSpecCount; ++i) {
        if (std::strcmp(planSpecs[i].capability, capability) == 0) {
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

// The Logs filter combo, 1:1 with the modern order: All entries / Diagnostics
// / Repairs / the three workflow filters / the 16 diagnostic sections. The
// combo cannot carry per-item data in Qt3, so the index semantics live in
// this single table. `kind` is the LogEntry::Kind filter (or Application for
// an entry that matches nothing), `stage` is the helper stage a Repair entry
// must carry for the workflow filters ("" = no stage check), `diagKey` is the
// diagnostic key a Diagnostic entry must carry for the section filters ("" =
// no key check). The File copy workflow has no lines in this frontend (the
// file-copy feature is greyed) but the entry stays for the 1:1 combo.
struct LogFilterSpec {
    const char *title;
    int kind;            // LogEntry::Kind or Application when unused
    const char *stage;   // single stage match for Repair workflows
    const char *diagKey; // diagnostic-key match for section filters
};
const LogFilterSpec logFilterSpecs[] = {
    { "All entries", LogEntry::Application, "", "" },
    { "Diagnostics", LogEntry::Diagnostic, "", "" },
    { "Repairs", LogEntry::Repair, "", "" },
    { "File system repair", LogEntry::Repair, "fs-inspect", "" },
    { "Package repair", LogEntry::Repair, "package", "" },
    { "File copy", LogEntry::Application, "", "" }
};
const int logFilterSpecCount = sizeof(logFilterSpecs) / sizeof(logFilterSpecs[0]);

// True when the space-separated stage list contains the legacy package stages
// mapped to the "Package repair" workflow (fix-broken, dpkg-configure,
// apt-update, apt-upgrade).
bool stagesContainPackageStage(const QString &stages)
{
    if (stages.find(QString::fromLatin1("fix-broken")) >= 0
        || stages.find(QString::fromLatin1("dpkg-configure")) >= 0
        || stages.find(QString::fromLatin1("apt-update")) >= 0
        || stages.find(QString::fromLatin1("apt-upgrade")) >= 0) {
        return true;
    }
    return false;
}

// Resolves a command through a read-only PATH search (plus the standard system
// directories for launchers with a reduced PATH); the probe never executes
// anything, mirroring CapabilityChecker::findExecutablePortable.
QString findExecutablePath(const QString &command)
{
    if (command.isEmpty()) {
        return QString::null;
    }
    const QString pathValue = QString::fromLocal8Bit(::getenv("PATH"));
    const QStringList dirs = QStringList::split(QChar(':'), pathValue, false);
    static const char *const standardDirs[] = {
        "/usr/local/sbin", "/usr/local/bin", "/usr/sbin", "/usr/bin",
        "/sbin", "/bin"
    };
    for (QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it) {
        if ((*it).isEmpty()) {
            continue;
        }
        const QString candidate = *it + QString::fromLatin1("/") + command;
        if (QFile::exists(candidate) && ::access(candidate.latin1(), X_OK) == 0) {
            return candidate;
        }
    }
    for (int i = 0; i < 6; ++i) {
        const QString candidate = QString::fromLatin1(standardDirs[i])
            + QString::fromLatin1("/") + command;
        if (QFile::exists(candidate) && ::access(candidate.latin1(), X_OK) == 0) {
            return candidate;
        }
    }
    return QString::null;
}

// One /etc/os-release value, read-only; "" when the file is missing or the
// key is absent (the Settings host-capabilities summary probes the running
// host exactly like CapabilityChecker::distributionLabel).
QString readOsReleaseValue(const QString &key)
{
    QFile file(QString::fromLatin1("/etc/os-release"));
    if (!file.open(IO_ReadOnly)) {
        return QString::null;
    }
    QTextStream stream(&file);
    QString line;
    while (!stream.atEnd()) {
        line = stream.readLine().stripWhiteSpace();
        if (line.isEmpty() || line[0] == QChar('#')) {
            continue;
        }
        const int eq = line.find(QChar('='));
        if (eq <= 0 || line.left(eq) != key) {
            continue;
        }
        QString value = line.mid(eq + 1).stripWhiteSpace();
        if (value.length() >= 2 && value[0] == QChar('"')
            && value[value.length() - 1] == QChar('"')) {
            value = value.mid(1, value.length() - 2);
        }
        file.close();
        return value;
    }
    file.close();
    return QString::null;
}

// Human-readable package-manager family for the running host, mirroring
// CapabilityChecker::packageManagerLabel with the same fallbacks.
QString packageManagerLabelForHost()
{
    const QString id = readOsReleaseValue(QString::fromLatin1("ID")).lower();
    const QString like = readOsReleaseValue(QString::fromLatin1("ID_LIKE")).lower();
    const bool debianLike = id == QString::fromLatin1("debian")
        || id == QString::fromLatin1("ubuntu")
        || id == QString::fromLatin1("tuxedo")
        || id == QString::fromLatin1("linuxmint")
        || id == QString::fromLatin1("pop")
        || id == QString::fromLatin1("elementary")
        || id == QString::fromLatin1("zorin")
        || like.find(QString::fromLatin1("debian")) >= 0
        || like.find(QString::fromLatin1("ubuntu")) >= 0;
    if (debianLike) {
        return QString::fromLatin1("APT / dpkg");
    }
    const bool fedoraLike = id == QString::fromLatin1("fedora")
        || id == QString::fromLatin1("rhel")
        || id == QString::fromLatin1("rocky")
        || id == QString::fromLatin1("almalinux")
        || like.find(QString::fromLatin1("fedora")) >= 0
        || like.find(QString::fromLatin1("rhel")) >= 0;
    if (fedoraLike) {
        return QString::fromLatin1("DNF / RPM");
    }
    const bool archLike = id == QString::fromLatin1("arch")
        || id == QString::fromLatin1("manjaro")
        || id == QString::fromLatin1("endeavouros")
        || id == QString::fromLatin1("garuda")
        || id == QString::fromLatin1("artix")
        || like.find(QString::fromLatin1("arch")) >= 0;
    if (archLike) {
        return QString::fromLatin1("pacman");
    }
    if (id == QString::fromLatin1("alpine")
        || like.find(QString::fromLatin1("alpine")) >= 0) {
        return QString::fromLatin1("apk / OpenRC");
    }
    if (id.startsWith(QString::fromLatin1("opensuse"))
        || id == QString::fromLatin1("suse") || id == QString::fromLatin1("sles")
        || like.find(QString::fromLatin1("suse")) >= 0) {
        return QString::fromLatin1("zypper / RPM");
    }
    return QString::fromLatin1("Unknown / unsupported automatic mapping");
}

// One read-only host-capability probe row, mirroring CapabilityChecker::scanHost
// requirement-by-requirement plus the one legacy-specific row. The suggested
// package follows packageForCommand's Debian-family mapping where it exists
// and the command's conventional package otherwise.
struct CapabilitySpec {
    const char *feature;
    const char *command;
    const char *scope;
    const char *package;
    const char *note;
};
const CapabilitySpec capabilitySpecs[] = {
    { "Device discovery", "lsblk", "Host", "util-linux",
      "Required for block-device inventory" },
    { "Filesystem identification", "blkid", "Host", "util-linux",
      "Used to identify filesystem metadata" },
    { "Mount inspection", "findmnt", "Host", "util-linux",
      "Used to understand active mounts" },
    { "LUKS support", "cryptsetup", "Host", "cryptsetup",
      "Required to unlock encrypted targets" },
    { "Btrfs support", "btrfs", "Host", "btrfs-progs",
      "Required for Btrfs inspection and snapshot rollback" },
    { "Bidirectional file copy", "rsync", "Host/Repair", "rsync",
      "Required for verified Host-to-Repair and Repair-to-Host transfer" },
    { "Chroot repair", "chroot", "Host", "coreutils",
      "Required for target-side repair commands" },
    { "Offline systemd repair", "systemctl", "Host/Repair", "systemd",
      "Used to restore graphical.target and the configured display manager without starting the target GUI" },
    { "UEFI NVRAM inspection", "efibootmgr", "Host", "efibootmgr",
      "Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available" },
    { "UKI verification", "objcopy", "Host", "binutils",
      "Used to verify the kernel embedded in a rebuilt unified kernel image" },
    { "GRUB EFI repair", "grub-install", "Target/Host", "grub2-common",
      "Required only for conventional GRUB-based EFI systems" },
    { "GRUB configuration", "update-grub", "Target", "grub-common",
      "Debian-family GRUB helper" },
    { "GRUB configuration", "grub-mkconfig", "Target", "grub-common",
      "Portable GRUB configuration generator used by Arch and other non-Debian systems" },
    { "Initramfs rebuild", "update-initramfs", "Target", "initramfs-tools",
      "Debian-family initramfs helper" },
    { "Initramfs rebuild", "mkinitcpio", "Target", "mkinitcpio",
      "Arch-family initramfs generator" },
    { "Initramfs rebuild", "dracut", "Target", "dracut",
      "Alternative initramfs generator used by Arch and other distributions" },
    { "Initramfs verification", "lsinitcpio", "Target", "mkinitcpio",
      "Read-only verification for mkinitcpio images" },
    { "Initramfs verification", "lsinitrd", "Target", "dracut",
      "Read-only verification for dracut images" },
    { "systemd-boot inspection", "bootctl", "Host/Target", "systemd",
      "Read-only inspection of systemd-boot and generic UKI layouts" },
    { "Arch package manager", "pacman", "Host/Target", "pacman",
      "Arch-family package database and transaction tool" },
    { "DKMS rebuild", "dkms", "Target", "dkms",
      "Required only when target uses DKMS modules" },
    { "LVM inspection", "lvs", "Host", "lvm2",
      "Optional LVM storage-stack support" },
    { "Software RAID", "mdadm", "Host", "mdadm",
      "Optional Linux MD RAID support" },
    // Legacy-specific row: Etch has no unshare, so the ported helper falls
    // back to a guarded plain chroot.
    { "Process namespace isolation", "unshare", "Host+Target", "util-linux",
      "The ported helper falls back to a guarded plain chroot when unshare is absent" }
};
const int capabilitySpecCount = sizeof(capabilitySpecs) / sizeof(capabilitySpecs[0]);

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

// Cycle 10: shared wrapped confirmation dialog. Qt3's QMessageBox sizes
// itself to the unwrapped text, so long repair confirmations stretched past
// the window; this stack dialog wraps the wording (Qt::WordBreak) inside a
// bounded width and keeps the Yes/No (or Yes/Cancel) question semantics.
bool confirmWrapped(QWidget *parent, const QString &title, const QString &text,
                    bool cancelInsteadOfNo = false)
{
    QDialog dialog(parent, "legacy-confirm", true);
    dialog.setCaption(title);
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *label = new QLabel(text, &dialog);
    enableLabelWordWrap(label);
    label->setMaximumWidth(kHiddenInputMaximumWidth);
    layout->addWidget(label);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *noButton = new QPushButton(
        cancelInsteadOfNo ? QString::fromLatin1("Cancel")
                          : QString::fromLatin1("No"),
        &dialog);
    QPushButton *yesButton = new QPushButton(QString::fromLatin1("Yes"), &dialog);
    yesButton->setDefault(true);
    buttons->addWidget(noButton);
    buttons->addWidget(yesButton);
    QObject::connect(noButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(yesButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    dialog.setMaximumWidth(kHiddenInputMaximumWidth + 40);
    return dialog.exec() == QDialog::Accepted;
}

QString fromStd(const std::string &text)
{
    return QString::fromLatin1(text.c_str());
}

std::string toStd(const QString &text)
{
    return std::string(text.latin1());
}

// Creates every missing path component with `mode` (mkdir); an existing
// component must be a directory or the whole call fails. The leaf itself is
// never chmod'ed here: callers that need a re-tightened leaf do it
// explicitly and only for the GUI's own private paths.
bool ensureDirectory(const QString &path, mode_t mode)
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
        if (::mkdir(current.latin1(), mode) != 0) {
            struct stat info;
            if (::stat(current.latin1(), &info) != 0 || !S_ISDIR(info.st_mode)) {
                return false;
            }
        }
    }
    return true;
}

// True when `path` is the GUI's own default log tree
// (~/.boot-repair-legacy/logs or below it). Only that private tree is ever
// chmod'ed by the GUI: a user-supplied --log-dir may be a shared path and
// keeps the permissions its owner chose.
bool isDefaultLogTree(const QString &path)
{
    const QString homeLogs = QDir::homeDirPath()
        + QString::fromLatin1("/.boot-repair-legacy/logs");
    return path == homeLogs
        || path.startsWith(homeLogs + QString::fromLatin1("/"));
}

// The pending secret-file paths for the termination handlers, kept in plain
// char buffers (never QString/QByteArray) so a handler only touches
// async-signal-safe primitives: unlink() and _exit(). The secret-file writer
// registers the created file (unlock keyfile or config-write content file),
// discardUnlockKeyfile()/discardConfigContentFile() clear it.
static char gUnlockKeyfilePath[PATH_MAX];
static char gConfigContentFilePath[PATH_MAX];

extern "C" void legacyKeyfileTerminationHandler(int signalNumber)
{
    if (gUnlockKeyfilePath[0] != '\0') {
        ::unlink(gUnlockKeyfilePath);
    }
    if (gConfigContentFilePath[0] != '\0') {
        ::unlink(gConfigContentFilePath);
    }
    ::_exit(128 + signalNumber);
}

// Installs SIGTERM/SIGINT/SIGHUP handlers once, so a terminated GUI never
// leaves a secret file behind: the handler unlinks the pending unlock
// keyfile and the pending config-write content file (best-effort) and exits
// with the conventional 128+signum status.
void installKeyfileTerminationHandlers()
{
    struct sigaction action;
    ::memset(&action, 0, sizeof(action));
    action.sa_handler = legacyKeyfileTerminationHandler;
    ::sigemptyset(&action.sa_mask);
    const int handledSignals[] = { SIGTERM, SIGINT, SIGHUP };
    for (std::size_t i = 0;
         i < sizeof(handledSignals) / sizeof(handledSignals[0]); ++i) {
        ::sigaction(handledSignals[i], &action, 0);
    }
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
// The filesystem label for one non-disk inventory row: the mounted/swapped
// filesystem first, then the read-only udev probe (so an unmounted ext3 or a
// locked crypto_LUKS component is still visible without opening the device),
// then the swap marker, and "unknown" as the last resort. The label is never
// empty: every Filesystem cell in the device list and the details panel
// renders something.
QString displayFsType(const DeviceRow &row)
{
    if (!row.fstype.empty()) {
        return fromStd(row.fstype);
    }
    if (!row.probedFstype.empty()) {
        return fromStd(row.probedFstype);
    }
    if (row.mountpoint == "[swap]") {
        return QString::fromLatin1("[swap]");
    }
    return QString::fromLatin1("unknown");
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
      m_toolSortColumn(-1),
      m_toolSortAscending(true),
      m_helpSignalMapper(0),
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
      m_busyLabel(0),
      m_busyTimer(0),
      m_busyFrame(0),
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
      m_fileCopyScopeLabel(0),
      m_fileCopySourceTitle(0),
      m_fileCopyDestinationTitle(0),
      m_fileCopyOptionsTitle(0),
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
      m_targetSummary(0),
      m_configLabel(0),
      m_configReasonLabel(0),
      m_chrootReasonLabel(0),
      m_authStatusLabel(0),
      m_settingsHelperLabel(0),
      m_settingsElevationLabel(0),
      m_settingsLogDirLabel(0),
      m_settingsSessionLabel(0),
      m_settingsVersionLabel(0),
      m_capDistributionLabel(0),
      m_capPackageManagerLabel(0),
      m_capAuthLabel(0),
      m_capabilityTable(0),
      m_scanButton(0),
      m_diagnosticsButton(0),
      m_runDiagnosticButton(0),
      m_copyResultsButton(0),
      m_saveResultsButton(0),
      m_configButton(0),
      m_unlockButton(0),
      m_setTargetButton(0),
      m_hostSystemLabel(0),
      m_hostStorageLabel(0),
      m_hostProtectedBadge(0),
      m_hostDetailsButton(0),
      m_detailsPaneTitle(0),
      m_inspectingHostDetails(false),
      m_hostCard(0),
      m_hostMaintenanceButton(0),
      m_authorizeButton(0),
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
      m_installSupportButton(0),
      m_hostDefaultButton(0),
      m_fileCopyAddFilesButton(0),
      m_fileCopyAddFolderButton(0),
      m_fileCopyRemoveButton(0),
      m_fileCopyClearButton(0),
      m_fileCopyPreviewButton(0),
      m_fileCopyRunButton(0),
      m_fileCopyDirectionCombo(0),
      m_fileCopyOwnershipCombo(0),
      m_fileCopySourceList(0),
      m_fileCopyDestinationEdit(0),
      m_fileCopyBrowseButton(0),
      m_systemsSplitter(0),
      m_repairVerticalSplitter(0),
      m_chrootGroup(0),
      m_fileCopySourceGroup(0),
      m_fileCopyDestinationGroup(0),
      m_fileCopyOptionsGroup(0),
      m_chrootTab(0),
      m_settingsContent(0),
      m_repairContent(0),
      m_resultDialog(0),
      m_resultView(0),
      m_chrootCommandHeading(0),
      m_runner(new HelperRunner(this)),
      m_logEntryKind(LogEntry::Application),
      m_updatingSelection(false),
      m_running(false),
      m_pendingDiagnostic(false),
      m_pendingQuiet(false),
      m_pendingUnlock(false),
      m_pendingConfig(false),
      m_pendingShell(false),
      m_pendingBrowse(false),
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
    // B3/B5: a SIGTERM/SIGINT/SIGHUP must never leave a secret file behind
    // (unlock keyfile or config-write content file). The handlers unlink the
    // pending files (if any) and exit 128+signum.
    installKeyfileTerminationHandlers();
    // Every section title/group title/button/list column must stay fully
    // visible at the 1024x768 contract size; the layouts only need a modest
    // floor below it.
    setMinimumSize(820, 600);

    // Global header, mirroring the modern Qt6 window: packaged icon, the
    // 1.65x bold "Boot Bitch" title, the subtle recovery subtitle and the
    // right-aligned guarded-repair badge. The icon falls back gracefully when
    // the PNG is not installed (source-tree or headless runs).
    QWidget *central = new QWidget(this);
    // The global header spans the full window width: the guarded-repair badge
    // must sit flush against the window's right edge (modern parity), which a
    // QBoxLayout margin cannot express per side. The main layout carries no
    // margin; the header row gets an explicit left inset and the tab pages
    // keep their own inner margins.
    QVBoxLayout *mainLayout = new QVBoxLayout(central, 0, 6);

    QHBoxLayout *headerLayout = new QHBoxLayout(mainLayout);
    headerLayout->setSpacing(8);
    headerLayout->addSpacing(8);
    QLabel *iconLabel = new QLabel(central);
    const QPixmap headerPixmap = legacyHeaderPixmap();
    if (!headerPixmap.isNull()) {
        iconLabel->setPixmap(headerPixmap);
        iconLabel->setFixedSize(kHeaderIconSize, kHeaderIconSize);
        iconLabel->setAlignment(Qt::AlignCenter);
        // The packaged PNG is the real Boot Bitch artwork (generated from the
        // modern master by legacy/gui/data/make-icons.py); the window/app icon
        // uses it too, falling back gracefully when the PNG is not installed.
        setIcon(headerPixmap);
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
    m_headerBadge->setFrameShape(QFrame::NoFrame);
    m_headerBadge->setMargin(5);
    m_headerBadge->setAlignment(Qt::AlignCenter);
    // Cycle 14 loop: the frameless badge keeps its needed width (the frame
    // removal shaved 2px and the smoke's clip gate caught it).
    m_headerBadge->setMinimumWidth(
        QFontMetrics(m_headerBadge->font()).width(m_headerBadge->text()) + 12);
    QToolTip::add(m_headerBadge, QString::fromLatin1(
        "Ordinary repairs require an explicitly selected non-host target. The "
        "protected running host has a separate deliberate maintenance mode with "
        "the same guarded repair stages and requires privilege authorization."));
    headerLayout->addWidget(m_headerBadge, 0, Qt::AlignTop);

    // Global busy indicator (modern parity): a reserved row BELOW the header
    // that shows the modern working text right-aligned under the guarded-
    // repair badge only while a helper command runs. The row keeps a fixed
    // height (the label never changes height), so showing or clearing the
    // text never moves the header widgets or the tab pages; the right-aligned
    // label grows toward the left when the text appears. A plain QTimer
    // cycles 0..3 trailing dots behind the text (no threads; Qt 3.3.7 needs
    // no animated-widget support for this).
    QHBoxLayout *busyRow = new QHBoxLayout();
    busyRow->setSpacing(0);
    busyRow->addStretch(1);
    m_busyLabel = new QLabel(QString::null, central);
    QFont busyFont = m_busyLabel->font();
    busyFont.setBold(true);
    m_busyLabel->setFont(busyFont);
    m_busyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_busyLabel->setFixedHeight(
        QFontMetrics(m_busyLabel->font()).height() + 4);
    busyRow->addWidget(m_busyLabel, 0, Qt::AlignRight);
    mainLayout->addLayout(busyRow);

    m_busyTimer = new QTimer(this);
    m_busyFrame = 0;
    connect(m_busyTimer, SIGNAL(timeout()), this, SLOT(advanceBusyAnimation()));

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
    // Modern parity: there is no About tab; Help -> About Boot Bitch opens the
    // dialog instead (showAbout()).

    buildMenus();
    // Load the persisted options before the first inventory scan so the
    // device-list filters are applied to the initial population.
    loadLegacySettings();

    connect(m_runner, SIGNAL(outputLine(const QString &)),
            this, SLOT(helperLine(const QString &)));
    connect(m_runner, SIGNAL(finished(bool, int)),
            this, SLOT(helperFinished(bool, int)));

    // Cycle 16: honor the pre-construction --log-dir so the constructor's own
    // readiness lines (written below) never land in the default $HOME tree.
    if (legacyInitialLogDirectory().isEmpty()) {
        m_logDirectory = QDir::homeDirPath() + QString::fromLatin1("/.boot-repair-legacy/logs");
    } else {
        m_logDirectory = legacyInitialLogDirectory();
    }
    scanDevices();
    autoDetectHostTarget();
    updateElevationLabel();
    updateHostCard();
    updateDriveDetails();
    updateUnlockStatus();
    updateDiagnosticDetails();
    refreshCapabilities();
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
    // Belt-and-braces persistence flush: every toggle already saves on change;
    // this final write covers any path that mutated state without re-entering
    // a handler (for example a programmatic scope change).
    saveLegacySettings();
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
    // Cycle 16: when a pre-construction --log-dir already routed the
    // constructor's readiness lines into `path`, the session file is already
    // open there; only a genuinely different directory reopens it (otherwise
    // one session would split into two files).
    if (path != m_logDirectory) {
        m_logDirectory = path;
        m_logPath = QString::null;
    }
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
// The label expands horizontally and keeps AlignLeft, so its text always
// starts at the same left edge as the frame content below it; without the
// Expanding policy some Qt3/KDE style combinations center the narrow label
// inside the row, which the smoke's left-edge probe rejects.
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
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    m_sectionTitles.push_back(label);
    return label;
}

// Cycle 14: the (i) help link beside each page title (modern
// contextHelpButton parity): a frameless hyperlink-style label with the
// page's informational text, shown in a width-constrained read-only dialog
// on click. No tooltip-only behavior remains.
int g_helpCounter = 0;

QWidget *LegacyMainWindow::makeHelpButton(QWidget *parent, const QString &title,
                                          const QString &text)
{
    // Cycle 14 loop: Qt 3.3.7's QLabel has no linkActivated signal at
    // runtime, so the (i) affordance is a flat auto-raised QToolButton that
    // opens the width-constrained info dialog through a QSignalMapper.
    ++g_helpCounter;
    const QString href = QString::fromLatin1("help:%1").arg(g_helpCounter);
    m_helpTitles.insert(href, title);
    m_helpTexts.insert(href, text);
    QToolButton *button = new QToolButton(parent);
    button->setText(QString::fromLatin1("i"));
    button->setAutoRaise(true);
    button->setFixedSize(18, 18);
    if (!m_helpSignalMapper) {
        m_helpSignalMapper = new QSignalMapper(this);
        connect(m_helpSignalMapper, SIGNAL(mapped(const QString &)),
                this, SLOT(showHelpPopup(const QString &)));
    }
    connect(button, SIGNAL(clicked()), m_helpSignalMapper, SLOT(map()));
    m_helpSignalMapper->setMapping(button, href);
    return button;
}

void LegacyMainWindow::showHelpPopup(const QString &href)
{
    const QString title = mapValue(m_helpTitles, href);
    const QString text = mapValue(m_helpTexts, href);
    if (text.isEmpty()) {
        return;
    }
    QDialog dialog(this, "legacy-info", true);
    dialog.setCaption(title.isEmpty()
                          ? QString::fromLatin1("Information") : title);
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *label = new QLabel(text, &dialog);
    enableLabelWordWrap(label);
    label->setMaximumWidth(kHiddenInputMaximumWidth);
    layout->addWidget(label);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->addStretch();
    QPushButton *close = new QPushButton(QString::fromLatin1("Close"), &dialog);
    close->setDefault(true);
    buttons->addWidget(close);
    QObject::connect(close, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    dialog.setMaximumWidth(kHiddenInputMaximumWidth + 40);
    dialog.exec();
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

// Every visible device-tree item (top-level disks then their children), in
// traversal order.  The device list is now a tree (B7-2): the flat firstChild
// walk only ever sees the disks.
std::vector<QListViewItem *> LegacyMainWindow::deviceTreeItems() const
{
    std::vector<QListViewItem *> items;
    if (!m_deviceList) {
        return items;
    }
    for (QListViewItem *item = m_deviceList->firstChild(); item;
         item = item->nextSibling()) {
        items.push_back(item);
        for (QListViewItem *child = item->firstChild(); child;
             child = child->nextSibling()) {
            items.push_back(child);
        }
    }
    return items;
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
    const std::vector<QListViewItem *> items = deviceTreeItems();
    for (int column = 0; column < m_deviceList->columns(); ++column) {
        int width = metrics.width(m_deviceList->columnText(column)) + kListHeaderPadding;
        for (std::size_t i = 0; i < items.size(); ++i) {
            width = QMAX(width,
                         metrics.width(items[i]->text(column)) + kListHeaderPadding);
        }
        m_deviceList->setColumnWidth(column, width);
    }
    // Qt 3.3.7 only re-stretches the LastColumn-mode final column from the
    // header's own resize event; a programmatic setColumnWidth() leaves the
    // viewport gutter behind (the Etch smoke's 382px-gutter regression). Fill
    // the viewport explicitly: the final column spans from the previous
    // column's end to the viewport's right edge, never below its auto-sized
    // width and never below the 20px Qt3 minimum.
    if (m_deviceList->columns() > 0 && m_deviceList->viewport()) {
        const int lastColumn = m_deviceList->columns() - 1;
        int others = 0;
        for (int column = 0; column < lastColumn; ++column) {
            others += m_deviceList->columnWidth(column);
        }
        const int fill = m_deviceList->viewport()->width() - others;
        const int lastWidth = QMAX(QMAX(m_deviceList->columnWidth(lastColumn), 20),
                                   fill);
        m_deviceList->setColumnWidth(lastColumn, lastWidth);
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
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_systemsHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, QString::fromLatin1("Systems"), QString::fromLatin1(
        "Select a physical drive; Boot Bitch resolves the most likely Linux "
        "system volume automatically. The running host stays protected from "
        "ordinary target repairs, with a separate explicit host-maintenance "
        "path for its own system. The Details button shows the protected "
        "host's facts in the details pane; selecting any drive row restores "
        "the per-drive pane.")));
    headingRow->addStretch();
    m_scanButton = makeButton(QString::fromLatin1("Refresh Devices"), page);
    QToolTip::add(m_scanButton, QString::fromLatin1(
        "Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, "
        "/proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev "
        "metadata database). No block device is opened and nothing is written."));
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(scanDevices()));
    headingRow->addWidget(m_scanButton, 0, Qt::AlignTop);

    // Modern protected-host card (buildSystemsPage parity): the running host
    // is shown in a permanently-protected card above the candidate list, with
    // the green-check indicator, the probe-based identity/storage line, the
    // PROTECTED badge and the Details / Host Maintenance / Make Default
    // actions on its right. The values are the legacy-available fields
    // (helper facts + the read-only inventory) and stay honest when unknown.
    m_hostCard = new QGroupBox(page);
    QHBoxLayout *hostCardLayout = new QHBoxLayout(m_hostCard, 10, 6);
    // Cycle 10: ASCII-only indicator — Etch's fonts garble the modern check
    // glyph.
    QLabel *hostCheck = new QLabel(QString::fromLatin1("[OK]"), m_hostCard);
    QFont checkFont = hostCheck->font();
    checkFont.setBold(true);
    checkFont.setPointSizeFloat(checkFont.pointSizeFloat() * 1.4);
    hostCheck->setFont(checkFont);
    QPalette checkPalette = hostCheck->palette();
    checkPalette.setColor(QColorGroup::Foreground, QColor(0x2e, 0xa0, 0x43));
    hostCheck->setPalette(checkPalette);
    QToolTip::add(hostCheck, QString::fromLatin1(
        "The running system was detected and stays protected from ordinary "
        "target repairs."));
    hostCardLayout->addWidget(hostCheck, 0, Qt::AlignTop);
    // Parentless sub-layouts owned by addLayout() (a layout constructed with
    // a parent layout is added automatically at construction; an explicit
    // addLayout() would parent it twice).
    QVBoxLayout *hostText = new QVBoxLayout();
    hostText->setSpacing(2);
    m_hostSystemLabel = new QLabel(
        QString::fromLatin1("Detecting running system..."), m_hostCard);
    QFont hostFont = m_hostSystemLabel->font();
    hostFont.setBold(true);
    m_hostSystemLabel->setFont(hostFont);
    m_hostSystemLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    m_hostSystemLabel->setMinimumWidth(0);
    hostText->addWidget(m_hostSystemLabel);
    m_hostStorageLabel = new QLabel(
        QString::fromLatin1("Detecting protected storage..."), m_hostCard);
    m_hostStorageLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    m_hostStorageLabel->setMinimumWidth(0);
    hostText->addWidget(m_hostStorageLabel);
    hostCardLayout->addLayout(hostText, 1);
    // The actions column is a ctor-parented sub-layout (auto-added at
    // construction, no explicit addLayout) like the other card sub-rows.
    QVBoxLayout *hostActions = new QVBoxLayout(hostCardLayout);
    hostActions->setSpacing(6);
    QHBoxLayout *hostBadgeRow = new QHBoxLayout(hostActions);
    m_hostProtectedBadge = new QLabel(QString::fromLatin1("PROTECTED"), m_hostCard);
    QFont protectedFont = m_hostProtectedBadge->font();
    protectedFont.setBold(true);
    m_hostProtectedBadge->setFont(protectedFont);
    m_hostProtectedBadge->setFrameShape(QFrame::NoFrame);
    m_hostProtectedBadge->setMargin(5);
    QToolTip::add(m_hostProtectedBadge, QString::fromLatin1(
        "The running host remains protected from ordinary repair-target "
        "operations."));
    hostBadgeRow->addWidget(m_hostProtectedBadge, 0, Qt::AlignLeft | Qt::AlignTop);
    QHBoxLayout *hostButtonsRow = new QHBoxLayout(hostActions);
    hostButtonsRow->setSpacing(6);
    m_hostDetailsButton = makeButton(QString::fromLatin1("Details"), m_hostCard);
    m_hostDetailsButton->setEnabled(false);
    QToolTip::add(m_hostDetailsButton, QString::fromLatin1(
        "Show read-only details for the protected running host."));
    connect(m_hostDetailsButton, SIGNAL(clicked()), this, SLOT(showHostDetails()));
    hostButtonsRow->addWidget(m_hostDetailsButton, 0, Qt::AlignVCenter);
    m_hostMaintenanceButton = makeButton(QString::fromLatin1("Host Maintenance"), m_hostCard);
    m_hostMaintenanceButton->setEnabled(false);
    connect(m_hostMaintenanceButton, SIGNAL(clicked()), this, SLOT(toggleHostMaintenance()));
    hostButtonsRow->addWidget(m_hostMaintenanceButton, 0, Qt::AlignVCenter);
    // Modern Make Default parity (B7-7): beside Host Maintenance, gated by
    // Host Maintenance + the cached host-default probe + the session.
    m_hostDefaultButton = makeButton(QString::fromLatin1("Make Default"), m_hostCard);
    m_hostDefaultButton->setEnabled(false);
    QToolTip::add(m_hostDefaultButton, QString::fromLatin1(
        "Make the canonical installed kernel entry the default GRUB-legacy "
        "boot entry on the running host (menu.lst default directive with a "
        "backup and rollback). Requires Host Maintenance and the cached "
        "host-default probe."));
    connect(m_hostDefaultButton, SIGNAL(clicked()), this, SLOT(makeDefault()));
    hostButtonsRow->addWidget(m_hostDefaultButton, 0, Qt::AlignVCenter);
    registerGroupBox(m_hostCard);
    pageLayout->addWidget(m_hostCard);

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
    m_systemsSplitter = splitter;

    // ---- Left column: inventory, action row, Unlock status ------------------
    QWidget *left = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(left, 0, 6);

    m_deviceList = new QListView(left);
    // Modern tree parity (B7-2): whole disks are top-level items with their
    // partitions/mappers as indented children (expand/collapse via the Qt3
    // tree features); Device, Size, Type, Filesystem columns only (Mount and
    // Note are dropped; the Filesystem column keeps the never-empty labels
    // with the disk aggregate).
    addListViewColumn(m_deviceList, QString::fromLatin1("Device"), 100);
    addListViewColumn(m_deviceList, QString::fromLatin1("Size"), 60);
    addListViewColumn(m_deviceList, QString::fromLatin1("Type"), 80);
    addListViewColumn(m_deviceList, QString::fromLatin1("Filesystem"), 75);
    m_deviceList->setAllColumnsShowFocus(true);
    m_deviceList->setShowSortIndicator(true);
    m_deviceList->setMultiSelection(false);
    m_deviceList->setResizeMode(QListView::LastColumn);
    m_deviceList->setMinimumHeight(70);
    m_deviceList->setRootIsDecorated(true);
    m_deviceList->setTreeStepSize(20);
    connect(m_deviceList, SIGNAL(selectionChanged()), this, SLOT(deviceSelectionChanged()));
    leftLayout->addWidget(m_deviceList, 1);

    // Modern action rows: Row 1 carries the repair-target actions (Select
    // Target / Unlock; Host Maintenance and Make Default live on the
    // protected-host card above); row 2 carries the deferred-authorization
    // affordance and the committed-target summary. The status label keeps its
    // Ignored horizontal policy so the row minimum is the buttons only.
    QHBoxLayout *actionRow = new QHBoxLayout(leftLayout);
    actionRow->setSpacing(6);
    m_setTargetButton = makeButton(QString::fromLatin1("Select Target"), left);
    m_setTargetButton->setEnabled(false);
    connect(m_setTargetButton, SIGNAL(clicked()), this, SLOT(setRepairTarget()));
    actionRow->addWidget(m_setTargetButton);

    m_unlockButton = makeButton(QString::fromLatin1("Unlock"), left);
    m_unlockButton->setEnabled(false);
    connect(m_unlockButton, SIGNAL(clicked()), this, SLOT(runUnlock()));
    actionRow->addWidget(m_unlockButton);
    // Host Maintenance and Make Default moved to the protected-host card at
    // the top of the page (modern parity); this row keeps the repair-target
    // actions and the authorization affordance follows.

    QHBoxLayout *authRow = new QHBoxLayout(leftLayout);
    authRow->setSpacing(6);
    m_authStatusLabel = new QLabel(left);
    m_authStatusLabel->setTextFormat(Qt::PlainText);
    m_authStatusLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    // The label may shrink to zero width (its text wraps within the space the
    // row leaves it); the row's minimum stays the buttons only.
    m_authStatusLabel->setSizePolicy(
        QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred, 1, 0, TRUE));
    authRow->addWidget(m_authStatusLabel, 1);

    m_authorizeButton = makeButton(QString::fromLatin1("Authorize"), left);
    m_authorizeButton->setEnabled(false);
    QToolTip::add(m_authorizeButton, QString::fromLatin1(
        "Establish the privileged helper session for the current scope now "
        "instead of waiting for the next privileged action."));
    connect(m_authorizeButton, SIGNAL(clicked()), this, SLOT(authorizeNow()));
    authRow->addWidget(m_authorizeButton, 0, Qt::AlignVCenter);

    // Cycle 13: the committed-target summary keeps the row's right edge even
    // when the Authorize affordance is hidden (the stretch lives before the
    // label, independent of the deferred state).
    authRow->addStretch(1);
    m_targetSummary = new QLabel(QString::fromLatin1("Committed target: none"), left);
    m_targetSummary->setTextFormat(Qt::PlainText);
    m_targetSummary->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    authRow->addWidget(m_targetSummary, 0, Qt::AlignRight | Qt::AlignVCenter);

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
    // The details pane keeps an explicit floor: Field (120px) + Value (220px)
    // plus the frame/layout margins must stay fully visible at 1024x768, and
    // the "Selected drive details" section title must never clip. The left
    // pane absorbs any extra width (stretch factor below).
    right->setMinimumWidth(372);
    QVBoxLayout *rightLayout = new QVBoxLayout(right, 0, 6);

    QGroupBox *details = new QGroupBox(right);
    QToolTip::add(details, QString::fromLatin1(
        "Read-only inventory plus helper-confirmed facts; mirrors the modern "
        "Qt6 Selected drive details panel."));
    QVBoxLayout *detailsLayout = new QVBoxLayout(details, 8, 4);
    m_detailsPaneTitle = makeSectionTitle(
        QString::fromLatin1("Selected drive details"), details);
    detailsLayout->addWidget(m_detailsPaneTitle);
    m_detailList = new QListView(details);
    addListViewColumn(m_detailList, QString::fromLatin1("Field"), 120);
    addListViewColumn(m_detailList, QString::fromLatin1("Value"), 220);
    m_detailList->setAllColumnsShowFocus(true);
    m_detailList->setResizeMode(QListView::LastColumn);
    // The documented details field order (Drive, Detected target, Model/label,
    // ...) must not be alphabetized by Qt3's default first-column sorting.
    m_detailList->setSorting(-1);
    m_detailList->setMinimumHeight(120);
    m_detailList->setMinimumWidth(340);
    detailsLayout->addWidget(m_detailList, 1);
    rightLayout->addWidget(details, 1);

    registerGroupBox(unlock);
    registerGroupBox(details);

    QValueList<int> sizes;
    sizes.append(600);
    sizes.append(372);
    splitter->setSizes(sizes);
    // The left inventory pane absorbs the splitter's extra width; the details
    // pane keeps its 372px floor (its explicit minimum width is never broken
    // by QSplitter, so the Field/Value columns and the section title stay
    // fully visible on Etch's default sizing path). The two-row action area
    // caps the left pane minimum near the button width (the status labels
    // shrink to zero), so the splitter's own minimum (left + 372 + handle)
    // stays inside the 1004px page bound at 1024x768 and the splitter can
    // never grow past the window.
    left->setMinimumWidth(400);
    splitter->setResizeMode(left, QSplitter::Stretch);
    splitter->setResizeMode(right, QSplitter::KeepSize);
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
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_diagHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, QString::fromLatin1("Diagnostics"), QString::fromLatin1(
        "Run All runs every available read-only diagnostic for the current "
        "scope; selecting a check runs it alone. Diagnostics are read-only "
        "and are the only evidence source for the gated repair actions.")));
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
    m_rawView->setMinimumHeight(80);
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

    // Cycle 9: the Selected diagnostic pane keeps usable floors when the
    // window shrinks — the results view needs its minimum height, the pane a
    // minimum width that fits the Copy/Save Results row, and the checks list
    // keeps its size while the detail pane absorbs the rest, so the results
    // buttons row never gets clipped.
    detail->setMinimumWidth(360);
    m_rawView->setMinimumWidth(200);
    splitter->setResizeMode(checks, QSplitter::KeepSize);
    splitter->setResizeMode(detail, QSplitter::Stretch);

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
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_repairHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(content, QString::fromLatin1("Repair"), QString::fromLatin1(
        "The Full Repair plan runs the selected legacy stages in order "
        "through the guarded helper; the individual tools run one stage at a "
        "time. Every action stays disabled until the cached capability lines "
        "say available and the helper keeps its runtime preflights.")));
    headingRow->addStretch();
    m_repairScopeLabel = new QLabel(QString::fromLatin1("Target: none selected"), content);
    m_repairScopeLabel->setTextFormat(Qt::PlainText);
    m_repairScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_repairScopeLabel, 0, Qt::AlignVCenter);

    m_planParagraph = new QLabel(QString::fromLatin1(kRepairPlanParagraph), content);
    m_planParagraph->setTextFormat(Qt::PlainText);
    m_planParagraph->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_planParagraph);

    // Modern Repair page: the Full Repair plan frame and the individual-tools
    // splitter live in one draggable vertical splitter, so the user can
    // resize the top and bottom panes. The page keeps its Qt3 QScrollView.
    QSplitter *vertical = new QSplitter(Qt::Vertical, content);
    vertical->setChildrenCollapsible(false);
    m_repairVerticalSplitter = vertical;

    // ---- Full Repair plan section -------------------------------------------
    QGroupBox *planBox = new QGroupBox(vertical);
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
    // Cycle 15: the plan actions live on their own right-aligned row so
    // Configure Plan... / Run Full Repair never clip at the minimum width
    // (the stretch absorbs the leftover instead of pushing the buttons out).
    QHBoxLayout *planButtons = new QHBoxLayout(planLayout);
    planButtons->setSpacing(6);
    planButtons->addStretch(1);
    m_configurePlanButton = makeButton(QString::fromLatin1("Configure Plan..."), planBox);
    QToolTip::add(m_configurePlanButton, QString::fromLatin1(
        "Open Settings to choose which Full Repair stages are part of the plan."));
    connect(m_configurePlanButton, SIGNAL(clicked()), this, SLOT(configurePlan()));
    planButtons->addWidget(m_configurePlanButton);
    m_runFullRepairButton = makeButton(QString::fromLatin1("Run Full Repair"), planBox);
    m_runFullRepairButton->setEnabled(false);
    QToolTip::add(m_runFullRepairButton, QString::fromLatin1(
        "Select a repair drive, or choose Host Maintenance on the protected "
        "running-host card."));
    connect(m_runFullRepairButton, SIGNAL(clicked()), this, SLOT(runFullRepair()));
    planButtons->addWidget(m_runFullRepairButton);

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
    // The plan frame floor keeps the header, readiness hint and a few stage
    // rows visible when the user drags the vertical divider all the way up.
    planBox->setMinimumHeight(150);

    // ---- Individual tools list + Selected tool pane -------------------------
    QSplitter *splitter = new QSplitter(Qt::Horizontal, vertical);
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
    // Cycle 14 loop: Qt3's setSorting(true) re-sorts immediately (breaking
    // the curated order), so header-click sorting is manual: the curated
    // insertion order stays the default and clicking a header column sorts
    // asc/desc with the Selected tool pane refreshed afterwards.
    if (m_toolList->header()) {
        connect(m_toolList->header(), SIGNAL(clicked(int)),
                this, SLOT(toolListHeaderClicked(int)));
    }
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
    // Cycle 15 loop: the title keeps its Expanding fill (the row has no
    // stretch item, so the label owns the space up to the Run button), may
    // shrink to zero so the run button (for example "Reconcile Boot Stack")
    // never clips at the minimum window width, and claims at most the
    // selected-tool pane's header slot (the maximum width bounds the claim
    // even when the button reports a stale size hint).
    m_toolTitle->setMinimumWidth(0);
    m_toolTitle->setMaximumWidth(380);
    detailHeader->addWidget(m_toolTitle, 0, Qt::AlignVCenter);
    // The smoke asserts the fill against the pane minus the run button and
    // the layout margins (pane - button - 30).
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

    QValueList<int> verticalSizes;
    verticalSizes.append(190);
    verticalSizes.append(340);
    vertical->setSizes(verticalSizes);
    layout->addWidget(vertical, 0);

    // Cycle 13: no legacy-only elevation section (modern has none); the
    // elevation state stays visible in Settings (privileged-authorization
    // support) and the status bar, and the deferred Authorize affordance
    // lives on the Systems page. The internal elevation re-resolution used
    // by the Authorize flow is unchanged.

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
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_chrootHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, QString::fromLatin1("Chroot shell"), QString::fromLatin1(
        "Offline commands run one at a time in a fresh chroot and cannot "
        "answer interactive prompts (apt-get -y upgrade works). Host-shell "
        "commands run directly on the running host. The helper's probe lines "
        "gate the command field; the exact reason appears in the tooltip.")));
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

    // Modern heading row (buildFileCopyPage parity): the File copy section
    // title + (i), then the live scope label, then the two primary actions
    // share the page-title row; both actions stay disabled until the
    // direction and scope are ready.
    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_fileCopyHeading = makeSectionTitle(QString::fromLatin1("File copy"), page);
    headingRow->addWidget(m_fileCopyHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_fileCopyHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, QString::fromLatin1("File copy"), QString::fromLatin1(
        "Copy and verify files in either direction through the guarded "
        "helper (cp -a plus ownership restoration and a per-file "
        "byte-compare). The helper's file-copy probe gates the controls and "
        "keeps the direction and path containment checks.")));
    headingRow->addStretch();
    m_fileCopyScopeLabel = new QLabel(QString::fromLatin1("Target: none selected"), page);
    m_fileCopyScopeLabel->setTextFormat(Qt::PlainText);
    m_fileCopyScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_fileCopyScopeLabel, 1);
    m_fileCopyPreviewButton = makeButton(QString::fromLatin1("Preview Changes"), page);
    m_fileCopyPreviewButton->setEnabled(false);
    QToolTip::add(m_fileCopyPreviewButton, QString::fromLatin1(
        "Run a copy dry-run through the guarded helper. No files are changed."));
    connect(m_fileCopyPreviewButton, SIGNAL(clicked()), this, SLOT(fileCopyPreview()));
    headingRow->addWidget(m_fileCopyPreviewButton, 0, Qt::AlignVCenter);
    m_fileCopyRunButton = makeButton(QString::fromLatin1("Copy and Verify"), page);
    m_fileCopyRunButton->setEnabled(false);
    QToolTip::add(m_fileCopyRunButton, QString::fromLatin1(
        "Copy staged items and verify the result. Existing destination names "
        "are overwritten when source content differs; unrelated destination "
        "files are never deleted."));
    connect(m_fileCopyRunButton, SIGNAL(clicked()), this, SLOT(fileCopyRun()));
    headingRow->addWidget(m_fileCopyRunButton, 0, Qt::AlignVCenter);

    // Direction row (modern parity: Host -> Repair / Repair -> Host). The
    // sub-layout is constructed WITHOUT a parent and added via addLayout(),
    // matching the existing-tab pattern: a layout constructed with a parent
    // layout is added automatically at construction, so an explicit
    // addLayout() would parent it twice ("QLayout::addChildLayout: layout
    // already has a parent") and double-free it at shutdown.
    QHBoxLayout *directionRow = new QHBoxLayout();
    directionRow->setSpacing(6);
    directionRow->addWidget(new QLabel(QString::fromLatin1("Direction:"), page));
    m_fileCopyDirectionCombo = new QComboBox(page);
    m_fileCopyDirectionCombo->insertItem(QString::fromUtf8("Host \xE2\x86\x92 Repair"));
    m_fileCopyDirectionCombo->insertItem(QString::fromUtf8("Repair \xE2\x86\x92 Host"));
    m_fileCopyDirectionCombo->setEnabled(false);
    QToolTip::add(m_fileCopyDirectionCombo, QString::fromLatin1(
        "Choose which system supplies the source files and which system "
        "receives them."));
    connect(m_fileCopyDirectionCombo, SIGNAL(activated(int)), this, SLOT(fileCopyDirectionChanged()));
    directionRow->addWidget(m_fileCopyDirectionCombo);
    directionRow->addStretch();
    layout->addLayout(directionRow);

    // Cycle 14: the vertical splitter owns its panes from construction, so
    // the three groups are always visible inside them (the cycle-13 wrap
    // created the panes after the groups and the reparenting hid them on
    // Etch).
    QSplitter *fileCopySplitter = new QSplitter(Qt::Vertical, page);
    fileCopySplitter->setChildrenCollapsible(false);
    QWidget *sourcePane = new QWidget(fileCopySplitter);
    QVBoxLayout *sourcePaneLayout = new QVBoxLayout(sourcePane, 0, 0);
    QWidget *lowerPane = new QWidget(fileCopySplitter);
    QVBoxLayout *lowerPaneLayout = new QVBoxLayout(lowerPane, 0, 6);

    // Group 1: staged sources with the Add/Remove/Clear button row. The
    // title adapts per direction (modern updateFileCopyDirection).
    m_fileCopySourceGroup = new QGroupBox(sourcePane);
    QVBoxLayout *sourceGroupLayout = new QVBoxLayout(m_fileCopySourceGroup, 8, 4);
    m_fileCopySourceTitle = makeSectionTitle(
        QString::fromLatin1("1. Select source files or folders from this host"),
        m_fileCopySourceGroup);
    // The group-1 title is the longest direction-adapted title: make it
    // wrap-capable (WordBreak + no minimum width) so a narrow details-less
    // pane can never clip it; the smoke's clipping gates exempt wrapping
    // section titles from the single-line width check.
    m_fileCopySourceTitle->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    m_fileCopySourceTitle->setMinimumWidth(0);
    sourceGroupLayout->addWidget(m_fileCopySourceTitle);
    m_fileCopySourceList = new QListView(m_fileCopySourceGroup);
    addListViewColumn(m_fileCopySourceList, QString::fromLatin1("Source"), 150);
    m_fileCopySourceList->setAllColumnsShowFocus(true);
    m_fileCopySourceList->setResizeMode(QListView::LastColumn);
    m_fileCopySourceList->setSorting(-1);
    m_fileCopySourceList->setEnabled(false);
    m_fileCopySourceList->setMinimumHeight(80);
    QToolTip::add(m_fileCopySourceList, QString::fromLatin1(
        "Files and folders staged for the verified copy. The legacy backend "
        "copies with cp -a and restores ownership with chown --reference; "
        "every regular file is byte-compared after the copy."));
    sourceGroupLayout->addWidget(m_fileCopySourceList, 1);
    QHBoxLayout *sourceButtons = new QHBoxLayout(sourceGroupLayout);
    sourceButtons->setSpacing(6);
    m_fileCopyAddFilesButton = makeButton(QString::fromLatin1("Add Files..."), m_fileCopySourceGroup);
    m_fileCopyAddFilesButton->setEnabled(false);
    connect(m_fileCopyAddFilesButton, SIGNAL(clicked()), this, SLOT(fileCopyAddFiles()));
    sourceButtons->addWidget(m_fileCopyAddFilesButton);
    m_fileCopyAddFolderButton = makeButton(QString::fromLatin1("Add Folder..."), m_fileCopySourceGroup);
    m_fileCopyAddFolderButton->setEnabled(false);
    connect(m_fileCopyAddFolderButton, SIGNAL(clicked()), this, SLOT(fileCopyAddFolder()));
    sourceButtons->addWidget(m_fileCopyAddFolderButton);
    m_fileCopyRemoveButton = makeButton(QString::fromLatin1("Remove"), m_fileCopySourceGroup);
    m_fileCopyRemoveButton->setEnabled(false);
    connect(m_fileCopyRemoveButton, SIGNAL(clicked()), this, SLOT(fileCopyRemoveSelected()));
    sourceButtons->addWidget(m_fileCopyRemoveButton);
    sourceButtons->addStretch();
    m_fileCopyClearButton = makeButton(QString::fromLatin1("Clear"), m_fileCopySourceGroup);
    m_fileCopyClearButton->setEnabled(false);
    QToolTip::add(m_fileCopyClearButton, QString::fromLatin1(
        "Clear the staged source list (nothing is copied or deleted)."));
    connect(m_fileCopyClearButton, SIGNAL(clicked()), this, SLOT(fileCopyClearStaging()));
    sourceButtons->addWidget(m_fileCopyClearButton);
    registerGroupBox(m_fileCopySourceGroup);
    sourcePaneLayout->addWidget(m_fileCopySourceGroup);

    // Group 2: destination with the Browse Target Folders... action.
    m_fileCopyDestinationGroup = new QGroupBox(lowerPane);
    QVBoxLayout *destinationLayout = new QVBoxLayout(m_fileCopyDestinationGroup, 8, 4);
    m_fileCopyDestinationTitle = makeSectionTitle(
        QString::fromLatin1("2. Choose destination in repaired system"),
        m_fileCopyDestinationGroup);
    destinationLayout->addWidget(m_fileCopyDestinationTitle);
    QHBoxLayout *destinationRow = new QHBoxLayout(destinationLayout);
    destinationRow->setSpacing(6);
    m_fileCopyDestinationEdit = new QLineEdit(m_fileCopyDestinationGroup);
    m_fileCopyDestinationEdit->setEnabled(false);
    m_fileCopyDestinationEdit->setText(QString::fromLatin1(
        "unavailable: see the helper's Legacy feature file-copy: probe reason above"));
    QToolTip::add(m_fileCopyDestinationEdit, QString::fromLatin1(
        "An absolute path inside the selected repair system (Host to Repair) "
        "or on the running host (Repair to Host)."));
    destinationRow->addWidget(m_fileCopyDestinationEdit, 1);
    m_fileCopyBrowseButton = makeButton(QString::fromLatin1("Browse Target Folders..."), m_fileCopyDestinationGroup);
    m_fileCopyBrowseButton->setEnabled(false);
    QToolTip::add(m_fileCopyBrowseButton, QString::fromLatin1(
        "Browse the selected repair system through the helper's temporary "
        "read-only mounts and choose an absolute destination path. No target "
        "files are changed while browsing."));
    connect(m_fileCopyBrowseButton, SIGNAL(clicked()), this, SLOT(fileCopyBrowse()));
    destinationRow->addWidget(m_fileCopyBrowseButton);
    registerGroupBox(m_fileCopyDestinationGroup);
    lowerPaneLayout->addWidget(m_fileCopyDestinationGroup);

    // Group 3: ownership and copy policy (the legacy equivalent set).
    m_fileCopyOptionsGroup = new QGroupBox(lowerPane);
    QVBoxLayout *optionsLayout = new QVBoxLayout(m_fileCopyOptionsGroup, 8, 4);
    m_fileCopyOptionsTitle = makeSectionTitle(
        QString::fromLatin1("3. Ownership and copy policy"),
        m_fileCopyOptionsGroup);
    optionsLayout->addWidget(m_fileCopyOptionsTitle);
    QHBoxLayout *ownershipRow = new QHBoxLayout(optionsLayout);
    ownershipRow->setSpacing(6);
    ownershipRow->addWidget(new QLabel(QString::fromLatin1("Ownership:"), m_fileCopyOptionsGroup));
    m_fileCopyOwnershipCombo = new QComboBox(m_fileCopyOptionsGroup);
    m_fileCopyOwnershipCombo->insertItem(QString::fromLatin1(
        "Smart destination ownership (recommended)"));
    m_fileCopyOwnershipCombo->insertItem(QString::fromLatin1(
        "Preserve source numeric UID/GID"));
    m_fileCopyOwnershipCombo->setEnabled(false);
    QToolTip::add(m_fileCopyOwnershipCombo, QString::fromLatin1(
        "Smart mode validates UID/GID identity mapping across the two systems "
        "and falls back to the destination-directory owner when the same "
        "numeric ID means a different account (the legacy backend implements "
        "it with chown --reference)."));
    ownershipRow->addWidget(m_fileCopyOwnershipCombo, 1);
    registerGroupBox(m_fileCopyOptionsGroup);
    lowerPaneLayout->addWidget(m_fileCopyOptionsGroup);

    sourcePane->setMinimumHeight(120);
    lowerPane->setMinimumHeight(140);
    QValueList<int> fileCopySizes;
    fileCopySizes.append(190);
    fileCopySizes.append(260);
    fileCopySplitter->setSizes(fileCopySizes);
    layout->addWidget(fileCopySplitter, 1);

    return page;
}

QWidget *LegacyMainWindow::buildLogTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 4);

    QHBoxLayout *logsHeadingRow = new QHBoxLayout(layout);
    logsHeadingRow->setSpacing(6);
    // Cycle 13: the page heading is "Application log" (modern parity); the
    // tab keeps the "Logs" label.
    m_logsHeading = makeSectionTitle(QString::fromLatin1("Application log"), page);
    logsHeadingRow->addWidget(m_logsHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_logsHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    logsHeadingRow->addWidget(makeHelpButton(page, QString::fromLatin1("Application log"), QString::fromLatin1(
        "The complete session register; Save As... writes every entry even "
        "while a filter hides lines. If a writable system share mount exists "
        "at /host, Save As... starts there; otherwise the log directory is "
        "the fallback. Prior session files are listed read-only.")));
    logsHeadingRow->addStretch();

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    QGroupBox *sessions = new QGroupBox(splitter);
    // Cycle 14: the session pane keeps a floor so Add Note and Refresh are
    // never cut off at the default window size.
    sessions->setMinimumWidth(240);
    QVBoxLayout *sessionLayout = new QVBoxLayout(sessions, 8, 4);
    sessionLayout->addWidget(makeSectionTitle(QString::fromLatin1("Session logs"), sessions));
    m_sessionLogList = new QListView(sessions);
    addListViewColumn(m_sessionLogList, QString::fromLatin1("Session"), 150);
    m_sessionLogList->setAllColumnsShowFocus(true);
    m_sessionLogList->setResizeMode(QListView::LastColumn);
    m_sessionLogList->setMinimumHeight(80);
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
    // Cycle 14: the log pane keeps a readability floor; the session pane
    // takes the leftover.
    applicationLog->setMinimumWidth(480);
    QVBoxLayout *logLayout = new QVBoxLayout(applicationLog, 8, 4);

    // Modern top row: the "Application log" section title on the left and the
    // Save As... / Clear Register actions on the right, above the
    // search/filter row (the page heading "Logs" stays above the splitter).
    QHBoxLayout *logHeader = new QHBoxLayout(logLayout);
    logHeader->setSpacing(4);
    logHeader->addWidget(makeSectionTitle(QString::fromLatin1("Application log"), applicationLog));
    logHeader->addStretch();
    QPushButton *save = makeButton(QString::fromLatin1("Save As..."), applicationLog);
    QToolTip::add(save, QString::fromLatin1(
        "Save the complete session log (all entries, not just the current filter)."));
    connect(save, SIGNAL(clicked()), this, SLOT(saveLog()));
    logHeader->addWidget(save);
    QPushButton *clear = makeButton(QString::fromLatin1("Clear Register"), applicationLog);
    QToolTip::add(clear, QString::fromLatin1(
        "Clear the live register and view; prior session files are never "
        "modified."));
    connect(clear, SIGNAL(clicked()), this, SLOT(clearLog()));
    logHeader->addWidget(clear);

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
    // 1:1 with the modern combo: All entries / Diagnostics / Repairs / the
    // three workflows / the 16 diagnostic section titles. Qt3's QComboBox
    // cannot carry per-item data, so the index semantics live in
    // logFilterSpecs plus the diagnosticSpecs order below.
    for (int i = 0; i < logFilterSpecCount; ++i) {
        m_logFilterCombo->insertItem(QString::fromLatin1(logFilterSpecs[i].title));
    }
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        m_logFilterCombo->insertItem(QString::fromLatin1(diagnosticSpecs[i].title));
    }
    QToolTip::add(m_logFilterCombo, QString::fromLatin1(
        "Filter the visible log by entry kind. Selecting a diagnostic section "
        "shows the lines captured for that section; a workflow filter such as "
        "File system repair or Package repair shows its mapped repair lines "
        "(File copy has no lines in this frontend). Save As always writes "
        "every entry."));
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
    // Cycle 14: long lines wrap at the widget width with no horizontal
    // scrollbar while wrapping is on (the default); turning wrap off
    // re-enables the scrollbar.
    m_logView->setHScrollBarMode(m_logWrapEnabled ? QScrollView::AlwaysOff
                                                 : QScrollView::Auto);
    m_logView->setMinimumHeight(80);
    logLayout->addWidget(m_logView, 1);

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

    QHBoxLayout *settingsHeadingRow = new QHBoxLayout(pageLayout);
    settingsHeadingRow->setSpacing(6);
    m_settingsHeading = makeSectionTitle(QString::fromLatin1("Settings"), page);
    settingsHeadingRow->addWidget(m_settingsHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_settingsHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    settingsHeadingRow->addWidget(makeHelpButton(page, QString::fromLatin1("Settings"), QString::fromLatin1(
        "Settings are stored per user under ~/.qt/, one file per settings "
        "group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved "
        "immediately on every change and on close. Launch the GUI as the "
        "same user to keep your overrides; a GUI started as root keeps its "
        "own copies.")));
    settingsHeadingRow->addStretch();

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

    // Cycle 13: the per-user settings note moved into the Settings (i) popup.

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

    // Modern Host capabilities and dependencies: the section title, the three
    // summary labels (Distribution / Package manager family / an adapted
    // authorization-support line in place of KAuth), the 6-column probe table
    // mirroring CapabilityChecker::scanHost plus the one legacy-specific row,
    // and the Refresh Capabilities / disabled Install Missing Support...
    // actions. Every probe is a read-only PATH search (never executed).
    QGroupBox *capability = new QGroupBox(content);
    QVBoxLayout *capabilityLayout = new QVBoxLayout(capability, 6, 3);
    capabilityLayout->addWidget(
        makeSectionTitle(QString::fromLatin1("Host capabilities and dependencies"), capability));
    QGridLayout *identityGrid = new QGridLayout(capabilityLayout, 3, 2, 4);
    identityGrid->setColStretch(1, 1);
    m_capDistributionLabel = new QLabel(capability);
    m_capPackageManagerLabel = new QLabel(capability);
    m_capAuthLabel = new QLabel(capability);
    QLabel *capValues[] = {
        m_capDistributionLabel, m_capPackageManagerLabel, m_capAuthLabel
    };
    const char *capNames[] = {
        "Distribution:", "Package manager family:",
        "Privileged authorization support:"
    };
    for (int i = 0; i < 3; ++i) {
        capValues[i]->setTextFormat(Qt::PlainText);
        capValues[i]->setAlignment(Qt::WordBreak | Qt::AlignLeft);
        identityGrid->addWidget(new QLabel(QString::fromLatin1(capNames[i]), capability), i, 0);
        identityGrid->addWidget(capValues[i], i, 1);
    }

    // Qt3 has no QTableWidget; the QTable fills the same 6-column
    // Feature|Command|Scope|Status|Suggested package|Notes contract. The last
    // column stretches to the frame edge, the table is read-only and the
    // rows are populated by refreshCapabilities().
    m_capabilityTable = new QTable(0, 6, capability);
    m_capabilityTable->setLeftMargin(0);
    // Full-row selection (modern parity): clicking any cell highlights the
    // whole row; the selected missing capability is the conceptual guard the
    // disabled Install Missing Support... button documents.  The first row is
    // selected by refreshCapabilities() after every repopulation.
    m_capabilityTable->setSelectionMode(QTable::SingleRow);
    m_capabilityTable->setReadOnly(true);
    m_capabilityTable->setMinimumHeight(230);
    // Cycle 13: header-click sorting on the six columns; refreshCapabilities
    // still restores the first-row selection after every repopulation.
    m_capabilityTable->setSorting(true);
    const char *capabilityHeaders[] = {
        "Feature", "Command", "Scope", "Status", "Suggested package", "Notes"
    };
    for (int column = 0; column < 6; ++column) {
        m_capabilityTable->horizontalHeader()->setLabel(
            column, QString::fromLatin1(capabilityHeaders[column]));
    }
    m_capabilityTable->setColumnWidth(0, 150);
    m_capabilityTable->setColumnWidth(1, 150);
    m_capabilityTable->setColumnWidth(2, 90);
    m_capabilityTable->setColumnWidth(3, 100);
    m_capabilityTable->setColumnWidth(4, 150);
    m_capabilityTable->setColumnStretchable(5, true);
    capabilityLayout->addWidget(m_capabilityTable);

    QHBoxLayout *capabilityButtons = new QHBoxLayout(capabilityLayout);
    capabilityButtons->setSpacing(4);
    m_refreshCapabilitiesButton = makeButton(
        QString::fromLatin1("Refresh Capabilities"), capability);
    QToolTip::add(m_refreshCapabilitiesButton, QString::fromLatin1(
        "Re-run the read-only host capability probes (a PATH search, nothing "
        "is executed) and refresh the distribution summary."));
    connect(m_refreshCapabilitiesButton, SIGNAL(clicked()), this, SLOT(refreshCapabilities()));
    capabilityButtons->addWidget(m_refreshCapabilitiesButton);
    capabilityButtons->addStretch();
    m_installSupportButton = makeButton(
        QString::fromLatin1("Install Missing Support..."), capability);
    m_installSupportButton->setEnabled(false);
    QToolTip::add(m_installSupportButton, QString::fromLatin1(
        "Automatic installation will require explicit package mapping and "
        "privilege authorization."));
    capabilityButtons->addWidget(m_installSupportButton);
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

void LegacyMainWindow::scanDevices()
{
    // The member slot shadows the namespace-level scanDevices(), so the
    // inventory function is called with its explicit namespace qualification.
    // The complete inventory is cached in m_rows/m_inventory; the visible list
    // is a filtered projection so a hidden row (device-discovery filter) can
    // never invalidate the selected/committed target state.
    // Cycle 14: the session unlock state survives a rescan while the
    // unlocked mapping still exists (leaving Host Maintenance must not lose
    // the already-unlocked target); it clears only when the mapping is gone.
    const QString priorUnlockedDisk = m_unlockedDisk;
    const QString priorUnlockedRoot = m_unlockedRoot;
    const QString priorUnlockedMapper = m_unlockedMapper;
    const QString priorUnlockedUuid = m_unlockedRootUuid;
    m_unlockedDisk = QString::null;
    m_unlockedRoot = QString::null;
    m_unlockedMapper = QString::null;
    m_unlockedRootUuid = QString::null;
    m_inventory = legacy::scanDevices();
    m_rows.clear();
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        m_rows.insert(fromStd(m_inventory[i].path), m_inventory[i]);
    }
    if (!priorUnlockedDisk.isEmpty() && !priorUnlockedRoot.isEmpty()
        && QFileInfo(priorUnlockedRoot).exists()) {
        m_unlockedDisk = priorUnlockedDisk;
        m_unlockedRoot = priorUnlockedRoot;
        m_unlockedMapper = priorUnlockedMapper;
        m_unlockedRootUuid = priorUnlockedUuid;
        injectUnlockedMapperRows(m_unlockedDisk, m_unlockedMapper,
                                 m_unlockedRoot, QString::null,
                                 m_unlockedRootUuid);
    }
    rebuildDeviceList();
    updateHostCard();
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

// Rebuilds the visible device tree from the cached inventory, applying the
// Settings device-discovery filters. Whole disks are the top-level items with
// their partitions/mappers as indented children (modern tree parity); the
// selected disk is re-selected when it is still visible, and a hidden drive
// hides its children while a visible drive with a hidden child shows the
// drive only. The selection/commit state stays intact when a row is hidden.
void LegacyMainWindow::rebuildDeviceList()
{
    if (!m_deviceList) {
        return;
    }
    const QString previousDisk = m_selectedDisk;
    m_deviceList->clear();

    // Pass 1: top-level disk items (inventory order, expanded).
    QMap<QString, QListViewItem *> diskItems;
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        const DeviceRow &row = m_inventory[i];
        if (!row.disk || !deviceRowVisible(row)) {
            continue;
        }
        // Cycle 15: the protected running host never appears in the
        // candidate tree (the host card above shows it); Host Maintenance
        // behavior is unchanged.
        if (!runningHostDisk().isEmpty()
            && fromStd(row.path) == runningHostDisk()) {
            continue;
        }
        const QString fsLabel = diskFilesystemSummary(row);
        const QString type = row.optical ? QString::fromLatin1("optical")
                                         : QString::fromLatin1("disk");
        QListViewItem *item = new QListViewItem(m_deviceList,
                                                fromStd(row.path),
                                                fromStd(row.size), type,
                                                fsLabel);
        // Cycle 9: the tree starts collapsed (disks rolled up; expand with +).
        diskItems.insert(fromStd(row.path), item);
    }

    // Pass 2: partitions and mappers as children of their owning disk. A
    // non-disk row whose owning disk is not in the tree (floppy stubs like
    // /dev/fd0, empty-device rows like /dev/hdc, helper-reported orphans) is
    // not a repair target and is omitted from the tree; its inspection data
    // stays in the cached inventory for the details pane.
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        const DeviceRow &row = m_inventory[i];
        if (row.disk || !deviceRowVisible(row)) {
            continue;
        }
        QString type;
        if (row.mapper) {
            type = QString::fromLatin1("mapper");
        } else {
            type = QString::fromLatin1("partition");
        }
        const QString fsLabel = displayFsType(row);
        const QString owner = owningDiskFor(row);
        QMap<QString, QListViewItem *>::const_iterator parent =
            diskItems.find(owner);
        if (parent != diskItems.end()) {
            new QListViewItem(parent.data(), fromStd(row.path),
                              fromStd(row.size), type, fsLabel);
        }
        // else: top-level = row.disk only; the orphan stays out of the tree.
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
{    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
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

// Filesystem label for a whole-disk row: aggregate the disk's children so the
// row is informative but never blank — the primary Linux filesystem of the
// children plus " + LUKS" when an encrypted child exists (e.g. "ext3 + LUKS",
// "ext3", "unknown" when nothing is known).
QString LegacyMainWindow::diskFilesystemSummary(const DeviceRow &row) const
{
    const QString diskPath = fromStd(row.path);
    QString linuxFs;
    bool hasLuks = false;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &child = it.data();
        if (!rowBelongsToDisk(child, diskPath)) {
            continue;
        }
        if (child.encrypted) {
            hasLuks = true;
        }
        if (linuxFs.isEmpty()) {
            if (legacy::isLinuxFileSystemName(child.fstype)) {
                linuxFs = fromStd(child.fstype);
            } else if (legacy::isLinuxFileSystemName(child.probedFstype)) {
                linuxFs = fromStd(child.probedFstype);
            }
        }
    }
    if (linuxFs.isEmpty()) {
        return hasLuks ? QString::fromLatin1("LUKS")
                       : QString::fromLatin1("unknown");
    }
    if (hasLuks) {
        return linuxFs + QString::fromLatin1(" + LUKS");
    }
    return linuxFs;
}

// Modern parity: selecting a drive row inspects the drive and resolves its
// best Linux root automatically; selecting a partition/mapper row inspects
// that component while keeping its owning drive as the target candidate.
void LegacyMainWindow::deviceSelectionChanged()
{
    if (m_updatingSelection) {
        return;
    }
    // Cycle 13: any drive-row selection restores the normal per-drive pane.
    m_inspectingHostDetails = false;
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
    // Cycle 15 (modern commit parity): a blank or non-Linux data disk
    // commits its physical path (the resolved root falls back to the disk
    // itself) so it can be inspected; diagnostics against it fail closed
    // with the helper's exact reason. A locked LUKS container without a
    // visible Linux filesystem stays refused.
    if (!autoResolvedLuks(disk).isEmpty()
        && autoResolvedRoot(disk).isEmpty()) {
        QMessageBox::information(
            this, QString::fromLatin1("Unlock or select a Linux system first"),
            QString::fromLatin1(
                "This encrypted drive has no visible Linux filesystem yet. "
                "Use Unlock, refresh devices, and select the target after its "
                "Linux root is detected."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QString root = selectedRoot();
    if (root.isEmpty()) {
        root = disk;
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
    QString statusText = QString::fromLatin1("Repair drive selected: %1")
                             .arg(m_committedDisk);
    if (m_committedRoot != m_committedDisk) {
        statusText += QString::fromLatin1("; best detected system component: %1")
                          .arg(m_committedRoot);
    }
    statusText += QString::fromLatin1(". No mount or repair action was performed.");
    statusBar()->message(statusText, 4000);
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
    // Gap #21: committing a target changes the scope identity, so the cached
    // diagnostics no longer describe the selected scope. With auto-refresh on
    // and the just-authorized session active, schedule one quiet Run All for
    // the new scope (never opens an authorization prompt by itself; the
    // skip/pending reasons are logged like the unlock/config-write paths).
    maybeAutoRefreshDiagnostics(QString::fromLatin1(
        "committing the repair target changed the diagnostics scope"));
}

// B7-7: Make Default for a GRUB-legacy BIOS host — the helper verifies
// /boot/grub/menu.lst, sets `default <N>` to the canonical installed kernel
// entry with a backup and rollback, and never writes a boot sector.
void LegacyMainWindow::makeDefault()
{
    if (m_running) {
        return;
    }
    if (!hostScope()) {
        QMessageBox::warning(this, QString::fromLatin1("Make Default unavailable"),
                             QString::fromLatin1(
                                 "Make Default is a running-host action on this "
                                 "frontend; enter Host Maintenance first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::string featureReason;
    if (!m_model.legacyFeatureAvailable("host-default", toStd(identity()),
                                        &featureReason)) {
        QMessageBox::warning(this, QString::fromLatin1("Make Default unavailable"),
                             fromStd(featureReason),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!administratorSessionActive()) {
        QMessageBox::warning(this, QString::fromLatin1("Administrator authorization required"),
                             QString::fromLatin1(
                                 "No administrator session is active; press "
                                 "Authorize first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool answer = confirmWrapped(
        this, QString::fromLatin1("Make Default"),
        QString::fromLatin1(
            "Make the canonical installed kernel entry the default GRUB-legacy "
            "boot entry on the running host?\n\n"
            "The helper verifies /boot/grub/menu.lst, sets the `default <N>` "
            "directive to the canonical entry, backs the menu up first and "
            "restores it on any failure."));
    if (!answer) {
        return;
    }
    QStringList args;
    args << QString::fromLatin1("host-default")
         << selectedDisk() << selectedRoot();
    const bool started = startCommand(args, false,
                                      QString::fromLatin1("Make Default"),
                                      false, false, false, false,
                                      QString::null,
                                      QString::fromLatin1("host-default"));
    if (started && !m_smokeMode) {
        showRepairResultDialog(QString::fromLatin1("Make Default"));
    }
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
    // Gap #21: entering Host Maintenance changes the scope identity, so the
    // cached diagnostics no longer describe the selected scope. With
    // auto-refresh on and the just-authorized session active, schedule one
    // quiet Run All for the running host (never opens an authorization prompt
    // by itself; the skip/pending reasons are logged).
    maybeAutoRefreshDiagnostics(QString::fromLatin1(
        "entering Host Maintenance changed the diagnostics scope"));
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
    // Cycle 11: after this session's successful unlock, the helper-confirmed
    // root component resolves even though the read-only inventory cannot see
    // the mapped LV's filesystem (the B7-1 locked-LUKS gate below would
    // otherwise keep Select Target disabled).
    if (m_unlockedDisk == disk && !m_unlockedRoot.isEmpty()
        && m_unlockedRoot.startsWith(QString::fromLatin1("/dev/"))) {
        return m_unlockedRoot;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;

    // Modern parity (B7-1): when the drive has a locked LUKS component (no
    // unlocked mapper child yet), the target root is not resolvable — do NOT
    // fall back to a plain partition candidate such as the unencrypted /boot,
    // which would commit an unusable component.  Select Target stays disabled
    // with the unlock-first reason until the unlock exposes the mapped Linux
    // root (after which autoResolvedLuks reports no locked candidate and the
    // mapper branch below resolves it).
    if (!autoResolvedLuks(disk).isEmpty()) {
        return QString::null;
    }

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
    // Whole-disk filesystem: the disk itself is a candidate only when its
    // own filesystem is Linux root-capable — a vfat share disk or a
    // blank/data disk must never become a committable target (Select Target
    // then stays disabled with the no-Linux-filesystem reason).
    const QMap<QString, DeviceRow>::const_iterator diskRow = m_rows.find(disk);
    if (diskRow != m_rows.end()) {
        const DeviceRow &own = diskRow.data();
        if (legacy::isLinuxFileSystemName(own.fstype)
            || legacy::isLinuxFileSystemName(own.probedFstype)) {
            return disk;
        }
    }
    return QString::null;
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
    // The combined run has no fixed section: the stream's `Diagnostic: <key>`
    // markers tag each section's lines.
    startCommand(args, true,
                 host ? QString::fromLatin1("host-diagnose all")
                      : QString::fromLatin1("diagnose all"),
                 false, false, false, quiet, QString::null, QString::null);
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
                     + key,
                 false, false, false, false, key, QString::null);
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
    // B5: the write transport is a private content file, never an argv
    // element, so the edited text can never surface in /proc/<pid>/cmdline,
    // helper logs or the session log. NUL stays rejected (the file transport
    // would carry it, so it is refused here explicitly), and the size cap is
    // the helper's 1 MiB file bound (raised from the old argv bound).
    if (edited.contains(QChar(0x0000))) {
        QMessageBox::warning(
            this, QString::fromLatin1("Configuration write refused"),
            QString::fromLatin1(
                "The edited content contains NUL bytes; the guarded write "
                "refuses it."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QByteArray contentBytes = edited.local8Bit();
    if (static_cast<int>(contentBytes.size()) > kConfigEditMaximumBytes) {
        QMessageBox::warning(
            this, QString::fromLatin1("File too large"),
            QString::fromLatin1(
                "The edited file is larger than 1 MiB. The guarded write "
                "refuses it; edit the file from a console instead."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool answer = confirmWrapped(
        this, QString::fromLatin1("Write target configuration"),
        QString::fromLatin1(
            "Write the edited contents to %1? This modifies the repair target "
            "and invalidates cached diagnostics.").arg(path),
        true);
    if (!answer) {
        return;
    }

    // The edited content travels through the private mode-600 content file
    // (the helper reads it, proves the ownership and never deletes it; the
    // GUI unlinks it on every completion path, including the signal
    // handlers).
    QString contentFilePath;
    if (!writeConfigContentFile(contentBytes, &contentFilePath)) {
        return;
    }
    m_configContentFilePath = contentFilePath;
    m_pendingConfigKey = key;
    m_pendingConfigPath = path;
    m_pendingConfigWrite = true;
    QStringList args;
    args << QString::fromLatin1("config-write") << selectedDisk() << selectedRoot()
         << key
         << QString::fromLatin1("--content-file") << contentFilePath
         << QString::fromLatin1("--content-owner")
         << QString::number(static_cast<unsigned long>(::getuid()));
    if (!startCommand(args, false, QString::fromLatin1("config-write %1").arg(path),
                      false, true)) {
        // startCommand failed before the runner started (no authorization
        // session, smoke elevation, ...); the secret file must not outlive
        // the refused attempt.
        discardConfigContentFile();
    }
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
    // Cycle 13 loop: Host Maintenance is a scope, not a lock — unlocking is
    // refused only when the selected drive IS the protected running host.
    if (!runningHostDisk().isEmpty()
        && selectedDisk() == runningHostDisk()) {
        QMessageBox::warning(this, QString::fromLatin1("Unlock not available"),
                             QString::fromLatin1(
                                 "The protected running host cannot be unlocked. "
                                 "Select an offline repair target to unlock."),
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
        const bool answer = confirmWrapped(
            this, QString::fromLatin1("Confirm LUKS unlock"),
            QString::fromLatin1(
                "Unlock %1 on %2?\n\n"
                "The helper opens a temporary device-mapper mapping with "
                "cryptsetup and keeps it open for this recovery session. The "
                "passphrase travels through a private keyfile and is never "
                "placed in command arguments or logs.")
                .arg(luks).arg(selectedDisk()));
        if (!answer) {
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

    // Qt 3.3.7's QProcess cannot deliver stdin reliably, so the passphrase
    // travels through a mode-600 keyfile argument (O_EXCL, in the GUI's log
    // directory, never in argv) that the helper reads, verifies and never
    // deletes — the GUI unlinks it on every completion path. The buffer is
    // wiped right after the write.
    QString keyfilePath;
    if (!writeUnlockKeyfile(secret, &keyfilePath)) {
        secret.fill('\0');
        secret = QByteArray();
        return;
    }
    secret.fill('\0');
    secret = QByteArray();

    m_unlockRetry = false;
    m_unlockDevice = luks;
    m_unlockDisk = selectedDisk();
    QStringList args;
    // Cycle 10: the GUI passes its own uid with the keyfile so the elevated
    // helper can accept the file under gksu/gksudo (no SUDO_UID) too.
    args << QString::fromLatin1("unlock") << selectedDisk() << luks
         << QString::fromLatin1("--key-file") << keyfilePath
         << QString::fromLatin1("--key-owner")
         << QString::number(static_cast<unsigned long>(::getuid()));
    m_unlockKeyfilePath = keyfilePath;
    startCommand(args, false, QString::fromLatin1("unlock %1").arg(luks), true);
}

// Shared secret-file writer (B5, generalized from the unlock keyfile path):
// write `data` to a mode-600, O_EXCL-created file named .<prefix><pid> in the
// dedicated 0700 `.keys` subdirectory of the GUI's log directory (never argv,
// never the session log). The subdirectory is created 0700 and re-tightened
// on every attempt, so a shared --log-dir cannot expose the file to other
// users. The created path is registered in the caller's termination-handler
// buffer (registeredPath) immediately, so a SIGTERM/SIGINT/SIGHUP always
// unlinks it before the GUI exits, even mid-write. Fails closed: an existing
// file, an unwritable directory or a partial write leaves nothing behind.
bool LegacyMainWindow::writeSecretFile(const QByteArray &data, const char *prefix,
                                       char *registeredPath,
                                       std::size_t registeredPathSize,
                                       QString *path)
{
    if (path) {
        *path = QString::null;
    }
    ensureLogDirectory();
    const QString keysDirectory = m_logDirectory + QString::fromLatin1("/.keys");
    if (::mkdir(keysDirectory.local8Bit().data(), 0700) != 0) {
        struct stat info;
        if (::stat(keysDirectory.local8Bit().data(), &info) != 0
            || !S_ISDIR(info.st_mode)) {
            return false;
        }
    }
    if (::chmod(keysDirectory.local8Bit().data(), 0700) != 0) {
        return false;
    }
    QString candidate = keysDirectory + QString::fromLatin1("/.")
        + QString::fromLatin1(prefix)
        + QString::number(static_cast<unsigned long>(::getpid()));
    const QByteArray candidateBytes = candidate.local8Bit();
    const int fd = ::open(candidateBytes.data(),
                          O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        return false;
    }
    // Register the created file with the termination handlers immediately
    // so a SIGTERM/SIGINT/SIGHUP always unlinks it before the GUI exits,
    // even mid-write.
    if (registeredPath && registeredPathSize > 0) {
        ::strncpy(registeredPath, candidateBytes.data(), registeredPathSize - 1);
        registeredPath[registeredPathSize - 1] = '\0';
    }
    const unsigned char *bytes =
        reinterpret_cast<const unsigned char *>(data.data());
    std::size_t remaining = static_cast<std::size_t>(data.size());
    while (remaining > 0) {
        const ssize_t written = ::write(fd, bytes, remaining);
        if (written <= 0) {
            ::close(fd);
            ::unlink(candidateBytes.data());
            if (registeredPath && registeredPathSize > 0) {
                registeredPath[0] = '\0';
            }
            return false;
        }
        bytes += written;
        remaining -= static_cast<std::size_t>(written);
    }
    ::close(fd);
    if (path) {
        *path = candidate;
    }
    return true;
}

// The unlock passphrase secret file: the dedicated .unlock-key-<pid> name in
// the same private .keys directory. Every failure path leaves nothing behind
// and the unlock is not started.
bool LegacyMainWindow::writeUnlockKeyfile(const QByteArray &secret, QString *path)
{
    if (writeSecretFile(secret, "unlock-key-",
                        gUnlockKeyfilePath, sizeof(gUnlockKeyfilePath), path)) {
        return true;
    }
    QMessageBox::warning(
        this, QString::fromLatin1("Unlock keyfile unavailable"),
        QString::fromLatin1(
            "The LUKS passphrase could not be written to a private keyfile in "
            "%1; the unlock was not started.")
            .arg(m_logDirectory),
        QMessageBox::Ok, QMessageBox::NoButton);
    return false;
}

// The config-write content file: the dedicated .config-content-<pid> name in
// the same private .keys directory. The helper never deletes the file; the
// GUI unlinks it on every completion path.
bool LegacyMainWindow::writeConfigContentFile(const QByteArray &content, QString *path)
{
    if (writeSecretFile(content, "config-content-",
                        gConfigContentFilePath, sizeof(gConfigContentFilePath),
                        path)) {
        return true;
    }
    QMessageBox::warning(
        this, QString::fromLatin1("Configuration write unavailable"),
        QString::fromLatin1(
            "The edited content could not be written to a private temporary "
            "file in %1; the write was not started.")
            .arg(m_logDirectory),
        QMessageBox::Ok, QMessageBox::NoButton);
    return false;
}

// Delete the unlock keyfile (best-effort) and clear the tracked path. Called
// from every completion and failure path, so the passphrase file never
// outlives the attempt.
void LegacyMainWindow::discardUnlockKeyfile()
{
    if (!m_unlockKeyfilePath.isEmpty()) {
        ::unlink(m_unlockKeyfilePath.local8Bit().data());
        m_unlockKeyfilePath = QString::null;
    }
    gUnlockKeyfilePath[0] = '\0';
}

// Delete the pending config-write content file (best-effort) and clear the
// tracked path. Called from every completion and failure path (the helper
// never deletes the caller's file), so the edited content file never
// outlives the attempt; the unlink is idempotent.
void LegacyMainWindow::discardConfigContentFile()
{
    if (!m_configContentFilePath.isEmpty()) {
        ::unlink(m_configContentFilePath.local8Bit().data());
        m_configContentFilePath = QString::null;
    }
    gConfigContentFilePath[0] = '\0';
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
        m_logView->setHScrollBarMode(enabled ? QScrollView::AlwaysOff
                                             : QScrollView::Auto);
    }
    if (m_resultView) {
        m_resultView->setWordWrap(enabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
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
    m_logEntries.clear();
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
    // Record the file identity before the confirmation, then re-check with
    // lstat() immediately before the unlink: only a regular file with the
    // same device/inode may be removed, so a path swapped for a symlink or
    // another file while the confirmation dialog is open is refused.
    struct stat earlier;
    if (::lstat(canonicalPath.local8Bit().data(), &earlier) != 0
        || !S_ISREG(earlier.st_mode)) {
        return;
    }
    const bool answer = confirmWrapped(
        this, QString::fromLatin1("Delete session log"),
        QString::fromLatin1("Delete %1 permanently? This cannot be undone.")
            .arg(fileName),
        true);
    if (!answer) {
        return;
    }
    struct stat current;
    if (::lstat(canonicalPath.local8Bit().data(), &current) != 0
        || !S_ISREG(current.st_mode)
        || current.st_dev != earlier.st_dev
        || current.st_ino != earlier.st_ino) {
        QMessageBox::warning(
            this, QString::fromLatin1("Delete session log"),
            QString::fromLatin1(
                "%1 changed while the confirmation was open; the delete was "
                "refused.").arg(fileName),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (::unlink(canonicalPath.local8Bit().data()) != 0) {
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
    if (legacySmokeSettingsIsolation()) {
        // --smoke-test is deterministic: persisted overrides are ignored and
        // the in-code defaults apply, so a user's ~/.qt/devicesrc|logsrc|
        // diagnosticsrc|repairrc overrides can never flip a smoke assertion,
        // and the smoke never reads or writes those files.
        applyLegacySettingsDefaults();
        return;
    }
    QSettings settings;
    // Qt 3.3.7 stores each settings group in its own per-user file under
    // ~/.qt/ named after the group, so the keys below live in
    // ~/.qt/devicesrc, ~/.qt/logsrc, ~/.qt/diagnosticsrc and ~/.qt/repairrc
    // (not one boot-bitchrc). The setPath organization/application pair
    // keeps the load and save sides identical; dpkg -r / dpkg -i reinstall
    // never touches these files.
    settings.setPath(QString::fromLatin1("boot-bitch"),
                     QString::fromLatin1("boot-repair"), QSettings::User);
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
    // Full Repair plan checkboxes (modern-compatible key names). The saved
    // preference is kept separately from the widget state so the availability
    // pass (updatePlanChecks) can force unavailable rows off without losing
    // the user's choice.
    m_planPreferences.assign(planSpecCount, false);
    for (int i = 0; i < planSpecCount; ++i) {
        m_planPreferences[i] = planSpecs[i].defaultChecked;
    }
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        const bool checked = settings.readBoolEntry(
            QString::fromLatin1(planSpecs[i].settingsKey),
            planSpecs[i].defaultChecked);
        m_planPreferences[i] = checked;
        m_planChecks[i]->blockSignals(true);
        m_planChecks[i]->setChecked(checked);
        m_planChecks[i]->blockSignals(false);
    }
}

void LegacyMainWindow::saveLegacySettings()
{
    if (legacySmokeSettingsIsolation()) {
        // The smoke never writes the user's settings files.
        return;
    }
    QSettings settings;
    // Same per-group layout as loadLegacySettings: Qt 3.3.7 writes each
    // settings group to its own per-user file under ~/.qt/ (devicesrc,
    // logsrc, diagnosticsrc, repairrc).
    settings.setPath(QString::fromLatin1("boot-bitch"),
                     QString::fromLatin1("boot-repair"), QSettings::User);
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
    // The persisted plan preference is written from the preference register
    // (never from the widget state, which the availability pass may have
    // forced off for unavailable rows).
    if (m_planPreferences.size() != static_cast<std::size_t>(planSpecCount)) {
        m_planPreferences.assign(planSpecCount, false);
        for (int i = 0; i < planSpecCount; ++i) {
            m_planPreferences[i] = planSpecs[i].defaultChecked;
        }
    }
    for (int i = 0; i < planSpecCount; ++i) {
        settings.writeEntry(QString::fromLatin1(planSpecs[i].settingsKey),
                            m_planPreferences[i]);
    }
    // The four per-group files Qt 3.3.7 wrote under ~/.qt carry the GUI's
    // persisted state; keep them private (0600) like the session logs. A
    // file that was never written (its group stayed untouched) is missing and
    // its chmod failure is ignored.
    const QString qtDirectory = QDir::homeDirPath() + QString::fromLatin1("/.qt");
    static const char *const settingsFiles[] = {
        "devicesrc", "logsrc", "diagnosticsrc", "repairrc"
    };
    for (int i = 0;
         i < static_cast<int>(sizeof(settingsFiles) / sizeof(settingsFiles[0])); ++i) {
        const QString settingsPath = qtDirectory + QString::fromLatin1("/")
            + QString::fromLatin1(settingsFiles[i]);
        ::chmod(settingsPath.local8Bit().data(), 0600);
    }
}

// The deterministic default settings (smoke isolation): the same values
// loadLegacySettings uses as read defaults, applied without touching QSettings.
void LegacyMainWindow::applyLegacySettingsDefaults()
{
    m_logWrapEnabled = true;
    m_autoRefreshEnabled = true;
    QCheckBox *checks[] = { m_showNonLinuxCheck, m_showRemovableCheck,
                            m_showEncryptedCheck, m_autoRefreshCheck,
                            m_logWrapCheck };
    const bool values[] = { true, true, true, true, true };
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
    m_planPreferences.assign(planSpecCount, false);
    for (int i = 0; i < planSpecCount; ++i) {
        m_planPreferences[i] = planSpecs[i].defaultChecked;
    }
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        m_planChecks[i]->blockSignals(true);
        m_planChecks[i]->setChecked(planSpecs[i].defaultChecked);
        m_planChecks[i]->blockSignals(false);
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
// Modern refreshCapabilities(): re-probe the running host read-only (a PATH
// search, never an execution, plus /etc/os-release) and repopulate the
// summary labels and the 6-column capability table. Missing optional tools
// grey only their related features; the Install Missing Support... button
// stays disabled with the modern tooltip.
void LegacyMainWindow::refreshCapabilities()
{
    if (m_capDistributionLabel) {
        const QString pretty = readOsReleaseValue(QString::fromLatin1("PRETTY_NAME"));
        m_capDistributionLabel->setText(pretty.isEmpty()
            ? QString::fromLatin1("Unknown Linux distribution") : pretty);
    }
    if (m_capPackageManagerLabel) {
        m_capPackageManagerLabel->setText(packageManagerLabelForHost());
    }
    if (m_capAuthLabel) {
        m_capAuthLabel->setText(QString::fromLatin1(
            "sudo / gksu (no KAuth on this frontend)"));
    }
    if (!m_capabilityTable) {
        return;
    }
    m_capabilityTable->setNumRows(capabilitySpecCount);
    for (int row = 0; row < capabilitySpecCount; ++row) {
        const CapabilitySpec &spec = capabilitySpecs[row];
        const QString command = QString::fromLatin1(spec.command);
        const bool available = !findExecutablePath(command).isEmpty();
        QString status;
        if (available) {
            status = QString::fromLatin1("Available");
        } else if (command == QString::fromLatin1("unshare")) {
            // The legacy-specific row: Etch has no unshare, so the ported
            // helper falls back to a guarded plain chroot.
            status = QString::fromLatin1("Missing on this frontend");
        } else {
            status = QString::fromLatin1("Missing");
        }
        m_capabilityTable->setText(row, 0, QString::fromLatin1(spec.feature));
        m_capabilityTable->setText(row, 1, command);
        m_capabilityTable->setText(row, 2, QString::fromLatin1(spec.scope));
        m_capabilityTable->setText(row, 3, status);
        m_capabilityTable->setText(row, 4, QString::fromLatin1(spec.package));
        m_capabilityTable->setText(row, 5, QString::fromLatin1(spec.note));
    }
    // Modern parity: the first row starts selected and clicking any cell
    // selects its whole row (QTable::SingleRow above); the selected missing
    // capability is the conceptual guard the disabled Install Missing
    // Support... button documents.
    if (m_capabilityTable->numRows() > 0) {
        m_capabilityTable->setCurrentCell(0, 0);
        m_capabilityTable->selectRow(0);
    }
    appendLog(QString::fromLatin1(
        "Host capability scan refreshed (read-only PATH probes; nothing was "
        "executed). Missing optional tools disable only the related features."));
}

// ---- Individual repair tools and the Full Repair plan -----------------------

// Cycle 14 loop: the tools list is sortable, so the selected tool resolves
// by the current row's TITLE (titles are unique), never by row position.
int LegacyMainWindow::toolIndexForTitle(const QString &title) const
{
    for (int i = 0; i < toolSpecCount; ++i) {
        if (QString::fromLatin1(toolSpecs[i].title) == title) {
            return i;
        }
    }
    return -1;
}

int LegacyMainWindow::selectedToolIndex() const
{
    if (!m_toolList) {
        return -1;
    }
    QListViewItem *item = m_toolList->currentItem();
    if (!item) {
        return -1;
    }
    return toolIndexForTitle(item->text(0));
}

void LegacyMainWindow::toolSelectionChanged()
{
    updateToolDetails();
    updateActionStates();
}

// Cycle 14 loop: manual header-click sorting for the individual tools list.
// Qt3 cannot keep the curated order as the default while sorting is enabled,
// so the items are detached, reordered by the clicked column (toggling
// asc/desc), and re-inserted; the current selection and the Selected tool
// pane are then restored.
void LegacyMainWindow::toolListHeaderClicked(int column)
{
    if (!m_toolList || !m_toolList->header()
        || column < 0 || column >= m_toolList->columns()) {
        return;
    }
    const bool ascending = m_toolSortColumn == column
        ? !m_toolSortAscending : true;
    // Deterministic on Qt3: rebuild the rows in the sorted order instead of
    // detaching/re-inserting live items. Column 0 sorts by the tool title;
    // column 1 sorts by a static per-tool key (the plan stage or the plan
    // base), never by the dynamic availability text.
    std::vector<int> order;
    for (int i = 0; i < toolSpecCount; ++i) {
        order.push_back(i);
    }
    for (std::size_t i = 1; i < order.size(); ++i) {
        const int key = order[i];
        QString keyText = column == 0
            ? QString::fromLatin1(toolSpecs[key].title)
            : (toolSpecs[key].planStage && *toolSpecs[key].planStage
                ? QString::fromLatin1(toolSpecs[key].planStage)
                : QString::fromLatin1(toolSpecs[key].planBase));
        std::size_t j = i;
        while (j > 0) {
            const int prev = order[j - 1];
            const QString prevText = column == 0
                ? QString::fromLatin1(toolSpecs[prev].title)
                : (toolSpecs[prev].planStage && *toolSpecs[prev].planStage
                    ? QString::fromLatin1(toolSpecs[prev].planStage)
                    : QString::fromLatin1(toolSpecs[prev].planBase));
            const bool before = ascending
                ? prevText > keyText : prevText < keyText;
            if (!before) {
                break;
            }
            order[j] = order[j - 1];
            --j;
        }
        order[j] = key;
    }
    m_toolList->clear();
    QListViewItem *lastTool = 0;
    for (std::size_t i = 0; i < order.size(); ++i) {
        const ToolSpec &spec = toolSpecs[order[i]];
        lastTool = new QListViewItem(m_toolList, lastTool,
                                     QString::fromLatin1(spec.title),
                                     QString::fromLatin1("not reported"));
    }
    m_toolSortColumn = column;
    m_toolSortAscending = ascending;
    if (m_toolList->firstChild()) {
        m_toolList->setSelected(m_toolList->firstChild(), true);
        m_toolList->setCurrentItem(m_toolList->firstChild());
    }
    updateToolDetails();
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
    if (spec.hostOnly && !hostScope()) {
        // The legacy display-manager repair is a host-scope stage; the
        // offline target form stays disabled with the exact remedy.
        if (reason) {
            *reason = QString::fromLatin1(
                "Restore Graphical Login is a host-scope stage on this legacy "
                "frontend; enter Host Maintenance to run it.");
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
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling()) {
            // The row's tool resolves by its title, so a sorted list still
            // mirrors the correct spec (the curated order is not assumed).
            const int rowIndex = toolIndexForTitle(row->text(0));
            if (rowIndex < 0) {
                continue;
            }
            const ToolSpec &spec = toolSpecs[rowIndex];
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
            if (!toolRunReady(rowIndex, &reason)) {
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
        const bool answer = confirmWrapped(
            this, QString::fromLatin1("Confirm repair"),
            QString::fromLatin1(spec.confirm));
        if (!answer) {
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
    const bool started = startCommand(args, false, QString::fromLatin1(spec.title),
                                       false, false, false, false, QString::null,
                                       stage);
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
    // Modern preference semantics: only the checkboxes the current scope
    // actually offers (enabled) carry the user's preference; rows forced off
    // by the availability pass must never clobber the saved preference.
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (m_planChecks[i] && m_planChecks[i]->isEnabled()) {
            if (m_planPreferences.size() != static_cast<std::size_t>(planSpecCount)) {
                m_planPreferences.assign(planSpecCount, false);
                for (int j = 0; j < planSpecCount; ++j) {
                    m_planPreferences[j] = planSpecs[j].defaultChecked;
                }
            }
            m_planPreferences[i] = m_planChecks[i]->isChecked();
        }
    }
    saveLegacySettings();
    updateActionStates();
}

// Modern availability semantics for the Settings plan checkboxes
// (MainWindow::updateRepairScopeControls() parity): every row is shown; an
// available stage is checkable with its saved preference (or the per-key
// default), an unavailable stage is disabled AND unchecked with the exact
// reason as its tooltip. The pass blocks signals so the presentation never
// re-records the preference; it runs from updateActionStates() whenever the
// scope or the cached diagnostics change.
void LegacyMainWindow::updatePlanChecks()
{
    if (m_planPreferences.size() != static_cast<std::size_t>(planSpecCount)) {
        m_planPreferences.assign(planSpecCount, false);
        for (int i = 0; i < planSpecCount; ++i) {
            m_planPreferences[i] = planSpecs[i].defaultChecked;
        }
    }
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        QString reason;
        const bool available = planStageAvailable(i, &reason);
        m_planChecks[i]->blockSignals(true);
        if (available) {
            m_planChecks[i]->setEnabled(true);
            m_planChecks[i]->setChecked(m_planPreferences[i]);
        } else {
            m_planChecks[i]->setChecked(false);
            m_planChecks[i]->setEnabled(false);
        }
        m_planChecks[i]->blockSignals(false);
        const QString tip = available
            ? QString::fromLatin1(
                  "Include the %1 stage in the Full Repair plan. The stage runs "
                  "in the plan order shown on the Repair tab.")
                  .arg(QString::fromLatin1(planSpecs[i].title))
            : QString::fromLatin1("Unavailable: %1").arg(reason);
        if (i >= static_cast<int>(m_planCheckTips.size())) {
            m_planCheckTips.resize(i + 1);
        }
        if (m_planCheckTips[i] != tip) {
            QToolTip::add(m_planChecks[i], tip);
            m_planCheckTips[i] = tip;
        }
    }
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
    // Frontend gap (fail closed): the capability may be available in the
    // helper, but this legacy frontend exposes no plan action for it, so the
    // row stays shown, disabled and unchecked with the exact reason.
    if (spec.frontendGap && *spec.frontendGap) {
        if (reason) {
            *reason = QString::fromLatin1(spec.frontendGap);
        }
        return false;
    }
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
    const bool answer = confirmWrapped(
        this, QString::fromLatin1("Run Full Repair"), text);
    if (!answer) {
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-repair") : QString::fromLatin1("repair"))
         << selectedDisk() << selectedRoot();
    args += stages;
    const bool started = startCommand(args, false, QString::fromLatin1("Full Repair"),
                                       false, false, false, false, QString::null,
                                       stages.join(QString::fromLatin1(" ")));
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
    // Qt 3.3.7's Qt::LogText mode disables word wrap entirely, so the popup
    // uses Qt::PlainText (the transcript is short-lived, not the Logs view)
    // and follows the Settings "Wrap long log lines" toggle like the Logs
    // view; the monospace font stays either way.
    m_resultView->setTextFormat(Qt::PlainText);
    m_resultView->setWordWrap(m_logWrapEnabled
                                  ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
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
        const bool answer = confirmWrapped(
            this, QString::fromLatin1("Confirm running-host command"),
            QString::fromLatin1(
                "Run this command as root on the protected running host?\n\n"
                "%1\n\n"
                "The helper keeps its runtime preflights; the command is passed "
                "as one argument and is never interpreted by the GUI.").arg(command));
        if (!answer) {
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
                                     bool quiet, const QString &logSection,
                                     const QString &logStages, bool browse)
{
    if (m_running) {
        return false;
    }
    // Every log entry is tagged at capture time; outside a running command
    // the register is plain application text.
    m_logEntryKind = LogEntry::Application;
    m_logEntryDiagnostic = QString::null;
    m_logEntryStages = QString::null;
    m_activeRepairStages.clear();
    if (m_smokeMode && m_runner->elevationNeedsPassword(0)) {
        // A modal password prompt would hang the headless smoke: fail with the
        // exact remedy instead. The unlock keyfile and the pending config
        // content file are discarded here too so no secret file ever outlives
        // the refused attempt.
        discardUnlockKeyfile();
        discardConfigContentFile();
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
        discardUnlockKeyfile();
        discardConfigContentFile();
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
        discardUnlockKeyfile();
        discardConfigContentFile();
        m_pendingConfigWrite = false;
        m_pendingConfigKey = QString::null;
        m_pendingConfigPath = QString::null;
        return false;
    }
    m_transcript = QString::null;
    m_pendingDiagnostic = diagnostic;
    m_pendingQuiet = quiet;
    // The single-key diagnostic key (empty for the combined `all` run) drives
    // the busy indicator's modern "Running diagnostic: <title>" text.
    m_pendingDiagnosticKey = diagnostic ? logSection : QString::null;
    m_pendingUnlock = unlock;
    m_pendingConfig = config;
    m_pendingShell = shell;
    m_pendingBrowse = browse;
    m_pendingIdentity = identity();
    m_pendingLabel = label;
    // Tag the command's own header lines and its streamed output: diagnostic
    // runs are Diagnostic entries (the section key follows the stream's
    // `Diagnostic: <key>` markers; a single-key run seeds the requested key),
    // repair runs are Repair entries carrying the helper stage (or the Full
    // Repair stage set), config/shell/unlock traffic stays Application.
    if (diagnostic) {
        m_logEntryKind = LogEntry::Diagnostic;
        m_logEntryDiagnostic = logSection;
    } else if (!unlock && !config && !shell && !browse) {
        m_logEntryKind = LogEntry::Repair;
        m_logEntryStages = logStages;
        m_activeRepairStages = QStringList::split(QString::fromLatin1(" "), logStages, false);
    } else {
        m_activeRepairStages.clear();
    }
    m_running = true;
    updateActionStates();
    updateBusyIndicator();

    // A10-05: the cancel token for this command.  The file lives in the
    // GUI's own private directory (0700) and is removed before the run - a
    // leftover from an interrupted session must never cancel the new command
    // immediately - and after it finishes.  HelperRunner passes it as
    // --cancel-file and cancel() touches it before the direct kill.
    ensureDirectory(QDir::homeDirPath()
                        + QString::fromLatin1("/.boot-repair-legacy"),
                    0700);
    m_cancelFilePath = QDir::homeDirPath()
        + QString::fromLatin1("/.boot-repair-legacy/cancel-request");
    ::unlink(m_cancelFilePath.local8Bit().data());
    m_runner->setCancelFile(m_cancelFilePath);

    QString elevation;
    m_runner->resolveElevation(&elevation);
    m_lastHelperDescription = elevation;
    updateElevationLabel();
    appendLog(QString::null);
    appendLog(QString::fromLatin1("=== %1 ===").arg(label));
    appendLog(QString::fromLatin1("helper: %1 (%2)").arg(m_runner->helperPath()).arg(elevation));
    // The command transcript never carries secrets. The guarded config-write
    // verb transports the edited content either through a private file
    // (--content-file <path>, B5) or — for the deprecated argv fallback — as
    // one argv element (the last one); the unlock carries its passphrase in
    // the --key-file argument: all are redacted before the line is logged.
    QStringList loggableArgs(args);
    if (loggableArgs.count() > 4
        && loggableArgs[0] == QString::fromLatin1("config-write")) {
        if (loggableArgs.count() > 5
            && loggableArgs[4] == QString::fromLatin1("--content-file")) {
            loggableArgs[5] =
                QString::fromLatin1("<config-write content path redacted>");
        } else {
            loggableArgs[loggableArgs.count() - 1] =
                QString::fromLatin1("<config-write content redacted>");
        }
    }
    if (loggableArgs.count() > 1) {
        for (int i = 0; i + 1 < static_cast<int>(loggableArgs.count()); ++i) {
            if (loggableArgs[i] == QString::fromLatin1("--key-file")) {
                loggableArgs[i + 1] =
                    QString::fromLatin1("<keyfile path redacted>");
            }
        }
    }
    appendLog(QString::fromLatin1("command: %1 %2")
                  .arg(m_runner->helperPath())
                  .arg(loggableArgs.join(QString::fromLatin1(" "))));
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
    // Diagnostic runs: the stream's `Diagnostic: <key>` markers drive the
    // section tagging of the lines that follow (the marker itself carries the
    // new key, so a section filter shows its header too). Unknown keys leave
    // the current section untouched (fail closed).
    if (m_pendingDiagnostic
        && line.startsWith(QString::fromLatin1("Diagnostic: "))) {
        const QString key = line.mid(12).stripWhiteSpace();
        for (int i = 0; i < diagnosticSpecCount; ++i) {
            if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
                m_logEntryDiagnostic = key;
                break;
            }
        }
    }
    appendLog(line);
    // Stream into the modern-style repair result popup when it is open.
    // The wrap mode is re-applied defensively so a toggle that flips through
    // any path mid-dialog is honored (cheap; the widget keeps its state).
    if (m_resultView) {
        m_resultView->setWordWrap(m_logWrapEnabled
                                      ? QTextEdit::WidgetWidth
                                      : QTextEdit::NoWrap);
        m_resultView->append(line);
    }
}

void LegacyMainWindow::helperFinished(bool ok, int exitCode)
{
    if (m_pendingLabel.isEmpty() && !m_running) {
        return;
    }
    // A10-05: the finished command's cancel token is removed so the next run
    // starts clean (startCommand unlinks it again defensively).
    if (!m_cancelFilePath.isEmpty()) {
        ::unlink(m_cancelFilePath.local8Bit().data());
    }
    appendLog(QString::fromLatin1("--- %1 %2 (exit %3) ---")
                  .arg(m_pendingLabel)
                  .arg(ok ? QString::fromLatin1("finished") : QString::fromLatin1("failed"))
                  .arg(exitCode));
    // The completion line belongs to the command's kind; everything appended
    // afterwards (invalidation notes, unlock follow-ups, dialogs) is plain
    // application text again.
    m_logEntryKind = LogEntry::Application;
    m_logEntryDiagnostic = QString::null;
    m_logEntryStages = QString::null;

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
    const bool wasBrowse = m_pendingBrowse;
    const bool wasFullRepair = m_pendingLabel == QString::fromLatin1("Full Repair");
    const QString finishedLabel = m_pendingLabel;
    QString configReadContent;
    if (m_pendingDiagnostic) {
        m_model.applyDiagnosticTranscript(toStd(m_pendingIdentity), transcript, ok);
        m_rawView->setText(m_transcript);
        if (ok) {
            // Cycle 9: remember the run's key+scope so the Results button can
            // offer "Re-run Diagnostic" while this cached result matches the
            // selected check (a scope change, an invalidation or a missing
            // cache flips it back to "Run Diagnostic").
            m_lastDiagnosticKey = m_logEntryDiagnostic;
            m_lastDiagnosticIdentity = m_pendingIdentity;
            updateFactView(parsed);
            mergeHelperDevices(parsed);
        }
    } else {
        m_model.applyCommandTranscript(transcript);
        reportChangeStatuses(parsed);
        // B7-4: the structured repair summary block after every repair
        // command (individual tool or Full Repair); unlock/config/shell and
        // the read-only browse listing stay out of the summary.
        if (!wasConfig && !m_pendingUnlock && !wasShell && !wasBrowse) {
            appendRepairSummaryBlock(parsed, ok);
        }
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
    m_pendingQuiet = false;
    m_pendingDiagnosticKey = QString::null;
    m_pendingUnlock = false;
    m_pendingConfig = false;
    m_pendingConfigWrite = false;
    m_pendingConfigKey = QString::null;
    m_pendingConfigPath = QString::null;
    m_pendingShell = false;
    m_pendingBrowse = false;
    m_pendingLabel = QString::null;
    // The unlock keyfile and the config-write content file are deleted on
    // every completion path (the helper has already read the files by the
    // time the command returns and never deletes them; these unlinks are
    // best-effort and idempotent).
    discardUnlockKeyfile();
    discardConfigContentFile();
    updateActionStates();
    updateBusyIndicator();
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

    // The read-only browse listing drives the repair-system folder picker.
    if (wasBrowse) {
        handleFileCopyBrowseResult(transcript, ok);
    }

    // Modern finishFullRepairPlan parity: once after the last plan stage
    // (success OR aborted), the scope's cached diagnostics are invalidated
    // and one quiet Run All is scheduled when the auto-refresh setting, the
    // administrator session and the idle state allow it; it never prompts.
    // Individual repair actions keep the existing stale-based scheduling.
    if (!wasDiagnostic && !wasConfig && !wasUnlock && !wasShell && !wasBrowse) {
        if (wasFullRepair) {
            m_model.reset();
            appendLog(QString::fromLatin1(
                "Full Repair completed; cached diagnostics were invalidated "
                "for the current scope."));
            maybeAutoRefreshDiagnostics(QString::fromLatin1("Full Repair"));
        } else if (ok && m_model.diagnosticsStale()) {
            maybeAutoRefreshDiagnostics(finishedLabel);
        }
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
    const std::string unlockedRoot = unlockRoot(transcript);
    const std::string unlockedFstype = unlockRootFstype(transcript);
    const std::string unlockedUuid = unlockRootUuid(transcript);
    const bool authFailed = unlockAuthFailed(transcript);
    const QString targetDisk = disk.isEmpty() ? selectedDisk() : disk;
    QString status;
    if (ok && !mapper.empty()) {
        status = QString::fromLatin1(
            "State: unlocked\nComponent: %1\nMapper: %2\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
            "Result: mapping opened for this recovery session.")
            .arg(device).arg(fromStd(mapper));
        // Opening the mapper changes the target topology: cached diagnostics
        // for the old identity no longer describe the selected scope.
        m_model.reset();
        appendLog(QString::fromLatin1(
            "LUKS volume unlocked; the target topology changed, so cached "
            "diagnostics were invalidated. Run diagnostics again."));
        scanDevices();
        // Cycle 11: record the helper-confirmed root AFTER the rescan (which
        // clears the previous session state) so Select Target can resolve
        // the mapped LV the read-only inventory cannot see.
        m_unlockedDisk = targetDisk;
        m_unlockedRoot = fromStd(unlockedRoot);
        m_unlockedMapper = fromStd(mapper);
        m_unlockedRootUuid = fromStd(unlockedUuid);
        if (m_unlockedRoot.isEmpty()) {
            m_unlockedDisk = QString::null;
        }
        if (!unlockedFstype.empty()) {
            appendLog(QString::fromLatin1(
                "Unlocked root candidate: %1 (fstype %2).")
                .arg(m_unlockedRoot).arg(fromStd(unlockedFstype)));
        }
        // Cycle 12: show the freshly activated mapper and root under the
        // selected drive so the tree and the inventory-backed resolution
        // both see them.
        injectUnlockedMapperRows(targetDisk, m_unlockedMapper,
                                 m_unlockedRoot, fromStd(unlockedFstype),
                                 m_unlockedRootUuid);
        updateDriveDetails();
        updateActionStates();
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

namespace {

// The helper's own failure lines carry a [HH:MM:SS] log prefix; strip it when
// present and return the message of an ERROR line ("" for any other line).
QString errorMessageOfLine(const QString &line)
{
    QString text = line.stripWhiteSpace();
    // The legacy helper prefixes log() lines with [HH:MM:SS] (ten characters
    // plus the separating space).
    if (text.length() > 11 && text[0] == QChar('[') && text[9] == QChar(']')
        && text[10] == QChar(' ')) {
        text = text.mid(11).stripWhiteSpace();
    }
    if (text.startsWith(QString::fromLatin1("ERROR:"))) {
        return text.mid(6).stripWhiteSpace();
    }
    return QString::null;
}

// The repair stage the helper names in its final `ERROR: stage 'X' failed:`
// line, mapped to its tool key ("" when the transcript names none). Decorated
// labels ("grub (EFI follow-up)") belong to their base stage, exactly like
// MainWindow::repairFailureStageKey().
QString repairFailureStageKey(const QString &transcript)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    QString stage;
    const QString prefix = QString::fromLatin1("stage '");
    const QString suffix = QString::fromLatin1("' failed:");
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString message = errorMessageOfLine(*it);
        if (message.isEmpty()) {
            continue;
        }
        const int begin = message.find(prefix);
        if (begin < 0) {
            continue;
        }
        const int end = message.find(suffix, begin + prefix.length());
        if (end < 0) {
            continue;
        }
        stage = message.mid(begin + prefix.length(),
                            end - begin - prefix.length()).stripWhiteSpace();
    }
    if (stage.isEmpty()) {
        return QString::null;
    }
    const int decoration = stage.find(QString::fromLatin1(" ("));
    if (decoration > 0) {
        stage = stage.left(decoration);
    }
    return fromStd(repairToolKeyForStage(toStd(stage)));
}

// The last ERROR message of the transcript (the helper may retry internally
// and only the final failure line describes where the plan stopped), like
// MainWindow::shortRepairFailureReason().
QString shortRepairFailureReason(const QString &transcript)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    QString last;
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString message = errorMessageOfLine(*it);
        if (!message.isEmpty()) {
            last = message;
        }
    }
    return last;
}

// The failure reason the helper reported for one named stage (the last
// `ERROR: stage '<tool-key stage>' failed: <reason>` line for that stage,
// reason only), so an unrelated ERROR line elsewhere in the transcript can
// never be misattributed to this stage — exactly like
// MainWindow::repairStageFailureReason(). Empty when the transcript names no
// failure for the stage.
QString repairStageFailureReason(const QString &transcript, const QString &toolKey)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    const QString prefix = QString::fromLatin1("stage '");
    const QString suffix = QString::fromLatin1("' failed:");
    QString reason;
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString message = errorMessageOfLine(*it);
        if (message.isEmpty()) {
            continue;
        }
        const int begin = message.find(prefix);
        if (begin < 0) {
            continue;
        }
        const int end = message.find(suffix, begin + prefix.length());
        if (end < 0) {
            continue;
        }
        QString stage = message.mid(begin + prefix.length(),
                                    end - begin - prefix.length()).stripWhiteSpace();
        const int decoration = stage.find(QString::fromLatin1(" ("));
        if (decoration > 0) {
            stage = stage.left(decoration);
        }
        if (fromStd(repairToolKeyForStage(toStd(stage))) == toolKey) {
            reason = message.mid(end + suffix.length()).stripWhiteSpace();
        }
    }
    return reason;
}

} // namespace

// B7-4: the structured repair summary block, appended after every repair
// command (individual tool or Full Repair) and built exclusively from the
// parsed `Repair change status` lines.  A requested stage with no parsed
// status counts as failed unless the helper reported a clean read-only
// outcome (nothing is fabricated).  The block is bracketed by
// `==== REPAIR ====` lines, tagged with the Repair kind so the Repairs
// log filter shows it, and uses ASCII markers ([OK]/[FAIL]/[-]) because
// Etch's fonts garble the modern check/cross/bullet glyphs.
void LegacyMainWindow::appendRepairSummaryBlock(const ParsedTranscript &parsed,
                                                bool commandOk)
{
    // Per-stage outcomes from the transcript, in first-seen order.
    QStringList seenStages;
    QMap<QString, QString> outcomes;
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        const QString key = fromStd(parsed.changeStatuses[i].first);
        const QString state = fromStd(parsed.changeStatuses[i].second);
        if (!outcomes.contains(key)) {
            seenStages.append(key);
        }
        outcomes.insert(key, state);
    }
    QStringList expected = m_activeRepairStages;
    if (expected.isEmpty()) {
        expected = seenStages;
    }

    // The read-only file system check counts as a clean outcome even without
    // a change-status line: `File system check summary: ... issues=0`.
    const bool cleanFsInspect = m_transcript.find(QString::fromLatin1("File system check summary:")) >= 0
        && m_transcript.find(QString::fromLatin1("issues=0")) >= 0;

    // Host-repair summary fix: a stage's result comes from its own
    // `Repair change status` line, never from the overall command exit code.
    // `unchanged|<reason>` is a proven no-op, `changed` is a stage that
    // completed (the helper only reports it after the stage succeeded), and
    // only the stage the helper names in its final `ERROR: stage 'X' failed:`
    // line owns the failure; requested stages after it never ran. This
    // mirrors MainWindow's Full Repair stage categorization.
    const QString failedStageKey = repairFailureStageKey(m_transcript);
    QString failureReason = shortRepairFailureReason(m_transcript);
    if (failureReason.isEmpty()) {
        failureReason = QString::fromLatin1("the privileged helper reported a failure");
    }

    int successful = 0;
    int failed = 0;
    int unchanged = 0;
    int notRun = 0;
    bool failureAttributed = false;
    QStringList handledKeys;
    QStringList lines;
    for (int i = 0; i < static_cast<int>(expected.size()); ++i) {
        const QString stage = expected[i];
        // Cycle 11 fix: the plan-stage keys (dpkg-configure, fix-broken,
        // apt-update, apt-upgrade) must map to the transcript's tool keys
        // (dpkg, fixbroken, aptupdate, upgrade) before matching the
        // `Repair change status <key>:` lines, so the four package stages
        // render their real outcomes instead of "not reported".
        const QString toolKey = fromStd(repairToolKeyForStage(toStd(stage)));
        const QString title = repairStageDisplayTitle(toolKey);
        handledKeys.append(toolKey);
        const QMap<QString, QString>::const_iterator found =
            outcomes.find(toolKey);
        QString detail;
        if (found != outcomes.end()) {
            const QString state = found.data();
            if (changeStatusIsUnchanged(toStd(state))) {
                ++unchanged;
                const QString reason = state.startsWith(QString::fromLatin1("unchanged|"))
                    ? state.mid(10) : QString::fromLatin1("no repair needed");
                // Cycle 10: ASCII markers only — Etch's fonts garble the
                // modern check/cross/bullet glyphs in the Qt3 log view.
                detail = QString::fromLatin1("[-] %1 - %2")
                             .arg(title).arg(reason);
            } else {
                // A reported change is a completed stage even when a later
                // stage (or the command's final bookkeeping) failed.
                ++successful;
                detail = QString::fromLatin1("[OK] %1 - changed")
                             .arg(title);
                const int separator = state.find(QChar('|'));
                if (separator > 0) {
                    detail += QString::fromLatin1(" - %1")
                                  .arg(state.mid(separator + 1).stripWhiteSpace());
                }
            }
        } else if (!commandOk && !failureAttributed
                   && (failedStageKey.isEmpty() || toolKey == failedStageKey)) {
            // The named failing stage (or, when the helper failed before
            // naming one, the first stage without a completed status) owns
            // the failure reason; it fails the overall summary. The
            // stage-specific reason line wins so an unrelated ERROR line
            // earlier in the transcript can never be misattributed.
            ++failed;
            failureAttributed = true;
            QString stageReason = repairStageFailureReason(m_transcript, toolKey);
            if (stageReason.isEmpty()) {
                stageReason = failureReason;
            }
            detail = QString::fromLatin1("[FAIL] %1 - %2")
                         .arg(title).arg(stageReason);
        } else if (!commandOk) {
            // A requested stage after the failure never ran; it is not a
            // failure of its own.
            ++notRun;
            detail = QString::fromLatin1(
                "[-] %1 - not run (the plan stopped before reaching this stage)")
                .arg(title);
        } else if ((stage == QString::fromLatin1("filesystem")
                    || stage == QString::fromLatin1("fs-inspect"))
                   && cleanFsInspect) {
            ++unchanged;
            detail = QString::fromLatin1(
                "[-] %1 - no file system errors found - no changes")
                .arg(title);
        } else {
            ++failed;
            detail = QString::fromLatin1(
                "[FAIL] %1 - not reported").arg(title);
        }
        lines.append(QString::fromLatin1("  ") + detail);
    }
    // A failure can belong to a stage the plan did not request (for example
    // the GRUB follow-up of an EFI repair); keep it visible in the summary.
    if (!commandOk && !failureAttributed && !failedStageKey.isEmpty()
        && !handledKeys.contains(failedStageKey)) {
        ++failed;
        failureAttributed = true;
        QString stageReason = repairStageFailureReason(m_transcript, failedStageKey);
        if (stageReason.isEmpty()) {
            stageReason = failureReason;
        }
        lines.append(QString::fromLatin1("  [FAIL] %1 - %2")
                         .arg(repairStageDisplayTitle(failedStageKey))
                         .arg(stageReason));
    }

    QString tail;
    if (failed > 0) {
        tail = QString::fromLatin1(
            "%1 stage(s) failed; review the helper output in Logs.").arg(failed);
    } else if (notRun > 0 && successful == 0) {
        tail = QString::fromLatin1(
            "the plan stopped before any stage completed.");
    } else if (successful == 0) {
        tail = QString::fromLatin1(
            "no repair was needed; cached diagnostics remain valid.");
    } else {
        tail = QString::fromLatin1(
            "repair completed; cached diagnostics were invalidated and must be "
            "regenerated.");
    }
    QString headline = QString::fromLatin1(
        "%1 results: [OK] %2 successful | [FAIL] %3 failed "
        "| [-] %4 no repair needed - %5")
        .arg(m_pendingLabel == QString::fromLatin1("Full Repair")
                 ? QString::fromLatin1("Full Repair")
                 : m_pendingLabel)
        .arg(successful).arg(failed).arg(unchanged).arg(tail);
    if (notRun > 0) {
        headline += QString::fromLatin1(" | [-] %1 not run").arg(notRun);
    }

    // The block is Repair-tagged so the Repairs log filter shows it.
    const int savedKind = m_logEntryKind;
    const QString savedStages = m_logEntryStages;
    m_logEntryKind = LogEntry::Repair;
    m_logEntryStages = m_activeRepairStages.join(QString::fromLatin1(" "));
    appendLog(QString::null);
    // Cycle 10: ASCII brackets, consistent with the ASCII stage markers.
    appendLog(QString::fromLatin1("==== REPAIR ===="));
    appendLog(headline);
    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        appendLog(lines[i]);
    }
    appendLog(QString::fromLatin1("==== REPAIR ===="));
    appendLog(QString::null);
    m_logEntryKind = savedKind;
    m_logEntryStages = savedStages;
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
                     QString::fromLatin1("smoke host-diagnose all"),
                     false, false, false, false, QString::null, QString::null);
    } else if (m_smokeStep == 1) {
        QStringList args;
        args << QString::fromLatin1("host-validate") << selectedDisk() << selectedRoot();
        startCommand(args, false,
                     QString::fromLatin1("smoke host-validate"),
                     false, false, false, false, QString::null,
                     QString::fromLatin1("validate"));
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
    if (!m_tabs || m_tabs->count() != 7) {
        problems->append(QString::fromLatin1("tab set missing (expected 7 tabs)"));
        ok = false;
    } else {
        // The Chroot Shell tab label switches to "Host Shell" in host mode
        // (modern updateChrootShellMode); the smoke runs with Host Maintenance
        // active, so expect the host label here. Modern parity: there is no
        // About tab (Help -> About Boot Bitch is the dialog).
        const char *expectedTabs[] = {
            "Systems", "Diagnostics", "Repair", "Host Shell", "File Copy",
            "Logs", "Settings"
        };
        for (int i = 0; i < 7; ++i) {
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
    // Global busy indicator (modern parity): a reserved row below the
    // guarded-repair badge that shows the modern working text only while a
    // helper command runs; the smoke runs between commands, so the label
    // must be empty now and the row must keep its reserved height.
    if (!m_busyLabel) {
        problems->append(QString::fromLatin1("busy indicator missing"));
        ok = false;
    } else {
        if (!m_busyTimer) {
            problems->append(QString::fromLatin1("busy animation timer missing"));
            ok = false;
        }
        if (!m_busyLabel->text().isEmpty()) {
            problems->append(QString::fromLatin1(
                "busy indicator shows '%1' while no helper command runs")
                .arg(m_busyLabel->text()));
            ok = false;
        }
        // Modern parity: the indicator sits BELOW the guarded-repair badge
        // and its right edge is flush with the badge's right edge.
        if (m_headerBadge) {
            const QRect badgeRect(m_headerBadge->mapTo(this, QPoint(0, 0)),
                                  m_headerBadge->size());
            const QRect busyRect(m_busyLabel->mapTo(this, QPoint(0, 0)),
                                 m_busyLabel->size());
            if (busyRect.top() < badgeRect.bottom()) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not below the guarded-repair badge"));
                ok = false;
            }
            if (QABS(busyRect.right() - badgeRect.right())
                > kBusyBadgeEdgeTolerance) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not right-aligned with the guarded-repair badge"));
                ok = false;
            }
        }
        // Show the widest animated frame; the reserved height must not
        // change and the slot must stay inside the window.
        const int reservedHeight = m_busyLabel->height();
        m_busyLabel->setText(QString::fromLatin1("Running all diagnostics..."));
        qApp->processEvents();
        if (m_busyLabel->height() != reservedHeight) {
            problems->append(QString::fromLatin1(
                "busy indicator row lost its reserved height when shown"));
            ok = false;
        }
        const QRect shownRect(m_busyLabel->mapTo(this, QPoint(0, 0)),
                              m_busyLabel->size());
        if (!rect().contains(shownRect)) {
            problems->append(QString::fromLatin1(
                "busy indicator is clipped by the window when shown"));
            ok = false;
        }
        m_busyLabel->setText(QString::null);
        qApp->processEvents();
        if (m_busyLabel->height() != reservedHeight) {
            problems->append(QString::fromLatin1(
                "busy indicator row lost its reserved height when cleared"));
            ok = false;
        }
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
    } else {
        // The ten modern details rows, in the modern order; unknown values
        // render as "-", never blank.
        const char *expectedDetailFields[] = {
            "Drive:", "Detected target:", "Model / label:", "Status:",
            "Size:", "Connection:", "Filesystem:", "UUID:", "Mounts:",
            "Protection:"
        };
        QListViewItem *detailRow = m_detailList->firstChild();
        for (int i = 0; i < 10 && detailRow;
             ++i, detailRow = detailRow->nextSibling()) {
            if (detailRow->text(0) != QString::fromLatin1(expectedDetailFields[i])) {
                problems->append(QString::fromLatin1(
                    "details panel row %1 is '%2', expected '%3'")
                    .arg(i + 1).arg(detailRow->text(0))
                    .arg(QString::fromLatin1(expectedDetailFields[i])));
                ok = false;
            }
            if (detailRow->text(1).isEmpty()) {
                problems->append(QString::fromLatin1(
                    "details value for '%1' is blank (must render '-')")
                    .arg(detailRow->text(0)));
                ok = false;
            }
        }
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
    if (!m_authorizeButton || !m_authStatusLabel) {
        problems->append(QString::fromLatin1("deferred-authorization controls missing"));
        ok = false;
    }
    if (!m_logFilterCombo
        || m_logFilterCombo->count() != logFilterSpecCount + diagnosticSpecCount) {
        problems->append(QString::fromLatin1(
            "log filter missing or not 1:1 with modern (all/diagnostics/repairs/"
            "3 workflows/16 sections)"));
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
    if (!m_repairVerticalSplitter
        || m_repairVerticalSplitter->orientation() != Qt::Vertical) {
        problems->append(QString::fromLatin1(
            "Repair vertical splitter missing or not vertical"));
        ok = false;
    }
    if (m_repairVerticalSplitter
        && m_repairVerticalSplitter->orientation() == Qt::Vertical) {
        // The modern Repair page keeps a draggable divider between the Full
        // Repair plan and the individual-tools panes; both panes must keep a
        // usable floor at 1024x768. Qt3's QSplitter exposes the panes through
        // sizes() (there is no count()/widget()).
        const QValueList<int> paneSizes = m_repairVerticalSplitter->sizes();
        if (paneSizes.count() != 2 || paneSizes[0] < 100 || paneSizes[1] < 200) {
            problems->append(QString::fromLatin1(
                "Repair vertical splitter panes are missing or too small "
                "(%1 panes: top=%2px, bottom=%3px)")
                .arg(paneSizes.count())
                .arg(paneSizes.count() > 0 ? paneSizes[0] : -1)
                .arg(paneSizes.count() > 1 ? paneSizes[1] : -1));
            ok = false;
        }
    }
    // B7 loop fix: the Systems horizontal splitter must keep the details pane
    // at its 372px floor (Field/Value columns 340px + margins) with the left
    // inventory pane absorbing the rest at 1024x768.
    if (m_systemsSplitter
        && m_systemsSplitter->orientation() == Qt::Horizontal) {
        const QValueList<int> paneSizes = m_systemsSplitter->sizes();
        if (paneSizes.count() != 2 || paneSizes[0] < 400 || paneSizes[1] < 360) {
            problems->append(QString::fromLatin1(
                "Systems splitter panes are missing or too small "
                "(%1 panes: left=%2px, details=%3px; the details pane needs "
                "its 372px floor)")
                .arg(paneSizes.count())
                .arg(paneSizes.count() > 0 ? paneSizes[0] : -1)
                .arg(paneSizes.count() > 1 ? paneSizes[1] : -1));
            ok = false;
        }
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
    // B7-7: the bootstack tool is the legacy one-pass stage, gated like the
    // other write tools (capability + host-maintenance on host scope) and
    // kept out of the Full Repair plan.
    {
        int bootstackRow = -1;
        int rowIndex = 0;
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling(), ++rowIndex) {
            if (row->text(0) == QString::fromLatin1("Boot stack reconciliation")) {
                bootstackRow = rowIndex;
                break;
            }
        }
        if (bootstackRow < 0) {
            problems->append(QString::fromLatin1("tool list rows missing (bootstack)"));
            ok = false;
        } else if (m_model.isAvailable("bootstack", toStd(identity()), 0)) {
            QListViewItem *bootItem = m_toolList->firstChild();
            for (int step = 0; bootItem && step < bootstackRow; ++step) {
                bootItem = bootItem->nextSibling();
            }
            m_toolList->setSelected(bootItem, true);
            m_toolList->setCurrentItem(bootItem);
            updateToolDetails();
            if (m_toolRunButton->text() != QString::fromLatin1("Reconcile Boot Stack")) {
                problems->append(QString::fromLatin1(
                    "bootstack tool button is '%1', expected Reconcile Boot Stack")
                    .arg(m_toolRunButton->text()));
                ok = false;
            }
            if (m_toolPlanStatus->text().find(
                    QString::fromLatin1("Full Repair plan: Manual recovery tool")) < 0) {
                problems->append(QString::fromLatin1(
                    "bootstack tool lost its plan status"));
                ok = false;
            }
            if (m_toolList->firstChild()) {
                m_toolList->setSelected(m_toolList->firstChild(), true);
                m_toolList->setCurrentItem(m_toolList->firstChild());
                updateToolDetails();
            }
        }
    }
    // Cycle 8: the File Copy tab matches modern's buildFileCopyPage — heading
    // row (title | scope label | Preview Changes | Copy and Verify), the
    // direction row, and the three titled groups (sources / destination with
    // Browse Target Folders... / ownership and copy policy).
    if (!m_fileCopyDirectionCombo || !m_fileCopySourceList
        || !m_fileCopyDestinationEdit || !m_fileCopyAddFilesButton
        || !m_fileCopyAddFolderButton || !m_fileCopyRemoveButton
        || !m_fileCopyClearButton || !m_fileCopyPreviewButton
        || !m_fileCopyRunButton || !m_fileCopyScopeLabel
        || !m_fileCopyBrowseButton || !m_fileCopyOwnershipCombo
        || !m_fileCopySourceGroup || !m_fileCopyDestinationGroup
        || !m_fileCopyOptionsGroup) {
        problems->append(QString::fromLatin1("File Copy controls missing"));
        ok = false;
    } else {
        if (m_fileCopyDirectionCombo->count() != 2) {
            problems->append(QString::fromLatin1(
                "File Copy direction combo must carry Host -> Repair / Repair -> Host"));
            ok = false;
        }
        if (m_fileCopyOwnershipCombo->count() != 2
            || m_fileCopyOwnershipCombo->text(0) != QString::fromLatin1(
                "Smart destination ownership (recommended)")
            || m_fileCopyOwnershipCombo->text(1) != QString::fromLatin1(
                "Preserve source numeric UID/GID")) {
            problems->append(QString::fromLatin1(
                "File Copy ownership combo lost the legacy ownership set"));
            ok = false;
        }
        const char *const expectedTitles[] = {
            "1. Select source files or folders from this host",
            "2. Choose destination in repaired system",
            "3. Ownership and copy policy"
        };
        for (std::size_t t = 0; t < sizeof(expectedTitles) / sizeof(expectedTitles[0]); ++t) {
            bool found = false;
            for (std::size_t i = 0; i < m_sectionTitles.size(); ++i) {
                if (m_sectionTitles[i]
                    && m_sectionTitles[i]->text() == QString::fromLatin1(expectedTitles[t])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                problems->append(QString::fromLatin1(
                    "File Copy group title missing: '%1'")
                    .arg(QString::fromLatin1(expectedTitles[t])));
                ok = false;
            }
        }
        if (m_fileCopyScopeLabel->text() != QString::fromLatin1("Target: none selected")
            && m_fileCopyScopeLabel->text().find(QString::fromLatin1("Host maintenance:")) < 0
            && m_fileCopyScopeLabel->text().find(QString::fromLatin1("Target:")) < 0) {
            problems->append(QString::fromLatin1(
                "File Copy scope label lost the modern scope text"));
            ok = false;
        }
        // Browse Target Folders... reads the repair tree through the helper
        // and needs a committed target; in the smoke the scope is the
        // running host with no committed repair target, so it stays disabled
        // in the default Host -> Repair direction.
        if (m_fileCopyBrowseButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Browse Target Folders... must stay disabled without a "
                "committed repair target"));
            ok = false;
        }
    }
    // B7-7: Make Default beside Host Maintenance, gated by Host Maintenance +
    // the cached host-default probe + the session.
    if (!m_hostDefaultButton) {
        problems->append(QString::fromLatin1("Make Default button missing"));
        ok = false;
    } else if (m_hostDefaultButton->text() != QString::fromLatin1("Make Default")) {
        problems->append(QString::fromLatin1("Make Default button text changed"));
        ok = false;
    }
    // Cycle 9: the protected-host card carries the green-check identity line,
    // the storage line, the PROTECTED badge and the Details action; the host
    // is known in the smoke, so Details must be enabled.
    if (!m_hostCard || !m_hostSystemLabel || !m_hostStorageLabel
        || !m_hostProtectedBadge || !m_hostDetailsButton) {
        problems->append(QString::fromLatin1("protected-host card controls missing"));
        ok = false;
    } else {
        if (m_hostSystemLabel->text().isEmpty()
            || m_hostSystemLabel->text() == QString::fromLatin1("Detecting running system...")) {
            problems->append(QString::fromLatin1(
                "protected-host identity line was never populated"));
            ok = false;
        }
        if (m_hostStorageLabel->text().isEmpty()
            || m_hostStorageLabel->text() == QString::fromLatin1("Detecting protected storage...")) {
            problems->append(QString::fromLatin1(
                "protected-host storage line was never populated"));
            ok = false;
        }
        if (m_hostProtectedBadge->text() != QString::fromLatin1("PROTECTED")) {
            problems->append(QString::fromLatin1("PROTECTED badge text changed"));
            ok = false;
        }
        if (m_hostDetailsButton->text() != QString::fromLatin1("Details")) {
            problems->append(QString::fromLatin1("Details button text changed"));
            ok = false;
        }
        if (!m_hostDetailsButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Details must be enabled while the running host is known"));
            ok = false;
        }
    }
    // Cycle 9: the device tree starts collapsed (disks rolled up).
    if (m_deviceList) {
        for (QListViewItem *diskItem = m_deviceList->firstChild(); diskItem;
             diskItem = diskItem->nextSibling()) {
            if (diskItem->isOpen()) {
                problems->append(QString::fromLatin1(
                    "device tree row '%1' must start collapsed")
                    .arg(diskItem->text(0)));
                ok = false;
            }
        }
    }
    // Cycle 9: the Diagnostics run button offers Run/Re-run Diagnostic; the
    // scope label is the two-line modern form (Host maintenance: over the
    // disk path in the smoke's host scope).
    if (m_runDiagnosticButton) {
        const QString runText = m_runDiagnosticButton->text();
        if (runText != QString::fromLatin1("Run Diagnostic")
            && runText != QString::fromLatin1("Re-run Diagnostic")) {
            problems->append(QString::fromLatin1(
                "diagnostic run button lost the Run/Re-run text ('%1')")
                .arg(runText));
            ok = false;
        }
    }
    if (m_scopeLabel
        && m_scopeLabel->text().find(QString::fromLatin1("Host maintenance:\n")) < 0) {
        problems->append(QString::fromLatin1(
            "scope label lost the two-line Host maintenance form"));
        ok = false;
    }
    // Cycle 14 loop: manual header-click sorting keeps the curated order by
    // default; clicking the Tool column sorts asc then desc, and the
    // Selected tool pane follows. Afterwards the curated order is restored
    // (detach + re-insert per toolSpecs) so later checks stay valid.
    if (m_toolList && m_toolList->header() && m_toolList->childCount() > 1
        && m_toolList->firstChild()) {
        // Cycle 14 loop: the sort case is deterministic — the pane-follow
        // invariant compares the pane against the CURRENT first row's spec
        // (looked up by the row's text, never a hard-coded expectation), and
        // the restore rebuilds the curated order from toolSpecs.
        const QString firstBefore = m_toolList->firstChild()->text(0);
        toolListHeaderClicked(0);
        if (!m_toolList->firstChild()) {
            problems->append(QString::fromLatin1(
                "tools list became empty after the first header click"));
            ok = false;
        } else {
            if (m_toolList->firstChild()->text(0) == firstBefore) {
                problems->append(QString::fromLatin1(
                    "tools list did not reorder on the first header click"));
                ok = false;
            }
            // Pane-follow invariant: title/button must match the current
            // first row's spec (the row's text IS the spec title).
            int firstSpec = -1;
            const QString firstRowText = m_toolList->firstChild()->text(0);
            for (int i = 0; i < toolSpecCount; ++i) {
                if (QString::fromLatin1(toolSpecs[i].title) == firstRowText) {
                    firstSpec = i;
                    break;
                }
            }
            if (firstSpec < 0) {
                problems->append(QString::fromLatin1(
                    "sorted first row has no tool spec"));
                ok = false;
            } else {
                if (m_toolTitle && m_toolTitle->text()
                    != QString::fromLatin1(toolSpecs[firstSpec].title)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool pane did not follow the sorted selection"));
                    ok = false;
                }
                if (m_toolRunButton && m_toolRunButton->text()
                    != QString::fromLatin1(toolSpecs[firstSpec].button)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool button did not follow the sorted selection"));
                    ok = false;
                }
            }
            const QString firstAfterFirstClick = m_toolList->firstChild()->text(0);
            toolListHeaderClicked(0);   // descending (toggle)
            if (!m_toolList->firstChild()) {
                problems->append(QString::fromLatin1(
                    "tools list became empty after the toggle click"));
                ok = false;
            } else {
                if (m_toolList->firstChild()->text(0) == firstAfterFirstClick) {
                    problems->append(QString::fromLatin1(
                        "tools list did not toggle on the second header click"));
                    ok = false;
                }
                const QString toggledRowText = m_toolList->firstChild()->text(0);
                int toggledSpec = -1;
                for (int i = 0; i < toolSpecCount; ++i) {
                    if (QString::fromLatin1(toolSpecs[i].title) == toggledRowText) {
                        toggledSpec = i;
                        break;
                    }
                }
                if (toggledSpec >= 0 && m_toolTitle && m_toolTitle->text()
                    != QString::fromLatin1(toolSpecs[toggledSpec].title)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool pane did not follow the toggled selection"));
                    ok = false;
                }
            }
            // Restore the curated order by rebuilding the rows.
            m_toolList->clear();
            QListViewItem *lastTool = 0;
            for (int i = 0; i < toolSpecCount; ++i) {
                lastTool = new QListViewItem(m_toolList, lastTool,
                                             QString::fromLatin1(toolSpecs[i].title),
                                             QString::fromLatin1("not reported"));
            }
            m_toolSortColumn = -1;
            m_toolSortAscending = true;
            if (m_toolList->firstChild()) {
                m_toolList->setSelected(m_toolList->firstChild(), true);
                m_toolList->setCurrentItem(m_toolList->firstChild());
                updateToolDetails();
                if (m_toolList->firstChild()->text(0)
                    != QString::fromLatin1("Validate environment")) {
                    problems->append(QString::fromLatin1(
                        "tools list did not restore the curated order"));
                    ok = false;
                }
                if (m_toolTitle && m_toolTitle->text()
                    != QString::fromLatin1(toolSpecs[0].title)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool pane did not return to the first curated tool"));
                    ok = false;
                }
                if (m_toolRunButton && m_toolRunButton->text()
                    != QString::fromLatin1(toolSpecs[0].button)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool button did not return to the first curated tool"));
                    ok = false;
                }
            } else {
                problems->append(QString::fromLatin1(
                    "tools list became empty after the restore"));
                ok = false;
            }
        }
    }
    // Cycle 11: the Unlock button is in its default state in the smoke (no
    // unlock ran), and the Authorize affordance is hidden while the
    // administrator session is active (modern deferred-only parity).
    if (m_unlockButton
        && m_unlockButton->text() != QString::fromLatin1("Unlock")) {
        problems->append(QString::fromLatin1(
            "Unlock button is not in its default state ('%1')")
            .arg(m_unlockButton->text()));
        ok = false;
    }
    if (m_authStatusLabel && !m_authStatusLabel->isHidden()) {
        problems->append(QString::fromLatin1(
            "authorization status label must be hidden while the session is active"));
        ok = false;
    }
    if (m_authorizeButton && !m_authorizeButton->isHidden()) {
        problems->append(QString::fromLatin1(
            "Authorize button must be hidden while the session is active"));
        ok = false;
    }
    // Cycle 12: the unlock result's mapper rows are injected into the
    // inventory owned by the selected drive (the read-only scan cannot see
    // them), so they show indented under the drive with the helper-probed
    // fstype and the inventory-backed resolution can find them.
    {
        const QString fakeDisk = QString::fromLatin1("/dev/bootrepair-smoke-inject");
        DeviceRow dk;
        dk.name = "bootrepair-smoke-inject";
        dk.path = toStd(fakeDisk);
        dk.disk = true;
        m_rows.insert(fakeDisk, dk);
        m_inventory.push_back(dk);
        injectUnlockedMapperRows(fakeDisk,
                                 QString::fromLatin1("/dev/mapper/luks-smoke-inject"),
                                 QString::fromLatin1("/dev/mapper/smoke-inject-root"),
                                 QString::fromLatin1("ext3"),
                                 QString::fromLatin1("smoke-uuid"));
        QListViewItem *injectedDisk = 0;
        for (QListViewItem *item = m_deviceList->firstChild(); item;
             item = item->nextSibling()) {
            if (item->text(0) == fakeDisk) {
                injectedDisk = item;
                break;
            }
        }
        if (!injectedDisk || injectedDisk->childCount() != 2) {
            problems->append(QString::fromLatin1(
                "unlocked mapper rows were not added under the selected drive"));
            ok = false;
        } else {
            for (QListViewItem *child = injectedDisk->firstChild(); child;
                 child = child->nextSibling()) {
                if (child->text(0) == QString::fromLatin1(
                        "/dev/mapper/smoke-inject-root")
                    && child->text(3) != QString::fromLatin1("ext3")) {
                    problems->append(QString::fromLatin1(
                        "unlocked root row lost the helper-probed fstype ('%1')")
                        .arg(child->text(3)));
                    ok = false;
                }
            }
            const QMap<QString, DeviceRow>::const_iterator rootRow =
                m_rows.find(QString::fromLatin1("/dev/mapper/smoke-inject-root"));
            if (rootRow == m_rows.end()
                || fromStd(rootRow.data().uuid) != QString::fromLatin1("smoke-uuid")) {
                problems->append(QString::fromLatin1(
                    "unlocked root row lost the helper-probed UUID"));
                ok = false;
            }
        }
        m_rows.remove(fakeDisk);
        for (std::vector<DeviceRow>::iterator it = m_inventory.begin();
             it != m_inventory.end(); ++it) {
            if (fromStd(it->path) == fakeDisk) {
                m_inventory.erase(it);
                break;
            }
        }
        m_rows.remove(QString::fromLatin1("/dev/mapper/luks-smoke-inject"));
        m_rows.remove(QString::fromLatin1("/dev/mapper/smoke-inject-root"));
        rebuildDeviceList();
    }
    // Cycle 9: the guarded-repair badge reaches the window's right edge.
    if (m_headerBadge) {
        const QRect badgeRect(m_headerBadge->mapTo(this, QPoint(0, 0)),
                              m_headerBadge->size());
        if (badgeRect.right() < width() - 2) {
            problems->append(QString::fromLatin1(
                "GUARDED REPAIR badge does not reach the window edge "
                "(right=%1, window=%2)")
                .arg(badgeRect.right()).arg(width()));
            ok = false;
        }
    }
    // B7-3: the unlock status pane keeps the modern field order.
    if (m_unlockStatusView) {
        const QString statusText = m_unlockStatusView->text();
        if (statusText.contains(QString::fromLatin1("State:"))
            && statusText.contains(QString::fromLatin1("Component:"))
            && statusText.contains(QString::fromLatin1("Mapper:"))
            && statusText.contains(QString::fromLatin1("Method:"))) {
            // Modern field order State -> Component -> Mapper -> Method.
            const int statePos = statusText.find(QString::fromLatin1("State:"));
            const int componentPos = statusText.find(QString::fromLatin1("Component:"));
            const int mapperPos = statusText.find(QString::fromLatin1("Mapper:"));
            const int methodPos = statusText.find(QString::fromLatin1("Method:"));
            if (!(statePos >= 0 && statePos < componentPos
                  && componentPos < mapperPos && mapperPos < methodPos)) {
                problems->append(QString::fromLatin1(
                    "unlock status fields are not in the modern order"));
                ok = false;
            }
        }
    }
    if (m_planChecks.size() != static_cast<std::size_t>(planSpecCount)) {
        problems->append(QString::fromLatin1(
            "Settings Full Repair plan checkboxes missing (found %1)")
            .arg(static_cast<int>(m_planChecks.size())));
        ok = false;
    } else {
        // Modern availability semantics: an available stage is checkable
        // (with its saved/default preference), an unavailable stage stays
        // SHOWN but disabled AND unchecked (the row set is never hidden).
        for (int i = 0; i < planSpecCount; ++i) {
            if (!m_planChecks[i]) {
                problems->append(QString::fromLatin1(
                    "Full Repair plan checkbox missing: %1")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel)));
                ok = false;
                continue;
            }
            QString reason;
            const bool available = planStageAvailable(i, &reason);
            if (m_planChecks[i]->isEnabled() != available) {
                problems->append(QString::fromLatin1(
                    "Full Repair plan checkbox enablement does not match its "
                    "availability: %1 (enabled=%2 available=%3)")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel))
                    .arg(m_planChecks[i]->isEnabled()
                             ? QString::fromLatin1("yes")
                             : QString::fromLatin1("no"))
                    .arg(available ? QString::fromLatin1("yes")
                                   : QString::fromLatin1("no")));
                ok = false;
            }
            if (!available && m_planChecks[i]->isChecked()) {
                problems->append(QString::fromLatin1(
                    "unavailable Full Repair plan checkbox stays checked: %1")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel)));
                ok = false;
            }
        }
        // Host-scope smoke parity: the legacy SysV display-manager capability
        // is available on this Etch host, so its plan row must be present,
        // checkable and off by default (the modern per-key default).
        const int displayPlan = planIndexForCapability("display");
        if (displayPlan < 0 || !m_planChecks[displayPlan]
            || !m_planChecks[displayPlan]->isEnabled()
            || m_planChecks[displayPlan]->isChecked()) {
            problems->append(QString::fromLatin1(
                "display-manager plan row is not a checkable off-by-default "
                "row on the host scope"));
            ok = false;
        }
        // The file system repair row is the documented frontend gap: it is
        // shown but never checkable, even though the capability is available.
        const int filesystemPlan = planIndexForCapability("filesystem");
        if (filesystemPlan < 0 || !m_planChecks[filesystemPlan]
            || m_planChecks[filesystemPlan]->isEnabled()
            || m_planChecks[filesystemPlan]->isChecked()) {
            problems->append(QString::fromLatin1(
                "file system repair plan row is not the disabled frontend-gap "
                "row (shown, never checkable)"));
            ok = false;
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
                // The column-mirror check is state-independent: it pins the
                // checkbox OFF then ON (restoring the prior state afterwards),
                // so a persisted non-default plan override can never invert
                // the expected texts.
                const bool original = m_planChecks[dpkgPlan]->isChecked();
                m_planChecks[dpkgPlan]->setChecked(false);
                const QString offText = dpkgItem->text(1);
                m_planChecks[dpkgPlan]->setChecked(true);
                const QString onText = dpkgItem->text(1);
                m_planChecks[dpkgPlan]->setChecked(original);
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
            // The display tool is the legacy host-scope stage: it runs only
            // with the display capability line available, Host Maintenance
            // active and the host-maintenance feature available; the offline
            // scope keeps it disabled with the host-scope reason.
            int displayRow = -1;
            rowIndex = 0;
            for (QListViewItem *row = m_toolList->firstChild(); row;
                 row = row->nextSibling(), ++rowIndex) {
                if (row->text(0) == QString::fromLatin1("Graphical login / display manager")) {
                    displayRow = rowIndex;
                    break;
                }
            }
            if (displayRow < 0) {
                problems->append(QString::fromLatin1("tool list rows missing (display)"));
                ok = false;
            } else {
                QListViewItem *displayItem = m_toolList->firstChild();
                for (int step = 0; displayItem && step < displayRow; ++step) {
                    displayItem = displayItem->nextSibling();
                }
                m_toolList->setSelected(displayItem, true);
                m_toolList->setCurrentItem(displayItem);
                updateToolDetails();
                if (m_toolRunButton->text() != QString::fromLatin1("Restore Graphical Login")) {
                    problems->append(QString::fromLatin1(
                        "display tool button is '%1', expected Restore Graphical Login")
                        .arg(m_toolRunButton->text()));
                    ok = false;
                }
                std::string displayReason;
                const bool displayCapable = m_model.isAvailable(
                    "display", toStd(identity()), &displayReason);
                std::string maintenanceReason;
                const bool maintenanceAvailable = m_model.legacyFeatureAvailable(
                    "host-maintenance", toStd(identity()), &maintenanceReason);
                const bool displayExpected = displayCapable && maintenanceAvailable
                    && m_hostMaintenance;
                if (m_toolRunButton->isEnabled() != displayExpected) {
                    problems->append(QString::fromLatin1(
                        "display tool runnable=%1 but capability=%2 host-maintenance=%3")
                        .arg(m_toolRunButton->isEnabled()
                                 ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                        .arg(displayCapable
                                 ? QString::fromLatin1("available") : QString::fromLatin1("unavailable"))
                        .arg(maintenanceAvailable
                                 ? QString::fromLatin1("available") : QString::fromLatin1("unavailable")));
                    ok = false;
                }
                // Offline scope: the host-only stage stays disabled and its
                // reason names Host Maintenance.
                const bool savedMaintenance = m_hostMaintenance;
                m_hostMaintenance = false;
                updateActionStates();
                updateToolDetails();
                if (m_toolRunButton->isEnabled()) {
                    problems->append(QString::fromLatin1(
                        "display tool enabled for the offline target scope"));
                    ok = false;
                }
                if (m_toolPlanStatus->text().find(QString::fromLatin1("Host Maintenance")) < 0) {
                    problems->append(QString::fromLatin1(
                        "display tool offline reason does not name Host Maintenance"));
                    ok = false;
                }
                m_hostMaintenance = savedMaintenance;
                updateActionStates();
                updateToolDetails();
            }
            // Every tool title plus the placeholder must fit the Selected tool
            // pane at 1024x768.  The assertions are deterministic: the title
            // wraps by construction (Qt::WordBreak + an Expanding horizontal
            // policy), the pane keeps its 300px floor, and after each title
            // is set the label must actually fill the header row (width >=
            // pane - Run button - tolerance), which is what lets long titles
            // wrap inside the full row width in the real (visible-page)
            // rendering.  A wrap-height threshold is deliberately not
            // asserted: Qt3's WordBreak sizeHint/height behavior is
            // style/font-dependent and unreliable headless.
            if (!(m_toolTitle->alignment() & Qt::WordBreak)) {
                problems->append(QString::fromLatin1(
                    "Selected tool title does not wrap (Qt::WordBreak)"));
                ok = false;
            }
            if (m_toolTitle->sizePolicy().horData() != QSizePolicy::Expanding) {
                problems->append(QString::fromLatin1(
                    "Selected tool title lacks the Expanding horizontal policy"));
                ok = false;
            }
            QStringList titleTexts;
            titleTexts.append(QString::fromLatin1("Select a repair tool"));
            for (int t = 0; t < toolSpecCount; ++t) {
                titleTexts.append(QString::fromLatin1(toolSpecs[t].title));
            }
            QWidget *titlePane = m_toolTitle->parentWidget();
            // The wrap measurements need the Repair page laid out for real:
            // while the page sits on a hidden tab the Etch Qt3 does not
            // reliably re-run its layouts from processEvents() alone, so the
            // WordBreak row never re-asks its heightForWidth and stays one
            // line at a stale width.  Switch to the page, flush the queued
            // events and explicitly activate every layout in the label's
            // ancestor chain (bottom-up, so the page's top-level layout runs
            // last and re-queries the nested boxes' heightForWidth).
            const int pageBeforeToolTitles = m_tabs->currentPageIndex();
            m_tabs->setCurrentPage(2);
            qApp->sendPostedEvents();
            qApp->processEvents();
            for (QWidget *ancestor = m_toolTitle->parentWidget(); ancestor;
                 ancestor = ancestor->parentWidget()) {
                if (ancestor->layout()) {
                    ancestor->layout()->activate();
                }
            }
            qApp->processEvents();
            if (titlePane->width() < 300) {
                // The pane floor (setMinimumWidth(300)) proves the page did
                // not lay out; report the exact geometry instead of letting
                // the per-title checks fire with a stale-size false positive.
                problems->append(QString::fromLatin1(
                    "Selected tool pane did not lay out (%1px wide, needs the "
                    "300px floor); page=%2 window=%3x%4")
                    .arg(titlePane->width())
                    .arg(m_tabs->currentPageIndex())
                    .arg(width()).arg(height()));
                ok = false;
            }
            for (QStringList::ConstIterator it = titleTexts.begin();
                 it != titleTexts.end(); ++it) {
                m_toolTitle->setText(*it);
                // Two passes deliver the layout invalidation; the ancestor
                // activation then forces the header row to recompute for the
                // new text even on a freshly shown page.
                qApp->processEvents();
                qApp->processEvents();
                for (QWidget *ancestor = m_toolTitle->parentWidget(); ancestor;
                     ancestor = ancestor->parentWidget()) {
                    if (ancestor->layout()) {
                        ancestor->layout()->activate();
                    }
                }
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
                // The key deterministic check: the Expanding title must fill
                // the header row up to the Run button (no stretch item sits
                // between them), so a stale sizeHint width can never truncate
                // the row; long titles then wrap inside the full row width in
                // the real rendering.
                if (m_toolTitle->width()
                    < titlePane->width() - m_toolRunButton->width() - 30) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' does not fill the header row "
                        "(%2px wide; pane %3px, run button %4px)")
                        .arg(*it).arg(m_toolTitle->width())
                        .arg(titlePane->width())
                        .arg(m_toolRunButton->width()));
                    ok = false;
                }
            }
            // Restore the previous tab and the first tool selection.
            m_tabs->setCurrentPage(pageBeforeToolTitles);
            qApp->processEvents();
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
        // (a read-only home must not fail a correct GUI). Under --smoke-test
        // isolation the user's files are never read or written, so the
        // round-trip probe is skipped and the deterministic defaults stand.
        if (!legacySmokeSettingsIsolation()) {
            QSettings probe;
            probe.setPath(QString::fromLatin1("boot-bitch"),
                          QString::fromLatin1("boot-repair"), QSettings::User);
            const bool settingsWritable = probe.writeEntry(
                QString::fromLatin1("/logs/wrapLines"), m_logWrapEnabled);
            if (settingsWritable) {
                QSettings readBack;
                readBack.setPath(QString::fromLatin1("boot-bitch"),
                                 QString::fromLatin1("boot-repair"), QSettings::User);
                if (readBack.readBoolEntry(QString::fromLatin1("/logs/wrapLines"),
                                           !m_logWrapEnabled) != m_logWrapEnabled) {
                    problems->append(QString::fromLatin1(
                        "Wrap Log Lines state was not persisted through QSettings"));
                    ok = false;
                }
            }
        }
    }
    // Cycle 9 loop 3: the smoke must run under settings isolation (set by
    // main() before the window was constructed); otherwise a persisted
    // ~/.qt/repairrc etc. could flip the assertions above.
    if (!legacySmokeSettingsIsolation()) {
        problems->append(QString::fromLatin1(
            "smoke ran without the settings isolation"));
        ok = false;
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
    if (!m_capDistributionLabel || !m_capPackageManagerLabel || !m_capAuthLabel
        || !m_capabilityTable || !m_refreshCapabilitiesButton
        || !m_installSupportButton) {
        problems->append(QString::fromLatin1("host capabilities group missing"));
        ok = false;
    }
    // Modern 1:1 capability table: six columns, the 23 probe rows plus the
    // one legacy-specific row, and the disabled Install Missing Support...
    // button with the modern tooltip.
    if (m_capabilityTable) {
        if (m_capabilityTable->numCols() != 6
            || m_capabilityTable->numRows() != capabilitySpecCount) {
            problems->append(QString::fromLatin1(
                "host capability table is %1x%2, expected 24x6")
                .arg(m_capabilityTable->numRows())
                .arg(m_capabilityTable->numCols()));
            ok = false;
        } else {
            const char *expectedHeaders[] = {
                "Feature", "Command", "Scope", "Status",
                "Suggested package", "Notes"
            };
            for (int column = 0; column < 6; ++column) {
                if (m_capabilityTable->horizontalHeader()->label(column)
                    != QString::fromLatin1(expectedHeaders[column])) {
                    problems->append(QString::fromLatin1(
                        "capability table header %1 changed").arg(column + 1));
                    ok = false;
                }
            }
            bool unshareRow = false;
            for (int row = 0; row < capabilitySpecCount; ++row) {
                if (m_capabilityTable->text(row, 0)
                    == QString::fromLatin1("Process namespace isolation")) {
                    unshareRow = true;
                    if (m_capabilityTable->text(row, 1)
                        != QString::fromLatin1("unshare")) {
                        problems->append(QString::fromLatin1(
                            "unshare capability row lost its command"));
                        ok = false;
                    }
                }
            }
            if (!unshareRow) {
                problems->append(QString::fromLatin1(
                    "legacy unshare capability row missing"));
                ok = false;
            }
        }
        // Full-row selection (modern parity): clicking any cell selects its
        // row, and the first row starts selected after every refresh.
        if (m_capabilityTable->selectionMode() != QTable::SingleRow) {
            problems->append(QString::fromLatin1(
                "capability table is not in QTable::SingleRow selection mode"));
            ok = false;
        }
        refreshCapabilities();
        if (m_capabilityTable->numRows() > 0
            && !m_capabilityTable->isRowSelected(0)) {
            problems->append(QString::fromLatin1(
                "capability table does not select its first row after refresh"));
            ok = false;
        }
        if (m_capabilityTable->numRows() > 3) {
            m_capabilityTable->setCurrentCell(2, 3);
            qApp->processEvents();
            if (!m_capabilityTable->isRowSelected(2)) {
                problems->append(QString::fromLatin1(
                    "clicking a capability cell did not select its row"));
                ok = false;
            }
            m_capabilityTable->setCurrentCell(0, 0);
            m_capabilityTable->selectRow(0);
        }
    }
    if (m_capAuthLabel
        && m_capAuthLabel->text().find(QString::fromLatin1("no KAuth")) < 0) {
        problems->append(QString::fromLatin1(
            "adapted authorization-support label does not name the missing KAuth"));
        ok = false;
    }
    if (m_installSupportButton
        && m_installSupportButton->text() != QString::fromLatin1("Install Missing Support...")) {
        problems->append(QString::fromLatin1("Install Missing Support... text changed: '%1'")
                             .arg(m_installSupportButton->text()));
        ok = false;
    }
    if (m_installSupportButton && m_installSupportButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Install Missing Support... must stay disabled"));
        ok = false;
    }
    if (m_installSupportButton
        && QToolTip::textFor(m_installSupportButton).find(
               QString::fromLatin1("Automatic installation will require")) < 0) {
        problems->append(QString::fromLatin1(
            "Install Missing Support... lost the modern tooltip"));
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
            const std::vector<QListViewItem *> treeItems = deviceTreeItems();
            for (std::size_t ti = 0; ti < treeItems.size(); ++ti) {
                QListViewItem *item = treeItems[ti];
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

    // Filesystem column: every visible device row carries a label (disk rows
    // aggregate their children, component rows fall back through the
    // mounted/udev/[swap]/unknown chain) - the cell is never empty.
    if (m_deviceList) {
        const std::vector<QListViewItem *> treeItems = deviceTreeItems();
        for (std::size_t i = 0; i < treeItems.size(); ++i) {
            if (treeItems[i]->text(3).isEmpty()) {
                problems->append(QString::fromLatin1(
                    "device row %1 has an empty Filesystem cell")
                    .arg(treeItems[i]->text(0)));
                ok = false;
            }
        }
        // Cycle 15: the protected running host must NOT appear in the
        // candidate tree (the host card above shows it); only offline drives
        // are repair candidates.
        const QString hostDisk = runningHostDisk();
        if (!hostDisk.isEmpty()) {
            for (QListViewItem *item = m_deviceList->firstChild(); item;
                 item = item->nextSibling()) {
                if (item->text(0) == hostDisk) {
                    problems->append(QString::fromLatin1(
                        "the protected running host %1 must not appear in the "
                        "candidate tree").arg(hostDisk));
                    ok = false;
                }
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

    // B7-2: the device list is a tree — top-level items are disks only and
    // their children are the disk's partitions/mappers; exactly four columns.
    // Orphan non-disk rows (/dev/fd0 floppy stubs, /dev/hdc empty devices,
    // helper-reported pass-through rows with no resolvable owner) must never
    // appear top-level. Rows the inventory does not know are exempt here
    // (helper-reported devices merged straight into the list before a
    // rebuild); the builder itself drops every orphan.
    if (m_deviceList && m_deviceList->columns() != 4) {
        problems->append(QString::fromLatin1(
            "device tree must carry exactly Device/Size/Type/Filesystem columns"));
        ok = false;
    }
    if (m_deviceList) {
        for (QListViewItem *disk = m_deviceList->firstChild(); disk;
             disk = disk->nextSibling()) {
            const QMap<QString, DeviceRow>::const_iterator row =
                m_rows.find(disk->text(0));
            if (row == m_rows.end()) {
                // Helper-reported pass-through rows merged before a rebuild.
                continue;
            }
            if (!row.data().disk) {
                problems->append(QString::fromLatin1(
                    "device tree top-level row '%1' is not a disk")
                    .arg(disk->text(0)));
                ok = false;
                continue;
            }
            const QString diskPath = disk->text(0);
            for (QListViewItem *child = disk->firstChild(); child;
                 child = child->nextSibling()) {
                const QMap<QString, DeviceRow>::const_iterator childRow =
                    m_rows.find(child->text(0));
                if (childRow == m_rows.end()) {
                    continue;
                }
                if (childRow.data().disk
                    || owningDiskFor(childRow.data()) != diskPath) {
                    problems->append(QString::fromLatin1(
                        "device tree child '%1' does not belong to its disk '%2'")
                        .arg(child->text(0)).arg(diskPath));
                    ok = false;
                }
            }
        }
    }

    // B7-1: a drive with a locked LUKS component and an unencrypted Linux
    // partition must NOT resolve that partition (unlock-first; Select Target
    // disabled with the modern reason); after the mapped Linux root appears,
    // the mapper resolves and the locked candidate disappears.
    {
        const QString fakeDisk = QString::fromLatin1("/dev/bootrepair-smoke-luks");
        const QString fakePart = QString::fromLatin1("/dev/bootrepair-smoke-luks1");
        const QString fakeLuks = QString::fromLatin1("/dev/bootrepair-smoke-luks5");
        const QString fakeRoot = QString::fromLatin1("/dev/mapper/bootrepair-smoke-root");
        DeviceRow dk; dk.name = "bootrepair-smoke-luks"; dk.path = toStd(fakeDisk); dk.disk = true;
        DeviceRow part; part.name = "bootrepair-smoke-luks1"; part.path = toStd(fakePart);
        part.parent = "bootrepair-smoke-luks"; part.probedFstype = "ext3";
        DeviceRow luks; luks.name = "bootrepair-smoke-luks5"; luks.path = toStd(fakeLuks);
        luks.parent = "bootrepair-smoke-luks"; luks.encrypted = true;
        luks.probedFstype = "crypto_LUKS";
        const QString savedDisk = selectedDisk();
        const QString savedRoot = selectedRoot();
        m_rows.insert(fakeDisk, dk);
        m_rows.insert(fakePart, part);
        m_rows.insert(fakeLuks, luks);
        setTarget(fakeDisk, QString::null);
        updateActionStates();
        if (!autoResolvedRoot(fakeDisk).isEmpty()) {
            problems->append(QString::fromLatin1(
                "locked-LUKS drive resolved a plain partition root (unlock-first regression)"));
            ok = false;
        }
        if (m_setTargetButton && m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target enabled for a locked-LUKS drive"));
            ok = false;
        }
        if (m_setTargetButton
            && QToolTip::textFor(m_setTargetButton).find(
                   QString::fromLatin1("Unlock the encrypted volume first")) < 0) {
            problems->append(QString::fromLatin1(
                "Select Target unlock-first reason missing for a locked-LUKS drive"));
            ok = false;
        }
        DeviceRow mapped; mapped.name = "bootrepair-smoke-root"; mapped.path = toStd(fakeRoot);
        mapped.mapper = true; mapped.parent = "bootrepair-smoke-luks5";
        mapped.fstype = "ext3";
        m_rows.insert(fakeRoot, mapped);
        if (autoResolvedRoot(fakeDisk) != fakeRoot) {
            problems->append(QString::fromLatin1(
                "unlocked mapper root did not resolve after the LUKS child appeared"));
            ok = false;
        }
        if (!autoResolvedLuks(fakeDisk).isEmpty()) {
            problems->append(QString::fromLatin1(
                "autoResolvedLuks still reports a locked candidate after unlock"));
            ok = false;
        }
        m_rows.remove(fakeDisk);
        m_rows.remove(fakePart);
        m_rows.remove(fakeLuks);
        m_rows.remove(fakeRoot);
        setTarget(savedDisk, savedRoot);
        updateActionStates();
    }

    // Cycle 13 loop: a root-less non-LUKS drive (the vfat share disk) must
    // show the no-Linux-filesystem reason — never the unlock-first wording.
    {
        const QString savedDisk2 = selectedDisk();
        const QString savedRoot2 = selectedRoot();
        const QString fakeVfat = QString::fromLatin1("/dev/bootrepair-smoke-vfat");
        DeviceRow vfat;
        vfat.name = "bootrepair-smoke-vfat";
        vfat.path = toStd(fakeVfat);
        vfat.disk = true;
        vfat.probedFstype = "vfat";
        m_rows.insert(fakeVfat, vfat);
        m_inventory.push_back(vfat);
        const bool savedHostMaintenance = m_hostMaintenance;
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton,
                             QString::fromLatin1("Host Maintenance"));
        }
        setTarget(fakeVfat, QString::null);
        updateActionStates();
        if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target disabled for a selectable root-less vfat drive"));
            ok = false;
        }
        if (m_setTargetButton
            && QToolTip::textFor(m_setTargetButton).find(
                   QString::fromLatin1("Commit this physical drive as the repair target.")) < 0) {
            problems->append(QString::fromLatin1(
                "Select Target lost the commit tooltip for a vfat drive"));
            ok = false;
        }
        m_rows.remove(fakeVfat);
        for (std::vector<DeviceRow>::iterator it = m_inventory.begin();
             it != m_inventory.end(); ++it) {
            if (fromStd(it->path) == fakeVfat) {
                m_inventory.erase(it);
                break;
            }
        }
        setTarget(savedDisk2, savedRoot2);
        m_hostMaintenance = savedHostMaintenance;
        updateActionStates();
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
            && !m_scopeLabel->text().contains(QString::fromLatin1(
                "Target:\n/dev/bootrepair-smoke-target + /dev/bootrepair-smoke-target1"))) {
            problems->append(QString::fromLatin1(
                "committed scope label is '%1'").arg(m_scopeLabel->text()));
            ok = false;
        }
        // Cycle 15 (modern parity): the committed drive keeps Select Target
        // enabled with the committed tooltip; a different selectable drive
        // switches the target.
        updateActionStates();
        if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target disabled for the committed drive"));
            ok = false;
        }
        if (m_setTargetButton
            && QToolTip::textFor(m_setTargetButton).find(
                   QString::fromLatin1("Committed repair target")) < 0) {
            problems->append(QString::fromLatin1(
                "Select Target lost the committed tooltip"));
            ok = false;
        }
        setTarget(QString::fromLatin1("/dev/bootrepair-smoke-other"),
                  QString::fromLatin1("/dev/bootrepair-smoke-other1"));
        updateActionStates();
        if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target disabled for a different selectable drive"));
            ok = false;
        }
        m_targetCommitted = false;
        m_committedDisk = QString::null;
        m_committedRoot = QString::null;
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
        && !m_scopeLabel->text().startsWith(QString::fromLatin1("Host maintenance:\n"))) {
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
        && !m_scopeLabel->text().contains(QString::fromLatin1("Target:\n"))) {
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
    // Cycle 11 deferred-only parity: in the authorized (root) smoke state the
    // Authorize affordance must be HIDDEN (its text may be empty by design);
    // it appears only in the deferred state (a ready scope without an active
    // session). The smoke runs as root with the session permanently active,
    // so the deferred/shown state cannot be exercised here.
    if (m_authStatusLabel && !m_authStatusLabel->isHidden()) {
        problems->append(QString::fromLatin1(
            "authorization status label must be hidden while the session is active"));
        ok = false;
    }
    if (m_authorizeButton && !m_authorizeButton->isHidden()) {
        problems->append(QString::fromLatin1(
            "Authorize button must be hidden while the session is active"));
        ok = false;
    }

    // Probe-based legacy feature gating (fail closed): the chroot/file-copy
    // probe reasons and the host-maintenance-gated tools must follow the
    // cached `Legacy feature` lines, never hardcoded text.
    const QString shellFeature = scopeFeatureKey();
    if (m_chrootReasonLabel
        && !m_chrootReasonLabel->text().contains(
               QString::fromLatin1("Run a command on the running host as root"))) {
        problems->append(QString::fromLatin1(
            "Chroot Shell notice lost the modern host-scope wording"));
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

    // Log filter (modern 1:1): All entries / Diagnostics / Repairs / the three
    // workflows / the 16 diagnostic sections, plus the capture-time kind
    // tagging: the smoke's real host-diagnose lines are Diagnostic entries
    // tagged per `Diagnostic: <key>` marker and the host-validate lines are
    // Repair entries with the validate stage.
    if (m_logFilterCombo && m_logView) {
        m_viewingPriorLog = false;
        if (m_logFilterCombo->text(0) != QString::fromLatin1("All entries")
            || m_logFilterCombo->text(1) != QString::fromLatin1("Diagnostics")
            || m_logFilterCombo->text(2) != QString::fromLatin1("Repairs")
            || m_logFilterCombo->text(3) != QString::fromLatin1("File system repair")
            || m_logFilterCombo->text(4) != QString::fromLatin1("Package repair")
            || m_logFilterCombo->text(5) != QString::fromLatin1("File copy")
            || m_logFilterCombo->text(6) != QString::fromLatin1(diagnosticSpecs[0].title)
            || m_logFilterCombo->text(logFilterSpecCount + diagnosticSpecCount - 1)
                   != QString::fromLatin1(diagnosticSpecs[diagnosticSpecCount - 1].title)) {
            problems->append(QString::fromLatin1(
                "log filter combo is not 1:1 with modern order"));
            ok = false;
        }
        // Synthetic tagged entries: the workflow and section mappings are
        // deterministic even when the helper transcript varies.
        LogEntry fsEntry;
        fsEntry.text = QString::fromLatin1("SMOKE-KIND fs-inspect synthetic line");
        fsEntry.kind = LogEntry::Repair;
        fsEntry.stages = QString::fromLatin1("fs-inspect");
        LogEntry pkgEntry;
        pkgEntry.text = QString::fromLatin1("SMOKE-KIND fix-broken synthetic line");
        pkgEntry.kind = LogEntry::Repair;
        pkgEntry.stages = QString::fromLatin1("fix-broken");
        LogEntry diagEntry;
        diagEntry.text = QString::fromLatin1("SMOKE-KIND environment synthetic line");
        diagEntry.kind = LogEntry::Diagnostic;
        diagEntry.diagnostic = QString::fromLatin1("environment");
        m_logEntries.push_back(fsEntry);
        m_logEntries.push_back(pkgEntry);
        m_logEntries.push_back(diagEntry);

        struct FilterExpectation {
            int index;              // combo row
            const char *mustShow;   // substring that must appear
            const char *mustHide;   // substring that must not appear
        };
        const FilterExpectation expectations[] = {
            { 1, "SMOKE-KIND environment synthetic line", "SMOKE-KIND fs-inspect synthetic line" },
            { 2, "SMOKE-KIND fs-inspect synthetic line", "SMOKE-KIND environment synthetic line" },
            { 3, "SMOKE-KIND fs-inspect synthetic line", "SMOKE-KIND fix-broken synthetic line" },
            { 4, "SMOKE-KIND fix-broken synthetic line", "SMOKE-KIND fs-inspect synthetic line" },
            { 5, "", "SMOKE-KIND" },
            { 6, "SMOKE-KIND environment synthetic line", "SMOKE-KIND fix-broken synthetic line" }
        };
        for (int i = 0; i < 6; ++i) {
            m_logFilterCombo->setCurrentItem(expectations[i].index);
            refreshLogView();
            const QString view = m_logView->text();
            if (expectations[i].mustShow[0] != '\0'
                && view.find(QString::fromLatin1(expectations[i].mustShow)) < 0) {
                problems->append(QString::fromLatin1(
                    "log filter row %1 did not show its entries").arg(expectations[i].index));
                ok = false;
            }
            if (expectations[i].mustHide[0] != '\0'
                && view.find(QString::fromLatin1(expectations[i].mustHide)) >= 0) {
                problems->append(QString::fromLatin1(
                    "log filter row %1 leaked entries").arg(expectations[i].index));
                ok = false;
            }
        }
        // The real stream markers tagged the smoke diagnostics per section:
        // pick the first section that really appears in the register and
        // verify its filter shows the helper's marker line while the
        // repair/validate lines stay hidden.
        int sectionRow = -1;
        QString sectionMarker;
        for (int d = 0; d < diagnosticSpecCount; ++d) {
            const QString key = QString::fromLatin1(diagnosticSpecs[d].key);
            bool seen = false;
            for (std::size_t e = 0; e < m_logEntries.size() && !seen; ++e) {
                if (m_logEntries[e].kind == LogEntry::Diagnostic
                    && m_logEntries[e].diagnostic == key) {
                    seen = true;
                }
            }
            if (seen) {
                sectionRow = logFilterSpecCount + d;
                sectionMarker = QString::fromLatin1("Diagnostic: %1").arg(key);
                break;
            }
        }
        if (sectionRow < 0) {
            problems->append(QString::fromLatin1(
                "smoke diagnostics did not tag any diagnostic section"));
            ok = false;
        } else {
            m_logFilterCombo->setCurrentItem(sectionRow);
            refreshLogView();
            if (m_logView->text().find(sectionMarker) < 0) {
                problems->append(QString::fromLatin1(
                    "diagnostic section filter did not show the tagged section"));
                ok = false;
            }
            if (m_logView->text().find(QString::fromLatin1("=== smoke host-validate ===")) >= 0) {
                problems->append(QString::fromLatin1(
                    "diagnostic section filter leaked the repair lines"));
                ok = false;
            }
        }
        // The Repairs filter shows the smoke validate run; the Package repair
        // workflow does not (validate maps to no workflow).
        m_logFilterCombo->setCurrentItem(2);
        refreshLogView();
        if (m_logView->text().find(QString::fromLatin1("=== smoke host-validate ===")) < 0) {
            problems->append(QString::fromLatin1(
                "Repairs filter did not show the repair entries"));
            ok = false;
        }
        m_logFilterCombo->setCurrentItem(4);
        refreshLogView();
        if (m_logView->text().find(QString::fromLatin1("=== smoke host-validate ===")) >= 0) {
            problems->append(QString::fromLatin1(
                "Package repair workflow leaked the validate stage"));
            ok = false;
        }
        // Remove the synthetic entries so they never reach a saved log.
        m_logEntries.pop_back();
        m_logEntries.pop_back();
        m_logEntries.pop_back();
        m_logFilterCombo->setCurrentItem(0);
        refreshLogView();
        if (!m_logView->text().contains(QString::fromLatin1("Diagnostic: "))
            || !m_logView->text().contains(QString::fromLatin1("=== smoke host-validate ==="))) {
            problems->append(QString::fromLatin1(
                "All entries did not restore the complete register"));
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

    // B7-4: the host-validate run produced the Repair-tagged summary block
    // with the unchanged Validate line and the no-repair-needed headline.
    {
        bool sawBracket = false;
        bool sawValidateLine = false;
        bool sawHeadline = false;
        for (std::size_t e = 0; e < m_logEntries.size(); ++e) {
            const LogEntry &entry = m_logEntries[e];
            if (entry.text.find(QString::fromLatin1("==== REPAIR ====")) >= 0) {
                sawBracket = true;
                if (entry.kind != LogEntry::Repair) {
                    problems->append(QString::fromLatin1(
                        "repair summary block is not Repair-tagged"));
                    ok = false;
                }
            }
            if (entry.text.find(QString::fromLatin1("[-] Validate - validation is read-only")) >= 0
                && entry.kind == LogEntry::Repair) {
                sawValidateLine = true;
            }
            if (entry.text.find(QString::fromLatin1("no repair was needed; cached diagnostics remain valid.")) >= 0) {
                sawHeadline = true;
            }
        }
        if (!sawBracket) {
            problems->append(QString::fromLatin1(
                "repair summary block missing after the validate run"));
            ok = false;
        }
        if (!sawValidateLine) {
            problems->append(QString::fromLatin1(
                "repair summary block lost the Validate stage line"));
            ok = false;
        }
        if (!sawHeadline) {
            problems->append(QString::fromLatin1(
                "repair summary headline lost the no-repair-needed tail"));
            ok = false;
        }
    }

    // Cycle 11 fix: a Full Repair summary must map the plan-stage keys to the
    // transcript's tool keys, so the four package stages render their real
    // outcomes instead of "not reported".
    {
        const QString fullRepairTranscript = QString::fromLatin1(
            "=== Full Repair ===\n"
            "Repair change status dpkg: changed\n"
            "Repair change status fixbroken: changed\n"
            "Repair change status aptupdate: unchanged|APT package lists are byte-identical and no repository index was fetched\n"
            "Repair change status upgrade: changed\n"
            "Repair change status initramfs: unchanged|already up to date\n"
            "Repair change status grub: changed\n");
        const ParsedTranscript fullParsed = parseTranscript(
            toStd(fullRepairTranscript));
        const QStringList savedStages = m_activeRepairStages;
        const QString savedPendingLabel = m_pendingLabel;
        const std::size_t savedEntries = m_logEntries.size();
        m_activeRepairStages = QStringList::split(QString::fromLatin1(" "),
            QString::fromLatin1("dpkg-configure fix-broken apt-update apt-upgrade initramfs grub"),
            false);
        m_pendingLabel = QString::fromLatin1("Full Repair");
        appendRepairSummaryBlock(fullParsed, true);
        QString blockText;
        for (std::size_t e = savedEntries; e < m_logEntries.size(); ++e) {
            blockText += m_logEntries[e].text;
            blockText += QString::fromLatin1("\n");
        }
        const char *const expectedLines[] = {
            "[OK] Complete interrupted package configuration - changed",
            "[OK] Repair broken package dependencies - changed",
            "[-] Refresh package metadata - APT package lists are byte-identical and no repository index was fetched",
            "[OK] Upgrade installed packages - changed",
            "[-] Initramfs - already up to date",
            "[OK] GRUB configuration - changed"
        };
        for (std::size_t e = 0; e < sizeof(expectedLines) / sizeof(expectedLines[0]); ++e) {
            if (blockText.find(QString::fromLatin1(expectedLines[e])) < 0) {
                problems->append(QString::fromLatin1(
                    "Full Repair summary lost the %1 stage outcome")
                    .arg(QString::fromLatin1(expectedLines[e])));
                ok = false;
            }
        }
        if (blockText.find(QString::fromLatin1("not reported")) >= 0) {
            problems->append(QString::fromLatin1(
                "Full Repair summary rendered 'not reported' for a real stage"));
            ok = false;
        }
        m_activeRepairStages = savedStages;
        m_pendingLabel = savedPendingLabel;
    }

    // Host-repair summary fix: a failed plan attributes the failure to the
    // stage the helper names (`ERROR: stage 'grub' failed:`) and keeps every
    // completed stage's own result (unchanged = no repair needed, changed =
    // successful) even though the overall command failed.
    {
        const QString failedTranscript = QString::fromLatin1(
            "Repair change status dpkg: unchanged|dpkg reported no packages pending configuration\n"
            "Repair change status fixbroken: unchanged|simulated fix-broken transaction proposed no package changes\n"
            "Repair change status aptupdate: changed\n"
            "Repair change status upgrade: unchanged|simulated upgrade transaction proposed no package changes\n"
            "Repair change status initramfs: changed\n"
            "[00:34:51] ERROR: stage 'grub' failed: GRUB legacy regeneration was rolled back because it removed an existing boot entry.\n");
        const ParsedTranscript failedParsed = parseTranscript(
            toStd(failedTranscript));
        const QStringList savedStages = m_activeRepairStages;
        const QString savedPendingLabel = m_pendingLabel;
        const QString savedTranscript = m_transcript;
        const std::size_t savedEntries = m_logEntries.size();
        m_activeRepairStages = QStringList::split(QString::fromLatin1(" "),
            QString::fromLatin1("dpkg-configure fix-broken apt-update apt-upgrade initramfs grub"),
            false);
        m_pendingLabel = QString::fromLatin1("Full Repair");
        m_transcript = failedTranscript;
        appendRepairSummaryBlock(failedParsed, false);
        QString failedBlockText;
        for (std::size_t e = savedEntries; e < m_logEntries.size(); ++e) {
            failedBlockText += m_logEntries[e].text;
            failedBlockText += QString::fromLatin1("\n");
        }
        const char *const failedExpectedLines[] = {
            "[OK] 2 successful | [FAIL] 1 failed | [-] 3 no repair needed",
            "[-] Complete interrupted package configuration - dpkg reported no packages pending configuration",
            "[-] Repair broken package dependencies - simulated fix-broken transaction proposed no package changes",
            "[OK] Refresh package metadata - changed",
            "[-] Upgrade installed packages - simulated upgrade transaction proposed no package changes",
            "[OK] Initramfs - changed",
            "[FAIL] GRUB configuration - GRUB legacy regeneration was rolled back because it removed an existing boot entry."
        };
        for (std::size_t e = 0;
             e < sizeof(failedExpectedLines) / sizeof(failedExpectedLines[0]); ++e) {
            if (failedBlockText.find(QString::fromLatin1(failedExpectedLines[e])) < 0) {
                problems->append(QString::fromLatin1(
                    "failed Full Repair summary lost the expected outcome: %1")
                    .arg(QString::fromLatin1(failedExpectedLines[e])));
                ok = false;
            }
        }
        if (failedBlockText.find(QString::fromLatin1("not reported")) >= 0) {
            problems->append(QString::fromLatin1(
                "failed Full Repair summary rendered 'not reported' for a "
                "stage whose status was reported"));
            ok = false;
        }
        m_activeRepairStages = savedStages;
        m_pendingLabel = savedPendingLabel;
        m_transcript = savedTranscript;
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
        if (m_capabilityTable && m_capabilityTable->isVisibleTo(m_tabs)) {
            ++checkedCount;
            QWidget *bound = layoutBoundFor(m_capabilityTable, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(m_capabilityTable->mapTo(bound, QPoint(0, 0)),
                               m_capabilityTable->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "capability table is clipped by the page (bound %1x%2)")
                    .arg(boundRect.width()).arg(boundRect.height()));
                ok = false;
            }
            if (m_capabilityTable->width() < 420) {
                problems->append(QString::fromLatin1(
                    "capability table is too narrow (%1px)").arg(m_capabilityTable->width()));
                ok = false;
            }
        }
        if (m_repairVerticalSplitter && m_repairVerticalSplitter->isVisibleTo(m_tabs)) {
            ++checkedCount;
            QWidget *bound = layoutBoundFor(m_repairVerticalSplitter, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(m_repairVerticalSplitter->mapTo(bound, QPoint(0, 0)),
                               m_repairVerticalSplitter->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "Repair vertical splitter is clipped by the page"));
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
            // Left-edge probe: the title's text must start at its container's
            // content-left (within the frame/layout margin), never centered
            // inside the row (the Repair-page centering regression). Qt3's
            // QWidget has no contentsRect(); the container is a QFrame
            // (QGroupBox) or a plain page.
            QWidget *container = title->parentWidget();
            if (container) {
                int contentLeft = 0;
                if (container->inherits("QFrame")) {
                    contentLeft = static_cast<QFrame *>(container)
                                      ->contentsRect().left();
                }
                const QPoint origin = title->mapTo(container, QPoint(0, 0));
                if (origin.x() < contentLeft - 2
                    || origin.x() > contentLeft + kSectionTitleLeftTolerance) {
                    problems->append(QString::fromLatin1(
                        "section title '%1' is not left-aligned with its content "
                        "(x=%2, content-left=%3)")
                        .arg(title->text()).arg(origin.x()).arg(contentLeft));
                    ok = false;
                }
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
    // The reserved busy-indicator slot must sit below the guarded-repair
    // badge (modern parity), flush with the badge's right edge, inside the
    // window at the 1024x768 contract size, and its widest animated frame
    // must fit the window width. The geometry is verified with the widest
    // frame shown (Qt 3.3.7's QRect::contains() rejects a null rect, so an
    // empty idle label can never be checked directly).
    if (m_busyLabel && m_busyLabel->isVisibleTo(this)) {
        ++checkedCount;
        const QString savedText = m_busyLabel->text();
        m_busyLabel->setText(QString::fromLatin1("Running all diagnostics..."));
        qApp->processEvents();
        const QRect mapped(m_busyLabel->mapTo(this, QPoint(0, 0)),
                           m_busyLabel->size());
        if (!rect().contains(mapped)) {
            problems->append(QString::fromLatin1(
                "busy indicator is clipped by the window"));
            ok = false;
        }
        if (m_headerBadge && m_headerBadge->isVisibleTo(this)) {
            const QRect badgeRect(m_headerBadge->mapTo(this, QPoint(0, 0)),
                                  m_headerBadge->size());
            if (mapped.top() < badgeRect.bottom()) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not below the guarded-repair badge"));
                ok = false;
            }
            if (QABS(mapped.right() - badgeRect.right())
                > kBusyBadgeEdgeTolerance) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not right-aligned with the guarded-repair badge"));
                ok = false;
            }
        }
        QFontMetrics metrics(m_busyLabel->font());
        if (metrics.width(QString::fromLatin1("Running all diagnostics...")) + 12
            > width()) {
            problems->append(QString::fromLatin1(
                "busy indicator's widest frame cannot fit the window width"));
            ok = false;
        }
        m_busyLabel->setText(savedText);
        qApp->processEvents();
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

    // Cycle 13: the label carries the modern scope notice, never the probe
    // status; the gating stays internal (the run button tooltips carry the
    // exact reason).
    m_chrootReasonLabel->setText(host
        ? QString::fromLatin1(
              "Run a command on the running host as root (sudo is not needed). "
              "Commands are executed directly on the active system; output is "
              "kept in this window and in the application log.")
        : QString::fromLatin1(
              "Run a command inside the selected repair system as root (sudo "
              "is not needed). Commands are executed one at a time in a fresh "
              "chroot and cannot answer interactive prompts; use non-interactive "
              "flags such as apt-get -y upgrade. Output is kept in this window "
              "and in the application log."));

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
    updateFileCopyTab();
}

// The File Copy direction key the helper's copy/copy-preview verbs expect.
QString LegacyMainWindow::fileCopyDirection() const
{
    if (m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentItem() == 1) {
        return QString::fromLatin1("repair-to-host");
    }
    return QString::fromLatin1("host-to-repair");
}

// File Copy gating: the cached `Legacy feature file-copy:` probe, a ready
// scope, the administrator session and no running command must all agree.
bool LegacyMainWindow::fileCopyReady(QString *reason) const
{
    if (m_running) {
        if (reason) {
            *reason = QString::fromLatin1("A helper command is already running.");
        }
        return false;
    }
    std::string featureReason;
    if (!m_model.legacyFeatureAvailable("file-copy", toStd(identity()),
                                        &featureReason)) {
        if (reason) {
            *reason = fromStd(featureReason);
        }
        return false;
    }
    if (!diagnosticsScopeReady()) {
        if (reason) {
            *reason = scopeReadyReason();
        }
        return false;
    }
    if (!administratorSessionActive()) {
        if (reason) {
            *reason = QString::fromLatin1(
                "No administrator session is active; press Authorize or "
                "re-enter Host Maintenance / commit a repair target.");
        }
        return false;
    }
    return true;
}

void LegacyMainWindow::updateFileCopyTab()
{
    if (!m_fileCopySourceGroup) {
        return;
    }
    QString reason;
    const bool ready = fileCopyReady(&reason);
    const bool hasSources = !m_fileCopySources.isEmpty();
    const bool anySources = hasSources && ready;
    const bool repairToHost = m_fileCopyDirectionCombo
        && m_fileCopyDirectionCombo->currentItem() == 1;
    const bool targetCommitted = !m_committedDisk.isEmpty();

    // Direction-driven labels (modern updateFileCopyDirection). The heading
    // stays the static "File copy" title — the direction arrow lives only in
    // the Direction combo — so the page-title row can never crowd it out.
    // The group-1 title uses the modern wording and is wrap-capable (see
    // buildFileCopyTab), so a narrow pane can never clip it.
    if (m_fileCopySourceTitle) {
        m_fileCopySourceTitle->setText(repairToHost
            ? QString::fromLatin1("1. Select source files or folders from the repaired system")
            : QString::fromLatin1("1. Select source files or folders from this host"));
    }
    if (m_fileCopyDestinationTitle) {
        m_fileCopyDestinationTitle->setText(repairToHost
            ? QString::fromLatin1("2. Choose destination on this host")
            : QString::fromLatin1("2. Choose destination in repaired system"));
    }
    if (m_fileCopyAddFilesButton) {
        updateButtonText(m_fileCopyAddFilesButton, repairToHost
            ? QString::fromLatin1("Add File Path...")
            : QString::fromLatin1("Add Files..."));
        m_fileCopyAddFilesButton->setEnabled(ready);
    }
    if (m_fileCopyAddFolderButton) {
        updateButtonText(m_fileCopyAddFolderButton, repairToHost
            ? QString::fromLatin1("Add Folder Path...")
            : QString::fromLatin1("Add Folder..."));
        m_fileCopyAddFolderButton->setEnabled(ready);
    }
    if (m_fileCopyBrowseButton) {
        updateButtonText(m_fileCopyBrowseButton, repairToHost
            ? QString::fromLatin1("Browse...")
            : QString::fromLatin1("Browse Target Folders..."));
        // Host to Repair browsing reads the selected repair tree through the
        // helper and needs a committed repair target (Host Maintenance alone
        // provides no repair tree); Repair to Host browses the running host
        // directly and needs no target.
        const bool browseEnabled = ready && (repairToHost || targetCommitted);
        m_fileCopyBrowseButton->setEnabled(browseEnabled);
        const QString browseTip = !browseEnabled
            ? (!ready
                   ? reason
                   : QString::fromLatin1(
                         "Select the repair drive in Systems before choosing a "
                         "destination inside it (Host Maintenance does not "
                         "provide a repair tree to browse)."))
            : (repairToHost
                   ? QString::fromLatin1(
                         "Choose a host destination folder directly.")
                   : QString::fromLatin1(
                         "Browse the selected repair system through the helper's "
                         "temporary read-only mounts and choose an absolute "
                         "destination path. No target files are changed while "
                         "browsing."));
        if (m_fileCopyBrowseTip != browseTip) {
            m_fileCopyBrowseTip = browseTip;
            QToolTip::add(m_fileCopyBrowseButton, browseTip);
        }
    }

    if (m_fileCopyDirectionCombo) {
        m_fileCopyDirectionCombo->setEnabled(ready);
    }
    if (m_fileCopyOwnershipCombo) {
        m_fileCopyOwnershipCombo->setEnabled(ready);
    }
    if (m_fileCopyDestinationEdit) {
        m_fileCopyDestinationEdit->setEnabled(ready);
        if (ready && m_fileCopyDestinationEdit->text()
                        == QString::fromLatin1("unavailable: see the helper's Legacy feature file-copy: probe reason above")) {
            m_fileCopyDestinationEdit->setText(QString::null);
        }
    }
    if (m_fileCopySourceList) {
        m_fileCopySourceList->setEnabled(ready);
    }
    if (m_fileCopyRemoveButton) {
        m_fileCopyRemoveButton->setEnabled(anySources);
    }
    if (m_fileCopyClearButton) {
        m_fileCopyClearButton->setEnabled(anySources);
    }
    // The primary actions share the page-title row with the scope label and
    // light up as soon as the direction and scope are ready (modern parity);
    // the run path itself re-validates the staged sources and destination.
    if (m_fileCopyPreviewButton) {
        m_fileCopyPreviewButton->setEnabled(ready);
        const QString previewTip = ready
            ? QString::fromLatin1(
                  "Run a copy dry-run through the guarded helper. No files are changed.")
            : reason;
        if (m_fileCopyPreviewTip != previewTip) {
            m_fileCopyPreviewTip = previewTip;
            QToolTip::add(m_fileCopyPreviewButton, previewTip);
        }
    }
    if (m_fileCopyRunButton) {
        m_fileCopyRunButton->setEnabled(ready);
        const QString runTip = ready
            ? QString::fromLatin1(
                  "Copy the staged files and folders with cp -a, restore "
                  "ownership with chown --reference and byte-compare every "
                  "regular file afterwards.")
            : reason;
        if (m_fileCopyRunTip != runTip) {
            m_fileCopyRunTip = runTip;
            QToolTip::add(m_fileCopyRunButton, runTip);
        }
    }
}

void LegacyMainWindow::fileCopyDirectionChanged()
{
    // Direction changes clear the staged sources and destination to avoid
    // mixing source/destination namespaces (modern parity).
    m_fileCopySources.clear();
    if (m_fileCopySourceList) {
        m_fileCopySourceList->clear();
    }
    if (m_fileCopyDestinationEdit) {
        m_fileCopyDestinationEdit->clear();
    }
    const bool repairToHost = m_fileCopyDirectionCombo
        && m_fileCopyDirectionCombo->currentItem() == 1;
    appendLog(QString::fromLatin1(
        "File Copy direction changed to %1; staged paths were cleared to "
        "avoid mixing source/destination namespaces.")
        .arg(repairToHost
                 ? QString::fromUtf8("Repair \xE2\x86\x92 Host")
                 : QString::fromUtf8("Host \xE2\x86\x92 Repair")));
    updateFileCopyTab();
}

// Browse Target Folders... (modern browseFileCopyDestination): Repair -> Host
// picks a host folder through QFileDialog; Host -> Repair walks the selected
// repair tree one directory view at a time through the helper's read-only
// browse-target verb (BROWSE_ENTRY records, base64-encoded names).
void LegacyMainWindow::fileCopyBrowse()
{
    if (m_running) {
        return;
    }
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::information(this, QString::fromLatin1("File Copy unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool repairToHost = m_fileCopyDirectionCombo
        && m_fileCopyDirectionCombo->currentItem() == 1;
    if (repairToHost) {
        const QString path = QFileDialog::getExistingDirectory(
            QString::null, this, "file-copy-host-dest",
            QString::fromLatin1("Choose host destination folder"));
        if (path.isEmpty()) {
            return;
        }
        if (m_fileCopyDestinationEdit) {
            m_fileCopyDestinationEdit->setText(path);
        }
        appendLog(QString::fromLatin1(
            "Staged Repair \xE2\x86\x92 Host destination: %1. No copy occurred.")
            .arg(path));
        updateFileCopyTab();
        return;
    }
    if (m_committedDisk.isEmpty() || m_committedRoot.isEmpty()) {
        QMessageBox::information(
            this, QString::fromLatin1("Select a repair target"),
            QString::fromLatin1(
                "Select the repair drive in Systems before choosing a "
                "destination inside it."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_fileCopyBrowsePath = QString::fromLatin1("/");
    QStringList args;
    args << QString::fromLatin1("browse-target")
         << m_committedDisk << m_committedRoot
         << m_fileCopyBrowsePath;
    startCommand(args, false,
                 QString::fromLatin1("File Copy - Browse Target Folders"),
                 false, false, false, false, QString::null, QString::null,
                 true);
}

// Qt 3.3.7 has no QByteArray::fromBase64, so the legacy helper emits the
// browse names raw with only %, TAB, CR and LF percent-encoded
// (% -> %25, TAB -> %09, CR -> %0D, LF -> %0A); decode inline.  The chain
// reverses the helper's encode order so a name whose text literally contains
// "%0A" / "%0D" / "%09" / "%25" round-trips instead of double-decoding.
static QString legacyPercentDecode(const QString &encoded)
{
    QString out = encoded;
    out.replace(QString::fromLatin1("%0A"), QString::fromLatin1("\n"));
    out.replace(QString::fromLatin1("%0D"), QString::fromLatin1("\r"));
    out.replace(QString::fromLatin1("%09"), QString::fromLatin1("\t"));
    out.replace(QString::fromLatin1("%25"), QString::fromLatin1("%"));
    return out;
}

// The browse-target transcript drives the folder picker: immediate
// subdirectories arrive as BROWSE_ENTRY records (the legacy port emits the
// raw percent-encoded name). The user can step into a folder, move up, or
// choose the current folder as the destination.
void LegacyMainWindow::handleFileCopyBrowseResult(const std::string &transcript,
                                                  bool ok)
{
    if (!ok) {
        QMessageBox::warning(
            this, QString::fromLatin1("Browse Target Folders"),
            QString::fromLatin1(
                "The helper could not list the repair-system folder:\n\n%1")
                .arg(transcript.size() > 400
                         ? QString::fromLatin1(transcript.substr(0, 400).c_str())
                         : QString::fromLatin1(transcript.c_str())),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QStringList entries;
    const QStringList lines = QStringList::split(QString::fromLatin1("\n"),
                                                fromStd(transcript), false);
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        if (!(*it).startsWith(QString::fromLatin1("BROWSE_ENTRY\t"))) {
            continue;
        }
        entries.append(legacyPercentDecode((*it).mid(13)));
    }
    entries.sort();
    QStringList items;
    items.append(QString::fromLatin1("(choose this folder: %1)")
                    .arg(m_fileCopyBrowsePath));
    if (m_fileCopyBrowsePath != QString::fromLatin1("/")) {
        items.append(QString::fromLatin1("(parent folder)"));
    }
    for (QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it) {
        items.append(*it);
    }
    // Stack-allocated picker (the config-editor pattern): every child is
    // parented to the stack dialog, so the whole browse-picker lifecycle
    // stays inside this frame and nothing is heap-allocated without a parent.
    // (Qt3's static QInputDialog::getItem heap-allocates and deletes its
    // dialog around the modal exec; the stack dialog avoids that pattern.)
    QDialog picker(this, "legacy-file-copy-browse", true);
    picker.setCaption(QString::fromLatin1("Select repair-system destination"));
    QVBoxLayout *pickerLayout = new QVBoxLayout(&picker, 10, 8);
    QLabel *pickerHint = new QLabel(
        QString::fromLatin1(
            "Choose a destination folder inside the repaired system "
            "(current folder: %1):")
            .arg(m_fileCopyBrowsePath),
        &picker);
    enableLabelWordWrap(pickerHint);
    pickerHint->setMaximumWidth(520);
    pickerLayout->addWidget(pickerHint);
    QListView *pickerList = new QListView(&picker);
    pickerList->addColumn(QString::fromLatin1("Folder"));
    pickerList->setSorting(-1);
    pickerList->setAllColumnsShowFocus(true);
    for (QStringList::ConstIterator it = items.begin(); it != items.end(); ++it) {
        new QListViewItem(pickerList, *it);
    }
    pickerLayout->addWidget(pickerList, 1);
    QHBoxLayout *pickerButtons = new QHBoxLayout(pickerLayout);
    pickerButtons->setSpacing(6);
    pickerButtons->addStretch();
    QPushButton *pickerCancel = new QPushButton(QString::fromLatin1("Cancel"), &picker);
    QPushButton *pickerOpen = new QPushButton(QString::fromLatin1("Open"), &picker);
    pickerOpen->setDefault(true);
    pickerButtons->addWidget(pickerCancel);
    pickerButtons->addWidget(pickerOpen);
    QObject::connect(pickerCancel, SIGNAL(clicked()), &picker, SLOT(reject()));
    QObject::connect(pickerOpen, SIGNAL(clicked()), &picker, SLOT(accept()));
    QObject::connect(pickerList, SIGNAL(doubleClicked(QListViewItem *)),
                     &picker, SLOT(accept()));
    picker.resize(520, 380);
    if (picker.exec() != QDialog::Accepted) {
        return;
    }
    QListViewItem *pickerItem = pickerList->currentItem();
    if (!pickerItem) {
        return;
    }
    const QString picked = pickerItem->text(0);
    if (picked.startsWith(QString::fromLatin1("(choose this folder:"))) {
        if (m_fileCopyDestinationEdit) {
            m_fileCopyDestinationEdit->setText(m_fileCopyBrowsePath);
        }
        appendLog(QString::fromLatin1(
            "Staged Host \xE2\x86\x92 Repair destination: %1. No copy occurred.")
            .arg(m_fileCopyBrowsePath));
        updateFileCopyTab();
        return;
    }
    if (picked == QString::fromLatin1("(parent folder)")) {
        const int slash = m_fileCopyBrowsePath.findRev('/');
        m_fileCopyBrowsePath = slash > 0
            ? m_fileCopyBrowsePath.left(slash) : QString::fromLatin1("/");
    } else {
        m_fileCopyBrowsePath = m_fileCopyBrowsePath == QString::fromLatin1("/")
            ? QString::fromLatin1("/") + picked
            : m_fileCopyBrowsePath + QString::fromLatin1("/") + picked;
    }
    // Step into the chosen folder (or the parent) with one more read-only
    // helper browse request.
    QStringList args;
    args << QString::fromLatin1("browse-target")
         << m_committedDisk << m_committedRoot
         << m_fileCopyBrowsePath;
    startCommand(args, false,
                 QString::fromLatin1("File Copy - Browse Target Folders"),
                 false, false, false, false, QString::null, QString::null,
                 true);
}

void LegacyMainWindow::fileCopyAddFiles()
{
    const QStringList files = QFileDialog::getOpenFileNames(
        QString::null, QString::null, this, "add-files",
        QString::fromLatin1("Add files to copy"));
    if (files.isEmpty()) {
        return;
    }
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
        if (!m_fileCopySources.contains(*it)) {
            m_fileCopySources.append(*it);
            new QListViewItem(m_fileCopySourceList, *it);
        }
    }
    updateFileCopyTab();
}

void LegacyMainWindow::fileCopyAddFolder()
{
    const QString folder = QFileDialog::getExistingDirectory(
        QString::null, this, "add-folder",
        QString::fromLatin1("Add folder to copy"));
    if (folder.isEmpty() || m_fileCopySources.contains(folder)) {
        return;
    }
    m_fileCopySources.append(folder);
    new QListViewItem(m_fileCopySourceList, folder);
    updateFileCopyTab();
}

void LegacyMainWindow::fileCopyRemoveSelected()
{
    QListViewItem *item = m_fileCopySourceList
        ? m_fileCopySourceList->currentItem() : 0;
    if (!item) {
        return;
    }
    m_fileCopySources.remove(item->text(0));
    delete item;
    updateFileCopyTab();
}

void LegacyMainWindow::fileCopyClearStaging()
{
    m_fileCopySources.clear();
    if (m_fileCopySourceList) {
        m_fileCopySourceList->clear();
    }
    updateFileCopyTab();
}

// Starts copy-preview or copy through the legacy helper (the helper keeps the
// direction/path containment checks, the sensitive-destination refusal and
// the per-file cmp verification).
bool LegacyMainWindow::startFileCopyCommand(bool realCopy)
{
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::information(this, QString::fromLatin1("File Copy unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    const QString destination = m_fileCopyDestinationEdit
        ? m_fileCopyDestinationEdit->text().stripWhiteSpace() : QString::null;
    if (m_fileCopySources.isEmpty() || destination.isEmpty()) {
        QMessageBox::information(
            this, QString::fromLatin1("File Copy"),
            QString::fromLatin1(
                "Stage at least one source and name a destination path first."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    if (realCopy) {
        const bool answer = confirmWrapped(
            this, QString::fromLatin1("Copy and Verify"),
            QString::fromLatin1(
                "Copy the %1 staged item(s) to %2?\n\nThe helper keeps its "
                "direction and path containment checks; a sensitive repair-system "
                "destination is refused unless the helper approves it, and every "
                "regular file is byte-compared after the copy.")
                .arg(static_cast<int>(m_fileCopySources.size()))
                .arg(destination));
        if (!answer) {
            return false;
        }
    }
    const QString ownership = m_fileCopyOwnershipCombo
        && m_fileCopyOwnershipCombo->currentItem() == 1
            ? QString::fromLatin1("preserve")
            : QString::fromLatin1("smart");
    QStringList args;
    args << (realCopy ? QString::fromLatin1("copy")
                      : QString::fromLatin1("copy-preview"))
         << selectedDisk() << selectedRoot()
         << fileCopyDirection()
         << ownership
         << QString::fromLatin1("normal")
         << destination;
    for (QStringList::ConstIterator it = m_fileCopySources.begin();
         it != m_fileCopySources.end(); ++it) {
        args << *it;
    }
    const QString label = realCopy ? QString::fromLatin1("File Copy - Copy and Verify")
                                   : QString::fromLatin1("File Copy - Preview Changes");
    const bool started = startCommand(args, false, label, false, false, false,
                                      false, QString::null,
                                      QString::fromLatin1("file-copy"));
    if (started && !m_smokeMode) {
        showRepairResultDialog(label);
    }
    return started;
}

void LegacyMainWindow::fileCopyPreview()
{
    startFileCopyCommand(false);
}

void LegacyMainWindow::fileCopyRun()
{
    startFileCopyCommand(true);
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
    updateHostCard();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::updateDriveDetails()
{
    if (!m_detailList) {
        return;
    }
    m_detailList->clear();
    const QString disk = m_inspectingHostDetails
        ? runningHostDisk() : selectedDisk();
    if (m_detailsPaneTitle) {
        m_detailsPaneTitle->setText(m_inspectingHostDetails
            ? QString::fromLatin1("Protected running host - details")
            : QString::fromLatin1("Selected drive details"));
    }
    // Qt3 prepends a plain insertion when sorting is disabled; chain the rows
    // with the after-form constructor to keep the documented field order
    // (Drive, Detected target, Model/label, ...).
    const std::vector<std::pair<QString, QString> > rows = driveDetailsRows(disk);
    QListViewItem *lastDetail = 0;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        lastDetail = new QListViewItem(m_detailList, lastDetail,
                                       rows[i].first, rows[i].second);
    }
}

// The Field/Value details rows for a drive (the same rows the Systems details
// pane and the protected-host Details dialog show). Shared by both so the two
// views cannot drift apart.
std::vector<std::pair<QString, QString> > LegacyMainWindow::driveDetailsRows(
    const QString &disk) const
{
    std::vector<std::pair<QString, QString> > rows;
    QMap<QString, DeviceRow>::const_iterator row = m_rows.find(disk);
    const DeviceRow *selected = row == m_rows.end() ? 0 : &row.data();
    const bool helperConfirmed = !disk.isEmpty() && m_helperDisk == disk;

    // Modern parity: the inspected row (a child component when one is
    // selected, otherwise the drive) supplies the details; the Drive field
    // always names the top-level drive. For the protected-host pane the
    // resolved root component supplies UUID and Mounts.
    QString inspectedPath = selectedComponent();
    if (inspectedPath.isEmpty() && m_inspectingHostDetails) {
        inspectedPath = autoResolvedRoot(disk);
    }
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
        // Model/label: the inspected row first, with the owning disk's
        // model/label as the fallback for mapper/partition children (the
        // inventory keeps the model only on disk rows).
        QString childModel = fromStd(inspected->model);
        QString childLabel = fromStd(inspected->label);
        if ((childModel.isEmpty() || childLabel.isEmpty()) && selected) {
            if (childModel.isEmpty()) {
                childModel = fromStd(selected->model);
            }
            if (childLabel.isEmpty()) {
                childLabel = fromStd(selected->label);
            }
        }
        if (childModel.isEmpty()) {
            model = childLabel;
        } else if (childLabel.isEmpty()) {
            model = childModel;
        } else {
            model = childModel + QString::fromLatin1(" (")
                + childLabel + QString::fromLatin1(")");
        }
        size = fromStd(inspected->size);
        transport = fromStd(inspected->transport);
        fstype = inspected->disk ? diskFilesystemSummary(*inspected)
                                 : displayFsType(*inspected);
        uuid = fromStd(inspected->uuid);
        // Cycle 15 disk-level UUID roll-up for the drive row itself:
        // 1) the unlocked root's UUID (UNLOCKED_ROOT_UUID), 2) the locked
        // LUKS container's by-uuid identity, 3) the resolved Linux root
        // component's UUID. A boot partition's UUID is never used at the
        // drive level.
        if (uuid.isEmpty() && inspected->disk) {
            if (!m_unlockedRootUuid.isEmpty() && disk == m_unlockedDisk) {
                uuid = m_unlockedRootUuid;
            } else {
                const QString luks = autoResolvedLuks(disk);
                if (!luks.isEmpty()) {
                    const QMap<QString, DeviceRow>::const_iterator luksRow =
                        m_rows.find(luks);
                    if (luksRow != m_rows.end()) {
                        uuid = fromStd(luksRow.data().uuid);
                    }
                }
                if (uuid.isEmpty()) {
                    const QString rootComponent = autoResolvedRoot(disk);
                    const QMap<QString, DeviceRow>::const_iterator rootRow =
                        m_rows.find(rootComponent);
                    if (rootRow != m_rows.end()) {
                        uuid = fromStd(rootRow.data().uuid);
                    }
                }
            }
        }
        // Helper-confirmed root UUID fallback: the helper names the root
        // component and the inventory carries its by-uuid identity.
        if (uuid.isEmpty() && helperConfirmed && !m_helperComponent.isEmpty()) {
            const QMap<QString, DeviceRow>::const_iterator helperRow =
                m_rows.find(m_helperComponent);
            if (helperRow != m_rows.end()) {
                uuid = fromStd(helperRow.data().uuid);
            }
        }
        // Mounts: the joined distinct targets (primary first); the single
        // mountpoint covers rows without a joined list. The freshly unlocked
        // root is never auto-mounted on selection (mounting happens during
        // diagnostics read-only and repairs read-write).
        mounts = fromStd(inspected->mountpoints);
        if (mounts.isEmpty()) {
            mounts = fromStd(inspected->mountpoint);
        }
        if (mounts.isEmpty() && inspectedPath == m_unlockedRoot
            && !m_unlockedRoot.isEmpty()) {
            mounts = QString::fromLatin1("not mounted (offline target)");
        }
    }
    if ((fstype.isEmpty() || fstype == QString::fromLatin1("unknown"))
        && helperConfirmed) {
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

    rows.push_back(std::make_pair(QString::fromLatin1("Drive:"),
                                  detailOrDash(disk)));
    rows.push_back(std::make_pair(QString::fromLatin1("Detected target:"),
                                  component.isEmpty()
                                      ? QString::fromLatin1("Pending inspection")
                                      : component));
    rows.push_back(std::make_pair(QString::fromLatin1("Model / label:"),
                                  detailOrDash(model)));
    rows.push_back(std::make_pair(QString::fromLatin1("Status:"), status));
    rows.push_back(std::make_pair(QString::fromLatin1("Size:"),
                                  detailOrDash(size)));
    rows.push_back(std::make_pair(QString::fromLatin1("Connection:"),
                                  detailOrDash(transport)));
    rows.push_back(std::make_pair(QString::fromLatin1("Filesystem:"),
                                  detailOrDash(fstype)));
    rows.push_back(std::make_pair(QString::fromLatin1("UUID:"),
                                  detailOrDash(uuid)));
    rows.push_back(std::make_pair(QString::fromLatin1("Mounts:"),
                                  detailOrDash(mounts)));
    rows.push_back(std::make_pair(QString::fromLatin1("Protection:"),
                                  protection));
    return rows;
}

// Cycle 9 protected-host card: the probe-based identity line (helper OS fact)
// and the legacy-available storage line (model/label, device path, size,
// transport, critical mounts from the read-only inventory). Every value stays
// honest when unknown.
void LegacyMainWindow::updateHostCard()
{
    if (!m_hostSystemLabel || !m_hostStorageLabel) {
        return;
    }
    const QString disk = runningHostDisk();
    if (disk.isEmpty()) {
        m_hostSystemLabel->setText(QString::fromLatin1(
            "Running system protection unresolved"));
        m_hostStorageLabel->setText(QString::fromLatin1(
            "No protected physical backing disk was identified"));
        return;
    }
    QString os = mapValue(m_factMap, QString::fromLatin1("OS:"));
    if (os.isEmpty()) {
        os = mapValue(m_factMap, QString::fromLatin1("Distribution:"));
    }
    if (os.isEmpty()) {
        os = QString::fromLatin1("Current running Linux system");
    }
    m_hostSystemLabel->setText(os);

    QStringList parts;
    const QMap<QString, DeviceRow>::const_iterator it = m_rows.find(disk);
    QString mounts;
    if (it != m_rows.end()) {
        const DeviceRow &hostRow = it.data();
        QString model = fromStd(hostRow.model);
        if (model.isEmpty()) {
            model = fromStd(hostRow.label);
        }
        if (!model.isEmpty()) {
            parts.append(model);
        }
        parts.append(disk);
        if (!fromStd(hostRow.size).isEmpty()) {
            parts.append(fromStd(hostRow.size));
        }
        if (!fromStd(hostRow.transport).isEmpty()) {
            parts.append(fromStd(hostRow.transport).upper());
        }
        mounts = fromStd(hostRow.mountpoints);
        if (mounts.isEmpty()) {
            mounts = fromStd(hostRow.mountpoint);
        }
    }
    QString summary = parts.join(QString::fromLatin1("  |  "));
    if (!mounts.isEmpty()) {
        if (!summary.isEmpty()) {
            summary += QString::fromLatin1("  |  ");
        }
        summary += QString::fromLatin1("Critical mounts: %1").arg(mounts);
    }
    m_hostStorageLabel->setText(summary);
    QToolTip::add(m_hostStorageLabel, QString::fromLatin1(
        "Read-only protected-running-system facts: the helper OS fact plus "
        "the inventory model, device path, size, transport and critical "
        "mounts. Nothing here is probed destructively."));
}

// Cycle 9 Details button: the running-host facts in a read-only dialog,
// reusing exactly the rows the Systems details pane shows.
// Cycle 13: the Details button (kept on the host card, since the running
// host is never selectable as a drive row) populates the right Selected
// drive details pane with the running-host facts — the same rows the
// popup used. No dialog: selecting any drive row restores the normal
// per-drive pane.
void LegacyMainWindow::showHostDetails()
{
    m_inspectingHostDetails = true;
    updateDriveDetails();
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
            "Mapper: (none)\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
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
            "Mapper: (none)\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
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
            "Mapper: (none)\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
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

// Cycle 12: after a successful unlock, the freshly activated LVs are added to
// the cached inventory as mapper rows owned by the selected drive (the
// read-only inventory scan cannot see the dm nodes' chain, so the tree would
// otherwise stay empty below the drive). The fstype comes from the helper's
// UNLOCKED_ROOT_FSTYPE probe line, never from a block read here.
void LegacyMainWindow::injectUnlockedMapperRows(const QString &disk,
                                                const QString &mapper,
                                                const QString &root,
                                                const QString &fstype,
                                                const QString &uuid)
{
    if (disk.isEmpty()) {
        return;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    const QStringList paths = (QStringList() << mapper << root);
    bool added = false;
    for (QStringList::ConstIterator it = paths.begin(); it != paths.end(); ++it) {
        const QString path = *it;
        if (path.isEmpty() || !path.startsWith(QString::fromLatin1("/dev/"))
            || m_rows.contains(path)) {
            continue;
        }
        DeviceRow row;
        row.name = toStd(path);
        row.path = toStd(path);
        row.mapper = true;
        // The tree's owning-disk resolution walks the parent chain by name,
        // so the row is parented to the selected drive for display.
        row.parent = toStd(diskName);
        if (path == root) {
            if (!fstype.isEmpty()) {
                row.probedFstype = toStd(fstype);
            } else {
                row.probedFstype = "unknown";
            }
            if (!uuid.isEmpty()) {
                row.uuid = toStd(uuid);
            }
        } else {
            row.probedFstype = "unknown";
        }
        m_rows.insert(path, row);
        m_inventory.push_back(row);
        added = true;
    }
    if (added) {
        rebuildDeviceList();
    }
}

void LegacyMainWindow::mergeHelperDevices(const ParsedTranscript &parsed)
{
    bool added = false;
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
        // device-discovery refilter cannot drop it from the details path.
        m_inventory.push_back(row);
        added = true;
    }
    // The tree builder is the single source of truth: helper-reported rows
    // enter the tree only as children of a top-level disk (or mappers whose
    // owning disk resolves); orphan pass-through rows stay out of the tree
    // while their inspection data remains in the inventory.
    if (added) {
        rebuildDeviceList();
    }
}

void LegacyMainWindow::updateElevationLabel()
{
    QString description;
    const bool ok = m_runner->resolveElevation(&description);
    m_lastHelperDescription = description;
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
    // Cycle 9: the scope label renders like modern's wrapped scope label —
    // "Host maintenance:" / "Target:" on the first line and the disk (or
    // disk + root) on the second.
    QString text;
    if (hostMaintenanceActive()) {
        text = QString::fromLatin1("Host maintenance:\n%1")
                   .arg(selectedDisk().isEmpty()
                            ? QString::fromLatin1("unresolved")
                            : selectedDisk());
    } else if (targetCommitted()) {
        text = QString::fromLatin1("Target:\n%1 + %2")
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
    if (m_fileCopyScopeLabel) {
        m_fileCopyScopeLabel->setText(text);
    }
}

// Deferred-authorization affordance (modern m_authorizationStatusLabel /
// Authorize): the status text and the button state on the Systems and Repair
// pages. The modal hidden-input dialog is opened by authorizeNow().
void LegacyMainWindow::updateAuthorizationAffordance()
{
    // Cycle 11 (modern parity): the Authorize button and status label are
    // only visible in the deferred-authorization state — a scope is ready
    // AND the administrator session is not active. With no scope, or once
    // the session is active, both are hidden (the modern frontend shows no
    // idle authorization row either).
    const bool ready = diagnosticsScopeReady();
    const bool active = administratorSessionActive();
    const bool deferred = ready && !active;

    QString text;
    bool canAuthorize = false;
    if (deferred) {
        text = QString::fromLatin1(
            "Authorization required: diagnostics and repairs fail closed until "
            "you press Authorize.");
        canAuthorize = true;
    }
    if (m_authStatusLabel) {
        m_authStatusLabel->setText(text);
        m_authStatusLabel->setHidden(!deferred);
    }
    if (m_authorizeButton) {
        m_authorizeButton->setHidden(!deferred);
        m_authorizeButton->setEnabled(canAuthorize);
        if (canAuthorize) {
            QToolTip::add(m_authorizeButton, QString::fromLatin1(
                "Re-establish the privileged helper session for the current "
                "scope now. The password is requested in the hidden-input "
                "modal and is never logged."));
        }
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
    // Cycle 9 Run/Re-run parity: once cached results exist for the current
    // scope+key (after an individual run or Run All), the button reads
    // "Re-run Diagnostic"; otherwise "Run Diagnostic".
    if (m_runDiagnosticButton) {
        const bool reRun = !m_lastDiagnosticKey.isEmpty()
            && key == m_lastDiagnosticKey
            && m_model.hasDiagnostics(toStd(identity()))
            && !m_model.diagnosticsStale();
        m_runDiagnosticButton->setText(reRun
            ? QString::fromLatin1("Re-run Diagnostic")
            : QString::fromLatin1("Run Diagnostic"));
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
    // updatePlanView() recompute their buttons and reasons, and the Settings
    // plan checkboxes re-apply the modern availability semantics.
    updateToolDetails();
    updatePlanView();
    updatePlanChecks();

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
        // Cycle 11 Already-Unlocked state (modern parity): after this
        // session's successful unlock of the selected drive the button reads
        // "Already Unlocked" and is disabled; it returns to the normal
        // gating on another selection or a rescan.
        const bool alreadyUnlocked = !selectedDisk().isEmpty()
            && m_unlockedDisk == selectedDisk()
            && !m_unlockedRoot.isEmpty();
        if (alreadyUnlocked && idle) {
            updateButtonText(m_unlockButton,
                             QString::fromLatin1("Already Unlocked"));
            m_unlockButton->setEnabled(false);
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "An unlocked Linux filesystem is already visible on this "
                "drive. Boot Bitch will reuse the existing mapper and will "
                "not close or reopen a mapping created by this recovery "
                "session. Mounting happens during diagnostics (read-only) "
                "and repairs (read-write); data filesystems are never "
                "auto-mounted on selection."));
        } else {
            updateButtonText(m_unlockButton, QString::fromLatin1("Unlock"));
            const QString luks = onRunningHost ? QString::null
                                               : unlockCandidateFor(selectedDisk());
            // Cycle 13: Host Maintenance is a scope, not a lock on other
            // drives — a selected offline drive stays unlockable while it is
            // active.
            const bool unlockEnabled = idle && !onRunningHost
                && !selectedDisk().isEmpty() && !luks.isEmpty();
            m_unlockButton->setEnabled(unlockEnabled);
            if (onRunningHost) {
                QToolTip::add(m_unlockButton, QString::fromLatin1(
                    "The protected running host cannot be unlocked; use Host "
                    "Maintenance for the protected running host."));
            } else if (hostScope()) {
                QToolTip::add(m_unlockButton, QString::fromLatin1(
                    "Host Maintenance is the current scope, but the selected "
                    "offline drive can still be unlocked."));
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
                    "passphrase travels through a private keyfile and is never "
                    "placed in command arguments or logs.").arg(luks));
            }
        }
    }
    if (m_setTargetButton) {
        const bool onRunningHost = !runningHostDisk().isEmpty()
            && selectedDisk() == runningHostDisk();
        QMap<QString, DeviceRow>::const_iterator selectedRow =
            m_rows.find(selectedDisk());
        const bool optical = selectedRow != m_rows.end()
            && selectedRow.data().optical;
        const QString root = autoResolvedRoot(selectedDisk());
        // Cycle 15 (modern diskIsSelectableRepairTarget parity): Select
        // Target is enabled for every selectable non-host, non-optical drive
        // — blank and non-Linux data disks included. The only drive-level
        // refusal besides the protected host and optical media is a locked
        // LUKS container without a visible Linux filesystem. The committed
        // drive stays enabled (modern never disables it) with the committed
        // tooltip instead.
        const bool lockedLuksNoLinux = !autoResolvedLuks(selectedDisk()).isEmpty()
            && root.isEmpty();
        const bool alreadyCommitted = !m_committedDisk.isEmpty()
            && selectedDisk() == m_committedDisk;
        const bool canCommit = idle && !selectedDisk().isEmpty() && !onRunningHost
            && !optical && !lockedLuksNoLinux;
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
        } else if (lockedLuksNoLinux) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Unlock the encrypted volume first; Select Target becomes "
                "available after a Linux filesystem is detected."));
        } else if (alreadyCommitted) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Committed repair target. Repair, Diagnostics and File Copy "
                "target this physical drive until another drive is explicitly "
                "selected with Select Target."));
        } else if (!idle) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "A helper command is already running."));
        } else if (hostScope()) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Commit %1 as the repair target; this leaves Host Maintenance "
                "and switches the scope to the selected drive.")
                .arg(selectedDisk()));
        } else {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Commit this physical drive as the repair target."));
        }
    }
    if (m_hostDefaultButton) {
        const bool hostReady = idle && hostScope() && !runningHostDisk().isEmpty();
        std::string defaultReason;
        const bool defaultAvailable = m_model.legacyFeatureAvailable(
            "host-default", toStd(identity()), &defaultReason);
        const bool canDefault = hostReady && defaultAvailable
            && administratorSessionActive();
        m_hostDefaultButton->setEnabled(canDefault);
        if (!canDefault && hostReady) {
            QToolTip::add(m_hostDefaultButton, fromStd(defaultReason));
        }
    }
    if (m_hostDetailsButton) {
        const bool hostKnown = !runningHostDisk().isEmpty();
        m_hostDetailsButton->setEnabled(hostKnown);
        QToolTip::add(m_hostDetailsButton, hostKnown
            ? QString::fromLatin1(
                  "Show read-only details for the protected running host.")
            : QString::fromLatin1(
                  "The running host target could not be detected."));
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

void LegacyMainWindow::updateBusyIndicator()
{
    if (!m_busyLabel) {
        return;
    }
    // Modern parity: the reserved row below the guarded-repair badge shows
    // the same working text the modern Qt6 GUI's global busy label uses for
    // the equivalent operation. While a helper command runs a plain QTimer
    // cycles 0..3 trailing dots behind the base text; the row keeps its
    // fixed height either way, so showing/clearing never moves the header or
    // the tab pages.
    if (!m_running) {
        if (m_busyTimer) {
            m_busyTimer->stop();
        }
        m_busyFrame = 0;
        m_busyBaseText = QString::null;
        m_busyLabel->setText(QString::null);
        return;
    }
    m_busyBaseText = busyIndicatorText();
    m_busyFrame = 0;
    m_busyLabel->setText(m_busyBaseText);
    if (m_busyTimer) {
        m_busyTimer->start(kBusyAnimationIntervalMs);
    }
}

void LegacyMainWindow::advanceBusyAnimation()
{
    if (!m_busyLabel) {
        return;
    }
    // The lightweight ellipsis cycle: the base working text plus (frame % 4)
    // dots. Plain ASCII dots only — Etch's fonts garble non-ASCII glyphs and
    // Qt 3.3.7 has no animated-widget busy rendering.
    QString text = m_busyBaseText;
    for (int i = 0; i < (m_busyFrame % 4); ++i) {
        text += QString::fromLatin1(".");
    }
    m_busyLabel->setText(text);
    ++m_busyFrame;
}

QString LegacyMainWindow::busyIndicatorText() const
{
    // The exact strings the modern Qt6 GUI's busy status label shows for the
    // equivalent operation, so the two frontends read identically while they
    // work.
    if (m_pendingDiagnostic) {
        if (m_pendingQuiet) {
            return QString::fromLatin1("Regenerating diagnostics automatically");
        }
        if (!m_pendingDiagnosticKey.isEmpty()) {
            return QString::fromLatin1("Running diagnostic: %1")
                .arg(diagnosticTitle(m_pendingDiagnosticKey));
        }
        return QString::fromLatin1("Running all diagnostics");
    }
    if (m_pendingUnlock) {
        return QString::fromLatin1("Unlocking %1").arg(m_unlockDevice);
    }
    // Repair tools, Full Repair, Make Default and File Copy already carry the
    // modern tool/operation titles as their pending label.
    return m_pendingLabel;
}

void LegacyMainWindow::updateStatus()
{
    // Cycle 15: the scope/readiness message is transient (modern parity) and
    // clears after a few seconds; nothing permanent stays in the bar.
    const QString scope = hostScope()
        ? QString::fromLatin1("running host")
        : QString::fromLatin1("offline target");
    const QString readiness = hostMaintenanceActive()
        ? QString::fromLatin1("host maintenance active")
        : (targetCommitted()
            ? QString::fromLatin1("repair target committed")
            : QString::fromLatin1("no committed scope"));
    statusBar()->message(QString::fromLatin1("%1 | %2").arg(scope).arg(readiness),
                         6000);
}

void LegacyMainWindow::appendLog(const QString &line)
{
    const QString text = line.isNull() ? QString::fromLatin1("") : line;
    // Tagged at capture time so the Logs filter combo can select entries by
    // kind, section key and repair stage.
    LogEntry entry;
    entry.text = text;
    entry.kind = m_logEntryKind;
    entry.diagnostic = m_logEntryDiagnostic;
    entry.stages = m_logEntryStages;
    m_logEntries.push_back(entry);
    ensureLogFile();
    if (!m_logPath.isEmpty()) {
        appendToLogFile(text);
    }
    if (!m_viewingPriorLog && logLinePassesFilter(entry)) {
        m_logView->append(text);
    }
}

// Ensures the session-log directory. The GUI's own default tree
// (~/.boot-repair-legacy/logs) is private: components are created 0700 and
// the leaf is re-tightened to 0700 (a pre-existing directory from an earlier
// run with looser permissions, or one weakened by umask, is fixed here). A
// user-supplied --log-dir is never chmod'ed: it may be a shared path and
// keeps the permissions its owner chose.
void LegacyMainWindow::ensureLogDirectory()
{
    if (m_logDirectory.isEmpty()) {
        m_logDirectory = QDir::homeDirPath()
            + QString::fromLatin1("/.boot-repair-legacy/logs");
    }
    if (isDefaultLogTree(m_logDirectory)) {
        ensureDirectory(m_logDirectory, 0700);
        ::chmod(m_logDirectory.local8Bit().data(), 0700);
    } else {
        ensureDirectory(m_logDirectory, 0755);
    }
}

void LegacyMainWindow::ensureLogFile()
{
    if (!m_logPath.isEmpty()) {
        return;
    }
    ensureLogDirectory();
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
    // Append-only open with O_NOFOLLOW + mode 0600: the session log is the
    // GUI's own file, a replaced symlink must never redirect the write, and
    // fchmod re-tightens the file to 0600 regardless of the process umask
    // (no symlink following, no umask dependence). Written through the
    // fdopen'ed FILE stream in local8Bit, the same encoding the previous
    // QTextStream path produced.
    const QByteArray pathBytes = m_logPath.local8Bit();
    const int fd = ::open(pathBytes.data(),
                          O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW, 0600);
    if (fd < 0) {
        return;
    }
    ::fchmod(fd, 0600);
    FILE *file = ::fdopen(fd, "a");
    if (!file) {
        ::close(fd);
        return;
    }
    const QByteArray lineBytes = line.local8Bit();
    if (lineBytes.size() > 0) {
        std::fwrite(lineBytes.data(), 1,
                    static_cast<std::size_t>(lineBytes.size()), file);
    }
    std::fputc('\n', file);
    std::fclose(file);
}

// The combo index -> filter semantics live in logFilterSpecs plus the 16
// diagnostic sections appended after them; the smoke addresses the rows by
// their known order (indexes 0-5 and 6..21 for the sections).
bool LegacyMainWindow::logLinePassesFilter(const LogEntry &entry) const
{
    if (m_logFilterCombo && m_logFilterCombo->currentItem() > 0) {
        const int index = m_logFilterCombo->currentItem();
        if (index == 5) {
            // The File copy workflow has no lines in this frontend (the
            // feature is greyed); the combo entry stays for the 1:1 order.
            return false;
        }
        if (index < logFilterSpecCount) {
            const LogFilterSpec &spec = logFilterSpecs[index];
            if (spec.kind != LogEntry::Application && entry.kind != spec.kind) {
                return false;
            }
            if (spec.kind == LogEntry::Repair && spec.stage[0] != '\0') {
                if (QString::fromLatin1(spec.stage) == QString::fromLatin1("package")) {
                    if (!stagesContainPackageStage(entry.stages)) {
                        return false;
                    }
                } else if (entry.stages.find(QString::fromLatin1(spec.stage)) < 0) {
                    return false;
                }
            }
        } else {
            // Diagnostic section filters: only Diagnostic entries carrying
            // that section's key.
            const int diagIndex = index - logFilterSpecCount;
            if (diagIndex >= diagnosticSpecCount) {
                return false;
            }
            if (entry.kind != LogEntry::Diagnostic
                || entry.diagnostic != QString::fromLatin1(diagnosticSpecs[diagIndex].key)) {
                return false;
            }
        }
    }
    if (m_logSearchEdit) {
        const QString search = m_logSearchEdit->text();
        if (!search.isEmpty() && entry.text.find(search, 0, false) < 0) {
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
    m_logView->setText(QString::null);
    if (m_viewingPriorLog) {
        // Prior session files are plain text: they carry no kind tags and
        // therefore appear under All entries (plus the search filter) only.
        for (QStringList::ConstIterator it = m_priorLogLines.begin();
             it != m_priorLogLines.end(); ++it) {
            LogEntry entry;
            entry.text = *it;
            if (logLinePassesFilter(entry)) {
                m_logView->append(*it);
            }
        }
        return;
    }
    for (std::size_t i = 0; i < m_logEntries.size(); ++i) {
        if (logLinePassesFilter(m_logEntries[i])) {
            m_logView->append(m_logEntries[i].text);
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
    // The Save As... dialog starts in /host when a writable system share
    // mount exists there (the etch-share vfat disk), so saved logs land on
    // the share consistently; otherwise the log directory is the fallback.
    // The host fetches the files offline: save to /host, shut the rig down
    // (both Etch domains - the peer rule), then mirror the share host-side;
    // the file appears under the host's shared dir. The dialog itself keeps
    // the ability to choose any directory the user can write.
    QString startDir = m_logDirectory;
    const QFileInfo hostShare(QString::fromLatin1("/host"));
    if (hostShare.exists() && hostShare.isDir() && hostShare.isWritable()) {
        startDir = QString::fromLatin1("/host");
    }
    const QString path = QFileDialog::getSaveFileName(
        startDir, QString::fromLatin1("Log files (*.log);;All files (*)"),
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
    QString text;
    for (std::size_t i = 0; i < m_logEntries.size(); ++i) {
        if (i > 0) {
            text += QString::fromLatin1("\n");
        }
        text += m_logEntries[i].text;
    }
    stream << text;
    if (!text.isEmpty()) {
        stream << "\n";
    }
    file.close();
}

void LegacyMainWindow::clearLog()
{
    // Clear Register: the live register and view only; prior session files are
    // never modified.
    m_logEntries.clear();
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
    // Modern Help -> About Boot Bitch (MainWindow::showAboutDialog), adapted
    // truthfully for the legacy frontend. The heading and the developer line
    // mirror the modern dialog; the body states exactly what this Qt 3.3.x
    // frontend and the ported guarded helper provide on Debian Etch-era
    // systems, and which modern-only features stay greyed.
    QMessageBox::about(
        this, QString::fromLatin1("About Boot Bitch"),
        QString::fromLatin1(
            "<h3>Boot Bitch %1</h3>"
            "<p><b>Developer:</b> CaptainMorgan12</p>"
            "<p>This GUI is the package's entry point: a Qt 3.3.x frontend with "
            "the ported guarded helper for Debian Etch-era systems.</p>"
            "<p><b>Guarded repair mode:</b> read-only diagnostics can inspect "
            "either the protected Running Host or an explicitly selected repair "
            "drive. The guarded Debian/APT package stages (interrupted "
            "configuration, broken dependencies, metadata refresh, upgrade), "
            "GRUB-legacy configuration regeneration, LUKS target unlock and "
            "guarded target-file editing run through the ported helper after "
            "confirmation; Host Maintenance enables the same supported stages "
            "natively on the active system after repeating the host identity "
            "and boot-mount checks.</p>"
            "<p>The modern-only features - verified File Copy, Btrfs snapshot "
            "rollback, EFI/UKI and extlinux repair, boot-stack reconciliation "
            "and Make Default - are greyed with the helper's own probe reasons "
            "on this frontend; Arch/Alpine/Fedora package backends stay "
            "diagnostics-only here.</p>"
            "<p>The first privileged action authorizes one cached administrator "
            "session per scope through a hidden-input modal (the Qt GUI remains "
            "unprivileged). It can be ended at any time from File - Lock "
            "Administrator Session.</p>").arg(QString::fromLatin1(LEGACY_VERSION)));
}

} // namespace legacy
