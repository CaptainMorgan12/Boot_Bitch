#pragma once

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

// One block device as parsed from `lsblk --json`, including its children and
// the classification the GUI uses for filtering, status text and protection.
// The scan only reads kernel-reported metadata; nothing is mounted or modified.
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

// Runs the trusted lsblk binary and returns the classified top-level physical
// disks, including children. Error details go to errorMessage; diagnosticLog
// receives the stable scan progress lines when the caller supplies one.
class SystemScanner final : public QObject
{
    Q_OBJECT

public:
    explicit SystemScanner(QObject *parent = nullptr);

    QList<DeviceNode> scan(QString *errorMessage = nullptr,
                           QStringList *diagnosticLog = nullptr) const;

    // Formats a byte count as a compact "TiB/GiB/MiB/KiB/B" string.
    static QString humanSize(quint64 bytes);
    // Joins a node's mount points for display, or an em dash when unmounted.
    static QString mountPointsText(const DeviceNode &node);

    // Fills a node's missing filesystem metadata from the world-readable udev
    // database (ID_FS_* / ID_PART_ENTRY_* properties). lsblk only reports these
    // values itself when it can probe the block device or is linked against
    // libudev; the Alpine util-linux build has neither, so an unprivileged
    // scan would otherwise report every unmounted filesystem as unknown.
    // deviceNumber is the kernel MAJ:MIN string; udevDataDir overrides the
    // default /run/udev/data (read-only test seam).
    static void applyUdevFilesystemEvidence(DeviceNode &node, const QString &deviceNumber,
                                            const QString &udevDataDir = QString());

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
