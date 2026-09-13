#include "CapabilityChecker.h"

#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTextStream>

namespace {
QString findExecutablePortable(const QString &command)
{
    QString path = QStandardPaths::findExecutable(command);
    if (!path.isEmpty()) {
        return path;
    }

    static const QStringList fallbackDirectories = {
        QStringLiteral("/usr/local/sbin"),
        QStringLiteral("/usr/local/bin"),
        QStringLiteral("/usr/sbin"),
        QStringLiteral("/usr/bin"),
        QStringLiteral("/sbin"),
        QStringLiteral("/bin")
    };

    for (const QString &directory : fallbackDirectories) {
        path = QStandardPaths::findExecutable(command, {directory});
        if (!path.isEmpty()) {
            return path;
        }
    }

    return QString();
}

QMap<QString, QString> readOsRelease()
{
    QMap<QString, QString> values;
    QFile file(QStringLiteral("/etc/os-release"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return values;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
            continue;
        }
        const qsizetype separator = line.indexOf(QLatin1Char('='));
        if (separator <= 0) {
            continue;
        }
        QString value = line.mid(separator + 1).trimmed();
        if (value.size() >= 2 && value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"'))) {
            value = value.mid(1, value.size() - 2);
        }
        values.insert(line.left(separator), value);
    }
    return values;
}
} // namespace

QList<Capability> CapabilityChecker::scanHost()
{
    struct Requirement {
        const char *feature;
        const char *command;
        const char *scope;
        const char *note;
        bool optional;
    };

    static const Requirement requirements[] = {
        {"Device discovery", "lsblk", "Host", "Required for block-device inventory", false},
        {"Filesystem identification", "blkid", "Host", "Used to identify filesystem metadata", false},
        {"Mount inspection", "findmnt", "Host", "Used to understand active mounts", false},
        {"LUKS support", "cryptsetup", "Host", "Required to unlock encrypted targets", true},
        {"Btrfs support", "btrfs", "Host", "Required for Btrfs inspection and snapshot rollback", true},
        {"Bidirectional file copy", "rsync", "Host/Repair", "Required for verified Host-to-Repair and Repair-to-Host transfer", true},
        {"Chroot repair", "chroot", "Host", "Required for target-side repair commands", true},
        {"Offline systemd repair", "systemctl", "Host/Repair", "Used to restore graphical.target and the configured display manager without starting the target GUI", true},
        {"UEFI NVRAM inspection", "efibootmgr", "Host", "Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available", true},
        {"UKI verification", "objcopy", "Host", "Used to verify the kernel embedded in a rebuilt unified kernel image", true},
        {"GRUB EFI repair", "grub-install", "Target/Host", "Required only for conventional GRUB-based EFI systems", true},
        {"GRUB configuration", "update-grub", "Target", "Debian-family GRUB helper", true},
        {"Initramfs rebuild", "update-initramfs", "Target", "Debian-family initramfs helper", true},
        {"DKMS rebuild", "dkms", "Target", "Required only when target uses DKMS modules", true},
        {"LVM inspection", "lvs", "Host", "Optional LVM storage-stack support", true},
        {"Software RAID", "mdadm", "Host", "Optional Linux MD RAID support", true}
    };

    const QString distro = distributionId();
    QList<Capability> capabilities;

    for (const Requirement &requirement : requirements) {
        Capability capability;
        capability.feature = QString::fromLatin1(requirement.feature);
        capability.command = QString::fromLatin1(requirement.command);
        capability.scope = QString::fromLatin1(requirement.scope);
        capability.note = QString::fromLatin1(requirement.note);
        capability.optional = requirement.optional;
        capability.executablePath = findExecutablePortable(capability.command);
        capability.available = !capability.executablePath.isEmpty();
        capability.packageName = packageForCommand(capability.command, distro);
        capabilities.append(capability);
    }

    return capabilities;
}

QString CapabilityChecker::distributionLabel()
{
    const QMap<QString, QString> values = readOsRelease();
    const QString prettyName = values.value(QStringLiteral("PRETTY_NAME"));
    return prettyName.isEmpty() ? QStringLiteral("Unknown Linux distribution") : prettyName;
}

QString CapabilityChecker::packageManagerLabel()
{
    const QString id = distributionId();
    if (id == QStringLiteral("debian") || id == QStringLiteral("ubuntu") || id == QStringLiteral("tuxedo") || id == QStringLiteral("linuxmint")) {
        return QStringLiteral("APT / dpkg");
    }
    if (id == QStringLiteral("fedora") || id == QStringLiteral("rhel") || id == QStringLiteral("rocky") || id == QStringLiteral("almalinux")) {
        return QStringLiteral("DNF / RPM");
    }
    if (id == QStringLiteral("arch") || id == QStringLiteral("manjaro")) {
        return QStringLiteral("pacman");
    }
    if (id == QStringLiteral("opensuse") || id == QStringLiteral("opensuse-tumbleweed") || id == QStringLiteral("suse")) {
        return QStringLiteral("zypper / RPM");
    }
    return QStringLiteral("Unknown / unsupported automatic mapping");
}

QString CapabilityChecker::distributionId()
{
    const QMap<QString, QString> values = readOsRelease();
    QString id = values.value(QStringLiteral("ID")).toLower();
    if (!id.isEmpty()) {
        return id;
    }

    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.isEmpty() ? QStringLiteral("unknown") : like.first();
}

QString CapabilityChecker::packageForCommand(const QString &command, const QString &distributionId)
{
    const bool debian = distributionId == QStringLiteral("debian")
        || distributionId == QStringLiteral("ubuntu")
        || distributionId == QStringLiteral("tuxedo")
        || distributionId == QStringLiteral("linuxmint");

    if (debian) {
        static const QMap<QString, QString> packages = {
            {QStringLiteral("lsblk"), QStringLiteral("util-linux")},
            {QStringLiteral("blkid"), QStringLiteral("util-linux")},
            {QStringLiteral("findmnt"), QStringLiteral("util-linux")},
            {QStringLiteral("cryptsetup"), QStringLiteral("cryptsetup")},
            {QStringLiteral("btrfs"), QStringLiteral("btrfs-progs")},
            {QStringLiteral("rsync"), QStringLiteral("rsync")},
            {QStringLiteral("chroot"), QStringLiteral("coreutils")},
            {QStringLiteral("systemctl"), QStringLiteral("systemd")},
            {QStringLiteral("efibootmgr"), QStringLiteral("efibootmgr")},
            {QStringLiteral("objcopy"), QStringLiteral("binutils")},
            {QStringLiteral("grub-install"), QStringLiteral("grub2-common")},
            {QStringLiteral("update-grub"), QStringLiteral("grub-common")},
            {QStringLiteral("update-initramfs"), QStringLiteral("initramfs-tools")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Unknown"));
    }

    if (distributionId == QStringLiteral("fedora") || distributionId == QStringLiteral("rhel")
        || distributionId == QStringLiteral("rocky") || distributionId == QStringLiteral("almalinux")) {
        static const QMap<QString, QString> packages = {
            {QStringLiteral("lsblk"), QStringLiteral("util-linux")},
            {QStringLiteral("blkid"), QStringLiteral("util-linux")},
            {QStringLiteral("findmnt"), QStringLiteral("util-linux")},
            {QStringLiteral("cryptsetup"), QStringLiteral("cryptsetup")},
            {QStringLiteral("btrfs"), QStringLiteral("btrfs-progs")},
            {QStringLiteral("rsync"), QStringLiteral("rsync")},
            {QStringLiteral("chroot"), QStringLiteral("coreutils")},
            {QStringLiteral("systemctl"), QStringLiteral("systemd")},
            {QStringLiteral("efibootmgr"), QStringLiteral("efibootmgr")},
            {QStringLiteral("objcopy"), QStringLiteral("binutils")},
            {QStringLiteral("grub-install"), QStringLiteral("grub2-tools")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Distribution-specific"));
    }

    if (distributionId == QStringLiteral("arch") || distributionId == QStringLiteral("manjaro")) {
        static const QMap<QString, QString> packages = {
            {QStringLiteral("lsblk"), QStringLiteral("util-linux")},
            {QStringLiteral("blkid"), QStringLiteral("util-linux")},
            {QStringLiteral("findmnt"), QStringLiteral("util-linux")},
            {QStringLiteral("cryptsetup"), QStringLiteral("cryptsetup")},
            {QStringLiteral("btrfs"), QStringLiteral("btrfs-progs")},
            {QStringLiteral("rsync"), QStringLiteral("rsync")},
            {QStringLiteral("chroot"), QStringLiteral("coreutils")},
            {QStringLiteral("systemctl"), QStringLiteral("systemd")},
            {QStringLiteral("efibootmgr"), QStringLiteral("efibootmgr")},
            {QStringLiteral("objcopy"), QStringLiteral("binutils")},
            {QStringLiteral("grub-install"), QStringLiteral("grub")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Distribution-specific"));
    }

    if (distributionId.startsWith(QStringLiteral("opensuse")) || distributionId == QStringLiteral("suse")) {
        static const QMap<QString, QString> packages = {
            {QStringLiteral("lsblk"), QStringLiteral("util-linux")},
            {QStringLiteral("blkid"), QStringLiteral("util-linux")},
            {QStringLiteral("findmnt"), QStringLiteral("util-linux")},
            {QStringLiteral("cryptsetup"), QStringLiteral("cryptsetup")},
            {QStringLiteral("btrfs"), QStringLiteral("btrfsprogs")},
            {QStringLiteral("rsync"), QStringLiteral("rsync")},
            {QStringLiteral("chroot"), QStringLiteral("coreutils")},
            {QStringLiteral("systemctl"), QStringLiteral("systemd")},
            {QStringLiteral("efibootmgr"), QStringLiteral("efibootmgr")},
            {QStringLiteral("objcopy"), QStringLiteral("binutils")},
            {QStringLiteral("grub-install"), QStringLiteral("grub2")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Distribution-specific"));
    }

    return QStringLiteral("Check distribution documentation");
}
