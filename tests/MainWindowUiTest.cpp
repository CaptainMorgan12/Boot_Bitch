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
    QCOMPARE(window.m_tabs->count(), 8);
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
    QCOMPARE(window.m_tabs->tabText(2), QStringLiteral("Repair"));
    QCOMPARE(window.m_tabs->tabText(3), QStringLiteral("Snapshots"));
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Chroot Shell"));
    QCOMPARE(window.m_tabs->tabText(5), QStringLiteral("File Copy"));
    QCOMPARE(window.m_tabs->tabText(6), QStringLiteral("Logs"));
    QCOMPARE(window.m_tabs->tabText(7), QStringLiteral("Settings"));
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

    window.m_tabs->setCurrentIndex(6);
    QPushButton *clearRegister = buttonWithText(window, QStringLiteral("Clear Register"));
    QVERIFY(clearRegister);
    QVERIFY(clearRegister->isEnabled());
    QVERIFY(window.m_logView);
    window.appendLog(QStringLiteral("UI_TEST_BEFORE_CLEAR"));
    clearRegister->click();
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Action register cleared")));
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
                              window.m_fullRepairAptUpdate, window.m_fullRepairUpgrade,
                              window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
                              window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                              window.m_fullRepairGrub}) {
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
