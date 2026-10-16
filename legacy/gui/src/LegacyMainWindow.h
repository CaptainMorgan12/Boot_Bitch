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
// refused. The LUKS passphrase travels only through a mode-600 keyfile
// (--key-file) written into the GUI's private .keys directory, never stdin or
// command arguments. Neither secret is ever logged or placed in command
// arguments.
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
#include <qvaluevector.h>

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
class QScrollView;
class PlanScrollView;
class CapabilityTable;
class QSplitter;
class QTabWidget;
class QTable;
class QTextEdit;
class QTimer;
class QSignalMapper;

namespace legacy {

class HelperRunner;
// The modal repair-result dialog (defined in LegacyMainWindow.cpp): its Close
// button and dismissal are gated until the helper request completes, matching
// the modern RepairProgressDialog (no silent mid-operation close).
class RepairResultDialog;

// One parsed `File system check <device>: ...` helper evidence line (modern
// MainWindow::FilesystemCheckResult parity). `result == "issues"` is the only
// positive evidence that the device needs repair; skipped/unsupported/tool-
// missing/clean devices never become repair candidates. Declared in the header
// (not the .cpp) so the single-popup repair-plan driver can hold a candidate
// list in member state across its asynchronous steps.
struct LegacyFsCheckResult {
    QString device;
    QString fstype;
    QString uuid;
    QString mount;
    QString tool;
    QString result;
    QString detail;
    LegacyFsCheckResult() {}
};

// Cycle 9 loop 2: --smoke-test runs with settings isolation so a user's
// persisted ~/.qt/*rc overrides can never flip a smoke assertion, and the
// smoke never reads or writes those files.
void legacySetSmokeSettingsIsolation(bool enabled);
bool legacySmokeSettingsIsolation();
// Cycle 16: the parsed --log-dir is published here BEFORE the window is
// constructed, so the constructor's own readiness lines follow it instead of
// landing in the default $HOME log tree (see the cpp comment).
void legacySetInitialLogDirectory(const QString &dir);
QString legacyInitialLogDirectory();

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

    // Item #3 (modern parity): session-only Up/Down command-history navigation
    // for the Shell tab command field. +1 steps Up (toward older entries), -1
    // steps Down (toward newer entries, then back to the saved draft). Called
    // by the ShellCommandLineEdit keyPressEvent subclass defined in the .cpp.
    void stepShellHistory(int direction);

signals:
    void smokeFinished(bool ok, const QString &summary);

protected:
    virtual void closeEvent(QCloseEvent *event);
    // Settings-splitter drag evidence (etch-agent-guide.md §6): fires on
    // the lower pane's Resize - every handle move resizes it (Qt 3.3.7's
    // QSplitter has no splitterMoved signal; the connect failed silently).
    virtual bool eventFilter(QObject *watched, QEvent *event);

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
    // Builds (without showing) the "Unlock LUKS repair target" dialog.
    // The passphrase QLineEdit is returned via editOut (may be null).
    QDialog *buildUnlockDialog(const QString &luks, QLineEdit **editOut);
    void logFilterChanged();
    void logSearchChanged();
    // Connected to QListView::selectionChanged(QListViewItem*) so the handler
    // reads the item the signal reports instead of QListView::currentItem(),
    // which Qt 3.3.x can leave stale (still pointing at the previous item)
    // while the selection-change signal is being delivered. That stale read
    // left the Delete button disabled for the first prior session selected
    // after launch (the most recent prior session), so it could not be deleted.
    void sessionLogSelectionChanged(QListViewItem *item);
    void refreshSessionLogs();
    void toggleLogWrap(bool enabled);
    void refreshCapabilities();
    // Item 2: installs the SELECTED capability row's suggested package through
    // the helper's guarded install verb — install-host-package for the Host
    // scope and install-target-package for the committed repair-target scope
    // (simulation-first, fail-closed; never a bare apt-get). Enabled only for a
    // missing, installable row (Action cell shows "Install"); the target scope
    // offers no install affordance while no target is committed.
    void installSelectedCapabilityPackage();
    // Item 2: QTable currentChanged; re-applies the Install button's enabled
    // state to the newly selected row.
    void capabilitySelectionChanged(int row, int col);
    void helperLine(const QString &line);
    void helperFinished(bool ok, int exitCode);
    // Emits the RAW answer for a waiting interactive repair menu: the input
    // line's text (verbatim, plus a single newline) is written to the
    // menu-answer file the helper polls; an empty input fails the prompt
    // closed.  Only fires while a PROMPT record is pending (the input line is
    // disabled otherwise).
    void sendPromptAnswer();
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
    void deviceFilterChanged(bool checked);
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
    // Re-entrant single-popup repair-plan step driver, scheduled from
    // helperFinished()/sendPromptAnswer() so a new command never starts inside
    // the finished signal's call stack. Starts the next plan command (or the
    // remaining Full Repair stages) inside the one repair result popup.
    void advanceRepairPlan();
    // Advances the busy-indicator ellipsis animation (0..3 trailing dots
    // behind the base working text) while a helper command runs.
    void advanceBusyAnimation();

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
    // Item #4: the highlighted (not yet committed) Systems tree row that the
    // Select Target, Unlock and details panes act on. While Host Maintenance
    // is active this is the highlighted preview row (kept apart from the
    // active-scope selection), otherwise it is the ordinary selection.
    QString previewDisk() const;
    QString previewComponent() const;
    QString autoResolvedRoot(const QString &disk) const;
    QString autoResolvedLuks(const QString &disk) const;
    QString resolvedUnlockedRoot(const QString &disk,
                                 const QString &reportedRoot) const;
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
    // B7-4: the structured repair summary block (-------- REPAIR --------
    // bracketed, Repair-tagged) appended after every repair command, built
    // from the parsed `Repair change status` lines.
    void appendRepairSummaryBlock(const ParsedTranscript &parsed, bool commandOk);
    // Shows/hides the global busy indicator (modern parity) from m_running:
    // a reserved row below the guarded-repair badge with the modern working
    // text, animated by a plain QTimer cycling 0..3 trailing dots. The fixed
    // row height keeps the header and the tab pages stable.
    void updateBusyIndicator();
    // The exact working text the modern Qt6 GUI's busy status label shows
    // for the equivalent in-flight operation (no helper command = empty).
    QString busyIndicatorText() const;
    void updateScopeLabel();
    // Modern updateTargetLabels() parity: the Settings "Show capabilities
    // for:" selector follows the active scope (Host Maintenance -> Host,
    // otherwise the committed repair target). It only picks which cached
    // scope the table reads and never creates a scope model of its own.
    void syncCapabilityScopeSelector();
    // Modern invalidateAllActiveScopeDiagnostics()/clearHostDiagnosticCache()
    // parity: the legacy frontend keeps ONE cached capability/evidence model
    // (identity-keyed, not two per-scope maps), so a scope switch clears the
    // model plus the diagnostic display cache so a target-scope run never
    // carries host-scope evidence (and vice-versa).
    void invalidateScopeDiagnostics();
    void updateElevationLabel();
    void updatePlanView();
    // Modern availability semantics for the Settings plan checkboxes: shown
    // rows only; available = checkable with the saved preference, unavailable
    // = disabled and unchecked with the exact reason (never hidden).
    void updatePlanChecks();
    // Re-applies the device-discovery filter checkboxes from their register
    // (m_showNonLinux/m_showRemovable/m_showEncrypted), mirroring how
    // updatePlanChecks() re-applies the plan register.
    void updateDeviceFilterChecks();
    void updateToolDetails();
    // Item 2: enables the Install Missing Support... button exactly when the
    // selected capability row is missing, installable and has a package.
    void updateInstallSupportButton();
    int selectedToolIndex() const;
    bool toolRunReady(int toolIndex, QString *reason) const;
    // Missing required diagnostic sections for a repair tool (empty when every
    // required section is cached in m_diagnosticSections). The fail-closed
    // repair gate uses this list, mirroring MainWindow::missingRequiredDiagnosticTitles.
    QStringList missingRequiredDiagnosticSections(const QString &toolKey) const;
    // Ported from MainWindow::kernelsRemovalPreview(): parses the cached
    // `kernel` diagnostic section into the concise superseded/orphaned/
    // incomplete/missing-headers kernel-inventory summary shown in the kernels
    // tool detail pane. Fail-closed: an empty/absent cached section returns an
    // empty string and the pane keeps its static description only.
    QString kernelsRemovalPreview() const;
    bool planStageSelected(int planIndex) const;
    bool planStageAvailable(int planIndex, QString *reason) const;
    // Presentation state for the Settings -> Full Repair checkboxes. The
    // checkboxes are plain user settings that stay checkable unless the stage
    // is EXPLICITLY unavailable (a cached `unavailable|<reason>` capability
    // line in the current scope, a frontend gap, or a host-only stage outside
    // Host Maintenance). Absent capability evidence keeps a stage checkable
    // with its saved preference, so the presentation never depends on
    // diagnostic-cache warmth. The fail-closed run gate
    // planStageAvailable() is planStageState() == PlanStageAvailable.
    enum PlanStageState {
        PlanStageUnavailable = 0,
        PlanStageAvailable = 1
    };
    PlanStageState planStageState(int planIndex, QString *reason) const;
    QStringList selectedPlanStages() const;
    QStringList selectedPlanTitles() const;
    bool planRunReady(QString *reason) const;
    // The file system repair flow (modern runFilesystemRepairFlow parity): runs
    // the read-only fs-inspect and the per-device repair inside the single
    // repair result popup, with the per-device decision surfaced inline through
    // the popup's prompt input line (no separate modal). Returns true when at
    // least one repair completed successfully.
    bool runFilesystemRepairFlow();
    // Begins the single-popup repair-plan driver (Full Repair plan or the
    // standalone file system flow): captures the scope, starts the first
    // command and leaves the rest to advanceRepairPlan(). runFilesystem is
    // false when a Full Repair plan has no file system stage selected.
    bool beginRepairPlan(bool fullRepair, const QStringList &remainingStages,
                         bool runFilesystem);
    // Starts the remaining Full Repair stages as one helper request; returns
    // false when there are none or the command could not start.
    bool startRemainingRepairStages();
    // Presents the numbered per-device repair decision through the popup's
    // prompt input line and waits for sendPromptAnswer().
    void poseRepairPlanInlineDecision();
    // Applies the typed inline answer (a numbered device list) and starts the
    // first chosen per-device repair.
    void processRepairPlanInlineAnswer(const QString &answer);
    // Starts the next chosen per-device fs-repair; returns false when none
    // remain (the caller then moves on to the remaining plan stages).
    bool startNextRepairPlanDevice();
    // Continues to the remaining Full Repair stages, or finalizes the plan.
    void continueOrFinishRepairPlan();
    // Finalizes the plan: enables the popup's Close, invalidates the cached
    // diagnostics (Full Repair) or schedules the stale-based refresh
    // (standalone), and clears the plan state.
    void finishRepairPlan();
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
    // Interactive repair menu-answer channel (Qt 3.3.7 QProcess cannot deliver
    // stdin): a mode-600 file the helper polls via
    // BOOT_REPAIR_LEGACY_MENU_ANSWER_FILE.  ensureMenuAnswerFile reserves the
    // empty path before the interactive helper starts; writeMenuAnswerRecord
    // writes the RAW answer plus a single newline (an empty answer = a lone
    // newline = cancel/fail-closed) on Send; discardMenuAnswerFile unlinks it
    // on every completion path.
    bool ensureMenuAnswerFile(QString *path);
    bool writeMenuAnswerRecord(const QString &answer);
    void discardMenuAnswerFile();
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
    void refreshDiagnosticResultsView();
    void updateAuthorizationAffordance();
    void mergeHelperDevices(const ParsedTranscript &parsed);
    void refreshSessionLogList();
    void refreshLogView();
    // Flushes the diagnostic stream buffered during a run into the register and
    // the session file, preserving each line's captured kind/section tag
    // (modern hide-until-complete parity: the buffered transcript is revealed
    // only once the run completes).
    void flushBufferedDiagnosticEntries();
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
                      bool browse = false,
                      bool interactive = false);
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
    // Item 1 (two-column plan): the Repair-tab Full Repair plan renders the
    // numbered stage titles as read-only QLabels in a single two-column
    // QGridLayout (ceil(planSpecCount/2) rows x 2 columns, matching the
    // Settings plan's two-column QGridLayout), left column first so the plan
    // order reads top-to-bottom down the left column then the right. The grid
    // lives inside a QScrollView so the stages scroll when the vertical
    // splitter is dragged up to hide them (the count/readiness/buttons stay
    // outside the scroll view). updatePlanView() fills the labels in a
    // BALANCED split (ceil(N/2) left / floor(N/2) right) and hides the unused
    // ones, keeping one "Full Repair plan" header with no per-column titles.
    QValueVector<QLabel *> m_planStageLabels;
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
    // Lightweight busy animation state: a plain QTimer (no threads) cycles
    // 0..3 trailing dots behind m_busyBaseText while a helper command runs.
    QTimer *m_busyTimer;
    int m_busyFrame;
    QString m_busyBaseText;
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
    // The Full Repair plan's scroll view. It is a PlanScrollView whose
    // minimumSizeHint() reports a small two-row floor (not AutoOneFit's
    // inflated minimum), so the vertical splitter can actually shrink the plan
    // frame instead of locking. See etch-lessons-learned.md §6.
    PlanScrollView *m_planScroll;
    // The Full Repair plan and Individual repair tools group frames, kept so
    // the smoke test can report their actual displayed heights (QWidget::height()
    // after the Repair page is laid out; see etch-agent-guide.md §6).
    QGroupBox *m_planGroup;
    QGroupBox *m_toolsGroup;
    // The plan stage grid's real content height (rows x rowHint + spacing +
    // padding), set by updatePlanView() so the smoke test can report the plan
    // frame's tight content height (see etch-agent-guide.md §6).
    int m_planGridContentHeight;
    QLabel *m_toolTitle;
    QLabel *m_toolDescription;
    QLabel *m_toolPlanStatus;
    QLabel *m_resultStatus;
    QLabel *m_targetSummary;
    QLabel *m_configLabel;
    QLabel *m_configReasonLabel;
    QLabel *m_chrootReasonLabel;
    QLabel *m_authStatusLabel;
    QLabel *m_settingsElevationLabel;
    QLabel *m_capDistributionLabel;
    QLabel *m_capPackageManagerLabel;
    QLabel *m_capAuthLabel;
    CapabilityTable *m_capabilityTable;
    // Settings -> Capabilities scope selector (Repair target / Host): picks
    // which cached scope's capability evidence the table shows. Item 5 parity.
    QComboBox *m_capabilityScopeSelector;
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
    // Settings tab section frames the drag controller's event filter walks
    // (the pane and splitter parents; see etch-agent-guide.md §6).
    QGroupBox *m_settingsPlanGroup;
    QGroupBox *m_settingsCapabilityGroup;
    QGroupBox *m_settingsSafetyGroup;
    // Live Settings-splitter drag control: Qt 3.3.7's own opaque drag path
    // decays the panes' sum by the handle size on every incremental mouse
    // move (logged in the user's real drag: 815 -> 773 across six moves,
    // with the mandatory box sliding 92px below the splitter's bottom edge
    // where it is clipped). The handle is intercepted by an event filter
    // and the drag is driven through setSizes() instead - the distribution
    // path that preserves the panes' sum and keeps every box inside.
    QWidget *m_settingsSplitterHandle;
    bool m_settingsDragActive;
    int m_settingsDragBaseY;
    int m_settingsDragBaseUpper;
    QWidget *m_repairContent;
    RepairResultDialog *m_resultDialog;
    QTextEdit *m_resultView;
    // Interactive repair prompt surface (modern parity): the persistent input
    // line + Send button in the Repair result dialog, enabled only while a
    // PROMPT record is waiting.  The answer travels to the helper through the
    // mode-600 menu-answer file (m_promptAnswerFile) the helper polls, because
    // Qt 3.3.7 QProcess cannot deliver stdin.
    QLineEdit *m_promptInput;
    QPushButton *m_promptSend;
    QLabel *m_promptHint;
    bool m_promptActive;
    // The BOOT_REPAIR_SESSION_REQUEST id of the running interactive repair,
    // and the GUI's mode-600 menu-answer file the helper polls via
    // BOOT_REPAIR_LEGACY_MENU_ANSWER_FILE.
    QString m_promptRequestId;
    QString m_promptAnswerFile;
    // Monotonic counter for per-request interactive session ids (never reused
    // across commands, so a stale PROMPT record can never match a later one).
    unsigned int m_interactiveRequestCounter;
    // Prompt-flow contract (L1/L2 + D5): the Shell tab's own persistent prompt
    // surface (hint label + input line + Send), enabled and focused only while
    // a shell PROMPT record is waiting.  A shell command's question is answered
    // inline in the Shell tab's output pane (m_shellOutput), never a separate
    // popup; the answer travels over the same mode-600 menu-answer file
    // (m_promptAnswerFile) as the repair menus.
    QLabel *m_shellPromptHint;
    QLineEdit *m_shellPromptInput;
    QPushButton *m_shellPromptSend;
    QLabel *m_chrootCommandHeading;
    std::vector<QCheckBox *> m_planChecks;
    // The saved per-stage plan preference, kept separately from the checkbox
    // state so the availability pass can force unavailable rows off (shown,
    // disabled, unchecked) without clobbering the user's choice, exactly like
    // MainWindow::m_fullRepairStagePreferences.
    std::vector<bool> m_planPreferences;
    // True once loadLegacySettings() has filled the preference register (and
    // the device-filter/auto-refresh/wrap state) from the persisted keys, or
    // from their defaults for absent keys.  Until then nothing may persist or
    // present the register: saveLegacySettings() and updatePlanChecks() refuse,
    // so the in-code defaults can never overwrite the saved Full Repair selection.
    bool m_settingsLoaded;
    // One-line diagnostic recorded by loadLegacySettings() (the resolved read
    // location plus the loaded repair/grub value), emitted by the constructor
    // once the session log directory exists (appendLog inside loadLegacySettings
    // would bypass a pre-set --log-dir).
    QString m_settingsLoadDiagnostic;
    // Last tooltip text applied per plan checkbox, so the availability pass
    // re-registers a tooltip only when the text changes (Qt3's QTipManager
    // reallocates the per-widget Tip record on every add).
    std::vector<QString> m_planCheckTips;
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

    // Per-section diagnostic results cache: a combined `diagnose all` run is
    // parsed into its per-key sections (title + body, exactly as the helper
    // framed them) so selecting a check shows only that check's own result.
    // The `report` aggregate shows the full combined report held in
    // m_fullDiagnosticReport. Cleared alongside m_model.reset() so the display
    // cache never outlives the capability cache of record.
    QMap<QString, QString> m_diagnosticSections;
    QString m_fullDiagnosticReport;

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
    // Item #4: the highlighted (preview) Systems tree row, separate from the
    // active-scope selection above. Merely highlighting a row (including an
    // unlocked target row while Host Maintenance is active) records the preview
    // here and never changes the scope or disables host-scope diagnostics/
    // repairs; only Select Target (commit) switches the scope.
    QString m_previewDisk;
    QString m_previewComponent;
    QString m_rootOverrideDisk;
    QString m_rootOverrideRoot;
    QString m_committedDisk;
    QString m_committedRoot;
    QString m_priorLogPath;
    std::vector<LogEntry> m_logEntries;
    // Item 4: the diagnostic stream buffered during a run in progress. The
    // lines are buffered (not appended live) so partial per-section results
    // stay out of the register until helperFinished() flushes the complete run
    // (modern hide-until-complete parity).
    std::vector<LogEntry> m_bufferedDiagnosticEntries;
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
    // True while the file system repair flow (fs-inspect + per-device
    // fs-repair) owns the session; the per-command diagnostics auto-refresh is
    // suppressed while it is set and re-scheduled once at the end of the flow.
    bool m_filesystemRepairFlowActive;
    // The exit status of the most recently finished helper command, set by
    // helperFinished(); the file system repair flow reads it to decide whether
    // a repair completed. Reset at the start of every startCommand().
    bool m_lastCommandOk;
    // Single-popup repair-plan driver: the Full Repair plan (and the standalone
    // Check File Systems flow) runs the fs-inspect -> inline per-device
    // decision -> per-device repair -> remaining stages sequence inside ONE
    // showRepairResultDialog popup, streaming continuously; the popup's Close
    // stays disabled until the whole sequence finishes. The driver is
    // re-entered from helperFinished()/sendPromptAnswer() through
    // advanceRepairPlan().
    enum RepairPlanPhase {
        RepairPlanIdle = 0,
        RepairPlanFsInspect,   // the read-only fs-inspect command is running
        RepairPlanFsDecision,  // the inline per-device decision is pending
        RepairPlanFsRepair,    // one per-device fs-repair command is running
        RepairPlanRemaining,   // the remaining Full Repair stages are running
        RepairPlanDone
    };
    bool m_repairPlanActive;          // the plan driver owns the session
    bool m_repairPlanFullRepair;      // Full Repair plan (else standalone fs flow)
    bool m_repairPlanHost;            // host-scope snapshot for the plan
    QString m_repairPlanDisk;
    QString m_repairPlanRoot;
    QString m_repairPlanScopeLabel;
    QStringList m_repairPlanRemainingStages; // helper stages after the fs stage
    QValueList<LegacyFsCheckResult> m_repairPlanCandidates;
    QValueList<QStringList> m_repairPlanCandidateModes;
    QStringList m_repairPlanChosenModes; // per candidate: chosen mode or ""
    int m_repairPlanRepairIndex;       // next candidate to repair, or -1
    bool m_repairPlanAnyRepairCompleted;
    bool m_repairPlanInlineQuestion;   // the inline decision is being asked
    RepairPlanPhase m_repairPlanPhase;
    bool m_pendingDiagnostic;
    // Quiet diagnostic runs are the scheduled auto-regeneration; the busy
    // indicator names them like the modern GUI does.
    bool m_pendingQuiet;
    QString m_pendingDiagnosticKey;
    bool m_pendingUnlock;
    bool m_pendingConfig;
    bool m_pendingShell;
    bool m_pendingBrowse;
    bool m_unlockRetry;
    bool m_targetCommitted;
    bool m_hostMaintenance;
    bool m_viewingPriorLog;
    // Device-discovery filter register (the plan path's m_planPreferences
    // mirror): loadLegacySettings() fills these, saveLegacySettings() writes
    // them, and updateDeviceFilterChecks() re-applies them to the checkboxes.
    bool m_showNonLinux;
    bool m_showRemovable;
    bool m_showEncrypted;
    bool m_logWrapEnabled;
    bool m_autoRefreshEnabled;
    QString m_pendingIdentity;
    QString m_pendingLabel;
    QString m_pendingConfigKey;
    QString m_pendingConfigPath;
    bool m_pendingConfigWrite;
    // Set when Host Maintenance is selected (or left) while a read-only
    // diagnostic is in flight: the diagnostic is cancelled and the scope
    // switch is deferred until helperFinished unwinds the cancelled run.
    // A mutating repair never sets this flag (never cancelled).
    bool m_scopeSwitchPending;
    // Set when Select Target commits a repair target while a read-only
    // diagnostic is in flight: the diagnostic is cancelled and the commit is
    // deferred until helperFinished unwinds the cancelled run (modern
    // setPreviewTarget() cancel parity). A mutating repair never sets it.
    // Mutually exclusive with m_scopeSwitchPending (the later action wins).
    bool m_commitPending;

    bool m_smokeMode;
    int m_smokeStep;
    bool m_smokeDiagnoseOk;
    bool m_smokeValidateOk;
    // Item #4: per-scope shell transcript. The Host Shell and Chroot Shell
    // keep separate command-history/transcript buffers so switching between
    // Host Maintenance and a committed repair target never shows the other
    // scope's output. The widget (m_shellOutput) always displays the ACTIVE
    // scope's buffer; updateChrootShellMode() parks the outgoing text and
    // restores the incoming one on the scope flip. Session-only (modern
    // parity: the shell history is not persisted to QSettings).
    QString m_shellTranscriptHost;
    QString m_shellTranscriptTarget;
    bool m_shellScopeHost;
    // Item #3 (modern parity): the Shell tab command history, session-only
    // (never persisted). Oldest -> newest; the index is -1 while editing a
    // fresh line and the draft holds the in-progress text saved when the user
    // steps Up away from that line.
    QStringList m_shellHistory;
    int m_shellHistoryIndex;
    QString m_shellHistoryDraft;
};

} // namespace legacy

#endif // LEGACY_MAIN_WINDOW_H
