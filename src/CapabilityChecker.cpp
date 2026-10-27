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
// seam used by the UI regression test to simulate another distribution; it is
// compiled out of production builds (BOOT_REPAIR_UI_TEST) and normal launches
// always read /etc/os-release.
QString osReleasePath()
{
#ifdef BOOT_REPAIR_UI_TEST
    const QByteArray overridePath = qgetenv("BOOT_REPAIR_OS_RELEASE");
    if (!overridePath.isEmpty()) {
        return QString::fromLocal8Bit(overridePath);
    }
#endif
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

// The `reason:<key>` token from a capability state such as
// `unavailable|reason:akmods-no-build-tree|param:6.20.1-300.fc44.x86_64`.
// Empty when the state is `available`, carries legacy English prose, or has no
// machine-readable reason key (mirrors MainWindow::rawCapabilityReasonKey).
QString capabilityReasonKey(const QString &state)
{
    if (!state.startsWith(QStringLiteral("unavailable|"))) {
        return QString();
    }
    const QString detail = state.mid(QStringLiteral("unavailable|").size()).trimmed();
    if (!detail.startsWith(QStringLiteral("reason:"))) {
        return QString();
    }
    return detail.mid(QStringLiteral("reason:").size()).split(QLatin1Char('|')).first();
}

// Builds one storage-stack Capability row from the cached capability
// preamble's `Storage <marker>:` and `Storage <marker> evidence:` lines.
// These markers are scope-neutral (emitted for both the repair target and the
// running host), so both scopes surface the row. A `reason:missing-*` row names
// the providing package and is installable, mirroring the dkms
// missing->installable flow; availability is read verbatim from the cache
// (never a filesystem probe).
Capability storageCapability(const QString &preamble,
                             const QString &marker,
                             const QString &featureTitle,
                             const QString &fallbackCommand,
                             const QString &missingReasonKey,
                             const QString &suggestedPackage,
                             const QString &scopeLabel,
                             bool targetScope)
{
    QString state;
    QString evidenceText;
    const QString markerColon = marker + QLatin1Char(':');
    const QString markerEvidence = marker + QStringLiteral(" evidence:");
    for (const QString &rawLine : preamble.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        if (line.startsWith(markerEvidence)) {
            evidenceText = line.mid(markerEvidence.size()).trimmed();
        } else if (line.startsWith(markerColon)) {
            state = line.mid(markerColon.size()).trimmed();
        }
    }
    if (state.isEmpty()) {
        return {};
    }
    Capability capability;
    capability.feature = featureTitle;
    capability.scope = scopeLabel.isEmpty()
        ? QCoreApplication::translate("CapabilityChecker", "Repair target")
        : scopeLabel;
    capability.targetScope = targetScope;
    capability.optional = true;
    capability.available = state == QStringLiteral("available");
    capability.command = evidenceText.isEmpty() ? fallbackCommand : evidenceText;
    capability.packageName.clear();
    capability.installable = false;
    if (!capability.available && capabilityReasonKey(state) == missingReasonKey) {
        capability.packageName = suggestedPackage;
        capability.installable = true;
    }
    if (!capability.available) {
        capability.note = state.startsWith(QStringLiteral("unavailable|"))
            ? state.mid(QStringLiteral("unavailable|").size())
            : state;
    }
    return capability;
}

// Builds the snapshot-rollback Capability row from the cached capability
// preamble's scope-specific marker. The running host surfaces
// `Host snapshot rollback:`; the repair target surfaces the scope-neutral
// `Snapshot rollback:`. Both scopes surface the row whenever the marker is
// present — available OR unavailable — exactly like the storage-stack rows.
// A `reason:missing-*` line names the providing package (host `missing-snapper`
// -> snapper; `missing-btrfs-progs` -> btrfs-progs in both scopes) and is
// installable; availability is read verbatim from the cache, never a probe.
Capability snapshotRollbackCapability(const QString &preamble,
                                      const QString &scopeLabel,
                                      bool targetScope)
{
    const QString marker = targetScope
        ? QStringLiteral("Snapshot rollback")
        : QStringLiteral("Host snapshot rollback");
    const QString markerColon = marker + QLatin1Char(':');
    const QString markerEvidence = marker + QStringLiteral(" evidence:");
    QString state;
    QString evidenceText;
    for (const QString &rawLine : preamble.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        if (line.startsWith(markerEvidence)) {
            evidenceText = line.mid(markerEvidence.size()).trimmed();
        } else if (line.startsWith(markerColon)) {
            state = line.mid(markerColon.size()).trimmed();
        }
    }
    if (state.isEmpty()) {
        return {};
    }
    Capability capability;
    capability.feature = QCoreApplication::translate("CapabilityChecker", "Snapshot rollback");
    capability.scope = scopeLabel.isEmpty()
        ? QCoreApplication::translate("CapabilityChecker", "Repair target")
        : scopeLabel;
    capability.targetScope = targetScope;
    capability.optional = true;
    capability.available = state == QStringLiteral("available");
    capability.command = evidenceText.isEmpty()
        ? (targetScope ? QStringLiteral("btrfs") : QStringLiteral("snapper"))
        : evidenceText;
    capability.packageName.clear();
    capability.installable = false;
    if (!capability.available) {
        const QString reasonKey = capabilityReasonKey(state);
        if (reasonKey == QStringLiteral("missing-snapper") && !targetScope) {
            capability.packageName = QStringLiteral("snapper");
            capability.installable = true;
        } else if (reasonKey == QStringLiteral("missing-btrfs-progs")) {
            capability.packageName = QStringLiteral("btrfs-progs");
            capability.installable = true;
        }
        capability.note = state.startsWith(QStringLiteral("unavailable|"))
            ? state.mid(QStringLiteral("unavailable|").size())
            : state;
    }
    return capability;
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

// Feature title for the module-build (dkms) row, derived from the detected
// module build system rather than a hardcoded "DKMS" label. The token comes
// from the cached backend section's `Module build system:` line
// (dkms|akmods|akms); an absent or unrecognised token falls back to "DKMS
// rebuild".
static QString moduleBuildFeatureTitle(const QString &moduleBuild)
{
    if (moduleBuild == QStringLiteral("akms")) {
        return QCoreApplication::translate("CapabilityChecker", "AKMS rebuild");
    }
    if (moduleBuild == QStringLiteral("akmods")) {
        return QCoreApplication::translate("CapabilityChecker", "AKMODS rebuild");
    }
    return QCoreApplication::translate("CapabilityChecker", "DKMS rebuild");
}

// The stable repair-tool key order emitted by the helper's
// diagnostic_repair_capabilities(). Iterating this list (instead of the parse
// map's key order) keeps the table deterministic and mirrors the helper.
// Functional grouping (user-verified): package chain first, then the kernel
// group (kernels with the module-build and initramfs stages), then the
// display/gpu group, then the boot chain, then boot-stack reconciliation.
static const QStringList &repairToolKeyOrder()
{
    static const QStringList keys = {
        QStringLiteral("validate"), QStringLiteral("filesystem"), QStringLiteral("dpkg"),
        QStringLiteral("fixbroken"), QStringLiteral("aptupdate"), QStringLiteral("upgrade"),
        QStringLiteral("kernels"), QStringLiteral("dkms"), QStringLiteral("initramfs"),
        QStringLiteral("display"), QStringLiteral("gpu"), QStringLiteral("efi"),
        QStringLiteral("grub"), QStringLiteral("extlinux"), QStringLiteral("bootstack")
    };
    return keys;
}

// Parses the cached repair-target capability preamble into one Capability per
// `Repair tool <key>` line. Availability is read verbatim from the cache; the
// `Repair capability evidence <key>` line fills the tool/notes, and the backend
// section's `Module build system:` line names the module-build row's backend
// (dkms/akmods/akms). No filesystem probing happens here.
QList<Capability> CapabilityChecker::capabilitiesFromRepairEvidence(const QString &capabilitiesPreamble,
                                                                    const QString &backendSection,
                                                                    const QString &scopeLabel,
                                                                    bool targetScope)
{
    QString moduleBuild = QStringLiteral("none");
    QString distributionFamily = QStringLiteral("unknown");
    for (const QString &rawLine : backendSection.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        if (line.startsWith(QStringLiteral("Module build system:"), Qt::CaseInsensitive)) {
            moduleBuild = line.section(QLatin1Char(':'), 1).trimmed();
        } else if (line.startsWith(QStringLiteral("Distribution family:"), Qt::CaseInsensitive)) {
            distributionFamily = line.section(QLatin1Char(':'), 1).trimmed();
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
        // The module-build row's Feature label reflects the detected build
        // system (DKMS/AKMS/AKMODS), not a hardcoded "DKMS"; every other key
        // keeps its static title.
        capability.feature = (key == QStringLiteral("dkms"))
            ? moduleBuildFeatureTitle(moduleBuild)
            : repairToolFeatureTitle(key);
        capability.scope = scopeLabel.isEmpty()
            ? QCoreApplication::translate("CapabilityChecker", "Repair target")
            : scopeLabel;
        capability.targetScope = targetScope;
        capability.optional = true;
        capability.available = state == QStringLiteral("available");
        const bool unavailable = !capability.available;
        if (key == QStringLiteral("dkms")) {
            // The module-build row reflects the target's detected build system
            // (dkms/akmods/akms) and names the matching package, never the host
            // distribution's package.  The suggested package and installability
            // are derived from the row's reason key, not just the build system:
            // - a `missing-*` reason means the build system itself is absent, so
            //   the suggested package is the build-system package (akmods on
            //   Fedora/RHEL, akms on Alpine, dkms elsewhere) and it is
            //   installable ONLY when the helper proved (read-only, against the
            //   TARGET's own repositories) that the package is present; the
            //   helper reports that with `Repair capability evidence
            //   dkms-installable: available` and the row fails closed without
            //   it (no repo evidence -> no Install);
            // - a `*-no-build-tree` reason means the build system is present but
            //   the matching kernel headers/devel build tree is missing; the
            //   correct package is version-specific, so no one-click install is
            //   offered (the headers package is surfaced as guidance instead);
            // - an `available` row (or a legacy/unknown reason) has nothing to
            //   install.
            capability.command = moduleBuild;
            const QString reasonKey = capabilityReasonKey(state);
            // Only the exact missing-build-system reason keys are installable
            // (the helper probes the target repo before emitting the matching
            // `dkms-installable` side-channel); any other reason fails closed.
            const bool installableMissingReason = reasonKey == QStringLiteral("missing-akmods")
                || reasonKey == QStringLiteral("missing-dkms-alpine")
                || reasonKey == QStringLiteral("missing-dkms-arch")
                || reasonKey == QStringLiteral("missing-dkms");
            const bool packageInTargetRepo =
                evidence.value(QStringLiteral("dkms-installable")).trimmed() == QStringLiteral("available");
            if (capability.available || !installableMissingReason || !packageInTargetRepo) {
                capability.packageName.clear();
                capability.installable = false;
            } else {
                capability.packageName = moduleBuild == QStringLiteral("none")
                    ? moduleBuildSystemPackageForFamily(distributionFamily)
                    : moduleBuildSystemPackageForBackend(moduleBuild);
                capability.installable = true;
            }
        } else if (key == QStringLiteral("display")) {
            capability.command = evidence.value(key);
            capability.packageName.clear();
            capability.installable = false;
            // Round 39 (Ubuntu #4) + round 42 (all-distribution coverage): the
            // missing-display-manager Install suggestion.  The helper derives
            // the distribution-appropriate DM package from the DETECTED
            // desktop/family (probe, don't prefilter — never a hardcoded
            // per-distro list) and proves it present in the scope's own
            // repositories; the side-channel line
            // `Repair capability evidence display-installable: <pkg>|available`
            // is the single source of truth.  Fail closed without it.  The
            // installable reason set covers every family's missing-DM reason,
            // including Alpine's OpenRC `no-openrc-display-manager`.
            const QString reasonKey = capabilityReasonKey(state);
            const bool installableReason = reasonKey == QStringLiteral("no-display-manager")
                || reasonKey == QStringLiteral("no-display-manager-unit")
                || reasonKey == QStringLiteral("no-openrc-display-manager");
            if (!capability.available && installableReason) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("display-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0
                    && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available")) {
                    capability.packageName = sideChannel.left(separator).trimmed();
                    capability.installable = !capability.packageName.isEmpty();
                }
            }
        } else if (key == QStringLiteral("bootstack")) {
            capability.command = evidence.value(key);
            capability.packageName.clear();
            capability.installable = false;
            // Round 39 (Ubuntu #4): the missing initramfs/GRUB prerequisite
            // Install suggestion (the dkms/display pattern).  The helper
            // derives the package list from the DETECTED initramfs/bootloader
            // backends + firmware mode and proves every package present in the
            // scope's repositories (`bootstack-installable: <pkg> [<pkg>]|
            // available`).  Fail closed without it.
            const QString reasonKey = capabilityReasonKey(state);
            if (!capability.available
                && reasonKey == QStringLiteral("bootstack-needs-initramfs-grub")) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("bootstack-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0
                    && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available")) {
                    capability.packageName = sideChannel.left(separator).trimmed();
                    capability.installable = !capability.packageName.isEmpty();
                }
            }
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

    // Storage-stack markers.  LVM and software-RAID are scope-neutral: the
    // helper emits them for both the repair target and the running host, so
    // both scopes surface the same two rows with the caller's scope label and
    // targetScope routing.
    const Capability lvm = storageCapability(
        capabilitiesPreamble, QStringLiteral("Storage lvm inspection"),
        QCoreApplication::translate("CapabilityChecker", "LVM inspection"),
        QStringLiteral("lvs"), QStringLiteral("missing-lvm2"),
        QStringLiteral("lvm2"), scopeLabel, targetScope);
    if (!lvm.feature.isEmpty()) {
        capabilities.append(lvm);
    }
    const Capability raid = storageCapability(
        capabilitiesPreamble, QStringLiteral("Storage software raid"),
        QCoreApplication::translate("CapabilityChecker", "Software RAID"),
        QStringLiteral("mdadm"), QStringLiteral("missing-mdadm"),
        QStringLiteral("mdadm"), scopeLabel, targetScope);
    if (!raid.feature.isEmpty()) {
        capabilities.append(raid);
    }

    // Snapshot rollback.  The host scope surfaces `Host snapshot rollback:` and
    // the repair-target scope `Snapshot rollback:`; the row appears in BOTH
    // available and unavailable states (never filtered out when available) so
    // the user always sees whether snapshots are usable and why.
    const Capability snapshot = snapshotRollbackCapability(capabilitiesPreamble, scopeLabel, targetScope);
    if (!snapshot.feature.isEmpty()) {
        capabilities.append(snapshot);
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

// Names the native module-build system package for a target distribution
// family (fedora/rhel -> akmods, alpine -> akms, anything else -> dkms).  Used
// only when no build system binary was actually detected, so the Suggested
// package column names the target's native package instead of a generic dkms.
QString CapabilityChecker::moduleBuildSystemPackageForFamily(const QString &family)
{
    const QString normalized = family.trimmed().toLower();
    if (normalized == QStringLiteral("fedora") || normalized == QStringLiteral("rhel")) {
        return QStringLiteral("akmods");
    }
    if (normalized == QStringLiteral("alpine")) {
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

