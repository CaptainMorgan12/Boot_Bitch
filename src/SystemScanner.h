#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QJsonObject>

struct DeviceNode
{
    QString name;
    QString kernelName;
    QString path;
    QString type;
    quint64 sizeBytes = 0;
    QString fileSystem;
    QString fileSystemVersion;
    QString label;
    QString partLabel;
    QString uuid;
    QString partUuid;
    QString model;
    QString serial;
    QString vendor;
    QString transport;
    QString parentKernelName;
    QStringList mountPoints;
    QString status;
    QString osName;
    bool removable = false;
    bool readOnly = false;
    bool protectedDevice = false;
    bool encrypted = false;
    bool installedLinux = false;
    bool linuxCapableFileSystem = false;
    QList<DeviceNode> children;
};

class SystemScanner final : public QObject
{
    Q_OBJECT

public:
    explicit SystemScanner(QObject *parent = nullptr);

    QList<DeviceNode> scan(QString *errorMessage = nullptr,
                           QStringList *diagnosticLog = nullptr) const;

    static QString humanSize(quint64 bytes);
    static QString mountPointsText(const DeviceNode &node);

private:
    DeviceNode parseNode(const QJsonObject &object) const;
    void classifyTree(DeviceNode &node) const;
    void applyProtection(DeviceNode &node, bool protectedTree) const;
    bool treeBacksRunningSystem(const DeviceNode &node) const;
    bool treeContainsInstalledLinux(const DeviceNode &node) const;
    bool treeContainsEncryptedVolume(const DeviceNode &node) const;

    static bool isProtectedMountPoint(const QString &mountPoint);
    static bool isLinuxCapableFileSystem(const QString &fileSystem);
    static QString detectMountedLinuxName(const QStringList &mountPoints);
};
