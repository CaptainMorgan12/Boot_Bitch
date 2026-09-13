#define private public
#include "MainWindow.h"
#undef private

#include <QApplication>
#include <QClipboard>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QTabWidget>
#include <QTabBar>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTimer>
#include <QTreeWidget>
#include <QToolButton>
#include <QtTest>

namespace {
QPushButton *buttonWithText(MainWindow &window, const QString &text)
{
    for (QPushButton *button : window.findChildren<QPushButton *>()) {
        if (button->text() == text) {
            return button;
        }
    }
    return nullptr;
}
}

class MainWindowUiTest final : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void tabsAndReadOnlyControls();
    void chrootShellControlsAreGuardedAndReadOnly();
    void hostDiagnosticAndActionRegister();
    void diagnosticOutputIsMirroredToMainLog();
    void narrowRepairAndFileCopyScrollAreasStayStable();
    void applicationDialogsFollowConsistentLayout();
    void snapshotRowsFitSingleLineContent();
    void unlockStatusPanelIsReadOnlyAndScoped();
    void individualDiagnosticEnablesMatchingRepair();
    void fullRepairUsesFreshEvidenceForSelectedStage();
    void displayManagerToolUsesGenericLanguage();
    void repairInvalidationExplainsDiagnosticRerun();
    void logSearchFiltersApplicationLog();
    void logViewShowsNewestEntriesFirst();
    void nonLinuxTargetIsRejectedSafely();
    void guardedWriteActionsStayDisabledWithoutTarget();
    void exportDialogsCanBeCancelledReadOnly();
    void actionRegisterPersistsAcrossWindows();
};

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

void MainWindowUiTest::hostDiagnosticAndActionRegister()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_diagnosticScopeCombo);
    QVERIFY(window.m_diagnosticList);
    QVERIFY(window.m_diagnosticResults);
    window.m_diagnosticScopeCombo->setCurrentIndex(1); // Running Host; no privilege or writes.
    QVERIFY(window.m_diagnosticList->count() > 0);
    bool hasBootEvidence = false;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->text().contains(QStringLiteral("Boot evidence"))) {
            hasBootEvidence = true;
            break;
        }
    }
    QVERIFY(hasBootEvidence);
    window.m_diagnosticList->setCurrentRow(0);
    window.runSelectedDiagnostic();

    QVERIFY(!window.m_diagnosticResults->toPlainText().isEmpty());
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Host diagnostic")));

    QPushButton *copy = buttonWithText(window, QStringLiteral("Copy Results"));
    QVERIFY(copy);
    QVERIFY(copy->isEnabled());
    window.copyDiagnosticResults();
    QVERIFY(!QApplication::clipboard()->text().isEmpty());
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("copied to the clipboard")));

    // Run All stays entirely in the running host scope and exercises every
    // registered read-only diagnostic implementation in one pass.
    window.runAllDiagnostics();
    QVERIFY(window.m_diagnosticResults->toPlainText().contains(QStringLiteral("========================================")));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Starting all available read-only diagnostics")));
}

void MainWindowUiTest::diagnosticOutputIsMirroredToMainLog()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_diagnosticScopeCombo);
    QVERIFY(window.m_diagnosticList);
    QVERIFY(window.m_diagnosticResults);
    QVERIFY(window.m_logView);
    window.m_diagnosticScopeCombo->setCurrentIndex(1); // Running Host; no privilege or writes.

    // Select a deterministic individual diagnostic so the test checks that
    // its complete captured output is copied to the application log, rather
    // than only the short "diagnostic run" status entry.
    int environmentRow = -1;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->data(Qt::UserRole).toString() == QStringLiteral("environment")) {
            environmentRow = row;
            break;
        }
    }
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();

    const QString result = window.m_diagnosticResults->toPlainText().trimmed();
    QVERIFY(!result.isEmpty());
    const QString log = window.m_logView->toPlainText();
    QVERIFY(log.contains(QStringLiteral("========================================")));
    QVERIFY(log.contains(QStringLiteral("Diagnostic: environment")));
    QVERIFY(log.contains(QStringLiteral("Scope: Running Host")));
    QVERIFY(log.contains(QStringLiteral("What was actually run")));
    QVERIFY(log.contains(result));

    // Run All should append a report block to the same persistent log, while
    // retaining the individual section markers used for later inspection.
    const int logLengthBeforeAll = log.size();
    window.runAllDiagnostics();
    const QString reportLog = window.m_logView->toPlainText();
    QVERIFY(reportLog.size() > logLengthBeforeAll);
    QVERIFY(reportLog.contains(QStringLiteral("Diagnostic: environment")));
    QVERIFY(reportLog.contains(QStringLiteral("Diagnostic: report")));

    // Re-running one diagnostic invalidates the prior aggregate report so
    // repair actions cannot consume mixed-generation evidence.
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    QVERIFY(!window.m_hostDiagnosticCache.contains(QStringLiteral("report")));
}

void MainWindowUiTest::narrowRepairAndFileCopyScrollAreasStayStable()
{
    MainWindow window;
    window.resize(560, 380);
    window.show();
    QTest::qWait(100);

    const QList<QPair<QString, int>> pages = {
        {QStringLiteral("repairPageScroll"), 2},
        {QStringLiteral("fileCopyPageScroll"), 4}
    };
    for (const auto &page : pages) {
        window.m_tabs->setCurrentIndex(page.second);
        QCoreApplication::processEvents();
        auto *scroll = window.findChild<QScrollArea *>(page.first);
        QVERIFY2(scroll, qPrintable(page.first));
        QVERIFY(scroll->widget());
        QCOMPARE(scroll->horizontalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);
        QCOMPARE(scroll->alignment(), Qt::AlignLeft | Qt::AlignTop);
        QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);

        const QRect settled = scroll->widget()->geometry();
        QVERIFY(settled.width() > 0);
        QVERIFY(settled.width() <= scroll->viewport()->width());
        const int settledX = settled.x();
        QTest::qWait(50);
        QCOMPARE(scroll->widget()->geometry(), settled);

        // Repeated tab/layout changes at the narrow width must not move the
        // page content into a hidden horizontal-scroll offset.
        window.m_tabs->setCurrentIndex(0);
        window.m_tabs->setCurrentIndex(page.second);
        QCoreApplication::processEvents();
        QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
        QCOMPARE(scroll->widget()->geometry().x(), settledX);
    }
}

void MainWindowUiTest::applicationDialogsFollowConsistentLayout()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QVERIFY(window.m_fileCopyDirectionCombo);
    QVERIFY(window.m_destinationEdit);

    // The repaired target is intentionally virtual/unmounted, so the custom
    // destination dialog is the deterministic app-owned modal we can exercise
    // without authorizing a privileged operation.  Capture its layout before
    // rejecting it and verify the shared KDE-style spacing contract.
    window.m_previewTargetPath = QStringLiteral("/dev/test-data-disk");
    window.m_fileCopyDirectionCombo->setCurrentIndex(0);
    QMargins margins;
    int spacing = -1;
    bool hasButtons = false;
    QTimer poll;
    poll.setInterval(5);
    connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(widget);
            if (!dialog || dialog->objectName() != QStringLiteral("repairDestinationDialog")) {
                continue;
            }
            if (auto *layout = dialog->layout()) {
                margins = layout->contentsMargins();
                spacing = layout->spacing();
            }
            hasButtons = dialog->findChild<QDialogButtonBox *>() != nullptr;
            poll.stop();
            dialog->reject();
            return;
        }
    });
    poll.start();
    window.browseFileCopyDestination();
    QVERIFY(hasButtons);
    QCOMPARE(margins.left(), 20);
    QCOMPARE(margins.right(), 20);
    QCOMPARE(margins.top(), 16);
    QCOMPARE(margins.bottom(), 16);
    QCOMPARE(spacing, 10);

    // The clear affordance supplied by QLineEdit must remain vertically
    // centered when the shared button normalization runs.  This guards the
    // small-window regression where the icon appeared pinned to the bottom.
    QVERIFY(window.m_logSearchEdit);
    window.m_tabs->setCurrentIndex(6);
    window.m_logSearchEdit->setText(QStringLiteral("theme"));
    QCoreApplication::processEvents();
    const auto clearButtons = window.m_logSearchEdit->findChildren<QToolButton *>();
    QVERIFY(!clearButtons.isEmpty());
    const QToolButton *clearButton = clearButtons.constFirst();
    QVERIFY(clearButton->isVisible());
    const int centerDelta = std::abs(clearButton->geometry().center().y()
                                     - window.m_logSearchEdit->rect().center().y());
    QVERIFY2(centerDelta <= window.m_logSearchEdit->height() / 4,
             "QLineEdit clear button is not vertically centered");
}

void MainWindowUiTest::snapshotRowsFitSingleLineContent()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QVERIFY(window.m_snapshotTable);
    QVERIFY(window.m_snapshotSplitter);
    QCOMPARE(window.m_snapshotSplitter->orientation(), Qt::Vertical);
    QCOMPARE(window.m_snapshotSplitter->count(), 2);
    QVERIFY(!window.m_snapshotSplitter->childrenCollapsible());
    QCOMPARE(window.m_snapshotSplitter->widget(0), window.m_snapshotTable);
    QCOMPARE(window.m_snapshotSplitter->widget(1), window.m_snapshotDetails);
    QCOMPARE(window.m_snapshotTable->verticalScrollBarPolicy(), Qt::ScrollBarAlwaysOn);
    window.m_tabs->setCurrentIndex(3);
    window.resize(1400, 900);
    QCoreApplication::processEvents();

    // Populate representative one-line rows after the table is visible. This
    // guards against asynchronous snapshot loading measuring rows while the
    // page is hidden and retaining a tall wrapped height.
    window.m_snapshotTable->setColumnWidth(0, 90);
    window.m_snapshotTable->setColumnWidth(1, 220);
    window.m_snapshotTable->setColumnWidth(2, 90);
    window.m_snapshotTable->setColumnWidth(3, 240);
    window.m_snapshotTable->setColumnWidth(4, 280);
    window.m_snapshotTable->setRowCount(1);
    window.m_snapshotTable->setItem(0, 0, new QTableWidgetItem(QStringLiteral("14")));
    window.m_snapshotTable->setItem(0, 1, new QTableWidgetItem(QStringLiteral("2026-09-10")));
    window.m_snapshotTable->setItem(0, 2, new QTableWidgetItem(QStringLiteral("pre")));
    window.m_snapshotTable->setItem(0, 3, new QTableWidgetItem(QStringLiteral("apt")));
    window.m_snapshotTable->setItem(0, 4, new QTableWidgetItem(QStringLiteral("Linux root")));
    window.resizeSnapshotRows();

    const int expected = QFontMetrics(window.m_snapshotTable->font()).lineSpacing() + 10;
    QVERIFY2(window.m_snapshotTable->rowHeight(0) <= expected + 2,
             "single-line snapshot content should use a compact row height");
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

void MainWindowUiTest::individualDiagnosticEnablesMatchingRepair()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = disk.path;
    window.m_targetDiagnosticCache.insert(QStringLiteral("grub"), QStringLiteral("Diagnostic: grub\nPASS"));

    QVERIFY(window.m_repairToolTree);
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        auto *item = window.m_repairToolTree->topLevelItem(row);
        if (item->data(0, Qt::UserRole).toString() == QStringLiteral("grub")) {
            window.m_repairToolTree->setCurrentItem(item);
            break;
        }
    }
    window.updateRepairToolDetails();
    QVERIFY(window.m_repairToolButton);
    QVERIFY2(window.m_repairToolButton->isEnabled(),
             "A fresh matching target diagnostic must enable its individual repair action");
}

void MainWindowUiTest::fullRepairUsesFreshEvidenceForSelectedStage()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = disk.path;
    window.m_targetDiagnosticCache.insert(QStringLiteral("display"), QStringLiteral("Diagnostic: display\nPASS"));

    QVERIFY(window.m_fullRepairDisplayManager);
    window.m_fullRepairDpkg->setChecked(false);
    window.m_fullRepairBrokenPackages->setChecked(false);
    window.m_fullRepairAptUpdate->setChecked(false);
    window.m_fullRepairUpgrade->setChecked(false);
    window.m_fullRepairDkms->setChecked(false);
    window.m_fullRepairDisplayManager->setChecked(true);
    window.m_fullRepairInitramfs->setChecked(false);
    window.m_fullRepairEfi->setChecked(false);
    window.m_fullRepairGrub->setChecked(false);
    window.updateFullRepairSummary();

    QVERIFY2(window.m_runFullRepairButton->isEnabled(),
             "Full Repair should use fresh evidence for the selected display-manager stage");
}

void MainWindowUiTest::displayManagerToolUsesGenericLanguage()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_repairToolTree);
    QTreeWidgetItem *displayItem = nullptr;
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        auto *item = window.m_repairToolTree->topLevelItem(row);
        if (item->data(0, Qt::UserRole).toString() == QStringLiteral("display")) {
            displayItem = item;
            break;
        }
    }
    QVERIFY(displayItem);
    QVERIFY(displayItem->text(0).contains(QStringLiteral("display manager"), Qt::CaseInsensitive));
    QVERIFY(!displayItem->text(0).contains(QStringLiteral("SDDM"), Qt::CaseInsensitive));

    window.m_repairToolTree->setCurrentItem(displayItem);
    window.updateRepairToolDetails();
    QVERIFY(window.m_repairToolTitle);
    QVERIFY(window.m_repairToolDescription);
    QVERIFY(window.m_repairToolTitle->text().contains(QStringLiteral("display manager"), Qt::CaseInsensitive));
    const QString description = window.m_repairToolDescription->text();
    QVERIFY(description.contains(QStringLiteral("SDDM")));
    QVERIFY(description.contains(QStringLiteral("GDM3")));
    QVERIFY(description.contains(QStringLiteral("LightDM")));
    QVERIFY(description.contains(QStringLiteral("boot evidence"), Qt::CaseInsensitive));
}

void MainWindowUiTest::repairInvalidationExplainsDiagnosticRerun()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = disk.path;
    window.m_targetDiagnosticsNeedRegeneration = true;

    window.updateFullRepairSummary();
    QVERIFY(window.m_runFullRepairButton);
    QVERIFY(!window.m_runFullRepairButton->isEnabled());
    QVERIFY(window.m_fullRepairReadinessLabel);
    QVERIFY(window.m_fullRepairReadinessLabel->text().contains(
        QStringLiteral("after the last diagnostic or repair action")));
    QVERIFY(window.m_fullRepairReadinessLabel->text().contains(
        QStringLiteral("regenerate diagnostics")));
}

void MainWindowUiTest::logSearchFiltersApplicationLog()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_logView);
    QVERIFY(window.m_logSearchEdit);

    const QString matching = QStringLiteral("UI_TEST_FuzzyDiagnosticMarker");
    const QString unrelated = QStringLiteral("UI_TEST_UnrelatedMarker");
    window.appendLog(matching);
    window.appendLog(unrelated);
    QVERIFY(window.m_logView->toPlainText().contains(matching));
    QVERIFY(window.m_logView->toPlainText().contains(unrelated));

    // The characters f-z-y occur in order in “Fuzzy”, so this exercises the
    // forgiving subsequence matcher rather than an exact substring search.
    window.m_logSearchEdit->setText(QStringLiteral("fzy diagnostic"));
    const QString filtered = window.m_logView->toPlainText();
    QVERIFY(filtered.contains(matching));
    QVERIFY(!filtered.contains(unrelated));

    window.m_logSearchEdit->clear();
    const QString restored = window.m_logView->toPlainText();
    QVERIFY(restored.contains(matching));
    QVERIFY(restored.contains(unrelated));
}

void MainWindowUiTest::logViewShowsNewestEntriesFirst()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const QString older = QStringLiteral("UI_TEST_LOG_OLDER_ENTRY");
    const QString newer = QStringLiteral("UI_TEST_LOG_NEWER_ENTRY");
    window.appendLog(older);
    window.appendLog(newer);

    const QString visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(older));
    QVERIFY(visible.contains(newer));
    QVERIFY(visible.indexOf(newer) < visible.indexOf(older));
    QVERIFY(window.m_logView->verticalScrollBar()->value() <= 2);

    // When target evidence is invalidated (for example after editing
    // /etc/default/grub), the complete affected diagnostic entry is marked,
    // not only the one-line stale warning.
    window.m_targetDiagnosticsNeedRegeneration = true;
    window.appendLog(QStringLiteral("Diagnostic: grub\nScope: Repair Target\nGRUB configuration evidence"));
    QVERIFY2(window.m_logView->extraSelections().size() >= 3,
             "stale target diagnostic sections should be visually marked as a whole");
}

void MainWindowUiTest::nonLinuxTargetIsRejectedSafely()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-data-disk");
    disk.type = QStringLiteral("disk");
    disk.model = QStringLiteral("Test data disk");
    window.m_deviceIndex.insert(disk.path, disk);

    // A disk with no Linux filesystem and no encrypted Linux candidate must
    // never expose Select Target; this also protects the click path from
    // stale/default-constructed device records during udev refreshes.
    window.showDeviceDetails(disk, true, &disk);
    QVERIFY(window.m_setTargetButton);
    QVERIFY(!window.m_setTargetButton->isEnabled());
    QVERIFY(window.m_setTargetButton->toolTip().contains(QStringLiteral("No Linux filesystem")));

    // A locked encrypted data volume must remain unlock-only until a Linux
    // root is visible.  It must never be committed as an unusable target.
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
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
}

void MainWindowUiTest::guardedWriteActionsStayDisabledWithoutTarget()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    // No target is selected in the test environment. The write-capable repair,
    // copy, snapshot inspection, and rollback controls must remain guarded.
    QVERIFY(window.m_runFullRepairButton);
    QVERIFY(!window.m_runFullRepairButton->isEnabled());
    QVERIFY(window.m_repairToolButton);
    QVERIFY(!window.m_repairToolButton->isEnabled());
    QVERIFY(window.m_fileCopyPreviewButton);
    QVERIFY(!window.m_fileCopyPreviewButton->isEnabled());
    QVERIFY(window.m_fileCopyRunButton);
    QVERIFY(!window.m_fileCopyRunButton->isEnabled());
    QVERIFY(window.m_fileCopyDirectionCombo);
    window.m_fileCopyDirectionCombo->setCurrentIndex(1);
    window.m_fileCopyDirectionCombo->setCurrentIndex(0);
    QVERIFY(window.m_saveDiagnosticButton);
    QVERIFY(!window.m_saveDiagnosticButton->isEnabled());
    QVERIFY(window.m_snapshotLoadButton);
    QVERIFY(!window.m_snapshotLoadButton->isEnabled());
    QVERIFY(window.m_snapshotInspectButton);
    QVERIFY(!window.m_snapshotInspectButton->isEnabled());
    QVERIFY(window.m_snapshotRollbackButton);
    QVERIFY(!window.m_snapshotRollbackButton->isEnabled());

    // Changing File Copy direction clears staged paths and records the user
    // action without invoking a helper or modifying a file.
    QVERIFY(window.m_fileCopyDirectionCombo);
    QVERIFY(window.m_sourceList);
    QVERIFY(window.m_destinationEdit);
    window.m_fileCopyDirectionCombo->setCurrentIndex(1);
    QCOMPARE(window.m_sourceList->count(), 0);
    QVERIFY(window.m_destinationEdit->text().isEmpty());
    window.m_fileCopyDirectionCombo->setCurrentIndex(0);
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("File Copy direction changed")));

    // Clicking guarded controls without a committed target must be a no-op;
    // no privileged request or write confirmation is allowed to appear.
    window.m_fileCopyPreviewButton->click();
    window.m_fileCopyRunButton->click();
    window.m_runFullRepairButton->click();
    window.m_repairToolButton->click();
    window.m_snapshotLoadButton->click();
    window.m_snapshotRollbackButton->click();
    QVERIFY(!window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Privileged action started:")));
}

void MainWindowUiTest::exportDialogsCanBeCancelledReadOnly()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);
    window.m_diagnosticScopeCombo->setCurrentIndex(1);
    window.m_diagnosticList->setCurrentRow(0);
    window.runSelectedDiagnostic();
    QVERIFY(window.m_diagnosticResults && !window.m_diagnosticResults->toPlainText().isEmpty());

    // Saving an export writes only a user-selected report file. Exercise both
    // modal export actions and cancel their dialogs so this test never writes
    // outside its temporary settings directory.
    auto cancelNextFileDialog = [] {
        QTimer *poll = new QTimer(qApp);
        poll->setInterval(10);
        QObject::connect(poll, &QTimer::timeout, poll, [poll] {
            for (QWidget *top : QApplication::topLevelWidgets()) {
                if (auto *dialog = qobject_cast<QFileDialog *>(top)) {
                    poll->stop();
                    dialog->reject();
                    poll->deleteLater();
                    return;
                }
            }
        });
        poll->start();
    };

    cancelNextFileDialog();
    window.saveDiagnosticResults();
    cancelNextFileDialog();
    window.saveLogAs();
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Host diagnostic")));
}

void MainWindowUiTest::actionRegisterPersistsAcrossWindows()
{
    const QString marker = QStringLiteral("UI_TEST_REGISTER_MARKER");
    {
        MainWindow window;
        window.appendLog(marker, QStringLiteral("TEST"));
        QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
        window.m_tabs->setCurrentIndex(5);
    }

    MainWindow reopened;
    QCOMPARE(reopened.m_tabs->currentIndex(), 0);
    QVERIFY(reopened.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
    QVERIFY(reopened.m_logView->toPlainText().contains(marker));
}

int main(int argc, char **argv)
{
    QTemporaryDir config;
    if (!config.isValid()) {
        return 2;
    }
    qputenv("XDG_CONFIG_HOME", config.path().toUtf8());
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    MainWindowUiTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "MainWindowUiTest.moc"
