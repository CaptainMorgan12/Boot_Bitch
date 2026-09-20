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
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QVariant>

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

// Numeric-aware sort support shared by every sortable table and tree in the
// window. Formatted text stays in Qt::DisplayRole; a dedicated role carries the
// underlying number, date or rank so a size/count/date column orders by its
// real value instead of its formatted string. Items without a key fall back to
// a natural-order text comparison, so /dev/sda2 sorts before /dev/sda10.
constexpr int BootRepairSortKeyRole = Qt::UserRole + 64;

// QTableWidgetItem with numeric-aware sorting: an optional explicit sort key
// (number, QDateTime or rank) wins over the formatted display text.
class SortableTableItem final : public QTableWidgetItem
{
public:
    explicit SortableTableItem(const QString &text = QString(),
                               const QVariant &sortKey = QVariant());
    bool operator<(const QTableWidgetItem &other) const override;
};

// QTreeWidgetItem with numeric-aware per-column sorting. The comparison uses
// the tree's current sort column, so top-level drives and their partitions are
// each ordered by the clicked column while the item identity (and therefore
// selection and expansion state) is preserved.
class SortableTreeWidgetItem final : public QTreeWidgetItem
{
public:
    using QTreeWidgetItem::QTreeWidgetItem;
    bool operator<(const QTreeWidgetItem &other) const override;
};

// One parsed "SNAPSHOT\t..." helper record. loadSnapshots() parses the
// read-only helper protocol into these rows and populateSnapshotTable()
// renders them into the Snapshots inventory. The type is shared with the UI
// tests so the production row population and its numeric/date sort keys can be
// exercised without a privileged helper.
struct SnapshotInventoryRow {
    QString id;
    QString created;
    QString type;
    QString description;
    QString status;
    QString relativePath;
};

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

// Custom-painted indeterminate busy indicator. A QProgressBar with range 0,0
// delegates its animation to the platform style, which renders as a static
// solid bar on some distributions (for example Fedora/Adwaita) and as animated
// stripes on others. This widget paints its own palette-derived segmented
// marquee instead, so the busy state looks and animates identically on every
// distribution and theme. The animation runs only while the widget is visible
// and the owner says the busy state is active.
class BusyIndicatorWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit BusyIndicatorWidget(QWidget *parent = nullptr);

    // True while the marquee timer is running. The UI tests use this to pin
    // that the indicator starts and stops with the busy state; the painted
    // animation itself is deliberately not pixel-tested.
    bool isAnimating() const;
    qreal animationPhase() const;

    // Starts or stops the marquee. A hidden widget never animates, so showing
    // the owner again resumes a pending busy state.
    void setAnimating(bool animating);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    QTimer *m_animationTimer = nullptr;
    qreal m_phase = 0.0;
    bool m_shouldAnimate = false;
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

    // Parses a portal Read/SettingChanged value into a scheme. Handles the
    // GNOME string values ("prefer-dark"/"prefer-light") and the extra variant
    // layer GNOME's Settings portal wraps around them. Public so the UI suite
    // can pin the parsing independently of a running portal.
    static Qt::ColorScheme colorSchemeFromPortalValue(const QVariant &value);

signals:
    void busyStateChanged(bool busy, const QString &label);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
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
    // Leaves explicit running-host maintenance through the single shared path
    // (button state, scope labels, session scope, authorization affordance).
    // Returns true when maintenance was active and has now been deselected.
    bool exitHostMaintenanceMode();
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
    // Target + resolved component identity of the Btrfs inventory a preload or
    // request serves. Empty when no repair target is committed.
    QString snapshotScopeKey() const;
    // Renders parsed snapshot rows into the inventory with numeric/date sort
    // keys and re-applies the active (or default Created-descending) sort.
    void populateSnapshotTable(const QList<SnapshotInventoryRow> &rows);
    // A non-Btrfs repair target has no Btrfs snapshot inventory. This is an
    // informational outcome: the Snapshots page states the applicability and
    // nothing is recorded at ERROR level.
    void showSnapshotInventoryNotApplicable(const QString &fileSystem);
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
    // Backend detection parsed from the cached read-only diagnostics.  Stage
    // labels and Settings text derive from these detected backends, never from
    // the distribution family.
    QString currentScopeEvidence() const;
    QStringList detectedPackageManagers() const;
    QString detectedServiceManager() const;
    QString detectedInitramfsBackend() const;
    QString detectedBootloaderBackend() const;
    // True when the cached evidence names the guarded rpm/dnf package backend
    // (Fedora/RPM family).  The package stage labels use the Fedora wording
    // only for a single detected manager; mixed-manager targets keep the
    // generic wording.
    bool detectedRpmBackend() const;
    // True when the detected GRUB backend uses Fedora's grub2 tooling and
    // paths (grub2-mkconfig, /boot/grub2, grub2-editenv) rather than the
    // Debian/Arch grub-* names.
    bool detectedGrub2Backend() const;
    // Name of the detected graphical login manager ("GDM", "GDM3", "SDDM",
    // ...) parsed from the cached display evidence; empty when the evidence
    // names no specific manager.
    QString detectedDisplayManagerName() const;
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
    // Adopts the component named by the helper's "Root component fallback"
    // evidence when a privileged target diagnostic had to move away from the
    // committed component. Keeps the committed target, later requests and the
    // log on the one resolved system.
    bool reconcileResolvedTargetComponent(const QString &output);
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
    // Diagnostics follow the Systems page: the protected Running Host while
    // Host Maintenance is active, otherwise the committed Repair Target. The
    // scope is derived, never independently selectable, so the cached results
    // and the run actions can never disagree with Systems.
    bool diagnosticHostScope() const;
    // Single actionable refusal for a running-host diagnostic request outside
    // the explicit host-maintenance scope. Returns true while host maintenance
    // is active; otherwise fills reason with the user-facing instruction.
    bool hostDiagnosticScopeAllowed(QString *reason = nullptr) const;
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
    // Runs the work that arrived while a privileged request owned the gate:
    // one deferred Btrfs snapshot preload and at most one coalesced automatic
    // diagnostics regeneration for the current scope. Both were refused as
    // requests while the gate was held; neither is queued on the gate.
    void runDeferredPrivilegedWork();

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
    // Runs the read-only fs-inspect/host-fs-inspect helper command as the
    // dedicated "File systems" diagnostic. Shares the diagnostic status and
    // caching contract with every other diagnostic; the result is the helper's
    // per-device "File system check ..." evidence.
    QString runFilesystemDiagnostic(bool hostScope, bool *succeeded = nullptr,
                                    bool showProgressDialog = false);
    // Splits the helper's shared capability preamble out of one diagnostic
    // result. The preamble carries the `Repair tool <key>` gating contract and
    // is cached under the dedicated capability key so it can never leak into
    // the diagnostic result itself (fstab stays purely fstab); body receives
    // the diagnostic-only output.
    static void splitDiagnosticCapabilityPreamble(const QString &captured,
                                                  QString *body,
                                                  QString *preamble);
    // Ordered privileged-helper resolution candidates for one application
    // directory, each with the reason it would be chosen. Installed layouts
    // (/usr/bin, /usr/local/bin) prefer the libexec helper; build/portable
    // layouts prefer the helper beside or above the executable. Exposed for
    // the UI regression tests so the resolution order can be asserted without
    // touching the host.
    struct HelperPathCandidate {
        QString path;
        QString reason;
    };
    static QList<HelperPathCandidate> repairHelperCandidates(const QString &appDir,
                                                             const QByteArray &explicitOverride);
    // Resolves the privileged helper: explicit override first, then the
    // ordered candidates; resolution names the chosen reason.
    static QString resolveRepairHelperPath(const QString &appDir,
                                           const QByteArray &explicitOverride,
                                           QString *resolution = nullptr);
    QString repairHelperPath(QString *resolution = nullptr) const;
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
    // failed form; the detail (the helper's proven-unchanged reason or the
    // held-back/skipped package-manager feedback appended to a change status)
    // is appended for the non-failed forms so kept-back package names stay
    // visible in the result summary.
    static QString repairResultSummary(RepairResultCategory category,
                                       const QString &toolKey = QString(),
                                       const QString &reason = QString(),
                                       const QString &detail = QString());
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
                                   const QString &toolKey = QString(),
                                   const QString &detail = QString());
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

    // Where the applied desktop scheme came from. The source is logged with
    // the scheme so the runtime resolution stays diagnosable: Qt auto-loads
    // the GTK3 platform theme on GNOME and that theme reports Adwaita's light
    // scheme even in a prefer-dark session, so the portal/gsettings evidence
    // is what actually decides.
    enum class ColorSchemeSource {
        TestOverride,
        Platform,
        Portal,
        GSettings,
        Palette
    };

    // Applies the resolved desktop color scheme to the application palette
    // whenever the current palette does not match it, so a GNOME/Fedora dark
    // desktop can never render the app light. Reacts to
    // QStyleHints::colorSchemeChanged at runtime and re-renders every
    // palette-derived custom color (log text/selection, stale highlighting,
    // repair-result glyphs, committed-target highlight, scroll fades).
    void applyColorScheme();
    void updateThemeDependentColors();
    // Coalesces a deferred scheme re-resolution into one event-loop turn.
    void scheduleColorSchemeRefresh();
    // Re-resolves the scheme after startup: the platform palette can be
    // replaced after the first paint and the portal reply can arrive late, so
    // every deferred check re-applies the best evidence.
    void refreshColorScheme();
    // Resolves the desktop scheme through the XDG portal before the first
    // paint (short synchronous timeout) and subscribes to SettingChanged so a
    // runtime GNOME appearance switch is picked up even when the platform
    // theme emits no ThemeChange.
    void queryPortalColorScheme();
    void requestPortalColorScheme(const QString &settingsNamespace, const QString &key);
    // Read-only gsettings fallback used when the portal is unreachable or
    // answers nothing. Never runs on the offscreen/minimal test platform.
    void queryGsettingsColorScheme();
    // One application-log + stderr line per scheme/source change.
    void logColorSchemeResolution(Qt::ColorScheme scheme, ColorSchemeSource source);
    static Qt::ColorScheme effectiveColorScheme();
    Qt::ColorScheme resolvedColorScheme(ColorSchemeSource *source = nullptr) const;
    // Test-only override so the offscreen suite can exercise both schemes
    // deterministically. Unknown means "use the desktop scheme".
    static void setColorSchemeOverrideForTests(Qt::ColorScheme scheme);
    static Qt::ColorScheme colorSchemeOverrideForTests();
    static Qt::ColorScheme s_colorSchemeOverride;

private slots:
    // Portal SettingChanged hook. The signal carries (namespace, key, value)
    // but this slot intentionally takes no arguments: the value is re-read
    // through the normal portal path so the parsing and fallback chain stay in
    // one place. Any settings change is rare enough to re-check on.
    void portalSettingsChanged();

private:
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

    // Desktop color scheme resolution state. Desktop evidence (the XDG portal,
    // then the GNOME gsettings value) wins over QStyleHints when they
    // disagree, because the GTK3 platform theme Qt auto-loads on GNOME reports
    // Adwaita light in a dark session. The last applied scheme plus the
    // coalescing/reentrancy guards keep the deferred re-checks from
    // re-rendering or re-applying a palette that already matches.
    Qt::ColorScheme m_portalColorScheme = Qt::ColorScheme::Unknown;
    Qt::ColorScheme m_gsettingsColorScheme = Qt::ColorScheme::Unknown;
    Qt::ColorScheme m_lastAppliedColorScheme = Qt::ColorScheme::Unknown;
    // Last "scheme|source" pair written to the application log, so repeated
    // palette events do not spam the register with identical lines.
    QString m_lastLoggedColorScheme;
    bool m_portalColorSchemeQueryStarted = false;
    bool m_gsettingsColorSchemeQueryStarted = false;
    bool m_colorSchemeRefreshScheduled = false;
    bool m_applyingColorScheme = false;

    QTabWidget *m_tabs = nullptr;
    QToolButton *m_tabScrollLeftButton = nullptr;
    QToolButton *m_tabScrollRightButton = nullptr;
    QAction *m_wrapLogsAction = nullptr;
    QAction *m_lockAuthorizationAction = nullptr;
    QWidget *m_busyIndicator = nullptr;
    BusyIndicatorWidget *m_busyProgress = nullptr;
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
    // Standard top-right scope line shared with the other tabs. Diagnostics
    // follow the Systems page: the protected Running Host while Host
    // Maintenance is active, otherwise the committed Repair Target. There is
    // deliberately no independent scope selector.
    QLabel *m_diagnosticScopeLabel = nullptr;
    QLabel *m_diagnosticTitle = nullptr;
    QLabel *m_diagnosticDescription = nullptr;
    QLabel *m_diagnosticAvailability = nullptr;
    QLabel *m_diagnosticResultsTitle = nullptr;
    QPlainTextEdit *m_diagnosticResults = nullptr;
    QPushButton *m_runDiagnosticButton = nullptr;
    QPushButton *m_runAllDiagnosticsButton = nullptr;
    QPushButton *m_copyDiagnosticButton = nullptr;
    QPushButton *m_saveDiagnosticButton = nullptr;
    QLabel *m_targetConfigLabel = nullptr;
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
    // Btrfs inventory dedupe. m_snapshotScopeGeneration is bumped on every
    // target/scope change (see updateSnapshotControls()); a completed
    // automatic preload records the generation it served in
    // m_snapshotLoadedGeneration, so re-scheduling within the same scope is a
    // no-op while a scope change still gets exactly one load. The in-flight
    // request identity deduplicates re-entrant requests for the same
    // target+component. The generation starts at 1 so an unloaded scope can
    // never compare equal to the zero-initialized loaded generation.
    quint64 m_snapshotScopeGeneration = 1;
    quint64 m_snapshotLoadedGeneration = 0;
    bool m_snapshotInventoryInFlight = false;
    QString m_snapshotRequestScopeKey;
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
    QCheckBox *m_fullRepairExtlinux = nullptr;

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
    // Coalescing delay for automatic evidence regeneration. Production keeps
    // the 1200 ms default; the UI regression tests shorten it so the
    // no-second-run assertions do not have to wait out the full delay.
    int m_evidenceRefreshDelayMs = 1200;
    QString m_evidenceRefreshReason;
    bool m_evidenceRefreshPending = false;
    bool m_evidenceRefreshInProgress = false;
    bool m_diagnosticsRunInProgress = false;
    bool m_runAllDiagnosticsQueued = false;
    bool m_privilegedOperationActive = false;
    // Scope identity (the request's disk-path argument) of the request that
    // owns the gate above; empty when no request is active. A request that
    // finds the gate held is re-entrant by definition and must never block on
    // it; a request for a different disk is dropped as superseded instead.
    QString m_privilegedOperationScopeKey;
    // Title of the request that owns the gate, used in refusal log lines.
    QString m_privilegedOperationTitle;
    // Bounded safety net for a privileged request: after this many
    // milliseconds the wait is aborted, the unresponsive helper session is
    // closed and the request is reported as failed, so the UI can never stay
    // busy indefinitely. The default is deliberately generous (two hours) so
    // a legitimate long package/repair transaction is never aborted; the UI
    // regression tests shorten it.
    int m_privilegedRequestTimeoutMs = 2 * 60 * 60 * 1000;
    // Set when a Btrfs snapshot preload was requested while the gate was
    // held. The gate release re-runs it once for the then-current target; the
    // request itself is never queued on the gate.
    bool m_snapshotPreloadDeferred = false;
    // Set when a scope change invalidated diagnostics while the gate was
    // held. The gate release schedules exactly one coalesced regeneration for
    // the then-current scope instead of dropping the invalidation.
    bool m_scopeChangeRefreshPending = false;
    QString m_scopeChangeRefreshReason;
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
    // Test-only per-key diagnostic result overrides so the offscreen suite can
    // inject realistic helper output (including the shared capability
    // preamble) without a privileged session.
    QMap<QString, QString> m_uiTestDiagnosticResults;
};
