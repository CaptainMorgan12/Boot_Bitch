// Part of the boot-repair-ui-tests read-only regression suite: log viewing and filtering.
// The class declaration lives in MainWindowUiTest.h and the shared
// fixtures/helpers in UiTestHelpers.h; this file holds the log viewing and filtering test
// implementations so the suite compiles as parallel translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

using namespace UiTestDetail;


void MainWindowUiTest::logSectionFilterShowsRelatedRepairsAndDiagnosticSection()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString kernelSection = QStringLiteral(
        "========================================\n"
        "Diagnostic: kernel\n"
        "Title: Kernel / initramfs\n"
        "Scope: Running Host\n"
        "What was actually run: Boot Bitch host diagnostic implementation (read-only): kernel\n"
        "Status: completed\n"
        "========================================\n"
        "KERNEL_EVIDENCE_MARKER vmlinuz and initramfs image inspected");
    window.appendLog(kernelSection, QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    window.appendLog(QStringLiteral("Starting privileged action: Rebuild initramfs on /dev/test-host (/dev/test-host-root)."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nINITRAMFS_REPAIR_MARKER images rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Rebuild initramfs finished with exit code 0 (success)."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS\nDKMS_REPAIR_MARKER modules rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Refresh package metadata\nAPT_REPAIR_MARKER metadata refreshed"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Regenerate GRUB configuration\nGRUB_REPAIR_MARKER regenerated"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Repair EFI / UKI bootloader\nEFI_REPAIR_MARKER reinstalled"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const int kernelIndex = window.m_logFilterCombo->findData(QStringLiteral("@kernel"));
    QVERIFY(kernelIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(kernelIndex);
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: kernel")),
             "the kernel diagnostic section must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Title: Kernel / initramfs")),
             "the complete section framing must be shown");
    QVERIFY2(visible.contains(QStringLiteral("KERNEL_EVIDENCE_MARKER")),
             "the complete section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("INITRAMFS_REPAIR_MARKER")),
             "initramfs repair entries belong to the Kernel / initramfs filter");
    QVERIFY2(visible.contains(QStringLiteral("Rebuild initramfs finished with exit code 0")),
             "the complete initramfs repair cycle must be shown");
    QVERIFY2(visible.contains(QStringLiteral("DKMS_REPAIR_MARKER")),
             "dkms repair entries belong to the Kernel / initramfs filter");
    QVERIFY2(!visible.contains(QStringLiteral("APT_REPAIR_MARKER")),
             "unrelated package repairs must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("GRUB_REPAIR_MARKER")),
             "unrelated grub repairs must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the Kernel / initramfs filter must not fall back to the empty notice");

    const int grubIndex = window.m_logFilterCombo->findData(QStringLiteral("@grub"));
    QVERIFY(grubIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(grubIndex);
    const QString grubVisible = window.m_logView->toPlainText();
    QVERIFY(grubVisible.contains(QStringLiteral("GRUB_REPAIR_MARKER")));
    QVERIFY(!grubVisible.contains(QStringLiteral("INITRAMFS_REPAIR_MARKER")));
    QVERIFY(!grubVisible.contains(QStringLiteral("DKMS_REPAIR_MARKER")));

    // EFI / UKI pairs with the efi repair workflow.
    const int ukiIndex = window.m_logFilterCombo->findData(QStringLiteral("@uki"));
    QVERIFY(ukiIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(ukiIndex);
    const QString ukiVisible = window.m_logView->toPlainText();
    QVERIFY(ukiVisible.contains(QStringLiteral("EFI_REPAIR_MARKER")));
    QVERIFY(!ukiVisible.contains(QStringLiteral("GRUB_REPAIR_MARKER")));
    QVERIFY(!ukiVisible.contains(QStringLiteral("INITRAMFS_REPAIR_MARKER")));

    // Environment pairs with the package-stage repair workflows.
    const int environmentIndex = window.m_logFilterCombo->findData(QStringLiteral("@environment"));
    QVERIFY(environmentIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(environmentIndex);
    const QString environmentVisible = window.m_logView->toPlainText();
    QVERIFY(environmentVisible.contains(QStringLiteral("APT_REPAIR_MARKER")));
    QVERIFY(!environmentVisible.contains(QStringLiteral("EFI_REPAIR_MARKER")));
    QVERIFY(!environmentVisible.contains(QStringLiteral("DKMS_REPAIR_MARKER")));

    // Prior sessions are filtered and expanded identically.
    const QString priorPath = logDir.path() + QStringLiteral("/session-20260101-000000.log");
    {
        QFile file(priorPath);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate));
        QTextStream stream(&file);
        stream << "──────── DIAGNOSTIC ────────\n"
               << kernelSection << "\n"
               << "──────── REPAIR ────────\n"
               << "[2026-01-01 00:00:02] [INFO] Repair output\n"
               << "Diagnostic: Rebuild initramfs\n"
               << "PRIOR_INITRAMFS_REPAIR_MARKER rebuilt\n";
    }
    window.displaySessionLog(priorPath);
    QVERIFY(window.m_viewingPriorLog);
    window.m_logFilterCombo->setCurrentIndex(kernelIndex);
    const QString priorVisible = window.m_logView->toPlainText();
    QVERIFY2(priorVisible.contains(QStringLiteral("KERNEL_EVIDENCE_MARKER")),
             "prior sessions must show the kernel diagnostic section");
    QVERIFY2(priorVisible.contains(QStringLiteral("PRIOR_INITRAMFS_REPAIR_MARKER")),
             "prior sessions must show the related repair entries");
    QVERIFY2(!priorVisible.contains(QStringLiteral("GRUB_REPAIR_MARKER")),
             "prior sessions must keep unrelated repairs hidden");
}

