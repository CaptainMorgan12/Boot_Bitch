// Shared fixtures and helpers for the boot-repair-ui-tests read-only
// regression suite.  These were moved out of MainWindowUiTest.cpp so the
// 895 KB suite can compile as several parallel translation units; every
// helper stays header-defined (inline) because the QtTest QVERIFY/QCOMPARE
// macros return from the calling test function.
#ifndef BOOT_REPAIR_UI_TEST_HELPERS_H
#define BOOT_REPAIR_UI_TEST_HELPERS_H

// MainWindow.h pulls in QFileDialog (through <filesystem>/<sstream>); include
// it before the private-access override so the standard headers are not
// rewritten with "private" redefined.
#include <QFileDialog>

#define private public
#include "MainWindow.h"
#undef private

#include <QApplication>
#include <QAbstractButton>
#include <QClipboard>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QGuiApplication>
#include <QInputDialog>
#include <QLayout>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QGroupBox>
#include <QHeaderView>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QMetaType>
#include <QPalette>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleOptionViewItem>
#include <QStyleHints>
#include <QTabWidget>
#include <QTabBar>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextFragment>
#include <QTextLayout>
#include <QTextStream>
#include <QTimer>
#include <QTreeWidget>
#include <QToolButton>
#include <QUrl>
#include <QtTest>

#ifdef BOOT_REPAIR_HAVE_DBUS
#include <QDBusVariant>
#endif

#include <algorithm>
#include <limits>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif


namespace UiTestDetail {

inline QPushButton *buttonWithText(MainWindow &window, const QString &text)
{
    for (QPushButton *button : window.findChildren<QPushButton *>()) {
        if (button->text() == text) {
            return button;
        }
    }
    return nullptr;
}

inline void prepareRepairScope(MainWindow &window, bool host = false)
{
    DeviceNode component;
    component.path = host ? QStringLiteral("/dev/test-host-root") : QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    component.protectedDevice = host;
    DeviceNode disk;
    disk.path = host ? QStringLiteral("/dev/test-host") : QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.protectedDevice = host;
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_hostMaintenanceMode = host;
    if (host) {
        window.m_hostPrimaryPath = disk.path;
        window.m_hostPrimaryComponentPath = component.path;
    } else {
        window.m_previewTargetPath = disk.path;
        window.m_previewTargetComponentPath = component.path;
        window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();
    }
}

// Enters the explicit running-host maintenance scope the way the real UI does.
// The Diagnostics scope is derived from the Systems state, so setting Host
// Maintenance active is what switches diagnostics to Running Host. Tests that
// exercise host diagnostics must establish this state first; the production
// transitions are exercised separately through the real
// selectHostForMaintenance()/exitHostMaintenanceMode() handlers.
inline void enterHostDiagnosticScope(MainWindow &window)
{
    window.m_hostMaintenanceMode = true;
    window.updateTargetLabels();
}

inline QTreeWidgetItem *repairItem(MainWindow &window, const QString &key)
{
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        auto *item = window.m_repairToolTree->topLevelItem(row);
        if (item->data(0, Qt::UserRole).toString() == key) {
            return item;
        }
    }
    return nullptr;
}

// Add and select a top-level drive row so the scope-confirmation handlers can
// read it exactly like a user selection in the device tree.
inline QTreeWidgetItem *selectTopLevelDisk(MainWindow &window, const QString &diskPath)
{
    window.m_deviceTree->clearSelection();
    auto *item = new QTreeWidgetItem(window.m_deviceTree);
    item->setData(0, Qt::UserRole, diskPath);
    window.m_deviceTree->setCurrentItem(item);
    item->setSelected(true);
    return item;
}

inline QString capabilityEvidence(bool arch, bool dkms)
{
    return QStringLiteral("Distribution family: %1\n"
                          "Package manager backend: %2\n"
                          "Package manager backends: %2\n"
                          "Service manager: systemd\n"
                          "Initramfs backend: %3\n"
                          "Bootloader backend: grub\n"
                          "Repair tool validate: available\n"
                          "Repair tool dpkg: %4\n"
                          "Repair tool fixbroken: available\n"
                          "Repair tool aptupdate: %5\n"
                          "Repair tool upgrade: available\n"
                          "Repair tool dkms: %6\n"
                          "Repair tool display: unavailable|No display manager installed\n"
                          "Repair tool initramfs: available\n"
                          "Repair tool efi: available\n"
                          "Repair tool grub: available\n"
                          "Repair tool bootstack: available\n")
        .arg(arch ? QStringLiteral("arch") : QStringLiteral("debian"),
             arch ? QStringLiteral("pacman") : QStringLiteral("apt/dpkg"),
             arch ? QStringLiteral("mkinitcpio") : QStringLiteral("initramfs-tools"),
             arch ? QStringLiteral("unavailable|Arch has no dpkg database") : QStringLiteral("available"),
             arch ? QStringLiteral("unavailable|Arch refuses metadata-only transactions") : QStringLiteral("available"),
             dkms ? QStringLiteral("available") : QStringLiteral("unavailable|DKMS is not installed"));
}

// The same capability evidence with the display-manager tool available so the
// scoped-invalidation tests can dispatch the display repair.
inline QString capabilityEvidenceWithDisplay(bool arch, bool dkms)
{
    QString evidence = capabilityEvidence(arch, dkms);
    evidence.replace(QStringLiteral("Repair tool display: unavailable|No display manager installed"),
                     QStringLiteral("Repair tool display: available"));
    return evidence;
}

inline void cacheRepairEvidence(MainWindow &window, const QString &evidence)
{
    // Injected evidence is fresh by definition: clear the scoped invalidation
    // markers so a test that re-caches does not keep a previous repair's
    // stale bookkeeping.
    window.m_targetDiagnosticsStaleSections.clear();
    window.m_hostDiagnosticsStaleSections.clear();
    auto &cache = window.m_hostMaintenanceMode ? window.m_hostDiagnosticCache : window.m_targetDiagnosticCache;
    cache.clear();
    // Mirror the production split: the `Repair tool <key>` gating contract is
    // cached under the dedicated capabilities entry, which the capability gate
    // reads exclusively.
    cache.insert(QStringLiteral("capabilities"), evidence);
    for (const QString &key : {QStringLiteral("report"), QStringLiteral("environment"),
                              QStringLiteral("kernel"), QStringLiteral("grub"), QStringLiteral("uki"),
                              QStringLiteral("display"), QStringLiteral("boot")}) {
        cache.insert(key, QStringLiteral("Diagnostic: %1\n%2").arg(key, evidence));
    }
}

// Configure a target-scope window for a file-system-only Full Repair plan with
// the filesystem capability available and every other stage unchecked.
inline void prepareFilesystemPlan(MainWindow &window)
{
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);
    // Apply the scope's availability to the widgets first: the presentation
    // pass restores the saved preferences, so the explicit setChecked calls
    // below emit toggled and record the new preferences.
    window.updateFullRepairSummary();

    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate, window.m_fullRepairUpgrade,
                              window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
                              window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                              window.m_fullRepairGrub}) {
        if (toggle) {
            toggle->setChecked(false);
        }
    }
    window.m_fullRepairFilesystem->setChecked(true);
    window.updateFullRepairSummary();
}

// Application-log refreshes are coalesced through a single-shot timer, so
// tests that assert on the rendered view must flush the pending refresh.
inline void flushLogRefresh(MainWindow &window)
{
    if (window.m_logRefreshTimer) {
        window.m_logRefreshTimer->stop();
    }
    window.refreshLogView();
}

// Decodes the ARG records of one captured privileged-session request. The
// capture is written by startFakePrivilegedSession().
inline QStringList capturedHelperArguments(const QString &capturePath)
{
    QFile file(capturePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringList();
    }
    const QStringList lines = QString::fromUtf8(file.readAll())
        .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    QStringList arguments;
    for (const QString &line : lines) {
        const QStringList fields = line.split(QLatin1Char('\t'));
        if (fields.size() >= 3 && fields.at(0) == QStringLiteral("ARG")) {
            arguments.append(QString::fromUtf8(QByteArray::fromBase64(fields.at(2).toLatin1())));
        }
    }
    return arguments;
}

// Starts a fake privileged-session process for the BOOT_REPAIR_UI_TEST build.
// It records the six protocol records of one request (BEGIN, four ARG, END)
// into capturePath, then emits a canned response:
//   success   - one OUT line followed by DONE with exit code 0
//   aptfail   - a failed-fetch OUT line followed by DONE with exit code 100
//   truncated - one OUT line and an exit with no DONE record at all
//   prompt    - a delayed interactive-prompt abort with the helper's
//               non-interactive hint, followed by DONE with exit code 1
//   aptupgrade - the helper's distribution-disabled apt-upgrade mapping line
//               and a successful full-upgrade retry, followed by DONE 0
//   shellpromptsecret - a passphrase prompt; reads one ANSWER record
//   promptburst - two prompt records, the second arriving while the first
//                 dialog is open; reads two ANSWER records
//   promptbudget - nine sequential prompt records, one ANSWER read each
//   promptafterdone - DONE followed by a stray PROMPT; never expects an answer
//   promptslow - a delayed prompt and a short post-answer delay before DONE
//   bigout - one ~2.3 MiB OUT payload followed by DONE
// The process is parented to the window so MainWindow owns its lifetime.
inline QProcess *startFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                     const QString &mode)
{
    auto *session = new QProcess(&window);
    const QString script = QStringLiteral(
        "capture=\"%1\"\n"
        "head -n 6 > \"$capture\"\n"
        "id=$(head -n 1 \"$capture\" | cut -f2)\n"
        "case \"%2\" in\n"
        "  success) printf 'OUT\\t%s\\tmock host command output\\n' \"$id\"; printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  aptfail) printf 'OUT\\t%s\\tTemporary failure resolving archive.example\\n' \"$id\"; printf 'DONE\\t%s\\t100\\n' \"$id\"; exit 100 ;;\n"
        "  truncated) printf 'OUT\\t%s\\tpartial helper output\\n' \"$id\"; exit 7 ;;\n"
        "  prompt) sleep 0.3; printf 'OUT\\t%s\\tIs this ok [y/N]: \\n' \"$id\"; "
        "printf 'OUT\\t%s\\tBoot Bitch: the command asked an interactive question; re-run it with the non-interactive flag, for example dnf update -y, apt-get -y upgrade or pacman --noconfirm -Syu.\\n' \"$id\"; "
        "printf 'DONE\\t%s\\t1\\n' \"$id\"; exit 1 ;;\n"
        "  aptupgrade) printf 'OUT\\t%s\\t[12:00:00] apt upgrade is disabled by this distribution; running '\\''apt full-upgrade'\\'' instead\\n' \"$id\"; "
        "printf 'OUT\\t%s\\tmock full-upgrade completed\\n' \"$id\"; printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  shellprompt) promptb64=$(printf 'Continue? [Y/n] ' | base64 | tr -d '\\n'); "
        "printf 'OUT\\t%s\\tContinue? [Y/n] \\n' \"$id\"; "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  shellpromptsecret) promptb64=$(printf 'Please enter the LUKS passphrase: ' | base64 | tr -d '\\n'); "
        "printf 'OUT\\t%s\\tPlease enter the LUKS passphrase: \\n' \"$id\"; "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  promptburst) promptb64=$(printf 'Continue? [y/N] ' | base64 | tr -d '\\n'); "
        "printf 'OUT\\t%s\\tContinue? [y/N] \\n' \"$id\"; "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "sleep 0.5; printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  promptbudget) promptb64=$(printf 'Continue? [y/N] ' | base64 | tr -d '\\n'); "
        "i=0; while [ \"$i\" -lt 9 ]; do "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "i=$((i+1)); done; "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  promptoversize) big=$(head -c 900 /dev/zero | tr '\\0' 'x'); "
        "promptb64=$(printf '%s' \"$big\" | base64 | tr -d '\\n'); "
        "printf 'OUT\\t%s\\tContinue? [y/N] \\n' \"$id\"; "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  promptafterdone) promptb64=$(printf 'Continue? [y/N] ' | base64 | tr -d '\\n'); "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer || true; "
        "[ -n \"$answer\" ] && printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "exit 0 ;;\n"
        "  promptslow) promptb64=$(printf 'Continue? [y/N] ' | base64 | tr -d '\\n'); "
        "sleep 0.25; printf 'OUT\\t%s\\tContinue? [y/N] \\n' \"$id\"; "
        "printf 'PROMPT\\t%s\\t%s\\n' \"$id\" \"$promptb64\"; "
        "IFS= read -r answer; printf '%s\\n' \"$answer\" >> \"$capture\"; "
        "sleep 0.15; printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  bigout) big=$(head -c 2300000 /dev/zero | tr '\\0' 'x'); "
        "printf 'OUT\\t%s\\t%s\\n' \"$id\" \"$big\"; "
        "printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "esac\n"
        "exit 9\n")
        .arg(capturePath, mode);
    session->start(QStringLiteral("/bin/sh"), {QStringLiteral("-c"), script});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session for multi-device file system repairs. The
// read-only inspection reports issues on two unmounted ext4 devices; every
// fs-repair request is answered for the device named in the request. The
// inspection response is delayed by inspectDelay (seconds, "0" for none) so a
// test can act while the flow is still in flight, and a non-zero repairExit
// makes the first repair request fail and end the session like a real helper
// failure. All requests are recorded so a blocked second attempt can be
// distinguished from an accepted one.
inline QProcess *startMultiDeviceFilesystemFakePrivilegedSession(MainWindow &window,
                                                          const QString &capturePath,
                                                          const QString &inspectDelay = QStringLiteral("0"),
                                                          int repairExit = 0)
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
inspect_delay="$2"
repair_exit="$3"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  case "$args" in
    *fs-inspect*)
      [ "$inspect_delay" = "0" ] || sleep "$inspect_delay"
      printf 'OUT\t%s\tFile system check /dev/test-root: ext4 uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=e2fsck result=issues\n' "$id"
      printf 'OUT\t%s\tFile system check detail /dev/test-root: tool=e2fsck output=Filesystem is not clean; errors found\n' "$id"
      printf 'OUT\t%s\tFile system check /dev/test-data: ext4 uuid=66666666-7777-8888-9999-000000000000 mount=unmounted tool=e2fsck result=issues\n' "$id"
      printf 'OUT\t%s\tFile system check detail /dev/test-data: tool=e2fsck output=Filesystem is not clean; errors found\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *fs-repair*)
      case "$args" in
        */dev/test-data*) dev=/dev/test-data ;;
        *) dev=/dev/test-root ;;
      esac
      printf 'OUT\t%s\tFile system repair %s: ext4 uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=e2fsck mode=repair result=clean\n' "$id" "$dev"
      printf 'OUT\t%s\tRepair change status filesystem: changed\n' "$id"
      printf 'DONE\t%s\t%s\n' "$id" "$repair_exit"
      [ "$repair_exit" = "0" ] || exit "$repair_exit"
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-multi-fs-session"),
                    capturePath, inspectDelay, QString::number(repairExit)});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session that answers every request with the
// helper's stable evidence-driven invalidation line. Each plan entry is
// "<helper-stage>=<state>"; a stage missing from the plan gets no status line
// (which the GUI must treat as changed/fail safe). Stage names are mapped to
// capability keys exactly like the helper does. A non-zero exitCode makes the
// session report DONE with that code and stop, like a failed helper run.
inline QProcess *startChangeStatusFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                                 const QStringList &plan, int exitCode = 0,
                                                 const QString &failedStage = QString(),
                                                 const QString &failureDetail = QString(),
                                                 const QString &earlyError = QString())
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="@CAPTURE@"
plan="@PLAN@"
failed_stage="@FAILED_STAGE@"
failure_detail="@FAILURE_DETAIL@"
early_error="@EARLY_ERROR@"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  printf 'OUT\t%s\tREPAIR_OUTPUT captured\n' "$id"
  if [ -n "$early_error" ]; then
    printf 'OUT\t%s\t%s\n' "$id" "$early_error"
  fi
  while IFS= read -r entry; do
    [ -n "$entry" ] || continue
    stage="${entry%%=*}"
    state="${entry#*=}"
    case " $args " in
      *" $stage "*) ;;
      *) continue ;;
    esac
    key="$stage"
    case "$stage" in
      dpkg-configure) key=dpkg ;;
      fix-broken) key=fixbroken ;;
      apt-update) key=aptupdate ;;
      apt-upgrade) key=upgrade ;;
      display-manager) key=display ;;
      boot-stack) key=bootstack ;;
      fs-repair|host-fs-repair) key=filesystem ;;
    esac
    printf 'OUT\t%s\tRepair change status %s: %s\n' "$id" "$key" "$state"
  done <<< "$plan"
  if [ -n "$failed_stage" ]; then
    printf 'OUT\t%s\t[00:00:00] ERROR: stage %s failed: %s\n' "$id" "'$failed_stage'" "$failure_detail"
  fi
  printf 'DONE\t%s\t@EXIT@\n' "$id"
  [ "@EXIT@" = "0" ] || exit @EXIT@
done
)SCRIPT");
    script.replace(QStringLiteral("@CAPTURE@"), capturePath);
    // One entry per line so a proven-unchanged reason may contain spaces.
    script.replace(QStringLiteral("@PLAN@"), plan.join(QLatin1Char('\n')));
    script.replace(QStringLiteral("@EXIT@"), QString::number(exitCode));
    script.replace(QStringLiteral("@FAILED_STAGE@"), failedStage);
    script.replace(QStringLiteral("@FAILURE_DETAIL@"), failureDetail);
    script.replace(QStringLiteral("@EARLY_ERROR@"), earlyError);
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-change-status-session")});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// The privileged repair path shows a modal progress dialog whose Close button
// only becomes usable once the helper's DONE record has been consumed. The
// offscreen tests therefore dismiss the dialog as soon as it reports that the
// request completed, so a repair cycle can run without user interaction.
inline void closeRepairProgressDialogWhenDone(QObject *context)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()) {
                continue;
            }
            auto *buttons = dialog->findChild<QDialogButtonBox *>();
            if (!buttons) {
                continue;
            }
            QPushButton *close = buttons->button(QDialogButtonBox::Close);
            if (!close || !close->isEnabled()) {
                continue;
            }
            poll->stop();
            dialog->reject();
            poll->deleteLater();
            return;
        }
    });
    poll->start();
}

// The scheduler's coalescing delay is 1200 ms; tests that observe a real
// automatic run shorten it through MainWindow::m_evidenceRefreshDelayMs and
// wait long enough that a second (unwanted) run would have fired.
constexpr int kEvidenceRefreshTestDelayMs = 50;

// Route every test window's session files into a throwaway directory so real
// user logs under AppDataLocation are never touched.
class ScopedSessionLogDir
{
public:
    ScopedSessionLogDir()
        : m_previous(qgetenv("BOOT_REPAIR_LOG_DIR"))
    {
        qputenv("BOOT_REPAIR_LOG_DIR", m_dir.path().toUtf8());
    }
    ~ScopedSessionLogDir()
    {
        qputenv("BOOT_REPAIR_LOG_DIR", m_previous);
    }

    bool isValid() const
    {
        return m_dir.isValid();
    }

    QString path() const
    {
        return m_dir.path();
    }

private:
    QTemporaryDir m_dir;
    QByteArray m_previous;
};

inline void acceptNextMessageBox(QObject *context, QMessageBox::StandardButton button)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll, button] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box) {
                continue;
            }
            poll->stop();
            if (QAbstractButton *target = box->button(button)) {
                target->click();
            } else {
                box->done(button);
            }
            poll->deleteLater();
            return;
        }
    });
    poll->start();
}

// Dismisses a sequence of modal message boxes with the same standard button.
// Each visible box is clicked once; the visibility check keeps the first click
// from being counted twice while the box is still closing.
inline void acceptNextMessageBoxes(QObject *context, int count, QMessageBox::StandardButton button)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll, count, button]() mutable {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            if (QAbstractButton *target = box->button(button)) {
                target->click();
            } else {
                box->done(button);
            }
            if (--count <= 0) {
                poll->stop();
                poll->deleteLater();
            }
            return;
        }
    });
    poll->start();
}

} // namespace UiTestDetail

#endif // BOOT_REPAIR_UI_TEST_HELPERS_H
