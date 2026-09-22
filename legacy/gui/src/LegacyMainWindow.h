// LegacyMainWindow - Qt3/Q3-widget frontend for the ported legacy helper.
//
// Layout mirrors the modern Boot Bitch Qt6 information hierarchy where Qt3
// allows it: Systems (read-only kernel inventory, helper-confirmed target
// facts, the selected-drive details panel, LUKS unlock with its status),
// Diagnostics (13 `Repair tool` capability lines with an
// all/available/unavailable filter, per-diagnostic runs, a read-only target
// configuration viewer, greyed probe reasons and raw evidence), Repair
// (privilege-elevation state, only the legacy-supported commands gated by the
// cached capability lines, including the guarded GRUB-legacy regeneration,
// plus the greyed modern-only feature list driven by the helper's
// `Legacy feature` probes), Chroot Shell and File Copy (deliberately greyed
// with their probe reasons), Logs (streamed helper output with an all/errors
// filter, a search box and the per-session log list), Settings (read-only
// configuration plus greyed modern-only options) and About.
//
// The window never reads block devices; all device/target confirmation and
// every privileged action goes through the helper (QProcess). Gating mirrors
// MainWindow::repairToolAvailable: fail closed, missing/unparseable evidence
// keeps the action disabled, and diagnostics/repairs additionally require an
// explicitly committed repair target or active Host Maintenance exactly like
// the modern Systems page. A plain interactive `sudo` is authorized through a
// modal hidden-input dialog that feeds `sudo -S -v` over a pipe; the LUKS
// passphrase travels only over the helper's standard input. Neither secret is
// ever logged or placed in command arguments.
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
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
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
    // then verifies the new controls (details/unlock/session/search/gating) and
    // the layout at 1024x768 (no clipped group titles, buttons, list columns or
    // combos). Used for the Etch VM validation (no screenshots).
    void startSmokeTest();

signals:
    void smokeFinished(bool ok, const QString &summary);

private slots:
    void scanDevices();
    void scopeChanged(int index);
    void deviceSelectionChanged();
    void targetEdited();
    void setRepairTarget();
    void toggleHostMaintenance();
    void runDiagnostics();
    void runSelectedDiagnostic();
    void diagnosticSelectionChanged();
    void viewConfigFile();
    void copyResults();
    void runAction();
    void runUnlock();
    void recheckElevation();
    void diagnosticsFilterChanged();
    void logFilterChanged();
    void logSearchChanged();
    void sessionLogSelectionChanged();
    void refreshSessionLogs();
    void toggleLogWrap(bool enabled);
    void helperLine(const QString &line);
    void helperFinished(bool ok, int exitCode);
    void cancelRun();
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
    void registerGroupBox(QGroupBox *box);
    void appendLog(const QString &line);
    void appendToLogFile(const QString &line);
    void ensureLogFile();
    QString identity() const;
    bool hostScope() const;
    QString selectedDisk() const;
    QString selectedRoot() const;
    bool selectionComplete() const;
    bool targetCommitted() const;
    bool hostMaintenanceActive() const;
    bool diagnosticsScopeReady() const;
    QString scopeReadyReason() const;
    QString runningHostDisk() const;
    void updateStatus();
    void updateActionStates();
    void updateElevationLabel();
    void updateCapabilityView();
    void updateLegacyFeatureView();
    void updateFeatureTab(QGroupBox *group, QLabel *label, const char *feature,
                          const QString &title);
    void legacyFeatureDisplay(const char *feature, QString *state,
                              QString *reason) const;
    void updateFactView(const ParsedTranscript &parsed);
    void updateDriveDetails();
    void updateUnlockStatus();
    void mergeHelperDevices(const ParsedTranscript &parsed);
    void refreshRootCombo();
    void refreshSessionLogList();
    void refreshLogView();
    bool logLinePassesFilter(const QString &line) const;
    QStringList sessionLogFiles() const;
    QString visibleMapperForDisk(const QString &disk) const;
    void autoDetectHostTarget();
    bool promptElevationPassword();
    void startCommand(const QStringList &args, bool diagnostic,
                      const QString &label, bool unlock = false,
                      bool config = false);
    void reportChangeStatuses(const ParsedTranscript &parsed);
    void handleUnlockFinished(bool ok, const std::string &transcript,
                              const QString &device, const QString &disk);
    bool verifySmokeControls(QString *problems);
    bool verifyLayout(QString *problems, int *checked);

    QTabWidget *m_tabs;
    QComboBox *m_scopeCombo;
    QComboBox *m_diskCombo;
    QComboBox *m_rootCombo;
    QComboBox *m_unlockCombo;
    QComboBox *m_diagFilterCombo;
    QComboBox *m_logFilterCombo;
    QComboBox *m_configCombo;
    QListView *m_deviceList;
    QListView *m_detailList;
    QListView *m_capabilityList;
    QListView *m_diagnosticList;
    QListView *m_unsupportedList;
    QListView *m_sessionLogList;
    QTextEdit *m_rawView;
    QTextEdit *m_configView;
    QTextEdit *m_logView;
    QTextEdit *m_unlockStatusView;
    QLineEdit *m_logSearchEdit;
    QCheckBox *m_logWrapCheck;
    QLabel *m_scopeHint;
    QLabel *m_gateHint;
    QLabel *m_elevationLabel;
    QLabel *m_targetSummary;
    QLabel *m_chrootReasonLabel;
    QLabel *m_fileCopyReasonLabel;
    QLabel *m_settingsHelperLabel;
    QLabel *m_settingsElevationLabel;
    QLabel *m_settingsLogDirLabel;
    QLabel *m_settingsSessionLabel;
    QLabel *m_settingsVersionLabel;
    QPushButton *m_scanButton;
    QPushButton *m_diagnosticsButton;
    QPushButton *m_runDiagnosticButton;
    QPushButton *m_copyResultsButton;
    QPushButton *m_configButton;
    QPushButton *m_cancelButton;
    QPushButton *m_unlockButton;
    QPushButton *m_elevateButton;
    QPushButton *m_setTargetButton;
    QPushButton *m_hostMaintenanceButton;
    QGroupBox *m_chrootGroup;
    QGroupBox *m_fileCopyGroup;
    QMap<QString, QPushButton *> m_actionButtons;
    std::vector<QPushButton *> m_buttons;
    std::vector<QGroupBox *> m_groupBoxes;

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
    QString m_committedDisk;
    QString m_committedRoot;
    QString m_priorLogPath;
    QStringList m_logLines;
    QStringList m_priorLogLines;

    bool m_updatingCombos;
    bool m_running;
    bool m_pendingDiagnostic;
    bool m_pendingUnlock;
    bool m_pendingConfig;
    bool m_unlockRetry;
    bool m_targetCommitted;
    bool m_hostMaintenance;
    bool m_viewingPriorLog;
    QString m_pendingIdentity;
    QString m_pendingLabel;

    bool m_smokeMode;
    int m_smokeStep;
    bool m_smokeDiagnoseOk;
    bool m_smokeValidateOk;
};

} // namespace legacy

#endif // LEGACY_MAIN_WINDOW_H
