#pragma once

#include "CapabilityChecker.h"
#include "SystemScanner.h"

#include <QByteArray>
#include <QDateTime>
#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QMap>

class QAction;
class QBoxLayout;
class QCheckBox;
class QCloseEvent;
class QComboBox;
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
class QTreeWidget;
class QTreeWidgetItem;
class QToolButton;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *buildSystemsPage();
    QWidget *buildDiagnosticsPage();
    QWidget *buildRepairPage();
    QWidget *buildSnapshotsPage();
    QWidget *buildChrootShellPage();
    QWidget *buildFileCopyPage();
    QWidget *buildLogsPage();
    QWidget *buildSettingsPage();

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

    void updateSnapshotControls();
    void clearSnapshotResults();
    void scheduleSnapshotPreload();
    void loadSnapshots();
    void inspectSelectedSnapshot();
    void rollbackSelectedSnapshot();
    void runChrootShellCommand();
    void clearChrootShellOutput();
    void saveLogAs();
    void appendLog(const QString &message, const QString &level = QStringLiteral("INFO"));
    void appendDiagnosticLog(const QString &key, const QString &title,
                             const QString &scope, const QString &result,
                             bool succeeded);
    void showAboutDialog();
    void showUsageHelp();

    void updateDiagnosticDetails();
    void runSelectedDiagnostic();
    void runAllDiagnostics();
    void copyDiagnosticResults();
    void saveDiagnosticResults();
    void editTargetConfig();
    QString diagnosticResultForKey(const QString &key) const;
    QString runTargetDiagnosticHelper(const QString &key, bool *succeeded = nullptr,
                                      bool showProgressDialog = false);
    void cacheTargetDiagnosticBundle(const QString &bundle);
    QString currentTargetDiagnosticCacheIdentity() const;
    void clearTargetDiagnosticCache();
    bool targetDiagnosticEvidenceReady(QString *reason = nullptr) const;

    bool ensurePrivilegedSession(QString *errorMessage = nullptr);
    QString runPrivilegedRequest(const QString &title, const QStringList &arguments,
                                 QByteArray secret = QByteArray(), bool *succeeded = nullptr,
                                 bool showProgressDialog = true);
    void closePrivilegedSession();

    void runSelectedRepairTool();
    void runFullRepair();
    QString repairHelperPath() const;
    QStringList selectedRepairStages() const;
    bool repairTargetReady(QString *reason = nullptr) const;
    bool repairEvidenceReadyForTool(const QString &toolKey, QString *reason = nullptr) const;
    bool confirmRepairAction(const QString &title, const QStringList &operations);
    void runRepairHelper(const QString &title, const QStringList &arguments);

    void updateResponsiveLayout();
    void updateFullRepairSummary();
    void updateRepairToolDetails();
    void setLogWrapEnabled(bool enabled);
    void refreshLogView();
    void autoSizeDeviceColumns();
    void applyDeviceColumnDefaults();
    void updateDeviceTreeHeight();
    void normalizeButtonSizing();
    bool devicePassesTopLevelFilters(const DeviceNode &device) const;

    static QString combinedModel(const DeviceNode &node);
    static QString deviceKind(const DeviceNode &node);
    static QIcon iconForDevice(const DeviceNode &node);

    SystemScanner *m_scanner = nullptr;
    QSettings *m_settings = nullptr;
    QStringList m_actionLogEntries;

    QTabWidget *m_tabs = nullptr;
    QToolButton *m_tabScrollLeftButton = nullptr;
    QToolButton *m_tabScrollRightButton = nullptr;
    QAction *m_wrapLogsAction = nullptr;
    QAction *m_lockAuthorizationAction = nullptr;

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

    QLabel *m_hostSystemLabel = nullptr;
    QLabel *m_hostStorageLabel = nullptr;
    QLabel *m_hostMountsLabel = nullptr;
    QPushButton *m_hostDetailsButton = nullptr;

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
    QSplitter *m_repairSplitter = nullptr;
    QTreeWidget *m_repairToolTree = nullptr;
    QLabel *m_repairToolTitle = nullptr;
    QLabel *m_repairToolDescription = nullptr;
    QLabel *m_repairToolPlanStatus = nullptr;
    QPushButton *m_repairToolButton = nullptr;
    QLabel *m_fullRepairCountLabel = nullptr;
    QLabel *m_fullRepairReadinessLabel = nullptr;
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
    QTableWidget *m_capabilityTable = nullptr;
    QLabel *m_distributionLabel = nullptr;
    QLabel *m_packageManagerLabel = nullptr;
    QLabel *m_authBuildLabel = nullptr;

    QCheckBox *m_showNonLinux = nullptr;
    QCheckBox *m_showRemovable = nullptr;
    QCheckBox *m_showEncrypted = nullptr;
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
    QString m_previewTargetPath;
    QString m_previewTargetComponentPath;
    QString m_snapshotResultIdentity;

    QMap<QString, QString> m_targetDiagnosticCache;
    QMap<QString, QDateTime> m_targetDiagnosticTimes;
    QMap<QString, QString> m_hostDiagnosticCache;
    QMap<QString, QDateTime> m_hostDiagnosticTimes;
    QString m_targetDiagnosticCacheIdentity;

    QProcess *m_privilegedSession = nullptr;
    quint64 m_privilegedRequestCounter = 0;
    bool m_privilegedSessionReady = false;
};
