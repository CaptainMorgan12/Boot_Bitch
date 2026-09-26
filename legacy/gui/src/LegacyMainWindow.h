// LegacyMainWindow - Qt3/Q3-widget frontend for the ported legacy helper.
//
// Layout mirrors the modern Boot Bitch Qt6 information hierarchy where Qt3
// allows it: a global header (icon, "Boot Bitch", the recovery subtitle and
// the guarded-repair badge), Systems (page heading, "Available repair targets"
// list, Select Target / Unlock / Host Maintenance / Authorize action row,
// Unlock status, Selected drive details), Diagnostics (Run All + scope label,
// diagnostic checks list, Selected diagnostic pane with Run Diagnostic,
// Results and Copy/Save Results, the target-only `Edit Target File...` control
// with the Etch-era configuration key list probed through the helper's
// read-only `Legacy config` lines), Repair (page heading + scope label,
// privilege-elevation state with Authorize, only the legacy-supported
// commands gated by the cached capability lines, including the guarded
// GRUB-legacy regeneration, plus the greyed modern-only feature list driven by
// the helper's `Legacy feature` probes), Chroot Shell / Host Shell and File
// Copy (greyed with their probe reasons), Logs (streamed helper output with a
// modern 1:1 kind filter, a search box and the per-session log list) and
// Settings (read-only configuration plus greyed modern-only options; the Help
// menu carries the About dialog instead of a tab).
//
// The window never reads block devices; all device/target confirmation and
// every privileged action goes through the helper (QProcess). Device and
// target selection comes from the read-only inventory row (inspection), and
// the best Linux root component is auto-resolved from the inventory's udev
// metadata (mapper with a Linux filesystem first, then a Linux partition,
// then the disk itself), exactly like the modern Systems page. Gating mirrors
// MainWindow::repairToolAvailable: fail closed, missing/unparseable evidence
// keeps the action disabled, and diagnostics/repairs additionally require an
// explicitly committed repair target or active Host Maintenance. The scope
// follows that state only (there is no independent scope selector). A plain
// interactive `sudo` is authorized once per scope through a modal hidden-input
// dialog when Host Maintenance is entered or a repair target is committed
// (never on Run All); the cached session is reused afterwards and an explicit
// Authorize control re-establishes it when the cached timestamp expired or was
// refused. The LUKS passphrase travels only over the helper's standard input.
// Neither secret is ever logged or placed in command arguments.
//
// Qt3 note: the Q3* class names of Qt4's Qt3-support module do not exist in
// Qt3 itself; the native Qt3 classes are QMainWindow/QListView/QTextEdit/
// QTabWidget, which is what this frontend uses.

#ifndef LEGACY_MAIN_WINDOW_H
#define LEGACY_MAIN_WINDOW_H

#include <qmainwindow.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

#include <cstddef>
#include <vector>

#include "DeviceInventory.h"
#include "EvidenceParser.h"

class QCheckBox;
class QCloseEvent;
class QComboBox;
class QDialog;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPopupMenu;
class QPushButton;
class QResizeEvent;
class QSplitter;
class QTabWidget;
class QTable;
class QTextEdit;
class QSignalMapper;

namespace legacy {

class HelperRunner;

// Cycle 9 loop 2: --smoke-test runs with settings isolation so a user's
// persisted ~/.qt/*rc overrides can never flip a smoke assertion, and the
// smoke never reads or writes those files.
void legacySetSmokeSettingsIsolation(bool enabled);
bool legacySmokeSettingsIsolation();

// One application-log register entry, tagged at capture time so the Logs
// filter combo (modern 1:1) can select entries by kind: lines captured while
// a diagnostic command runs are Diagnostic (with the current section key from
// the stream's `Diagnostic: <key>` markers), lines captured while a repair
// action runs are Repair (with the helper stage or the space-separated Full
// Repair stage set), everything else is Application.
struct LogEntry {
    enum Kind { Application = 0, Diagnostic = 1, Repair = 2 };
    LogEntry() : kind(Application) {}
    QString text;
    int kind;
    QString diagnostic; // diagnostic key for Diagnostic entries
    QString stages;     // space-separated helper stages for Repair entries
};

class LegacyMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    LegacyMainWindow(QWidget *parent = 0, const char *name = 0);

    void setHelperPath(const QString &path);
    void setElevationOverride(const QString &override);
    void setNoElevate(bool noElevate);
    void setLogDirectory(const QString &path);
    void setTarget(const QString &disk, const QString &root);

    // Smoke test: auto-detects the running host target when none was set,
    // runs `host-diagnose all` followed by `host-validate` through the helper,
    // then verifies the new controls (details/unlock/session/search/gating)
    // and the layout at 1024x768 (no clipped section titles, group titles,
    // buttons, list columns or combos). Used for the Etch VM validation (no
    // screenshots).
    void startSmokeTest();

signals:
    void smokeFinished(bool ok, const QString &summary);

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void scanDevices();
    void deviceSelectionChanged();
    void setRepairTarget();
    void toggleHostMaintenance();
    void authorizeNow();
    void runDiagnostics();
    void runSelectedDiagnostic();
    void diagnosticSelectionChanged();
    void editTargetConfigFile();
    void copyResults();
    void saveResults();
    void toolSelectionChanged();
    void runSelectedTool();
    void configurePlan();
    void runFullRepair();
    void planCheckboxChanged();
    void runChrootShell();
    void clearChrootOutput();
    void runUnlock();
    void logFilterChanged();
    void logSearchChanged();
    void sessionLogSelectionChanged();
    void refreshSessionLogs();
    void toggleLogWrap(bool enabled);
    void refreshCapabilities();
    void helperLine(const QString &line);
    void helperFinished(bool ok, int exitCode);
    void saveLog();
    void clearLog();
    void showAbout();
    void showUsageHelp();
    void lockAdministratorSession();
    void showSystemsTab();
    void showDiagnosticsTab();
    void showLogsTab();
    void showSettingsTab();
    void autoSizeDeviceColumns();
    void toggleLogWrapFromMenu();
    void startNewSessionLog();
    void addSessionNote();
    void deleteSelectedSessionLog();
    void deviceFilterChanged();
    void autoRefreshToggled(bool enabled);
    void makeDefault();
    void showHostDetails();
    void fileCopyDirectionChanged();
    void fileCopyAddFiles();
    void fileCopyAddFolder();
    void fileCopyRemoveSelected();
    void fileCopyClearStaging();
    void fileCopyPreview();
    void fileCopyRun();
    void fileCopyBrowse();
    void showHelpPopup(const QString &href);
    void toolListHeaderClicked(int column);
    int toolIndexForTitle(const QString &title) const;
    void runScheduledAutoRefresh();
    void runSmokeStep();

private:
    void buildMenus();
    QWidget *buildTargetsTab();
    QWidget *buildDiagnosticsTab();
    QWidget *buildActionsTab();
    QWidget *buildChrootShellTab();
    QWidget *buildFileCopyTab();
    QWidget *buildLogTab();
    QWidget *buildSettingsTab();

    QPushButton *makeButton(const QString &text, QWidget *parent);
    // Re-applies the font-metric minimum width after a runtime button-text
    // change (for example "Host Maintenance" -> "Exit Host Maintenance"), so
    // the layout guard can never clip the new label.
    void updateButtonText(QPushButton *button, const QString &text);
    QLabel *makeSectionTitle(const QString &text, QWidget *parent);
    void addListViewColumn(QListView *list, const QString &title, int width);
    void registerGroupBox(QGroupBox *box);
    QWidget *makeHelpButton(QWidget *parent, const QString &title,
                            const QString &text);
    QMap<QString, QString> m_helpTitles;
    QMap<QString, QString> m_helpTexts;
    int m_toolSortColumn;
    bool m_toolSortAscending;
    QSignalMapper *m_helpSignalMapper;
    void appendLog(const QString &line);
    void appendToLogFile(const QString &line);
    void ensureLogFile();
    // Ensures the session-log directory: the GUI's own default tree is
    // created 0700 with the leaf re-tightened, a user-supplied --log-dir is
    // created but never chmod'ed.
    void ensureLogDirectory();
    QString identity() const;
    bool hostScope() const;
    QString selectedDisk() const;
    QString selectedComponent() const;
    QString selectedRoot() const;
    QString autoResolvedRoot(const QString &disk) const;
    QString autoResolvedLuks(const QString &disk) const;
    QString unlockCandidateFor(const QString &disk) const;
    QString owningDiskFor(const DeviceRow &row) const;
    bool selectionComplete() const;
    bool targetCommitted() const;
    bool hostMaintenanceActive() const;
    bool diagnosticsScopeReady() const;
    QString scopeReadyReason() const;
    QString scopeFeatureKey() const;
    QString runningHostDisk() const;
    QString runningHostRoot() const;
    void setSelection(const QString &disk, const QString &component,
                      const QString &rootOverride);
    void selectInventoryRow(const QString &disk);
    void rebuildDeviceList();
    // Every visible device-tree item (top-level disks then their children),
    // in traversal order; used by the smoke checks and the auto-size pass.
    std::vector<QListViewItem *> deviceTreeItems() const;
    bool deviceRowVisible(const DeviceRow &row) const;
    bool rowBelongsToDisk(const DeviceRow &row, const QString &diskPath) const;
    bool diskHasEncryptedRow(const QString &diskPath) const;
    bool diskHasLinuxCandidate(const QString &diskPath) const;
    // Filesystem label for a whole-disk row: the primary Linux filesystem of
    // its children plus " + LUKS" when an encrypted child exists (never empty;
    // "unknown" when nothing is known).
    QString diskFilesystemSummary(const DeviceRow &row) const;
    void loadLegacySettings();
    void saveLegacySettings();
    void updateStatus();
    void updateActionStates();
    // B7-4: the structured repair summary block (──────── REPAIR ────────
    // bracketed, Repair-tagged) appended after every repair command, built
    // from the parsed `Repair change status` lines.
    void appendRepairSummaryBlock(const ParsedTranscript &parsed, bool commandOk);
    // Shows/hides the reserved header busy indicator ("Working...") from
    // m_running; the fixed slot width keeps the header layout stable.
    void updateBusyIndicator();
    void updateScopeLabel();
    void updateElevationLabel();
    void updatePlanView();
    void updateToolDetails();
    int selectedToolIndex() const;
    bool toolRunReady(int toolIndex, QString *reason) const;
    bool planStageSelected(int planIndex) const;
    bool planStageAvailable(int planIndex, QString *reason) const;
    QStringList selectedPlanStages() const;
    QStringList selectedPlanTitles() const;
    bool planRunReady(QString *reason) const;
    void showRepairResultDialog(const QString &title);
    QWidget *layoutBoundFor(QWidget *widget, QWidget *pageWidget) const;
    void updateConfigView();
    void openConfigEditor(const QString &content, const QString &key,
                          const QString &path);
    void updateLegacyFeatureView();
    void updateFeatureTab(QLabel *label, const char *feature);
    void updateFileCopyTab();
    QString fileCopyDirection() const;
    bool fileCopyReady(QString *reason) const;
    std::vector<std::pair<QString, QString> > driveDetailsRows(
        const QString &disk) const;
    void updateHostCard();
    void applyLegacySettingsDefaults();
    bool writeUnlockKeyfile(const QByteArray &secret, QString *path);
    // B5: shared mode-600 O_EXCL secret-file writer in the private 0700
    // `.keys` subdirectory of the GUI's log directory; the created path is
    // registered in `registeredPath` (a static termination-handler buffer)
    // before the first byte is written, so a SIGTERM/SIGINT/SIGHUP always
    // unlinks it. Dialog-free: callers report their own failure wording.
    bool writeSecretFile(const QByteArray &data, const char *prefix,
                         char *registeredPath, std::size_t registeredPathSize,
                         QString *path);
    // The config-write content file (--content-file transport): the helper
    // never deletes it; the GUI unlinks it on every completion path.
    bool writeConfigContentFile(const QByteArray &content, QString *path);
    void discardConfigContentFile();
    void discardUnlockKeyfile();
    void injectUnlockedMapperRows(const QString &disk, const QString &mapper,
                                  const QString &root, const QString &fstype,
                                  const QString &uuid = QString::null);
    bool startFileCopyCommand(bool realCopy);
    void handleFileCopyBrowseResult(const std::string &transcript, bool ok);
    void updateChrootShellState();
    void updateChrootShellMode();
    void legacyFeatureDisplay(const char *feature, QString *state,
                              QString *reason) const;
    void updateFactView(const ParsedTranscript &parsed);
    void updateDriveDetails();
    void updateUnlockStatus();
    void updateDiagnosticDetails();
    void updateAuthorizationAffordance();
    void mergeHelperDevices(const ParsedTranscript &parsed);
    void refreshSessionLogList();
    void refreshLogView();
    bool logLinePassesFilter(const LogEntry &entry) const;
    QStringList sessionLogFiles() const;
    QString visibleMapperForDisk(const QString &disk) const;
    void autoDetectHostTarget();
    bool ensureAdministratorSession(const QString &context);
    bool administratorSessionActive() const;
    bool sessionStillCurrent();
    bool startCommand(const QStringList &args, bool diagnostic,
                      const QString &label, bool unlock = false,
                      bool config = false, bool shell = false,
                      bool quiet = false,
                      const QString &logSection = QString::null,
                      const QString &logStages = QString::null,
                      bool browse = false);
    void runDiagnosticsInternal(bool quiet);
    void maybeAutoRefreshDiagnostics(const QString &reason);
    void reportChangeStatuses(const ParsedTranscript &parsed);
    void handleUnlockFinished(bool ok, const std::string &transcript,
                              const QString &device, const QString &disk);
    bool verifySmokeControls(QString *problems);
    bool verifyLayout(QString *problems, int *checked);

    QTabWidget *m_tabs;
    QPopupMenu *m_fileMenu;
    QPopupMenu *m_viewMenu;
    QPopupMenu *m_helpMenu;
    // Qt3's insertItem() auto-generates NEGATIVE menu ids, so the id's sign
    // cannot be used as a validity signal; this flag says the Wrap Log Lines
    // item exists and m_wrapLogsMenuId may be used with isItemChecked()/
    // setItemChecked().
    bool m_wrapLogsMenuValid;
    int m_wrapLogsMenuId;
    QComboBox *m_logFilterCombo;
    QComboBox *m_configCombo;
    QListView *m_deviceList;
    QListView *m_detailList;
    QListView *m_diagnosticList;
    QListView *m_planStageList;
    QListView *m_toolList;
    QListView *m_sessionLogList;
    QTextEdit *m_rawView;
    QTextEdit *m_logView;
    QTextEdit *m_unlockStatusView;
    QLineEdit *m_logSearchEdit;
    QLineEdit *m_shellCommandEdit;
    QTextEdit *m_shellOutput;
    QCheckBox *m_logWrapCheck;
    QCheckBox *m_showNonLinuxCheck;
    QCheckBox *m_showRemovableCheck;
    QCheckBox *m_showEncryptedCheck;
    QCheckBox *m_autoRefreshCheck;
    QLabel *m_priorLogBanner;
    QLabel *m_headerTitle;
    QLabel *m_headerSubtitle;
    QLabel *m_headerBadge;
    QLabel *m_busyLabel;
    QLabel *m_systemsHeading;
    QLabel *m_targetsHeading;
    QLabel *m_repairHeading;
    QLabel *m_repairScopeLabel;
    QLabel *m_diagHeading;
    QLabel *m_scopeLabel;
    QLabel *m_diagTitle;
    QLabel *m_diagDescription;
    QLabel *m_diagAvailability;
    QLabel *m_chrootHeading;
    QLabel *m_chrootScopeLabel;
    QLabel *m_fileCopyHeading;
    QLabel *m_fileCopyScopeLabel;
    QLabel *m_fileCopySourceTitle;
    QLabel *m_fileCopyDestinationTitle;
    QLabel *m_fileCopyOptionsTitle;
    QLabel *m_logsHeading;
    QLabel *m_settingsHeading;
    QLabel *m_gateHint;
    QLabel *m_planParagraph;
    QLabel *m_planCountLabel;
    QLabel *m_planReadinessLabel;
    QLabel *m_toolTitle;
    QLabel *m_toolDescription;
    QLabel *m_toolPlanStatus;
    QLabel *m_resultStatus;
    QLabel *m_targetSummary;
    QLabel *m_configLabel;
    QLabel *m_configReasonLabel;
    QLabel *m_chrootReasonLabel;
    QLabel *m_authStatusLabel;
    QLabel *m_settingsHelperLabel;
    QLabel *m_settingsElevationLabel;
    QLabel *m_settingsLogDirLabel;
    QLabel *m_settingsSessionLabel;
    QLabel *m_settingsVersionLabel;
    QLabel *m_capDistributionLabel;
    QLabel *m_capPackageManagerLabel;
    QLabel *m_capAuthLabel;
    QTable *m_capabilityTable;
    QPushButton *m_scanButton;
    QPushButton *m_diagnosticsButton;
    QPushButton *m_runDiagnosticButton;
    QPushButton *m_copyResultsButton;
    QPushButton *m_saveResultsButton;
    QPushButton *m_configButton;
    QPushButton *m_unlockButton;
    QPushButton *m_setTargetButton;
    QLabel *m_hostSystemLabel;
    QLabel *m_hostStorageLabel;
    QLabel *m_hostProtectedBadge;
    QPushButton *m_hostDetailsButton;
    QLabel *m_detailsPaneTitle;
    bool m_inspectingHostDetails;
    QGroupBox *m_hostCard;
    QPushButton *m_hostMaintenanceButton;
    QPushButton *m_authorizeButton;
    QPushButton *m_configurePlanButton;
    QPushButton *m_runFullRepairButton;
    QPushButton *m_toolRunButton;
    QPushButton *m_resultCloseButton;
    QPushButton *m_shellRunButton;
    QPushButton *m_shellClearButton;
    QPushButton *m_newSessionLogButton;
    QPushButton *m_addNoteButton;
    QPushButton *m_deleteSessionLogButton;
    QPushButton *m_refreshCapabilitiesButton;
    QPushButton *m_installSupportButton;
    QPushButton *m_hostDefaultButton;
    QPushButton *m_fileCopyAddFilesButton;
    QPushButton *m_fileCopyAddFolderButton;
    QPushButton *m_fileCopyRemoveButton;
    QPushButton *m_fileCopyClearButton;
    QPushButton *m_fileCopyPreviewButton;
    QPushButton *m_fileCopyRunButton;
    QComboBox *m_fileCopyDirectionCombo;
    QComboBox *m_fileCopyOwnershipCombo;
    QListView *m_fileCopySourceList;
    QLineEdit *m_fileCopyDestinationEdit;
    QPushButton *m_fileCopyBrowseButton;
    QSplitter *m_systemsSplitter;
    QSplitter *m_repairVerticalSplitter;
    QGroupBox *m_chrootGroup;
    QGroupBox *m_fileCopySourceGroup;
    QGroupBox *m_fileCopyDestinationGroup;
    QGroupBox *m_fileCopyOptionsGroup;
    QWidget *m_chrootTab;
    QWidget *m_settingsContent;
    QWidget *m_repairContent;
    QDialog *m_resultDialog;
    QTextEdit *m_resultView;
    QLabel *m_chrootCommandHeading;
    std::vector<QCheckBox *> m_planChecks;
    std::vector<QPushButton *> m_buttons;
    std::vector<QGroupBox *> m_groupBoxes;
    std::vector<QLabel *> m_sectionTitles;
    std::vector<DeviceRow> m_inventory;

    HelperRunner *m_runner;
    CapabilityModel m_model;
    QMap<QString, DeviceRow> m_rows;
    QMap<QString, QString> m_factMap;
    QMap<QString, QString> m_unlockStatusCache;
    QMap<QString, QString> m_diagKeyByTitle;

    QString m_transcript;
    QString m_logDirectory;
    QString m_logPath;
    QString m_lastHelperDescription;
    // A10-05: the GUI-owned cancel-token file for the running helper command
    // (~/.boot-repair-legacy/cancel-request); passed as --cancel-file and
    // touched by HelperRunner::cancel() before the direct kill.
    QString m_cancelFilePath;
    QString m_unlockDevice;
    QString m_unlockDisk;
    QString m_helperDisk;
    QString m_helperComponent;
    QString m_hostDetectedDisk;
    QString m_hostDetectedRoot;
    QString m_selectedDisk;
    QString m_selectedComponent;
    QString m_rootOverrideDisk;
    QString m_rootOverrideRoot;
    QString m_committedDisk;
    QString m_committedRoot;
    QString m_priorLogPath;
    std::vector<LogEntry> m_logEntries;
    QStringList m_priorLogLines;
    QStringList m_fileCopySources;
    QString m_fileCopyBrowsePath;
    QString m_lastDiagnosticKey;
    QString m_lastDiagnosticIdentity;
    // The GUI's mode-600 unlock keyfile (Qt 3.3.7 QProcess cannot deliver
    // stdin); created per unlock attempt and deleted on every path.
    QString m_unlockKeyfilePath;
    // B5: the GUI's mode-600 config-write content file (--content-file
    // transport, never argv); created per write attempt and deleted on every
    // path, including the termination handlers.
    QString m_configContentFilePath;
    // Cycle 11: the drive unlocked in this session and the helper-confirmed
    // root component (UNLOCKED_ROOT) for the Select Target fallback.
    QString m_unlockedDisk;
    QString m_unlockedRoot;
    QString m_unlockedMapper;
    QString m_unlockedRootUuid;
    // Last applied dynamic tooltips, so updateFileCopyTab() re-registers a
    // tooltip only when the text actually changes (Qt3's QTipManager deletes
    // and reallocates the per-widget Tip record on every add; the churn is
    // needless and is avoided by construction).
    QString m_fileCopyBrowseTip;
    QString m_fileCopyPreviewTip;
    QString m_fileCopyRunTip;
    // Live tagging context for the next appendLog: the kind and the section
    // key/stage set of the helper command currently streaming (Application
    // outside a command). helperLine() updates the diagnostic key when the
    // stream carries a `Diagnostic: <key>` marker.
    int m_logEntryKind;
    QString m_logEntryDiagnostic;
    QString m_logEntryStages;
    // The helper stages the running repair command carries, for the B7-4
    // repair summary block (empty for non-repair commands).
    QStringList m_activeRepairStages;

    bool m_updatingSelection;
    bool m_running;
    bool m_pendingDiagnostic;
    bool m_pendingUnlock;
    bool m_pendingConfig;
    bool m_pendingShell;
    bool m_pendingBrowse;
    bool m_unlockRetry;
    bool m_targetCommitted;
    bool m_hostMaintenance;
    bool m_viewingPriorLog;
    bool m_logWrapEnabled;
    bool m_autoRefreshEnabled;
    QString m_pendingIdentity;
    QString m_pendingLabel;
    QString m_pendingConfigKey;
    QString m_pendingConfigPath;
    bool m_pendingConfigWrite;

    bool m_smokeMode;
    int m_smokeStep;
    bool m_smokeDiagnoseOk;
    bool m_smokeValidateOk;
};

} // namespace legacy

#endif // LEGACY_MAIN_WINDOW_H
