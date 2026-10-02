// Part of the boot-repair-ui-tests read-only regression suite: repair plans, change status, result summaries and authorization.
// The class declaration lives in MainWindowUiTest.h and the shared
// fixtures/helpers in UiTestHelpers.h; this file holds the repair plans, change status, result summaries and authorization test
// implementations so the suite compiles as parallel translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

using namespace UiTestDetail;

// Test seam compiled into boot-repair-ui-tests only (see MainWindow.cpp).
QString hostDefaultVerifiedEntryIdForTest(const QString &output);

// Fedora BIOS already-correct saved_entry must be reported as informational
// success ("unchanged"), never as an error. The helper proves the default is
// the running kernel's BLS id and emits only keyed evidence plus the unchanged
// change status; the GUI parser must extract the BLS id from those lines so
// the Make Default result stays verified.
void MainWindowUiTest::hostDefaultFedoraUnchangedParsesAsVerified()
{
    const QString target = QStringLiteral("cfe2564d1eaf4bd88faaff5dd35b5e31-7.2.8-200.fc44.x86_64");

    const QString unchangedOutput = QStringLiteral(
        "[12:00:01] msg:fedora-default-unchanged|param:%1|param:%1\n"
        "Repair change status host-default: unchanged|reason:fedora-saved-entry-already-correct|param:%1\n")
        .arg(target);
    QCOMPARE(hostDefaultVerifiedEntryIdForTest(unchangedOutput), target);

    // The written+read-back path reports the same id through msg:fedora-default-set
    // and -set-current and must verify as well.
    const QString changedOutput = QStringLiteral(
        "[12:00:02] msg:fedora-default-set|param:stale-entry|param:no|param:%1\n"
        "[12:00:03] msg:fedora-default-set-current|param:%1|param:%1\n"
        "Repair change status host-default: changed|reason:fedora-saved-entry-set|param:%1\n")
        .arg(target);
    QCOMPARE(hostDefaultVerifiedEntryIdForTest(changedOutput), target);

    // A transcript that names no verified entry must still fail closed.
    QCOMPARE(hostDefaultVerifiedEntryIdForTest(QStringLiteral(
                 "Repair change status host-default: unchanged|reason:host-default-already-correct\n")),
             QString());
}

void MainWindowUiTest::repairChangeStatusInvalidatesMappedSectionsOrFailsSafe()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("changed-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                    {QStringLiteral("display-manager=changed")}),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
             "a changed repair must mark its mapped display section stale");
    QCOMPARE(window.m_targetDiagnosticsStaleSections.size(), 1);
    QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
             "a narrow changed repair must not invalidate the complete set");
    QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the stale display section must be dropped from the cache");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "unrelated cached sections must remain valid");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("STALE DIAGNOSTICS")),
             "a changed repair must emit the stale notice");
    QVERIFY2(log.contains(QStringLiteral("Repair result: ✓ display manager repaired — changes were applied")),
             "a changed repair must report the success category");
    QVERIFY2(!log.contains(QStringLiteral("No system changes were detected")),
             "a changed repair must not claim that nothing changed");
    // The summary is the newest entry of the replaceable repair section, so it
    // renders above the captured helper output.
    flushLogRefresh(window);
    const QString visible = window.m_logView->toPlainText();
    const int summaryIndex = visible.indexOf(QStringLiteral("Repair result: ✓ display manager repaired"));
    const int outputIndex = visible.indexOf(
        QStringLiteral("Repair output\nDiagnostic: Restore detected graphical login manager"));
    QVERIFY2(summaryIndex >= 0 && outputIndex >= 0 && summaryIndex < outputIndex,
             "the repair summary must render above the captured output");

    // --- merged from missingRepairChangeStatusInvalidates() ---
    {
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());

        MainWindow window;
        window.show();
        QTest::qWait(50);
        prepareRepairScope(window);
        window.m_snapshotPreloadScheduled = true;
        window.m_autoRefreshDiagnostics->setChecked(false);
        cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

        QTemporaryDir requestDir;
        QVERIFY(requestDir.isValid());
        const QString capturePath = requestDir.filePath(QStringLiteral("missing-status-requests.log"));
        QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath, QStringList()),
                 "the scripted privileged session must start");
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
        QVERIFY(item);
        window.m_repairToolTree->setCurrentItem(item);
        window.runSelectedRepairTool();

        QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
                 "a missing change status must fail safe and invalidate the mapped section");
        QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
                 "the fail-safe invalidation must drop the cached section");
        const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QVERIFY2(!log.contains(QStringLiteral("No system changes were detected")),
                 "a missing status must never be reported as unchanged");
        QVERIFY2(log.contains(QStringLiteral("Repair result: ✓ display manager repaired — changes were applied")),
                 "a missing status keeps the fail-safe success category, matching the invalidation decision");
    }
}

// The Full Repair progress dialog must appear the moment the plan starts —
// before the read-only file system check pre-stage completes — and the
// pre-stage output must stream into it. The plan-owned dialog stays open
// across the pre-stage inspection, the per-device repairs and the remaining
// helper stages, and becomes closable only when the plan ends.
void MainWindowUiTest::fullRepairProgressDialogShowsAtPlanStartBeforePreStageCompletes()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-dialog-requests.log"));
    // Delay the inspection so the plan dialog's existence can be asserted
    // while the pre-stage is still in flight.
    QVERIFY2(startMultiDeviceFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("0.5")),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    // The plan-owned dialog becomes closable when the plan ends; dismiss it
    // then so the run completes headless.
    closeRepairProgressDialogWhenDone(&window);

    bool sawPlanDialogBeforePreStageComplete = false;
    bool sawSelectionDialog = false;
    bool planDialogStreamedInspectOutput = false;

    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        // While the pre-stage runs, the plan-owned progress dialog must
        // already exist and be visible, and the post-inspection selection
        // dialog must not exist yet.
        if (window.m_fullRepairProgressDialog
            && window.m_fullRepairProgressDialog->isVisible()) {
            bool selectionVisible = false;
            for (QWidget *top : QApplication::topLevelWidgets()) {
                auto *dialog = qobject_cast<QDialog *>(top);
                if (dialog && dialog->isVisible()
                    && dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                    selectionVisible = true;
                    break;
                }
            }
            if (!selectionVisible) {
                sawPlanDialogBeforePreStageComplete = true;
            }
        }
        // Drive the post-inspection device/mode selection dialog once it
        // appears; at that point the pre-stage output must already be
        // streamed into the plan dialog pane.
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 2) {
                continue;
            }
            sawSelectionDialog = true;
            if (window.m_fullRepairProgressDialog
                && window.m_fullRepairProgressDialog->outputPane()) {
                planDialogStreamedInspectOutput =
                    window.m_fullRepairProgressDialog->outputPane()->toPlainText()
                        .contains(QStringLiteral("File system check"));
            }
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
            return;
        }
    });
    poll.start();

    QTimer safety;
    safety.setSingleShot(true);
    safety.setInterval(8000);
    QObject::connect(&safety, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (dialog && dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                dialog->reject();
            }
        }
    });
    safety.start();

    // The plan confirmation and the per-device repair confirmation.
    acceptNextMessageBoxes(&window, 2, QMessageBox::Yes);

    window.runFullRepair();
    poll.stop();

    QVERIFY2(sawPlanDialogBeforePreStageComplete,
             "the Full Repair progress dialog must exist before the read-only "
             "file system check pre-stage completes");
    QVERIFY2(sawSelectionDialog,
             "the plan must still reach the device/mode selection dialog");
    QVERIFY2(planDialogStreamedInspectOutput,
             "the pre-stage output must stream into the plan-owned progress dialog");
    QVERIFY2(!window.m_fullRepairPlanInProgress, "the plan must finish");
    QVERIFY2(window.m_activeBusyOperations.isEmpty(), "the plan must release the busy state");
    QVERIFY2(capturedHelperArguments(capturePath).contains(QStringLiteral("fs-inspect")),
             "the pre-stage inspection must have run");
}

// Regression for the Qt 6.4 host-maintenance crash: a second privileged
// request that arrives while the first pkexec/Polkit prompt is still open must
// coalesce onto the in-flight request through a NON-nested wait. The previous
// implementation waited in a stack-local QEventLoop::exec(), which re-enters
// the event loop and can crash on Qt 6.4; the fix polls with
// QCoreApplication::processEvents() instead. This drives the coalescing branch
// deterministically offscreen (the real QProcess is seamed out): the test marks
// an owning request in flight, schedules its outcome, then asserts the
// coalescing caller returns promptly with the published outcome and no
// nested-loop hang.
void MainWindowUiTest::privilegedSessionCoalescingWaitDoesNotNestEventLoop()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // Success outcome: the owning pkexec conversation resolves while the
    // second caller is waiting, publishing a usable session.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_autoRefreshDiagnostics->setChecked(false);
        window.m_uiTestPrivilegedSessionGranted = true;

        // Mark the owning request in flight and schedule its outcome so the
        // coalescing caller observes it during the processEvents poll.
        window.m_privilegedSessionRequestInFlight = true;
        QVERIFY(!window.privilegedSessionUsable());
        QTimer::singleShot(30, &window, [&window] {
            window.m_privilegedSessionReady = true;
            window.m_privilegedSessionRequestInFlight = false;
        });

        QElapsedTimer timer;
        timer.start();
        QString error;
        const bool ok = window.ensurePrivilegedSession(&error);
        const qint64 elapsed = timer.elapsed();

        QVERIFY2(ok, "a coalescing caller must observe the owning request's success");
        QVERIFY2(error.isEmpty(), "a coalesced success must carry no error");
        QVERIFY(window.privilegedSessionUsable());
        QVERIFY2(elapsed < 1000,
                 "the coalescing wait must return promptly without nesting");
        window.closePrivilegedSession();
    }

    // Failure outcome: the owning request is cancelled while the second caller
    // waits; the caller must stop waiting as soon as the failure is published
    // and report it, without spinning.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_autoRefreshDiagnostics->setChecked(false);
        window.m_uiTestPrivilegedSessionGranted = false;

        window.m_privilegedSessionRequestInFlight = true;
        QVERIFY(!window.privilegedSessionUsable());
        QTimer::singleShot(30, &window, [&window] {
            window.m_privilegedSessionRequestFailed = true;
            window.m_privilegedSessionRequestInFlight = false;
        });

        QElapsedTimer timer;
        timer.start();
        QString error;
        const bool ok = window.ensurePrivilegedSession(&error);
        const qint64 elapsed = timer.elapsed();

        QVERIFY2(!ok, "a coalescing caller must observe the owning request's failure");
        QVERIFY2(!error.isEmpty(), "a coalesced failure must surface the authorization error");
        QVERIFY2(elapsed < 1000,
                 "the coalescing wait must return promptly without nesting");
        window.closePrivilegedSession();
    }
}

