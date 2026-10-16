// Part of the boot-repair-ui-tests read-only regression suite: the running-host
// UEFI firmware boot-entry manager (Boot entries tab). The class declaration
// lives in MainWindowUiTest.h and the shared fixtures/helpers in
// UiTestHelpers.h; this file holds the efi-list parser unit test and the page's
// gating/reorder/save/rename UI tests so the suite compiles as parallel
// translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>

using namespace UiTestDetail;

// Test seam compiled into boot-repair-ui-tests only (see MainWindow.cpp).
BootEntriesInventory parseBootEntriesOutputForTest(const QString &output);
GrubMenuInventory parseGrubMenuOutputForTest(const QString &output);
ExtlinuxInventory parseExtlinuxOutputForTest(const QString &output);
GrubLegacyInventory parseGrubLegacyOutputForTest(const QString &output);

// A fixed efi-list transcript covering the protocol shapes the parser and the
// table must handle: an empty BootCurrent, a space-containing label, a
// device-path-only loader, a listed/unlisted mix and an empty drive model.
static QString bootEntriesFixture()
{
    return QStringLiteral(
        "BootCurrent: \n"
        "BootOrder: 0000,0001,0002\n"
        "Boot0000\tLinux boot\tdevice-path-only\thost\tuki\t11111111-2222-3333-4444-555555555555\tTest Drive\tlisted\n"
        "Boot0001\tWindows Boot Manager\t\\EFI\\Microsoft\\Boot\\bootmgfw.efi\tforeign\tloader\tunknown\t\tlisted\n"
        "Boot0002\tDiagnostics\tdevice-path-only\tunknown\tfallback\tunknown\t\tunlisted\n");
}

// Persistent fake privileged session for the boot-entries requests. It records
// every request and answers the efi-* commands with the helper's change-status
// protocol so the mutation flows run headless; the efi-reorder answer is the
// proven-unchanged shape, which keeps the reorder-only Save flow free of the
// success information box (the CSV argument is what the test asserts).
static QProcess *startBootEntriesFakeSession(MainWindow &window, const QString &capturePath)
{
    auto *session = new QProcess(&window);
    const QString script = QStringLiteral(R"SCRIPT(
capture="$1"
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
    *efi-reorder*)
      printf 'OUT\t%s\tRepair change status efi-reorder: unchanged\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *efi-delete*)
      printf 'OUT\t%s\tRepair change status efi-delete: changed\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *efi-rename*)
      printf 'OUT\t%s\tRepair change status efi-rename: changed\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *efi-list*)
      printf 'OUT\t%s\tBootCurrent: 0000\n' "$id"
      printf 'OUT\t%s\tBootOrder: 0000,0001\n' "$id"
      printf 'OUT\t%s\tBoot0000\tLinux boot\tdevice-path-only\thost\tuki\t11111111-2222-3333-4444-555555555555\tTest Drive\tlisted\n' "$id"
      printf 'OUT\t%s\tBoot0001\tWindows Boot Manager\t\\EFI\\Microsoft\\Boot\\bootmgfw.efi\tforeign\tloader\tunknown\t\tlisted\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *grub-list*)
      printf 'OUT\t%s\tGRUB config: /boot/grub/grub.cfg\n' "$id"
      printf 'OUT\t%s\tGRUB default: gnulinux-simple-abc\n' "$id"
      printf 'OUT\t%s\tUbuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n' "$id"
      printf 'OUT\t%s\tAdvanced options for Ubuntu\tubuntu\tgnulinux-advanced-abc\t0\t0\t0\tsubmenu\t\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-boot-entries-session"), capturePath});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Like startBootEntriesFakeSession(), but delays the efi-list answer by the
// given number of seconds so a test can switch scope while the request is
// still in flight (runPrivilegedRequest()'s wait loop processes input).
static QProcess *startBootEntriesFakeSessionWithDelay(MainWindow &window,
                                                      const QString &capturePath,
                                                      const QString &delaySeconds)
{
    auto *session = new QProcess(&window);
    const QString script = QStringLiteral(R"SCRIPT(
capture="$1"
delay="$2"
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
    *efi-list*)
      sleep "$delay"
      printf 'OUT\t%s\tBootCurrent: 0000\n' "$id"
      printf 'OUT\t%s\tBootOrder: 0000,0001\n' "$id"
      printf 'OUT\t%s\tBoot0000\tLinux boot\tdevice-path-only\thost\tuki\t11111111-2222-3333-4444-555555555555\tTest Drive\tlisted\n' "$id"
      printf 'OUT\t%s\tBoot0001\tWindows Boot Manager\t\\EFI\\Microsoft\\Boot\\bootmgfw.efi\tforeign\tloader\tunknown\t\tlisted\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-boot-entries-session-delayed"),
                    capturePath, delaySeconds});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Closes every privileged-request progress dialog as soon as its Close button
// becomes usable. Unlike closeRepairProgressDialogWhenDone() (one-shot), this
// keeps polling so a combined Save that runs several sequential helper requests
// (each showing its own progress dialog) completes headless.
static void closeEveryRepairProgressDialog(QObject *context)
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
            dialog->reject();
            return;
        }
    });
    poll->start();
}

// The efi-list protocol parser: empty output, header-only output, entries with
// a space-containing label and a device-path-only loader, a listed/unlisted
// mix, an empty BootCurrent and BootOrder preservation.
void MainWindowUiTest::bootEntriesParserParsesProtocol()
{
    // Empty output yields no header and no rows.
    {
        const BootEntriesInventory inventory = parseBootEntriesOutputForTest(QString());
        QVERIFY(inventory.bootCurrent.isEmpty());
        QVERIFY(inventory.bootOrder.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // Header-only output (no rows) still carries BootCurrent/BootOrder.
    {
        const BootEntriesInventory inventory = parseBootEntriesOutputForTest(
            QStringLiteral("BootCurrent: 0003\nBootOrder: 0003,0000\n"));
        QCOMPARE(inventory.bootCurrent, QStringLiteral("0003"));
        QCOMPARE(inventory.bootOrder, QStringLiteral("0003,0000"));
        QVERIFY(inventory.rows.isEmpty());
    }

    // A full transcript: empty BootCurrent, TAB-only field separation, a
    // space-containing label, a device-path-only loader and an unlisted row.
    const BootEntriesInventory inventory = parseBootEntriesOutputForTest(bootEntriesFixture());
    QVERIFY(inventory.bootCurrent.isEmpty());
    QCOMPARE(inventory.bootOrder, QStringLiteral("0000,0001,0002"));
    QCOMPARE(inventory.rows.size(), 3);

    QCOMPARE(inventory.rows.at(0).id, QStringLiteral("0000"));
    QCOMPARE(inventory.rows.at(0).label, QStringLiteral("Linux boot"));
    QCOMPARE(inventory.rows.at(0).loader, QStringLiteral("device-path-only"));
    QCOMPARE(inventory.rows.at(0).entryClass, QStringLiteral("host"));
    QCOMPARE(inventory.rows.at(0).role, QStringLiteral("uki"));
    QCOMPARE(inventory.rows.at(0).partuuid, QStringLiteral("11111111-2222-3333-4444-555555555555"));
    QCOMPARE(inventory.rows.at(0).deviceModel, QStringLiteral("Test Drive"));
    QVERIFY(inventory.rows.at(0).listed);

    QCOMPARE(inventory.rows.at(1).id, QStringLiteral("0001"));
    QCOMPARE(inventory.rows.at(1).label, QStringLiteral("Windows Boot Manager"));
    QVERIFY(inventory.rows.at(1).deviceModel.isEmpty());
    QVERIFY(inventory.rows.at(1).listed);

    QCOMPARE(inventory.rows.at(2).id, QStringLiteral("0002"));
    QVERIFY(!inventory.rows.at(2).listed);
    QCOMPARE(inventory.rows.at(2).role, QStringLiteral("fallback"));

    // The comma-joined BootOrder header is preserved verbatim.
    QCOMPARE(inventory.bootOrder, QStringLiteral("0000,0001,0002"));
}

// bootEntriesReady() must accept a selected repair target (target scope) and
// fail closed with a clear reason when neither scope is selected.
void MainWindowUiTest::bootEntriesReadyInTargetScope()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    // Nothing selected: fail closed with a reason naming both entry points.
    QString reason;
    QVERIFY2(!window.bootEntriesReady(&reason),
             "boot entries must fail closed when no scope is selected");
    QVERIFY2(reason.contains(QStringLiteral("Host Maintenance")),
             "the reason must point at Host Maintenance");
    QVERIFY2(reason.contains(QStringLiteral("Systems")),
             "the reason must point at selecting a repair target in Systems");

    window.updateBootEntriesControls();
    QVERIFY(!window.m_bootEntriesLoadButton->isEnabled());

    // A selected repair target makes the page ready in target scope.
    prepareRepairScope(window, /*host=*/false);
    window.updateBootEntriesControls();
    QVERIFY(window.bootEntriesReady(&reason));
    QVERIFY(window.m_bootEntriesLoadButton->isEnabled());
    QVERIFY(window.m_bootEntriesScopeLabel);
    QVERIFY2(window.m_bootEntriesScopeLabel->text().contains(QStringLiteral("Repair target")),
             "the scope label must name the repair target");
}

// Outside Host Maintenance the page must fail closed: every action disabled and
// a reason produced that points at the missing scope.
void MainWindowUiTest::bootEntriesPageGatedOutsideHostMaintenance()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    QVERIFY(window.m_bootEntriesLoadButton);
    QVERIFY(window.m_bootEntriesMoveUpButton);
    QVERIFY(window.m_bootEntriesMoveDownButton);
    QVERIFY(window.m_bootEntriesSaveOrderButton);
    QVERIFY(window.m_bootEntriesDeleteButton);

    QString reason;
    QVERIFY2(!window.bootEntriesReady(&reason),
             "boot entries must fail closed outside Host Maintenance");
    QVERIFY2(!reason.isEmpty(), "a refusal reason must be produced");
    QVERIFY2(reason.contains(QStringLiteral("Host Maintenance")),
             "the reason must point at Host Maintenance");

    window.updateBootEntriesControls();
    QVERIFY(!window.m_bootEntriesLoadButton->isEnabled());
    QVERIFY(!window.m_bootEntriesMoveUpButton->isEnabled());
    QVERIFY(!window.m_bootEntriesMoveDownButton->isEnabled());
    QVERIFY(!window.m_bootEntriesSaveOrderButton->isEnabled());
    QVERIFY(!window.m_bootEntriesDeleteButton->isEnabled());
    QVERIFY(window.m_bootEntriesStatusLabel);
    QVERIFY2(window.m_bootEntriesStatusLabel->text().contains(QStringLiteral("Host Maintenance")),
             "the page must show a clear Select Host Maintenance state");

    // Entering Host Maintenance enables the read-only Load action (the page is
    // still empty, so the mutation actions stay disabled).
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();
    QVERIFY(window.bootEntriesReady(&reason));
    QVERIFY(window.m_bootEntriesLoadButton->isEnabled());
    QVERIFY(!window.m_bootEntriesSaveOrderButton->isEnabled());
    QVERIFY(!window.m_bootEntriesDeleteButton->isEnabled());
}

// In target scope, deleting or renaming a `host`-class row (the running host's
// own firmware entry) is refused client-side, before any privileged request.
void MainWindowUiTest::bootEntriesHostClassRefusedClientSideInTargetScope()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/false);
    window.updateBootEntriesControls();

    window.m_bootEntriesInventory = window.parseBootEntriesOutput(bootEntriesFixture());
    window.m_bootEntriesLoaded = true;
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    // Row 0 (Boot0000) is class `host` in the fixture.
    window.m_bootEntriesTable->selectRow(0);
    acceptNextMessageBox(&window, QMessageBox::Ok);
    window.deleteSelectedBootEntry();
    QVERIFY2(!window.m_privilegedSession,
             "deleting a host-class entry in target scope must not issue a privileged request");

    // Rename path: change the Label cell, then drive renameSelectedBootEntry.
    QTableWidgetItem *labelItem = window.m_bootEntriesTable->item(0, 1);
    QVERIFY(labelItem);
    window.m_bootEntriesUpdating = true;
    labelItem->setText(QStringLiteral("Renamed"));
    window.m_bootEntriesUpdating = false;
    window.m_bootEntriesTable->setCurrentCell(0, 1);
    acceptNextMessageBox(&window, QMessageBox::Ok);
    window.renameSelectedBootEntry();
    QVERIFY2(!window.m_privilegedSession,
             "renaming a host-class entry in target scope must not issue a privileged request");
    // The unpersisted label edit is reverted.
    QCOMPARE(labelItem->text(), QStringLiteral("Linux boot"));
}

// The Restore / regenerate boot entries button fails closed outside a scope,
// fails closed when the cached `efi` capability is unavailable, and once a
// ready scope has fresh `efi` capability evidence it issues the guarded
// `efi`-stage repair with the correct scope identity (host-repair for Host
// Maintenance, repair for a repair target).
void MainWindowUiTest::bootEntriesRestoreButtonGatedAndIssuesEfiRepair()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // (a) Outside any scope the Restore action stays disabled.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        QVERIFY(window.m_bootEntriesRestoreButton);
        window.updateBootEntriesControls();
        QVERIFY(!window.m_bootEntriesRestoreButton->isEnabled());
    }

    // (a2) A ready scope whose cached diagnostics report the `efi` capability
    // as unavailable keeps the Restore action disabled and surfaces the
    // capability reason (not the generic scope reason) as its tooltip — the
    // BIOS/syslinux and BIOS-GRUB case.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        window.m_autoRefreshDiagnostics->setChecked(false);
        QString evidence = capabilityEvidence(false, true);
        evidence.replace(QStringLiteral("Repair tool efi: available"),
                         QStringLiteral("Repair tool efi: unavailable|syslinux-bios-no-efi"));
        cacheRepairEvidence(window, evidence);
        window.updateBootEntriesControls();
        QString readyReason;
        QVERIFY(window.bootEntriesReady(&readyReason));
        QVERIFY(!window.m_bootEntriesRestoreButton->isEnabled());
        QVERIFY2(window.m_bootEntriesRestoreButton->toolTip().contains(
                     QStringLiteral("syslinux-bios-no-efi")),
                 "the Restore tooltip must carry the efi capability reason when efi is unavailable");
    }

    // (b) Host scope with fresh `efi` capability evidence: the click issues
    // host-repair <disk> <root> efi with the running-host identity.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/true);
        window.m_autoRefreshDiagnostics->setChecked(false);
        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.updateBootEntriesControls();
        QVERIFY(window.m_bootEntriesRestoreButton->isEnabled());

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-restore-host.log"));
        QVERIFY2(startBootEntriesFakeSession(window, capturePath),
                 "the scripted privileged session must start");
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        window.m_bootEntriesRestoreButton->click();
        QTest::qWait(50);

        const QStringList args = capturedHelperArguments(capturePath);
        bool sawHostRepair = false;
        for (int i = 0; i + 3 < args.size(); ++i) {
            if (args.at(i) == QStringLiteral("host-repair")
                && args.at(i + 1) == QStringLiteral("/dev/test-host")
                && args.at(i + 2) == QStringLiteral("/dev/test-host-root")
                && args.at(i + 3) == QStringLiteral("efi")) {
                sawHostRepair = true;
            }
        }
        QVERIFY2(sawHostRepair,
                 "host-scope Restore must issue host-repair <disk> <root> efi with the running-host identity");
    }

    // (c) Repair-target scope with fresh `efi` capability evidence: the click
    // issues repair <disk> <root> efi with the selected target identity.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        window.m_autoRefreshDiagnostics->setChecked(false);
        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.updateBootEntriesControls();
        QVERIFY(window.m_bootEntriesRestoreButton->isEnabled());

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-restore-target.log"));
        QVERIFY2(startBootEntriesFakeSession(window, capturePath),
                 "the scripted privileged session must start");
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        window.m_bootEntriesRestoreButton->click();
        QTest::qWait(50);

        const QStringList args = capturedHelperArguments(capturePath);
        bool sawRepair = false;
        for (int i = 0; i + 3 < args.size(); ++i) {
            if (args.at(i) == QStringLiteral("repair")
                && args.at(i + 1) == QStringLiteral("/dev/test-system")
                && args.at(i + 2) == QStringLiteral("/dev/test-root")
                && args.at(i + 3) == QStringLiteral("efi")) {
                sawRepair = true;
            }
        }
        QVERIFY2(sawRepair,
                 "target-scope Restore must issue repair <disk> <root> efi with the repair-target identity");
    }
}

// restoreBootEntries() must refuse (warn + return) when the cached `efi`
// capability is unavailable, without issuing the doomed privileged helper call.
void MainWindowUiTest::bootEntriesRestoreRefusesWhenEfiUnavailable()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/false);
    window.m_autoRefreshDiagnostics->setChecked(false);
    QString evidence = capabilityEvidence(false, true);
    evidence.replace(QStringLiteral("Repair tool efi: available"),
                     QStringLiteral("Repair tool efi: unavailable|syslinux-bios-no-efi"));
    cacheRepairEvidence(window, evidence);
    window.updateBootEntriesControls();
    QVERIFY(!window.m_bootEntriesRestoreButton->isEnabled());

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-restore-refuse.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");
    acceptNextMessageBox(&window, QMessageBox::Ok);

    window.restoreBootEntries();
    QTest::qWait(50);

    const QStringList args = capturedHelperArguments(capturePath);
    QVERIFY2(!args.contains(QStringLiteral("repair")) && !args.contains(QStringLiteral("host-repair")),
             "restoreBootEntries must refuse before issuing any repair helper request when efi is unavailable");
    QVERIFY2(!window.m_bootEntriesRestoreButton->isEnabled(),
             "the Restore action stays disabled when efi is unavailable");
}

// Navigating to the Boot entries tab must NOT auto-reload the firmware entries
// (or the boot menu) just because the user opened the tab. Entries are
// populated by diagnostics / report (via the scheduled boot-entries refresh)
// and by the explicit Load Entries action; opening the tab alone keeps showing
// whatever was last loaded and never issues an efi-list/grub-list request of
// its own, even with an established administrator session in Host Maintenance.
void MainWindowUiTest::bootEntriesAutoReloadOnTabShow()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // Host Maintenance active with an established administrator session: the
    // old behavior issued an efi-list request on tab entry; it must not now.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/true);
        window.updateBootEntriesControls();

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-no-auto-reload.log"));
        QVERIFY2(startBootEntriesFakeSession(window, capturePath),
                 "the scripted privileged session must start");

        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.refreshCapabilities();

        window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_bootEntriesPage));
        QTest::qWait(300);

        const QStringList args = capturedHelperArguments(capturePath);
        QVERIFY2(!args.contains(QStringLiteral("efi-list")),
                 "opening the Boot entries tab must not auto-reload via efi-list");
        QVERIFY2(!args.contains(QStringLiteral("grub-list")),
                 "opening the Boot entries tab must not auto-reload via grub-list");
        QVERIFY(!window.m_bootEntriesLoaded);
    }
}

// Regression: entering the Boot entries tab while diagnostics are still
// generating used to skip the tab-show auto-reload (the privileged-request
// gate was held), and nothing re-triggered the load once diagnostics finished.
// The fix schedules a coalescing boot-entries refresh at diagnostics
// completion, so the entries appear without navigating away and back. This
// proves the wiring: after runDiagnosticSections() completes, the refresh timer
// is active and, once it fires, the read-only efi-list and grub-list requests
// are both issued.
void MainWindowUiTest::bootEntriesRefreshScheduledAfterDiagnosticsComplete()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();
    // Keep the automatic evidence regeneration out of the picture so this test
    // exercises exactly the diagnostics-completion hook.
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_bootEntriesRefreshDelayMs = kEvidenceRefreshTestDelayMs;

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-refresh-after-diagnostics.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");

    // Nothing is scheduled before diagnostics run.
    QVERIFY(!window.m_bootEntriesRefreshTimer || !window.m_bootEntriesRefreshTimer->isActive());

    // Complete a scoped diagnostic run; its completion point schedules the
    // boot-entries refresh.
    window.runDiagnosticSections({QStringLiteral("grub")});

    QVERIFY(window.m_bootEntriesRefreshTimer);
    QVERIFY2(window.m_bootEntriesRefreshTimer->isActive(),
             "completing diagnostics must schedule the boot-entries refresh");

    // Let the timer fire and the read-only reloads complete.
    QTest::qWait(300);

    const QStringList args = capturedHelperArguments(capturePath);
    QVERIFY2(args.contains(QStringLiteral("efi-list")),
             "the post-diagnostics refresh must issue the read-only efi-list request");
    QVERIFY2(args.contains(QStringLiteral("grub-list")),
             "the post-diagnostics refresh must issue the read-only grub-list request");
    QVERIFY(window.m_bootEntriesLoaded);
}

// Regression: the firmware boot entries and boot menu must be cached per scope
// so a host→target→host round-trip re-renders the previously loaded entries
// from cache instead of clearing the table. When the host diagnostics are still
// cached/fresh (so no automatic re-diagnosis re-fetches the entries), returning
// to Host Maintenance must restore the cached inventory, not show a blank page.
void MainWindowUiTest::bootEntriesRenderFromCacheAcrossScopeRoundTrip()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_privilegedSessionReady = true;

    // Resolve the protected running host and park a committed repair target,
    // then return to ordinary target mode so the round-trip can start there.
    prepareRepairScope(window, /*host=*/false);
    prepareRepairScope(window, /*host=*/true);
    window.m_hostMaintenanceMode = false;

    // Enter Host Maintenance for the first time (target → host).
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath.isEmpty());

    // Populate the running-host firmware boot entries exactly as a successful
    // efi-list load would (no privileged helper needed), then render.
    window.m_bootEntriesInventory = window.parseBootEntriesOutput(bootEntriesFixture());
    window.m_bootEntriesLoaded = true;
    window.m_bootEntriesSavedOrder = QStringList{QStringLiteral("0000"),
                                                 QStringLiteral("0001"),
                                                 QStringLiteral("0002")};
    window.populateBootEntriesTable();
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 3);

    // Leave Host Maintenance (host → target): the parked target is restored and
    // the host's inventory is parked in the per-scope cache.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    QVERIFY(!window.m_previewTargetPath.isEmpty());

    // Re-enter Host Maintenance (target → host): the host diagnostics are still
    // cached/fresh (auto-regeneration is disabled here), so the boot entries
    // must render from the per-scope cache rather than showing a blank table.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath.isEmpty());

    QVERIFY(window.m_bootEntriesLoaded);
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 3);
    QCOMPARE(window.m_bootEntriesTable->item(0, 0)->text(), QStringLiteral("Boot0000"));
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Linux boot"));
    QCOMPARE(window.m_bootEntriesTable->item(1, 1)->text(), QStringLiteral("Windows Boot Manager"));
}

// Regression: on the very first visit to a scope the firmware boot entries AND
// the boot menu must both auto-load. The first scope visit never calls
// updateBootEntriesControls() (the repair-target selection path skips it), so
// the scope key is still empty when the post-diagnostics refresh runs. The old
// refreshBootEntries() populated the table and then its own trailing
// updateBootEntriesControls() treated the just-loaded rows as a stale-scope
// change and reset them to empty — while the boot menu survived only because
// refreshGrubMenu() ran after that reset. This drives the exact bug: a target
// is selected without touching the boot-entries controls, then the scheduled
// refresh fires and both read-only loads must land and stay rendered.
void MainWindowUiTest::bootEntriesFirstScopeVisitLoadsFirmwareAndMenu()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    // Select a repair target without updating the boot-entries controls first,
    // mirroring setPreviewTarget() (which never calls updateBootEntriesControls()).
    prepareRepairScope(window, /*host=*/false);

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-first-visit.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");

    // Reproduce the bug's precondition: the scope key is not yet established.
    QVERIFY(window.m_bootEntriesScopeKey.isEmpty());

    // The post-diagnostics boot-entries refresh fires on the first scope visit.
    window.runScheduledBootEntriesRefresh();

    // The firmware entries must be loaded and rendered, not reset.
    QVERIFY(window.m_bootEntriesLoaded);
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 2);
    QCOMPARE(window.m_bootEntriesTable->item(0, 0)->text(), QStringLiteral("✓ Boot0000"));
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Linux boot"));
    QCOMPARE(window.m_bootEntriesTable->item(1, 1)->text(), QStringLiteral("Windows Boot Manager"));

    // The boot menu must be loaded too.
    QVERIFY(window.m_grubMenuLoaded);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(0), QStringLiteral("✓ Ubuntu"));

    // The scope key is now established for the target, so a later scope switch
    // parks/restores correctly instead of treating the entries as stale.
    QVERIFY(window.m_bootEntriesScopeKey.startsWith(QStringLiteral("target:")));

    const QStringList args = capturedHelperArguments(capturePath);
    QVERIFY2(args.contains(QStringLiteral("efi-list")),
             "the first-scope-visit refresh must issue the read-only efi-list request");
    QVERIFY2(args.contains(QStringLiteral("grub-list")),
             "the first-scope-visit refresh must issue the read-only grub-list request");
}

// Regression: a host→target→host round-trip plus exit-host, with both the
// firmware entries and the boot menu populated, must not crash and must
// re-render the previously loaded inventories from the per-scope cache. This
// exercises the park/restore in updateBootEntriesControls() with both view
// models populated (the earlier round-trip test only populated the entries
// table). The entries and menu are loaded through the real read-only paths so
// the scope key is established the same way the production first-visit flow
// does after the Issue-1 fix.
void MainWindowUiTest::bootEntriesRoundTripWithPopulatedEntriesDoesNotCrash()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-round-trip.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");

    // Resolve the protected running host and park a committed repair target,
    // then return to ordinary target mode so the round-trip can start there.
    prepareRepairScope(window, /*host=*/false);
    prepareRepairScope(window, /*host=*/true);
    window.m_hostMaintenanceMode = false;

    // Enter Host Maintenance and populate both views through the real read-only
    // load paths.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    window.refreshBootEntries();
    window.refreshGrubMenu();
    QVERIFY(window.m_bootEntriesLoaded);
    QVERIFY(window.m_grubMenuLoaded);
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);

    // Exit Host Maintenance (host → target): the host's inventories are parked.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    QVERIFY(!window.m_previewTargetPath.isEmpty());

    // Re-enter Host Maintenance (target → host): restore from the per-scope
    // cache instead of a blank table, and re-render both views.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_bootEntriesLoaded);
    QVERIFY(window.m_grubMenuLoaded);
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 2);
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Linux boot"));
    QCOMPARE(window.m_bootEntriesTable->item(1, 1)->text(), QStringLiteral("Windows Boot Manager"));
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);

    // A second exit + re-enter exercises the park/restore again and must not
    // crash or clear the restored rows.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_bootEntriesLoaded);
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 2);
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Linux boot"));

    // Final exit-host leaves ordinary repair-target mode.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    QVERIFY(!window.m_previewTargetPath.isEmpty());
}

// Regression: a boot-entries load whose privileged efi-list request is still in
// flight when the user switches Host↔Target must discard the outgoing scope's
// result instead of writing it into the incoming scope's live members (and
// parking it under the incoming key on the next scope switch, which permanently
// contaminates the per-scope cache). runPrivilegedRequest()'s wait loop runs a
// nested event loop that processes input, so the scope switch below fires while
// the request is still waiting for its (delayed) answer.
void MainWindowUiTest::bootEntriesMidRequestScopeSwitchDiscardsStaleResult()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    // Resolve the protected running host and park a committed repair target,
    // then return to ordinary target mode so the round-trip can start there.
    prepareRepairScope(window, /*host=*/false);
    prepareRepairScope(window, /*host=*/true);
    window.m_hostMaintenanceMode = false;

    // Enter Host Maintenance for the first time (target → host); this also
    // establishes the host boot-entries scope key.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QCOMPARE(window.m_bootEntriesScopeKey, QStringLiteral("host:/dev/test-host"));

    // A fake privileged session whose efi-list answer is delayed long enough
    // for the scope switch below to happen while the request is in flight.
    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-mid-switch.log"));
    QVERIFY2(startBootEntriesFakeSessionWithDelay(window, capturePath, QStringLiteral("0.8")),
             "the scripted privileged session must start");

    // Switch to the repair target while the efi-list request is in flight.
    QTimer::singleShot(200, &window, [&window] {
        window.exitHostMaintenanceMode();
    });

    window.refreshBootEntries();

    // The scope changed mid-request: the incoming (target) scope must not be
    // populated with the outgoing (host) scope's rows.
    QVERIFY(!window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath == QStringLiteral("/dev/test-system"));
    QVERIFY2(!window.m_bootEntriesLoaded,
             "the incoming target scope must not inherit the outgoing host load");
    QCOMPARE(window.m_bootEntriesTable->rowCount(), 0);

    // Round-trip once more (target → host): if the stale host rows had landed
    // in the target scope's live members, this would park them under the
    // target cache key, permanently contaminating it.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY2(!window.m_bootEntriesCache.contains(QStringLiteral("target:/dev/test-system"))
             || window.m_bootEntriesCache.value(QStringLiteral("target:/dev/test-system")).entries.rows.isEmpty(),
             "the target cache key must not be contaminated with the outgoing host rows");
}

// Delete Selected stages a deletion (strikethrough + pending set) without any
// privileged helper write; pressing it again on the same row cancels the mark.
void MainWindowUiTest::bootEntriesDeleteStagesPendingWithoutHelperWrite()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();

    window.m_bootEntriesInventory = window.parseBootEntriesOutput(bootEntriesFixture());
    window.m_bootEntriesLoaded = true;
    window.m_bootEntriesSavedOrder = QStringList{QStringLiteral("0000"),
                                                 QStringLiteral("0001"),
                                                 QStringLiteral("0002")};
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    // Row 1 is Boot0001 (foreign loader): deletable in host scope.
    window.m_bootEntriesTable->selectRow(1);
    window.deleteSelectedBootEntry();

    QVERIFY(window.m_bootEntriesPendingDelete.contains(QStringLiteral("0001")));
    QVERIFY2(!window.m_privilegedSession,
             "marking an entry for deletion must not issue any privileged helper write");
    QVERIFY(window.m_bootEntriesTable->item(1, 1)->font().strikeOut());

    // Re-selecting the row and pressing Delete Selected again cancels the mark.
    window.deleteSelectedBootEntry();
    QVERIFY(!window.m_bootEntriesPendingDelete.contains(QStringLiteral("0001")));
    QVERIFY(!window.m_bootEntriesTable->item(1, 1)->font().strikeOut());
    QVERIFY2(!window.m_privilegedSession,
             "un-marking an entry must not issue any privileged helper write");
}

// An inline Label edit stages a pending rename (italic + pending map) without
// any privileged helper write; the persisted label is left untouched until Save.
void MainWindowUiTest::bootEntriesRenameStagesPendingWithoutHelperWrite()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();

    window.m_bootEntriesInventory = window.parseBootEntriesOutput(bootEntriesFixture());
    window.m_bootEntriesLoaded = true;
    window.m_bootEntriesSavedOrder = QStringList{QStringLiteral("0000"),
                                                 QStringLiteral("0001"),
                                                 QStringLiteral("0002")};
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    QTableWidgetItem *labelItem = window.m_bootEntriesTable->item(0, 1);
    QVERIFY(labelItem);
    window.m_bootEntriesUpdating = true;
    labelItem->setText(QStringLiteral("Renamed OS"));
    window.m_bootEntriesUpdating = false;
    window.bootEntryLabelChanged(0, 1);

    QCOMPARE(window.m_bootEntriesPendingRename.value(QStringLiteral("0000")),
             QStringLiteral("Renamed OS"));
    QVERIFY2(!window.m_privilegedSession,
             "staging a rename must not issue any privileged helper write");
    QVERIFY(window.m_bootEntriesTable->item(0, 1)->font().italic());

    // Editing the cell back to the persisted label cancels the pending rename.
    window.m_bootEntriesUpdating = true;
    window.m_bootEntriesTable->item(0, 1)->setText(QStringLiteral("Linux boot"));
    window.m_bootEntriesUpdating = false;
    window.bootEntryLabelChanged(0, 1);
    QVERIFY(!window.m_bootEntriesPendingRename.contains(QStringLiteral("0000")));
    QVERIFY(!window.m_bootEntriesTable->item(0, 1)->font().italic());
}

// Save issues the staged set in order — reorder CSV first, then efi-delete per
// pending id, then efi-rename per pending id — and clears the pending state on
// success. The Save button label is "Save".
void MainWindowUiTest::bootEntriesSaveIssuesReorderDeleteRenameAndClearsPending()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();

    QCOMPARE(window.m_bootEntriesSaveOrderButton->text(), QStringLiteral("Save"));

    window.m_bootEntriesInventory = window.parseBootEntriesOutput(bootEntriesFixture());
    window.m_bootEntriesLoaded = true;
    window.m_bootEntriesSavedOrder = QStringList{QStringLiteral("0000"),
                                                 QStringLiteral("0001"),
                                                 QStringLiteral("0002")};
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    // Reorder so the CSV becomes 0001,0000,0002.
    window.m_bootEntriesTable->selectRow(0);
    window.moveBootEntry(false);

    // Mark Boot0001 (now row 0) for deletion.
    window.m_bootEntriesTable->selectRow(0);
    window.deleteSelectedBootEntry();

    // Stage a rename of Boot0002 (now row 2).
    window.m_bootEntriesTable->setCurrentCell(2, 1);
    window.m_bootEntriesUpdating = true;
    window.m_bootEntriesTable->item(2, 1)->setText(QStringLiteral("Renamed Diag"));
    window.m_bootEntriesUpdating = false;
    window.bootEntryLabelChanged(2, 1);

    QVERIFY(window.m_bootEntriesPendingDelete.contains(QStringLiteral("0001")));
    QCOMPARE(window.m_bootEntriesPendingRename.value(QStringLiteral("0002")),
             QStringLiteral("Renamed Diag"));

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-save.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");
    closeEveryRepairProgressDialog(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    window.applyBootEntryOrder();

    const QStringList args = capturedHelperArguments(capturePath);
    int reorderIndex = -1;
    int deleteIndex = -1;
    int renameIndex = -1;
    for (int i = 0; i < args.size(); ++i) {
        if (args.at(i) == QStringLiteral("efi-reorder")) {
            reorderIndex = i;
        } else if (args.at(i) == QStringLiteral("efi-delete")) {
            deleteIndex = i;
        } else if (args.at(i) == QStringLiteral("efi-rename")) {
            renameIndex = i;
        }
    }
    QVERIFY2(reorderIndex >= 0, "Save must issue an efi-reorder request");
    QVERIFY2(deleteIndex >= 0, "Save must issue an efi-delete request");
    QVERIFY2(renameIndex >= 0, "Save must issue an efi-rename request");
    QVERIFY2(reorderIndex < deleteIndex && deleteIndex < renameIndex,
             "Save must run reorder, then deletes, then renames");

    QCOMPARE(args.at(reorderIndex + 1), QStringLiteral("/dev/test-host"));
    QCOMPARE(args.at(reorderIndex + 2), QStringLiteral("/dev/test-host-root"));
    QCOMPARE(args.at(reorderIndex + 3), QStringLiteral("0001,0000,0002"));

    QCOMPARE(args.at(deleteIndex + 1), QStringLiteral("/dev/test-host"));
    QCOMPARE(args.at(deleteIndex + 2), QStringLiteral("/dev/test-host-root"));
    QCOMPARE(args.at(deleteIndex + 3), QStringLiteral("0001"));

    QCOMPARE(args.at(renameIndex + 1), QStringLiteral("/dev/test-host"));
    QCOMPARE(args.at(renameIndex + 2), QStringLiteral("/dev/test-host-root"));
    QCOMPARE(args.at(renameIndex + 3), QStringLiteral("0002"));
    QCOMPARE(args.at(renameIndex + 4), QStringLiteral("Renamed Diag"));

    // On success the pending delete/rename state is cleared and Save disables.
    QVERIFY(window.m_bootEntriesPendingDelete.isEmpty());
    QVERIFY(window.m_bootEntriesPendingRename.isEmpty());
    QVERIFY2(!window.m_bootEntriesSaveOrderButton->isEnabled(),
             "Save must disable after a successful commit");
}

// The grub-list protocol parser: empty output, a missing-config header, and a
// full transcript with a submenu, a --class/$menuentry_id_option entry, a
// foreign os-prober entry and a default mark.
void MainWindowUiTest::grubMenuParserParsesProtocol()
{
    // Empty output yields no header and no rows.
    {
        const GrubMenuInventory inventory = parseGrubMenuOutputForTest(QString());
        QVERIFY(inventory.configPath.isEmpty());
        QVERIFY(inventory.defaultEntry.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // A missing-config transcript is header-only: the path carries the
    // "(not found)" suffix and there are no rows.
    {
        const GrubMenuInventory inventory = parseGrubMenuOutputForTest(
            QStringLiteral("GRUB config: /boot/grub/grub.cfg (not found)\n"));
        QCOMPARE(inventory.configPath, QStringLiteral("/boot/grub/grub.cfg (not found)"));
        QVERIFY(inventory.defaultEntry.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // Full transcript: a default entry, a submenu without a class, and a
    // foreign os-prober entry; row order is preserved verbatim.
    const GrubMenuInventory inventory = parseGrubMenuOutputForTest(
        QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                       "GRUB default: gnulinux-simple-abc\n"
                       "Ubuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n"
                       "Advanced options for Ubuntu\t\tgnulinux-advanced-abc\t0\t0\t0\tsubmenu\t\n"
                       "Windows Boot Manager (on /dev/sda1)\twindows\tosprober-efi-1234\t1\t0\t0\tmenuentry\t\n"));
    QCOMPARE(inventory.configPath, QStringLiteral("/boot/grub/grub.cfg"));
    QCOMPARE(inventory.defaultEntry, QStringLiteral("gnulinux-simple-abc"));
    QCOMPARE(inventory.rows.size(), 3);

    QCOMPARE(inventory.rows.at(0).title, QStringLiteral("Ubuntu"));
    QCOMPARE(inventory.rows.at(0).cls, QStringLiteral("ubuntu"));
    QCOMPARE(inventory.rows.at(0).id, QStringLiteral("gnulinux-simple-abc"));
    QVERIFY(!inventory.rows.at(0).foreign);
    QVERIFY(inventory.rows.at(0).isDefault);

    QCOMPARE(inventory.rows.at(1).title, QStringLiteral("Advanced options for Ubuntu"));
    QVERIFY(inventory.rows.at(1).cls.isEmpty());
    QVERIFY(!inventory.rows.at(1).foreign);
    QVERIFY(!inventory.rows.at(1).isDefault);

    QCOMPARE(inventory.rows.at(2).title, QStringLiteral("Windows Boot Manager (on /dev/sda1)"));
    QCOMPARE(inventory.rows.at(2).id, QStringLiteral("osprober-efi-1234"));
    QVERIFY(inventory.rows.at(2).foreign);
    QVERIFY(!inventory.rows.at(2).isDefault);

    // Class-less declarations must survive as rows with empty class/id rather
    // than aborting the helper: a menuentry that carries only an id, and a
    // submenu with neither class nor id.
    const GrubMenuInventory classless = parseGrubMenuOutputForTest(
        QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                       "GRUB default: uefi-firmware\n"
                       "UEFI Firmware Settings\t\tuefi-firmware\t0\t1\t0\tmenuentry\t\n"
                       "TUXEDO OS snapshots\t\t\t0\t0\t0\tsubmenu\t\n"));
    QCOMPARE(classless.rows.size(), 2);
    QCOMPARE(classless.rows.at(0).title, QStringLiteral("UEFI Firmware Settings"));
    QVERIFY(classless.rows.at(0).cls.isEmpty());
    QCOMPARE(classless.rows.at(0).id, QStringLiteral("uefi-firmware"));
    QVERIFY(classless.rows.at(0).isDefault);
    QCOMPARE(classless.rows.at(1).title, QStringLiteral("TUXEDO OS snapshots"));
    QVERIFY(classless.rows.at(1).cls.isEmpty());
    QVERIFY(classless.rows.at(1).id.isEmpty());
    QVERIFY(!classless.rows.at(1).isDefault);
}

// Switching the view selector to the GRUB menu issues a read-only grub-list
// request with the active scope identity and renders the menu without any
// editing affordance (mutation buttons live on the hidden EFI page).
void MainWindowUiTest::grubMenuViewSwitchIssuesGrubListAndReadOnly()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/false);
    window.updateBootEntriesControls();

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("grub-menu-view-switch.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");

    QVERIFY(window.m_bootEntriesViewCombo);
    QCOMPARE(window.m_bootEntriesViewCombo->count(), 2);
    window.m_bootEntriesViewCombo->setCurrentIndex(1); // GRUB menu
    QTest::qWait(300);

    // The switch issues exactly one read-only grub-list request with the
    // repair-target identity.  Each entry is "<disk>|<root>" so the identity
    // can be asserted without a QCOMPARE inside the counting lambda.
    const auto grubListRequests = [&] {
        QStringList identities;
        const QStringList args = capturedHelperArguments(capturePath);
        for (int i = 0; i + 2 < args.size(); ++i) {
            if (args.at(i) == QStringLiteral("grub-list")) {
                identities.append(args.at(i + 1) + QLatin1Char('|') + args.at(i + 2));
            }
        }
        return identities;
    };
    {
        const QStringList requests = grubListRequests();
        QCOMPARE(requests.size(), 1);
        QCOMPARE(requests.first(), QStringLiteral("/dev/test-system|/dev/test-root"));
    }

    // The GRUB tree is populated and strictly read-only.
    QVERIFY(window.m_grubMenuLoaded);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(0), QStringLiteral("✓ Ubuntu"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(2), QStringLiteral("✓"));
    QVERIFY(window.m_grubMenuTree->editTriggers() == QAbstractItemView::NoEditTriggers);
    QVERIFY(!window.m_grubMenuTree->topLevelItem(0)->flags().testFlag(Qt::ItemIsEditable));
    // The GRUB view is the active stacked page; the EFI mutation buttons are
    // hidden with it.
    QCOMPARE(window.m_bootEntriesStack->currentIndex(), 1);
    QVERIFY(!window.m_bootEntriesSaveOrderButton->isVisible());
    QVERIFY(!window.m_bootEntriesMoveUpButton->isVisible());
    QVERIFY(!window.m_bootEntriesDeleteButton->isVisible());

    // Changing the view selector back and forth must NOT re-fetch the boot
    // menu: it only swaps which already-loaded model is displayed.  The loaded
    // menu stays intact and no second grub-list request is issued.
    window.m_bootEntriesViewCombo->setCurrentIndex(0); // EFI/UKI entries
    QTest::qWait(50);
    QCOMPARE(window.m_bootEntriesStack->currentIndex(), 0);
    window.m_bootEntriesViewCombo->setCurrentIndex(1); // Boot menu (again)
    QTest::qWait(300);
    QCOMPARE(window.m_bootEntriesStack->currentIndex(), 1);
    QVERIFY(window.m_grubMenuLoaded);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    QVERIFY2(grubListRequests().size() == 1,
             "switching the view selector back to Boot menu must not re-fetch grub-list");
}

// The grub-legacy-list protocol parser: empty output, a missing-config header,
// and a full two-entry menu.lst transcript with a default index and a
// kernel-with-options entry (the options must never leak into the kernel field).
void MainWindowUiTest::grubLegacyParserParsesProtocol()
{
    // Empty output yields no header and no rows.
    {
        const GrubLegacyInventory inventory = parseGrubLegacyOutputForTest(QString());
        QVERIFY(inventory.configPath.isEmpty());
        QVERIFY(inventory.defaultEntry.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // A missing-config transcript is header-only: the path carries the
    // "(not found)" suffix and there are no rows.
    {
        const GrubLegacyInventory inventory = parseGrubLegacyOutputForTest(
            QStringLiteral("GRUB legacy config: (not found)\n"));
        QCOMPARE(inventory.configPath, QStringLiteral("(not found)"));
        QVERIFY(inventory.defaultEntry.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // Full transcript: file order is preserved, exactly one row is marked
    // default, and the kernel field carries only the path (never the command
    // line, which can hold secrets).
    const GrubLegacyInventory inventory = parseGrubLegacyOutputForTest(
        QStringLiteral("GRUB legacy config: /boot/grub/menu.lst\n"
                       "GRUB legacy default: 0\n"
                       "Debian GNU/Linux, kernel 2.6.18-6-686\t/vmlinuz-2.6.18-6-686\t/initrd.img-2.6.18-6-686\t1\n"
                       "Debian GNU/Linux, kernel 2.6.18-6-686 (single-user mode)\t/vmlinuz-2.6.18-6-686\t/initrd.img-2.6.18-6-686\t0\n"));
    QCOMPARE(inventory.configPath, QStringLiteral("/boot/grub/menu.lst"));
    QCOMPARE(inventory.defaultEntry, QStringLiteral("0"));
    QCOMPARE(inventory.rows.size(), 2);

    QCOMPARE(inventory.rows.at(0).title, QStringLiteral("Debian GNU/Linux, kernel 2.6.18-6-686"));
    QCOMPARE(inventory.rows.at(0).kernel, QStringLiteral("/vmlinuz-2.6.18-6-686"));
    QCOMPARE(inventory.rows.at(0).initrd, QStringLiteral("/initrd.img-2.6.18-6-686"));
    QVERIFY(inventory.rows.at(0).isDefault);

    QCOMPARE(inventory.rows.at(1).title, QStringLiteral("Debian GNU/Linux, kernel 2.6.18-6-686 (single-user mode)"));
    QCOMPARE(inventory.rows.at(1).kernel, QStringLiteral("/vmlinuz-2.6.18-6-686"));
    QCOMPARE(inventory.rows.at(1).initrd, QStringLiteral("/initrd.img-2.6.18-6-686"));
    QVERIFY(!inventory.rows.at(1).isDefault);
}

