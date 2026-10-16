// Part of the boot-repair-ui-tests read-only regression suite: target scope, snapshots, session logs and capability gating.
// The class declaration lives in MainWindowUiTest.h and the shared
// fixtures/helpers in UiTestHelpers.h; this file holds the target scope, snapshots, session logs and capability gating test
// implementations so the suite compiles as parallel translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

#include <QTranslator>
#include <functional>

using namespace UiTestDetail;

void MainWindowUiTest::blankAndDataTargetsSelectableWhileOpticalAndLuksStayGated()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_uiTestPrivilegedSessionGranted = false;

    // A blank virtio data disk (no filesystem at all) is a valid inspection
    // target: it must enable Select Target and advertise itself as eligible.
    DeviceNode blank;
    blank.path = QStringLiteral("/dev/test-blank-vdb");
    blank.type = QStringLiteral("disk");
    blank.model = QStringLiteral("Test blank disk");
    blank.transport = QStringLiteral("virtio");
    blank.sizeBytes = 20ULL * 1024ULL * 1024ULL * 1024ULL;
    window.m_deviceIndex.insert(blank.path, blank);

    window.showDeviceDetails(blank, true, &blank);
    QVERIFY(window.m_setTargetButton);
    QVERIFY2(window.m_setTargetButton->isEnabled(),
             qPrintable(QStringLiteral("Select Target must be enabled for a blank data disk "
                                       "(tooltip: %1)").arg(window.m_setTargetButton->toolTip())));
    QCOMPARE(window.m_detailProtection->text(), QStringLiteral("Eligible repair candidate"));

    // A non-Linux data volume (NTFS) is selectable for inspection as well.
    DeviceNode dataPartition;
    dataPartition.path = QStringLiteral("/dev/test-data-disk1");
    dataPartition.type = QStringLiteral("part");
    dataPartition.fileSystem = QStringLiteral("ntfs");
    DeviceNode dataDisk;
    dataDisk.path = QStringLiteral("/dev/test-data-disk");
    dataDisk.type = QStringLiteral("disk");
    dataDisk.model = QStringLiteral("Test data disk");
    dataDisk.children.append(dataPartition);
    window.m_deviceIndex.insert(dataDisk.path, dataDisk);
    window.m_deviceIndex.insert(dataPartition.path, dataPartition);
    window.showDeviceDetails(dataDisk, true, &dataDisk);
    QVERIFY(window.m_setTargetButton->isEnabled());
    QCOMPARE(window.m_detailProtection->text(), QStringLiteral("Eligible repair candidate"));

    // Committing the blank disk through the real handler stores the physical
    // path as the component (there is nothing else to resolve) and still
    // refuses repair until a Linux root filesystem appears.
    selectTopLevelDisk(window, blank.path);
    QVERIFY(window.m_setTargetButton->isEnabled());
    window.m_setTargetButton->click();
    QCOMPARE(window.m_previewTargetPath, blank.path);
    QCOMPARE(window.m_previewTargetComponentPath, blank.path);
    QVERIFY2(window.m_systemTargetLabel->text().contains(blank.path),
             qPrintable(QStringLiteral("the committed-target line must name the blank drive: %1")
                            .arg(window.m_systemTargetLabel->text())));
    QString readyReason;
    QVERIFY2(!window.repairTargetReady(&readyReason),
             "a blank target must not satisfy the repair readiness gate");
    QVERIFY2(readyReason.contains(QStringLiteral("No mountable Linux root filesystem")),
             qPrintable(QStringLiteral("unexpected readiness reason: %1").arg(readyReason)));
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();

    // A locked encrypted data volume must remain unlock-only until a Linux
    // root is visible. It must never be committed as an unusable target.
    DeviceNode encrypted;
    encrypted.path = QStringLiteral("/dev/test-data-disk2p1");
    encrypted.type = QStringLiteral("crypt");
    encrypted.fileSystem = QStringLiteral("crypto_LUKS");
    encrypted.encrypted = true;
    DeviceNode encryptedDisk;
    encryptedDisk.path = QStringLiteral("/dev/test-data-disk2");
    encryptedDisk.type = QStringLiteral("disk");
    encryptedDisk.children.append(encrypted);
    window.m_deviceIndex.insert(encryptedDisk.path, encryptedDisk);
    window.m_deviceIndex.insert(encrypted.path, encrypted);
    window.showDeviceDetails(encryptedDisk, true, &encryptedDisk);
    QVERIFY(!window.m_setTargetButton->isEnabled());
    QVERIFY(window.m_setTargetButton->toolTip().contains(QStringLiteral("Unlock")));
    QCOMPARE(window.m_detailProtection->text(), QStringLiteral("Unlock required before selection"));

    // Live/installer media (sr0-style rom carrying iso9660) is never a repair
    // target and must not claim eligibility.
    DeviceNode liveMedia;
    liveMedia.path = QStringLiteral("/dev/test-sr0");
    liveMedia.type = QStringLiteral("rom");
    liveMedia.fileSystem = QStringLiteral("iso9660");
    liveMedia.removable = true;
    liveMedia.readOnly = true;
    liveMedia.model = QStringLiteral("QEMU DVD-ROM");
    window.m_deviceIndex.insert(liveMedia.path, liveMedia);
    window.showDeviceDetails(liveMedia, true, &liveMedia);
    QVERIFY2(!window.m_setTargetButton->isEnabled(),
             "live/installer media must not expose Select Target");
    QVERIFY2(window.m_setTargetButton->toolTip().contains(QStringLiteral("Live / installer media")),
             qPrintable(QStringLiteral("unexpected tooltip: %1").arg(window.m_setTargetButton->toolTip())));
    QCOMPARE(window.m_detailProtection->text(), QStringLiteral("Live / installer media — not selectable"));

    // The protected running host stays protected and non-selectable.
    DeviceNode hostRoot;
    hostRoot.path = QStringLiteral("/dev/test-host1");
    hostRoot.type = QStringLiteral("part");
    hostRoot.fileSystem = QStringLiteral("ext4");
    hostRoot.linuxCapableFileSystem = true;
    hostRoot.installedLinux = true;
    hostRoot.protectedDevice = true;
    DeviceNode hostDisk;
    hostDisk.path = QStringLiteral("/dev/test-host");
    hostDisk.type = QStringLiteral("disk");
    hostDisk.protectedDevice = true;
    hostDisk.children.append(hostRoot);
    window.m_deviceIndex.insert(hostDisk.path, hostDisk);
    window.m_deviceIndex.insert(hostRoot.path, hostRoot);
    window.showDeviceDetails(hostDisk, true, &hostDisk);
    QVERIFY(!window.m_setTargetButton->isEnabled());
    QVERIFY(window.m_setTargetButton->toolTip().contains(QStringLiteral("protected running host")));
    QCOMPARE(window.m_detailProtection->text(),
             QStringLiteral("PROTECTED — running system; read-only details only"));
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
}

// Regression for the LUKS unlock being routed through the shared privileged
// session: it must reuse the already-granted administrator authorization (no
// second Polkit prompt) and, when a different-scope request owns the gate,
// defer (never nested-queue) until the gate clears instead of being superseded.
// Mapper reuse (a second unlock of the same drive is a helper-side no-op) stays
// out of the GUI.
void MainWindowUiTest::unlockRoutesThroughSharedSessionAndQueuesBehindBusyGate()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // A deterministic fake lsblk with a single locked LUKS target (the crypt
    // child carries no unlocked Linux child), so every unlock attempt resolves
    // the same locked encrypted node and the post-unlock refreshDevices() never
    // flips the topology to "already unlocked".
    QTemporaryDir lsblkDir;
    QVERIFY(lsblkDir.isValid());
    const QString lockedJson = QStringLiteral(
        "{\"blockdevices\":["
        "{\"name\":\"vdb\",\"kname\":\"vdb\",\"path\":\"/dev/vdb\",\"type\":\"disk\","
        "\"maj:min\":\"252:16\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Test target\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vdb2\",\"kname\":\"vdb2\",\"path\":\"/dev/vdb2\",\"type\":\"part\","
        "\"maj:min\":\"252:18\",\"size\":\"10695475200\",\"fstype\":\"crypto_LUKS\",\"mountpoints\":[],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vdb\"}]}]}");
    const QString scriptPath = lsblkDir.filePath(QStringLiteral("fake-lsblk.sh"));
    {
        QFile script(scriptPath);
        QVERIFY(script.open(QIODevice::WriteOnly | QIODevice::Text));
        script.write("#!/bin/sh\n");
        script.write("printf '%s' '");
        script.write(lockedJson.toUtf8());
        script.write("'\n");
        script.close();
        QVERIFY(QFile::setPermissions(scriptPath,
                                      QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                          | QFileDevice::ExeOwner));
    }
    qputenv("BOOT_REPAIR_LSBLK", scriptPath.toUtf8());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    // The locked LUKS target must be visible so the unlock path can be driven.
    QVERIFY(window.m_deviceIndex.contains(QStringLiteral("/dev/vdb")));
    QVERIFY(window.m_deviceIndex.contains(QStringLiteral("/dev/vdb2")));

    const QStringList expectedRequest = {
        QStringLiteral("unlock"), QStringLiteral("/dev/vdb"), QStringLiteral("/dev/vdb2")
    };

    // Phase 1: the unlock runs over the shared privileged session and writes
    // the unlock request there (authentication carries forward, no dedicated
    // pkexec process).
    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("session.capture"));
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));
    QVERIFY(window.m_privilegedSessionReady);

    selectTopLevelDisk(window, QStringLiteral("/dev/vdb"));
    fillUnlockPassphrase(&window, QStringLiteral("correct horse battery staple"));
    window.unlockSelectedTarget();

    QCOMPARE(capturedHelperArguments(capturePath), expectedRequest);

    // Phase 2: while a different-scope request owns the gate the unlock must
    // defer behind it (not be superseded and not nested-queue) and then proceed
    // once the gate clears.
    QTemporaryDir queueCaptureDir;
    QVERIFY(queueCaptureDir.isValid());
    const QString queueCapturePath = queueCaptureDir.filePath(QStringLiteral("session.capture"));
    QVERIFY(startFakePrivilegedSession(window, queueCapturePath, QStringLiteral("success")));
    QVERIFY(window.m_privilegedSessionReady);

    window.m_privilegedOperationActive = true;
    window.m_privilegedOperationScopeKey = QStringLiteral("/dev/other-disk");
    window.m_privilegedOperationTitle = QStringLiteral("host diagnostic");

    // Clear the gate shortly after the unlock starts waiting, so the unlock is
    // exercised through its deferred path and then proceeds.
    QTimer::singleShot(300, &window, [&window] {
        window.m_privilegedOperationActive = false;
        window.m_privilegedOperationScopeKey.clear();
        window.m_privilegedOperationTitle.clear();
    });

    selectTopLevelDisk(window, QStringLiteral("/dev/vdb"));
    fillUnlockPassphrase(&window, QStringLiteral("correct horse battery staple"));
    window.unlockSelectedTarget();

    // The unlock re-enters after the gate clears, so the request reaches the
    // shared session (rather than being superseded) without the caller blocking
    // on a nested queue wait. Wait for the deferred unlock to complete.
    QTRY_COMPARE_WITH_TIMEOUT(capturedHelperArguments(queueCapturePath), expectedRequest, 5000);
    const QString actionLog = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(actionLog.contains(QStringLiteral("is waiting for the running")),
             "the unlock must defer behind the running host diagnostic instead of being superseded");

    qunsetenv("BOOT_REPAIR_LSBLK");
}

void MainWindowUiTest::scopeSwitchingRejectsStaleTargetEvidence()
{
    // The harness shares one settings file for the whole process; simulate the
    // first run (no persisted stage selections) so the no-evidence cases below
    // are deterministic: the Settings checkboxes stay disabled until evidence
    // exists, while the tool/plan/Run gates fail closed regardless.
    QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    for (const char *key : {"repair/filesystem", "repair/dpkgConfigure", "repair/fixBroken",
                            "repair/kernelCleanup", "repair/refreshMetadata", "repair/upgradePackages",
                            "repair/dkms", "repair/displayManager", "repair/gpuDriver", "repair/initramfs",
                            "repair/efiBootloader", "repair/grub", "repair/extlinux"}) {
        persist.remove(QString::fromLatin1(key));
    }
    persist.sync();

    MainWindow window;
    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidence(true, false));
    prepareRepairScope(window, true);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.m_fullRepairDkms->setChecked(true);
    auto *item = repairItem(window, QStringLiteral("dkms"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.updateFullRepairSummary();
    QVERIFY(window.m_fullRepairDkms->isEnabled());
    QVERIFY(window.m_repairToolButton->isEnabled());
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dkms")));

    window.m_hostMaintenanceMode = false;
    window.updateFullRepairSummary();
    QVERIFY(!window.m_fullRepairDkms->isEnabled());
    QVERIFY(!window.m_fullRepairDkms->isChecked());
    QVERIFY(!window.m_fullRepairDkms->isCheckable());
    QVERIFY(!window.m_repairToolButton->isEnabled());
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dkms")));
    QVERIFY(item->text(1).contains(QStringLiteral("DKMS is not installed")));

    window.m_hostMaintenanceMode = true;
    window.updateFullRepairSummary();
    QVERIFY(window.m_repairToolButton->isEnabled());
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dkms")));

    window.m_hostMaintenanceMode = false;
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.m_previewTargetPath = QStringLiteral("/dev/another-target");
    QString reason;
    QVERIFY(!window.repairToolAvailable(QStringLiteral("dkms"), &reason));
    QVERIFY(reason.contains(QStringLiteral("different target")));
    QVERIFY(window.selectedRepairStages().isEmpty());
    window.updateFullRepairSummary();
    QVERIFY(!window.m_fullRepairDkms->isEnabled());
    QVERIFY(!window.m_repairToolButton->isEnabled());
    QVERIFY(!window.m_runFullRepairButton->isEnabled());
}

void MainWindowUiTest::scopeSwitchAlwaysRegeneratesDiagnostics()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    window.m_privilegedSessionReady = true;
    window.m_evidenceRefreshDelayMs = kEvidenceRefreshTestDelayMs;

    // Resolve the protected running host and park a committed repair target,
    // then return to ordinary target mode so the round-trip can start there.
    prepareRepairScope(window, /*host=*/false);
    prepareRepairScope(window, /*host=*/true);
    window.m_hostMaintenanceMode = false;

    // Seed both scopes with FRESH evidence (non-empty report, matching target
    // identity) so the freshness gate alone would suppress the regeneration on
    // the pre-fix code path.
    window.m_targetDiagnosticCache.insert(
        QStringLiteral("report"), QStringLiteral("Diagnostic: report\nFROZEN-TARGET-EVIDENCE\n"));
    window.m_hostDiagnosticCache.insert(
        QStringLiteral("report"), QStringLiteral("Diagnostic: report\nFROZEN-HOST-EVIDENCE\n"));

    // target → host: entering Host Maintenance must schedule a forced
    // regeneration even though the running-host cache is still fresh.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_evidenceRefreshTimer);
    QVERIFY2(window.m_evidenceRefreshTimer->isActive(),
             "entering Host Maintenance must schedule automatic diagnostics regeneration "
             "even when the running-host cache is fresh");
    QTRY_VERIFY_WITH_TIMEOUT(!window.m_evidenceRefreshTimer->isActive(), 3000);
    QVERIFY2(!window.m_hostDiagnosticCache.value(QStringLiteral("report"))
                  .contains(QStringLiteral("FROZEN-HOST-EVIDENCE")),
             "the host direction must regenerate diagnostics instead of reusing the fresh cache");

    // host → target: leaving Host Maintenance must force the restored target's
    // regeneration too (the parked target cache is still fresh).
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    QVERIFY(window.m_evidenceRefreshTimer);
    QVERIFY2(window.m_evidenceRefreshTimer->isActive(),
             "leaving Host Maintenance must schedule automatic diagnostics regeneration "
             "for the restored target even when its cache is fresh");
    QTRY_VERIFY_WITH_TIMEOUT(!window.m_evidenceRefreshTimer->isActive(), 3000);
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("report"))
                  .contains(QStringLiteral("FROZEN-TARGET-EVIDENCE")),
             "the target direction must regenerate diagnostics instead of reusing the fresh cache");
}

// The scope-transition announcement must be tagged with the NEW scope's
// [SCOPE] marker: updateSessionScope() runs BEFORE the transition appendLog()
// in selectHostForMaintenance()/exitHostMaintenanceMode(), so the notice lands
// in the new scope's block, never the outgoing scope's. The session file is
// append-only in chronological order, so asserting the marker precedes the
// announcement proves the ordering.
void MainWindowUiTest::scopeTransitionAnnouncementTaggedWithNewScope()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    auto sessionLog = [](MainWindow &window) {
        QFile file(window.m_sessionLogPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString();
        }
        return QString::fromUtf8(file.readAll());
    };

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

    // target → host: the Running Host [SCOPE] marker must precede the
    // host-maintenance announcement.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    {
        const QString log = sessionLog(window);
        const int marker = log.indexOf(QStringLiteral("[SCOPE] Running Host"));
        const int notice = log.indexOf(QStringLiteral("Running host selected for explicit maintenance"));
        QVERIFY2(marker >= 0, "the Running Host [SCOPE] marker must be written");
        QVERIFY2(notice >= 0, "the host-maintenance announcement must be written");
        QVERIFY2(marker < notice,
                 "the [SCOPE] marker must precede the host-maintenance announcement");
    }

    // host → target: the Repair Target [SCOPE] marker must precede the
    // exit announcement.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    {
        const QString log = sessionLog(window);
        const int marker = log.indexOf(QStringLiteral("[SCOPE] Repair Target"));
        const int notice = log.indexOf(QStringLiteral("Running-host maintenance deselected"));
        QVERIFY2(marker >= 0, "the Repair Target [SCOPE] marker must be written");
        QVERIFY2(notice >= 0, "the exit announcement must be written");
        QVERIFY2(marker < notice,
                 "the [SCOPE] marker must precede the exit announcement");
    }
}

void MainWindowUiTest::missingCapabilityEvidenceFailsClosed()
{
    // Simulate the first run (no persisted stage selections) so the checkbox
    // presentation is deterministic: with no persisted selections and no
    // capability evidence the Settings checkboxes stay disabled while the
    // tool/plan/Run gates fail closed.
    QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    for (const char *key : {"repair/filesystem", "repair/dpkgConfigure", "repair/fixBroken",
                            "repair/kernelCleanup", "repair/refreshMetadata", "repair/upgradePackages",
                            "repair/dkms", "repair/displayManager", "repair/gpuDriver", "repair/initramfs",
                            "repair/efiBootloader", "repair/grub", "repair/extlinux"}) {
        persist.remove(QString::fromLatin1(key));
    }
    persist.sync();

    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, QStringLiteral("PASS\nDistribution family: debian\n"));
        window.m_fullRepairDpkg->setChecked(true);
        window.updateFullRepairSummary();
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        QVERIFY(window.selectedRepairStages().isEmpty());
        QVERIFY(!window.m_runFullRepairButton->isEnabled());
        for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
            auto *item = window.m_repairToolTree->topLevelItem(row);
            const QString key = item->data(0, Qt::UserRole).toString();
            QString reason;
            QVERIFY(!window.repairToolAvailable(key, &reason));
            QVERIFY(!reason.isEmpty());
            QVERIFY(!window.repairEvidenceReadyForTool(key));
            window.m_repairToolTree->setCurrentItem(item);
            window.updateRepairToolDetails();
            QVERIFY(!window.m_repairToolButton->isEnabled());
            QVERIFY(item->text(1).startsWith(QStringLiteral("Unavailable:")));
        }
        cacheRepairEvidence(window, QStringLiteral("Repair tool unknown: available\nRepair tool dpkg: maybe\n"));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("unknown")));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("dpkg")));
        cacheRepairEvidence(window, QStringLiteral("Repair tool dpkg: available\nRepair tool dpkg: unavailable|Conflict\n"));
        QString reason;
        QVERIFY(!window.repairToolAvailable(QStringLiteral("dpkg"), &reason));
        QCOMPARE(reason, QStringLiteral("Conflict"));
    }
}

// Settings → Full Repair plan: a stage whose capability gate is unavailable on
// the current scope must be shown disabled AND unchecked (the tool cannot
// run), while available stages keep the saved user preference and the built
// plan excludes the unavailable keys. Re-availabling a stage restores the
// saved preference instead of the forced-off display state.
void MainWindowUiTest::unavailablePlanStagesAreDisabledAndUnchecked()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // Establish the saved preferences through a scope where every involved
    // stage is available first, exactly like a user on a Debian target:
    // dpkg/aptupdate/dkms/grub on, initramfs off.
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.updateFullRepairSummary();
    window.m_fullRepairDpkg->setChecked(true);
    window.m_fullRepairAptUpdate->setChecked(true);
    window.m_fullRepairDkms->setChecked(true);
    window.m_fullRepairGrub->setChecked(true);
    window.m_fullRepairInitramfs->setChecked(false);
    window.updateFullRepairSummary();
    QVERIFY2(window.m_fullRepairDkms->isChecked(),
             "the saved dkms preference must be on in the available scope");
    QVERIFY2(window.m_fullRepairGrub->isChecked(),
             "the saved grub preference must be on in the available scope");

    // The scope's diagnostics now mark dkms and grub unavailable; dpkg and
    // aptupdate stay available for the saved-preference assertions.
    QString evidence = capabilityEvidence(false, false);
    evidence.replace(QStringLiteral("Repair tool grub: available"),
                     QStringLiteral("Repair tool grub: unavailable|grub-mkconfig is not installed"));
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();

    QVERIFY2(window.repairToolAvailable(QStringLiteral("dpkg")), "dpkg must be available");
    QVERIFY2(window.repairToolAvailable(QStringLiteral("aptupdate")), "aptupdate must be available");
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("dkms")), "dkms must be unavailable");
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("grub")), "grub must be unavailable");

    // Available stages keep the saved preference, checked and unchecked alike.
    QVERIFY(window.m_fullRepairDpkg->isEnabled());
    QVERIFY(window.m_fullRepairDpkg->isCheckable());
    QVERIFY(window.m_fullRepairDpkg->isChecked());
    QVERIFY(window.m_fullRepairAptUpdate->isEnabled());
    QVERIFY(window.m_fullRepairAptUpdate->isCheckable());
    QVERIFY(window.m_fullRepairAptUpdate->isChecked());
    QVERIFY(window.m_fullRepairInitramfs->isEnabled());
    QVERIFY(window.m_fullRepairInitramfs->isCheckable());
    QVERIFY(!window.m_fullRepairInitramfs->isChecked());

    // Unavailable stages are disabled AND unchecked AND not checkable, with
    // the helper's reason on the tooltip.
    QVERIFY2(!window.m_fullRepairDkms->isEnabled(),
             "an unavailable dkms stage must be disabled");
    QVERIFY2(!window.m_fullRepairDkms->isChecked(),
             "an unavailable dkms stage must not stay checked");
    QVERIFY2(!window.m_fullRepairDkms->isCheckable(),
             "an unavailable dkms stage must not be checkable");
    QVERIFY2(window.m_fullRepairDkms->toolTip().contains(QStringLiteral("DKMS is not installed")),
             "the disabled stage must keep the helper's reason in its tooltip");
    QVERIFY2(!window.m_fullRepairGrub->isEnabled(),
             "an unavailable grub stage must be disabled");
    QVERIFY2(!window.m_fullRepairGrub->isChecked(),
             "an unavailable grub stage must not stay checked");
    QVERIFY2(!window.m_fullRepairGrub->isCheckable(),
             "an unavailable grub stage must not be checkable");
    QVERIFY2(window.m_fullRepairGrub->toolTip().contains(QStringLiteral("grub-mkconfig")),
             "the disabled stage must keep the helper's reason in its tooltip");

    // The built plan excludes the unavailable keys even though their saved
    // preferences are on, and neither title appears in the plan list.
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dkms")));
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("grub")));
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dpkg-configure")));
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("apt-update")));
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("initramfs")));
    const QStringList planStageTexts = fullRepairPlanStageTexts(window);
    for (const QString &text : planStageTexts) {
        QVERIFY2(!text.contains(QStringLiteral("DKMS")),
                 "an unavailable stage must not appear in the plan list");
        QVERIFY2(!text.contains(QStringLiteral("GRUB")),
                 "an unavailable stage must not appear in the plan list");
    }

    // The forced-off display never clobbers the saved preference: when the
    // tools become available again the checkboxes come back checked, and the
    // user's explicit off preference (initramfs) stays off.
    evidence.replace(QStringLiteral("Repair tool dkms: unavailable|DKMS is not installed"),
                     QStringLiteral("Repair tool dkms: available"));
    evidence.replace(QStringLiteral("Repair tool grub: unavailable|grub-mkconfig is not installed"),
                     QStringLiteral("Repair tool grub: available"));
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();
    QVERIFY(window.m_fullRepairDkms->isEnabled());
    QVERIFY(window.m_fullRepairDkms->isCheckable());
    QVERIFY2(window.m_fullRepairDkms->isChecked(),
             "a re-available stage must restore the saved user preference");
    QVERIFY(window.m_fullRepairGrub->isEnabled());
    QVERIFY(window.m_fullRepairGrub->isCheckable());
    QVERIFY2(window.m_fullRepairGrub->isChecked(),
             "a re-available stage must restore the saved user preference");
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dkms")));
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("grub")));
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("initramfs")));
}

// First run of a fresh install: no persisted stage selections and no
// capability evidence yet. Every Settings → Full Repair checkbox stays
// disabled (greyed) until diagnostics exist, exactly the historical behavior;
// the gate opens once evidence arrives.
void MainWindowUiTest::settingsStageCheckboxesFirstRunStayDisabledUntilEvidence()
{
    // Simulate the first run: no persisted stage selections.
    QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    for (const char *key : {"repair/filesystem", "repair/dpkgConfigure", "repair/fixBroken",
                            "repair/kernelCleanup", "repair/refreshMetadata", "repair/upgradePackages",
                            "repair/dkms", "repair/displayManager", "repair/gpuDriver", "repair/initramfs",
                            "repair/efiBootloader", "repair/grub", "repair/extlinux"}) {
        persist.remove(QString::fromLatin1(key));
    }
    persist.sync();

    MainWindow window;
    prepareRepairScope(window);
    window.updateFullRepairSummary();
    for (const auto &stage : window.fullRepairStageCheckboxes()) {
        QVERIFY(stage.first);
        QVERIFY2(!stage.first->isEnabled(),
                 qPrintable(QStringLiteral("first-run stage %1 must stay disabled without evidence")
                                .arg(stage.second)));
        QVERIFY2(!stage.first->isChecked(),
                 qPrintable(QStringLiteral("first-run stage %1 must stay unchecked without evidence")
                                .arg(stage.second)));
        QVERIFY2(!stage.first->isCheckable(),
                 qPrintable(QStringLiteral("first-run stage %1 must not be checkable without evidence")
                                .arg(stage.second)));
    }
    QVERIFY(window.selectedRepairStages().isEmpty());
    QVERIFY(!window.m_runFullRepairButton->isEnabled());

    // Evidence arrives: every stage the evidence reports available becomes
    // enabled (default-on checked, default-off unchecked); the display stage
    // stays disabled because the evidence explicitly reports it unavailable.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    evidence += QStringLiteral("Repair tool extlinux: available\n");
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();
    for (const auto &stage : window.fullRepairStageCheckboxes()) {
        if (stage.second == QStringLiteral("display")) {
            QVERIFY2(!stage.first->isEnabled(),
                     "an explicitly unavailable stage must stay disabled");
            QVERIFY2(!stage.first->isChecked(),
                     "an explicitly unavailable stage must stay unchecked");
            QVERIFY2(stage.first->toolTip().contains(QStringLiteral("No display manager")),
                     "the disabled stage must carry the evidence reason");
            continue;
        }
        QVERIFY2(stage.first->isEnabled(),
                 qPrintable(QStringLiteral("stage %1 must enable once evidence exists")
                                .arg(stage.second)));
    }
    QVERIFY(window.m_fullRepairFilesystem->isChecked());
    QVERIFY(window.m_fullRepairDpkg->isChecked());
    QVERIFY(window.m_fullRepairBrokenPackages->isChecked());
    QVERIFY(window.m_fullRepairAptUpdate->isChecked());
    QVERIFY(window.m_fullRepairDkms->isChecked());
    QVERIFY(window.m_fullRepairInitramfs->isChecked());
    QVERIFY(window.m_fullRepairGrub->isChecked());
    QVERIFY(window.m_fullRepairExtlinux->isChecked());
    QVERIFY(!window.m_fullRepairUpgrade->isChecked());
    QVERIFY(!window.m_fullRepairDisplayManager->isChecked());
    QVERIFY(!window.m_fullRepairEfi->isChecked());
}

// Regression: a stage selection made during a run must survive a relaunch even
// when the window never reached closeEvent() (so saveSettings() was not called
// to write every stage key). The toggle handler persists the selection eagerly;
// without that, a fresh selection was only held in memory, and on the next
// launch the no-evidence first-launch branch greyed out the stage until
// diagnostics ran again.
void MainWindowUiTest::settingsStageSelectionSurvivesRelaunchWithoutCleanClose()
{
    QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    for (const char *key : {"repair/filesystem", "repair/dpkgConfigure", "repair/fixBroken",
                            "repair/kernelCleanup", "repair/refreshMetadata", "repair/upgradePackages",
                            "repair/dkms", "repair/displayManager", "repair/gpuDriver", "repair/initramfs",
                            "repair/efiBootloader", "repair/grub", "repair/extlinux"}) {
        persist.remove(QString::fromLatin1(key));
    }
    persist.sync();

    {
        // First run: extlinux available, the user turns it off, and the window
        // is torn down without a clean close (no saveSettings()).
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Repair tool filesystem: available\n");
        evidence += QStringLiteral("Repair tool extlinux: available\n");
        cacheRepairEvidence(window, evidence);
        window.updateFullRepairSummary();
        QVERIFY(window.m_fullRepairExtlinux->isEnabled());
        QVERIFY(window.m_fullRepairExtlinux->isChecked());
        window.m_fullRepairExtlinux->setChecked(false);
    }
    {
        // Relaunch before any diagnostics: the saved selection must stay
        // visible, editable and unchecked — never greyed out on the
        // no-evidence path.
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window);
        window.updateFullRepairSummary();
        QVERIFY2(window.m_fullRepairExtlinux->isEnabled(),
                 "a toggled stage must stay enabled on relaunch before diagnostics run");
        QVERIFY(window.m_fullRepairExtlinux->isCheckable());
        QVERIFY2(!window.m_fullRepairExtlinux->isChecked(),
                 "the persisted off selection must survive the relaunch");
        // Fail-closed gating is unchanged: no evidence, no executable plan.
        QVERIFY(window.selectedRepairStages().isEmpty());
        QVERIFY(!window.m_runFullRepairButton->isEnabled());
    }
}

// Regression: switching to Host Maintenance on the Systems tab right after a
// successful Full Repair (and a fresh install with empty persisted settings)
// used to crash. The switch path re-runs the device scan and the full Repair/
// Settings/Diagnostics refresh while the plan-owned progress dialog is still
// open, and the first-run (no persisted stage selections) state exercises the
// NoEvidence branch of updateRepairScopeControls().
void MainWindowUiTest::hostMaintenanceSwitchAfterFullRepairDoesNotCrash()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // Simulate the first run: no persisted stage selections.
    QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    for (const char *key : {"repair/filesystem", "repair/dpkgConfigure", "repair/fixBroken",
                            "repair/kernelCleanup", "repair/refreshMetadata", "repair/upgradePackages",
                            "repair/dkms", "repair/displayManager", "repair/gpuDriver", "repair/initramfs",
                            "repair/efiBootloader", "repair/grub", "repair/extlinux"}) {
        persist.remove(QString::fromLatin1(key));
    }
    persist.sync();

    // A deterministic fake lsblk: one protected host disk backing "/" and one
    // selectable repair target, so the device re-scan performed by the Full
    // Repair completion and the Host Maintenance switch never depends on the
    // test host's real block devices.
    QTemporaryDir lsblkDir;
    QVERIFY(lsblkDir.isValid());
    const QString json = QStringLiteral(
        "{\"blockdevices\":["
        "{\"name\":\"vda\",\"kname\":\"vda\",\"path\":\"/dev/vda\",\"type\":\"disk\","
        "\"maj:min\":\"252:0\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Host disk\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vda2\",\"kname\":\"vda2\",\"path\":\"/dev/vda2\",\"type\":\"part\","
        "\"maj:min\":\"252:2\",\"size\":\"10695475200\",\"fstype\":\"ext4\",\"mountpoints\":[\"/\"],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vda\"}]},"
        "{\"name\":\"vdb\",\"kname\":\"vdb\",\"path\":\"/dev/vdb\",\"type\":\"disk\","
        "\"maj:min\":\"252:16\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Test target\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vdb2\",\"kname\":\"vdb2\",\"path\":\"/dev/vdb2\",\"type\":\"part\","
        "\"maj:min\":\"252:18\",\"size\":\"10695475200\",\"fstype\":\"ext4\",\"mountpoints\":[],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vdb\"}]}]}");
    const QString scriptPath = lsblkDir.filePath(QStringLiteral("fake-lsblk.sh"));
    {
        QFile script(scriptPath);
        QVERIFY(script.open(QIODevice::WriteOnly | QIODevice::Text));
        script.write("#!/bin/sh\nprintf '%s' '");
        script.write(json.toUtf8());
        script.write("'\n");
        script.close();
        QVERIFY(QFile::setPermissions(scriptPath,
                                      QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                          | QFileDevice::ExeOwner));
    }
    qputenv("BOOT_REPAIR_LSBLK", scriptPath.toUtf8());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    // The fake scan resolved the protected running host.
    QCOMPARE(window.m_hostPrimaryPath, QStringLiteral("/dev/vda"));
    QCOMPARE(window.m_hostPrimaryComponentPath, QStringLiteral("/dev/vda2"));

    // Commit a repair target and cache the full diagnostic evidence.
    window.m_previewTargetPath = QStringLiteral("/dev/vdb");
    window.m_previewTargetComponentPath = QStringLiteral("/dev/vdb2");
    window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.updateFullRepairSummary();

    // A filesystem-free plan (grub only) so the Full Repair reaches the helper
    // directly without the interactive per-device repair flow.
    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairExtlinux,
                              window.m_fullRepairFilesystem}) {
        if (toggle) {
            toggle->setChecked(false);
        }
    }
    window.m_fullRepairGrub->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("grub")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                    {QStringLiteral("grub=changed")}),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    window.runFullRepair();
    // Drain the plan completion and its device re-scan.
    QTRY_VERIFY_WITH_TIMEOUT(!window.m_fullRepairPlanInProgress, 8000);
    QVERIFY(window.m_activeBusyOperations.isEmpty());

    // The Full Repair re-scanned devices; the protected host must still be
    // resolved so the Host Maintenance switch can commit the running host.
    QVERIFY(window.m_deviceIndex.contains(window.m_hostPrimaryPath));

    // The switch itself: this is where the crash occurred.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath.isEmpty());
    QVERIFY(window.m_previewTargetComponentPath.isEmpty());

    // Leaving maintenance through the single shared exit path must not crash
    // either (the device re-scan from Full Repair completion rebuilt the tree
    // from the fake lsblk, so no committed target remains to restore here).
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);

    qunsetenv("BOOT_REPAIR_LSBLK");
}

// Regression for the resize + Host-Maintenance crash reported on TUXEDO OS:
// after unlocking/selecting a repair target, resizing the window (which repaints
// the ElidedLabel Selected-tool title and re-applies the responsive splitter
// sizes/tab labels), returning to the Systems tab and then selecting Host
// Maintenance used to segfault. The earlier defensive guards in
// updateRepairScopeControls()/updateRepairToolDetails() did not fix it, so this
// test drives the full gesture chain headlessly.
//
// The crash is deterministic and locale-independent: it reproduces in English,
// so no translator is loaded here. The faithful chain is: scan a locked LUKS
// target, "unlock" it (the lsblk topology gains the crypt mapper child) and
// re-scan, commit the target, move to the Repair tab, resize the window across
// the responsive splitter breakpoints, return to the Systems tab and click the
// Host Maintenance button, then drain the event loop so the deferred snapshot
// preload and repaint paths run.
void MainWindowUiTest::hostMaintenanceSwitchAfterResizeDoesNotCrash()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // Simulate the first run: no persisted stage selections.
    QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    for (const char *key : {"repair/filesystem", "repair/dpkgConfigure", "repair/fixBroken",
                            "repair/kernelCleanup", "repair/refreshMetadata", "repair/upgradePackages",
                            "repair/dkms", "repair/displayManager", "repair/gpuDriver", "repair/initramfs",
                            "repair/efiBootloader", "repair/grub", "repair/extlinux"}) {
        persist.remove(QString::fromLatin1(key));
    }
    persist.sync();

    // A deterministic fake lsblk with two topologies, selected by a marker file:
    // before "unlock" the target is a locked LUKS container; after "unlock" a
    // crypt mapper child carrying the ext4 Linux root appears. This lets the
    // test exercise the real unlock -> refreshDevices() -> select-target chain.
    QTemporaryDir lsblkDir;
    QVERIFY(lsblkDir.isValid());
    const QString markerPath = lsblkDir.filePath(QStringLiteral("unlocked"));
    const QString lockedJson = QStringLiteral(
        "{\"blockdevices\":["
        "{\"name\":\"vda\",\"kname\":\"vda\",\"path\":\"/dev/vda\",\"type\":\"disk\","
        "\"maj:min\":\"252:0\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Host disk\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vda2\",\"kname\":\"vda2\",\"path\":\"/dev/vda2\",\"type\":\"part\","
        "\"maj:min\":\"252:2\",\"size\":\"10695475200\",\"fstype\":\"ext4\",\"mountpoints\":[\"/\"],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vda\"}]},"
        "{\"name\":\"vdb\",\"kname\":\"vdb\",\"path\":\"/dev/vdb\",\"type\":\"disk\","
        "\"maj:min\":\"252:16\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Test target\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vdb2\",\"kname\":\"vdb2\",\"path\":\"/dev/vdb2\",\"type\":\"part\","
        "\"maj:min\":\"252:18\",\"size\":\"10695475200\",\"fstype\":\"crypto_LUKS\",\"mountpoints\":[],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vdb\"}]}]}");
    const QString unlockedJson = QStringLiteral(
        "{\"blockdevices\":["
        "{\"name\":\"vda\",\"kname\":\"vda\",\"path\":\"/dev/vda\",\"type\":\"disk\","
        "\"maj:min\":\"252:0\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Host disk\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vda2\",\"kname\":\"vda2\",\"path\":\"/dev/vda2\",\"type\":\"part\","
        "\"maj:min\":\"252:2\",\"size\":\"10695475200\",\"fstype\":\"ext4\",\"mountpoints\":[\"/\"],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vda\"}]},"
        "{\"name\":\"vdb\",\"kname\":\"vdb\",\"path\":\"/dev/vdb\",\"type\":\"disk\","
        "\"maj:min\":\"252:16\",\"size\":\"10737418240\",\"mountpoints\":[],"
        "\"model\":\"Test target\",\"tran\":\"virtio\",\"rm\":false,\"ro\":false,\"pkname\":\"\","
        "\"children\":["
        "{\"name\":\"vdb2\",\"kname\":\"vdb2\",\"path\":\"/dev/vdb2\",\"type\":\"crypt\","
        "\"maj:min\":\"252:18\",\"size\":\"10695475200\",\"fstype\":\"crypto_LUKS\",\"mountpoints\":[],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vdb\","
        "\"children\":["
        "{\"name\":\"luks-test\",\"kname\":\"dm-0\",\"path\":\"/dev/mapper/luks-test\",\"type\":\"lvm\","
        "\"maj:min\":\"253:0\",\"size\":\"10695475200\",\"fstype\":\"ext4\",\"mountpoints\":[],"
        "\"model\":null,\"tran\":null,\"rm\":false,\"ro\":false,\"pkname\":\"vdb2\"}]}]}]}");
    const QString scriptPath = lsblkDir.filePath(QStringLiteral("fake-lsblk.sh"));
    {
        QFile script(scriptPath);
        QVERIFY(script.open(QIODevice::WriteOnly | QIODevice::Text));
        script.write("#!/bin/sh\n");
        script.write("if [ -f '");
        script.write(markerPath.toUtf8());
        script.write("' ]; then printf '%s' '");
        script.write(unlockedJson.toUtf8());
        script.write("'; else printf '%s' '");
        script.write(lockedJson.toUtf8());
        script.write("'; fi\n");
        script.close();
        QVERIFY(QFile::setPermissions(scriptPath,
                                      QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                          | QFileDevice::ExeOwner));
    }
    qputenv("BOOT_REPAIR_LSBLK", scriptPath.toUtf8());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_uiTestPrivilegedSessionGranted = false;

    // The first scan sees the locked LUKS target.
    QCOMPARE(window.m_hostPrimaryPath, QStringLiteral("/dev/vda"));
    QCOMPARE(window.m_hostPrimaryComponentPath, QStringLiteral("/dev/vda2"));
    QVERIFY(window.m_deviceIndex.contains(QStringLiteral("/dev/vdb2")));
    QVERIFY(!window.m_deviceIndex.contains(QStringLiteral("/dev/mapper/luks-test")));

    // Unlock: the topology gains the crypt mapper child, exactly like the
    // post-unlock refreshDevices() in the real GUI. This rebuilds the device
    // tree while the window is live.
    {
        QFile marker(markerPath);
        QVERIFY(marker.open(QIODevice::WriteOnly | QIODevice::Text));
        marker.write("1");
        marker.close();
    }
    window.refreshDevices();
    QCoreApplication::processEvents();
    QVERIFY(window.m_deviceIndex.contains(QStringLiteral("/dev/mapper/luks-test")));

    // Select the unlocked target.
    QVERIFY(window.m_deviceIndex.contains(QStringLiteral("/dev/vdb")));
    selectTopLevelDisk(window, QStringLiteral("/dev/vdb"));
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/vdb"));
    QCOMPARE(window.m_previewTargetComponentPath, QStringLiteral("/dev/mapper/luks-test"));

    // Move to the Repair tab before resizing, exactly like the user who
    // resized while inspecting the repair tool titles/buttons: the repair
    // splitter and the ElidedLabel Selected-tool title must be laid out and
    // painted while the window changes width.
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    QCOMPARE(window.m_tabs->currentWidget(), window.m_repairPage);
    QTest::qWait(50);
    QCoreApplication::processEvents();
    QVERIFY(window.m_repairToolTree);
    window.m_repairToolTree->setCurrentItem(repairItem(window, QStringLiteral("display")));
    window.updateRepairToolDetails();

    // Resize the window through several widths so the responsive layout
    // re-applies the tab labels and splitter sizes and repaints the
    // ElidedLabel Selected-tool title at narrow and wide breakpoints.
    window.resize(1400, 900);
    QTest::qWait(50);
    QCoreApplication::processEvents();
    window.resize(520, 480);
    QTest::qWait(50);
    QCoreApplication::processEvents();
    window.resize(960, 720);
    QTest::qWait(50);
    QCoreApplication::processEvents();
    window.resize(480, 500);
    QTest::qWait(50);
    QCoreApplication::processEvents();

    // Navigate back to the Systems tab before the maintenance switch.
    QVERIFY(window.m_repairToolTitle);
    QVERIFY(window.m_hostMaintenanceButton);
    QVERIFY(!window.m_hostMaintenanceButton->text().isEmpty());
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_systemsPage));
    QCOMPARE(window.m_tabs->currentWidget(), window.m_systemsPage);
    QTest::qWait(50);
    QCoreApplication::processEvents();

    // The switch itself: this is where the crash occurred. Click the real
    // button and drain the event loop so the deferred snapshot preload and
    // repaint paths run, exactly like the real GUI.
    window.m_hostMaintenanceButton->click();
    QTest::qWait(150);
    QCoreApplication::processEvents();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath.isEmpty());
    QVERIFY(window.m_previewTargetComponentPath.isEmpty());

    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);

    qunsetenv("BOOT_REPAIR_LSBLK");
}

// Cancellation of the scope-commit authorization request must be terminal for
// that request cycle: the failure is published once, the deferred-scope
// affordance is shown, and no second authorization conversation is launched —
// not even by a background caller (a snapshot/diagnostics refresh) that reaches
// ensurePrivilegedSession() in the same cycle. Only the explicit Authorize
// retry starts the next request.
void MainWindowUiTest::privilegedSessionCancellationPublishesFailureAndDefersWithoutRetry()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_uiTestPrivilegedSessionGranted = false;

    prepareRepairScope(window);
    const QString scopeKey = QStringLiteral("target:/dev/test-system");
    QCOMPARE(window.currentPrivilegedScopeKey(), scopeKey);

    // The scope-commit authorization request. In the offscreen seam the
    // request is "cancelled" (grant flag is false) and returns promptly.
    window.requestPrivilegedSessionForScope(scopeKey);
    QTest::qWait(100);
    QCoreApplication::processEvents();

    // One request, exactly once: the cancellation published its failure and
    // the deferred-scope affordance is set (Authorize button visible).
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));
    QVERIFY(window.m_privilegedSessionRequestFailed);
    QCOMPARE(window.m_authorizationDeferredScope, scopeKey);
    QVERIFY(window.m_authorizeNowButton);
    QVERIFY(window.m_authorizeNowButton->isVisible());
    QVERIFY(window.m_authorizeNowButton->isEnabled());

    // A background caller (snapshot/diagnostics refresh) reaching the helper in
    // the same cycle must observe the published failure, not launch a second
    // conversation.
    QString backgroundError;
    QVERIFY(!window.ensurePrivilegedSession(&backgroundError));
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));
    QVERIFY(!backgroundError.isEmpty());

    // The explicit Authorize retry is what starts the next request.
    window.authorizePrivilegedSessionNow();
    QTest::qWait(100);
    QCoreApplication::processEvents();
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(2));
}

// The granted path is unchanged: a successful authorization produces exactly
// one request and reuses the established session for later callers.
void MainWindowUiTest::privilegedSessionGrantProducesSingleRequest()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_uiTestPrivilegedSessionGranted = true;

    prepareRepairScope(window);
    const QString scopeKey = QStringLiteral("target:/dev/test-system");
    QCOMPARE(window.currentPrivilegedScopeKey(), scopeKey);

    window.requestPrivilegedSessionForScope(scopeKey);
    QTest::qWait(100);
    QCoreApplication::processEvents();

    // One request, and the session is established (no deferred state).
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));
    QVERIFY(window.privilegedSessionUsable());
    QVERIFY(window.m_authorizationDeferredScope.isEmpty());

    // A follow-up caller reuses the established session without a new request.
    QString reuseError;
    QVERIFY(window.ensurePrivilegedSession(&reuseError));
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    window.closePrivilegedSession();
}

// The automatic Btrfs snapshot preload is background work: it must never claim
// the busy indicator label, so a scope switch does not flash a spurious "Btrfs"
// label before the diagnostics run owns the indicator.  A manual Load Snapshots
// click still shows the normal Btrfs busy label.
void MainWindowUiTest::backgroundSnapshotPreloadDoesNotClaimBusyLabel()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window, /*host=*/false);
    // A Btrfs scope so the preload reaches the privileged request instead of
    // taking the non-Btrfs informational shortcut.
    window.m_deviceIndex[QStringLiteral("/dev/test-root")].fileSystem = QStringLiteral("btrfs");

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.filePath(QStringLiteral("snapshot-preload.log"));
    QVERIFY2(startMultiDeviceFilesystemFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");

    QSignalSpy busySpy(&window, &MainWindow::busyStateChanged);

    // Background (automatic) preload: no busy emission may carry a Btrfs label.
    window.loadSnapshots(true);
    for (const QList<QVariant> &args : busySpy) {
        if (args.at(0).toBool()) {
            const QString label = args.at(1).toString();
            QVERIFY2(!label.contains(QStringLiteral("Btrfs"), Qt::CaseInsensitive),
                     qPrintable(QStringLiteral("background preload must not claim a Btrfs busy label (got '%1')").arg(label)));
        }
    }

    // Manual Load Snapshots: the Btrfs busy label is present.
    busySpy.clear();
    window.loadSnapshots(false);
    bool sawBtrfsLabel = false;
    for (const QList<QVariant> &args : busySpy) {
        if (args.at(0).toBool()
            && args.at(1).toString().contains(QStringLiteral("Btrfs"), Qt::CaseInsensitive)) {
            sawBtrfsLabel = true;
        }
    }
    QVERIFY2(sawBtrfsLabel, "a manual Load Snapshots must show the Btrfs busy label");
}

// Closing the window while a read-only diagnostic runs must surface the
// transient cancel state through the progress-detail label before teardown,
// instead of silently closing.
void MainWindowUiTest::closeDuringDiagnosticShowsCancelState()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    // Simulate an in-flight read-only diagnostic owning the privileged gate and
    // the busy indicator (no real session, so closePrivilegedSession() returns
    // immediately).
    window.beginBusyOperation(QStringLiteral("Running diagnostics"));
    window.m_privilegedOperationActive = true;
    window.m_privilegedOperationIsDiagnostic = true;

    window.close();
    QCoreApplication::processEvents();

    QVERIFY(window.m_privilegedRequestAbortRequested);
    QVERIFY(window.m_progressDetailLabel);
    QVERIFY2(window.m_progressDetailLabel->text().contains(QStringLiteral("Cancelling")),
             qPrintable(window.m_progressDetailLabel->text()));
    // The progress-detail state is active (it would render in the busy slot).
    QVERIFY(window.m_progressDetailActive);
}

