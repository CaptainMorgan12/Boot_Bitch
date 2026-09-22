// LegacyMainWindow implementation. Qt3. See LegacyMainWindow.h.

#include "LegacyMainWindow.h"
#include "DeviceInventory.h"
#include "HelperRunner.h"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qglobal.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtooltip.h>

#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>

#include <string>

#ifndef LEGACY_VERSION
#define LEGACY_VERSION "unknown"
#endif

namespace legacy {

namespace {

struct ActionSpec {
    const char *stage;
    const char *capability;
    const char *label;
    const char *hostLabel;
    bool write;
    const char *confirm;
};

// The legacy-supported command set only (task contract): validate, diagnose,
// fs-inspect, fix-broken, dpkg-configure, apt-update, apt-upgrade, initramfs.
// Every stage maps to the capability key of MainWindow::repairToolKeyForStage.
const ActionSpec actionSpecs[] = {
    { "validate", "validate",
      "Validate selection (read-only)", "Validate running host (read-only)",
      false, 0 },
    { "fs-inspect", "filesystem",
      "Inspect filesystems (read-only)", "Inspect running-host filesystems (read-only)",
      false, 0 },
    { "fix-broken", "fixbroken",
      "Repair broken packages (fix-broken)", "Repair broken packages on the running host (fix-broken)",
      true, "Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards." },
    { "dpkg-configure", "dpkg",
      "Finish dpkg configuration (dpkg-configure)", "Finish dpkg configuration on the running host (dpkg-configure)",
      true, "Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights." },
    { "apt-update", "aptupdate",
      "Refresh package metadata (apt-update)", "Refresh running-host package metadata (apt-update)",
      true, "Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source." },
    { "apt-upgrade", "upgrade",
      "Upgrade installed packages (apt-upgrade)", "Upgrade running-host packages (apt-upgrade)",
      true, "Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards." },
    { "initramfs", "initramfs",
      "Rebuild initramfs (initramfs)", "Rebuild running-host initramfs (initramfs)",
      true, "Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights." }
};
const int actionSpecCount = sizeof(actionSpecs) / sizeof(actionSpecs[0]);

// Modern-GUI features that stay deliberately greyed on this frontend. They are
// informational only: no stage string here is ever wired to a command, so the
// legacy action surface keeps its exact command set.
struct UnsupportedSpec {
    const char *feature;
    const char *reason;
};
const UnsupportedSpec unsupportedSpecs[] = {
    { "Full Repair plan",
      "run the individual gated repair tools instead; the legacy helper exposes no combined plan" },
    { "Snapshots (Btrfs/Snapper)",
      "no Btrfs/snapper evidence on the selected target (probe-gated)" },
    { "File copy (rsync)",
      "missing rsync or host/target containment evidence on Etch" },
    { "Chroot shell",
      "the legacy helper exposes no chroot shell command" },
    { "Host default / host reboot",
      "not supported by the legacy helper on Etch" },
    { "EFI / UKI / extlinux repair",
      "no EFI or extlinux boot path; see the Diagnostics capability lines" },
    { "Settings tab",
      "the legacy frontend has no persistent settings; use the CLI flags and environment" }
};
const int unsupportedSpecCount = sizeof(unsupportedSpecs) / sizeof(unsupportedSpecs[0]);

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

QString describeState(const std::string &state)
{
    if (state.empty()) {
        return QString::fromLatin1("not reported");
    }
    if (state == "available") {
        return QString::fromLatin1("available");
    }
    if (state.size() >= 12 && state.compare(0, 12, "unavailable|") == 0) {
        return QString::fromLatin1("unavailable");
    }
    return QString::fromLatin1("unrecognised");
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

} // namespace

LegacyMainWindow::LegacyMainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name),
      m_tabs(0),
      m_scopeCombo(0),
      m_diskCombo(0),
      m_rootCombo(0),
      m_unlockCombo(0),
      m_diagFilterCombo(0),
      m_logFilterCombo(0),
      m_deviceList(0),
      m_detailList(0),
      m_capabilityList(0),
      m_unsupportedList(0),
      m_sessionLogList(0),
      m_rawView(0),
      m_logView(0),
      m_unlockStatusView(0),
      m_logSearchEdit(0),
      m_logWrapCheck(0),
      m_scopeHint(0),
      m_gateHint(0),
      m_elevationLabel(0),
      m_targetSummary(0),
      m_settingsHelperLabel(0),
      m_settingsElevationLabel(0),
      m_settingsLogDirLabel(0),
      m_settingsSessionLabel(0),
      m_settingsVersionLabel(0),
      m_scanButton(0),
      m_diagnosticsButton(0),
      m_cancelButton(0),
      m_unlockButton(0),
      m_elevateButton(0),
      m_setTargetButton(0),
      m_hostMaintenanceButton(0),
      m_runner(new HelperRunner(this)),
      m_updatingCombos(false),
      m_running(false),
      m_pendingDiagnostic(false),
      m_pendingUnlock(false),
      m_unlockRetry(false),
      m_targetCommitted(false),
      m_hostMaintenance(false),
      m_viewingPriorLog(false),
      m_smokeMode(false),
      m_smokeStep(0),
      m_smokeDiagnoseOk(false),
      m_smokeValidateOk(false)
{
    setCaption(QString::fromLatin1("Boot Bitch Legacy (Etch / KDE 3.5 era)"));
    // Every group title/button/list column must stay fully visible at the
    // 1024x768 contract size; the layouts only need a modest floor below it.
    setMinimumSize(820, 600);

    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);
    m_tabs->addTab(buildTargetsTab(), QString::fromLatin1("Systems"));
    m_tabs->addTab(buildDiagnosticsTab(), QString::fromLatin1("Diagnostics"));
    m_tabs->addTab(buildActionsTab(), QString::fromLatin1("Repair"));
    m_tabs->addTab(buildChrootShellTab(), QString::fromLatin1("Chroot Shell"));
    m_tabs->addTab(buildFileCopyTab(), QString::fromLatin1("File Copy"));
    m_tabs->addTab(buildLogTab(), QString::fromLatin1("Logs"));
    m_tabs->addTab(buildSettingsTab(), QString::fromLatin1("Settings"));
    m_tabs->addTab(buildAboutTab(), QString::fromLatin1("About / TUI"));

    buildMenus();

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
    m_updatingCombos = true;
    if (!disk.isEmpty()) {
        if (m_diskCombo->currentText() != disk) {
            int index = m_diskCombo->currentItem();
            bool found = false;
            for (int i = 0; i < m_diskCombo->count(); ++i) {
                if (m_diskCombo->text(i) == disk) {
                    index = i;
                    found = true;
                    break;
                }
            }
            if (found) {
                m_diskCombo->setCurrentItem(index);
            } else {
                m_diskCombo->insertItem(disk);
                m_diskCombo->setCurrentItem(m_diskCombo->count() - 1);
            }
        }
    }
    if (!root.isEmpty()) {
        if (m_rootCombo->currentText() != root) {
            int index = m_rootCombo->currentItem();
            bool found = false;
            for (int i = 0; i < m_rootCombo->count(); ++i) {
                if (m_rootCombo->text(i) == root) {
                    index = i;
                    found = true;
                    break;
                }
            }
            if (found) {
                m_rootCombo->setCurrentItem(index);
            } else {
                m_rootCombo->insertItem(root);
                m_rootCombo->setCurrentItem(m_rootCombo->count() - 1);
            }
        }
    }
    m_updatingCombos = false;
    refreshRootCombo();
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

QPushButton *LegacyMainWindow::makeButton(const QString &text, QWidget *parent)
{
    QPushButton *button = new QPushButton(text, parent);
    QFontMetrics metrics(button->font());
    button->setMinimumWidth(QMAX(button->minimumWidth(), metrics.width(text) + 34));
    button->setMinimumHeight(QMAX(button->minimumHeight(), metrics.height() + 14));
    m_buttons.push_back(button);
    return button;
}

void LegacyMainWindow::registerGroupBox(QGroupBox *box)
{
    if (!box) {
        return;
    }
    QFontMetrics metrics(box->font());
    const int titleWidth = metrics.width(box->title()) + 2 * box->frameWidth() + 18;
    box->setMinimumWidth(QMAX(box->minimumWidth(), titleWidth));
    m_groupBoxes.push_back(box);
}

void LegacyMainWindow::buildMenus()
{
    QPopupMenu *fileMenu = new QPopupMenu(this);
    menuBar()->insertItem(QString::fromLatin1("&File"), fileMenu);
    fileMenu->insertItem(QString::fromLatin1("&Save log..."), this, SLOT(saveLog()));
    fileMenu->insertItem(QString::fromLatin1("&Clear log"), this, SLOT(clearLog()));
    fileMenu->insertSeparator();
    fileMenu->insertItem(QString::fromLatin1("&Quit"), qApp, SLOT(quit()), CTRL + Key_Q);

    QPopupMenu *helpMenu = new QPopupMenu(this);
    menuBar()->insertItem(QString::fromLatin1("&Help"), helpMenu);
    helpMenu->insertItem(QString::fromLatin1("&About"), this, SLOT(showAbout()));
}

QWidget *LegacyMainWindow::buildTargetsTab()
{
    QSplitter *splitter = new QSplitter(Qt::Horizontal, m_tabs);
    splitter->setChildrenCollapsible(false);

    // ---- Left column: read-only inventory + the selected scope/target -------
    QWidget *left = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(left, 8, 6);

    QGroupBox *devices = new QGroupBox(
        QString::fromLatin1("Devices - read-only kernel inventory"), left);
    QToolTip::add(devices, QString::fromLatin1(
        "Sources: /proc/partitions, /proc/mounts, /proc/swaps, /sys/block, "
        "/dev/mapper and /dev/disk/by-*. No block device is opened and nothing "
        "is written."));
    QVBoxLayout *devicesLayout = new QVBoxLayout(devices, 8, 4);

    m_deviceList = new QListView(devices);
    m_deviceList->addColumn(QString::fromLatin1("Device"), 100);
    m_deviceList->addColumn(QString::fromLatin1("Size"), 60);
    m_deviceList->addColumn(QString::fromLatin1("Type"), 80);
    m_deviceList->addColumn(QString::fromLatin1("Filesystem"), 75);
    m_deviceList->addColumn(QString::fromLatin1("Mount"), 100);
    m_deviceList->addColumn(QString::fromLatin1("Note"), 155);
    m_deviceList->setAllColumnsShowFocus(true);
    m_deviceList->setShowSortIndicator(true);
    m_deviceList->setMultiSelection(false);
    m_deviceList->setMinimumHeight(80);
    connect(m_deviceList, SIGNAL(selectionChanged()), this, SLOT(deviceSelectionChanged()));
    devicesLayout->addWidget(m_deviceList, 1);

    QHBoxLayout *deviceButtons = new QHBoxLayout(devicesLayout);
    deviceButtons->setSpacing(6);
    m_scanButton = makeButton(QString::fromLatin1("Rescan devices"), devices);
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(scanDevices()));
    deviceButtons->addWidget(m_scanButton);
    deviceButtons->addStretch();

    QLabel *deviceHint = new QLabel(
        QString::fromLatin1("A disk fills the drive field; a partition or mapper fills the root field."),
        devices);
    deviceHint->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    devicesLayout->addWidget(deviceHint);
    leftLayout->addWidget(devices, 3);

    QGroupBox *target = new QGroupBox(
        QString::fromLatin1("Selected scope and target"), left);
    QToolTip::add(target, QString::fromLatin1(
        "Confirmed by the helper's read-only diagnostics; the GUI never reads "
        "a block device itself."));
    QGridLayout *targetLayout = new QGridLayout(target, 7, 2, 8, 4);
    targetLayout->setColStretch(1, 1);

    targetLayout->addWidget(new QLabel(QString::fromLatin1("Scope:"), target), 0, 0);
    m_scopeCombo = new QComboBox(target);
    m_scopeCombo->insertItem(QString::fromLatin1("Running host (read-only + host-repair stages)"));
    m_scopeCombo->insertItem(QString::fromLatin1("Offline repair target (validate/diagnose/repair)"));
    connect(m_scopeCombo, SIGNAL(activated(int)), this, SLOT(scopeChanged(int)));
    targetLayout->addWidget(m_scopeCombo, 0, 1);

    targetLayout->addWidget(new QLabel(QString::fromLatin1("Physical drive:"), target), 1, 0);
    m_diskCombo = new QComboBox(true, target);
    connect(m_diskCombo, SIGNAL(textChanged(const QString &)), this, SLOT(targetEdited()));
    targetLayout->addWidget(m_diskCombo, 1, 1);

    targetLayout->addWidget(new QLabel(QString::fromLatin1("Root component:"), target), 2, 0);
    m_rootCombo = new QComboBox(true, target);
    connect(m_rootCombo, SIGNAL(textChanged(const QString &)), this, SLOT(targetEdited()));
    targetLayout->addWidget(m_rootCombo, 2, 1);

    targetLayout->addWidget(new QLabel(QString::fromLatin1("LUKS component:"), target), 3, 0);
    m_unlockCombo = new QComboBox(true, target);
    QToolTip::add(m_unlockCombo, QString::fromLatin1(
        "The encrypted component the helper should open with cryptsetup. It is "
        "verified against the selected drive by the helper's own preflights."));
    targetLayout->addWidget(m_unlockCombo, 3, 1);

    m_unlockButton = makeButton(QString::fromLatin1("Unlock encrypted target..."), target);
    m_unlockButton->setEnabled(false);
    connect(m_unlockButton, SIGNAL(clicked()), this, SLOT(runUnlock()));
    targetLayout->addWidget(m_unlockButton, 4, 0, Qt::AlignLeft);

    m_hostMaintenanceButton = makeButton(QString::fromLatin1("Host Maintenance"), target);
    m_hostMaintenanceButton->setEnabled(false);
    connect(m_hostMaintenanceButton, SIGNAL(clicked()), this, SLOT(toggleHostMaintenance()));
    targetLayout->addWidget(m_hostMaintenanceButton, 4, 1, Qt::AlignLeft);

    m_setTargetButton = makeButton(QString::fromLatin1("Set as repair target"), target);
    m_setTargetButton->setEnabled(false);
    connect(m_setTargetButton, SIGNAL(clicked()), this, SLOT(setRepairTarget()));
    targetLayout->addWidget(m_setTargetButton, 5, 0, Qt::AlignLeft);

    m_targetSummary = new QLabel(QString::fromLatin1("Committed target: none"), target);
    m_targetSummary->setTextFormat(Qt::PlainText);
    m_targetSummary->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    targetLayout->addWidget(m_targetSummary, 5, 1);

    m_scopeHint = new QLabel(
        QString::fromLatin1("Host scope: live-root disk (e.g. /dev/hda) + mounted root component."),
        target);
    m_scopeHint->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    targetLayout->addMultiCellWidget(m_scopeHint, 6, 6, 0, 1);

    leftLayout->addWidget(target, 2);

    // ---- Right column: selected drive details + unlock status ----------------
    QWidget *right = new QWidget(splitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(right, 8, 6);

    QGroupBox *details = new QGroupBox(
        QString::fromLatin1("Selected drive details"), right);
    QToolTip::add(details, QString::fromLatin1(
        "Read-only inventory plus helper-confirmed facts; mirrors the modern "
        "Qt6 Selected drive details panel."));
    QVBoxLayout *detailsLayout = new QVBoxLayout(details, 8, 4);
    m_detailList = new QListView(details);
    m_detailList->addColumn(QString::fromLatin1("Field"), 120);
    m_detailList->addColumn(QString::fromLatin1("Value"), 220);
    m_detailList->setAllColumnsShowFocus(true);
    m_detailList->setMinimumHeight(140);
    detailsLayout->addWidget(m_detailList, 1);
    rightLayout->addWidget(details, 3);

    QGroupBox *unlock = new QGroupBox(QString::fromLatin1("Unlock status"), right);
    QToolTip::add(unlock, QString::fromLatin1(
        "Unlock state for the selected drive; the LUKS passphrase is never logged."));
    QVBoxLayout *unlockLayout = new QVBoxLayout(unlock, 8, 4);
    m_unlockStatusView = new QTextEdit(unlock);
    m_unlockStatusView->setReadOnly(true);
    m_unlockStatusView->setTextFormat(Qt::PlainText);
    m_unlockStatusView->setWordWrap(QTextEdit::WidgetWidth);
    m_unlockStatusView->setMinimumHeight(110);
    unlockLayout->addWidget(m_unlockStatusView, 1);
    rightLayout->addWidget(unlock, 2);

    registerGroupBox(devices);
    registerGroupBox(target);
    registerGroupBox(details);
    registerGroupBox(unlock);

    QValueList<int> sizes;
    sizes.append(620);
    sizes.append(380);
    splitter->setSizes(sizes);
    return splitter;
}

QWidget *LegacyMainWindow::buildDiagnosticsTab()
{
    QSplitter *splitter = new QSplitter(Qt::Vertical, m_tabs);
    splitter->setChildrenCollapsible(false);

    QGroupBox *capabilities = new QGroupBox(
        QString::fromLatin1("Repair capability lines"), splitter);
    QToolTip::add(capabilities, QString::fromLatin1(
        "Unavailable tools are greyed with the helper's probe reason; a "
        "missing line keeps the tool disabled (fail closed)."));
    QVBoxLayout *capLayout = new QVBoxLayout(capabilities, 8, 4);

    QHBoxLayout *filterRow = new QHBoxLayout(capLayout);
    filterRow->setSpacing(6);
    filterRow->addWidget(new QLabel(QString::fromLatin1("Filter:"), capabilities));
    m_diagFilterCombo = new QComboBox(capabilities);
    m_diagFilterCombo->insertItem(QString::fromLatin1("All tools"));
    m_diagFilterCombo->insertItem(QString::fromLatin1("Available only"));
    m_diagFilterCombo->insertItem(QString::fromLatin1("Unavailable only"));
    connect(m_diagFilterCombo, SIGNAL(activated(int)), this, SLOT(diagnosticsFilterChanged()));
    filterRow->addWidget(m_diagFilterCombo);
    filterRow->addStretch();
    m_diagnosticsButton = makeButton(QString::fromLatin1("Run All diagnostics (read-only)"), capabilities);
    connect(m_diagnosticsButton, SIGNAL(clicked()), this, SLOT(runDiagnostics()));
    filterRow->addWidget(m_diagnosticsButton);
    m_cancelButton = makeButton(QString::fromLatin1("Cancel running command"), capabilities);
    m_cancelButton->setEnabled(false);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancelRun()));
    filterRow->addWidget(m_cancelButton);

    m_capabilityList = new QListView(capabilities);
    m_capabilityList->addColumn(QString::fromLatin1("Repair tool"), 130);
    m_capabilityList->addColumn(QString::fromLatin1("State"), 100);
    m_capabilityList->addColumn(QString::fromLatin1("Reason / evidence"), 500);
    m_capabilityList->setAllColumnsShowFocus(true);
    m_capabilityList->setMinimumHeight(90);
    capLayout->addWidget(m_capabilityList, 1);

    QGroupBox *raw = new QGroupBox(
        QString::fromLatin1("Raw helper evidence (read-only diagnostics)"), splitter);
    QVBoxLayout *rawLayout = new QVBoxLayout(raw, 8, 4);
    m_rawView = new QTextEdit(raw);
    m_rawView->setReadOnly(true);
    m_rawView->setTextFormat(Qt::LogText);
    m_rawView->setWordWrap(QTextEdit::NoWrap);
    m_rawView->setMinimumHeight(70);
    rawLayout->addWidget(m_rawView);

    registerGroupBox(capabilities);
    registerGroupBox(raw);

    QValueList<int> sizes;
    sizes.append(260);
    sizes.append(220);
    splitter->setSizes(sizes);
    return splitter;
}

QWidget *LegacyMainWindow::buildActionsTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    QGroupBox *elevation = new QGroupBox(
        QString::fromLatin1("Privilege elevation"), page);
    QVBoxLayout *elevationLayout = new QVBoxLayout(elevation, 8, 4);
    m_elevationLabel = new QLabel(elevation);
    m_elevationLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    elevationLayout->addWidget(m_elevationLabel);
    QHBoxLayout *elevationButtons = new QHBoxLayout(elevationLayout);
    elevationButtons->setSpacing(6);
    m_elevateButton = makeButton(QString::fromLatin1("Re-check elevation"), elevation);
    connect(m_elevateButton, SIGNAL(clicked()), this, SLOT(recheckElevation()));
    elevationButtons->addWidget(m_elevateButton);
    elevationButtons->addStretch();

    m_gateHint = new QLabel(page);
    m_gateHint->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_gateHint);

    QGroupBox *actions = new QGroupBox(
        QString::fromLatin1("Individual repair tools (gated by cached capability lines)"),
        page);
    QGridLayout *grid = new QGridLayout(actions, (actionSpecCount + 1) / 2, 2, 8, 6);
    grid->setColStretch(0, 1);
    grid->setColStretch(1, 1);
    for (int i = 0; i < actionSpecCount; ++i) {
        const ActionSpec &spec = actionSpecs[i];
        QPushButton *button = makeButton(QString::fromLatin1(spec.label), actions);
        button->setName(spec.stage);
        connect(button, SIGNAL(clicked()), this, SLOT(runAction()));
        m_actionButtons.insert(QString::fromLatin1(spec.stage), button);
        grid->addWidget(button, i / 2, i % 2);
    }
    layout->addWidget(actions);

    QLabel *note = new QLabel(
        QString::fromLatin1(
            "Write actions ask for confirmation and then run the helper's own "
            "runtime preflights; the GUI never weakens them. A repair that is "
            "not proven 'unchanged' invalidates the cached diagnostics and "
            "disables the gated actions until diagnostics run again."),
        page);
    note->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(note);

    QGroupBox *unsupported = new QGroupBox(
        QString::fromLatin1("Modern features not available on this legacy frontend"),
        page);
    QVBoxLayout *unsupportedLayout = new QVBoxLayout(unsupported, 8, 4);
    m_unsupportedList = new QListView(unsupported);
    m_unsupportedList->addColumn(QString::fromLatin1("Modern feature"), 180);
    m_unsupportedList->addColumn(QString::fromLatin1("State"), 90);
    m_unsupportedList->addColumn(QString::fromLatin1("Reason"), 420);
    m_unsupportedList->setAllColumnsShowFocus(true);
    m_unsupportedList->setMinimumHeight(80);
    for (int i = 0; i < unsupportedSpecCount; ++i) {
        QListViewItem *item = new QListViewItem(
            m_unsupportedList,
            QString::fromLatin1(unsupportedSpecs[i].feature),
            QString::fromLatin1("unavailable"),
            QString::fromLatin1(unsupportedSpecs[i].reason));
        item->setEnabled(false);
    }
    unsupportedLayout->addWidget(m_unsupportedList);
    layout->addWidget(unsupported);

    registerGroupBox(elevation);
    registerGroupBox(actions);
    registerGroupBox(unsupported);
    return page;
}

QWidget *LegacyMainWindow::buildChrootShellTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    QGroupBox *shell = new QGroupBox(
        QString::fromLatin1("Chroot shell (unavailable on this legacy frontend)"),
        page);
    QVBoxLayout *shellLayout = new QVBoxLayout(shell, 8, 4);

    QLabel *reason = new QLabel(
        QString::fromLatin1(
            "The legacy helper exposes no chroot shell command, and Etch has no "
            "unshare/timeout containment evidence, so a target chroot shell "
            "cannot be started here. This mirrors the modern tab layout with "
            "every control greyed and the exact probe reason."),
        shell);
    reason->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    shellLayout->addWidget(reason);

    QHBoxLayout *commandRow = new QHBoxLayout(shellLayout);
    commandRow->setSpacing(6);
    commandRow->addWidget(new QLabel(QString::fromLatin1("Command:"), shell));
    QLineEdit *commandEdit = new QLineEdit(shell);
    commandEdit->setEnabled(false);
    commandEdit->setText(QString::fromLatin1("unavailable: no chroot shell command in the legacy helper"));
    commandRow->addWidget(commandEdit, 1);
    QPushButton *run = makeButton(QString::fromLatin1("Run in chroot"), shell);
    run->setEnabled(false);
    commandRow->addWidget(run);
    QPushButton *clear = makeButton(QString::fromLatin1("Clear output"), shell);
    clear->setEnabled(false);
    commandRow->addWidget(clear);

    QTextEdit *output = new QTextEdit(shell);
    output->setReadOnly(true);
    output->setEnabled(false);
    output->setTextFormat(Qt::LogText);
    output->setMinimumHeight(140);
    output->setText(QString::fromLatin1(
        "Modern-only workflow. On Etch the helper reports shell/host-shell "
        "unavailable (missing unshare/timeout); use boot-repair-legacy --tui "
        "for the supported helper commands."));
    shellLayout->addWidget(output, 1);

    registerGroupBox(shell);
    layout->addWidget(shell, 1);
    return page;
}

QWidget *LegacyMainWindow::buildFileCopyTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    QGroupBox *copy = new QGroupBox(
        QString::fromLatin1("File copy (unavailable on this legacy frontend)"),
        page);
    QVBoxLayout *copyLayout = new QVBoxLayout(copy, 8, 4);

    QLabel *reason = new QLabel(
        QString::fromLatin1(
            "rsync is not installed in the recovery environment and the legacy "
            "helper exposes no file-copy command, so this workflow stays "
            "greyed. The modern tab layout is mirrored with the helper's probe "
            "reason."),
        copy);
    reason->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    copyLayout->addWidget(reason);

    QHBoxLayout *body = new QHBoxLayout(copyLayout);
    body->setSpacing(8);
    QVBoxLayout *sources = new QVBoxLayout(body);
    sources->addWidget(new QLabel(QString::fromLatin1("Sources:"), copy));
    QListView *sourceList = new QListView(copy);
    sourceList->addColumn(QString::fromLatin1("Source"), 150);
    sourceList->setEnabled(false);
    sourceList->setMinimumHeight(90);
    sources->addWidget(sourceList, 1);

    QVBoxLayout *controls = new QVBoxLayout(body);
    controls->addWidget(new QLabel(QString::fromLatin1("Destination:"), copy));
    QLineEdit *destination = new QLineEdit(copy);
    destination->setEnabled(false);
    destination->setText(QString::fromLatin1("unavailable: missing rsync/containment evidence"));
    controls->addWidget(destination);
    QPushButton *addFiles = makeButton(QString::fromLatin1("Add files..."), copy);
    addFiles->setEnabled(false);
    controls->addWidget(addFiles);
    QPushButton *addFolder = makeButton(QString::fromLatin1("Add folder..."), copy);
    addFolder->setEnabled(false);
    controls->addWidget(addFolder);
    QPushButton *remove = makeButton(QString::fromLatin1("Remove selected"), copy);
    remove->setEnabled(false);
    controls->addWidget(remove);
    QPushButton *runCopy = makeButton(QString::fromLatin1("Copy"), copy);
    runCopy->setEnabled(false);
    controls->addWidget(runCopy);
    controls->addStretch();

    registerGroupBox(copy);
    layout->addWidget(copy, 1);
    return page;
}

QWidget *LegacyMainWindow::buildLogTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 4);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    QGroupBox *sessions = new QGroupBox(QString::fromLatin1("Session logs"), splitter);
    QVBoxLayout *sessionLayout = new QVBoxLayout(sessions, 8, 4);
    m_sessionLogList = new QListView(sessions);
    m_sessionLogList->addColumn(QString::fromLatin1("Session"), 150);
    m_sessionLogList->setAllColumnsShowFocus(true);
    m_sessionLogList->setMinimumHeight(120);
    QToolTip::add(m_sessionLogList, QString::fromLatin1(
        "The first entry is the live session; earlier files in the log "
        "directory are listed read-only below it."));
    connect(m_sessionLogList, SIGNAL(selectionChanged()), this, SLOT(sessionLogSelectionChanged()));
    sessionLayout->addWidget(m_sessionLogList, 1);
    QHBoxLayout *sessionButtons = new QHBoxLayout(sessionLayout);
    sessionButtons->setSpacing(4);
    QPushButton *refresh = makeButton(QString::fromLatin1("Refresh"), sessions);
    connect(refresh, SIGNAL(clicked()), this, SLOT(refreshSessionLogs()));
    sessionButtons->addWidget(refresh);
    sessionButtons->addStretch();

    QGroupBox *applicationLog = new QGroupBox(
        QString::fromLatin1("Application log"), splitter);
    QVBoxLayout *logLayout = new QVBoxLayout(applicationLog, 8, 4);

    QHBoxLayout *filterRow = new QHBoxLayout(logLayout);
    filterRow->setSpacing(4);
    filterRow->addWidget(new QLabel(QString::fromLatin1("Search log:"), applicationLog));
    m_logSearchEdit = new QLineEdit(applicationLog);
    m_logSearchEdit->setText(QString::null);
    QToolTip::add(m_logSearchEdit, QString::fromLatin1(
        "Type any characters to show matching log entries (case-insensitive). "
        "Save log always writes every entry."));
    connect(m_logSearchEdit, SIGNAL(textChanged(const QString &)), this, SLOT(logSearchChanged()));
    filterRow->addWidget(m_logSearchEdit, 1);
    filterRow->addWidget(new QLabel(QString::fromLatin1("Filter:"), applicationLog));
    m_logFilterCombo = new QComboBox(applicationLog);
    m_logFilterCombo->insertItem(QString::fromLatin1("All entries"));
    m_logFilterCombo->insertItem(QString::fromLatin1("Errors and warnings"));
    QToolTip::add(m_logFilterCombo, QString::fromLatin1(
        "Filter the visible log; Save log always writes every entry."));
    connect(m_logFilterCombo, SIGNAL(activated(int)), this, SLOT(logFilterChanged()));
    filterRow->addWidget(m_logFilterCombo);

    m_logView = new QTextEdit(applicationLog);
    m_logView->setReadOnly(true);
    m_logView->setTextFormat(Qt::LogText);
    m_logView->setWordWrap(QTextEdit::NoWrap);
    m_logView->setMinimumHeight(120);
    logLayout->addWidget(m_logView, 1);

    QHBoxLayout *buttons = new QHBoxLayout(logLayout);
    buttons->setSpacing(4);
    QPushButton *save = makeButton(QString::fromLatin1("Save log..."), applicationLog);
    QToolTip::add(save, QString::fromLatin1("Save the complete session log (all entries, not just the current filter)."));
    connect(save, SIGNAL(clicked()), this, SLOT(saveLog()));
    buttons->addWidget(save);
    QPushButton *clear = makeButton(QString::fromLatin1("Clear log"), applicationLog);
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
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    QGroupBox *application = new QGroupBox(
        QString::fromLatin1("Application configuration"), page);
    QGridLayout *appGrid = new QGridLayout(application, 5, 2, 8, 4);
    appGrid->setColStretch(1, 1);
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
        appGrid->addWidget(new QLabel(QString::fromLatin1(fieldNames[i]), application), i, 0);
        appGrid->addWidget(valueLabels[i], i, 1);
    }
    layout->addWidget(application);

    QGroupBox *diagnostics = new QGroupBox(QString::fromLatin1("Diagnostics"), page);
    QVBoxLayout *diagnosticsLayout = new QVBoxLayout(diagnostics, 8, 4);
    QCheckBox *autoRefresh = new QCheckBox(
        QString::fromLatin1("Automatically regenerate read-only diagnostics after repairs or target changes"),
        diagnostics);
    autoRefresh->setChecked(true);
    autoRefresh->setEnabled(false);
    QToolTip::add(autoRefresh, QString::fromLatin1(
        "Not available on the legacy frontend: the helper has no persistent "
        "privileged session, so diagnostics must be re-run manually."));
    diagnosticsLayout->addWidget(autoRefresh);
    layout->addWidget(diagnostics);

    QGroupBox *discovery = new QGroupBox(QString::fromLatin1("Device discovery"), page);
    QVBoxLayout *discoveryLayout = new QVBoxLayout(discovery, 8, 4);
    const char *discoveryItems[] = {
        "Show devices without an identified Linux installation",
        "Show removable and USB storage",
        "Show encrypted devices before unlocking"
    };
    for (int i = 0; i < 3; ++i) {
        QCheckBox *check = new QCheckBox(QString::fromLatin1(discoveryItems[i]), discovery);
        check->setChecked(true);
        check->setEnabled(false);
        QToolTip::add(check, QString::fromLatin1(
            "The legacy frontend always renders the complete read-only kernel "
            "inventory; this modern filter is not implemented here."));
        discoveryLayout->addWidget(check);
    }
    layout->addWidget(discovery);

    QGroupBox *logs = new QGroupBox(QString::fromLatin1("Logs"), page);
    QVBoxLayout *logsLayout = new QVBoxLayout(logs, 8, 4);
    m_logWrapCheck = new QCheckBox(QString::fromLatin1("Wrap long log lines"), logs);
    connect(m_logWrapCheck, SIGNAL(toggled(bool)), this, SLOT(toggleLogWrap(bool)));
    logsLayout->addWidget(m_logWrapCheck);
    QLabel *saveNote = new QLabel(
        QString::fromLatin1("Save log always writes every entry, independent of the search and filter controls."),
        logs);
    saveNote->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    logsLayout->addWidget(saveNote);
    layout->addWidget(logs);

    QGroupBox *safety = new QGroupBox(
        QString::fromLatin1("Mandatory safety controls"), page);
    QVBoxLayout *safetyLayout = new QVBoxLayout(safety, 8, 4);
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
    layout->addStretch();

    registerGroupBox(application);
    registerGroupBox(diagnostics);
    registerGroupBox(discovery);
    registerGroupBox(logs);
    registerGroupBox(safety);
    return page;
}

QWidget *LegacyMainWindow::buildAboutTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 12, 8);

    QLabel *about = new QLabel(
        QString::fromLatin1(
            "<b>Boot Bitch Legacy</b> - Qt3 frontend for Debian Etch / KDE 3.5-era systems.<br><br>"
            "This GUI is a thin client for the ported privileged helper "
            "(<tt>/usr/sbin/boot-repair-legacy-helper</tt>). It renders the "
            "helper's read-only diagnostics, greys unavailable tools with the "
            "helper's own probe reason, and only enables commands whose "
            "capability line says <tt>available</tt>. The LUKS passphrase "
            "travels only over the helper's standard input; a plain sudo "
            "password is requested in a modal hidden-input dialog and fed to "
            "<tt>sudo -S -v</tt> over a pipe.<br><br>"
            "<b>Modern GUI parity:</b> Systems (inventory, selected drive "
            "details, scope/target commit, Host Maintenance, unlock status), "
            "Diagnostics (Run All + all/available/unavailable filter), Repair "
            "(gated tools + elevation state), Chroot Shell and File Copy "
            "(greyed with their probe reasons), Logs (search, all/errors filter "
            "and session list) and Settings (read-only configuration + greyed "
            "modern options) mirror the Qt6 hierarchy. Snapshots, host "
            "default/reboot and the Full Repair plan stay deliberately "
            "omitted.<br><br>"
            "<b>TUI fallback:</b> when no X session is available, run "
            "<tt>boot-repair-legacy --tui</tt> in a terminal for the same "
            "helper commands through the dialog/console menu. The helper-only "
            "launcher stays installed as the fallback frontend."),
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
    const std::vector<DeviceRow> rows = legacy::scanDevices();
    m_deviceList->clear();
    m_rows.clear();

    QString diskSelection = m_diskCombo->currentText();
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const DeviceRow &row = rows[i];
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
        new QListViewItem(m_deviceList, fromStd(row.path), fromStd(row.size),
                          type, fromStd(row.fstype), fromStd(row.mountpoint),
                          note);
        m_rows.insert(fromStd(row.path), row);
    }
    if (!diskSelection.isEmpty()) {
        setTarget(diskSelection, m_rootCombo->currentText());
    }
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::scopeChanged(int)
{
    if (hostScope()) {
        autoDetectHostTarget();
    } else {
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            m_hostMaintenanceButton->setText(QString::fromLatin1("Host Maintenance"));
        }
    }
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::deviceSelectionChanged()
{
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
        setTarget(path, m_rootCombo->currentText());
        refreshRootCombo();
    } else {
        setTarget(m_diskCombo->currentText(), path);
    }
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::targetEdited()
{
    if (m_updatingCombos) {
        return;
    }
    refreshRootCombo();
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::setRepairTarget()
{
    if (hostScope()) {
        QMessageBox::warning(this, QString::fromLatin1("Repair target scope"),
                             QString::fromLatin1(
                                 "Switch to the offline repair target scope before "
                                 "committing a repair target."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!selectionComplete()) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Select a physical drive and a root component first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!runningHostDisk().isEmpty() && selectedDisk() == runningHostDisk()) {
        QMessageBox::warning(this, QString::fromLatin1("Protected system"),
                             QString::fromLatin1(
                                 "The running system cannot be selected as a repair "
                                 "target. Choose the disposable repair disk."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_targetCommitted = true;
    m_committedDisk = selectedDisk();
    m_committedRoot = selectedRoot();
    appendLog(QString::fromLatin1("Repair target committed: %1 + %2. Diagnostics and gated repairs now target this scope.")
                  .arg(m_committedDisk).arg(m_committedRoot));
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::toggleHostMaintenance()
{
    if (!hostScope()) {
        m_scopeCombo->setCurrentItem(0);
        autoDetectHostTarget();
    }
    m_hostMaintenance = !m_hostMaintenance;
    if (m_hostMaintenanceButton) {
        m_hostMaintenanceButton->setText(m_hostMaintenance
            ? QString::fromLatin1("Exit Host Maintenance")
            : QString::fromLatin1("Host Maintenance"));
    }
    appendLog(m_hostMaintenance
        ? QString::fromLatin1("Running-host maintenance activated; diagnostics and gated repairs now target the protected running host.")
        : QString::fromLatin1("Running-host maintenance deselected; commit an offline repair target to run diagnostics."));
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::refreshRootCombo()
{
    if (m_updatingCombos) {
        return;
    }
    m_updatingCombos = true;
    const QString disk = m_diskCombo->currentText();
    const QString currentRoot = m_rootCombo->currentText();
    const QString currentUnlock = m_unlockCombo->currentText();
    m_rootCombo->clear();
    m_unlockCombo->clear();

    // Partitions of the selected disk plus every mapper (kernel 2.6.18 has no
    // dm/name, so mapper backing devices are resolved best-effort only).
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (row.mapper) {
            m_rootCombo->insertItem(it.key());
        } else if (!row.disk && !row.parent.empty() && row.parent == diskName) {
            m_rootCombo->insertItem(it.key());
            m_unlockCombo->insertItem(it.key());
        }
    }
    if (!currentRoot.isEmpty()) {
        m_rootCombo->insertItem(currentRoot);
        for (int i = 0; i < m_rootCombo->count(); ++i) {
            if (m_rootCombo->text(i) == currentRoot) {
                m_rootCombo->setCurrentItem(i);
                break;
            }
        }
    }
    // The unlock candidate defaults to the root component when that component
    // is a partition (a mapper is already open); the helper still verifies the
    // LUKS container and drive ownership before touching anything.
    if (!currentUnlock.isEmpty()) {
        m_unlockCombo->insertItem(currentUnlock);
        for (int i = 0; i < m_unlockCombo->count(); ++i) {
            if (m_unlockCombo->text(i) == currentUnlock) {
                m_unlockCombo->setCurrentItem(i);
                break;
            }
        }
    } else if (!currentRoot.isEmpty() && !currentRoot.startsWith(QString::fromLatin1("/dev/mapper/"))) {
        for (int i = 0; i < m_unlockCombo->count(); ++i) {
            if (m_unlockCombo->text(i) == currentRoot) {
                m_unlockCombo->setCurrentItem(i);
                break;
            }
        }
        if (m_unlockCombo->currentText() != currentRoot) {
            m_unlockCombo->insertItem(currentRoot);
            m_unlockCombo->setCurrentItem(m_unlockCombo->count() - 1);
        }
    }
    m_updatingCombos = false;
}

void LegacyMainWindow::autoDetectHostTarget()
{
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        m_hostDetectedDisk = fromStd(disk);
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
    return m_scopeCombo->currentItem() == 0;
}

QString LegacyMainWindow::selectedDisk() const
{
    return m_diskCombo->currentText().stripWhiteSpace();
}

QString LegacyMainWindow::selectedRoot() const
{
    return m_rootCombo->currentText().stripWhiteSpace();
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

bool LegacyMainWindow::targetCommitted() const
{
    if (hostScope()) {
        return false;
    }
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

QString LegacyMainWindow::scopeReadyReason() const
{
    if (!selectionComplete()) {
        return QString::fromLatin1(
            "Select a physical drive and a root component first.");
    }
    if (hostScope()) {
        if (!m_hostMaintenance) {
            return QString::fromLatin1(
                "Running-host diagnostics require Host Maintenance. Choose "
                "Host Maintenance on the Systems tab first.");
        }
        return QString::fromLatin1("The running host is ready for diagnostics.");
    }
    if (!m_targetCommitted) {
        return QString::fromLatin1(
            "No repair target is committed. Choose Set as repair target on the "
            "Systems tab first.");
    }
    return QString::fromLatin1(
        "The selection changed after the target was committed. Choose Set as "
        "repair target again.");
}

void LegacyMainWindow::runDiagnostics()
{
    if (!selectionComplete()) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Select a physical drive and a root component first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, QString::fromLatin1("Diagnostics scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-diagnose") : QString::fromLatin1("diagnose"))
         << selectedDisk() << selectedRoot() << QString::fromLatin1("all");
    startCommand(args, true,
                 host ? QString::fromLatin1("host-diagnose all")
                      : QString::fromLatin1("diagnose all"));
}

void LegacyMainWindow::runUnlock()
{
    if (m_running) {
        return;
    }
    if (hostScope()) {
        QMessageBox::warning(this, QString::fromLatin1("Unlock not available"),
                             QString::fromLatin1(
                                 "The protected running host cannot be unlocked. "
                                 "Select the offline repair target scope first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!selectionComplete()) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Select a physical drive and a root component first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString luks = m_unlockCombo->currentText().stripWhiteSpace();
    if (luks.isEmpty()) {
        QMessageBox::warning(this, QString::fromLatin1("Unlock not available"),
                             QString::fromLatin1("Select or type the LUKS component to unlock first."),
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

    // Authorize elevation before collecting the disk passphrase, so the two
    // secrets are never held in memory at the same time.
    if (!promptElevationPassword()) {
        return;
    }

    bool ok = false;
    QString passphrase = QInputDialog::getText(
        QString::fromLatin1("Unlock LUKS repair target"),
        QString::fromLatin1("Enter the passphrase for %1.\n"
                            "It is sent only to cryptsetup over the helper's "
                            "standard input and is never logged or placed on a "
                            "command line.").arg(luks),
        QLineEdit::Password, QString::null, &ok, this);
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

void LegacyMainWindow::diagnosticsFilterChanged()
{
    updateCapabilityView();
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
    refreshLogView();
}

void LegacyMainWindow::refreshSessionLogs()
{
    refreshSessionLogList();
    statusBar()->message(QString::fromLatin1("Session log list refreshed."), 3000);
}

void LegacyMainWindow::toggleLogWrap(bool enabled)
{
    if (m_logView) {
        m_logView->setWordWrap(enabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    }
}

void LegacyMainWindow::runAction()
{
    const QObject *emitter = sender();
    if (!emitter) {
        return;
    }
    const QString stage = QString::fromLatin1(emitter->name());
    for (int i = 0; i < actionSpecCount; ++i) {
        const ActionSpec &spec = actionSpecs[i];
        if (stage != QString::fromLatin1(spec.stage)) {
            continue;
        }
        if (!selectionComplete()) {
            QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                                 QString::fromLatin1("Select a physical drive and a root component first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
        if (!diagnosticsScopeReady()) {
            QMessageBox::information(this, QString::fromLatin1("Scope required"),
                                     scopeReadyReason(),
                                     QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
        if (spec.write) {
            const int answer = QMessageBox::question(
                this, QString::fromLatin1("Confirm repair"),
                QString::fromLatin1(spec.confirm),
                QMessageBox::Yes, QMessageBox::No);
            if (answer != QMessageBox::Yes) {
                return;
            }
        }
        const bool host = hostScope();
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
        startCommand(args, false,
                     (host ? QString::fromLatin1(spec.hostLabel)
                           : QString::fromLatin1(spec.label)));
        return;
    }
}

bool LegacyMainWindow::promptElevationPassword()
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
    QString password = QInputDialog::getText(
        QString::fromLatin1("Administrator authorization"),
        QString::fromLatin1(
            "Administrator authorization is required to run the privileged helper.\n\n"
            "Enter the password for %1 (sudo). It is used only for this sudo "
            "authentication, is sent over a pipe and is never logged or placed "
            "on a command line.")
            .arg(user.isEmpty() ? QString::fromLatin1("your account") : user),
        QLineEdit::Password, QString::null, &ok, this);
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
        "Administrator authorization established with sudo for this session."));
    updateElevationLabel();
    return true;
}

void LegacyMainWindow::startCommand(const QStringList &args,
                                    bool diagnostic, const QString &label,
                                    bool unlock)
{
    if (m_running) {
        return;
    }
    if (m_smokeMode && m_runner->elevationNeedsPassword(0)) {
        // A modal password prompt would hang the headless smoke: fail with the
        // exact remedy instead.
        m_smokeMode = false;
        emit smokeFinished(false, QString::fromLatin1(
            "elevation requires an interactive sudo password; run the smoke as "
            "root or after `sudo -S -v` with --elevate 'sudo -n'"));
        return;
    }
    if (!promptElevationPassword()) {
        // Never leave a collected secret behind when authorization was refused.
        m_runner->setInputData(QByteArray());
        return;
    }
    m_transcript = QString::null;
    m_pendingDiagnostic = diagnostic;
    m_pendingUnlock = unlock;
    m_pendingIdentity = identity();
    m_pendingLabel = label;
    m_running = true;
    m_cancelButton->setEnabled(true);
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
        return;
    }
    m_cancelButton->setEnabled(m_runner->isRunning());
    updateStatus();
}

void LegacyMainWindow::helperLine(const QString &line)
{
    m_transcript += line;
    m_transcript += QString::fromLatin1("\n");
    appendLog(line);
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

    // A cached sudo authorization can expire between commands: drop the cached
    // decision so the next command asks for the password again.
    if (!ok
        && (m_transcript.contains(QString::fromLatin1("sudo:"))
            || m_transcript.contains(QString::fromLatin1("a password is required")))) {
        m_runner->resetElevation();
        appendLog(QString::fromLatin1(
            "The sudo authorization expired or was refused; the next privileged "
            "command will request the password again."));
        updateElevationLabel();
    }

    const std::string transcript = toStd(m_transcript);
    const ParsedTranscript parsed = parseTranscript(transcript);
    if (m_pendingDiagnostic) {
        m_model.applyDiagnosticTranscript(toStd(m_pendingIdentity), transcript, ok);
        updateCapabilityView();
        m_rawView->setText(m_transcript);
        if (ok) {
            updateFactView(parsed);
            mergeHelperDevices(parsed);
        }
    } else {
        m_model.applyCommandTranscript(transcript);
        reportChangeStatuses(parsed);
    }

    const bool wasSmoke = m_smokeMode;
    const bool wasDiagnostic = m_pendingDiagnostic;
    const bool wasUnlock = m_pendingUnlock;
    const QString unlockDevice = m_unlockDevice;
    const QString unlockDisk = m_unlockDisk;
    m_running = false;
    m_pendingDiagnostic = false;
    m_pendingUnlock = false;
    m_pendingLabel = QString::null;
    m_cancelButton->setEnabled(false);
    updateActionStates();
    updateStatus();
    updateDriveDetails();

    if (wasUnlock) {
        handleUnlockFinished(ok, transcript, unlockDevice, unlockDisk);
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
        updateCapabilityView();
        appendLog(QString::fromLatin1(
            "LUKS volume unlocked; the target topology changed, so cached "
            "diagnostics were invalidated. Run diagnostics again."));
        scanDevices();
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
        m_scopeCombo->setCurrentItem(0);
        m_hostMaintenance = true;
        if (m_hostMaintenanceButton) {
            m_hostMaintenanceButton->setText(QString::fromLatin1("Exit Host Maintenance"));
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
        const char *expectedTabs[] = {
            "Systems", "Diagnostics", "Repair", "Chroot Shell", "File Copy",
            "Logs", "Settings", "About / TUI"
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
    if (!m_unlockButton || !m_unlockCombo) {
        problems->append(QString::fromLatin1("unlock controls missing"));
        ok = false;
    }
    if (!m_elevationLabel || m_elevationLabel->text().isEmpty() || !m_elevateButton) {
        problems->append(QString::fromLatin1("elevation control missing or empty"));
        ok = false;
    }
    if (!m_diagFilterCombo || m_diagFilterCombo->count() != 3) {
        problems->append(QString::fromLatin1("diagnostics filter missing (all/available/unavailable)"));
        ok = false;
    }
    if (!m_logFilterCombo || m_logFilterCombo->count() != 2) {
        problems->append(QString::fromLatin1("log filter missing (all/errors)"));
        ok = false;
    }
    if (!m_unsupportedList || m_unsupportedList->childCount() < 5) {
        problems->append(QString::fromLatin1("unsupported-modern-features list missing or incomplete"));
        ok = false;
    }
    if (!m_setTargetButton || !m_hostMaintenanceButton) {
        problems->append(QString::fromLatin1("target commit / Host Maintenance controls missing"));
        ok = false;
    }

    // Fail closed for the protected running host: Unlock must stay disabled.
    m_scopeCombo->setCurrentItem(0);
    m_hostMaintenance = false;
    updateActionStates();
    if (m_unlockButton && m_unlockButton->isEnabled()) {
        problems->append(QString::fromLatin1("unlock button enabled for the running host scope"));
        ok = false;
    }

    // Modern gating: Run Diagnostic needs a committed repair target or active
    // Host Maintenance, never just an auto-filled selection.
    if (m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run Diagnostic enabled without a committed target or Host Maintenance"));
        ok = false;
    }
    m_hostMaintenance = true;
    updateActionStates();
    if (!m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run Diagnostic disabled while Host Maintenance is active"));
        ok = false;
    }
    m_hostMaintenance = false;
    m_scopeCombo->setCurrentItem(1);
    m_targetCommitted = true;
    m_committedDisk = selectedDisk();
    m_committedRoot = selectedRoot();
    updateActionStates();
    if (!m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run Diagnostic disabled for a committed repair target"));
        ok = false;
    }
    m_targetCommitted = false;
    updateActionStates();
    if (m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run Diagnostic enabled for an uncommitted target"));
        ok = false;
    }
    // Restore the running-host scope the smoke's helper commands used.
    m_scopeCombo->setCurrentItem(0);
    m_hostMaintenance = true;
    if (m_hostMaintenanceButton) {
        m_hostMaintenanceButton->setText(QString::fromLatin1("Exit Host Maintenance"));
    }
    updateActionStates();

    // Diagnostics filter: exact counts for all/available/unavailable.
    if (m_diagFilterCombo && m_capabilityList) {
        int available = 0;
        const std::vector<std::string> keys = capabilityKeys();
        for (std::size_t i = 0; i < keys.size(); ++i) {
            if (m_model.state(keys[i]) == "available") {
                ++available;
            }
        }
        const int total = static_cast<int>(keys.size());
        m_diagFilterCombo->setCurrentItem(0);
        updateCapabilityView();
        const int shownAll = m_capabilityList->childCount();
        m_diagFilterCombo->setCurrentItem(1);
        updateCapabilityView();
        const int shownAvailable = m_capabilityList->childCount();
        m_diagFilterCombo->setCurrentItem(2);
        updateCapabilityView();
        const int shownUnavailable = m_capabilityList->childCount();
        m_diagFilterCombo->setCurrentItem(0);
        updateCapabilityView();
        if (shownAll != total || shownAvailable != available
            || shownUnavailable != total - available) {
            problems->append(QString::fromLatin1(
                "diagnostics filter counts wrong: all=%1 available=%2 unavailable=%3 (expected %4/%5/%6)")
                .arg(shownAll).arg(shownAvailable).arg(shownUnavailable)
                .arg(total).arg(available).arg(total - available));
            ok = false;
        }
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
        const QRect pageRect = pageWidget->rect();

        for (std::size_t i = 0; i < m_groupBoxes.size(); ++i) {
            QGroupBox *box = m_groupBoxes[i];
            if (!box->isVisibleTo(m_tabs)) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(box->font());
            const int needed = metrics.width(box->title())
                + 2 * box->frameWidth() + 16;
            if (box->width() < needed) {
                problems->append(QString::fromLatin1(
                    "group title clipped in '%1' (%2px needed, %3px available)")
                    .arg(box->title()).arg(needed).arg(box->width()));
                ok = false;
            }
            const QRect mapped(box->mapTo(pageWidget, QPoint(0, 0)), box->size());
            if (!pageRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "group box '%1' is clipped by the page (at %2,%3 %4x%5, page %6x%7)")
                    .arg(box->title())
                    .arg(mapped.x()).arg(mapped.y())
                    .arg(mapped.width()).arg(mapped.height())
                    .arg(pageRect.width()).arg(pageRect.height()));
                ok = false;
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
            const QRect mapped(button->mapTo(pageWidget, QPoint(0, 0)), button->size());
            if (!pageRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "button '%1' is clipped by the page (at %2,%3 %4x%5, page %6x%7)")
                    .arg(button->text())
                    .arg(mapped.x()).arg(mapped.y())
                    .arg(mapped.width()).arg(mapped.height())
                    .arg(pageRect.width()).arg(pageRect.height()));
                ok = false;
            }
        }

        if (m_deviceList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_deviceList, QString::fromLatin1("device list"), problems)) {
                ok = false;
            }
        }
        if (m_detailList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_detailList, QString::fromLatin1("details list"), problems)) {
                ok = false;
            }
        }
        if (m_capabilityList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_capabilityList, QString::fromLatin1("capability list"), problems)) {
                ok = false;
            }
        }
        if (m_unsupportedList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_unsupportedList, QString::fromLatin1("unsupported list"), problems)) {
                ok = false;
            }
        }
        if (m_sessionLogList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_sessionLogList, QString::fromLatin1("session log list"), problems)) {
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
        if (m_scopeCombo->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!comboTextFits(m_scopeCombo, QString::fromLatin1("scope combo"), problems)) {
                ok = false;
            }
        }
        if (m_diskCombo->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!comboTextFits(m_diskCombo, QString::fromLatin1("drive combo"), problems)) {
                ok = false;
            }
        }
        if (m_rootCombo->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!comboTextFits(m_rootCombo, QString::fromLatin1("root combo"), problems)) {
                ok = false;
            }
        }
        if (m_unlockCombo->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!comboTextFits(m_unlockCombo, QString::fromLatin1("LUKS combo"), problems)) {
                ok = false;
            }
        }
    }
    m_tabs->setCurrentPage(originalPage);
    if (checked) {
        *checked = checkedCount;
    }
    return ok;
}

void LegacyMainWindow::cancelRun()
{
    if (m_runner->isRunning()) {
        appendLog(QString::fromLatin1("Cancelling the running helper command..."));
        m_runner->cancel();
    }
}

void LegacyMainWindow::updateCapabilityView()
{
    const int filter = m_diagFilterCombo ? m_diagFilterCombo->currentItem() : 0;
    m_capabilityList->clear();
    const std::vector<std::string> keys = capabilityKeys();
    for (std::size_t i = 0; i < keys.size(); ++i) {
        const std::string &key = keys[i];
        const std::string state = m_model.state(key);
        const bool available = state == "available";
        if ((filter == 1 && !available) || (filter == 2 && available)) {
            continue;
        }
        QString reason;
        if (state.empty()) {
            reason = QString::fromLatin1("No 'Repair tool %1:' line cached; fail closed.")
                         .arg(fromStd(key));
        } else if (available) {
            const std::string detail = m_model.evidence(key);
            reason = detail.empty()
                ? QString::fromLatin1("available (no evidence line)")
                : fromStd(detail);
        } else {
            std::string detail;
            capabilityIsAvailable(state, &detail);
            reason = fromStd(detail);
        }
        QListViewItem *item = new QListViewItem(
            m_capabilityList, fromStd(key), describeState(state), reason);
        item->setEnabled(available);
    }
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

    QString model;
    QString size;
    QString transport;
    QString fstype;
    QString uuid;
    QString mounts;
    if (selected) {
        model = fromStd(selected->model);
        if (model.isEmpty()) {
            model = fromStd(selected->label);
        } else if (!selected->label.empty()) {
            model += QString::fromLatin1(" (") + fromStd(selected->label) + QString::fromLatin1(")");
        }
        size = fromStd(selected->size);
        transport = fromStd(selected->transport);
        fstype = fromStd(selected->fstype);
        uuid = fromStd(selected->uuid);
        mounts = fromStd(selected->mountpoint);
    }
    if (fstype.isEmpty() && helperConfirmed) {
        fstype = mapValue(m_factMap, QString::fromLatin1("Filesystem:"));
    }

    QString component = selectedRoot();
    if (component.isEmpty() && helperConfirmed) {
        component = m_helperComponent;
    }
    QString status;
    if (disk.isEmpty()) {
        status = QString::fromLatin1("Select a drive to see its details.");
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
    } else {
        protection = QString::fromLatin1("Eligible repair candidate");
    }

    new QListViewItem(m_detailList, QString::fromLatin1("Drive:"), detailOrDash(disk));
    new QListViewItem(m_detailList, QString::fromLatin1("Detected target:"),
                      component.isEmpty() ? QString::fromLatin1("Pending inspection")
                                          : component);
    new QListViewItem(m_detailList, QString::fromLatin1("Model / label:"), detailOrDash(model));
    new QListViewItem(m_detailList, QString::fromLatin1("Status:"), status);
    new QListViewItem(m_detailList, QString::fromLatin1("Size:"), detailOrDash(size));
    new QListViewItem(m_detailList, QString::fromLatin1("Connection:"), detailOrDash(transport));
    new QListViewItem(m_detailList, QString::fromLatin1("Filesystem:"), detailOrDash(fstype));
    new QListViewItem(m_detailList, QString::fromLatin1("UUID:"), detailOrDash(uuid));
    new QListViewItem(m_detailList, QString::fromLatin1("Mounts:"), detailOrDash(mounts));
    new QListViewItem(m_detailList, QString::fromLatin1("Protection:"), protection);
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
    if (hostScope() || (!runningHostDisk().isEmpty() && disk == runningHostDisk())) {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: protected\n"
            "Component: %1\n"
            "The protected running host cannot be unlocked or modified; unlock "
            "is available for an offline repair target only.")
            .arg(m_unlockCombo->currentText().stripWhiteSpace().isEmpty()
                     ? QString::fromLatin1("(none selected)")
                     : m_unlockCombo->currentText().stripWhiteSpace()));
        return;
    }
    const QMap<QString, QString>::const_iterator cached = m_unlockStatusCache.find(disk);
    if (cached != m_unlockStatusCache.end() && !cached.data().stripWhiteSpace().isEmpty()) {
        m_unlockStatusView->setText(cached.data());
        return;
    }
    const QString mapper = visibleMapperForDisk(disk);
    const QString component = m_unlockCombo->currentText().stripWhiteSpace();
    const bool helperUnlocked = !m_helperComponent.isEmpty()
        && m_helperComponent.startsWith(QString::fromLatin1("/dev/mapper/"))
        && !m_helperDisk.isEmpty() && m_helperDisk == disk;
    if (!mapper.isEmpty()) {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: unlocked\n"
            "Component: %1\n"
            "Mapper: %2\n"
            "Method: already open before this session (visible mapper; Boot "
            "Bitch will reuse it and will not close it).")
            .arg(component.isEmpty() ? QString::fromLatin1("(none selected)") : component)
            .arg(mapper));
    } else if (helperUnlocked) {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: unlocked\n"
            "Component: %1\n"
            "Mapper: %2\n"
            "Method: helper-confirmed mapping from the last read-only diagnostics.")
            .arg(component.isEmpty() ? QString::fromLatin1("(none selected)") : component)
            .arg(m_helperComponent));
    } else {
        m_unlockStatusView->setText(QString::fromLatin1(
            "State: locked or no encrypted component detected\n"
            "Component: %1\n"
            "Method: helper unlock (cryptsetup; passphrase on stdin only)\n"
            "No unlock operation recorded for this drive in the current session.")
            .arg(component.isEmpty() ? QString::fromLatin1("(none selected)") : component));
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

void LegacyMainWindow::updateActionStates()
{
    const bool complete = selectionComplete();
    const bool idle = !m_running;
    const bool ready = diagnosticsScopeReady();
    const QString id = identity();

    for (int i = 0; i < actionSpecCount; ++i) {
        const ActionSpec &spec = actionSpecs[i];
        QPushButton *button = m_actionButtons[QString::fromLatin1(spec.stage)];
        if (!button) {
            continue;
        }
        bool enabled = complete && idle && ready;
        QString reason;
        if (!complete) {
            reason = QString::fromLatin1("Select a physical drive and a root component first.");
        } else if (!idle) {
            reason = QString::fromLatin1("A helper command is already running.");
        } else if (!ready) {
            reason = scopeReadyReason();
        } else {
            std::string capabilityReason;
            enabled = m_model.isAvailable(spec.capability, toStd(id), &capabilityReason);
            reason = fromStd(capabilityReason);
        }
        button->setEnabled(enabled);
        QToolTip::add(button, reason);
    }

    const bool diagnosticsEnabled = complete && idle && ready;
    if (m_diagnosticsButton) {
        m_diagnosticsButton->setEnabled(diagnosticsEnabled);
        QToolTip::add(m_diagnosticsButton,
                      diagnosticsEnabled
                          ? QString::fromLatin1("Run the read-only diagnostics for the selected scope; this unlocks the gated actions.")
                          : (complete ? scopeReadyReason()
                                      : QString::fromLatin1("Select a target and wait for any running command first.")));
    }
    if (m_scanButton) {
        m_scanButton->setEnabled(idle);
    }

    // Unlock mirrors the modern GUI: it is offered only for an offline target
    // (never the protected running host) and needs the helper's own
    // cryptsetup preflights at runtime.
    if (m_unlockButton) {
        const bool unlockEnabled = complete && idle && !hostScope();
        m_unlockButton->setEnabled(unlockEnabled);
        if (hostScope()) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "The protected running host cannot be unlocked; select the offline repair target scope."));
        } else if (!complete) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "Select a physical drive and the LUKS component first."));
        } else if (!idle) {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "A helper command is already running."));
        } else {
            QToolTip::add(m_unlockButton, QString::fromLatin1(
                "Run the helper's unlock for the selected LUKS component; the "
                "passphrase is sent on standard input only."));
        }
    }
    if (m_elevateButton) {
        m_elevateButton->setEnabled(idle);
    }
    if (m_setTargetButton) {
        const bool canCommit = !hostScope() && complete && idle
            && !(!runningHostDisk().isEmpty() && selectedDisk() == runningHostDisk());
        m_setTargetButton->setEnabled(canCommit);
        if (hostScope()) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Switch to the offline repair target scope to commit a repair target."));
        } else if (!complete) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Select a physical drive and a root component first."));
        } else if (!runningHostDisk().isEmpty() && selectedDisk() == runningHostDisk()) {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "The protected running system cannot be selected as a repair target."));
        } else {
            QToolTip::add(m_setTargetButton, QString::fromLatin1(
                "Commit this drive + root component as the repair target for "
                "diagnostics and gated repairs."));
        }
    }
    if (m_hostMaintenanceButton) {
        const bool canMaintain = idle && !runningHostDisk().isEmpty();
        m_hostMaintenanceButton->setEnabled(canMaintain);
        QToolTip::add(m_hostMaintenanceButton, canMaintain
            ? (m_hostMaintenance
                ? QString::fromLatin1("Leave Host Maintenance and return to repair-target mode.")
                : QString::fromLatin1("Select the running host for deliberate guarded maintenance."))
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
        if (!complete) {
            m_gateHint->setText(QString::fromLatin1(
                "Select a disk and a root component, then commit the target or "
                "enter Host Maintenance. Actions are unlocked only by the "
                "helper's cached capability lines."));
        } else if (!ready) {
            m_gateHint->setText(scopeReadyReason());
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
    for (QListViewItem *item = m_sessionLogList->firstChild(); item;
         item = item->nextSibling()) {
        const bool matches = wanted == QString::fromLatin1("current")
            ? item == m_sessionLogList->firstChild()
            : item->text(0) == QFileInfo(wanted).fileName();
        if (matches) {
            m_sessionLogList->setSelected(item, true);
            m_sessionLogList->setCurrentItem(item);
            break;
        }
    }
}

void LegacyMainWindow::saveLog()
{
    const QString path = QFileDialog::getSaveFileName(
        QString::null, QString::fromLatin1("Log files (*.log);;All files (*)"),
        this, "save-log", QString::fromLatin1("Save helper log"));
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
    // The complete session is saved even while a filter hides entries.
    stream << m_logLines.join(QString::fromLatin1("\n"));
    if (!m_logLines.isEmpty()) {
        stream << "\n";
    }
    file.close();
}

void LegacyMainWindow::clearLog()
{
    m_logLines.clear();
    m_viewingPriorLog = false;
    m_priorLogPath = QString::null;
    m_priorLogLines.clear();
    refreshSessionLogList();
    refreshLogView();
}

void LegacyMainWindow::showAbout()
{
    QMessageBox::about(
        this, QString::fromLatin1("About Boot Bitch Legacy"),
        QString::fromLatin1(
            "<b>Boot Bitch Legacy</b><br>"
            "Qt3 frontend for Debian Etch / KDE 3.5-era systems.<br><br>"
            "Thin client for the ported privileged helper "
            "<tt>/usr/sbin/boot-repair-legacy-helper</tt>. Unavailable tools are "
            "greyed with the helper's own probe reason; every repair keeps the "
            "helper's runtime preflights.<br><br>"
            "TUI fallback: <tt>boot-repair-legacy --tui</tt>."));
}

} // namespace legacy
