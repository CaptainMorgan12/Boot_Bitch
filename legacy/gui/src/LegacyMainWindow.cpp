// LegacyMainWindow implementation. Qt3. See LegacyMainWindow.h.

#include "LegacyMainWindow.h"
#include "DeviceInventory.h"
#include "HelperRunner.h"

#include <qapplication.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qfont.h>
#include <qgrid.h>
#include <qgroupbox.h>
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

#include <sys/stat.h>
#include <sys/types.h>

#include <string>

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

} // namespace

LegacyMainWindow::LegacyMainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name),
      m_tabs(0),
      m_scopeCombo(0),
      m_diskCombo(0),
      m_rootCombo(0),
      m_deviceList(0),
      m_factList(0),
      m_capabilityList(0),
      m_rawView(0),
      m_logView(0),
      m_scopeHint(0),
      m_gateHint(0),
      m_scanButton(0),
      m_diagnosticsButton(0),
      m_cancelButton(0),
      m_runner(new HelperRunner(this)),
      m_updatingCombos(false),
      m_running(false),
      m_pendingDiagnostic(false),
      m_smokeMode(false),
      m_smokeStep(0),
      m_smokeDiagnoseOk(false)
{
    setCaption(QString::fromLatin1("Boot Bitch Legacy (Etch / KDE 3.5 era)"));
    setMinimumSize(760, 540);

    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);
    m_tabs->addTab(buildTargetsTab(), QString::fromLatin1("Systems / targets"));
    m_tabs->addTab(buildDiagnosticsTab(), QString::fromLatin1("Diagnostics"));
    m_tabs->addTab(buildActionsTab(), QString::fromLatin1("Actions"));
    m_tabs->addTab(buildLogTab(), QString::fromLatin1("Log"));
    m_tabs->addTab(buildAboutTab(), QString::fromLatin1("About / TUI"));

    buildMenus();

    connect(m_runner, SIGNAL(outputLine(const QString &)),
            this, SLOT(helperLine(const QString &)));
    connect(m_runner, SIGNAL(finished(bool, int)),
            this, SLOT(helperFinished(bool, int)));

    m_logDirectory = QDir::homeDirPath() + QString::fromLatin1("/.boot-repair-legacy/logs");
    scanDevices();
    autoDetectHostTarget();
    appendLog(QString::fromLatin1(
        "Boot Bitch legacy GUI ready. Every privileged command runs through the "
        "ported helper; actions stay disabled until diagnostics report their "
        "capability line."));
    updateStatus();
    updateActionStates();
}

void LegacyMainWindow::setHelperPath(const QString &path)
{
    m_runner->setHelperPath(path);
    updateStatus();
}

void LegacyMainWindow::setElevationOverride(const QString &override)
{
    m_runner->setElevationOverride(override);
}

void LegacyMainWindow::setNoElevate(bool noElevate)
{
    m_runner->setNoElevate(noElevate);
}

void LegacyMainWindow::setLogDirectory(const QString &path)
{
    m_logDirectory = path;
    // The constructor already opened a session file under the default
    // directory; move to the requested one before the first helper output.
    m_logPath = QString::null;
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
    updateStatus();
    updateActionStates();
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
    QSplitter *splitter = new QSplitter(Qt::Vertical, m_tabs);

    QGroupBox *devices = new QGroupBox(
        QString::fromLatin1("Devices - read-only kernel inventory (/proc/partitions, /proc/mounts, /sys/block)"),
        splitter);
    QVBoxLayout *devicesLayout = new QVBoxLayout(devices, 6, 4);

    m_deviceList = new QListView(devices);
    m_deviceList->addColumn(QString::fromLatin1("Device"), 110);
    m_deviceList->addColumn(QString::fromLatin1("Size"), 70);
    m_deviceList->addColumn(QString::fromLatin1("Type"), 110);
    m_deviceList->addColumn(QString::fromLatin1("Filesystem"), 90);
    m_deviceList->addColumn(QString::fromLatin1("Mount"), 130);
    m_deviceList->addColumn(QString::fromLatin1("Note"), 200);
    m_deviceList->setAllColumnsShowFocus(true);
    m_deviceList->setShowSortIndicator(true);
    m_deviceList->setMultiSelection(false);
    connect(m_deviceList, SIGNAL(selectionChanged()), this, SLOT(deviceSelectionChanged()));
    devicesLayout->addWidget(m_deviceList);

    QHBoxLayout *deviceButtons = new QHBoxLayout(devicesLayout);
    m_scanButton = new QPushButton(QString::fromLatin1("Rescan devices"), devices);
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(scanDevices()));
    deviceButtons->addWidget(m_scanButton);
    deviceButtons->addStretch();
    QLabel *deviceHint = new QLabel(
        QString::fromLatin1("Selecting a disk fills the physical-drive field; selecting a partition or mapper fills the root field."),
        devices);
    deviceButtons->addWidget(deviceHint);

    QGroupBox *target = new QGroupBox(
        QString::fromLatin1("Selected scope and target (confirmed by the helper's read-only diagnostics)"),
        splitter);
    QGridLayout *targetLayout = new QGridLayout(target, 4, 2, 6, 4);

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

    m_scopeHint = new QLabel(
        QString::fromLatin1("The running-host scope requires the physical disk that hosts the live root, e.g. /dev/hda, and the mounted root component, e.g. /dev/mapper/root."),
        target);
    m_scopeHint->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    targetLayout->addWidget(m_scopeHint, 3, 0);

    QGroupBox *facts = new QGroupBox(
        QString::fromLatin1("Helper-confirmed target facts (from the last diagnostic/validation run)"),
        splitter);
    QVBoxLayout *factsLayout = new QVBoxLayout(facts, 6, 4);
    m_factList = new QListView(facts);
    m_factList->addColumn(QString::fromLatin1("Field"), 200);
    m_factList->addColumn(QString::fromLatin1("Value"), 420);
    m_factList->setAllColumnsShowFocus(true);
    factsLayout->addWidget(m_factList);

    QValueList<int> sizes;
    sizes.append(280);
    sizes.append(150);
    sizes.append(140);
    splitter->setSizes(sizes);
    return splitter;
}

QWidget *LegacyMainWindow::buildDiagnosticsTab()
{
    QSplitter *splitter = new QSplitter(Qt::Vertical, m_tabs);

    QGroupBox *capabilities = new QGroupBox(
        QString::fromLatin1("Repair capability lines - unavailable tools are greyed with their probe reason"),
        splitter);
    QVBoxLayout *capLayout = new QVBoxLayout(capabilities, 6, 4);
    m_capabilityList = new QListView(capabilities);
    m_capabilityList->addColumn(QString::fromLatin1("Repair tool"), 130);
    m_capabilityList->addColumn(QString::fromLatin1("State"), 100);
    m_capabilityList->addColumn(QString::fromLatin1("Reason / evidence"), 520);
    m_capabilityList->setAllColumnsShowFocus(true);
    capLayout->addWidget(m_capabilityList);

    QGroupBox *raw = new QGroupBox(
        QString::fromLatin1("Raw helper evidence (read-only diagnostics)"), splitter);
    QVBoxLayout *rawLayout = new QVBoxLayout(raw, 6, 4);
    m_rawView = new QTextEdit(raw);
    m_rawView->setReadOnly(true);
    m_rawView->setTextFormat(Qt::LogText);
    m_rawView->setWordWrap(QTextEdit::NoWrap);
    rawLayout->addWidget(m_rawView);

    QHBoxLayout *buttons = new QHBoxLayout();
    m_diagnosticsButton = new QPushButton(
        QString::fromLatin1("Run diagnostics (read-only)"), capabilities);
    connect(m_diagnosticsButton, SIGNAL(clicked()), this, SLOT(runDiagnostics()));
    buttons->addWidget(m_diagnosticsButton);
    m_cancelButton = new QPushButton(QString::fromLatin1("Cancel running command"), capabilities);
    m_cancelButton->setEnabled(false);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancelRun()));
    buttons->addWidget(m_cancelButton);
    buttons->addStretch();
    capLayout->addLayout(buttons);

    QValueList<int> sizes;
    sizes.append(260);
    sizes.append(280);
    splitter->setSizes(sizes);
    return splitter;
}

QWidget *LegacyMainWindow::buildActionsTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 10, 8);

    m_gateHint = new QLabel(page);
    m_gateHint->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_gateHint);

    QGroupBox *actions = new QGroupBox(
        QString::fromLatin1("Legacy-supported commands (gated by the cached capability lines)"),
        page);
    QGridLayout *grid = new QGridLayout(actions, actionSpecCount, 2, 8, 6);
    for (int i = 0; i < actionSpecCount; ++i) {
        const ActionSpec &spec = actionSpecs[i];
        QPushButton *button = new QPushButton(QString::fromLatin1(spec.label), actions);
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
    layout->addStretch();
    return page;
}

QWidget *LegacyMainWindow::buildLogTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    m_logView = new QTextEdit(page);
    m_logView->setReadOnly(true);
    m_logView->setTextFormat(Qt::LogText);
    m_logView->setWordWrap(QTextEdit::NoWrap);
    layout->addWidget(m_logView);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    QPushButton *save = new QPushButton(QString::fromLatin1("Save log..."), page);
    connect(save, SIGNAL(clicked()), this, SLOT(saveLog()));
    buttons->addWidget(save);
    QPushButton *clear = new QPushButton(QString::fromLatin1("Clear log"), page);
    connect(clear, SIGNAL(clicked()), this, SLOT(clearLog()));
    buttons->addWidget(clear);
    buttons->addStretch();
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
            "capability line says <tt>available</tt>.<br><br>"
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
}

void LegacyMainWindow::scopeChanged(int)
{
    if (hostScope()) {
        autoDetectHostTarget();
    }
    updateStatus();
    updateActionStates();
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
}

void LegacyMainWindow::targetEdited()
{
    if (m_updatingCombos) {
        return;
    }
    refreshRootCombo();
    updateStatus();
    updateActionStates();
}

void LegacyMainWindow::refreshRootCombo()
{
    if (m_updatingCombos) {
        return;
    }
    m_updatingCombos = true;
    const QString disk = m_diskCombo->currentText();
    const QString currentRoot = m_rootCombo->currentText();
    m_rootCombo->clear();

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
    m_updatingCombos = false;
}

void LegacyMainWindow::autoDetectHostTarget()
{
    if (selectionComplete()) {
        return;
    }
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
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

void LegacyMainWindow::runDiagnostics()
{
    if (!selectionComplete()) {
        QMessageBox::warning(this, QString::fromLatin1("Boot Bitch Legacy"),
                             QString::fromLatin1("Select a physical drive and a root component first."),
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

void LegacyMainWindow::startCommand(const QStringList &args,
                                    bool diagnostic, const QString &label)
{
    if (m_running) {
        return;
    }
    m_transcript = QString::null;
    m_pendingDiagnostic = diagnostic;
    m_pendingIdentity = identity();
    m_pendingLabel = label;
    m_running = true;
    m_cancelButton->setEnabled(true);
    updateActionStates();

    QString elevation;
    m_runner->resolveElevation(&elevation);
    m_lastHelperDescription = elevation;
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
    m_running = false;
    m_pendingDiagnostic = false;
    m_pendingLabel = QString::null;
    m_cancelButton->setEnabled(false);
    updateActionStates();
    updateStatus();

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
        }
        ++m_smokeStep;
        runSmokeStep();
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
    } else {
        int reported = 0;
        const std::vector<std::string> keys = capabilityKeys();
        for (std::size_t i = 0; i < keys.size(); ++i) {
            if (!m_model.state(keys[i]).empty()) {
                ++reported;
            }
        }
        const bool ok = m_smokeDiagnoseOk && reported == static_cast<int>(keys.size());
        const QString summary = QString::fromLatin1(
            "host-diagnose: %1 capability lines parsed; host-validate: %2")
            .arg(reported)
            .arg(ok ? QString::fromLatin1("ok") : QString::fromLatin1("incomplete"));
        m_smokeMode = false;
        emit smokeFinished(ok, summary);
    }
}

void LegacyMainWindow::startSmokeTest()
{
    m_smokeMode = true;
    m_smokeStep = 0;
    m_smokeDiagnoseOk = false;
    QTimer::singleShot(0, this, SLOT(runSmokeStep()));
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
    m_capabilityList->clear();
    const std::vector<std::string> keys = capabilityKeys();
    for (std::size_t i = 0; i < keys.size(); ++i) {
        const std::string &key = keys[i];
        const std::string state = m_model.state(key);
        QString reason;
        if (state.empty()) {
            reason = QString::fromLatin1("No 'Repair tool %1:' line cached; fail closed.")
                         .arg(fromStd(key));
        } else if (state == "available") {
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
        item->setEnabled(state == "available");
    }
}

void LegacyMainWindow::updateFactView(const ParsedTranscript &parsed)
{
    m_factList->clear();
    for (std::size_t i = 0; i < parsed.targetFacts.size(); ++i) {
        const std::string &line = parsed.targetFacts[i];
        const std::string::size_type colon = line.find(": ");
        if (colon == std::string::npos) {
            continue;
        }
        new QListViewItem(m_factList, fromStd(line.substr(0, colon + 1)),
                          fromStd(line.substr(colon + 2)));
    }
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

void LegacyMainWindow::updateActionStates()
{
    const bool complete = selectionComplete();
    const bool idle = !m_running;
    const QString id = identity();

    for (int i = 0; i < actionSpecCount; ++i) {
        const ActionSpec &spec = actionSpecs[i];
        QPushButton *button = m_actionButtons[QString::fromLatin1(spec.stage)];
        if (!button) {
            continue;
        }
        bool enabled = complete && idle;
        QString reason;
        if (!complete) {
            reason = QString::fromLatin1("Select a physical drive and a root component first.");
        } else if (!idle) {
            reason = QString::fromLatin1("A helper command is already running.");
        } else {
            std::string capabilityReason;
            enabled = m_model.isAvailable(spec.capability, toStd(id), &capabilityReason);
            reason = fromStd(capabilityReason);
        }
        button->setEnabled(enabled);
        QToolTip::add(button, reason);
    }

    const bool diagnosticsEnabled = complete && idle;
    m_diagnosticsButton->setEnabled(diagnosticsEnabled);
    QToolTip::add(m_diagnosticsButton,
                  diagnosticsEnabled
                      ? QString::fromLatin1("Run the read-only diagnostics for the selected scope; this unlocks the gated actions.")
                      : QString::fromLatin1("Select a target and wait for any running command first."));
    m_scanButton->setEnabled(idle);

    if (!complete) {
        m_gateHint->setText(QString::fromLatin1(
            "Select a disk and a root component, then run diagnostics (read-only). "
            "Actions are unlocked only by the helper's cached capability lines."));
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

void LegacyMainWindow::updateStatus()
{
    QString scope = hostScope()
        ? QString::fromLatin1("running host")
        : QString::fromLatin1("offline target");
    QString freshness;
    const QString id = identity();
    if (m_model.hasDiagnostics(toStd(id))) {
        freshness = m_model.diagnosticsStale()
            ? QString::fromLatin1("diagnostics stale")
            : QString::fromLatin1("diagnostics cached");
    } else {
        freshness = QString::fromLatin1("no diagnostics");
    }
    statusBar()->message(QString::fromLatin1("%1 | disk: %2 | root: %3 | %4")
                             .arg(scope)
                             .arg(selectedDisk().isEmpty() ? QString::fromLatin1("(none)") : selectedDisk())
                             .arg(selectedRoot().isEmpty() ? QString::fromLatin1("(none)") : selectedRoot())
                             .arg(freshness));
}

void LegacyMainWindow::appendLog(const QString &line)
{
    if (line.isNull()) {
        m_logView->append(QString::fromLatin1(""));
    } else {
        m_logView->append(line);
    }
    ensureLogFile();
    if (!m_logPath.isEmpty()) {
        appendToLogFile(line.isNull() ? QString::fromLatin1("") : line);
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
    stream << m_logView->text();
    file.close();
}

void LegacyMainWindow::clearLog()
{
    m_logView->setText(QString::null);
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
