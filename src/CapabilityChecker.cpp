#include "CapabilityChecker.h"

#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringList>
#include <QTextStream>

namespace {
// Resolves a command through the normal PATH search and then through the
// standard system directories, which are not always present in PATH when the
// application is started from an AppImage or a desktop launcher.
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

// The running host's os-release. BOOT_REPAIR_OS_RELEASE is a read-only test
// seam used by the UI regression test to simulate another distribution; normal
// launches always read /etc/os-release.
QString osReleasePath()
{
    const QByteArray overridePath = qgetenv("BOOT_REPAIR_OS_RELEASE");
    if (!overridePath.isEmpty()) {
        return QString::fromLocal8Bit(overridePath);
    }
    return QStringLiteral("/etc/os-release");
}

QMap<QString, QString> readOsRelease()
{
    QMap<QString, QString> values;
    QFile file(osReleasePath());
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

bool isArchLike(const QString &id, const QMap<QString, QString> &values)
{
    if (id == QStringLiteral("arch") || id == QStringLiteral("manjaro")
        || id == QStringLiteral("endeavouros") || id == QStringLiteral("garuda")
        || id == QStringLiteral("artix")) {
        return true;
    }
    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.contains(QStringLiteral("arch"));
}

bool isAlpineLike(const QString &id, const QMap<QString, QString> &values)
{
    if (id == QStringLiteral("alpine")) {
        return true;
    }
    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.contains(QStringLiteral("alpine"));
}

bool isDebianLike(const QString &id, const QMap<QString, QString> &values)
{
    if (id == QStringLiteral("debian") || id == QStringLiteral("ubuntu")
        || id == QStringLiteral("tuxedo") || id == QStringLiteral("linuxmint")
        || id == QStringLiteral("pop") || id == QStringLiteral("elementary")
        || id == QStringLiteral("zorin")) {
        return true;
    }
    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.contains(QStringLiteral("debian")) || like.contains(QStringLiteral("ubuntu"));
}

bool isFedoraLike(const QString &id, const QMap<QString, QString> &values)
{
    if (id == QStringLiteral("fedora") || id == QStringLiteral("rhel")
        || id == QStringLiteral("rocky") || id == QStringLiteral("almalinux")) {
        return true;
    }
    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.contains(QStringLiteral("fedora")) || like.contains(QStringLiteral("rhel"));
}

bool isSuseLike(const QString &id, const QMap<QString, QString> &values)
{
    if (id.startsWith(QStringLiteral("opensuse")) || id == QStringLiteral("suse")
        || id == QStringLiteral("sles")) {
        return true;
    }
    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.contains(QStringLiteral("suse"));
}
} // namespace

// Probes the fixed requirement table in declaration order and returns one
// Capability per entry, with the resolved executable path and the distribution
// package that provides the command.
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
        {"GRUB configuration", "grub-mkconfig", "Target", "Portable GRUB configuration generator used by Arch and other non-Debian systems", true},
        {"Initramfs rebuild", "update-initramfs", "Target", "Debian-family initramfs helper", true},
        {"Initramfs rebuild", "mkinitcpio", "Target", "Arch-family initramfs generator", true},
        {"Initramfs rebuild", "dracut", "Target", "Alternative initramfs generator used by Arch and other distributions", true},
        {"Initramfs verification", "lsinitcpio", "Target", "Read-only verification for mkinitcpio images", true},
        {"Initramfs verification", "lsinitrd", "Target", "Read-only verification for dracut images", true},
        {"systemd-boot inspection", "bootctl", "Host/Target", "Read-only inspection of systemd-boot and generic UKI layouts", true},
        {"Arch package manager", "pacman", "Host/Target", "Arch-family package database and transaction tool", true},
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

// PRETTY_NAME from /etc/os-release, with a stable fallback when the file is
// missing or unreadable.
QString CapabilityChecker::distributionLabel()
{
    const QMap<QString, QString> values = readOsRelease();
    const QString prettyName = values.value(QStringLiteral("PRETTY_NAME"));
    return prettyName.isEmpty() ? QStringLiteral("Unknown Linux distribution") : prettyName;
}

// Human-readable package-manager family for the running host.
QString CapabilityChecker::packageManagerLabel()
{
    const QMap<QString, QString> values = readOsRelease();
    const QString id = values.value(QStringLiteral("ID")).toLower();
    if (isDebianLike(id, values)) {
        return QStringLiteral("APT / dpkg");
    }
    if (isFedoraLike(id, values)) {
        return QStringLiteral("DNF / RPM");
    }
    if (isArchLike(id, values)) {
        return QStringLiteral("pacman");
    }
    if (isAlpineLike(id, values)) {
        return QStringLiteral("apk / OpenRC");
    }
    if (isSuseLike(id, values)) {
        return QStringLiteral("zypper / RPM");
    }
    return QStringLiteral("Unknown / unsupported automatic mapping");
}

// Normalized distribution-family key ("arch", "debian", "fedora", "alpine",
// "opensuse") used by packageForCommand(), or the raw ID/ID_LIKE value when
// unrecognized.
QString CapabilityChecker::distributionId()
{
    const QMap<QString, QString> values = readOsRelease();
    QString id = values.value(QStringLiteral("ID")).toLower();
    if (!id.isEmpty()) {
        if (isArchLike(id, values)) {
            return QStringLiteral("arch");
        }
        if (isDebianLike(id, values)) {
            return QStringLiteral("debian");
        }
        if (isFedoraLike(id, values)) {
            return QStringLiteral("fedora");
        }
        if (isAlpineLike(id, values)) {
            return QStringLiteral("alpine");
        }
        if (isSuseLike(id, values)) {
            return QStringLiteral("opensuse");
        }
        return id;
    }

    const QStringList like = values.value(QStringLiteral("ID_LIKE")).toLower().split(
        QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    return like.isEmpty() ? QStringLiteral("unknown") : like.first();
}

// Best-effort mapping from a probed command to the package that provides it in
// the given distribution family. Unknown commands return a family-appropriate
// placeholder instead of an empty string.
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
            {QStringLiteral("grub-mkconfig"), QStringLiteral("grub-common")},
            {QStringLiteral("update-initramfs"), QStringLiteral("initramfs-tools")},
            {QStringLiteral("lsinitramfs"), QStringLiteral("initramfs-tools-core")},
            {QStringLiteral("bootctl"), QStringLiteral("systemd")},
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
            {QStringLiteral("grub-mkconfig"), QStringLiteral("grub2-tools")},
            {QStringLiteral("dracut"), QStringLiteral("dracut")},
            {QStringLiteral("lsinitrd"), QStringLiteral("dracut")},
            {QStringLiteral("bootctl"), QStringLiteral("systemd")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Distribution-specific"));
    }

    if (distributionId == QStringLiteral("arch") || distributionId == QStringLiteral("manjaro")
        || distributionId == QStringLiteral("endeavouros") || distributionId == QStringLiteral("garuda")
        || distributionId == QStringLiteral("artix")) {
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
            {QStringLiteral("grub-mkconfig"), QStringLiteral("grub")},
            {QStringLiteral("mkinitcpio"), QStringLiteral("mkinitcpio")},
            {QStringLiteral("dracut"), QStringLiteral("dracut")},
            {QStringLiteral("lsinitcpio"), QStringLiteral("mkinitcpio")},
            {QStringLiteral("lsinitrd"), QStringLiteral("dracut")},
            {QStringLiteral("bootctl"), QStringLiteral("systemd")},
            {QStringLiteral("pacman"), QStringLiteral("pacman")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Distribution-specific"));
    }

    if (distributionId == QStringLiteral("alpine")) {
        static const QMap<QString, QString> packages = {
            {QStringLiteral("lsblk"), QStringLiteral("util-linux")},
            {QStringLiteral("blkid"), QStringLiteral("util-linux")},
            {QStringLiteral("findmnt"), QStringLiteral("util-linux")},
            {QStringLiteral("cryptsetup"), QStringLiteral("cryptsetup")},
            {QStringLiteral("btrfs"), QStringLiteral("btrfs-progs")},
            {QStringLiteral("rsync"), QStringLiteral("rsync")},
            {QStringLiteral("chroot"), QStringLiteral("coreutils")},
            {QStringLiteral("systemctl"), QStringLiteral("Not available on Alpine")},
            {QStringLiteral("efibootmgr"), QStringLiteral("efibootmgr")},
            {QStringLiteral("objcopy"), QStringLiteral("binutils")},
            {QStringLiteral("grub-install"), QStringLiteral("grub")},
            {QStringLiteral("update-grub"), QStringLiteral("Not available on Alpine")},
            {QStringLiteral("grub-mkconfig"), QStringLiteral("grub")},
            {QStringLiteral("update-initramfs"), QStringLiteral("Not available on Alpine")},
            {QStringLiteral("mkinitcpio"), QStringLiteral("mkinitfs")},
            {QStringLiteral("dracut"), QStringLiteral("dracut")},
            {QStringLiteral("lsinitcpio"), QStringLiteral("Not available on Alpine")},
            {QStringLiteral("lsinitrd"), QStringLiteral("dracut")},
            {QStringLiteral("bootctl"), QStringLiteral("Not available on Alpine")},
            {QStringLiteral("pacman"), QStringLiteral("Not available on Alpine")},
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
            {QStringLiteral("grub-mkconfig"), QStringLiteral("grub2")},
            {QStringLiteral("dracut"), QStringLiteral("dracut")},
            {QStringLiteral("lsinitrd"), QStringLiteral("dracut")},
            {QStringLiteral("bootctl"), QStringLiteral("systemd")},
            {QStringLiteral("dkms"), QStringLiteral("dkms")},
            {QStringLiteral("lvs"), QStringLiteral("lvm2")},
            {QStringLiteral("mdadm"), QStringLiteral("mdadm")}
        };
        return packages.value(command, QStringLiteral("Distribution-specific"));
    }

    return QStringLiteral("Check distribution documentation");
}
