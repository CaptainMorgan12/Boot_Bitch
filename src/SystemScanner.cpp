#include "SystemScanner.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>

namespace {
QString jsonString(const QJsonObject &object, const char *key)
{
    const QJsonValue value = object.value(QLatin1String(key));
    return value.isString() ? value.toString() : QString();
}

bool jsonBool(const QJsonObject &object, const char *key)
{
    const QJsonValue value = object.value(QLatin1String(key));
    if (value.isBool()) {
        return value.toBool();
    }
    if (value.isDouble()) {
        return value.toInt() != 0;
    }
    if (value.isString()) {
        const QString text = value.toString().trimmed().toLower();
        return text == QStringLiteral("1") || text == QStringLiteral("true") || text == QStringLiteral("yes");
    }
    return false;
}

QStringList jsonMountPoints(const QJsonObject &object)
{
    QStringList result;
    const QJsonValue value = object.value(QStringLiteral("mountpoints"));

    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &entry : array) {
            if (entry.isString() && !entry.toString().isEmpty()) {
                result.append(entry.toString());
            }
        }
    } else if (value.isString() && !value.toString().isEmpty()) {
        result.append(value.toString());
    }

    return result;
}

// Reads PRETTY_NAME from the os-release file of a mounted root, so a repair
// target is named the same way the running host is.
QString prettyNameFromOsRelease(const QString &root)
{
    QString path;
    if (root == QStringLiteral("/")) {
        path = QStringLiteral("/etc/os-release");
    } else {
        path = root + QStringLiteral("/etc/os-release");
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        if (!line.startsWith(QStringLiteral("PRETTY_NAME="))) {
            continue;
        }

        QString value = line.mid(QStringLiteral("PRETTY_NAME=").size()).trimmed();
        if (value.size() >= 2 && value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"'))) {
            value = value.mid(1, value.size() - 2);
        }
        return value;
    }

    return QString();
}
} // namespace

SystemScanner::SystemScanner(QObject *parent)
    : QObject(parent)
{
}

QList<DeviceNode> SystemScanner::scan(QString *errorMessage, QStringList *diagnosticLog) const
{
    const QStringList columns = {
        QStringLiteral("NAME"), QStringLiteral("KNAME"), QStringLiteral("PATH"), QStringLiteral("TYPE"),
        QStringLiteral("SIZE"), QStringLiteral("FSTYPE"), QStringLiteral("FSVER"), QStringLiteral("LABEL"),
        QStringLiteral("PARTLABEL"), QStringLiteral("UUID"), QStringLiteral("PARTUUID"),
        QStringLiteral("MOUNTPOINTS"), QStringLiteral("MODEL"), QStringLiteral("SERIAL"),
        QStringLiteral("VENDOR"), QStringLiteral("TRAN"), QStringLiteral("RM"), QStringLiteral("RO"),
        QStringLiteral("PKNAME")
    };

    QProcess process;
    QString lsblkPath;
    for (const QString &candidate : {QStringLiteral("/usr/bin/lsblk"), QStringLiteral("/bin/lsblk")}) {
        const QFileInfo info(candidate);
        if (info.isFile() && info.isExecutable()) {
            lsblkPath = candidate;
            break;
        }
    }
    if (lsblkPath.isEmpty()) {
        if (errorMessage) *errorMessage = QStringLiteral("Unable to locate the trusted lsblk executable.");
        return {};
    }
    process.setProgram(lsblkPath);
    process.setArguments({
        QStringLiteral("--json"),
        QStringLiteral("--bytes"),
        QStringLiteral("--tree"),
        QStringLiteral("--output"),
        columns.join(QLatin1Char(','))
    });
    process.start();

    if (!process.waitForStarted(3000)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Unable to start lsblk. Install util-linux and try again.");
        }
        return {};
    }

    if (!process.waitForFinished(8000)) {
        process.kill();
        process.waitForFinished();
        if (errorMessage) {
            *errorMessage = QStringLiteral("lsblk did not finish within 8 seconds.");
        }
        return {};
    }

    const QByteArray standardError = process.readAllStandardError();
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("lsblk failed: %1").arg(QString::fromLocal8Bit(standardError).trimmed());
        }
        return {};
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(process.readAllStandardOutput(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Unable to parse lsblk JSON output: %1").arg(parseError.errorString());
        }
        return {};
    }

    QList<DeviceNode> devices;
    const QJsonArray blockDevices = document.object().value(QStringLiteral("blockdevices")).toArray();
    for (const QJsonValue &value : blockDevices) {
        if (!value.isObject()) {
            continue;
        }

        DeviceNode node = parseNode(value.toObject());
        const bool protectedTree = treeBacksRunningSystem(node);
        applyProtection(node, protectedTree);
        classifyTree(node);

        if (node.type == QStringLiteral("disk")) {
            if (protectedTree) {
                node.status = QStringLiteral("Protected current system");
            } else if (treeContainsInstalledLinux(node)) {
                node.status = QStringLiteral("Installed Linux detected");
            } else if (treeContainsEncryptedVolume(node)) {
                node.status = QStringLiteral("Encrypted volume — inspection required");
            } else {
                node.status = QStringLiteral("Available for inspection");
            }
        }

        devices.append(node);
    }

    if (diagnosticLog) {
        diagnosticLog->append(QStringLiteral("lsblk scan completed successfully."));
        diagnosticLog->append(QStringLiteral("Discovered %1 top-level physical disk(s).").arg(devices.size()));
    }

    return devices;
}

// Recursively converts one lsblk JSON object (and its children) into a node.
DeviceNode SystemScanner::parseNode(const QJsonObject &object) const
{
    DeviceNode node;
    node.name = jsonString(object, "name");
    node.kernelName = jsonString(object, "kname");
    node.path = jsonString(object, "path");
    node.type = jsonString(object, "type");
    node.fileSystem = jsonString(object, "fstype");
    node.fileSystemVersion = jsonString(object, "fsver");
    node.label = jsonString(object, "label");
    node.partLabel = jsonString(object, "partlabel");
    node.uuid = jsonString(object, "uuid");
    node.partUuid = jsonString(object, "partuuid");
    node.model = jsonString(object, "model").trimmed();
    node.serial = jsonString(object, "serial").trimmed();
    node.vendor = jsonString(object, "vendor").trimmed();
    node.transport = jsonString(object, "tran").trimmed();
    node.parentKernelName = jsonString(object, "pkname");
    node.mountPoints = jsonMountPoints(object);
    node.removable = jsonBool(object, "rm");
    node.readOnly = jsonBool(object, "ro");

    const QJsonValue sizeValue = object.value(QStringLiteral("size"));
    if (sizeValue.isDouble()) {
        node.sizeBytes = static_cast<quint64>(sizeValue.toDouble());
    } else if (sizeValue.isString()) {
        node.sizeBytes = sizeValue.toString().toULongLong();
    }

    const QJsonArray children = object.value(QStringLiteral("children")).toArray();
    for (const QJsonValue &child : children) {
        if (child.isObject()) {
            node.children.append(parseNode(child.toObject()));
        }
    }

    return node;
}

void SystemScanner::classifyTree(DeviceNode &node) const
{
    node.encrypted = node.fileSystem.compare(QStringLiteral("crypto_LUKS"), Qt::CaseInsensitive) == 0;
    node.linuxCapableFileSystem = isLinuxCapableFileSystem(node.fileSystem);
    node.osName = detectMountedLinuxName(node.mountPoints);
    node.installedLinux = !node.osName.isEmpty();

    // Classify children first so an encrypted container can report that a
    // mapped Linux filesystem is already visible instead of continuing to say
    // "unlock required" after another recovery tool (or Boot Bitch itself)
    // has opened it.
    for (DeviceNode &child : node.children) {
        classifyTree(child);
    }
    auto hasVisibleLinux = [&](auto &&self, const DeviceNode &candidate) -> bool {
        if (candidate.installedLinux || candidate.linuxCapableFileSystem) {
            return true;
        }
        for (const DeviceNode &child : candidate.children) {
            if (self(self, child)) {
                return true;
            }
        }
        return false;
    };
    bool unlockedEncrypted = false;
    if (node.encrypted) {
        for (const DeviceNode &child : node.children) {
            if (hasVisibleLinux(hasVisibleLinux, child)) {
                unlockedEncrypted = true;
                break;
            }
        }
    }

    if (node.protectedDevice) {
        node.status = QStringLiteral("Protected current system");
    } else if (node.installedLinux) {
        node.status = QStringLiteral("Installed Linux: %1").arg(node.osName);
    } else if (node.encrypted && unlockedEncrypted) {
        node.status = QStringLiteral("LUKS unlocked — mapped Linux filesystem visible");
    } else if (node.encrypted) {
        node.status = QStringLiteral("LUKS encrypted — unlock required");
    } else if (node.fileSystem == QStringLiteral("swap")) {
        node.status = QStringLiteral("Swap");
    } else if (node.fileSystem == QStringLiteral("iso9660")) {
        node.status = QStringLiteral("Live / installer media");
    } else if (node.fileSystem == QStringLiteral("vfat") || node.fileSystem == QStringLiteral("fat") || node.fileSystem == QStringLiteral("fat32")) {
        node.status = QStringLiteral("FAT / EFI-compatible volume");
    } else if (node.fileSystem == QStringLiteral("ntfs") || node.fileSystem == QStringLiteral("exfat")) {
        node.status = QStringLiteral("Non-Linux data volume");
    } else if (node.linuxCapableFileSystem && node.mountPoints.isEmpty()) {
        node.status = QStringLiteral("Linux-capable filesystem — inspection required");
    } else if (node.linuxCapableFileSystem) {
        node.status = QStringLiteral("Linux-capable filesystem");
    } else if (node.type == QStringLiteral("part") && node.fileSystem.isEmpty()) {
        node.status = QStringLiteral("Unknown / unformatted partition");
    } else if (node.type != QStringLiteral("disk")) {
        node.status = node.fileSystem.isEmpty() ? QStringLiteral("Unknown") : QStringLiteral("Filesystem: %1").arg(node.fileSystem);
    }
}

// Marks every node of a tree as protected once any node backs the running
// system, so children cannot be offered as repair targets.
void SystemScanner::applyProtection(DeviceNode &node, bool protectedTree) const
{
    node.protectedDevice = protectedTree;
    for (DeviceNode &child : node.children) {
        applyProtection(child, protectedTree);
    }
}

// True when any node in the tree is mounted at a protected mount point.
bool SystemScanner::treeBacksRunningSystem(const DeviceNode &node) const
{
    for (const QString &mountPoint : node.mountPoints) {
        if (isProtectedMountPoint(mountPoint)) {
            return true;
        }
    }

    for (const DeviceNode &child : node.children) {
        if (treeBacksRunningSystem(child)) {
            return true;
        }
    }

    return false;
}

// True when any node in the tree is a detected Linux installation.
bool SystemScanner::treeContainsInstalledLinux(const DeviceNode &node) const
{
    if (node.installedLinux || !detectMountedLinuxName(node.mountPoints).isEmpty()) {
        return true;
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsInstalledLinux(child)) {
            return true;
        }
    }
    return false;
}

// True when any node in the tree is a LUKS-encrypted volume.
bool SystemScanner::treeContainsEncryptedVolume(const DeviceNode &node) const
{
    if (node.encrypted || node.fileSystem.compare(QStringLiteral("crypto_LUKS"), Qt::CaseInsensitive) == 0) {
        return true;
    }
    for (const DeviceNode &child : node.children) {
        if (treeContainsEncryptedVolume(child)) {
            return true;
        }
    }
    return false;
}

bool SystemScanner::isProtectedMountPoint(const QString &mountPoint)
{
    return mountPoint == QStringLiteral("/")
        || mountPoint == QStringLiteral("/boot")
        || mountPoint == QStringLiteral("/boot/efi");
}

bool SystemScanner::isLinuxCapableFileSystem(const QString &fileSystem)
{
    static const QStringList types = {
        QStringLiteral("btrfs"), QStringLiteral("ext2"), QStringLiteral("ext3"),
        QStringLiteral("ext4"), QStringLiteral("xfs"), QStringLiteral("f2fs")
    };
    return types.contains(fileSystem, Qt::CaseInsensitive);
}

// Returns the first mount point's os-release PRETTY_NAME, if readable.
QString SystemScanner::detectMountedLinuxName(const QStringList &mountPoints)
{
    for (const QString &mountPoint : mountPoints) {
        if (mountPoint.isEmpty()) {
            continue;
        }
        const QString name = prettyNameFromOsRelease(mountPoint);
        if (!name.isEmpty()) {
            return name;
        }
    }
    return QString();
}

QString SystemScanner::humanSize(quint64 bytes)
{
    constexpr double kib = 1024.0;
    constexpr double mib = kib * 1024.0;
    constexpr double gib = mib * 1024.0;
    constexpr double tib = gib * 1024.0;

    if (bytes >= static_cast<quint64>(tib)) {
        return QStringLiteral("%1 TiB").arg(bytes / tib, 0, 'f', 2);
    }
    if (bytes >= static_cast<quint64>(gib)) {
        return QStringLiteral("%1 GiB").arg(bytes / gib, 0, 'f', 1);
    }
    if (bytes >= static_cast<quint64>(mib)) {
        return QStringLiteral("%1 MiB").arg(bytes / mib, 0, 'f', 1);
    }
    if (bytes >= static_cast<quint64>(kib)) {
        return QStringLiteral("%1 KiB").arg(bytes / kib, 0, 'f', 1);
    }
    return QStringLiteral("%1 B").arg(bytes);
}

QString SystemScanner::mountPointsText(const DeviceNode &node)
{
    return node.mountPoints.isEmpty() ? QStringLiteral("—") : node.mountPoints.join(QStringLiteral(", "));
}
