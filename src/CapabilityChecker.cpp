#include "CapabilityChecker.h"

#include <QCoreApplication>
#include <QFile>
#include <QMap>
#include <QRegularExpression>
#include <QStringList>

namespace {
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

// A package name that cannot actually be installed (empty or a placeholder)
// must never be offered as an installable "Suggested package" in the Boot
// capabilities table.
bool isPlaceholderPackage(const QString &package)
{
    return package.isEmpty()
        || package == QStringLiteral("Unknown")
        || package == QStringLiteral("Distribution-specific")
        || package == QStringLiteral("Not available on Alpine")
        || package == QStringLiteral("Check distribution documentation");
}
} // namespace

// Human-readable title for a repair-tool capability key, mirroring the Repair
// page's tool list. Unknown keys fall back to the key itself.
static QString repairToolFeatureTitle(const QString &key)
{
    if (key == QStringLiteral("validate")) return QCoreApplication::translate("CapabilityChecker", "Validate environment");
    if (key == QStringLiteral("filesystem")) return QCoreApplication::translate("CapabilityChecker", "File system repair");
    if (key == QStringLiteral("dpkg")) return QCoreApplication::translate("CapabilityChecker", "Complete package configuration");
    if (key == QStringLiteral("fixbroken")) return QCoreApplication::translate("CapabilityChecker", "Repair broken dependencies");
    if (key == QStringLiteral("aptupdate")) return QCoreApplication::translate("CapabilityChecker", "Refresh package metadata");
    if (key == QStringLiteral("upgrade")) return QCoreApplication::translate("CapabilityChecker", "Upgrade installed packages");
    if (key == QStringLiteral("dkms")) return QCoreApplication::translate("CapabilityChecker", "DKMS rebuild");
    if (key == QStringLiteral("display")) return QCoreApplication::translate("CapabilityChecker", "Graphical login / display manager");
    if (key == QStringLiteral("gpu")) return QCoreApplication::translate("CapabilityChecker", "GPU driver repair");
    if (key == QStringLiteral("initramfs")) return QCoreApplication::translate("CapabilityChecker", "Initramfs rebuild");
    if (key == QStringLiteral("efi")) return QCoreApplication::translate("CapabilityChecker", "EFI / UKI bootloader");
    if (key == QStringLiteral("grub")) return QCoreApplication::translate("CapabilityChecker", "GRUB configuration");
    if (key == QStringLiteral("extlinux")) return QCoreApplication::translate("CapabilityChecker", "extlinux configuration");
    if (key == QStringLiteral("bootstack")) return QCoreApplication::translate("CapabilityChecker", "Boot stack reconciliation");
    if (key == QStringLiteral("kernels")) return QCoreApplication::translate("CapabilityChecker", "Kernel repair");
    return key;
}

// Human-readable title for a recovery-host prerequisite command.
static QString hostCommandFeatureTitle(const QString &command)
{
    if (command == QStringLiteral("lsblk")) return QCoreApplication::translate("CapabilityChecker", "Device discovery");
    if (command == QStringLiteral("blkid")) return QCoreApplication::translate("CapabilityChecker", "Filesystem identification");
    if (command == QStringLiteral("findmnt")) return QCoreApplication::translate("CapabilityChecker", "Mount inspection");
    if (command == QStringLiteral("cryptsetup")) return QCoreApplication::translate("CapabilityChecker", "LUKS support");
    if (command == QStringLiteral("btrfs")) return QCoreApplication::translate("CapabilityChecker", "Btrfs support");
    if (command == QStringLiteral("rsync")) return QCoreApplication::translate("CapabilityChecker", "Bidirectional file copy");
    if (command == QStringLiteral("chroot")) return QCoreApplication::translate("CapabilityChecker", "Chroot repair");
    if (command == QStringLiteral("systemctl")) return QCoreApplication::translate("CapabilityChecker", "Offline systemd repair");
    if (command == QStringLiteral("efibootmgr")) return QCoreApplication::translate("CapabilityChecker", "UEFI NVRAM inspection");
    if (command == QStringLiteral("objcopy")) return QCoreApplication::translate("CapabilityChecker", "UKI verification");
    if (command == QStringLiteral("lvs")) return QCoreApplication::translate("CapabilityChecker", "LVM inspection");
    if (command == QStringLiteral("mdadm")) return QCoreApplication::translate("CapabilityChecker", "Software RAID");
    return command;
}

// The stable repair-tool key order emitted by the helper's
// diagnostic_repair_capabilities(). Iterating this list (instead of the parse
// map's key order) keeps the table deterministic and mirrors the helper.
static const QStringList &repairToolKeyOrder()
{
    static const QStringList keys = {
        QStringLiteral("validate"), QStringLiteral("filesystem"), QStringLiteral("dpkg"),
        QStringLiteral("fixbroken"), QStringLiteral("aptupdate"), QStringLiteral("upgrade"),
        QStringLiteral("dkms"), QStringLiteral("display"), QStringLiteral("gpu"),
        QStringLiteral("initramfs"), QStringLiteral("efi"), QStringLiteral("grub"),
        QStringLiteral("extlinux"), QStringLiteral("bootstack"), QStringLiteral("kernels")
    };
    return keys;
}

// Parses the cached repair-target capability preamble into one Capability per
// `Repair tool <key>` line. Availability is read verbatim from the cache; the
// `Repair capability evidence <key>` line fills the tool/notes, and the backend
// section's `Module build system:` line names the module-build row's backend
// (dkms/akmods/akms). No filesystem probing happens here.
QList<Capability> CapabilityChecker::capabilitiesFromRepairEvidence(const QString &capabilitiesPreamble,
                                                                    const QString &backendSection)
{
    QString moduleBuild = QStringLiteral("none");
    for (const QString &rawLine : backendSection.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        if (line.startsWith(QStringLiteral("Module build system:"), Qt::CaseInsensitive)) {
            moduleBuild = line.section(QLatin1Char(':'), 1).trimmed();
            break;
        }
    }

    QMap<QString, QString> states;
    QMap<QString, QString> evidence;
    for (const QString &rawLine : capabilitiesPreamble.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        const QString toolPrefix = QStringLiteral("Repair tool ");
        if (line.startsWith(toolPrefix)) {
            const QString rest = line.mid(toolPrefix.size());
            const int separator = rest.indexOf(QLatin1Char(':'));
            if (separator <= 0) {
                continue;
            }
            states.insert(rest.left(separator).trimmed(), rest.mid(separator + 1).trimmed());
            continue;
        }
        const QString evidencePrefix = QStringLiteral("Repair capability evidence ");
        if (line.startsWith(evidencePrefix)) {
            const QString rest = line.mid(evidencePrefix.size());
            const int separator = rest.indexOf(QLatin1Char(':'));
            if (separator <= 0) {
                continue;
            }
            evidence.insert(rest.left(separator).trimmed(), rest.mid(separator + 1).trimmed());
        }
    }

    QList<Capability> capabilities;
    for (const QString &key : repairToolKeyOrder()) {
        if (!states.contains(key)) {
            continue;
        }
        const QString state = states.value(key);
        Capability capability;
        capability.feature = repairToolFeatureTitle(key);
        capability.scope = QCoreApplication::translate("CapabilityChecker", "Repair target");
        capability.targetScope = true;
        capability.optional = true;
        capability.available = state == QStringLiteral("available");
        const bool unavailable = !capability.available;
        if (key == QStringLiteral("dkms")) {
            // The module-build row reflects the target's detected build system
            // (dkms/akmods/akms) and names the matching package, never the host
            // distribution's package.
            capability.command = moduleBuild;
            capability.packageName = moduleBuildSystemPackageForBackend(moduleBuild);
            capability.installable = true;
        } else {
            capability.command = evidence.value(key);
            capability.packageName.clear();
            capability.installable = false;
        }
        // Notes: the machine-readable reason for an unavailable tool, otherwise
        // the evidence line (only when the Command column does not already show
        // it — the dkms row's Command is the short backend token).
        if (unavailable) {
            capability.note = state.startsWith(QStringLiteral("unavailable|"))
                ? state.mid(QStringLiteral("unavailable|").size())
                : state;
        } else if (key == QStringLiteral("dkms")) {
            capability.note = evidence.value(key);
        }
        capabilities.append(capability);
    }
    return capabilities;
}

// Parses the cached running-host `host-capabilities` diagnostic section into one
// Capability per `Host capability:` line.
QList<Capability> CapabilityChecker::capabilitiesFromHostEvidence(const QString &hostCapabilitiesSection)
{
    static const QRegularExpression lineRe(
        QStringLiteral("^Host capability: (\\S+) package=(\\S+) status=(available|missing) scope=(\\S+) note=(.*)$"));
    QList<Capability> capabilities;
    for (const QString &rawLine : hostCapabilitiesSection.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        const QRegularExpressionMatch match = lineRe.match(line);
        if (!match.hasMatch()) {
            continue;
        }
        Capability capability;
        capability.command = match.captured(1);
        capability.feature = hostCommandFeatureTitle(capability.command);
        capability.packageName = match.captured(2);
        capability.available = match.captured(3) == QStringLiteral("available");
        capability.scope = QCoreApplication::translate("CapabilityChecker", "Host");
        capability.note = match.captured(5);
        capability.targetScope = false;
        capability.optional = true;
        capability.installable = !isPlaceholderPackage(capability.packageName);
        capabilities.append(capability);
    }
    return capabilities;
}

QString CapabilityChecker::moduleBuildSystemPackageForBackend(const QString &backend)
{
    if (backend == QStringLiteral("akmods")) {
        return QStringLiteral("akmods");
    }
    if (backend == QStringLiteral("akms")) {
        return QStringLiteral("akms");
    }
    return QStringLiteral("dkms");
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

