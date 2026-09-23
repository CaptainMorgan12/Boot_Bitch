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
// Copy (greyed with their probe reasons), Logs (streamed helper output with an
// all/errors filter, a search box and the per-session log list), Settings
// (read-only configuration plus greyed modern-only options) and About.
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

#include <vector>

#include "DeviceInventory.h"
#include "EvidenceParser.h"

class QCheckBox;
class QCloseEvent;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
class QResizeEvent;
class QTabWidget;
class QTextEdit;

namespace legacy {

class HelperRunner;

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
    void runAction();
    void runChrootShell();
    void clearChrootOutput();
    void runUnlock();
    void recheckElevation();
    void logFilterChanged();
    void logSearchChanged();
    void sessionLogSelectionChanged();
    void refreshSessionLogs();
    void toggleLogWrap(bool enabled);
    void helperLine(const QString &line);
    void helperFinished(bool ok, int exitCode);
    void saveLog();
    void clearLog();
    void showAbout();
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
    QWidget *buildAboutTab();

    QPushButton *makeButton(const QString &text, QWidget *parent);
    // Re-applies the font-metric minimum width after a runtime button-text
    // change (for example "Host Maintenance" -> "Exit Host Maintenance"), so
    // the layout guard can never clip the new label.
    void updateButtonText(QPushButton *button, const QString &text);
    QLabel *makeSectionTitle(const QString &text, QWidget *parent);
    void addListViewColumn(QListView *list, const QString &title, int width);
    void registerGroupBox(QGroupBox *box);
    void appendLog(const QString &line);
    void appendToLogFile(const QString &line);
    void ensureLogFile();
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
    void updateStatus();
    void updateActionStates();
    void updateScopeLabel();
    void updateElevationLabel();
    void updateConfigView();
    void openConfigEditor(const QString &content, const QString &key,
                          const QString &path);
    void updateLegacyFeatureView();
    void updateFeatureTab(QGroupBox *group, QLabel *label, const char *feature,
                          const QString &title);
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
    bool logLinePassesFilter(const QString &line) const;
    QStringList sessionLogFiles() const;
    QString visibleMapperForDisk(const QString &disk) const;
    void autoDetectHostTarget();
    bool ensureAdministratorSession(const QString &context);
    bool administratorSessionActive() const;
    bool sessionStillCurrent();
    void startCommand(const QStringList &args, bool diagnostic,
                      const QString &label, bool unlock = false,
                      bool config = false, bool shell = false);
    void reportChangeStatuses(const ParsedTranscript &parsed);
    void handleUnlockFinished(bool ok, const std::string &transcript,
                              const QString &device, const QString &disk);
    bool verifySmokeControls(QString *problems);
    bool verifyLayout(QString *problems, int *checked);

    QTabWidget *m_tabs;
    QComboBox *m_logFilterCombo;
    QComboBox *m_configCombo;
    QListView *m_deviceList;
    QListView *m_detailList;
    QListView *m_diagnosticList;
    QListView *m_unsupportedList;
    QListView *m_sessionLogList;
    QTextEdit *m_rawView;
    QTextEdit *m_logView;
    QTextEdit *m_unlockStatusView;
    QLineEdit *m_logSearchEdit;
    QLineEdit *m_shellCommandEdit;
    QTextEdit *m_shellOutput;
    QCheckBox *m_logWrapCheck;
    QLabel *m_headerTitle;
    QLabel *m_headerSubtitle;
    QLabel *m_headerBadge;
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
    QLabel *m_logsHeading;
    QLabel *m_settingsHeading;
    QLabel *m_gateHint;
    QLabel *m_elevationLabel;
    QLabel *m_targetSummary;
    QLabel *m_configLabel;
    QLabel *m_configReasonLabel;
    QLabel *m_chrootReasonLabel;
    QLabel *m_fileCopyReasonLabel;
    QLabel *m_authStatusLabel;
    QLabel *m_repairAuthStatusLabel;
    QLabel *m_settingsHelperLabel;
    QLabel *m_settingsElevationLabel;
    QLabel *m_settingsLogDirLabel;
    QLabel *m_settingsSessionLabel;
    QLabel *m_settingsVersionLabel;
    QPushButton *m_scanButton;
    QPushButton *m_diagnosticsButton;
    QPushButton *m_runDiagnosticButton;
    QPushButton *m_copyResultsButton;
    QPushButton *m_saveResultsButton;
    QPushButton *m_configButton;
    QPushButton *m_unlockButton;
    QPushButton *m_elevateButton;
    QPushButton *m_setTargetButton;
    QPushButton *m_hostMaintenanceButton;
    QPushButton *m_authorizeButton;
    QPushButton *m_repairAuthorizeButton;
    QPushButton *m_shellRunButton;
    QPushButton *m_shellClearButton;
    QGroupBox *m_chrootGroup;
    QGroupBox *m_fileCopyGroup;
    QWidget *m_chrootTab;
    QMap<QString, QPushButton *> m_actionButtons;
    std::vector<QPushButton *> m_buttons;
    std::vector<QGroupBox *> m_groupBoxes;
    std::vector<QLabel *> m_sectionTitles;

    HelperRunner *m_runner;
    CapabilityModel m_model;
    QMap<QString, DeviceRow> m_rows;
    QMap<QString, QString> m_factMap;
    QMap<QString, QString> m_unlockStatusCache;

    QString m_transcript;
    QString m_logDirectory;
    QString m_logPath;
    QString m_lastHelperDescription;
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
    QStringList m_logLines;
    QStringList m_priorLogLines;

    bool m_updatingSelection;
    bool m_running;
    bool m_pendingDiagnostic;
    bool m_pendingUnlock;
    bool m_pendingConfig;
    bool m_pendingShell;
    bool m_unlockRetry;
    bool m_targetCommitted;
    bool m_hostMaintenance;
    bool m_viewingPriorLog;
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
