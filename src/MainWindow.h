#pragma once

#include "CapabilityChecker.h"
#include "SystemScanner.h"

#include <QByteArray>
#include <QDateTime>
#include <QDialog>
#include <QHash>
#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QSet>
#include <QStringList>

class QAction;
class QBoxLayout;
class QGridLayout;
class QCheckBox;
class QCloseEvent;
class QComboBox;
class QEventLoop;
class QFile;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QProcess;
class QProgressBar;
class QPushButton;
class QResizeEvent;
class QSettings;
class QSplitter;
class QTabWidget;
class QTableWidget;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;
class QToolButton;

// One parsed "File system check <device>: ..." helper evidence line.  It is
// shared with the repair confirmation dialog, so it lives at namespace scope
// next to MainWindow.
struct FilesystemCheckResult {
    QString device;
    QString fstype;
    QString uuid;
    QString mount;
    QString tool;
    QString result;
    QString detail;
    bool needsRepair() const { return result == QStringLiteral("issues"); }
};

// Confirmation dialog for the File system repair tool.  It lists only the
// devices whose read-only inspection reported issues, lets the user select
// which devices to repair and which filesystem-specific mode to use, and
// returns the confirmed (device, mode) pairs.  The privileged helper repeats
// every safety preflight independently; this dialog only shapes the request.
class FilesystemRepairDialog final : public QDialog
{
public:
    struct Selection {
        QString device;
        QString mode;
    };

    FilesystemRepairDialog(const QString &scopeLabel,
                           const QList<FilesystemCheckResult> &results,
                           const QList<QStringList> &modes,
                           QWidget *parent = nullptr);

    QList<Selection> selections() const;

private:
    void updateRepairButtonState();

    QTableWidget *m_table = nullptr;
    QPushButton *m_repairButton = nullptr;
    QList<FilesystemCheckResult> m_results;
};

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    // Explicit operation kind recorded with every application-log entry. The
    // kind is chosen by the caller at append time and is written into the
    // entry's display header; the message text is never inspected to classify
    // an entry. The Logs filter dropdown selects entries by this kind.
    enum class LogEntryKind {
        Application,
        Diagnostic,
        Repair,
        Unlock,
        Snapshot,
        FileCopy,
        ChrootShell,
        HostShell
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Global informational busy indicator shared by every asynchronous
    // operation. beginBusyOperation()/endBusyOperation() are reference-counted
    // by label, so overlapping operations keep the indicator visible until the
    // last one finishes. The most recently started label is displayed. These
    // calls never change what an operation does or how it blocks.
    void beginBusyOperation(const QString &label);
    void endBusyOperation(const QString &label);

signals:
    void busyStateChanged(bool busy, const QString &label);

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // ---- Page construction -------------------------------------------------

    QWidget *buildSystemsPage();
    QWidget *buildDiagnosticsPage();
    QWidget *buildRepairPage();
    QWidget *buildSnapshotsPage();
    QWidget *buildChrootShellPage();
    QWidget *buildFileCopyPage();
    QWidget *buildLogsPage();
    QWidget *buildSettingsPage();

    // ---- Application setup, devices and target selection --------------------

    void buildMenuBar();
    void loadSettings();
    void saveSettings();
    void refreshDevices();
    void refreshCapabilities();
    void resizeCapabilityRows();
    void resizeSnapshotRows();
    void populateDeviceTree(const QList<DeviceNode> &devices);
    void updateHostSystemSummary(const QList<DeviceNode> &devices);
    void addDeviceItem(QTreeWidgetItem *parent, const DeviceNode &node);
    void indexDevice(const DeviceNode &node);
    void updateDeviceDetails();
    void showHostDetails();
    void selectHostForMaintenance();
    void setHostDefaultBootEntry();
    void showDeviceDetails(const DeviceNode &disk, bool allowRepairTarget, const DeviceNode *inspectedNode = nullptr);
    void setPreviewTarget();
    void unlockSelectedTarget();
    void updateTargetLabels();
    void updateCommittedTargetVisual();
    void updateFileCopyDirection();
    void updateFileCopyControls();
    void browseFileCopyDestination();
    void addSourceFiles();
    void addSourceFolder();
    void removeSelectedSources();
    void clearSourceList();
    void runFileCopyPreview();
    void runFileCopy();
    bool fileCopyReady(QString *reason = nullptr) const;

    // ---- Snapshots, shell and File Copy --------------------------------------

    void updateSnapshotControls();
    void clearSnapshotResults();
    void scheduleSnapshotPreload();
    void loadSnapshots();
    void inspectSelectedSnapshot();
    void rollbackSelectedSnapshot();
    void runChrootShellCommand();
    void clearChrootShellOutput();
    bool shellCommandReady(QString *reason = nullptr) const;
    void updateChrootShellMode();
    // APT release-info-change handling for user-run Chroot/Host shell commands.
    // The metadata-refresh repair stage names the affected repositories and
    // retries once with Acquire::AllowReleaseInfoChange=true; these helpers
    // provide the same detection and retry-command construction for the shell
    // so a vendor Origin/Label/Suite/Codename/Version change is never a dead
    // end. The option is confined to the retried command and never relaxes
    // signature, key or package verification.
    static bool outputHasAptReleaseInfoChange(const QString &output);
    static QStringList aptReleaseInfoChangedRepositories(const QString &output);
    static QString aptReleaseInfoChangeDetails(const QString &output);
    // Returns the command with -o Acquire::AllowReleaseInfoChange=true inserted
    // directly after the apt/apt-get/aptitude executable, or an empty string
    // when the command is not a confidently recognized apt-family command
    // (unknown prefixes, unterminated quotes, unsupported shell wrappers).
    static QString aptReleaseInfoChangeRetryCommand(const QString &command);

    // ---- Application log and session logs ------------------------------------

    void saveLogAs();
    void appendLog(const QString &message, const QString &level = QStringLiteral("INFO"),
                   LogEntryKind kind = LogEntryKind::Application);
    // Appends a recurring one-line status (device scan, diagnostic run framing,
    // automatic regeneration, authorization, stale notice) under a stable
    // identity. A repeat replaces the previous entry for the same identity at
    // the newest position instead of accumulating a copy per cycle, while
    // everything else keeps append-only history.
    void appendStatusLog(const QString &identity, const QString &message,
                         const QString &level = QStringLiteral("INFO"),
                         LogEntryKind kind = LogEntryKind::Application);
    QString trackedEntryIdentity(const QString &entry) const;
    QString currentLogScopeKey() const;
    // Diagnostic status identities are derived from exactly the fields the
    // status text contains, so two identities can never share an entry text
    // (which would make text-based replacement ambiguous).
    QString diagnosticRequestTitle(bool hostScope, const QString &key) const;
    QString diagnosticStartStatusIdentity(bool hostScope, const QString &key) const;
    QString diagnosticCompletionStatusIdentity(bool hostScope, const QString &key) const;
    QString diagnosticRequestStatusIdentity(bool hostScope, const QString &key) const;
    void appendDiagnosticRunStartStatus(bool hostScope, const QString &key);
    void appendDiagnosticRunCompletionStatus(bool hostScope, const QString &key,
                                             bool succeeded);
    QString sessionLogDirectory() const;
    QStringList sessionLogFiles() const;
    QString sessionLogLabel(const QString &path) const;
    QString sessionLogScopeSummary(const QString &path) const;
    QString sessionLogGenerationTime(const QString &path) const;
    QString detectedDistributionFamily() const;
    bool adoptActiveSessionLog();
    void restoreSessionScopeFromLog(const QString &logText);
    void flushPendingSessionEntries();
    void updateSessionScope();
    void createSessionLogFile(const QString &scopeLabel, const QString &disk,
                              const QString &component, const QString &family);
    void appendSessionScopeMarker(const QString &scopeLabel, const QString &disk,
                                  const QString &component, const QString &family);
    void writeSessionLogEntry(const QString &entry);
    void closeSessionLogFile();
    void refreshSessionLogList();
    void displaySessionLog(const QString &path);
    void startNewSessionLog();
    void clearCurrentSessionLog();
    void addSessionNote();
    void deleteSelectedSessionLog();
    void pruneSessionLogs();
    static QStringList parseSessionLogEntries(const QString &text);
    void appendDiagnosticLog(const QString &key, const QString &title,
                             const QString &scope, const QString &result,
                             bool succeeded);
    void rebuildDiagnosticLogIndex();
    // Replaceable repair sections. repairLogSectionIdentity() namespaces a
    // repair tool/stage key with the active host/target scope so the same tool
    // re-run replaces its previous block while a different tool or scope stays
    // a separate block. beginRepairLogSection()/finishRepairLogSection()
    // bracket one repair cycle; every entry appended in between (start notice,
    // privileged completion line and captured helper output) is captured.
    QString repairLogSectionIdentity(const QString &key) const;
    void beginRepairLogSection(const QString &section);
    void finishRepairLogSection(const QString &section);
    void showAboutDialog();
    void showUsageHelp();

    // ---- Diagnostics and evidence gating -------------------------------------

    void updateDiagnosticDetails();
    void runSelectedDiagnostic();
    void runAllDiagnostics();
    void copyDiagnosticResults();
    void saveDiagnosticResults();
    void editTargetConfig();
    QString diagnosticResultForKey(const QString &key) const;
    QString runTargetDiagnosticHelper(const QString &key, bool *succeeded = nullptr,
                                      bool showProgressDialog = false);
    QString runHostDiagnosticHelper(const QString &key, bool *succeeded = nullptr,
                                    bool showProgressDialog = false);
    void cacheTargetDiagnosticBundle(const QString &bundle);
    void cacheHostDiagnosticBundle(const QString &bundle);
    QString currentTargetDiagnosticCacheIdentity() const;
    void clearTargetDiagnosticCache();
    bool currentDiagnosticScopeReady(QString *reason = nullptr) const;
    bool currentScopeEvidenceStale(QString *reason = nullptr) const;
    bool shouldScheduleEvidenceRefresh(QString *reason = nullptr) const;
    void scheduleEvidenceRefresh(const QString &reason);
    void handlePrivilegedSessionEstablished();
    void runScheduledEvidenceRefresh();

    // ---- Privileged session and authorization --------------------------------

    bool ensurePrivilegedSession(QString *errorMessage = nullptr);
    void requestPrivilegedSessionForScope(const QString &scopeKey);
    void authorizePrivilegedSessionNow();
    void updateAuthorizationAffordance();
    QString currentPrivilegedScopeKey() const;
    QString runPrivilegedRequest(const QString &title, const QStringList &arguments,
                                 QByteArray secret = QByteArray(), bool *succeeded = nullptr,
                                 bool showProgressDialog = true,
                                 LogEntryKind kind = LogEntryKind::Application,
                                 const QString &statusIdentity = QString());
    void closePrivilegedSession();

    // ---- Repair tools and result reporting -----------------------------------

    void runSelectedRepairTool();
    void runFullRepair();
    // Refuses a new repair entry point while runFilesystemRepairFlow() owns the
    // read-only check and the sequential per-device repairs. Returns true when
    // the request was refused: a clear modal message was shown and the refusal
    // recorded in the repair log, and the caller must stop immediately instead
    // of queueing behind the running flow.
    bool repairBlockedByFilesystemFlow(const QString &actionTitle);
    // Ends a Full Repair plan execution and performs the single post-plan
    // invalidation and evidence regeneration (success, failure or abort).
    void finishFullRepairPlan();
    bool efiRepairReuseAvailable() const;
    // File system repair inspect/repair plumbing.  Inspection is read-only and
    // parses the helper's stable "File system check ..." evidence lines; a
    // repair only ever runs for a device that the inspection reported as
    // needing one and that the user explicitly selected. partOfFullRepair
    // selects the plan's pre-stage identity and title; a standalone run gets
    // its own replaceable section so it can never overwrite the plan's audit
    // trail in the live register.
    static QList<FilesystemCheckResult> parseFilesystemCheckOutput(const QString &output);
    // Human-readable audit detail for the parsed file system check evidence:
    // issue, skipped, unsupported/tool-missing and clean counts with the
    // affected devices, so the plan summary can never claim "no repair needed"
    // for a filesystem whose offline-only check was skipped.
    static QString filesystemCheckDetail(const QList<FilesystemCheckResult> &results);
    static QStringList filesystemRepairModes(const QString &fstype, bool mounted);
    QString runFilesystemInspect(bool partOfFullRepair);
    bool runFilesystemRepairFlow(bool partOfFullRepair = false);
    QString repairHelperPath() const;
    QStringList selectedRepairStages() const;
    bool repairTargetReady(QString *reason = nullptr) const;
    bool hostBootTargetReady(QString *reason = nullptr) const;
    bool hostMaintenanceReady(QString *reason = nullptr) const;
    bool hostDefaultBootReady(QString *reason = nullptr) const;
    void updateHostDefaultButtonState();
    bool repairToolAvailable(const QString &key, QString *reason = nullptr) const;
    bool repairEvidenceReadyForTool(const QString &toolKey, QString *reason = nullptr) const;
    // Single explicit mapping from a repair tool/stage key to the diagnostic
    // sections whose cached evidence that repair invalidates. "all" is the
    // combined-report pseudo-section and means the complete set; an empty list
    // means the action is read-only. Unknown keys map to "all" (fail safe).
    // The documented table lives in MainWindow.cpp; every invalidation decision
    // must go through this function.
    QStringList diagnosticSectionsForRepair(const QString &toolKey) const;
    QString diagnosticSectionTitle(const QString &key) const;
    // Evidence-driven invalidation. Every successful modifying helper action
    // ends with one stable "Repair change status <tool-key>: changed" or
    // "Repair change status <tool-key>: unchanged|<reason>" line. The GUI reads
    // the transcript and skips cached-evidence invalidation only for a proven
    // unchanged action; a missing or malformed status keeps the fail-safe
    // changed behavior. repairToolKeyForStage() normalizes helper stage names
    // (dpkg-configure, fix-broken, ...) to capability/UI keys.
    static QString repairToolKeyForStage(const QString &stage);
    static QMap<QString, QString> parseRepairChangeStatuses(const QString &output);
    static bool repairChangeStatusIsUnchanged(const QString &status);
    // Tool keys that must invalidate their mapped sections: every requested key
    // without an explicit unchanged status, plus any additional key the helper
    // reported changed (for example the GRUB follow-up of an EFI repair).
    static QStringList invalidatingRepairToolKeys(const QStringList &requestedKeys,
                                                  const QString &output);
    // Repair-result categorization. Every repair action result is reported as
    // exactly one of: success (exit 0 with a proven change), failed (non-zero
    // exit or an issue that could not be repaired), no-repair-needed (exit 0
    // with a proven no-op or a read-only preflight), skipped (the stage ran
    // no check or repair at all, for example a mounted filesystem whose
    // offline-only check was skipped) or not-run (a Full Repair stage the plan
    // never reached because an earlier stage failed; it is not a failure of
    // its own). The categorization uses the same change-status evidence and
    // the same fail-safe default as invalidatingRepairToolKeys(), so
    // no-repair-needed always implies no cached-evidence invalidation.
    enum class RepairResultCategory {
        Success,
        Failed,
        NoRepairNeeded,
        Skipped,
        NotRun
    };
    // One categorized Full Repair stage. The plan summary is built from the
    // ordered list so it stays auditable even though each stage's captured
    // output lives in its own replaceable section. reason carries the helper's
    // short failure reason; detail carries the proven-unchanged helper reason
    // or the file system check evidence.
    struct RepairStageResult {
        QString toolKey;
        RepairResultCategory category = RepairResultCategory::NoRepairNeeded;
        QString reason;
        QString detail;
    };
    static RepairResultCategory categorizeRepairResult(bool processSucceeded,
                                                       const QStringList &requestedKeys,
                                                       const QString &output);
    // "Repair result: ✓ <action-accurate phrase>" and its sibling forms. The
    // phrase comes from the shared per-tool vocabulary table so the
    // single-action summary and the plan stage lines can never drift; an empty
    // toolKey selects the generic fallback. The reason is only used for the
    // failed form.
    static QString repairResultSummary(RepairResultCategory category,
                                       const QString &toolKey = QString(),
                                       const QString &reason = QString());
    static QString repairResultSymbol(RepairResultCategory category);
    static QString repairResultStageLine(const RepairStageResult &stage);
    // Short, single-line failure reason extracted from the helper transcript
    // (last "ERROR:" line, else the last line mentioning a failure).
    static QString shortRepairFailureReason(const QString &output);
    // Tool key of the stage the helper named in its last
    // "ERROR: stage '<stage>' failed:" line, normalized like
    // repairToolKeyForStage(). Empty when the helper failed before naming a
    // stage (for example a preflight failure). A decorated stage label such as
    // "grub (EFI follow-up)" resolves to its base tool key.
    static QString repairFailureStageKey(const QString &output);
    // "Full Repair results: ✓ n successful · ✗ n failed · ▪ n no repair
    // needed [· ▪ n not checked — see the <tool> stage line] [· ▪ n not run
    // — see the <tool> stage line]" plus one auditable result line per stage.
    // The not-checked reference names the skipped stage(s) so the aggregate
    // stays traceable to the stage line that carries the affected device
    // list; the not-run reference names the stages an earlier failure stopped
    // before, so they are never confused with the failed stage.
    static QString fullRepairPlanSummary(const QList<RepairStageResult> &stages);
    // Records one Full Repair stage result; a no-op outside a running plan.
    void recordPlanStageResult(const QString &toolKey, RepairResultCategory category,
                               const QString &reason = QString(),
                               const QString &detail = QString());
    void appendRepairResultSummary(RepairResultCategory category, const QString &reason,
                                   LogEntryKind kind,
                                   const QString &toolKey = QString());
    // Colors the ✓/✗/▪ symbols of repair-result summary lines in the log view
    // with character formats only (the document stays plain text). text is the
    // just-written content and documentOffset its start position in the
    // document (0 for a prepend or a full rebuild).
    void applyRepairResultColoring(const QString &text, int documentOffset = 0);
    // True while the active scope has invalidated diagnostic sections (or the
    // full target set) that the next coalesced regeneration must refresh.
    bool diagnosticsInvalidationPending() const;
    void clearHostDiagnosticCache();
    void invalidateAllActiveScopeDiagnostics();
    void invalidateAllTargetDiagnostics();
    void markDiagnosticSectionsStale(bool hostScope, const QStringList &sections);
    // Marks the mapped sections stale in the active scope, updates the
    // readiness controls and schedules one coalesced regeneration. Never called
    // while a Full Repair plan owns the repairs; the plan accumulates instead.
    void invalidateDiagnosticsForRepair(const QStringList &toolKeys, bool failed,
                                        const QString &reason, LogEntryKind kind);
    // Accumulates the union of sections invalidated by a Full Repair plan so
    // the plan regenerates them once at the end.
    void accumulatePlanInvalidation(const QStringList &toolKeys, bool failed);
    // Sequentially regenerates only the requested sections in the active scope
    // through the existing per-section helper path and replaces only their
    // cached entries and log blocks.
    void runDiagnosticSections(const QStringList &sections);
    bool confirmRepairAction(const QString &title, const QStringList &operations);
    void runRepairHelper(const QString &title, const QStringList &arguments,
                         LogEntryKind kind = LogEntryKind::Repair,
                         const QString &sectionKey = QString());

    // ---- Presentation and responsive layout ----------------------------------

    void updateBusyIndicator();
    void updateResponsiveLayout();
    void updateFullRepairSummary();
    void updateFullRepairPlanHeight();
    void updateRepairScopeControls();
    void updateRepairToolDetails();
    void setLogWrapEnabled(bool enabled);
    void refreshLogView();
    void scheduleLogRefresh();
    void autoSizeDeviceColumns();
    void applyDeviceColumnDefaults();
    void updateDeviceTreeHeight();
    void normalizeButtonSizing();
    bool devicePassesTopLevelFilters(const DeviceNode &device) const;

    static QString combinedModel(const DeviceNode &node);
    static QIcon iconForDevice(const DeviceNode &node);

    // ---- Data members --------------------------------------------------------

    SystemScanner *m_scanner = nullptr;
    QSettings *m_settings = nullptr;
    QStringList m_actionLogEntries;
    // Diagnostic sections are indexed by (scope, key) so a newer result can
    // replace the previous section in place instead of accumulating
    // duplicates across repeated automatic regenerations.
    QMap<QString, QString> m_diagnosticLogEntries;
    // Repair output blocks are indexed by the same kind of stable
    // (scope, tool/stage key) identity. Each value is the ordered list of
    // application-log entries that make up one repair cycle; a re-run replaces
    // the whole list instead of appending a second copy.
    QMap<QString, QStringList> m_repairLogEntries;
    // Recurring one-line status entries are indexed by a stable identity so a
    // repeat updates the existing entry in place. Values are the current
    // register entry text; the reverse map lets the incremental renderer
    // recognize a tracked entry from the register text alone.
    QMap<QString, QString> m_statusLogEntries;
    QHash<QString, QString> m_statusEntryIdentities;
    // Identity of the repair section currently being captured, and the entries
    // appended while it is active. appendLog() records into this pair.
    QString m_activeRepairSection;
    QStringList m_activeRepairEntries;
    // Deferred incremental view updates: entries prepended since the last
    // refresh, and rendered replaceable entries (diagnostic sections and
    // tracked status lines) whose replacement must remove the old block range
    // before the new entry is prepended.
    QStringList m_pendingLogEntries;
    QSet<QString> m_pendingEntryRemovals;
    // Character ranges (start, end) of the replaceable entries currently in
    // the rendered document, keyed by their stable identity.
    QHash<QString, QPair<int, int>> m_renderedEntryRanges;
    QTimer *m_logRefreshTimer = nullptr;
    quint64 m_logRefreshCount = 0;
    bool m_logViewRendered = false;
    bool m_logModelTrimmed = false;
    bool m_staleHighlightValid = false;
    bool m_staleHighlightFlag = false;
    QSet<QString> m_staleHighlightSections;
    QString m_renderedLogQuery;
    QString m_renderedLogFilter;
    bool m_renderedPriorLog = false;

    QTabWidget *m_tabs = nullptr;
    QToolButton *m_tabScrollLeftButton = nullptr;
    QToolButton *m_tabScrollRightButton = nullptr;
    QAction *m_wrapLogsAction = nullptr;
    QAction *m_lockAuthorizationAction = nullptr;
    QWidget *m_busyIndicator = nullptr;
    QProgressBar *m_busyProgress = nullptr;
    QLabel *m_busyStatusLabel = nullptr;
    QStringList m_activeBusyOperations;

    QTreeWidget *m_deviceTree = nullptr;
    QSplitter *m_systemSplitter = nullptr;
    QPushButton *m_refreshDevicesButton = nullptr;
    QPushButton *m_setTargetButton = nullptr;
    QPushButton *m_unlockTargetButton = nullptr;
    QLabel *m_systemTargetLabel = nullptr;
    QGroupBox *m_unlockStatusBox = nullptr;
    QPlainTextEdit *m_unlockStatusView = nullptr;
    QMap<QString, QString> m_unlockStatusCache;
    bool m_targetDiagnosticsNeedRegeneration = false;
    // Scoped invalidation: sections whose cached evidence was invalidated by a
    // repair action but not yet regenerated. The full flag above supersedes
    // this set, and the host set covers the same bookkeeping in host mode.
    QSet<QString> m_targetDiagnosticsStaleSections;
    QSet<QString> m_hostDiagnosticsStaleSections;

    QLabel *m_hostSystemLabel = nullptr;
    QLabel *m_hostStorageLabel = nullptr;
    QLabel *m_hostMountsLabel = nullptr;
    QPushButton *m_hostDetailsButton = nullptr;
    QPushButton *m_hostMaintenanceButton = nullptr;
    QPushButton *m_hostDefaultButton = nullptr;
    QBoxLayout *m_hostCardLayout = nullptr;
    QGridLayout *m_hostActionsLayout = nullptr;
    QLabel *m_hostProtectedBadge = nullptr;
    bool m_hostActionsStacked = false;

    QLabel *m_detailPath = nullptr;
    QLabel *m_detailResolvedTarget = nullptr;
    QLabel *m_detailModel = nullptr;
    QLabel *m_detailSize = nullptr;
    QLabel *m_detailTransport = nullptr;
    QLabel *m_detailFilesystem = nullptr;
    QLabel *m_detailUuid = nullptr;
    QLabel *m_detailMounts = nullptr;
    QLabel *m_detailStatus = nullptr;
    QLabel *m_detailProtection = nullptr;
    QGroupBox *m_selectedDriveDetailsBox = nullptr;

    QLabel *m_repairTargetLabel = nullptr;
    QLabel *m_snapshotTargetLabel = nullptr;
    QLabel *m_fileCopyTargetLabel = nullptr;
    QLabel *m_chrootShellTargetLabel = nullptr;
    QLabel *m_chrootShellHeading = nullptr;
    QLabel *m_chrootShellNotice = nullptr;
    QLabel *m_chrootShellWarning = nullptr;
    QLineEdit *m_chrootShellCommandEdit = nullptr;
    QPlainTextEdit *m_chrootShellOutput = nullptr;
    QPushButton *m_chrootShellRunButton = nullptr;

    QSplitter *m_diagnosticSplitter = nullptr;
    QListWidget *m_diagnosticList = nullptr;
    QComboBox *m_diagnosticScopeCombo = nullptr;
    QLabel *m_diagnosticTitle = nullptr;
    QLabel *m_diagnosticDescription = nullptr;
    QLabel *m_diagnosticAvailability = nullptr;
    QPlainTextEdit *m_diagnosticResults = nullptr;
    QPushButton *m_runDiagnosticButton = nullptr;
    QPushButton *m_runAllDiagnosticsButton = nullptr;
    QPushButton *m_copyDiagnosticButton = nullptr;
    QPushButton *m_saveDiagnosticButton = nullptr;
    QComboBox *m_targetConfigCombo = nullptr;
    QPushButton *m_editTargetConfigButton = nullptr;

    QSplitter *m_repairVerticalSplitter = nullptr;
    bool m_repairPlanSplitterUserAdjusted = false;
    QSplitter *m_repairSplitter = nullptr;
    QTreeWidget *m_repairToolTree = nullptr;
    QLabel *m_repairToolTitle = nullptr;
    QLabel *m_repairToolDescription = nullptr;
    QLabel *m_repairToolPlanStatus = nullptr;
    QPushButton *m_repairToolButton = nullptr;
    QLabel *m_fullRepairCountLabel = nullptr;
    QLabel *m_fullRepairReadinessLabel = nullptr;
    QGroupBox *m_fullRepairPlanBox = nullptr;
    QListWidget *m_fullRepairStageList = nullptr;
    QPushButton *m_runFullRepairButton = nullptr;

    QTableWidget *m_snapshotTable = nullptr;
    QPlainTextEdit *m_snapshotDetails = nullptr;
    QSplitter *m_snapshotSplitter = nullptr;
    QBoxLayout *m_snapshotButtonLayout = nullptr;
    QPushButton *m_snapshotLoadButton = nullptr;
    QPushButton *m_snapshotInspectButton = nullptr;
    QPushButton *m_snapshotRollbackButton = nullptr;
    bool m_snapshotPreloadScheduled = false;
    QLabel *m_fileCopyHeading = nullptr;
    QComboBox *m_fileCopyDirectionCombo = nullptr;
    QGroupBox *m_fileCopySourceBox = nullptr;
    QGroupBox *m_fileCopyDestinationBox = nullptr;
    QListWidget *m_sourceList = nullptr;
    QPushButton *m_fileCopyAddFilesButton = nullptr;
    QPushButton *m_fileCopyAddFolderButton = nullptr;
    QPushButton *m_fileCopyBrowseDestinationButton = nullptr;
    QPushButton *m_fileCopyPreviewButton = nullptr;
    QPushButton *m_fileCopyRunButton = nullptr;
    QLineEdit *m_destinationEdit = nullptr;
    QComboBox *m_ownershipCombo = nullptr;

    QPlainTextEdit *m_logView = nullptr;
    QLineEdit *m_logSearchEdit = nullptr;
    QComboBox *m_logFilterCombo = nullptr;
    QSplitter *m_sessionLogSplitter = nullptr;
    QWidget *m_sessionLogPanel = nullptr;
    QWidget *m_sessionLogViewer = nullptr;
    QListWidget *m_sessionLogList = nullptr;
    QLabel *m_priorLogBanner = nullptr;
    QPushButton *m_newSessionLogButton = nullptr;
    QPushButton *m_clearSessionLogButton = nullptr;
    QPushButton *m_addNoteButton = nullptr;
    QPushButton *m_deleteSessionLogButton = nullptr;
    QPushButton *m_refreshSessionLogsButton = nullptr;
    QFile *m_sessionLogFile = nullptr;
    QString m_sessionLogPath;
    // Path of the session file owned by this process's current run. Empty on a
    // fresh launch until the first scope identification creates the file; a
    // second MainWindow in the same process adopts this path instead of
    // starting another session or loading a prior file.
    static QString s_activeSessionPath;
    QStringList m_pendingSessionEntries;
    QString m_sessionScopeKey;
    QString m_sessionScopeLabel;
    QString m_sessionScopeDisk;
    // True once this window has identified (or re-confirmed) a scope for the
    // current session file itself. A scope restored from an adopted file stays
    // false so construction-time device refreshes never write a "No scope"
    // marker for a scope this window has not touched.
    bool m_sessionScopeIdentifiedHere = false;
    QString m_viewedLogPath;
    QStringList m_viewedLogEntries;
    bool m_viewingPriorLog = false;
    QTableWidget *m_capabilityTable = nullptr;
    QLabel *m_distributionLabel = nullptr;
    QLabel *m_packageManagerLabel = nullptr;
    QLabel *m_authBuildLabel = nullptr;

    QCheckBox *m_showNonLinux = nullptr;
    QCheckBox *m_showRemovable = nullptr;
    QCheckBox *m_showEncrypted = nullptr;
    QCheckBox *m_autoRefreshDiagnostics = nullptr;
    QCheckBox *m_fullRepairFilesystem = nullptr;
    QCheckBox *m_fullRepairDpkg = nullptr;
    QCheckBox *m_fullRepairBrokenPackages = nullptr;
    QCheckBox *m_fullRepairAptUpdate = nullptr;
    QCheckBox *m_fullRepairUpgrade = nullptr;
    QCheckBox *m_fullRepairDkms = nullptr;
    QCheckBox *m_fullRepairDisplayManager = nullptr;
    QCheckBox *m_fullRepairInitramfs = nullptr;
    QCheckBox *m_fullRepairEfi = nullptr;
    QCheckBox *m_fullRepairGrub = nullptr;

    QList<DeviceNode> m_lastDevices;
    QMap<QString, DeviceNode> m_deviceIndex;
    QString m_hostPrimaryPath;
    QString m_hostPrimaryComponentPath;
    bool m_hostMaintenanceMode = false;
    QString m_previewTargetPath;
    QString m_previewTargetComponentPath;
    QString m_snapshotResultIdentity;

    QMap<QString, QString> m_targetDiagnosticCache;
    QMap<QString, QDateTime> m_targetDiagnosticTimes;
    QMap<QString, QString> m_hostDiagnosticCache;
    QMap<QString, QDateTime> m_hostDiagnosticTimes;
    QString m_targetDiagnosticCacheIdentity;

    QTimer *m_evidenceRefreshTimer = nullptr;
    QString m_evidenceRefreshReason;
    bool m_evidenceRefreshPending = false;
    bool m_evidenceRefreshInProgress = false;
    bool m_diagnosticsRunInProgress = false;
    bool m_runAllDiagnosticsQueued = false;
    bool m_privilegedOperationActive = false;
    // True while runFullRepair() owns the complete plan. Modifying stages keep
    // the evidence they were approved with and do not schedule their own
    // regeneration; finishFullRepairPlan() invalidates and regenerates once
    // after the last stage, including aborted and failed plans.
    bool m_fullRepairPlanInProgress = false;
    // True from the read-only file system check through the last selected
    // device's repair. Repair entry points refuse a second request with a
    // clear message while it is set, and the global busy indicator stays
    // visible until the complete flow ends (success, failure or cancel).
    bool m_filesystemRepairFlowActive = false;
    bool m_fullRepairPlanModified = false;
    // True when the plan executed at least one modifying stage, even if every
    // one of them reported unchanged. finishFullRepairPlan() uses it to log the
    // no-change notice instead of staying silent for a read-only-only plan.
    bool m_fullRepairPlanModifyingStageRan = false;
    // Union of the diagnostic sections invalidated by the plan's stages and
    // whether any stage required the complete set. Applied exactly once by
    // finishFullRepairPlan().
    QSet<QString> m_fullRepairPlanInvalidatedSections;
    bool m_fullRepairPlanRequiresFullInvalidation = false;
    // Ordered result of every Full Repair plan stage (the file system
    // pre-stage first, then the helper stages) plus whether its summary was
    // already written into the plan's log section. finishFullRepairPlan()
    // writes the summary for plans that never reached the helper (for example
    // a file-system-only plan) and clears both.
    QList<RepairStageResult> m_fullRepairPlanStageResults;
    bool m_fullRepairPlanSummaryLogged = false;
    // True when the EFI / UKI bootloader stage repaired the current scope in
    // this session. Reconcile boot stack then passes the explicit --post-efi
    // hint so the helper skips the second UKI rebuild and the duplicate GRUB
    // regeneration instead of guessing from the stage list.
    bool m_efiBootloaderRepaired = false;
    QString m_efiBootloaderRepairedScope;

    QProcess *m_privilegedSession = nullptr;
    // Non-modal wait loop owned by the current ensurePrivilegedSession() call;
    // null unless a pkexec/Polkit authorization is being awaited. The session
    // QProcess handlers end the wait as soon as the outcome is known.
    QEventLoop *m_privilegedSessionWaitLoop = nullptr;
    quint64 m_privilegedRequestCounter = 0;
    bool m_privilegedSessionReady = false;
    // True while one ensurePrivilegedSession() call owns the single Polkit
    // conversation. Concurrent callers coalesce onto that request instead of
    // launching a second pkexec prompt.
    bool m_privilegedSessionRequestInFlight = false;
    // Set as soon as an in-flight authorization request is known to have
    // failed or been cancelled, so coalescing callers can stop waiting instead
    // of blocking until the authorization wait loop ends.
    bool m_privilegedSessionRequestFailed = false;

    QLabel *m_authorizationStatusLabel = nullptr;
    QPushButton *m_authorizeNowButton = nullptr;
    QString m_authorizationScopeRequested;
    QString m_authorizationDeferredScope;
    bool m_authorizationRequestInFlight = false;
    quint64 m_privilegedSessionRequestCount = 0;
    bool m_uiTestPrivilegedSessionGranted = false;
    // Test-only hold time that keeps the UI-test authorization request
    // "in flight" so concurrency/coalescing can be exercised offscreen.
    int m_uiTestPrivilegedSessionDelayMs = 0;
};
