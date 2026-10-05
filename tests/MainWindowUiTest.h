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
    void clearRegisterClearsDiagnosticCachesWithConfirmation();
    void repairToolStatusLineNamesMissingDiagnosticByTitle();
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
    void capabilityRepairEvidenceParsesFromCache();
    void capabilityHostEvidenceParsesFromCache();
    void capabilityTablePopulatesFromCacheWithoutProbe();
    void capabilityScopeSelectorSwitchesCaches();
    void capabilityGroupBoxTitleIsScopeNeutral();
    void capabilityRefreshAfterDiagnosticsAndClear();
    void repairDetailPaneShowsMissingToolGuidance();
    void capabilityUnavailableRowsRenderAsMissing();
    void fallbackAdoptionRefusesProtectedAndCrossDisk();
    void disabledSettingsDoNotLeakIntoFullRepairAndPreservePreferences();
    void unavailablePlanStagesAreDisabledAndUnchecked();
    void settingsStageCheckboxesFirstRunStayDisabledUntilEvidence();
    void settingsStageCheckboxesPersistAcrossDiagnosticWarmth();
    void settingsStageSelectionSurvivesRelaunchWithoutCleanClose();
    void fullRepairProgressDialogShowsAtPlanStartBeforePreStageCompletes();
    void repairChangeStatusInvalidatesMappedSectionsOrFailsSafe();
    void hostDefaultFedoraUnchangedParsesAsVerified();
    void unlockRoutesThroughSharedSessionAndQueuesBehindBusyGate();
    void targetShellDispatchStillUsesShell();
    void shellRunCommandDisabledDuringDiagnosticsRefresh();
    void chrootShellHistoryNavigatesUpAndDown();
    void diagnosticListItemTooltipsAreShort();
    void kernelRemovalPreviewListsTuxedoPlanAndKeepsNewest();
    void kernelRemovalPreviewDetectsMissingHeadersAndGroupsCommonHeaders();
    void kernelRemovalPreviewSurfacesRepoLatestAndOrphanedHeaders();
    void kernelRepairActionsDeriveFromEvidenceAndProtectRunning();
    void kernelRepairSessionSerializesCachedBaseline();
    void gpuRepairSessionSerializesGpuAndKernelBaselines();
    void bootstackSerializesComponentBaselines();
    void repairToolRefusesRunWhenRequiredDiagnosticMissing();
    void requiredDiagnosticSectionsMapping();
    void hostMaintenanceSwitchAfterFullRepairDoesNotCrash();
    void hostMaintenanceSwitchAfterResizeDoesNotCrash();
    void hostMaintenanceSwitchAfterResizeRealScanDoesNotCrash();
    void privilegedSessionCoalescingWaitDoesNotNestEventLoop();
    void bootEntriesParserParsesProtocol();
    void bootEntriesParserKeepsRepairClass();
    void bootEntriesPageGatedOutsideHostMaintenance();
    void bootEntriesReadyInTargetScope();
    void bootEntriesParserWiredToFixtureAndMoveReorder();
    void bootEntriesSaveOrderAssemblesCsv();
    void bootEntriesBlankLabelRenameRevertsCell();
    void bootEntriesEfiListUsesTargetIdentity();
    void bootEntriesHostClassRefusedClientSideInTargetScope();
    void bootEntriesBootsColumnRendersRoleLabel();
    void bootEntriesRestoreButtonGatedAndIssuesEfiRepair();
    void bootEntriesRestoreRefusesWhenEfiUnavailable();
    void bootEntriesAutoReloadOnTabShow();
    void bootEntriesRefreshScheduledAfterDiagnosticsComplete();
    void bootEntriesDeleteStagesPendingWithoutHelperWrite();
    void bootEntriesRenameStagesPendingWithoutHelperWrite();
    void bootEntriesSaveIssuesReorderDeleteRenameAndClearsPending();
    void bootEntriesSaveButtonLabelIsSave();
    void grubMenuParserParsesProtocol();
    void grubMenuParserTracksSubmenuNesting();
    void grubMenuParserParsesEspAndDriveHeaders();
    void grubMenuViewSwitchIssuesGrubListAndReadOnly();
    void grubMenuMissingConfigYieldsHeaderOnly();
    void grubMenuHeadingUpdatesOnViewSwitch();
    void grubMenuCrossReferenceListsMatchingFirmwareEntries();
    void grubMenuSubmenuConfigfileSurfacedInDetails();
    void extlinuxParserParsesProtocol();
    void bootMenuFallbackToExtlinuxWhenGrubAbsent();
    void grubLegacyParserParsesProtocol();
    void bootMenuFallbackToGrubLegacyWhenGrubAndExtlinuxAbsent();
    void bootEntriesHeaderComboRightOfScopeAndButtonRowWraps();
};

#endif // BOOT_REPAIR_UI_TEST_H
