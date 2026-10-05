// Part of the boot-repair-ui-tests read-only regression suite: core layout and diagnostics.
// The class declaration lives in MainWindowUiTest.h and the shared
// fixtures/helpers in UiTestHelpers.h; this file holds the core layout and diagnostics test
// implementations so the suite compiles as parallel translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

using namespace UiTestDetail;

void MainWindowUiTest::initTestCase()
{
    QCoreApplication::setOrganizationName(QStringLiteral("BootRepair"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("bootrepair.org"));
    QCoreApplication::setApplicationName(QStringLiteral("BootRepairUiTests"));
    QCoreApplication::setApplicationVersion(QStringLiteral(BOOT_REPAIR_VERSION));

    QSettings settings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    settings.clear();
    settings.sync();
}

void MainWindowUiTest::tabsAndReadOnlyControls()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_tabs);
    QCOMPARE(window.m_tabs->count(), 9);
    QVERIFY(!window.m_tabs->tabBar()->usesScrollButtons());
    QVERIFY(window.m_tabScrollLeftButton);
    QVERIFY(window.m_tabScrollRightButton);
    QCOMPARE(window.m_tabScrollLeftButton->arrowType(), Qt::LeftArrow);
    QCOMPARE(window.m_tabScrollRightButton->arrowType(), Qt::RightArrow);
    window.m_tabScrollRightButton->click();
    QCOMPARE(window.m_tabs->currentIndex(), 1);
    window.m_tabScrollLeftButton->click();
    QCOMPARE(window.m_tabs->currentIndex(), 0);
    QCOMPARE(window.m_tabs->tabText(0), QStringLiteral("Systems"));
    QCOMPARE(window.m_tabs->tabText(1), QStringLiteral("Diagnostics"));
    QCOMPARE(window.m_tabs->tabText(2), QStringLiteral("EFI boot entries"));
    QCOMPARE(window.m_tabs->tabText(3), QStringLiteral("Repair"));
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Snapshots"));
    QCOMPARE(window.m_tabs->tabText(5), QStringLiteral("Chroot Shell"));
    QCOMPARE(window.m_tabs->tabText(6), QStringLiteral("File Copy"));
    QCOMPARE(window.m_tabs->tabText(7), QStringLiteral("Logs"));
    QCOMPARE(window.m_tabs->tabText(8), QStringLiteral("Settings"));
    QCOMPARE(window.m_tabs->currentIndex(), 0);
    window.resize(480, 500);
    QTest::qWait(50);
    QWidget *tabControls = window.m_tabs->cornerWidget(Qt::TopLeftCorner);
    QVERIFY(tabControls);
    QVERIFY(tabControls->geometry().right() <= window.m_tabs->tabBar()->geometry().left());
    for (QToolButton *native : window.m_tabs->tabBar()->findChildren<QToolButton *>()) {
        if (native->objectName().startsWith(QStringLiteral("Scroll"))) {
            QVERIFY(!native->isVisible());
        }
    }
    QVERIFY(window.m_chrootShellCommandEdit);
    QVERIFY(window.m_chrootShellOutput);
    QVERIFY(window.m_chrootShellRunButton);
    QVERIFY(window.m_repairVerticalSplitter);
    QCOMPARE(window.m_repairVerticalSplitter->orientation(), Qt::Vertical);
    QCOMPARE(window.m_repairVerticalSplitter->count(), 2);
    QVERIFY(!window.m_repairVerticalSplitter->childrenCollapsible());
    QVERIFY(window.m_repairSplitter);
    QCOMPARE(window.m_repairVerticalSplitter->widget(1), window.m_repairSplitter);
    QVERIFY(window.m_repairToolTree);
    QCOMPARE(window.m_repairToolTree->verticalScrollBarPolicy(), Qt::ScrollBarAlwaysOn);
    QPushButton *configurePlan = buttonWithText(window, QStringLiteral("Configure Plan…"));
    QVERIFY(configurePlan);
    QVERIFY2(configurePlan->sizeHint().width() < 300,
             "Full Repair plan controls should retain standard content-sized widths");
    QVERIFY(window.m_deviceTree);
    QCOMPARE(window.m_deviceTree->horizontalScrollBarPolicy(), Qt::ScrollBarAsNeeded);

    for (int index = 0; index < window.m_tabs->count(); ++index) {
        window.m_tabs->setCurrentIndex(index);
        QCOMPARE(window.m_tabs->currentIndex(), index);
    }

    window.m_tabs->setCurrentIndex(7);
    QPushButton *clearRegister = buttonWithText(window, QStringLiteral("Clear Register"));
    QVERIFY(clearRegister);
    QVERIFY(clearRegister->isEnabled());
    QVERIFY(window.m_logView);
    window.appendLog(QStringLiteral("UI_TEST_BEFORE_CLEAR"));
    clearRegister->click();
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Action register cleared")));
}

// Clear Register must keep the cached diagnostic evidence and the on-screen
// register coherent: with warm caches it confirms first, then clears both the
// register and the diagnostic caches so every repair tool fails closed.
void MainWindowUiTest::clearRegisterClearsDiagnosticCachesWithConfirmation()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    // Warm both scopes' caches so the confirmation path is exercised.
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    QVERIFY(!window.m_targetDiagnosticCache.isEmpty());
    window.m_hostDiagnosticCache.insert(QStringLiteral("report"),
                                        QStringLiteral("Diagnostic: report\nhost evidence"));
    QVERIFY(!window.m_hostDiagnosticCache.isEmpty());
    QVERIFY(window.repairToolAvailable(QStringLiteral("gpu")));

    QPushButton *clearRegister = buttonWithText(window, QStringLiteral("Clear Register"));
    QVERIFY(clearRegister);
    acceptNextMessageBoxByText(&window, QStringLiteral("Clear"));
    clearRegister->click();

    QVERIFY2(window.m_targetDiagnosticCache.isEmpty(), "target cache must be cleared");
    QVERIFY2(window.m_hostDiagnosticCache.isEmpty(), "host cache must be cleared");
    QVERIFY2(window.m_targetDiagnosticsNeedRegeneration,
             "target diagnostics must be marked for regeneration");
    QVERIFY2(window.m_logView->toPlainText().contains(
                 QStringLiteral("repair tools are disabled")),
             "the clear must log that repair tools are disabled");
    // The repair tools now fail closed on the cleared single source of truth.
    QString reason;
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("gpu"), &reason),
             "repair tools must be unavailable after the clear");
    QTreeWidgetItem *gpu = repairItem(window, QStringLiteral("gpu"));
    QVERIFY(gpu);
    QVERIFY2(gpu->text(1).startsWith(QStringLiteral("Unavailable:")),
             qPrintable(gpu->text(1)));
}

// The repair tool detail pane must name the missing diagnostic by its
// human-readable title, never the raw programmatic key.
void MainWindowUiTest::repairToolStatusLineNamesMissingDiagnosticByTitle()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    // GPU capability evidence is available, but the gpu diagnostic section is
    // not cached, so the tool is gated only by the missing diagnostic.
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    QVERIFY(window.m_targetDiagnosticCache.value(QStringLiteral("gpu")).trimmed().isEmpty());
    QVERIFY(window.repairToolAvailable(QStringLiteral("gpu")));

    QTreeWidgetItem *gpu = repairItem(window, QStringLiteral("gpu"));
    QVERIFY(gpu);
    window.m_repairToolTree->setCurrentItem(gpu);
    window.updateRepairToolDetails();

    QVERIFY(window.m_repairToolPlanStatus);
    const QString status = window.m_repairToolPlanStatus->text();
    QVERIFY2(status.contains(QStringLiteral("missing diagnostic: Graphics / GPU driver stack")),
             qPrintable(status));
    QVERIFY2(!status.contains(QStringLiteral("missing diagnostic: gpu")),
             qPrintable(status));
    QVERIFY2(!window.m_repairToolButton->isEnabled(),
             "the Run action must stay disabled while the diagnostic is missing");
}

// Each Diagnostics list item carries a short one-sentence tooltip, never the
// full detail-pane description (which is still shown via m_diagnosticDescription).
void MainWindowUiTest::diagnosticListItemTooltipsAreShort()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QVERIFY(window.m_diagnosticList);
    QCOMPARE(window.m_diagnosticList->count(), 18);

    const QHash<QString, QString> expected = {
        {QStringLiteral("environment"), QStringLiteral("Summarize the selected system, protection state and inspection readiness.")},
        {QStringLiteral("backend"), QStringLiteral("Detect the distribution, bootloader, ESP and current repair capability.")},
        {QStringLiteral("package"), QStringLiteral("Summarize pending configuration and broken dependency state read-only.")},
        {QStringLiteral("boot"), QStringLiteral("Show boot mounts, /boot and EFI contents read-only.")},
        {QStringLiteral("boot-evidence"), QStringLiteral("Correlate the boot chain, loader, kernel/initramfs and unlock evidence.")},
        {QStringLiteral("kernel"), QStringLiteral("Verify kernel files and matching initramfs images read-only.")},
        {QStringLiteral("grub"), QStringLiteral("Review GRUB configuration and /etc/default/grub read-only.")},
        {QStringLiteral("uki"), QStringLiteral("Inspect the ESP, UKI images and firmware entries.")},
        {QStringLiteral("display"), QStringLiteral("Review the configured display manager and login target.")},
        {QStringLiteral("gpu"), QStringLiteral("Review the kernel GPU driver and per-kernel DKMS build status read-only.")},
        {QStringLiteral("errors"), QStringLiteral("Read recent boot error-priority journal entries.")},
        {QStringLiteral("usage"), QStringLiteral("Summarize filesystem capacity and free space.")},
        {QStringLiteral("filesystem"), QStringLiteral("Run the read-only file system check and report each device's result.")},
        {QStringLiteral("fstab"), QStringLiteral("Display the selected system's fstab read-only.")},
        {QStringLiteral("btrfs"), QStringLiteral("Show Btrfs filesystem and subvolume information.")},
        {QStringLiteral("mapper"), QStringLiteral("Show device-mapper ancestry and cryptsetup state.")},
        {QStringLiteral("luks"), QStringLiteral("Show LUKS ancestry plus crypttab and fstab mapper references.")},
        {QStringLiteral("report"), QStringLiteral("Combine every read-only diagnostic for the selected scope.")}
    };

    for (int i = 0; i < window.m_diagnosticList->count(); ++i) {
        QListWidgetItem *item = window.m_diagnosticList->item(i);
        const QString key = item->data(Qt::UserRole).toString();
        const QString tooltip = item->toolTip();
        QVERIFY2(expected.contains(key), qPrintable(QStringLiteral("unexpected diagnostic key: %1").arg(key)));
        QCOMPARE(tooltip, expected.value(key));
        // The tooltip must be a single concise sentence, far shorter than the
        // detail-pane description it replaced (all are well under 80 chars).
        QVERIFY2(tooltip.size() < 80,
                 qPrintable(QStringLiteral("tooltip for %1 is not short: %2").arg(key, tooltip)));
    }
}

void MainWindowUiTest::chrootShellControlsAreGuardedAndReadOnly()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_chrootShellCommandEdit);
    QVERIFY(window.m_chrootShellOutput);
    QVERIFY(window.m_chrootShellRunButton);
    QVERIFY(window.m_chrootShellOutput->isReadOnly());
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
    window.updateTargetLabels();
    QVERIFY(!window.m_chrootShellRunButton->isEnabled());
    QCOMPARE(window.m_chrootShellTargetLabel->text(), QStringLiteral("Target: none selected"));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("update-grub"));
    QVERIFY(window.m_chrootShellCommandEdit->text().contains(QStringLiteral("update-grub")));
}

void MainWindowUiTest::unlockStatusPanelIsReadOnlyAndScoped()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_unlockStatusView);
    QVERIFY(window.m_unlockStatusBox);
    // The status frame expands into the left pane's remaining space so its
    // bottom edge stays aligned with the selected-drive details frame.
    QCOMPARE(window.m_unlockStatusBox->sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
    QCOMPARE(window.m_unlockStatusView->sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
    QVERIFY(window.m_selectedDriveDetailsBox);
    if (window.m_systemSplitter->orientation() == Qt::Horizontal) {
        QCoreApplication::processEvents();
        const int unlockBottom = window.m_unlockStatusBox->mapTo(
            window.m_systemSplitter, QPoint(0, window.m_unlockStatusBox->height())).y();
        const int detailsBottom = window.m_selectedDriveDetailsBox->mapTo(
            window.m_systemSplitter, QPoint(0, window.m_selectedDriveDetailsBox->height())).y();
        QVERIFY2(qAbs(unlockBottom - detailsBottom) <= 2,
                 "unlock status and selected-drive details frames should share a bottom edge");
    }
    QVERIFY(window.m_unlockStatusView->isReadOnly());
    QVERIFY(!window.m_unlockStatusView->toPlainText().isEmpty());

    // Session cleanup must discard cached unlock evidence so a subsequent
    // target selection cannot display a mapper that is no longer owned.
    window.m_unlockStatusCache.insert(QStringLiteral("/dev/test-disk"), QStringLiteral("UNLOCKED=/dev/mapper/test"));
    window.closePrivilegedSession();
    QVERIFY(window.m_unlockStatusCache.isEmpty());
    QVERIFY(window.m_unlockStatusView->toPlainText().contains(QStringLiteral("No unlock operation")));
}

void MainWindowUiTest::filesystemRepairToolRowIsGatedByCapabilityEvidence()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    prepareRepairScope(window);
    QVERIFY(window.m_repairToolTree);
    QTreeWidgetItem *filesystemItem = repairItem(window, QStringLiteral("filesystem"));
    QVERIFY2(filesystemItem, "the File system repair tool row must exist");
    QCOMPARE(filesystemItem->text(0), QStringLiteral("File system repair"));

    // Unavailable evidence disables the row and surfaces the helper's reason.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: unavailable|No supported file system check tool is installed in the recovery environment\n");
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();
    filesystemItem = repairItem(window, QStringLiteral("filesystem"));
    QVERIFY(filesystemItem);
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("filesystem")),
             "an unavailable filesystem capability must fail closed");
    QVERIFY(filesystemItem->text(1).contains(QStringLiteral("No supported file system check tool")));
    QVERIFY(window.m_fullRepairFilesystem);
    QVERIFY2(!window.m_fullRepairFilesystem->isEnabled(),
             "the Settings stage must be disabled with the unavailable capability");
    QVERIFY2(window.m_fullRepairFilesystem->toolTip().contains(
                 QStringLiteral("No supported file system check tool")),
             "the disabled Settings stage must surface the helper's reason");
    QVERIFY2(!window.selectedRepairStages().contains(QStringLiteral("filesystem")),
             "an unavailable filesystem stage must never enter the execution plan");

    // Available evidence enables the row.
    evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();
    QVERIFY2(window.repairToolAvailable(QStringLiteral("filesystem")),
             "available filesystem capability evidence must enable the tool");
    QVERIFY(window.m_fullRepairFilesystem->isEnabled());

    window.m_repairToolTree->setCurrentItem(repairItem(window, QStringLiteral("filesystem")));
    window.updateRepairToolDetails();
    QVERIFY(window.m_repairToolTitle);
    QVERIFY(window.m_repairToolTitle->text().contains(QStringLiteral("File system repair")));
    QVERIFY(window.m_repairToolButton);
    QVERIFY2(window.m_repairToolButton->isEnabled(),
             "the Check File Systems action must be enabled for an available filesystem capability");
    QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("read-only")));
    QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("mounted filesystem")));
}

void MainWindowUiTest::kernelRemovalPreviewListsTuxedoPlanAndKeepsNewest()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    const QString bootSpace =
        QStringLiteral("Kernel removable: 7.1.8.2-tuxedo-amd64 size=524288000 "
                       "pkgs=linux-image-7.1.8.2-tuxedo-amd64,linux-modules-7.1.8.2-tuxedo-amd64,"
                       "linux-headers-7.1.8.2-tuxedo-amd64,linux-headers-7.1.8.2-common-tuxedo\n")
        + QStringLiteral("Kernel latest: 7.1.9.1-tuxedo-amd64\n");
    window.m_targetDiagnosticCache.insert(QStringLiteral("kernel"), bootSpace);

    const QString preview = window.kernelsRemovalPreview();
    QVERIFY2(preview.contains(QStringLiteral("1. linux-image-7.1.8.2-tuxedo-amd64")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("2. linux-modules-7.1.8.2-tuxedo-amd64")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("3. linux-headers-7.1.8.2-tuxedo-amd64")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("4. linux-headers-7.1.8.2-common-tuxedo")),
             qPrintable(preview));
    // The grouped common headers are part of the same old kernel's plan, never
    // a separate removable kernel.
    QVERIFY2(preview.contains(QStringLiteral("Superseded kernel(s) available for removal: 1.")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("Approximately 500.0 MB would be restored.")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("Newest installed kernel: 7.1.9.1-tuxedo-amd64.")),
             qPrintable(preview));
    // No protected package or metapackage leaks into the plan text.
    QVERIFY2(!preview.contains(QStringLiteral("linux-headers-tuxedo-amd64")),
             qPrintable(preview));
    QVERIFY2(!preview.contains(QStringLiteral("linux-image-tuxedo")),
             qPrintable(preview));
}

void MainWindowUiTest::kernelRemovalPreviewDetectsMissingHeadersAndGroupsCommonHeaders()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    const QString bootSpace =
        QStringLiteral("Kernel removable: 6.2.0-1000-generic size=1048576 "
                       "pkgs=linux-image-6.2.0-1000-generic,linux-modules-6.2.0-1000-generic,"
                       "linux-headers-6.2.0-1000-generic,linux-headers-6.2.0-1000\n")
        + QStringLiteral("Kernel latest: 6.5.0-10038-generic\n")
        + QStringLiteral("Kernel missing headers: 6.5.0-10038-generic\n");
    window.m_targetDiagnosticCache.insert(QStringLiteral("kernel"), bootSpace);

    const QString preview = window.kernelsRemovalPreview();
    // The default -amd64/generic naming groups the bare linux-headers-<series>
    // common headers with the old kernel.
    QVERIFY2(preview.contains(QStringLiteral("4. linux-headers-6.2.0-1000")),
             qPrintable(preview));
    // The missing-headers note surfaces the newest kernel by version.
    QVERIFY2(preview.contains(QStringLiteral("Missing headers (latest kernel has no matching headers): 6.5.0-10038-generic")),
             qPrintable(preview));
    // The newest kernel is reported.
    QVERIFY2(preview.contains(QStringLiteral("Newest installed kernel: 6.5.0-10038-generic.")),
             qPrintable(preview));
}

void MainWindowUiTest::kernelRemovalPreviewSurfacesRepoLatestAndOrphanedHeaders()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    // Rich evidence: a broken newest (not running), a removable older kernel,
    // a repository-latest ahead of installed, orphaned headers, an orphaned
    // kernel (zombie) and an incomplete kernel.
    const QString bootSpace =
        QStringLiteral("Kernel running: 7.1.8.2-tuxedo-amd64\n")
        + QStringLiteral("Kernel latest: 7.1.9.1-tuxedo-amd64\n")
        + QStringLiteral("Kernel repository latest: 7.1.9.3-tuxedo-amd64\n")
        + QStringLiteral("Kernel removable: 7.1.7.0-tuxedo-amd64 size=1048576 "
                         "pkgs=linux-image-7.1.7.0-tuxedo-amd64\n")
        + QStringLiteral("Kernel orphaned headers: 6.2.0-1000-generic\n")
        + QStringLiteral("Kernel orphaned kernel: 7.0.0-0-tuxedo-amd64\n")
        + QStringLiteral("Kernel orphaned build support: 7.1.10.2 pkg=linux-kbuild-7.1.10.2\n")
        + QStringLiteral("Kernel incomplete: 7.1.9.1-tuxedo-amd64\n")
        + QStringLiteral("Kernel missing headers: 7.1.9.1-tuxedo-amd64\n");
    window.m_targetDiagnosticCache.insert(QStringLiteral("kernel"), bootSpace);

    const QString preview = window.kernelsRemovalPreview();
    QVERIFY2(preview.contains(QStringLiteral("Running kernel: 7.1.8.2-tuxedo-amd64.")),
             qPrintable(preview));
    // Repository-latest note surfaces the newer available kernel.
    QVERIFY2(preview.contains(QStringLiteral("Repository latest kernel: 7.1.9.3-tuxedo-amd64")),
             qPrintable(preview));
    // Orphaned headers are surfaced distinctly from missing headers.
    QVERIFY2(preview.contains(QStringLiteral("Orphaned headers (no matching kernel): 6.2.0-1000-generic")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("Missing headers (latest kernel has no matching headers): 7.1.9.1-tuxedo-amd64")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("Orphaned kernel components (no owning package): 7.0.0-0-tuxedo-amd64")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("Orphaned kernel build support (kernel gone): 7.1.10.2 pkg=linux-kbuild-7.1.10.2")),
             qPrintable(preview));
    QVERIFY2(preview.contains(QStringLiteral("Incomplete kernels (installed package without its on-disk files): 7.1.9.1-tuxedo-amd64")),
             qPrintable(preview));
}

void MainWindowUiTest::kernelRepairActionsDeriveFromEvidenceAndProtectRunning()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);

    // Evidence with every action class present (running != latest so the newest
    // kernel is removable too).
    const QString fullEvidence =
        QStringLiteral("Kernel running: 7.1.8.2-tuxedo-amd64\n")
        + QStringLiteral("Kernel latest: 7.1.9.1-tuxedo-amd64\n")
        + QStringLiteral("Kernel repository latest: 7.1.9.3-tuxedo-amd64\n")
        + QStringLiteral("Kernel removable: 7.1.7.0-tuxedo-amd64 size=1048576 "
                         "pkgs=linux-image-7.1.7.0-tuxedo-amd64\n")
        + QStringLiteral("Kernel orphaned headers: 6.2.0-1000-generic\n")
        + QStringLiteral("Kernel orphaned kernel: 7.0.0-0-tuxedo-amd64\n")
        + QStringLiteral("Kernel orphaned build support: 7.1.10.2 pkg=linux-kbuild-7.1.10.2\n")
        + QStringLiteral("Kernel incomplete: 7.1.9.1-tuxedo-amd64\n")
        + QStringLiteral("Kernel missing headers: 7.1.9.1-tuxedo-amd64\n");
    const QList<QPair<QString, QString>> actions = MainWindow::kernelRepairActionsFor(fullEvidence);
    QStringList labels;
    for (const QPair<QString, QString> &action : actions) {
        labels << action.first;
    }
    QVERIFY2(labels.contains(QStringLiteral("Remove oldest/superseded kernel(s)")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Remove newest kernel")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Remove orphaned headers")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Remove orphaned kernel components")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Remove orphaned kernel build support")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Install missing headers")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Reinstall/complete a kernel")),
             qPrintable(labels.join(QLatin1Char('|'))));
    QVERIFY2(labels.contains(QStringLiteral("Install repository-latest kernel")),
             qPrintable(labels.join(QLatin1Char('|'))));

    // The running kernel is never offered for removal: no action label names it
    // and no stage for a removal action carries the running version. The newest
    // is removable here (running != latest) but the running kernel must never
    // appear in any action.
    QVERIFY2(!labels.join(QLatin1Char(' ')).contains(QStringLiteral("7.1.8.2-tuxedo-amd64")),
             qPrintable(labels.join(QLatin1Char('|'))));

    // When the newest kernel IS the running kernel, "Remove newest kernel" is
    // never offered.
    const QString runningIsNewest =
        QStringLiteral("Kernel running: 7.1.9.1-tuxedo-amd64\n")
        + QStringLiteral("Kernel latest: 7.1.9.1-tuxedo-amd64\n")
        + QStringLiteral("Kernel removable: 7.1.7.0-tuxedo-amd64 size=1048576 "
                         "pkgs=linux-image-7.1.7.0-tuxedo-amd64\n");
    const QList<QPair<QString, QString>> runningNewest = MainWindow::kernelRepairActionsFor(runningIsNewest);
    for (const QPair<QString, QString> &action : runningNewest) {
        QVERIFY2(action.first != QStringLiteral("Remove newest kernel"),
                 "the running kernel must never be offered as the newest to remove");
    }

    // No evidence yields no actions (the menu then reports nothing actionable).
    const QList<QPair<QString, QString>> empty = MainWindow::kernelRepairActionsFor(QString());
    QVERIFY2(empty.isEmpty(), "no evidence must derive no actions");

    // Each individual evidence line maps to exactly its own action (no
    // auto-removal: an action only fires when its evidence exists).
    QVERIFY2(MainWindow::kernelRepairActionsFor(QStringLiteral("Kernel removable: 6.2.0-1000-generic pkgs=linux-image-6.2.0-1000-generic\n")).size() == 1,
             "only the removable evidence must produce one action");
    QVERIFY2(MainWindow::kernelRepairActionsFor(QStringLiteral("Kernel running: a\nKernel latest: b\n")).size() == 1,
             "a broken newest (running != latest) must produce exactly the remove-newest action");
    QVERIFY2(MainWindow::kernelRepairActionsFor(QStringLiteral("Kernel orphaned kernel: 1.0\n")).size() == 1,
             "an orphaned kernel must produce exactly one action");
    QVERIFY2(MainWindow::kernelRepairActionsFor(
                 QStringLiteral("Kernel orphaned build support: 7.1.10.2 pkg=linux-kbuild-7.1.10.2\n")).size() == 1,
             "an orphaned build-support package must produce exactly one action");
    QVERIFY2(MainWindow::kernelRepairActionsFor(QStringLiteral("Kernel incomplete: 1.0\n")).size() == 1,
             "an incomplete kernel must produce exactly one action");
    QVERIFY2(MainWindow::kernelRepairActionsFor(
                 QStringLiteral("Kernel latest: a\nKernel repository latest: b\n")).size() == 1,
             "repo-latest newer than installed must produce exactly one action");
}

// The interactive kernel repair must hand the cached `kernel` diagnostic
// section to the helper as its --baseline (serialized to base64), so the menu
// consumes the single cached source of truth instead of re-collecting.
void MainWindowUiTest::kernelRepairSessionSerializesCachedBaseline()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    const QString kernelSection = QStringLiteral(
        "Kernel running: 7.1.9.1-tuxedo-amd64\n"
        "Kernel latest: 7.1.9.1-tuxedo-amd64\n"
        "Kernel removable: 7.1.8.2-tuxedo-amd64 size=1048576 "
        "pkgs=linux-image-7.1.8.2-tuxedo-amd64\n");
    window.m_targetDiagnosticCache.insert(QStringLiteral("kernel"), kernelSection);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("kernels-request.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath, QStringList()),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("kernels"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    const QStringList args = capturedHelperArguments(capturePath);
    QVERIFY2(args.contains(QStringLiteral("kernels")),
             qPrintable(args.join(QLatin1Char(' '))));
    const int baselineIndex = args.indexOf(QStringLiteral("--baseline"));
    QVERIFY2(baselineIndex >= 0 && baselineIndex + 1 < args.size(),
             "the kernel repair must serialize the cached section as --baseline <base64>");
    const QString decoded = QString::fromUtf8(
        QByteArray::fromBase64(args.at(baselineIndex + 1).toLatin1()));
    QCOMPARE(decoded, kernelSection);
}

// The interactive GPU repair must hand the cached `gpu` diagnostic section to
// the helper as --gpu-baseline and the cached `kernel` section as --baseline
// (both serialized to base64), so the GPU menu and the kernel-dependent GPU
// actions consume the single cached sources of truth instead of re-collecting.
void MainWindowUiTest::gpuRepairSessionSerializesGpuAndKernelBaselines()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    const QString gpuSection = QStringLiteral(
        "GPU hardware: 10de:24b0 (10de:24b0) [driver-in-use=nvidia]\n"
        "GPU hardware note: recovery-host\n"
        "GPU kernel module loaded: nvidia\n"
        "GPU installed driver packages: nvidia-driver@560.28.03\n"
        "GPU expected driver: nvidia (source=hardware-pciid)\n"
        "GPU driver mismatch: nvidia vs nouveau (reason=expected nvidia but the installed driver is nouveau)\n");
    const QString kernelSection = QStringLiteral(
        "Kernel running: 7.1.9.1-tuxedo-amd64\n"
        "Kernel latest: 7.1.9.1-tuxedo-amd64\n"
        "Kernel removable: 7.1.8.2-tuxedo-amd64 size=1048576 "
        "pkgs=linux-image-7.1.8.2-tuxedo-amd64\n");
    window.m_targetDiagnosticCache.insert(QStringLiteral("gpu"), gpuSection);
    window.m_targetDiagnosticCache.insert(QStringLiteral("kernel"), kernelSection);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("gpu-request.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath, QStringList()),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("gpu"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    const QStringList args = capturedHelperArguments(capturePath);
    QVERIFY2(args.contains(QStringLiteral("gpu")),
             qPrintable(args.join(QLatin1Char(' '))));
    const int gpuBaselineIndex = args.indexOf(QStringLiteral("--gpu-baseline"));
    QVERIFY2(gpuBaselineIndex >= 0 && gpuBaselineIndex + 1 < args.size(),
             "the GPU repair must serialize the cached section as --gpu-baseline <base64>");
    const QString decodedGpu = QString::fromUtf8(
        QByteArray::fromBase64(args.at(gpuBaselineIndex + 1).toLatin1()));
    QCOMPARE(decodedGpu, gpuSection);

    const int kernelBaselineIndex = args.indexOf(QStringLiteral("--baseline"));
    QVERIFY2(kernelBaselineIndex >= 0 && kernelBaselineIndex + 1 < args.size(),
             "the GPU repair must serialize the cached kernel section as --baseline <base64>");
    const QString decodedKernel = QString::fromUtf8(
        QByteArray::fromBase64(args.at(kernelBaselineIndex + 1).toLatin1()));
    QCOMPARE(decodedKernel, kernelSection);
}

// The boot-stack reconciliation must hand its four component baselines to the
// helper as --evidence <section> <base64> (mapper/crypttab boot-evidence,
// initramfs kernel, EFI/UKI uki, GRUB grub), so the reconciliation reuses the
// cached evidence instead of re-collecting it.
void MainWindowUiTest::bootstackSerializesComponentBaselines()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidence(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("bootstack-request.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath, QStringList()),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("bootstack"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    const QStringList args = capturedHelperArguments(capturePath);
    QVERIFY2(args.contains(QStringLiteral("boot-stack")),
             qPrintable(args.join(QLatin1Char(' '))));
    for (const QString &section : {QStringLiteral("boot-evidence"), QStringLiteral("kernel"),
                                   QStringLiteral("uki"), QStringLiteral("grub")}) {
        int found = -1;
        for (int i = 0; i + 2 < args.size(); ++i) {
            if (args.at(i) == QStringLiteral("--evidence") && args.at(i + 1) == section) {
                found = i;
                break;
            }
        }
        QVERIFY2(found >= 0,
                 qPrintable(QStringLiteral("missing --evidence %1 in %2")
                                .arg(section, args.join(QLatin1Char(' ')))));
        QVERIFY2(!QByteArray::fromBase64(args.at(found + 2).toLatin1()).isEmpty(),
                 qPrintable(QStringLiteral("empty %1 baseline payload").arg(section)));
    }
}

// A repair tool must refuse to run — with a warning naming the missing
// diagnostic section by its human-readable title and without launching the
// privileged helper — when any required cached section is absent. The DKMS
// repair requires the `kernel` section, so dropping it must abort.
void MainWindowUiTest::repairToolRefusesRunWhenRequiredDiagnosticMissing()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    // DKMS requires the `kernel` section; remove it so the run must refuse.
    window.m_targetDiagnosticCache.remove(QStringLiteral("kernel"));

    QString reason;
    QVERIFY2(!window.repairEvidenceReadyForTool(QStringLiteral("dkms"), &reason),
             "the DKMS repair must refuse when its required kernel section is missing");
    QVERIFY2(reason.contains(QStringLiteral("Kernel / initramfs")),
             qPrintable(reason));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("refused-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath, QStringList()),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Ok);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("dkms"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    // The refusal must have aborted before the helper was launched: no request
    // record exists (the fake session only writes once it reads a BEGIN line).
    QFile file(capturePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QString captured = QString::fromUtf8(file.readAll());
        QVERIFY2(!captured.contains(QStringLiteral("BEGIN")),
                 "a missing required diagnostic must abort before launching the helper");
    }
}

// The required-sections mapping is the single list the repair-run gate and the
// baseline serialization both consume; every tool maps to its documented set.
void MainWindowUiTest::requiredDiagnosticSectionsMapping()
{
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("validate")),
             QStringList({QStringLiteral("environment")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("filesystem")),
             QStringList({QStringLiteral("filesystem")}));
    for (const QString &key : {QStringLiteral("dpkg"), QStringLiteral("fixbroken"),
                               QStringLiteral("aptupdate"), QStringLiteral("upgrade")}) {
        QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(key),
                 QStringList({QStringLiteral("package")}));
    }
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("dkms")),
             QStringList({QStringLiteral("kernel")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("display")),
             QStringList({QStringLiteral("display")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("initramfs")),
             QStringList({QStringLiteral("kernel")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("efi")),
             QStringList({QStringLiteral("uki")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("grub")),
             QStringList({QStringLiteral("grub")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("extlinux")),
             QStringList({QStringLiteral("boot-evidence")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("bootstack")),
             QStringList({QStringLiteral("boot-evidence"), QStringLiteral("kernel"),
                          QStringLiteral("uki"), QStringLiteral("grub")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("kernels")),
             QStringList({QStringLiteral("kernel")}));
    QCOMPARE(MainWindow::requiredDiagnosticSectionsForTool(QStringLiteral("gpu")),
             QStringList({QStringLiteral("gpu"), QStringLiteral("kernel")}));
}

void MainWindowUiTest::reasonKeyTokensMapToLocalizedStringsAndUnknownKeysPassThrough()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // A known reason key maps to its translatable source (the English fallback
    // renders when no translator is loaded) and still fails closed.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool dpkg: unavailable|reason:missing-dpkg\n");
    cacheRepairEvidence(window, evidence);
    QString reason;
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("dpkg"), &reason),
             "a reason-key unavailable state must fail closed");
    QCOMPARE(reason, QStringLiteral("dpkg is not installed in the target."));

    // An unknown reason key passes through verbatim (raw fallback), never a
    // fabricated string, and still fails closed.
    evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool dpkg: unavailable|reason:totally-unknown-key\n");
    cacheRepairEvidence(window, evidence);
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("dpkg"), &reason),
             "an unknown reason key must fail closed");
    QCOMPARE(reason, QStringLiteral("reason:totally-unknown-key"));

    // A legacy English-prose reason passes through verbatim (backward
    // compatibility for a mixed helper/GUI rollout).
    evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool dpkg: unavailable|dpkg is not installed in the target\n");
    cacheRepairEvidence(window, evidence);
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("dpkg"), &reason),
             "a legacy English reason must fail closed");
    QCOMPARE(reason, QStringLiteral("dpkg is not installed in the target"));
}

void MainWindowUiTest::fullRepairFilesystemStageIsFirstWhenEnabled()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    prepareRepairScope(window);
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);
    // Apply the scope's availability first so the explicit setChecked calls
    // below change widget state, emit toggled and record the preferences.
    window.updateFullRepairSummary();

    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    QVERIFY(window.m_fullRepairFilesystem);
    window.m_fullRepairFilesystem->setChecked(true);
    window.updateFullRepairSummary();

    QVERIFY(window.m_fullRepairStageList);
    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY2(window.m_fullRepairStageList->item(0)->text().contains(QStringLiteral("Repair file system errors")),
             "the file system stage must be the first Full Repair stage when enabled");
    QVERIFY2(window.m_fullRepairStageList->item(0)->text().startsWith(QStringLiteral("1. ")),
             "the file system stage must be ordered first in the execution plan");
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    // The execution list Full Repair sends to the helper must also start with
    // the file system stage; otherwise the inspect/confirm/repair flow is
    // skipped even though the plan lists it.
    window.m_fullRepairGrub->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("filesystem"), QStringLiteral("grub")}));
    window.m_fullRepairGrub->setChecked(false);

    // Disabling the stage removes it from the plan.
    window.m_fullRepairFilesystem->setChecked(false);
    window.updateFullRepairSummary();
    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY2(!window.m_fullRepairStageList->item(0)->text().contains(QStringLiteral("Repair file system errors")),
             "a disabled file system stage must not appear in the plan");
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("filesystem")));
}


int main(int argc, char **argv)
{
    QTemporaryDir config;
    QTemporaryDir sessionLogs;
    if (!config.isValid() || !sessionLogs.isValid()) {
        return 2;
    }
    qputenv("XDG_CONFIG_HOME", config.path().toUtf8());
    qputenv("BOOT_REPAIR_LOG_DIR", sessionLogs.path().toUtf8());
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    MainWindowUiTest test;
    return QTest::qExec(&test, argc, argv);
}
