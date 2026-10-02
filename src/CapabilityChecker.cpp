#include "CapabilityChecker.h"

#include <QCoreApplication>
#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringList>

namespace {
// A8-05: resolve a command through the fixed system directories first and the
// environment PATH last. A GUI launched from a desktop session can inherit a
// PATH that differs from the user's shell PATH; the system directories are
// tried first so a lookalike executable in a user- or attacker-controlled
// directory can never win ahead of the real system binary.
QString resolveExecutablePortable(const QString &command)
{
    static const QStringList systemDirectories = {
        QStringLiteral("/usr/local/sbin"),
        QStringLiteral("/usr/local/bin"),
        QStringLiteral("/usr/sbin"),
        QStringLiteral("/usr/bin"),
        QStringLiteral("/sbin"),
        QStringLiteral("/bin")
    };
    for (const QString &directory : systemDirectories) {
        const QString path = QStandardPaths::findExecutable(command, {directory});
        if (!path.isEmpty()) {
            return path;
        }
    }
    // PATH fallback: last, and only for commands the system directories lack.
    return QStandardPaths::findExecutable(command);
}

// A8-04: strips C0/C1 control characters and the Unicode bidi control
// characters (U+202A-U+202E, U+2066-U+2069) from display strings, so a hostile
// os-release can never inject terminal control sequences or bidi reordering
// into the interface.
QString stripDisplayControls(const QString &text)
{
    QString cleaned;
    cleaned.reserve(text.size());
    for (const QChar character : text) {
        const ushort code = character.unicode();
        if (character.category() == QChar::Other_Control
            || (code >= 0x202A && code <= 0x202E)
            || (code >= 0x2066 && code <= 0x2069)) {
            continue;
        }
        cleaned.append(character);
    }
    return cleaned;
}

// A8-01: bounded line reading for os-release. Lines longer than 64 KiB are
// skipped conservatively — their remainder is drained so the tail of a huge
// line can never be misread as a separate entry — and at most 256 lines are
// inspected; every line is decoded only after it was read completely.
class BoundedLineReader
{
public:
    explicit BoundedLineReader(QFile &file)
        : m_file(file)
    {
    }

    bool hasMore() const
    {
        return m_linesRead < kMaxLines && !m_file.atEnd();
    }

    // Reads the next complete line (newline stripped) into line. Returns
    // false at EOF, after the line budget, or when the line exceeded the
    // length bound (the oversized line is consumed and skipped).
    bool readLine(QByteArray *line)
    {
        if (m_linesRead >= kMaxLines) {
            return false;
        }
        bool draining = false;
        for (;;) {
            QByteArray raw = m_file.readLine(kMaxLineBytes + 2);
            if (raw.isEmpty()) {
                return false;
            }
            const bool complete = raw.endsWith('\n') || m_file.atEnd();
            const qsizetype content = raw.size() - (raw.endsWith('\n') ? 1 : 0);
            if (draining) {
                if (complete) {
                    return false;
                }
                continue;
            }
            if (content > kMaxLineBytes) {
                ++m_linesRead;
                if (complete) {
                    return false;
                }
                draining = true;
                continue;
            }
            if (raw.endsWith('\n')) {
                raw.chop(1);
            }
            if (raw.endsWith('\r')) {
                raw.chop(1);
            }
            *line = raw;
            ++m_linesRead;
            return true;
        }
    }

private:
    static constexpr qsizetype kMaxLineBytes = 64 * 1024;
    static constexpr int kMaxLines = 256;
    QFile &m_file;
    int m_linesRead = 0;
};

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
    if (!file.open(QIODevice::ReadOnly)) {
        return values;
    }

    BoundedLineReader reader(file);
    while (reader.hasMore()) {
        QByteArray rawLine;
        if (!reader.readLine(&rawLine)) {
            continue; // oversized line skipped
        }
        const QString line = QString::fromUtf8(rawLine).trimmed();
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
        const QString key = line.left(separator);
        // A8-04: PRETTY_NAME is a display string and is sanitized at parse
        // time; the machine-read keys (ID, ID_LIKE, ...) are matched verbatim
        // and stay untouched.
        values.insert(key, key == QStringLiteral("PRETTY_NAME") ? stripDisplayControls(value) : value);
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

// A8-05: public wrapper over the file-local resolver so the UI regression
// tests can pin the fixed-system-directories-first order directly.
QString CapabilityChecker::findExecutablePortable(const QString &command)
{
    return resolveExecutablePortable(command);
}

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
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Device discovery"), "lsblk", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required for block-device inventory"), false},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Filesystem identification"), "blkid", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Used to identify filesystem metadata"), false},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Mount inspection"), "findmnt", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Used to understand active mounts"), false},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "LUKS support"), "cryptsetup", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required to unlock encrypted targets"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Btrfs support"), "btrfs", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required for Btrfs inspection and snapshot rollback"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Bidirectional file copy"), "rsync", QT_TRANSLATE_NOOP("CapabilityChecker", "Host/Repair"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required for verified Host-to-Repair and Repair-to-Host transfer"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Chroot repair"), "chroot", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required for target-side repair commands"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Offline systemd repair"), "systemctl", QT_TRANSLATE_NOOP("CapabilityChecker", "Host/Repair"), QT_TRANSLATE_NOOP("CapabilityChecker", "Used to restore graphical.target and the configured display manager without starting the target GUI"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "UEFI NVRAM inspection"), "efibootmgr", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "UKI verification"), "objcopy", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Used to verify the kernel embedded in a rebuilt unified kernel image"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "GRUB EFI repair"), "grub-install", QT_TRANSLATE_NOOP("CapabilityChecker", "Target/Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required only for conventional GRUB-based EFI systems"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "GRUB configuration"), "update-grub", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Debian-family GRUB helper"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "GRUB configuration"), "grub-mkconfig", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Portable GRUB configuration generator used by Arch and other non-Debian systems"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Initramfs rebuild"), "update-initramfs", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Debian-family initramfs helper"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Initramfs rebuild"), "mkinitcpio", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Arch-family initramfs generator"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Initramfs rebuild"), "dracut", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Alternative initramfs generator used by Arch and other distributions"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Initramfs verification"), "lsinitcpio", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Read-only verification for mkinitcpio images"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Initramfs verification"), "lsinitrd", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Read-only verification for dracut images"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "systemd-boot inspection"), "bootctl", QT_TRANSLATE_NOOP("CapabilityChecker", "Host/Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Read-only inspection of systemd-boot and generic UKI layouts"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Arch package manager"), "pacman", QT_TRANSLATE_NOOP("CapabilityChecker", "Host/Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Arch-family package database and transaction tool"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "DKMS rebuild"), "dkms", QT_TRANSLATE_NOOP("CapabilityChecker", "Target"), QT_TRANSLATE_NOOP("CapabilityChecker", "Required only when target uses DKMS modules"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "LVM inspection"), "lvs", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Optional LVM storage-stack support"), true},
        {QT_TRANSLATE_NOOP("CapabilityChecker", "Software RAID"), "mdadm", QT_TRANSLATE_NOOP("CapabilityChecker", "Host"), QT_TRANSLATE_NOOP("CapabilityChecker", "Optional Linux MD RAID support"), true}
    };

    const QString distro = distributionId();
    QList<Capability> capabilities;

    for (const Requirement &requirement : requirements) {
        Capability capability;
        capability.feature = QCoreApplication::translate("CapabilityChecker", requirement.feature);
        capability.command = QString::fromLatin1(requirement.command);
        capability.scope = QCoreApplication::translate("CapabilityChecker", requirement.scope);
        capability.note = QCoreApplication::translate("CapabilityChecker", requirement.note);
        capability.optional = requirement.optional;
        capability.executablePath = resolveExecutablePortable(capability.command);
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
