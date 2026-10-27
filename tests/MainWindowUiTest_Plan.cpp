// Part of the boot-repair-ui-tests read-only regression suite: repair plans, change status, result summaries and authorization.
// The class declaration lives in MainWindowUiTest.h and the shared
// fixtures/helpers in UiTestHelpers.h; this file holds the repair plans, change status, result summaries and authorization test
// implementations so the suite compiles as parallel translation units.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

#include <algorithm>

using namespace UiTestDetail;

// The Full Repair plan stage rows must be TIGHT and UNIFORM: every row's height
// is its own wrapped content height (the earlier ResponsiveColumnsLayout floor
// inflated one-line rows to their wrapped-at-minimum-width height, leaving 2–3
// blank lines between rows), the gaps between rows are constant, and the
// two-column (wide) layout is meaningfully shorter than the single-column
// (narrow) layout.
void MainWindowUiTest::fullRepairPlanRowsAreTightAndUniform()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QLayout *stageLayout = window.m_fullRepairStageList->layout();
    QVERIFY2(stageLayout && stageLayout->hasHeightForWidth(),
             "the plan stages must use a responsive (two-column) columns layout");
    QVERIFY2(stageLayout->count() >= 4, "the plan must list several stages");

    const auto rowTop = [](QWidget *w) { return w->y(); };
    const auto rowBottom = [](QWidget *w) { return w->y() + w->height(); };
    struct Row { int top = 0; int bottom = 0; };
    auto collectRows = [&]() -> QList<Row> {
        QList<Row> rows;
        for (int i = 0; i < stageLayout->count(); ++i) {
            QWidget *w = stageLayout->itemAt(i)->widget();
            if (!w) {
                continue;
            }
            Row *match = nullptr;
            for (Row &row : rows) {
                if (qAbs(row.top - rowTop(w)) <= 1) {
                    match = &row;
                    break;
                }
            }
            if (match) {
                match->bottom = qMax(match->bottom, rowBottom(w));
            } else {
                rows.append({rowTop(w), rowBottom(w)});
            }
        }
        std::sort(rows.begin(), rows.end(), [](const Row &a, const Row &b) {
            return a.top < b.top;
        });
        return rows;
    };

    // Wide: two side-by-side columns.
    const int wideContentWidth = window.m_fullRepairStageList->width();
    const int wideHeight = stageLayout->heightForWidth(wideContentWidth);
    QVERIFY2(wideHeight > 0, "the wide plan layout must have a positive height");
    {
        bool sideBySide = false;
        for (int i = 0; i < stageLayout->count() && !sideBySide; ++i) {
            for (int j = i + 1; j < stageLayout->count(); ++j) {
                QWidget *a = stageLayout->itemAt(i)->widget();
                QWidget *b = stageLayout->itemAt(j)->widget();
                if (a && b && qAbs(a->y() - b->y()) <= 1 && qAbs(a->x() - b->x()) > 40) {
                    sideBySide = true;
                    break;
                }
            }
        }
        QVERIFY2(sideBySide, "at a wide width the plan stages must wrap into side-by-side columns");

        // Every row is exactly its own wrapped content height (no blank lines).
        for (int i = 0; i < stageLayout->count(); ++i) {
            QWidget *w = stageLayout->itemAt(i)->widget();
            if (!w) {
                continue;
            }
            const int expected = w->hasHeightForWidth()
                ? w->heightForWidth(w->width())
                : w->sizeHint().height();
            QVERIFY2(qAbs(w->height() - expected) <= 2,
                     qPrintable(QStringLiteral("stage row %1 must be content-sized (got %2, want %3)")
                                    .arg(i).arg(w->height()).arg(expected)));
        }

        // The gaps between consecutive rows are uniform and tight (the single
        // column-spacing, never blank lines).
        const QList<Row> rows = collectRows();
        QList<int> gaps;
        for (int i = 1; i < rows.size(); ++i) {
            gaps.append(rows.at(i).top - rows.at(i - 1).bottom);
        }
        QVERIFY(!gaps.isEmpty());
        for (const int gap : gaps) {
            QVERIFY2(qAbs(gap - gaps.first()) <= 1,
                     qPrintable(QStringLiteral("row spacing must be uniform (gaps: %1)")
                                    .arg([&] {
                                        QStringList s;
                                        for (int g : gaps) s.append(QString::number(g));
                                        return s.join(QLatin1Char(','));
                                    }())));
            QVERIFY2(gap < 24,
                     qPrintable(QStringLiteral("row spacing must be tight, not blank lines (gap %1)").arg(gap)));
        }
    }

    // Narrow: the stages reflow to a single column and the section grows; the
    // two-column layout must be meaningfully shorter than the single-column one.
    window.resize(520, 760);
    QApplication::processEvents();
    QTest::qWait(50);
    const int narrowHeight = stageLayout->heightForWidth(window.m_fullRepairStageList->width());
    QVERIFY2(narrowHeight > 0, "the narrow plan layout must have a positive height");
    QVERIFY2(wideHeight < narrowHeight,
             qPrintable(QStringLiteral("two columns must shrink the section (wide=%1 narrow=%2)")
                            .arg(wideHeight).arg(narrowHeight)));
    QVERIFY2(wideHeight * 3 <= narrowHeight * 2 + 24,
             qPrintable(QStringLiteral("two columns must shrink the section roughly by half (wide=%1 narrow=%2)")
                            .arg(wideHeight).arg(narrowHeight)));

    // Narrow rows are single-column and equally tight.
    {
        const QList<Row> rows = collectRows();
        QVERIFY(rows.size() >= stageLayout->count());
        QList<int> gaps;
        for (int i = 1; i < rows.size(); ++i) {
            gaps.append(rows.at(i).top - rows.at(i - 1).bottom);
        }
        for (const int gap : gaps) {
            QVERIFY2(qAbs(gap - gaps.first()) <= 1, "narrow row spacing must be uniform");
            QVERIFY2(gap < 24, "narrow row spacing must be tight");
        }
    }
}



// The Full Repair plan frame must shrink to its (two-column) content instead of
// keeping a stale tall height, and the freed vertical space must go to the
// individual-tools/details pane below it.  The pre-two-column layout persisted
// its taller plan pane under the old splitter-state key; the key was bumped so
// that stale height is discarded and the plan re-adopts its shrink-to-content
// default.
void MainWindowUiTest::fullRepairPlanFrameShrinksToContentAndGrowsOutputPane()
{
    // Seed a stale pre-two-column splitter state under the OLD key (a plan pane
    // much taller than the repair/details pane).  The new layout reads a bumped
    // key, so this legacy height must be ignored.
    {
        QSettings legacy(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
        QSplitter staleSplitter(Qt::Vertical);
        QWidget stalePlan;
        QWidget staleOutput;
        staleSplitter.addWidget(&stalePlan);
        staleSplitter.addWidget(&staleOutput);
        staleSplitter.resize(1000, 1000);
        staleSplitter.setSizes({700, 300});
        legacy.setValue(QStringLiteral("repair/verticalSplitterStateV2"), staleSplitter.saveState());
        legacy.sync();
    }

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fullRepairPlanBox);
    QVERIFY(window.m_repairSplitter);
    const int planHeight = window.m_fullRepairPlanBox->height();
    const int planContentHeight = window.m_fullRepairPlanBox->minimumSizeHint().height();
    QVERIFY2(planContentHeight > 0, "the plan frame must have a measurable content height");
    // The frame is content-sized: at most a small tolerance above its own
    // shrink-to-content minimum, never the stale single-column height. The
    // tolerance also accepts the deliberate +1 visible-row bump below.
    QVERIFY2(planHeight <= planContentHeight + 40,
             qPrintable(QStringLiteral("the plan frame must shrink to its content "
                                       "(height %1 vs content %2)")
                            .arg(planHeight).arg(planContentHeight)));
    // The default plan-list minimum height derives from the list's ACTUAL
    // wrapped content height (ResponsiveColumnsLayout::heightForWidth), clamped
    // between a one-row floor and a four-row maximum, instead of a fixed
    // four-row frame.
    QVERIFY(window.m_fullRepairStageScroll);
    QVERIFY(window.m_fullRepairStageList);
    const int fallbackRowHeight = qMax(28, window.m_fullRepairStageScroll->fontMetrics().lineSpacing() + 12);
    int contentHeight = 0;
    if (window.m_fullRepairStageList->width() > 0) {
        QLayout *stageLayout = window.m_fullRepairStageList->layout();
        if (stageLayout && stageLayout->hasHeightForWidth()) {
            contentHeight = stageLayout->heightForWidth(window.m_fullRepairStageList->width());
        }
    }
    const int expectedMinimum = qBound(fallbackRowHeight, contentHeight, 4 * fallbackRowHeight);
    QVERIFY2(contentHeight > 0, "the plan list must have a measurable content height");
    QCOMPARE(window.m_fullRepairStageScroll->minimumHeight(), expectedMinimum);
    // The freed vertical space goes to the details/output pane below it.
    QVERIFY2(window.m_repairSplitter->height() > planHeight,
             qPrintable(QStringLiteral("the output/details pane (%1) must be taller than "
                                       "the plan frame (%2)")
                            .arg(window.m_repairSplitter->height()).arg(planHeight)));

    // Leave no stale legacy key behind for later tests.
    QSettings cleanup(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    cleanup.remove(QStringLiteral("repair/verticalSplitterStateV2"));
    cleanup.sync();
}

// The Full Repair result summary derives the module-build stage wording from the
// detected module build system instead of a hardcoded "DKMS". An Alpine (akms)
// target renders "AKMS modules rebuilt" (and never "DKMS"), while the default
// falls back to "DKMS".
void MainWindowUiTest::fullRepairSummaryNamesDetectedModuleBuildSystem()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    MainWindow::RepairStageResult stage;
    stage.toolKey = QStringLiteral("dkms");
    stage.category = MainWindow::RepairResultCategory::Success;

    // akms fixture: the cached backend evidence names the Alpine build system.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Module build system: akms\n");
    cacheRepairEvidence(window, evidence);

    const QString akmsSummary = window.fullRepairPlanSummary({stage});
    QVERIFY2(akmsSummary.contains(QStringLiteral("AKMS modules rebuilt")),
             qPrintable(akmsSummary));
    QVERIFY2(!akmsSummary.contains(QStringLiteral("DKMS modules rebuilt")),
             qPrintable(akmsSummary));

    // Default fixture: no module-build evidence falls back to DKMS.
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    const QString dkmsSummary = window.fullRepairPlanSummary({stage});
    QVERIFY2(dkmsSummary.contains(QStringLiteral("DKMS modules rebuilt")),
             qPrintable(dkmsSummary));
}

// The Full Repair plan checkbox (Settings) and the repair-tool table row
// (Repair) must name the detected module-build system as soon as the akms
// evidence is cached, never flashing the generic "DKMS"/"Rebuild DKMS modules"
// label for an Alpine (akms) target.
void MainWindowUiTest::fullRepairPlanLabelsNameDetectedModuleBuildSystem()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);

    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Module build system: akms\n");
    cacheRepairEvidence(window, evidence);

    // The single summary refresh is what re-derives both labels from the cached
    // evidence; neither label may keep a hardcoded "DKMS" value.
    window.updateFullRepairSummary();

    QVERIFY(window.m_fullRepairDkms);
    QCOMPARE(window.m_fullRepairDkms->text(), QStringLiteral("Rebuild AKMS modules"));

    QTreeWidgetItem *dkmsItem = repairItem(window, QStringLiteral("dkms"));
    QVERIFY(dkmsItem);
    QCOMPARE(dkmsItem->text(0), QStringLiteral("AKMS"));
}

// The DISPLAYED diagnostic/repair log rewrites the stable `dkms` module-build
// key token to the detected build-system name (AKMS / AKMODS / DKMS) so an
// Alpine (akms) target surfaces "AKMS" in the log, while the machine-parsed
// `dkms` key stays in the cached evidence. The non-dkms contract lines pass
// through verbatim.
void MainWindowUiTest::displayedLogNamesDetectedModuleBuildSystem()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    const QString transcript = QStringLiteral(
        "Repair tool dkms: available\n"
        "Repair capability evidence dkms: akms executable present; kernel 6.18.55-0-lts build tree present\n"
        "Repair change status dkms: unchanged|reason:module-build-no-modules\n"
        "Repair tool validate: available\n");

    // akms fixture: the cached backend evidence names the Alpine build system.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Module build system: akms\n");
    cacheRepairEvidence(window, evidence);

    const QString akmsDisplay = window.displayLocalizedHelperOutput(transcript);
    QVERIFY2(akmsDisplay.contains(QStringLiteral("Repair tool AKMS: available")),
             qPrintable(akmsDisplay));
    QVERIFY2(akmsDisplay.contains(QStringLiteral("Repair capability evidence AKMS: akms executable present")),
             qPrintable(akmsDisplay));
    QVERIFY2(akmsDisplay.contains(QStringLiteral("Repair change status AKMS: unchanged|reason:module-build-no-modules")),
             qPrintable(akmsDisplay));
    QVERIFY2(!akmsDisplay.contains(QStringLiteral("Repair tool dkms:")), qPrintable(akmsDisplay));
    QVERIFY2(!akmsDisplay.contains(QStringLiteral("Repair capability evidence dkms:")), qPrintable(akmsDisplay));
    QVERIFY2(!akmsDisplay.contains(QStringLiteral("Repair change status dkms:")), qPrintable(akmsDisplay));
    // The reason payload and every other contract line stay verbatim.
    QVERIFY2(akmsDisplay.contains(QStringLiteral("reason:module-build-no-modules")), qPrintable(akmsDisplay));
    QVERIFY2(akmsDisplay.contains(QStringLiteral("Repair tool validate: available")), qPrintable(akmsDisplay));

    // Fedora (akmods) surfaces the AKMODS label.
    QString akmodsEvidence = capabilityEvidence(false, true);
    akmodsEvidence += QStringLiteral("Module build system: akmods\n");
    cacheRepairEvidence(window, akmodsEvidence);
    const QString akmodsDisplay = window.displayLocalizedHelperOutput(transcript);
    QVERIFY2(akmodsDisplay.contains(QStringLiteral("Repair tool AKMODS: available")),
             qPrintable(akmodsDisplay));
    QVERIFY2(!akmodsDisplay.contains(QStringLiteral("Repair tool dkms:")), qPrintable(akmodsDisplay));

    // No module-build evidence (and a dkms backend) falls back to the DKMS
    // label, still replacing the lowercase key token.
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    const QString dkmsDisplay = window.displayLocalizedHelperOutput(transcript);
    QVERIFY2(dkmsDisplay.contains(QStringLiteral("Repair tool DKMS: available")),
             qPrintable(dkmsDisplay));
    QVERIFY2(!dkmsDisplay.contains(QStringLiteral("Repair tool dkms:")), qPrintable(dkmsDisplay));
}

// The Full Repair plan pane sizes to its ACTUAL stage content: a short plan
// (Alpine's ~6 stages in two columns) shrinks the plan list to ~2-3 rows and
// the individual-repair details pane below reclaims the freed vertical space.
void MainWindowUiTest::fullRepairPlanShrinksForFewStagesAndGrowsDetailsPane()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);

    // An Alpine target with only six configurable stages selected: filesystem,
    // fixbroken, dkms (akms), display, initramfs and extlinux. Everything else
    // is unavailable or deselected, so the plan lists a short stage set.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Module build system: akms\n");
    // Drop the Debian-only stages that an Alpine plan does not offer.
    evidence.replace(QStringLiteral("Repair tool dpkg: available"),
                     QStringLiteral("Repair tool dpkg: unavailable|reason:dpkg-not-on-alpine"));
    evidence.replace(QStringLiteral("Repair tool aptupdate: available"),
                     QStringLiteral("Repair tool aptupdate: unavailable|reason:aptupdate-not-on-alpine"));
    evidence.replace(QStringLiteral("Repair tool grub: available"),
                     QStringLiteral("Repair tool grub: unavailable|reason:not-grub-bootloader|param:syslinux/extlinux"));
    evidence.replace(QStringLiteral("Repair tool efi: available"),
                     QStringLiteral("Repair tool efi: unavailable|reason:syslinux-bios-no-efi"));
    // The base fixture marks display unavailable; Alpine's OpenRC display-manager
    // stage is available, so restore it.
    evidence.replace(QStringLiteral("Repair tool display: unavailable|No display manager installed"),
                     QStringLiteral("Repair tool display: available"));
    // filesystem and extlinux are available on Alpine BIOS.
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    evidence += QStringLiteral("Repair tool extlinux: available\n");
    cacheRepairEvidence(window, evidence);

    // Select exactly six stages regardless of the persisted defaults.
    const QHash<QString, bool> selections = {
        {QStringLiteral("filesystem"), true},
        {QStringLiteral("dpkg"), false},
        {QStringLiteral("fixbroken"), true},
        {QStringLiteral("aptupdate"), false},
        {QStringLiteral("upgrade"), false},
        {QStringLiteral("dkms"), true},
        {QStringLiteral("display"), true},
        {QStringLiteral("initramfs"), true},
        {QStringLiteral("efi"), false},
        {QStringLiteral("grub"), false},
        {QStringLiteral("extlinux"), true},
    };
    for (auto it = selections.constBegin(); it != selections.constEnd(); ++it) {
        window.m_fullRepairStagePreferences.insert(it.key(), it.value());
    }

    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fullRepairStageList);
    QLayout *stageLayout = window.m_fullRepairStageList->layout();
    QVERIFY2(stageLayout && stageLayout->hasHeightForWidth(),
             "the plan stages must use a responsive columns layout");
    // Six selected stages in two columns = three rows; no fixed four-row frame.
    QCOMPARE(stageLayout->count(), 6);

    const int fallbackRowHeight = qMax(28, window.m_fullRepairStageScroll->fontMetrics().lineSpacing() + 12);
    int contentHeight = 0;
    if (window.m_fullRepairStageList->width() > 0) {
        contentHeight = stageLayout->heightForWidth(window.m_fullRepairStageList->width());
    }
    QVERIFY2(contentHeight > 0, "the plan list must have a measurable content height");
    // The plan list minimum tracks the actual content height (not a fixed four
    // rows) and stays above the one-row floor.
    QCOMPARE(window.m_fullRepairStageScroll->minimumHeight(),
             qBound(fallbackRowHeight, contentHeight, 4 * fallbackRowHeight));
    QVERIFY2(window.m_fullRepairStageScroll->minimumHeight() < 4 * fallbackRowHeight,
             qPrintable(QStringLiteral("a short plan must drop below the four-row frame "
                                       "(minimum %1 vs four rows %2)")
                            .arg(window.m_fullRepairStageScroll->minimumHeight())
                            .arg(4 * fallbackRowHeight)));
    // The freed space goes to the individual-repair details pane below.
    QVERIFY2(window.m_repairSplitter->height() > window.m_fullRepairPlanBox->height(),
             qPrintable(QStringLiteral("the details pane (%1) must be taller than the "
                                        "plan frame (%2)")
                            .arg(window.m_repairSplitter->height())
                            .arg(window.m_fullRepairPlanBox->height())));
}

// A seven-stage Alpine BIOS plan (filesystem, fixbroken, upgrade, dkms/akms,
// display, initramfs, extlinux — the reported "7 stages, ~4 rows" case) must
// not leave trailing empty space below the last stage.  The plan frame's height
// is its fixed chrome (group-box frame, header, readiness hint, spacing) plus
// the stage list's actual wrapped content, never the inflated
// sizeHint()/minimumSizeHint() that reserves dead rows for a short list.
void MainWindowUiTest::fullRepairPlanFrameHasNoTrailingGapForSevenStages()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);

    // Alpine BIOS evidence: apk backend, akms, OpenRC display, extlinux; the
    // Debian-only and EFI/GRUB stages are unavailable so the plan lists the
    // seven Alpine stages.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Module build system: akms\n");
    evidence.replace(QStringLiteral("Repair tool dpkg: available"),
                     QStringLiteral("Repair tool dpkg: unavailable|reason:dpkg-not-on-alpine"));
    evidence.replace(QStringLiteral("Repair tool aptupdate: available"),
                     QStringLiteral("Repair tool aptupdate: unavailable|reason:aptupdate-not-on-alpine"));
    evidence.replace(QStringLiteral("Repair tool grub: available"),
                     QStringLiteral("Repair tool grub: unavailable|reason:not-grub-bootloader|param:syslinux/extlinux"));
    evidence.replace(QStringLiteral("Repair tool efi: available"),
                     QStringLiteral("Repair tool efi: unavailable|reason:syslinux-bios-no-efi"));
    evidence.replace(QStringLiteral("Repair tool display: unavailable|No display manager installed"),
                     QStringLiteral("Repair tool display: available"));
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    evidence += QStringLiteral("Repair tool extlinux: available\n");
    cacheRepairEvidence(window, evidence);

    const QHash<QString, bool> selections = {
        {QStringLiteral("filesystem"), true},
        {QStringLiteral("dpkg"), false},
        {QStringLiteral("fixbroken"), true},
        {QStringLiteral("aptupdate"), false},
        {QStringLiteral("upgrade"), true},
        {QStringLiteral("dkms"), true},
        {QStringLiteral("display"), true},
        {QStringLiteral("initramfs"), true},
        {QStringLiteral("efi"), false},
        {QStringLiteral("grub"), false},
        {QStringLiteral("extlinux"), true},
    };
    for (auto it = selections.constBegin(); it != selections.constEnd(); ++it) {
        window.m_fullRepairStagePreferences.insert(it.key(), it.value());
    }

    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fullRepairStageList);
    QVERIFY(window.m_fullRepairPlanBox);
    QVERIFY(window.m_fullRepairStageScroll);
    QLayout *stageLayout = window.m_fullRepairStageList->layout();
    QVERIFY2(stageLayout && stageLayout->hasHeightForWidth(),
             "the plan stages must use a responsive columns layout");
    QCOMPARE(stageLayout->count(), 7);

    // The frame height is the fixed chrome plus the clamped stage content.  The
    // chrome is measured as the sizeHint() difference (the scroll area's own
    // hint cancels out), so this is independent of the scroll area's inflated
    // size hint and asserts the frame never sits taller than its content.
    const int chromeHeight = window.m_fullRepairPlanBox->sizeHint().height()
                           - window.m_fullRepairStageScroll->sizeHint().height();
    const int expectedHeight = chromeHeight + window.m_fullRepairStageScroll->minimumHeight();
    QVERIFY2(expectedHeight > 0, "the plan frame must have a measurable content height");
    QVERIFY2(window.m_fullRepairPlanBox->height() <= expectedHeight + 4,
             qPrintable(QStringLiteral("the plan frame must not leave dead rows below the "
                                        "last stage (height %1 vs content %2)")
                            .arg(window.m_fullRepairPlanBox->height())
                            .arg(expectedHeight)));
    // The plan frame hugs its content with the deliberate Round-3 ~6 px
    // bottom padding after the last stage row (matching the Settings plan
    // box), so the last row's bottom sits 6 px plus the 1-3 px frame border
    // above the frame edge — never a dead empty row.
    QVERIFY(window.m_fullRepairPlanBox->layout());
    QCOMPARE(window.m_fullRepairPlanBox->layout()->contentsMargins().bottom(), 6);
    const int lastStageBottom = window.m_fullRepairStageList->mapTo(
        window.m_fullRepairPlanBox, QPoint(0, window.m_fullRepairStageList->height())).y();
    QVERIFY2(window.m_fullRepairPlanBox->height() - lastStageBottom >= 6
                 && window.m_fullRepairPlanBox->height() - lastStageBottom <= 10,
             qPrintable(QStringLiteral("the plan frame must keep the ~6 px bottom padding "
                                        "after its last stage row (gap %1 px)")
                            .arg(window.m_fullRepairPlanBox->height() - lastStageBottom)));
}

// The Repair tab's Full Repair plan uses the same two-column dynamic wrapping
// as the Settings page: the stage rows live in a height-for-width
// (ResponsiveColumnsLayout) container that places them side by side at wide
// widths and reflows to a single column as the window narrows.
void MainWindowUiTest::fullRepairPlanUsesTwoColumnWrapping()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fullRepairStageList);
    QLayout *stageLayout = window.m_fullRepairStageList->layout();
    QVERIFY2(stageLayout && stageLayout->hasHeightForWidth(),
             "the plan stages must use a responsive (two-column) columns layout");
    QVERIFY2(stageLayout->count() >= 2, "the plan must list several stages");

    // At a wide width at least two stage rows share a row (side-by-side columns).
    auto positions = [stageLayout] {
        QList<QPoint> points;
        for (int i = 0; i < stageLayout->count(); ++i) {
            if (QWidget *row = stageLayout->itemAt(i)->widget()) {
                points.append(row->pos());
            }
        }
        return points;
    };
    bool sideBySide = false;
    {
        const QList<QPoint> points = positions();
        for (int i = 0; i < points.size() && !sideBySide; ++i) {
            for (int j = i + 1; j < points.size(); ++j) {
                if (qAbs(points.at(i).y() - points.at(j).y()) <= 1
                    && qAbs(points.at(i).x() - points.at(j).x()) > 40) {
                    sideBySide = true;
                    break;
                }
            }
        }
    }
    QVERIFY2(sideBySide, "at a wide width the plan stages must wrap into side-by-side columns");

    // At a narrow width the stages reflow to a single column (no two rows share
    // a y-coordinate).
    window.resize(520, 760);
    QApplication::processEvents();
    QTest::qWait(50);
    bool singleColumn = true;
    {
        const QList<QPoint> points = positions();
        for (int i = 0; i < points.size() && singleColumn; ++i) {
            for (int j = i + 1; j < points.size(); ++j) {
                if (qAbs(points.at(i).y() - points.at(j).y()) <= 1) {
                    singleColumn = false;
                    break;
                }
            }
        }
    }
    QVERIFY2(singleColumn, "at a narrow width the plan stages must reflow to a single column");
}

// The Settings page's mandatory safety controls use the same two-column dynamic
// wrapping as the Full Repair plan: side-by-side at wide widths and reflowing
// to a single column as the window narrows.
void MainWindowUiTest::safetyControlsUseTwoColumnWrapping()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_settingsPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    QApplication::processEvents();
    QTest::qWait(50);

    QGroupBox *safetyBox = nullptr;
    const auto groups = window.findChildren<QGroupBox *>();
    for (QGroupBox *group : groups) {
        if (group->title() == QStringLiteral("Mandatory safety controls")) {
            safetyBox = group;
            break;
        }
    }
    QVERIFY2(safetyBox, "the mandatory safety controls group box must exist");
    const auto checks = safetyBox->findChildren<QCheckBox *>();
    QCOMPARE(checks.size(), 5);
    for (QCheckBox *check : checks) {
        QVERIFY(check->isChecked());
        QVERIFY(!check->isEnabled());
    }

    bool sideBySide = false;
    for (int i = 0; i < checks.size() && !sideBySide; ++i) {
        for (int j = i + 1; j < checks.size(); ++j) {
            if (qAbs(checks.at(i)->pos().y() - checks.at(j)->pos().y()) <= 1
                && qAbs(checks.at(i)->pos().x() - checks.at(j)->pos().x()) > 40) {
                sideBySide = true;
                break;
            }
        }
    }
    QVERIFY2(sideBySide, "at a wide width the safety controls must wrap into side-by-side columns");
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

// The Full Repair plan's stage list must scroll vertically inside its own
// section when the stages reflow into a single column and overflow the plan
// pane's fixed height. The plan stages live in a QScrollArea
// (m_fullRepairStageScroll) around the ResponsiveColumnsLayout; when the window
// narrows the stages drop from side-by-side columns into one column, the last
// rows extend below the pane, and the scroll area must gain a positive vertical
// scroll range instead of leaving those rows unreachable until the user drags
// the vertical splitter. The plan section's outer height must not grow to fit
// the wrapped content.
void MainWindowUiTest::fullRepairPlanScrollsVerticallyWhenWrapped()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);
    // Every configurable stage is available (filesystem and extlinux included)
    // and selected, so the plan lists a full set of rows that used to overflow
    // the pane once it wrapped to a single column; the Settings-parity model
    // now grows the plan pane to the full wrapped content and lets the repair
    // page's OUTER scroll bar provide the room instead.
    QString evidence = capabilityEvidenceWithDisplay(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    evidence += QStringLiteral("Repair tool extlinux: available\n");
    cacheRepairEvidence(window, evidence);
    for (const auto &stage : window.fullRepairStageCheckboxes()) {
        window.m_fullRepairStagePreferences.insert(stage.second, true);
    }
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fullRepairStageScroll);
    QVERIFY(window.m_fullRepairStageList);
    QLayout *stageLayout = window.m_fullRepairStageList->layout();
    QVERIFY2(stageLayout && stageLayout->hasHeightForWidth(),
             "the plan stages must use a responsive (two-column) columns layout");
    const int widePaneHeight = window.m_fullRepairStageScroll->height();
    QVERIFY2(widePaneHeight > 0, "the plan scroll area must have a measurable height");

    // Narrow the window so the stages reflow to a single column (no two rows
    // share a y-coordinate).
    window.resize(520, 760);
    QApplication::processEvents();
    QTest::qWait(80);
    QApplication::processEvents();

    bool singleColumn = true;
    for (int i = 0; i < stageLayout->count() && singleColumn; ++i) {
        QWidget *a = stageLayout->itemAt(i)->widget();
        if (!a) {
            continue;
        }
        for (int j = i + 1; j < stageLayout->count(); ++j) {
            QWidget *b = stageLayout->itemAt(j)->widget();
            if (b && qAbs(a->y() - b->y()) <= 1) {
                singleColumn = false;
                break;
            }
        }
    }
    QVERIFY2(singleColumn, "the plan must reflow to a single column at the narrow width");

    // The wrapped content now determines the plan section's own height
    // (Settings-parity full-content model, no visible-row clamp): the pane
    // hugs the FULL single-column content, so every stage row is visible
    // without an inner scrollbar and the frame's bottom line tracks the
    // content exactly.
    const int contentHeight = window.m_fullRepairStageList->height();
    const int viewportHeight = window.m_fullRepairStageScroll->viewport()->height();
    QVERIFY2(qAbs(contentHeight - viewportHeight) <= 4,
             qPrintable(QStringLiteral("the plan section must hug the full wrapped "
                                        "content (list %1 vs viewport %2)")
                            .arg(contentHeight).arg(viewportHeight)));
    QVERIFY2(window.m_fullRepairStageScroll->verticalScrollBar()->maximum() == 0,
             qPrintable(QStringLiteral("a full wrapped plan must show every row without an "
                                        "inner scrollbar (maximum %1)")
                            .arg(window.m_fullRepairStageScroll->verticalScrollBar()->maximum())));
    // The chrome (group-box frame, header, readiness hint, spacing, bottom
    // padding) is measured from the real rendered geometry — the scroll
    // area's top offset inside the box plus the space below the scroll —
    // exactly like updateFullRepairPlanHeight does (the sizeHint difference
    // is inflated by the word-wrapped labels' minimum-width hints).
    const int chromeHeight = window.m_fullRepairStageScroll->mapTo(
                                 window.m_fullRepairPlanBox, QPoint(0, 0)).y()
                           + window.m_fullRepairPlanBox->layout()->contentsMargins().bottom()
                           + window.m_fullRepairPlanBox->contentsMargins().bottom();
    QVERIFY2(qAbs(window.m_fullRepairPlanBox->height() - (chromeHeight + contentHeight)) <= 4,
             qPrintable(QStringLiteral("the plan frame must hug the full wrapped content "
                                        "(height %1 vs chrome+content %2)")
                            .arg(window.m_fullRepairPlanBox->height())
                            .arg(chromeHeight + contentHeight)));

    // The room comes from the repair page's outer vertical scroll area: its
    // widget grows taller than its viewport and the outer scrollbar gains
    // range, exactly the way the Settings page behaves on a short window.
    QScrollArea *pageScroll = window.findChild<QScrollArea *>(QStringLiteral("repairPageScroll"));
    QVERIFY2(pageScroll, "the repair page must keep its outer scroll area");
    QVERIFY2(pageScroll->widget()->height() > pageScroll->viewport()->height(),
             qPrintable(QStringLiteral("the page content (%1) must grow beyond the outer "
                                        "viewport (%2) so the outer scroll bar provides "
                                        "the room")
                            .arg(pageScroll->widget()->height())
                            .arg(pageScroll->viewport()->height())));
    QVERIFY2(pageScroll->verticalScrollBar()->maximum() > 0,
             qPrintable(QStringLiteral("the page's outer scroll bar must gain a vertical "
                                        "range for the grown plan (maximum %1)")
                            .arg(pageScroll->verticalScrollBar()->maximum())));
}

// Growing the plan frame by dragging the vertical splitter handle down must
// grow the scroll viewport WITH the frame (more rows become visible instead of
// dead space beside a scrollbar), while the stage rows stay EXACTLY as laid
// out: pegged to the upper-left with unchanged heights, and every pixel beyond
// the content absorbed below the rows by the trailing expansion placeholder.
void MainWindowUiTest::fullRepairPlanGrowsWithSplitterWithoutReflowingRows()
{
    // Earlier tests in the same process close windows whose default-size plan
    // splitter state is persisted and later restored, which pins the plan pane
    // at the restored position (user-adjusted semantics). Discard it so this
    // test always measures from the content-hugged default, the same cleanup
    // the frame-shrink test applies to the stale V2 key.
    {
        QSettings cleanup(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
        cleanup.remove(QStringLiteral("repair/verticalSplitterStateV3"));
        cleanup.sync();
    }

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);
    // A full plan (every configurable stage available and selected) so the
    // stage list has real content to measure.
    QString evidence = capabilityEvidenceWithDisplay(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    evidence += QStringLiteral("Repair tool extlinux: available\n");
    cacheRepairEvidence(window, evidence);
    for (const auto &stage : window.fullRepairStageCheckboxes()) {
        window.m_fullRepairStagePreferences.insert(stage.second, true);
    }
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_repairPage));
    window.resize(1180, 760);
    QTest::qWait(50);
    window.updateFullRepairSummary();
    QApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fullRepairStageScroll);
    QVERIFY(window.m_fullRepairStageList);
    QVERIFY(window.m_fullRepairPlanBox);
    QVERIFY(window.m_repairVerticalSplitter);
    QLayout *stageLayout = window.m_fullRepairStageList->layout();
    QVERIFY(stageLayout);

    struct Row { int x = 0; int y = 0; int w = 0; int h = 0; };
    const auto collectRows = [stageLayout] {
        QList<Row> rows;
        for (int i = 0; i < stageLayout->count(); ++i) {
            if (QWidget *w = stageLayout->itemAt(i)->widget()) {
                rows.append({w->x(), w->y(), w->width(), w->height()});
            }
        }
        return rows;
    };

    const QList<Row> before = collectRows();
    QVERIFY(!before.isEmpty());
    const int viewportBefore = window.m_fullRepairStageScroll->viewport()->height();
    const int frameBefore = window.m_fullRepairPlanBox->height();
    QVERIFY2(viewportBefore > 0 && frameBefore > 0, "the plan frame must be laid out");

    // Drag the handle down: grow the plan pane well beyond its content.
    window.m_repairVerticalSplitter->setSizes({frameBefore + 120, 100000});
    window.m_repairPlanSplitterUserAdjusted = true;
    QApplication::processEvents();
    QTest::qWait(50);
    QApplication::processEvents();

    const int viewportAfter = window.m_fullRepairStageScroll->viewport()->height();
    const int frameAfter = window.m_fullRepairPlanBox->height();
    QVERIFY2(frameAfter > frameBefore,
             qPrintable(QStringLiteral("the plan frame must grow when the handle is dragged "
                                       "down (before %1 after %2)")
                            .arg(frameBefore).arg(frameAfter)));
    QVERIFY2(viewportAfter > viewportBefore,
             qPrintable(QStringLiteral("the scroll viewport must expand with the grown frame "
                                       "(before %1 after %2)")
                            .arg(viewportBefore).arg(viewportAfter)));

    // The rows stay byte-identical: same positions (pegged upper-left), same
    // heights — no reflow, no stretching, no redistributed spacing.
    const QList<Row> after = collectRows();
    QCOMPARE(after.size(), before.size());
    for (int i = 0; i < before.size(); ++i) {
        QCOMPARE(after.at(i).x, before.at(i).x);
        QCOMPARE(after.at(i).y, before.at(i).y);
        QCOMPARE(after.at(i).w, before.at(i).w);
        QCOMPARE(after.at(i).h, before.at(i).h);
    }

    // The growth sits BELOW the rows (the trailing expansion placeholder): the
    // rows stay byte-identical (asserted above) and the scroll viewport fills
    // the grown frame exactly — no dead space beside a scrollbar.
    QVERIFY2(qAbs((viewportAfter - viewportBefore) - (frameAfter - frameBefore)) <= 1,
             qPrintable(QStringLiteral("the scroll viewport must fill the grown frame "
                                       "(viewport +%1 vs frame +%2)")
                            .arg(viewportAfter - viewportBefore)
                            .arg(frameAfter - frameBefore)));

    // Settings-parity drag cap: the requested +120 px growth is clamped to
    // exactly two real plan rows beyond the full content-hugged height, so the
    // handle physically stops there instead of overexpanding toward the lower
    // pane's floor.
    const int rowHeight = before.first().h;
    QVERIFY2(qAbs((frameAfter - frameBefore) - 2 * rowHeight) <= 1,
             qPrintable(QStringLiteral("the plan frame must grow by exactly the 2-row cap "
                                       "(grew %1 px vs 2 rows = %2 px)")
                            .arg(frameAfter - frameBefore).arg(2 * rowHeight)));
    QVERIFY2(frameAfter - frameBefore < 120,
             "the drag must not overexpand beyond the 2-row growth cap");
}
