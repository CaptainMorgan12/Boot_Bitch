// Read-only MainWindow regression suite: single QTest class whose 64 test
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
    void fullRepairPlanRowsAreTightAndUniform();
    void fullRepairPlanFrameShrinksToContentAndGrowsOutputPane();
    void clearRegisterClearsDiagnosticCachesWithConfirmation();
    void repairToolStatusLineNamesMissingDiagnosticByTitle();
    void filesystemRepairToolRowIsGatedByCapabilityEvidence();
    void reasonKeyTokensMapToLocalizedStringsAndUnknownKeysPassThrough();
    void blankAndDataTargetsSelectableWhileOpticalAndLuksStayGated();
    void scopeSwitchingRejectsStaleTargetEvidence();
    void scopeSwitchAlwaysRegeneratesDiagnostics();
    void scopeTransitionAnnouncementTaggedWithNewScope();
    void missingCapabilityEvidenceFailsClosed();
    void rawCapabilityReasonKeyFailsClosedOnForeignTargetCache();
    void capabilityGateReadsOnlyCapabilitiesEntry();
    void capabilityRepairEvidenceParsesFromCache();
    void capabilityHostEvidenceParsesFromCache();
    void capabilityStorageStackRowsParseFromCache();
    void capabilitySnapshotRollbackRowsParseFromCache();
    void capabilityRowsCachedAcrossScopeSwitch();
    void capabilityRowsSurviveHostMaintenanceRoundTrip();
    void capabilityInstallRoutesHostVsTarget();
    void capabilityTablePopulatesFromCacheWithoutProbe();
    void capabilityScopeSelectorSwitchesCaches();
    void capabilityRefreshAfterDiagnosticsAndClear();
    void repairDetailPaneShowsMissingToolGuidance();
    void capabilityUnavailableRowsRenderAsMissing();
    void settingsSplitterSeparatesPlanFromCapabilitiesAndWidensCommandColumn();
    void capabilityTableKeepsFixedHeightWhenEmpty();
    void fallbackAdoptionRefusesProtectedAndCrossDisk();
    void unavailablePlanStagesAreDisabledAndUnchecked();
    void settingsStageCheckboxesFirstRunStayDisabledUntilEvidence();
    void settingsStageSelectionSurvivesRelaunchWithoutCleanClose();
    void repairChangeStatusInvalidatesMappedSectionsOrFailsSafe();
    void fullRepairSummaryNamesDetectedModuleBuildSystem();
    void fullRepairPlanLabelsNameDetectedModuleBuildSystem();
    void displayedLogNamesDetectedModuleBuildSystem();
    void fullRepairPlanShrinksForFewStagesAndGrowsDetailsPane();
    void fullRepairPlanFrameHasNoTrailingGapForSevenStages();
    void fullRepairPlanUsesTwoColumnWrapping();
    void safetyControlsUseTwoColumnWrapping();
    void unlockRoutesThroughSharedSessionAndQueuesBehindBusyGate();
    void targetShellDispatchStillUsesShell();
    void shellRunCommandDisabledDuringDiagnosticsRefresh();
    void diagnosticListItemTooltipsAreShort();
    void kernelRepairActionsDeriveFromEvidenceAndProtectRunning();
    void kernelRepairSessionSerializesCachedBaseline();
    void gpuRepairSessionSerializesGpuAndKernelBaselines();
    void bootstackSerializesComponentBaselines();
    void repairToolRefusesRunWhenRequiredDiagnosticMissing();
    void requiredDiagnosticSectionsMapping();
    void hostMaintenanceSwitchAfterFullRepairDoesNotCrash();
    void hostMaintenanceSwitchAfterResizeDoesNotCrash();
    void hostExitWithSelectedSnapshotRowDoesNotCrash();
    void privilegedSessionCoalescingWaitDoesNotNestEventLoop();
    void privilegedSessionCancellationPublishesFailureAndDefersWithoutRetry();
    void privilegedSessionGrantProducesSingleRequest();
    void privilegedSessionRestagesTamperedHelperCopy();
    void backgroundSnapshotPreloadDoesNotClaimBusyLabel();
    void closeDuringDiagnosticShowsCancelState();
    void bootEntriesParserParsesProtocol();
    void bootEntriesPageGatedOutsideHostMaintenance();
    void bootEntriesReadyInTargetScope();
    void bootEntriesHostClassRefusedClientSideInTargetScope();
    void bootEntriesRestoreButtonGatedAndIssuesEfiRepair();
    void bootEntriesRestoreRefusesWhenEfiUnavailable();
    void bootEntriesAutoReloadOnTabShow();
    void bootEntriesRefreshScheduledAfterDiagnosticsComplete();
    void bootEntriesFirstScopeVisitLoadsFirmwareAndMenu();
    void bootEntriesRenderFromCacheAcrossScopeRoundTrip();
    void bootEntriesRoundTripWithPopulatedEntriesDoesNotCrash();
    void bootEntriesMidRequestScopeSwitchDiscardsStaleResult();
    void bootEntriesDeleteStagesPendingWithoutHelperWrite();
    void bootEntriesRenameStagesPendingWithoutHelperWrite();
    void bootEntriesSaveIssuesReorderDeleteRenameAndClearsPending();
    void grubMenuParserParsesProtocol();
    void grubMenuViewSwitchIssuesGrubListAndReadOnly();
    void grubLegacyParserParsesProtocol();
    void snapshotsTabVisibilityFollowsSnapshotCapability();
    void bootEntriesTabVisibilityFollowsBootEvidence();
    void fullRepairPlanScrollsVerticallyWhenWrapped();
    void fullRepairPlanGrowsWithSplitterWithoutReflowingRows();
    void capabilityTableSortKeepsInstallWidgetsOnTheirRows();
};

#endif // BOOT_REPAIR_UI_TEST_H
