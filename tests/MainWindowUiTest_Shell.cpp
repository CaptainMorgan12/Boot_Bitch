// Part of the boot-repair-ui-tests read-only regression suite: shell, sortable tables, host default and low-level policies.
// The class declaration lives in MainWindowUiTest.h and the shared
// fixtures/helpers in UiTestHelpers.h; this file holds the shell, sortable tables, host default and low-level policies test
// implementations so the suite compiles as parallel translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

using namespace UiTestDetail;


void MainWindowUiTest::targetShellDispatchStillUsesShell()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.resize(1180, 760);
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    window.updateTargetLabels();
    QVERIFY(window.m_chrootShellRunButton->isEnabled());
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Chroot Shell"));

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("update-grub"));
    window.runChrootShellCommand();

    QCOMPARE(capturedHelperArguments(capturePath),
             QStringList({QStringLiteral("shell"), QStringLiteral("/dev/test-system"),
                          QStringLiteral("/dev/test-root"), QStringLiteral("update-grub")}));
    QVERIFY(window.m_chrootShellOutput->toPlainText().contains(QStringLiteral("[exit 0]")));
}

// While a diagnostics generation/refresh is in flight the Chroot/Host Shell
// Run Command button is disabled with a clear reason (no command may run
// against a half-refreshed evidence state), and it re-enables when the
// refresh completes.
void MainWindowUiTest::shellRunCommandDisabledDuringDiagnosticsRefresh()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    window.updateTargetLabels();
    QVERIFY2(window.m_chrootShellRunButton->isEnabled(),
             "a resolved repair target must enable the shell Run Command");

    // Automatic evidence regeneration in flight: disabled with the reason.
    window.m_evidenceRefreshInProgress = true;
    window.updateShellRunButtonState();
    QVERIFY2(!window.m_chrootShellRunButton->isEnabled(),
             "the Run Command must be disabled while an evidence refresh is in flight");
    QVERIFY2(window.m_chrootShellRunButton->toolTip().contains(QStringLiteral("diagnostics"), Qt::CaseInsensitive),
             "the disabled tooltip must name the diagnostics refresh");
    QVERIFY2(window.m_chrootShellRunButton->toolTip().contains(QStringLiteral("half-refreshed"), Qt::CaseInsensitive),
             "the disabled tooltip must explain the half-refreshed evidence state");

    // Refresh completed: re-enabled.
    window.m_evidenceRefreshInProgress = false;
    window.updateShellRunButtonState();
    QVERIFY(window.m_chrootShellRunButton->isEnabled());

    // A manual Run All / scoped regeneration disables it the same way.
    window.m_diagnosticsRunInProgress = true;
    window.updateShellRunButtonState();
    QVERIFY2(!window.m_chrootShellRunButton->isEnabled(),
             "the Run Command must be disabled while diagnostics are generating");
    window.m_diagnosticsRunInProgress = false;
    window.updateShellRunButtonState();
    QVERIFY(window.m_chrootShellRunButton->isEnabled());

    // A real Run All run disables the button while it owns the generation
    // and re-enables it on completion.
    bool observedDisabledDuringRun = false;
    QTimer probe;
    probe.setInterval(0);
    QObject::connect(&probe, &QTimer::timeout, &window, [&] {
        if (window.m_chrootShellRunButton && !window.m_chrootShellRunButton->isEnabled()) {
            observedDisabledDuringRun = true;
            probe.stop();
        }
    });
    probe.start();
    window.runAllDiagnostics();
    probe.stop();
    QVERIFY2(observedDisabledDuringRun,
             "the Run Command must be disabled while Run All diagnostics is in flight");
    QVERIFY2(window.m_chrootShellRunButton->isEnabled(),
             "the Run Command must re-enable after the diagnostics run completes");
}

// A6-05: the capability gate reads only the dedicated capabilities entry
// (plus the combined report's extracted preamble as the legacy fallback).
// A per-diagnostic section quoting a `Repair tool <key>` line can neither
// open the gate nor override the genuine unavailable reason.
void MainWindowUiTest::capabilityGateReadsOnlyCapabilitiesEntry()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);

    // A per-key section quoting an available line can never open the gate
    // while the dedicated capabilities entry says unavailable: the genuine
    // reason wins.
    window.m_targetDiagnosticCache.clear();
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), QStringLiteral(
        "Repair tool efi: unavailable|no EFI System Partition candidate on the selected disk\n"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("fstab"), QStringLiteral(
        "Diagnostic: fstab\n"
        "Repair tool efi: available\n"
        "UUID=11111111-2222-3333-4444-555555555555 / ext4 defaults 0 1\n"));
    QString reason;
    QVERIFY(!window.repairToolAvailable(QStringLiteral("efi"), &reason));
    QCOMPARE(reason, QStringLiteral("no EFI System Partition candidate on the selected disk"));

    // No capabilities entry (and no report preamble): the gate fails closed
    // even though a per-key section quotes an available line.
    window.m_targetDiagnosticCache.remove(QStringLiteral("capabilities"));
    QVERIFY(!window.repairToolAvailable(QStringLiteral("efi"), &reason));
    QVERIFY2(reason.contains(QStringLiteral("No capability evidence")), qPrintable(reason));

    // The dedicated entry opens the gate.
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"),
                                          QStringLiteral("Repair tool efi: available\n"));
    QVERIFY2(window.repairToolAvailable(QStringLiteral("efi"), &reason), qPrintable(reason));

    // The report-preamble fallback gates on the same evidence for a cache
    // captured before the split entry existed.
    window.m_targetDiagnosticCache.remove(QStringLiteral("capabilities"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("report"), QStringLiteral(
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool efi: available\n"
        "Repair capability evidence efi: efibootmgr present\n\n"
        "Diagnostic: report\n..."));
    QVERIFY2(window.repairToolAvailable(QStringLiteral("efi"), &reason), qPrintable(reason));

    // ... and an unavailable reason in the preamble also gates through the
    // fallback.
    window.m_targetDiagnosticCache.insert(QStringLiteral("report"), QStringLiteral(
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool efi: unavailable|no EFI System Partition candidate on the selected disk\n\n"
        "Diagnostic: report\n..."));
    QVERIFY(!window.repairToolAvailable(QStringLiteral("efi"), &reason));
    QCOMPARE(reason, QStringLiteral("no EFI System Partition candidate on the selected disk"));
}

// A6-06: a fallback-resolved component is adopted only when it neither backs
// the protected running system nor (for a known previous component) lies on a
// different top-level device; refusals log a WARNING and keep the committed
// component.
void MainWindowUiTest::fallbackAdoptionRefusesProtectedAndCrossDisk()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    DeviceNode bootA;
    bootA.path = QStringLiteral("/dev/test-vda1");
    bootA.kernelName = QStringLiteral("vda1");
    bootA.parentKernelName = QStringLiteral("vda");
    bootA.type = QStringLiteral("part");
    bootA.fileSystem = QStringLiteral("ext4");
    bootA.linuxCapableFileSystem = true;
    DeviceNode rootA;
    rootA.path = QStringLiteral("/dev/test-vda2");
    rootA.kernelName = QStringLiteral("vda2");
    rootA.parentKernelName = QStringLiteral("vda");
    rootA.type = QStringLiteral("part");
    rootA.fileSystem = QStringLiteral("ext4");
    rootA.linuxCapableFileSystem = true;
    DeviceNode diskA;
    diskA.path = QStringLiteral("/dev/test-vda");
    diskA.kernelName = QStringLiteral("vda");
    diskA.type = QStringLiteral("disk");
    DeviceNode rootB;
    rootB.path = QStringLiteral("/dev/test-vdb1");
    rootB.kernelName = QStringLiteral("vdb1");
    rootB.parentKernelName = QStringLiteral("vdb");
    rootB.type = QStringLiteral("part");
    rootB.fileSystem = QStringLiteral("ext4");
    rootB.linuxCapableFileSystem = true;
    DeviceNode diskB;
    diskB.path = QStringLiteral("/dev/test-vdb");
    diskB.kernelName = QStringLiteral("vdb");
    diskB.type = QStringLiteral("disk");

    window.m_deviceIndex.insert(diskA.path, diskA);
    window.m_deviceIndex.insert(bootA.path, bootA);
    window.m_deviceIndex.insert(rootA.path, rootA);
    window.m_deviceIndex.insert(diskB.path, diskB);
    window.m_deviceIndex.insert(rootB.path, rootB);
    window.m_previewTargetPath = diskA.path;
    window.m_previewTargetComponentPath = bootA.path;

    // Same-disk fallback (shared PKNAME top-level ancestor) is adopted.
    window.appendDiagnosticLog(QStringLiteral("environment"),
                               QStringLiteral("Environment validation"),
                               QStringLiteral("Repair Target"),
                               QStringLiteral("[12:00:02] Root component fallback: selected component /dev/test-vda1 (ext4) lacks /etc/os-release; resolved /dev/test-vda2 (ext4) from /dev/test-vda.\n"),
                               true);
    QCOMPARE(window.m_previewTargetComponentPath, QStringLiteral("/dev/test-vda2"));

    // Cross-disk fallback: refused with a WARNING; the committed component is
    // kept.
    window.m_previewTargetComponentPath = bootA.path;
    window.appendDiagnosticLog(QStringLiteral("environment"),
                               QStringLiteral("Environment validation"),
                               QStringLiteral("Repair Target"),
                               QStringLiteral("[12:00:02] Root component fallback: selected component /dev/test-vda1 (ext4) lacks /etc/os-release; resolved /dev/test-vdb1 (ext4) from /dev/test-vdb.\n"),
                               true);
    QCOMPARE(window.m_previewTargetComponentPath, bootA.path);
    QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("Root component fallback refused")),
             qPrintable(log));
    QVERIFY2(log.contains(QStringLiteral("not provably on the same top-level device")),
             qPrintable(log));

    // A resolved component that backs the protected running system is refused
    // even when it shares the committed component's disk.
    window.m_deviceIndex[rootA.path].protectedDevice = true;
    window.appendDiagnosticLog(QStringLiteral("environment"),
                               QStringLiteral("Environment validation"),
                               QStringLiteral("Repair Target"),
                               QStringLiteral("[12:00:02] Root component fallback: selected component /dev/test-vda1 (ext4) lacks /etc/os-release; resolved /dev/test-vda2 (ext4) from /dev/test-vda.\n"),
                               true);
    QCOMPARE(window.m_previewTargetComponentPath, bootA.path);
    log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("backs the protected running system")),
             qPrintable(log));
}
