// Read-only MainWindow regression suite: single QTest class whose 20 test
// slots (plus initTestCase) are implemented across several translation units
// (MainWindowUiTest.cpp plus the MainWindowUiTest_<feature>.cpp files).
// Keeping one class keeps the single ctest registration boot-repair-ui-read-only
// unchanged; only the runtime slot count shrinks.
#ifndef BOOT_REPAIR_UI_TEST_H
#define BOOT_REPAIR_UI_TEST_H

#include <QObject>

class MainWindowUiTest final : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void tabsAndReadOnlyControls();
    void chrootShellControlsAreGuardedAndReadOnly();
    void unlockStatusPanelIsReadOnlyAndScoped();
    void filesystemRepairToolRowIsGatedByCapabilityEvidence();
    void reasonKeyTokensMapToLocalizedStringsAndUnknownKeysPassThrough();
    void fullRepairFilesystemStageIsFirstWhenEnabled();
    void logSectionFilterShowsRelatedRepairsAndDiagnosticSection();
    void blankAndDataTargetsSelectableWhileOpticalAndLuksStayGated();
    void scopeSwitchingRejectsStaleTargetEvidence();
    void missingCapabilityEvidenceFailsClosed();
    void filesystemDiagnosticCachedByScopeAutoRegeneration();
    void capabilityGateReadsOnlyCapabilitiesEntry();
    void fallbackAdoptionRefusesProtectedAndCrossDisk();
    void disabledSettingsDoNotLeakIntoFullRepairAndPreservePreferences();
    void unavailablePlanStagesAreDisabledAndUnchecked();
    void settingsStageCheckboxesFirstRunStayDisabledUntilEvidence();
    void settingsStageCheckboxesPersistAcrossDiagnosticWarmth();
    void settingsStageSelectionSurvivesRelaunchWithoutCleanClose();
    void fullRepairProgressDialogShowsAtPlanStartBeforePreStageCompletes();
    void repairChangeStatusInvalidatesMappedSectionsOrFailsSafe();
    void hostDefaultFedoraUnchangedParsesAsVerified();
    void targetShellDispatchStillUsesShell();
    void shellRunCommandDisabledDuringDiagnosticsRefresh();
    void hostMaintenanceSwitchAfterFullRepairDoesNotCrash();
    void hostMaintenanceSwitchAfterResizeDoesNotCrash();
    void hostMaintenanceSwitchAfterResizeRealScanDoesNotCrash();
    void privilegedSessionCoalescingWaitDoesNotNestEventLoop();
};

#endif // BOOT_REPAIR_UI_TEST_H
