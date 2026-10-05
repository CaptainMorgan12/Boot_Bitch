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

// The parser must preserve a `repair` class verbatim (the target scope emits it
// so the GUI can show the selected drive's own entries; only host scope
// collapses repair into host in the helper).
void MainWindowUiTest::bootEntriesParserKeepsRepairClass()
{
    const BootEntriesInventory inventory = parseBootEntriesOutputForTest(
        QStringLiteral("BootCurrent: 0001\n"
                       "BootOrder: 0001,0000,0002\n"
                       "Boot0001\tTarget OS\t\\EFI\\target\\grubx64.efi\trepair\tloader\t22222222-3333-4444-5555-666666666666\tTarget Drive\tlisted\n"
                       "Boot0000\tRunning host\tdevice-path-only\thost\tuki\t11111111-2222-3333-4444-555555555555\tHost Drive\tlisted\n"
                       "Boot0002\tUSB\tdevice-path-only\tunknown\tfallback\tunknown\t\tunlisted\n"));
    QCOMPARE(inventory.rows.size(), 3);
    QCOMPARE(inventory.rows.at(0).id, QStringLiteral("0001"));
    QCOMPARE(inventory.rows.at(0).entryClass, QStringLiteral("repair"));
    QCOMPARE(inventory.rows.at(0).deviceModel, QStringLiteral("Target Drive"));
    QCOMPARE(inventory.rows.at(1).entryClass, QStringLiteral("host"));
    QCOMPARE(inventory.rows.at(2).entryClass, QStringLiteral("unknown"));
    QVERIFY(!inventory.rows.at(2).listed);
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

// The parser is wired to the table and Move Up/Move Down reorder rows in the
// table only (no privileged helper write).
void MainWindowUiTest::bootEntriesParserWiredToFixtureAndMoveReorder()
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
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    QCOMPARE(window.m_bootEntriesTable->rowCount(), 3);
    // BootCurrent is empty in the fixture, so no row carries the current
    // marker; the unlisted row is prefixed with "—" and the listed rows show
    // the plain Boot#### id.
    QCOMPARE(window.m_bootEntriesTable->item(0, 0)->text(), QStringLiteral("Boot0000"));
    QCOMPARE(window.m_bootEntriesTable->item(2, 0)->text(), QStringLiteral("— Boot0002"));
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Linux boot"));

    // Move Up reorders row 1 above row 0 in the table only.
    window.m_bootEntriesTable->selectRow(1);
    window.moveBootEntry(true);
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Windows Boot Manager"));
    QCOMPARE(window.m_bootEntriesTable->item(1, 1)->text(), QStringLiteral("Linux boot"));
    QVERIFY2(!window.m_privilegedSession,
             "move up/down must not issue any privileged helper write");

    // Move Down restores the original order.
    window.m_bootEntriesTable->selectRow(0);
    window.moveBootEntry(false);
    QCOMPARE(window.m_bootEntriesTable->item(0, 1)->text(), QStringLiteral("Linux boot"));
    QCOMPARE(window.m_bootEntriesTable->item(1, 1)->text(), QStringLiteral("Windows Boot Manager"));
    QVERIFY(!window.m_privilegedSession);
}

// Save assembles the complete comma-joined Boot#### CSV in the current table
// order and sends it through efi-reorder with the running-host identity. The
// reorder-only staged flow commits through the single Save action.
void MainWindowUiTest::bootEntriesSaveOrderAssemblesCsv()
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

    // Reorder so the intended CSV is 0001,0000,0002.
    window.m_bootEntriesTable->selectRow(0);
    window.moveBootEntry(false);

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-requests.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    window.m_bootEntriesTable->selectRow(1);
    window.applyBootEntryOrder();

    const QStringList args = capturedHelperArguments(capturePath);
    bool sawReorder = false;
    for (int i = 0; i + 3 < args.size(); ++i) {
        if (args.at(i) != QStringLiteral("efi-reorder")) {
            continue;
        }
        sawReorder = true;
        QCOMPARE(args.at(i + 1), QStringLiteral("/dev/test-host"));
        QCOMPARE(args.at(i + 2), QStringLiteral("/dev/test-host-root"));
        QCOMPARE(args.at(i + 3), QStringLiteral("0001,0000,0002"));
    }
    QVERIFY2(sawReorder, "Save must issue an efi-reorder request with the running-host identity");
}

// A blank/whitespace inline Label edit is rejected: the cell is reverted to the
// persisted label and no efi-rename request is issued.
void MainWindowUiTest::bootEntriesBlankLabelRenameRevertsCell()
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
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    QTableWidgetItem *labelItem = window.m_bootEntriesTable->item(0, 1);
    QVERIFY(labelItem);
    QCOMPARE(labelItem->text(), QStringLiteral("Linux boot"));

    // Simulate the committed blank edit (whitespace-only) and drive the
    // itemChanged handler directly, exactly as the inline delegate would.
    window.m_bootEntriesUpdating = true;
    labelItem->setText(QStringLiteral("   "));
    window.m_bootEntriesUpdating = false;
    acceptNextMessageBox(&window, QMessageBox::Ok);
    window.bootEntryLabelChanged(0, 1);

    QCOMPARE(labelItem->text(), QStringLiteral("Linux boot"));
    QVERIFY2(!window.m_privilegedSession,
             "a blank label must be reverted without any privileged helper request");
}

// In target scope the read-only efi-list request must carry the selected repair
// target identity, exactly like the host scope carries the running host.
void MainWindowUiTest::bootEntriesEfiListUsesTargetIdentity()
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
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-target-list.log"));
    QVERIFY2(startBootEntriesFakeSession(window, capturePath),
             "the scripted privileged session must start");

    window.refreshBootEntries();

    const QStringList args = capturedHelperArguments(capturePath);
    bool sawEfiList = false;
    for (int i = 0; i + 2 < args.size(); ++i) {
        if (args.at(i) == QStringLiteral("efi-list")) {
            sawEfiList = true;
            QCOMPARE(args.at(i + 1), QStringLiteral("/dev/test-system"));
            QCOMPARE(args.at(i + 2), QStringLiteral("/dev/test-root"));
        }
    }
    QVERIFY2(sawEfiList,
             "efi-list must be issued with the repair-target identity in target scope");
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

// The Boots column renders the role-derived human label: a `loader` role shows
// "GRUB menu", a `uki` role shows "Direct (UKI)", and the column is read-only
// even though the Label column stays inline-editable.
void MainWindowUiTest::bootEntriesBootsColumnRendersRoleLabel()
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
    window.populateBootEntriesTable();
    window.updateBootEntriesControls();

    // Boot, Label, Loader, Boots, Class, Drive — the new column is index 3.
    constexpr int kBootsColumn = 3;
    QCOMPARE(window.m_bootEntriesTable->columnCount(), 6);
    QCOMPARE(window.m_bootEntriesTable->horizontalHeaderItem(kBootsColumn)->text(),
             QStringLiteral("Boots"));
    // Fixture row 0 is role `uki`, row 1 is role `loader`.
    QCOMPARE(window.m_bootEntriesTable->item(0, kBootsColumn)->text(),
             QStringLiteral("Direct (UKI)"));
    QCOMPARE(window.m_bootEntriesTable->item(1, kBootsColumn)->text(),
             QStringLiteral("GRUB menu"));
    // The Boots column is read-only (no edit flag), while Label stays editable.
    QVERIFY(!window.m_bootEntriesTable->item(0, kBootsColumn)->flags().testFlag(Qt::ItemIsEditable));
    QVERIFY(window.m_bootEntriesTable->item(0, 1)->flags().testFlag(Qt::ItemIsEditable));
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

// Navigating to the Boot entries tab must reload the running host's firmware
// entries automatically (no manual Load Entries click) whenever an active Host
// Maintenance scope has an established administrator session. Outside Host
// Maintenance, or with no session ready, the auto-reload stays silent and must
// never raise its own authorization prompt.
void MainWindowUiTest::bootEntriesAutoReloadOnTabShow()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // (b) Outside Host Maintenance the auto-reload is skipped even though a
    // privileged session exists; no efi-list request may be issued.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-auto-outside.log"));
        QVERIFY2(startBootEntriesFakeSession(window, capturePath),
                 "the scripted privileged session must start");

        window.m_tabs->setCurrentIndex(2); // Boot entries tab
        QTest::qWait(200);

        const QStringList args = capturedHelperArguments(capturePath);
        QVERIFY2(!args.contains(QStringLiteral("efi-list")),
                 "no efi-list request may be issued while Host Maintenance is off");
    }

    // (b) Host Maintenance active but the administrator session not ready: the
    // auto-reload stays silent and must never prompt for authorization.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/true);
        window.updateBootEntriesControls();

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-auto-notready.log"));
        QVERIFY2(startBootEntriesFakeSession(window, capturePath),
                 "the scripted privileged session must start");
        window.m_privilegedSessionReady = false;

        window.m_tabs->setCurrentIndex(2); // Boot entries tab
        QTest::qWait(200);

        const QStringList args = capturedHelperArguments(capturePath);
        QVERIFY2(!args.contains(QStringLiteral("efi-list")),
                 "no efi-list request may be issued while no administrator session is ready");
        QVERIFY(!window.m_bootEntriesLoaded);
    }

    // (a) Host Maintenance active with an established session: the auto-reload
    // issues the read-only efi-list request and populates the table without any
    // manual Load Entries click.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/true);
        window.updateBootEntriesControls();

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.filePath(QStringLiteral("boot-entries-auto-reload.log"));
        QVERIFY2(startBootEntriesFakeSession(window, capturePath),
                 "the scripted privileged session must start");

        window.m_tabs->setCurrentIndex(2); // Boot entries tab
        QTest::qWait(300);

        const QStringList args = capturedHelperArguments(capturePath);
        QVERIFY2(args.contains(QStringLiteral("efi-list")),
                 "navigating to Boot entries with an active host session must auto-reload via efi-list");
        QVERIFY(window.m_bootEntriesLoaded);
        QCOMPARE(window.m_bootEntriesTable->rowCount(), 2);
        // The captured request names the running-host identity, exactly like
        // the manual Load Entries flow.
        bool sawEfiList = false;
        for (int i = 0; i + 2 < args.size(); ++i) {
            if (args.at(i) == QStringLiteral("efi-list")) {
                sawEfiList = true;
                QCOMPARE(args.at(i + 1), QStringLiteral("/dev/test-host"));
                QCOMPARE(args.at(i + 2), QStringLiteral("/dev/test-host-root"));
            }
        }
        QVERIFY2(sawEfiList, "the auto-reload must pass the running-host identity to efi-list");
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

// The Save button label is exactly "Save" (the staged reorder/delete/rename
// commit action), not the old "Save Order".
void MainWindowUiTest::bootEntriesSaveButtonLabelIsSave()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    QVERIFY(window.m_bootEntriesSaveOrderButton);
    QCOMPARE(window.m_bootEntriesSaveOrderButton->text(), QStringLiteral("Save"));
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

// The seven-field grub-list rows carry the helper's brace-tracked submenu
// nesting: a grub.cfg with an if/then/fi-wrapped menuentry (depth 0), a
// top-level submenu (depth 0) with two children (depth 1), and a `function`
// block (which yields no row) round-trips with the correct depth/kind, and the
// table renders the nesting (indent + ▸ marker + bold submenu).
void MainWindowUiTest::grubMenuParserTracksSubmenuNesting()
{
    const GrubMenuInventory inventory = parseGrubMenuOutputForTest(
        QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                       "GRUB default: 0\n"
                       "Boot normally\tlinux\tnormal-1\t0\t1\t0\tmenuentry\t\n"
                       "Advanced options for Ubuntu\t\tsubmenu-x\t0\t0\t0\tsubmenu\t\n"
                       "Ubuntu, with Linux 6.8\tlinux\tchild-1\t0\t0\t1\tmenuentry\t\n"
                       "Ubuntu, with Linux 6.8 (recovery mode)\trecovery\tchild-2\t0\t0\t1\tmenuentry\t\n"));
    QCOMPARE(inventory.rows.size(), 4);

    QCOMPARE(inventory.rows.at(0).kind, QStringLiteral("menuentry"));
    QCOMPARE(inventory.rows.at(0).depth, 0);
    QVERIFY(inventory.rows.at(0).isDefault);

    QCOMPARE(inventory.rows.at(1).kind, QStringLiteral("submenu"));
    QCOMPARE(inventory.rows.at(1).depth, 0);
    QVERIFY(!inventory.rows.at(1).isDefault);

    QCOMPARE(inventory.rows.at(2).kind, QStringLiteral("menuentry"));
    QCOMPARE(inventory.rows.at(2).depth, 1);

    QCOMPARE(inventory.rows.at(3).kind, QStringLiteral("menuentry"));
    QCOMPARE(inventory.rows.at(3).depth, 1);

    // Rendering: the default mark stays at the front of its row, submenu rows
    // become bold expandable parents (collapsed by default), and their children
    // nest underneath without any manual indentation.
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_grubMenuInventory = inventory;
    window.m_grubMenuLoaded = true;
    window.populateGrubMenuTable();
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    QTreeWidgetItem *defaultItem = window.m_grubMenuTree->topLevelItem(0);
    QCOMPARE(defaultItem->text(0), QStringLiteral("✓ Boot normally"));
    QVERIFY(defaultItem->font(0).bold());
    QTreeWidgetItem *submenuItem = window.m_grubMenuTree->topLevelItem(1);
    QCOMPARE(submenuItem->text(0), QStringLiteral("Advanced options for Ubuntu"));
    QVERIFY(submenuItem->font(0).bold());
    // The submenu is an expandable parent with its children nested under it and
    // stays collapsed by default.
    QCOMPARE(submenuItem->childCount(), 2);
    QVERIFY(!submenuItem->isExpanded());
    QCOMPARE(submenuItem->child(0)->text(0), QStringLiteral("Ubuntu, with Linux 6.8"));
    QVERIFY(!submenuItem->child(0)->font(0).bold());
    QCOMPARE(submenuItem->child(1)->text(0), QStringLiteral("Ubuntu, with Linux 6.8 (recovery mode)"));
    QVERIFY(!submenuItem->child(1)->font(0).bold());
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
    // repair-target identity.
    const QStringList args = capturedHelperArguments(capturePath);
    bool sawGrubList = false;
    for (int i = 0; i + 2 < args.size(); ++i) {
        if (args.at(i) == QStringLiteral("grub-list")) {
            sawGrubList = true;
            QCOMPARE(args.at(i + 1), QStringLiteral("/dev/test-system"));
            QCOMPARE(args.at(i + 2), QStringLiteral("/dev/test-root"));
        }
    }
    QVERIFY2(sawGrubList,
             "switching to the GRUB view must issue a grub-list request with the repair-target identity");

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
}

// A missing/empty grub.cfg yields the header-only transcript, and the view then
// falls back to extlinux-list; when that is also missing it renders the
// combined "no boot menu configuration" message without crashing (zero rows,
// no default mark).
void MainWindowUiTest::grubMenuMissingConfigYieldsHeaderOnly()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());

    // A minimal fake session that answers both grub-list and extlinux-list with
    // their missing-config headers (no rows), so the fallback also comes up
    // empty.
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
    *extlinux-list*)
      printf 'OUT\t%s\tEXTLINUX config: (not found)\n' "$id"
      ;;
    *grub-list*)
      printf 'OUT\t%s\tGRUB config: /boot/grub/grub.cfg (not found)\n' "$id"
      ;;
  esac
  printf 'DONE\t%s\t0\n' "$id"
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-boot-menu-missing-session"),
                    captureDir.filePath(QStringLiteral("boot-menu-missing.log"))});
    if (!session->waitForStarted(5000)) {
        delete session;
        QVERIFY2(false, "the scripted missing-config session must start");
        return;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;

    window.refreshGrubMenu();

    QVERIFY(window.m_grubMenuLoaded);
    QVERIFY(window.m_grubMenuInventory.rows.isEmpty());
    QVERIFY(window.m_grubMenuInventory.configPath.contains(QStringLiteral("(not found)")));
    QVERIFY(window.m_extlinuxInventory.rows.isEmpty());
    QVERIFY(window.m_extlinuxInventory.configPath.contains(QStringLiteral("(not found)")));
    QVERIFY(!window.m_bootMenuShowingExtlinux);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 0);
    if (window.m_grubMenuDetails) {
        QVERIFY(window.m_grubMenuDetails->toPlainText().contains(QStringLiteral("No boot menu configuration was found")));
    }
}

// The grub-list parser must carry the two new correlation headers into
// GrubMenuInventory: the ESP GPT PARTUUID (lowercased by the helper) and the
// drive model, both empty when no ESP is resolved.
void MainWindowUiTest::grubMenuParserParsesEspAndDriveHeaders()
{
    const GrubMenuInventory inventory = parseGrubMenuOutputForTest(
        QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                       "GRUB esp: 11111111-2222-3333-4444-555555555555\n"
                       "GRUB drive: Test Drive\n"
                       "GRUB default: gnulinux-simple-abc\n"
                       "Ubuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n"));
    QCOMPARE(inventory.configPath, QStringLiteral("/boot/grub/grub.cfg"));
    QCOMPARE(inventory.espPartuuid, QStringLiteral("11111111-2222-3333-4444-555555555555"));
    QCOMPARE(inventory.driveModel, QStringLiteral("Test Drive"));
    QCOMPARE(inventory.defaultEntry, QStringLiteral("gnulinux-simple-abc"));
    QCOMPARE(inventory.rows.size(), 1);
    QVERIFY(inventory.rows.at(0).isDefault);

    // A header with an empty value (no ESP / unresolved model) parses to empty
    // strings so the GUI omits the cross-reference section downstream.
    const GrubMenuInventory empty = parseGrubMenuOutputForTest(
        QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                       "GRUB esp: \n"
                       "GRUB drive: \n"
                       "GRUB default: 0\n"
                       "Ubuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n"));
    QVERIFY(empty.espPartuuid.isEmpty());
    QVERIFY(empty.driveModel.isEmpty());
    QCOMPARE(empty.defaultEntry, QStringLiteral("0"));
}

// The page heading follows the view selector: "EFI boot entries" in the
// EFI/UKI view and "Boot menu" in the read-only boot-menu view, while the
// tab label itself stays "EFI boot entries".
void MainWindowUiTest::grubMenuHeadingUpdatesOnViewSwitch()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    QVERIFY(window.m_bootEntriesHeading);
    QVERIFY(window.m_bootEntriesViewCombo);
    QCOMPARE(window.m_bootEntriesHeading->text(), QStringLiteral("EFI boot entries"));

    window.m_bootEntriesViewCombo->setCurrentIndex(1); // Boot menu
    QCOMPARE(window.m_bootEntriesHeading->text(), QStringLiteral("Boot menu"));

    window.m_bootEntriesViewCombo->setCurrentIndex(0); // EFI/UKI entries
    QCOMPARE(window.m_bootEntriesHeading->text(), QStringLiteral("EFI boot entries"));
}

// The GRUB details pane cross-references the EFI firmware entries on the same
// disk: it lists every loaded entry whose GPT PARTUUID equals the GRUB menu's
// ESP PARTUUID (case-insensitive), shows a hint when the ESP is known but the
// EFI inventory is not loaded, and omits the section when there is no ESP.
void MainWindowUiTest::grubMenuCrossReferenceListsMatchingFirmwareEntries()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/true);
    window.updateBootEntriesControls();

    // (a) Matching PARTUUID entries are listed (case-insensitive); a
    // non-matching entry is omitted.
    {
        window.m_bootEntriesInventory = window.parseBootEntriesOutput(
            QStringLiteral("BootCurrent: 0000\n"
                           "BootOrder: 0000,0001\n"
                           "Boot0000\tLinux boot\tdevice-path-only\thost\tuki\tAAAAaaaa-bbbb-cccc-dddd-eeeeeeeeeeee\tTest Drive\tlisted\n"
                           "Boot0001\tOther disk\t\\EFI\\other\\grubx64.efi\tforeign\tloader\tffffffff-ffff-ffff-ffff-ffffffffffff\tOther Drive\tlisted\n"));
        window.m_bootEntriesLoaded = true;
        window.m_grubMenuInventory = window.parseGrubMenuOutput(
            QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                           "GRUB esp: aaaaAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE\n"
                           "GRUB drive: Test Drive\n"
                           "GRUB default: gnulinux-simple-abc\n"
                           "Ubuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n"));
        window.m_grubMenuLoaded = true;
        window.populateGrubMenuTable();

        const QString details = window.m_grubMenuDetails->toPlainText();
        QVERIFY2(details.contains(QStringLiteral("Firmware boot entries on this disk")),
                 "the cross-reference section must be present");
        QVERIFY2(details.contains(QStringLiteral("Boot0000")),
                 "the matching firmware entry id must be listed");
        QVERIFY2(details.contains(QStringLiteral("Linux boot")),
                 "the matching firmware entry label must be listed");
        QVERIFY2(details.contains(QStringLiteral("Direct (UKI)")),
                 "the boots label for the matching entry must be listed");
        QVERIFY2(!details.contains(QStringLiteral("Other disk")),
                 "a non-matching PARTUUID entry must be omitted");
    }

    // (b) The EFI inventory is not loaded but the ESP PARTUUID is known: the
    // section carries the hint pointing at the EFI/UKI view.
    {
        window.m_bootEntriesInventory = BootEntriesInventory();
        window.m_bootEntriesLoaded = false;
        window.m_grubMenuInventory = window.parseGrubMenuOutput(
            QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                           "GRUB esp: aaaaAAAA-BBBB-CCCC-DDDD-EEEEEEEEEEEE\n"
                           "GRUB default: gnulinux-simple-abc\n"
                           "Ubuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n"));
        window.m_grubMenuLoaded = true;
        window.populateGrubMenuTable();
        // The row was already selected from (a), so re-drive the selection to
        // refresh the details pane for the new inventory.
        window.m_grubMenuTree->clearSelection();
        window.m_grubMenuTree->setCurrentItem(window.m_grubMenuTree->topLevelItem(0));

        const QString details = window.m_grubMenuDetails->toPlainText();
        QVERIFY2(details.contains(QStringLiteral("Firmware boot entries on this disk")),
                 "the section header must still be present");
        QVERIFY2(details.contains(QStringLiteral("Switch to the EFI/UKI entries view")),
                 "the hint must point at the EFI/UKI entries view");
    }

    // (c) No ESP PARTUUID: the section is omitted entirely.
    {
        window.m_bootEntriesInventory = BootEntriesInventory();
        window.m_bootEntriesLoaded = false;
        window.m_grubMenuInventory = window.parseGrubMenuOutput(
            QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                           "GRUB default: gnulinux-simple-abc\n"
                           "Ubuntu\tubuntu\tgnulinux-simple-abc\t0\t1\t0\tmenuentry\t\n"));
        window.m_grubMenuLoaded = true;
        window.populateGrubMenuTable();
        window.m_grubMenuTree->clearSelection();
        window.m_grubMenuTree->setCurrentItem(window.m_grubMenuTree->topLevelItem(0));

        const QString details = window.m_grubMenuDetails->toPlainText();
        QVERIFY2(!details.contains(QStringLiteral("Firmware boot entries on this disk")),
                 "the cross-reference section must be omitted without an ESP PARTUUID");
    }
}

// A submenu that loads an external config file (configfile directive) has no
// menuentry children; the 8-field grub-list protocol carries the target path so
// the GUI can surface it. The parser must capture the configfile, and the
// details pane must render "Loads: …" for the submenu.
void MainWindowUiTest::grubMenuSubmenuConfigfileSurfacedInDetails()
{
    // (a) The 8-field protocol parse: a configfile submenu row carries its
    // target, a sibling menuentry row carries an empty configfile.
    const GrubMenuInventory inventory = parseGrubMenuOutputForTest(
        QStringLiteral("GRUB config: /boot/grub/grub.cfg\n"
                       "GRUB default: 0\n"
                       "TUXEDO OS snapshots\t\t\t0\t0\t0\tsubmenu\t${prefix}/grub-btrfs.cfg\n"
                       "Boot normally\tlinux\tnormal-1\t0\t1\t0\tmenuentry\t\n"));
    QCOMPARE(inventory.rows.size(), 2);
    QCOMPARE(inventory.rows.at(0).kind, QStringLiteral("submenu"));
    QCOMPARE(inventory.rows.at(0).configfile, QStringLiteral("${prefix}/grub-btrfs.cfg"));
    QCOMPARE(inventory.rows.at(1).kind, QStringLiteral("menuentry"));
    QVERIFY(inventory.rows.at(1).configfile.isEmpty());

    // (b) The details pane (and by extension the row tooltip) surfaces the
    // configfile target for the submenu, so an empty expandable node is no
    // longer unexplained.
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_grubMenuInventory = inventory;
    window.m_grubMenuLoaded = true;
    window.populateGrubMenuTable();
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    window.m_grubMenuTree->setCurrentItem(window.m_grubMenuTree->topLevelItem(0));

    const QString details = window.m_grubMenuDetails->toPlainText();
    QVERIFY2(details.contains(QStringLiteral("Loads: ${prefix}/grub-btrfs.cfg")),
             "a configfile submenu must surface its target in the details pane");
}

// A fixed extlinux-list transcript covering the protocol shapes the parser and
// the fallback tree must handle: a DEFAULT header, a MENU TITLE, two LABEL
// blocks (one with MENU LABEL/KERNEL/INITRD, one with LINUX and a fallback
// menu label) and a default mark.
static QString extlinuxFixture()
{
    return QStringLiteral(
        "EXTLINUX config: /boot/extlinux.conf\n"
        "EXTLINUX menu title: Boot Menu\n"
        "EXTLINUX default: lts\n"
        "lts\tLinux lts\t/boot/vmlinuz-lts\t/boot/initramfs-lts\t1\n"
        "virt\tvirt\t/boot/vmlinuz-virt\t/boot/initramfs-virt\t0\n");
}

// The extlinux-list protocol parser: empty output, a missing-config header, and
// a full transcript with a default mark and a fallback menu label.
void MainWindowUiTest::extlinuxParserParsesProtocol()
{
    // Empty output yields no header and no rows.
    {
        const ExtlinuxInventory inventory = parseExtlinuxOutputForTest(QString());
        QVERIFY(inventory.configPath.isEmpty());
        QVERIFY(inventory.menuTitle.isEmpty());
        QVERIFY(inventory.defaultEntry.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // A missing-config transcript is header-only: the path carries the
    // "(not found)" suffix and there are no rows.
    {
        const ExtlinuxInventory inventory = parseExtlinuxOutputForTest(
            QStringLiteral("EXTLINUX config: (not found)\n"));
        QCOMPARE(inventory.configPath, QStringLiteral("(not found)"));
        QVERIFY(inventory.menuTitle.isEmpty());
        QVERIFY(inventory.defaultEntry.isEmpty());
        QVERIFY(inventory.rows.isEmpty());
    }

    // Full transcript: file order is preserved, the MENU LABEL overrides the
    // fallback, and exactly one row is marked default.
    const ExtlinuxInventory inventory = parseExtlinuxOutputForTest(extlinuxFixture());
    QCOMPARE(inventory.configPath, QStringLiteral("/boot/extlinux.conf"));
    QCOMPARE(inventory.menuTitle, QStringLiteral("Boot Menu"));
    QCOMPARE(inventory.defaultEntry, QStringLiteral("lts"));
    QCOMPARE(inventory.rows.size(), 2);

    QCOMPARE(inventory.rows.at(0).label, QStringLiteral("lts"));
    QCOMPARE(inventory.rows.at(0).menuLabel, QStringLiteral("Linux lts"));
    QCOMPARE(inventory.rows.at(0).kernel, QStringLiteral("/boot/vmlinuz-lts"));
    QCOMPARE(inventory.rows.at(0).initrd, QStringLiteral("/boot/initramfs-lts"));
    QVERIFY(inventory.rows.at(0).isDefault);

    QCOMPARE(inventory.rows.at(1).label, QStringLiteral("virt"));
    QCOMPARE(inventory.rows.at(1).menuLabel, QStringLiteral("virt"));
    QCOMPARE(inventory.rows.at(1).kernel, QStringLiteral("/boot/vmlinuz-virt"));
    QCOMPARE(inventory.rows.at(1).initrd, QStringLiteral("/boot/initramfs-virt"));
    QVERIFY(!inventory.rows.at(1).isDefault);
}

// When the GRUB menu is empty/"(not found)", the boot-menu view falls back to
// extlinux-list and populates the read-only tree from the extlinux rows (Label
// column = menu label, Class column = kernel path, Default marker = ✓).
void MainWindowUiTest::bootMenuFallbackToExtlinuxWhenGrubAbsent()
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
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-menu-fallback.log"));

    // A fake session that answers grub-list with "(not found)" and extlinux-list
    // with a full two-entry menu, so the fallback path is exercised end-to-end.
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
    *extlinux-list*)
      printf 'OUT\t%s\tEXTLINUX config: /boot/extlinux.conf\n' "$id"
      printf 'OUT\t%s\tEXTLINUX menu title: Boot Menu\n' "$id"
      printf 'OUT\t%s\tEXTLINUX default: lts\n' "$id"
      printf 'OUT\t%s\tlts\tLinux lts\t/boot/vmlinuz-lts\t/boot/initramfs-lts\t1\n' "$id"
      printf 'OUT\t%s\tvirt\tvirt\t/boot/vmlinuz-virt\t/boot/initramfs-virt\t0\n' "$id"
      ;;
    *grub-list*)
      printf 'OUT\t%s\tGRUB config: /boot/grub/grub.cfg (not found)\n' "$id"
      ;;
  esac
  printf 'DONE\t%s\t0\n' "$id"
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-boot-menu-fallback-session"),
                    capturePath});
    if (!session->waitForStarted(5000)) {
        delete session;
        QVERIFY2(false, "the scripted fallback session must start");
        return;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;

    window.refreshGrubMenu();

    // Both requests were issued, grub-list first then extlinux-list.
    const QStringList args = capturedHelperArguments(capturePath);
    int grubIndex = -1;
    int extIndex = -1;
    for (int i = 0; i + 2 < args.size(); ++i) {
        if (args.at(i) == QStringLiteral("grub-list") && grubIndex < 0) {
            grubIndex = i;
        } else if (args.at(i) == QStringLiteral("extlinux-list") && extIndex < 0) {
            extIndex = i;
        }
    }
    QVERIFY2(grubIndex >= 0 && extIndex >= 0,
             "the fallback must issue grub-list first and then extlinux-list");
    QVERIFY2(grubIndex < extIndex,
             "grub-list must be issued before extlinux-list");
    QCOMPARE(args.at(extIndex + 1), QStringLiteral("/dev/test-system"));
    QCOMPARE(args.at(extIndex + 2), QStringLiteral("/dev/test-root"));

    // The tree renders the extlinux rows: Label column = menu label, Class
    // column = kernel, and the default row carries the ✓ marker.
    QVERIFY(window.m_grubMenuLoaded);
    QVERIFY(window.m_bootMenuShowingExtlinux);
    QCOMPARE(window.m_extlinuxInventory.rows.size(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(0), QStringLiteral("✓ Linux lts"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(1), QStringLiteral("/boot/vmlinuz-lts"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(2), QStringLiteral("✓"));
    QVERIFY(window.m_grubMenuTree->topLevelItem(0)->font(0).bold());
    QCOMPARE(window.m_grubMenuTree->topLevelItem(1)->text(0), QStringLiteral("virt"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(1)->text(1), QStringLiteral("/boot/vmlinuz-virt"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(1)->text(2), QString());
    // Strictly read-only, exactly like the GRUB menu view.
    QVERIFY(window.m_grubMenuTree->editTriggers() == QAbstractItemView::NoEditTriggers);
    QVERIFY(!window.m_grubMenuTree->topLevelItem(0)->flags().testFlag(Qt::ItemIsEditable));
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

// When both the GRUB menu and the extlinux menu are empty/"(not found)", the
// boot-menu view falls back to grub-legacy-list and populates the read-only
// tree from the menu.lst rows (Label column = title, Class column = kernel
// path, Default marker = ✓).
void MainWindowUiTest::bootMenuFallbackToGrubLegacyWhenGrubAndExtlinuxAbsent()
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
    const QString capturePath = captureDir.filePath(QStringLiteral("boot-menu-grub-legacy-fallback.log"));

    // A fake session that answers grub-list and extlinux-list with "(not found)"
    // and grub-legacy-list with a full two-entry menu.lst, so the three-step
    // fallback is exercised end-to-end.
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
    *grub-legacy-list*)
      printf 'OUT\t%s\tGRUB legacy config: /boot/grub/menu.lst\n' "$id"
      printf 'OUT\t%s\tGRUB legacy default: 0\n' "$id"
      printf 'OUT\t%s\tDebian GNU/Linux, kernel 2.6.18-6-686\t/vmlinuz-2.6.18-6-686\t/initrd.img-2.6.18-6-686\t1\n' "$id"
      printf 'OUT\t%s\tDebian GNU/Linux, kernel 2.6.18-6-686 (single-user mode)\t/vmlinuz-2.6.18-6-686\t/initrd.img-2.6.18-6-686\t0\n' "$id"
      ;;
    *extlinux-list*)
      printf 'OUT\t%s\tEXTLINUX config: (not found)\n' "$id"
      ;;
    *grub-list*)
      printf 'OUT\t%s\tGRUB config: /boot/grub/grub.cfg (not found)\n' "$id"
      ;;
  esac
  printf 'DONE\t%s\t0\n' "$id"
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-boot-menu-grub-legacy-session"),
                    capturePath});
    if (!session->waitForStarted(5000)) {
        delete session;
        QVERIFY2(false, "the scripted grub-legacy fallback session must start");
        return;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;

    window.refreshGrubMenu();

    // All three requests were issued in order: grub-list, then extlinux-list,
    // then grub-legacy-list.
    const QStringList args = capturedHelperArguments(capturePath);
    int grubIndex = -1;
    int extIndex = -1;
    int legacyIndex = -1;
    for (int i = 0; i + 2 < args.size(); ++i) {
        if (args.at(i) == QStringLiteral("grub-list") && grubIndex < 0) {
            grubIndex = i;
        } else if (args.at(i) == QStringLiteral("extlinux-list") && extIndex < 0) {
            extIndex = i;
        } else if (args.at(i) == QStringLiteral("grub-legacy-list") && legacyIndex < 0) {
            legacyIndex = i;
        }
    }
    QVERIFY2(grubIndex >= 0 && extIndex >= 0 && legacyIndex >= 0,
             "the fallback must issue grub-list, then extlinux-list, then grub-legacy-list");
    QVERIFY2(grubIndex < extIndex && extIndex < legacyIndex,
             "the fallback order must be grub-list -> extlinux-list -> grub-legacy-list");
    QCOMPARE(args.at(legacyIndex + 1), QStringLiteral("/dev/test-system"));
    QCOMPARE(args.at(legacyIndex + 2), QStringLiteral("/dev/test-root"));

    // The tree renders the GRUB legacy rows: Label column = title, Class
    // column = kernel, and the default row carries the ✓ marker.
    QVERIFY(window.m_grubMenuLoaded);
    QVERIFY(window.m_bootMenuShowingGrubLegacy);
    QVERIFY(!window.m_bootMenuShowingExtlinux);
    QCOMPARE(window.m_grubLegacyInventory.rows.size(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItemCount(), 2);
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(0),
             QStringLiteral("✓ Debian GNU/Linux, kernel 2.6.18-6-686"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(1), QStringLiteral("/vmlinuz-2.6.18-6-686"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(0)->text(2), QStringLiteral("✓"));
    QVERIFY(window.m_grubMenuTree->topLevelItem(0)->font(0).bold());
    QCOMPARE(window.m_grubMenuTree->topLevelItem(1)->text(0),
             QStringLiteral("Debian GNU/Linux, kernel 2.6.18-6-686 (single-user mode)"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(1)->text(1), QStringLiteral("/vmlinuz-2.6.18-6-686"));
    QCOMPARE(window.m_grubMenuTree->topLevelItem(1)->text(2), QString());
    // Strictly read-only, exactly like the GRUB and extlinux menu views.
    QVERIFY(window.m_grubMenuTree->editTriggers() == QAbstractItemView::NoEditTriggers);
    QVERIFY(!window.m_grubMenuTree->topLevelItem(0)->flags().testFlag(Qt::ItemIsEditable));
}

// The Boot entries header follows the Diagnostics responsive pattern: the view
// selector sits to the right of the scope label (both right-aligned), and the
// six action buttons live in a wrapping (height-for-width) flow layout rather
// than a non-reflowing QBoxLayout.
void MainWindowUiTest::bootEntriesHeaderComboRightOfScopeAndButtonRowWraps()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    QVERIFY(window.m_bootEntriesViewCombo);
    QVERIFY(window.m_bootEntriesScopeLabel);
    QVERIFY(window.m_bootEntriesButtonLayout);

    // The heading is the first child of the scroll content's vertical layout;
    // both the combo and the scope label are items of that grid.
    QWidget *content = window.m_bootEntriesScopeLabel->parentWidget();
    QVERIFY(content);
    QVBoxLayout *contentLayout = qobject_cast<QVBoxLayout *>(content->layout());
    QVERIFY(contentLayout);
    QVERIFY(contentLayout->itemAt(0));
    QGridLayout *headingGrid = qobject_cast<QGridLayout *>(contentLayout->itemAt(0)->layout());
    QVERIFY(headingGrid);

    const int comboIndex = headingGrid->indexOf(window.m_bootEntriesViewCombo);
    const int scopeIndex = headingGrid->indexOf(window.m_bootEntriesScopeLabel);
    QVERIFY2(comboIndex >= 0, "the view selector must be an item of the header grid");
    QVERIFY2(scopeIndex >= 0, "the scope label must be an item of the header grid");

    int comboRow = -1, comboCol = -1, comboRowSpan = 0, comboColSpan = 0;
    int scopeRow = -1, scopeCol = -1, scopeRowSpan = 0, scopeColSpan = 0;
    headingGrid->getItemPosition(comboIndex, &comboRow, &comboCol, &comboRowSpan, &comboColSpan);
    headingGrid->getItemPosition(scopeIndex, &scopeRow, &scopeCol, &scopeRowSpan, &scopeColSpan);
    QVERIFY2(comboCol > scopeCol,
             "the view selector must sit to the right of the scope label");

    // The button row is a wrapping flow layout (height-for-width), not a
    // QBoxLayout, and every action button is present in it.
    QVERIFY2(window.m_bootEntriesButtonLayout->hasHeightForWidth(),
             "the action row must be a wrapping (height-for-width) layout");
    QVERIFY2(!qobject_cast<QBoxLayout *>(window.m_bootEntriesButtonLayout),
             "the action row must not be a non-reflowing QBoxLayout");
    QVERIFY(window.m_bootEntriesButtonLayout->indexOf(window.m_bootEntriesLoadButton) >= 0);
    QVERIFY(window.m_bootEntriesButtonLayout->indexOf(window.m_bootEntriesMoveUpButton) >= 0);
    QVERIFY(window.m_bootEntriesButtonLayout->indexOf(window.m_bootEntriesMoveDownButton) >= 0);
    QVERIFY(window.m_bootEntriesButtonLayout->indexOf(window.m_bootEntriesSaveOrderButton) >= 0);
    QVERIFY(window.m_bootEntriesButtonLayout->indexOf(window.m_bootEntriesDeleteButton) >= 0);
    QVERIFY(window.m_bootEntriesButtonLayout->indexOf(window.m_bootEntriesRestoreButton) >= 0);
}
