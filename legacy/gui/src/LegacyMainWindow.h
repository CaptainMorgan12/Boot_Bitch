// LegacyMainWindow - Qt3/Q3-widget frontend for the ported legacy helper.
//
// Layout mirrors the modern Boot Bitch Qt6 information hierarchy where Qt3
// allows it: Systems (read-only kernel inventory + helper-confirmed target
// facts + LUKS unlock), Diagnostics (13 `Repair tool` capability lines with an
// all/available/unavailable filter, greyed probe reasons and raw evidence),
// Repair (privilege-elevation state, only the legacy-supported commands gated
// by the cached capability lines, plus the greyed modern-only feature list),
// Logs (streamed helper output with an all/errors filter) and About/TUI.
//
// The window never reads block devices; all device/target confirmation and
// every privileged action goes through the helper (QProcess). Gating mirrors
// MainWindow::repairToolAvailable: fail closed, missing/unparseable evidence
// keeps the action disabled. The LUKS passphrase travels only over the
// helper's standard input and is never logged or placed in command arguments.
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

class QComboBox;
class QGroupBox;
class QLabel;
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
    // then verifies the new controls (unlock/elevation/filters) and the layout
    // at 1024x768 (no clipped group titles, buttons, list columns or combos).
    // Used for the Etch VM validation (no screenshots).
    void startSmokeTest();

signals:
    void smokeFinished(bool ok, const QString &summary);

private slots:
    void scanDevices();
    void scopeChanged(int index);
    void deviceSelectionChanged();
    void targetEdited();
    void runDiagnostics();
    void runAction();
    void runUnlock();
    void recheckElevation();
    void diagnosticsFilterChanged();
    void logFilterChanged();
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
    QWidget *buildLogTab();
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
    void updateStatus();
    void updateActionStates();
    void updateElevationLabel();
    void updateCapabilityView();
    void updateFactView(const ParsedTranscript &parsed);
    void mergeHelperDevices(const ParsedTranscript &parsed);
    void refreshRootCombo();
    void autoDetectHostTarget();
    void startCommand(const QStringList &args, bool diagnostic,
                      const QString &label, bool unlock = false);
    void reportChangeStatuses(const ParsedTranscript &parsed);
    void handleUnlockFinished(bool ok, const std::string &transcript,
                              const QString &device);
    bool verifySmokeControls(QString *problems);
    bool verifyLayout(QString *problems, int *checked);

    QTabWidget *m_tabs;
    QComboBox *m_scopeCombo;
    QComboBox *m_diskCombo;
    QComboBox *m_rootCombo;
    QComboBox *m_unlockCombo;
    QComboBox *m_diagFilterCombo;
    QComboBox *m_logFilterCombo;
    QListView *m_deviceList;
    QListView *m_factList;
    QListView *m_capabilityList;
    QListView *m_unsupportedList;
    QTextEdit *m_rawView;
    QTextEdit *m_logView;
    QLabel *m_unlockStatusView;
    QLabel *m_scopeHint;
    QLabel *m_gateHint;
    QLabel *m_elevationLabel;
    QPushButton *m_scanButton;
    QPushButton *m_diagnosticsButton;
    QPushButton *m_cancelButton;
    QPushButton *m_unlockButton;
    QPushButton *m_elevateButton;
    QMap<QString, QPushButton *> m_actionButtons;
    std::vector<QPushButton *> m_buttons;
    std::vector<QGroupBox *> m_groupBoxes;

    HelperRunner *m_runner;
    CapabilityModel m_model;
    QMap<QString, DeviceRow> m_rows;

    QString m_transcript;
    QString m_logDirectory;
    QString m_logPath;
    QString m_lastHelperDescription;
    QString m_unlockDevice;
    QString m_unlockStatus;
    QStringList m_logLines;

    bool m_updatingCombos;
    bool m_running;
    bool m_pendingDiagnostic;
    bool m_pendingUnlock;
    bool m_unlockRetry;
    QString m_pendingIdentity;
    QString m_pendingLabel;

    bool m_smokeMode;
    int m_smokeStep;
    bool m_smokeDiagnoseOk;
    bool m_smokeValidateOk;
};

} // namespace legacy

#endif // LEGACY_MAIN_WINDOW_H
