#include "CapabilityChecker.h"

#include <QCoreApplication>
#include <QDir>
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

// Round 141 (ITEM O1/O2): the verdict of one `<key>-installable: <pkg>|…`
// side-channel line from the cached evidence map.  Returns 1 (proven
// available) only when the line names exactly the expected package and the
// verdict token is `available`; 0 for a proven `unavailable`; -1 when no
// matching side-channel exists (fail closed — no repo evidence, no Install).
int installableSideChannelVerdict(const QMap<QString, QString> &evidence,
                                  const QString &key,
                                  const QString &package)
{
    const QString channel = evidence.value(key + QStringLiteral("-installable")).trimmed();
    const int separator = channel.indexOf(QLatin1Char('|'));
    if (separator <= 0 || channel.left(separator).trimmed() != package) {
        return -1;
    }
    return channel.mid(separator + 1).trimmed() == QStringLiteral("available") ? 1 : 0;
}

// Builds one storage-stack Capability row from the cached capability
// preamble's `Storage <marker>:` and `Storage <marker> evidence:` lines.
// These markers are scope-neutral (emitted for both the repair target and the
// running host), so both scopes surface the row. A `reason:missing-*` row names
// the providing package; availability is read verbatim from the cache (never
// a filesystem probe).  Round 141 (ITEM O1/O2): the Install affordance is
// gated on the helper's proven `<key>-installable` side-channel (the helper
// derives the package, proves it present in the scope's own repositories and
// reports `available`; a reason alone never offers an Install — the
// dkms-installable fail-closed pattern).  repoUnavailable reports the honest
// not-available caveat state.  The note is the shared static description
// (round 105, ITEM 3) with the machine-readable reason folded in.
Capability storageCapability(const QString &preamble,
                             const QString &marker,
                             const QString &featureTitle,
                             const QString &fallbackCommand,
                             const QString &missingReasonKey,
                             const QString &suggestedPackage,
                             const QString &installableKey,
                             const QString &noteText,
                             const QString &scopeLabel,
                             bool targetScope,
                             const QMap<QString, QString> &evidence,
                             bool *repoUnavailable)
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
    // Round 106 (ITEM 2): the Packages column names the providing package for
    // EVERY row state (lvm2/mdadm provide the capability when installed too);
    // the Install affordance keeps the missing-reason derivation.
    capability.packageName = suggestedPackage;
    const bool missingReason = !capability.available
        && capabilityReasonKey(state) == missingReasonKey;
    const int verdict = missingReason
        ? installableSideChannelVerdict(evidence, installableKey, suggestedPackage)
        : -1;
    capability.installable = verdict == 1;
    capability.note = noteText;
    if (!capability.available) {
        const QString reason = state.startsWith(QStringLiteral("unavailable|"))
            ? state.mid(QStringLiteral("unavailable|").size())
            : state;
        if (!reason.isEmpty()) {
            capability.note += capability.note.isEmpty()
                ? reason
                : QStringLiteral(" (") + reason + QStringLiteral(")");
        }
        // Round 141 (ITEM O1/O2): the honest not-available caveat — the
        // helper proved the package unavailable in this system's repositories
        // (the shared string, the round-106 wording).
        if (verdict == 0) {
            capability.note += QStringLiteral(" (")
                + QCoreApplication::translate("CapabilityChecker",
                                              "Packages not available in this system's repositories")
                + QStringLiteral(")");
        }
        if (repoUnavailable) {
            *repoUnavailable = verdict == 0;
        }
    }
    return capability;
}

// Builds the snapshot-rollback Capability row from the cached capability
// preamble's scope-specific marker. The running host surfaces
// `Host snapshot rollback:`; the repair target surfaces the scope-neutral
// `Snapshot rollback:`. Both scopes surface the row whenever the marker is
// present — available OR unavailable — exactly like the storage-stack rows.
// A `reason:missing-*` line names the providing package (host `missing-snapper`
// -> snapper; `missing-btrfs-progs` -> btrfs-progs in both scopes);
// availability is read verbatim from the cache, never a probe.  Round 141
// Round 143 (ITEM T2): the FULL-ENABLE channels of the snapshot rows.  The
// helper's `snapshot-enable-packages: <set>|available` side-channel names the
// complete derived package set (the Packages column — every package the
// enablement needs, derived by the helper) and its verdict proves every
// MISSING piece present in the scope's repositories;
// `snapshot-config-enableable: available` is added only when the packages are
// all present and selects the Enable label over the Install label.  Either
// way ONE guarded full-enable verb runs the whole enablement.  Returns true
// when the action was offered (fail closed without the channel); repoUnavailable
// reports the honest not-available caveat.
static bool snapshotFullEnableChannels(const QMap<QString, QString> &evidence,
                                       Capability &capability,
                                       bool *repoUnavailable)
{
    const QString channel = evidence.value(
        QStringLiteral("snapshot-enable-packages")).trimmed();
    const int separator = channel.indexOf(QLatin1Char('|'));
    if (separator <= 0) {
        return false;
    }
    capability.packageName = channel.left(separator).trimmed();
    const bool verdictAvailable = channel.mid(separator + 1).trimmed()
        == QStringLiteral("available");
    if (repoUnavailable && !verdictAvailable) {
        *repoUnavailable = true;
    }
    if (!verdictAvailable) {
        return false;
    }
    const bool configOnly = evidence.value(
        QStringLiteral("snapshot-config-enableable")).trimmed()
        == QStringLiteral("available");
    capability.enableSnapshotConfig = configOnly;
    capability.snapshotFullEnable = !configOnly;
    capability.installable = !configOnly;
    return true;
}

// Builds the snapshot-rollback Capability row from the cached capability
// preamble's `Snapshot rollback:` / `Host snapshot rollback:` lines.
// (ITEM O1/O2): the Install affordance is gated on the helper's proven
// side-channels for the matching reason; round 143 (ITEM T2): every
// enable-able reason (missing package or missing config/store) now offers
// ONE full-enable action through `snapshot-enable-packages` +
// `snapshot-config-enableable` — the derived package set names the Packages
// cell.  repoUnavailable reports the honest not-available caveat state.  The
// note is the shared static description (round 105, ITEM 3) with the
// machine-readable reason folded in.
Capability snapshotRollbackCapability(const QString &preamble,
                                      const QString &scopeLabel,
                                      bool targetScope,
                                      const QMap<QString, QString> &evidence,
                                      bool *repoUnavailable)
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
    capability.unavailableWithReason = state.startsWith(QStringLiteral("unavailable|"));
    capability.command = evidenceText.isEmpty()
        ? (targetScope ? QStringLiteral("btrfs") : QStringLiteral("snapper"))
        : evidenceText;
    // Round 141 (ITEM O1/O2): the Packages cell names the providing package
    // for the missing reason the row actually reports (the host scope can
    // name btrfs-progs too); round 143 (ITEM T2): an enable-able reason names
    // the COMPLETE derived package set instead.  A scope with no
    // missing-reason keeps the default package name as before (snapper on the
    // host / btrfs-progs on the target).
    capability.packageName = targetScope
        ? QStringLiteral("btrfs-progs") : QStringLiteral("snapper");
    // Round 146 (ITEM 3): the helper emits the `snapshot-enable-packages`
    // channel in EVERY state — available, missing or refused (separate-boot,
    // timeshift …) — naming the COMPLETE derived set (snapper + btrfs-progs).
    // The Packages column therefore always shows the full set, never a
    // partial list; the Enable/Install affordance below still consumes the
    // channel only for the enable-able reasons.
    const QString packagesChannel = evidence.value(
        QStringLiteral("snapshot-enable-packages")).trimmed();
    const int packagesSeparator = packagesChannel.indexOf(QLatin1Char('|'));
    if (packagesSeparator > 0) {
        const QString fullSet = packagesChannel.left(packagesSeparator).trimmed();
        if (!fullSet.isEmpty()) {
            capability.packageName = fullSet;
        }
    }
    capability.installable = false;
    capability.note = QCoreApplication::translate("CapabilityChecker",
                                                  "Optional Btrfs snapshot rollback support");
    int verdict = -1;
    if (!capability.available) {
        const QString reasonKey = capabilityReasonKey(state);
        // Round 145 (ITEM E3): the missing-package reasons keep the STATUS
        // column at "Missing" even when the full-enable side-channel proved
        // the packages installable (the enableable proof must not flip the
        // status — the capability itself stays missing until the enablement
        // completes; the Install button is unaffected).
        capability.missingPackageStatus = reasonKey == QStringLiteral("missing-snapper")
            || reasonKey == QStringLiteral("missing-btrfs-progs");
        // Round 143 (ITEM T2): the full-enable reason set — a missing
        // prerequisite package (either scope) or the missing config/store
        // (host no-snapper-root-config / target no-snapshot-store).  All of
        // them offer ONE guarded full-enable verb through the derived
        // side-channels; the reason alone never offers anything.
        const bool fullEnableReason = reasonKey == QStringLiteral("missing-snapper")
            || reasonKey == QStringLiteral("missing-btrfs-progs")
            || reasonKey == QStringLiteral("no-snapper-root-config")
            || reasonKey == QStringLiteral("no-snapshot-store");
        if (fullEnableReason) {
            // The helper offered the full-enable action (the verdict carries
            // the proven repositories); the Enable-only state keeps
            // installable=false (the Enable label) while the full Install
            // state sets it.
            if (snapshotFullEnableChannels(evidence, capability, repoUnavailable)) {
                verdict = capability.installable ? 1 : -1;
            } else {
                verdict = -1;
            }
        }
        capability.installable = verdict == 1;
        const QString reason = state.startsWith(QStringLiteral("unavailable|"))
            ? state.mid(QStringLiteral("unavailable|").size())
            : state;
        if (!reason.isEmpty()) {
            capability.note += QStringLiteral(" (") + reason + QStringLiteral(")");
        }
        // Round 141 (ITEM O1/O2): the honest not-available caveat (the
        // shared round-106 string) when the helper proved the package
        // unavailable in this system's repositories.
        if (verdict == 0) {
            capability.note += QStringLiteral(" (")
                + QCoreApplication::translate("CapabilityChecker",
                                              "Packages not available in this system's repositories")
                + QStringLiteral(")");
        }
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
    if (key == QStringLiteral("input-devices")) return QCoreApplication::translate("CapabilityChecker", "Input device repair");
    if (key == QStringLiteral("network")) return QCoreApplication::translate("CapabilityChecker", "Network / connectivity repair");
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

// The concise per-row note for one repair-tool capability row — the legacy
// capability table's Notes-column content style (what the capability does
// plus the package/tool info).  Round 105 (ITEM 3): these strings are SHARED
// with the legacy GUI (legacy/gui/src/LegacyMainWindow.cpp capabilitySpecs +
// capabilityNoteSpecs); the ui-parity-qt3 capability-notes block locks the two
// sources, so a wording change must touch both.
static QString capabilityNoteForTool(const QString &key)
{
    if (key == QStringLiteral("validate")) return QCoreApplication::translate("CapabilityChecker", "Read-only preflight of mounts, filesystem metadata, boot files and mapper consistency before any repair");
    if (key == QStringLiteral("filesystem")) return QCoreApplication::translate("CapabilityChecker", "Read-only check of the selected system's root and /boot filesystems, then repair only the devices that report errors");
    if (key == QStringLiteral("dpkg")) return QCoreApplication::translate("CapabilityChecker", "Completes interrupted dpkg package configuration in the selected repair system");
    if (key == QStringLiteral("fixbroken")) return QCoreApplication::translate("CapabilityChecker", "Repairs broken package dependencies after the simulation-first safety preflight");
    if (key == QStringLiteral("aptupdate")) return QCoreApplication::translate("CapabilityChecker", "Refreshes APT metadata without upgrading installed packages");
    if (key == QStringLiteral("upgrade")) return QCoreApplication::translate("CapabilityChecker", "Simulates the APT transaction, then applies a safe upgrade");
    if (key == QStringLiteral("kernels")) return QCoreApplication::translate("CapabilityChecker", "Kernel removal, cleanup and install run through the helper's interactive menu; nothing is removed or modified until you pick an action and confirm it");
    if (key == QStringLiteral("dkms")) return QCoreApplication::translate("CapabilityChecker", "Rebuilds out-of-tree kernel modules; required only when the system uses a module build system");
    if (key == QStringLiteral("initramfs")) return QCoreApplication::translate("CapabilityChecker", "Rebuilds the initramfs images for the installed kernels");
    if (key == QStringLiteral("display")) return QCoreApplication::translate("CapabilityChecker", "Restores the configured display manager and its startup link without starting the GUI");
    if (key == QStringLiteral("gpu")) return QCoreApplication::translate("CapabilityChecker", "Evidence-driven GPU driver repair through the helper's interactive menu");
    if (key == QStringLiteral("input-devices")) return QCoreApplication::translate("CapabilityChecker", "Evidence-driven keyboard, mouse and touchpad repair through the helper's interactive menu");
    if (key == QStringLiteral("network")) return QCoreApplication::translate("CapabilityChecker", "Evidence-driven network repair through the helper's interactive menu; live connectivity changes never touch the interface holding the default route");
    if (key == QStringLiteral("efi")) return QCoreApplication::translate("CapabilityChecker", "Repairs the EFI / UKI boot path for UEFI systems");
    if (key == QStringLiteral("grub")) return QCoreApplication::translate("CapabilityChecker", "Regenerates the GRUB configuration (backed up, every entry preserved)");
    if (key == QStringLiteral("extlinux")) return QCoreApplication::translate("CapabilityChecker", "Alternative bootloader; on this system extlinux is not part of the default boot path");
    if (key == QStringLiteral("bootstack")) return QCoreApplication::translate("CapabilityChecker", "One guarded pass: mapper/crypttab validation, initramfs rebuild and bootloader configuration regeneration");
    return QString();
}

// ===== The shared capability -> package table (round 106) ==================
// Loads data/capability-packages.tsv (embedded at build time via
// resources/boot-repair.qrc as ":/data/capability-packages.tsv", with the
// source-tree file fallback for dev/test runs).  The SAME file backs the
// legacy Qt3 GUI (staged into the Etch package); the parity contract
// (legacy/tests/test-ui-parity-qt3.sh) locks the row set + a golden checksum.
// Rows: <key> TAB <family> TAB <packages>; the exact family row wins, "*" is
// the fallback.  Package names are raw (never translated).
namespace {

struct CapabilityPackagesRow
{
    QString key;
    QString family;
    QString packages;
};

const QList<CapabilityPackagesRow> &capabilityPackagesRows()
{
    static QList<CapabilityPackagesRow> rows;
    static bool loaded = false;
    if (loaded) {
        return rows;
    }
    loaded = true;
    QString content;
    QFile resourceFile(QStringLiteral(":/data/capability-packages.tsv"));
    if (resourceFile.open(QIODevice::ReadOnly)) {
        content = QString::fromUtf8(resourceFile.readAll());
    } else {
        QFile fileFile(QDir(QCoreApplication::applicationDirPath())
                           .absoluteFilePath(QStringLiteral("data/capability-packages.tsv")));
        if (fileFile.open(QIODevice::ReadOnly)) {
            content = QString::fromUtf8(fileFile.readAll());
        }
    }
    if (content.isEmpty()) {
        qWarning("capability-packages.tsv: the shared capability package table "
                 "is unavailable; the Packages column falls back to the "
                 "probe-derived suggestions only");
        return rows;
    }
    for (const QString &rawLine : content.split(QLatin1Char('\n'))) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
            continue;
        }
        const QStringList columns = line.split(QLatin1Char('\t'));
        if (columns.size() < 3 || columns.at(0).isEmpty()) {
            continue;
        }
        CapabilityPackagesRow row;
        row.key = columns.at(0).trimmed();
        row.family = columns.at(1).trimmed();
        row.packages = columns.at(2).trimmed();
        rows.append(row);
    }
    return rows;
}

// The static package set for one capability key on one distribution family
// (empty when the table has no row — the Packages cell then shows only the
// probe-derived suggestion).  The exact family row wins; "*" is the fallback.
QString capabilityStaticPackages(const QString &key, const QString &family)
{
    QString fallback;
    const QString normalizedFamily = family.trimmed().toLower();
    const QList<CapabilityPackagesRow> &rows = capabilityPackagesRows();
    for (const CapabilityPackagesRow &row : rows) {
        if (row.key != key) {
            continue;
        }
        if (row.family == normalizedFamily) {
            return row.packages;
        }
        if (row.family == QLatin1Char('*') && fallback.isEmpty()) {
            fallback = row.packages;
        }
    }
    return fallback;
}

} // namespace

// The stable repair-tool key order emitted by the helper's
// diagnostic_repair_capabilities(). Iterating this list (instead of the parse
// map's key order) keeps the table deterministic and mirrors the helper.// Functional grouping (user-verified): package chain first, then the kernel
// group (kernels with the module-build and initramfs stages), then the
// display/gpu group, then the boot chain, then boot-stack reconciliation.
static const QStringList &repairToolKeyOrder()
{
    static const QStringList keys = {
        QStringLiteral("validate"), QStringLiteral("filesystem"), QStringLiteral("dpkg"),
        QStringLiteral("fixbroken"), QStringLiteral("aptupdate"), QStringLiteral("upgrade"),
        QStringLiteral("kernels"), QStringLiteral("dkms"), QStringLiteral("initramfs"),
        QStringLiteral("display"), QStringLiteral("gpu"),
        QStringLiteral("input-devices"), QStringLiteral("network"),
        QStringLiteral("efi"),
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
        capability.unavailableWithReason = state.startsWith(QStringLiteral("unavailable|"));
        const bool unavailable = !capability.available;
        // Round 106 (ITEM 2): the probe-derived package suggestion (even
        // without a repo proof) + the helper's explicit unavailable verdict.
        QString probePackage;
        bool repoUnavailable = false;
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
            const QString dkmsChannel = evidence.value(
                QStringLiteral("dkms-installable")).trimmed();
            const bool packageInTargetRepo = dkmsChannel == QStringLiteral("available");
            if (!capability.available && installableMissingReason) {
                // Round 106 (ITEM 2): the Packages column names the package
                // that WOULD enable the build system even when the helper
                // proved it unavailable in the scope's repositories (the
                // note carries the honest caveat).
                probePackage = moduleBuild == QStringLiteral("none")
                    ? moduleBuildSystemPackageForFamily(distributionFamily)
                    : moduleBuildSystemPackageForBackend(moduleBuild);
                capability.installable = packageInTargetRepo;
            } else {
                capability.installable = false;
            }
            repoUnavailable = !packageInTargetRepo && !dkmsChannel.isEmpty();
        } else if (key == QStringLiteral("display")) {
            capability.command = evidence.value(key);
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
                if (separator > 0) {
                    // Round 106 (ITEM 2): the suggested DM package names the
                    // Packages cell regardless of the repo proof; only the
                    // Install affordance stays gated on |available.
                    probePackage = sideChannel.left(separator).trimmed();
                    capability.installable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available");
                    repoUnavailable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("unavailable");
                }
            }
        } else if (key == QStringLiteral("input-devices")) {
            capability.command = evidence.value(key);
            capability.installable = false;
            // Round 85: the missing input driver/firmware Install suggestion
            // (the display-installable fail-closed pattern).  The helper only
            // emits the side-channel on the proven reasons (firmware-missing /
            // module-missing) and only after proving the package present in
            // the scope's own repositories; `Repair capability evidence
            // input-installable: <pkg>|available` is the single source of
            // truth.  Round 106 (ITEM 2): the suggested package names the
            // Packages cell even when the repo proof says unavailable.
            if (capability.available) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("input-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0) {
                    probePackage = sideChannel.left(separator).trimmed();
                    capability.installable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available");
                    repoUnavailable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("unavailable");
                }
            }
        } else if (key == QStringLiteral("network")) {
            capability.command = evidence.value(key);
            capability.installable = false;
            // Round 85: the network firmware Install suggestion (firmware-only
            // per the plan), the same fail-closed side-channel pattern.
            // Round 106 (ITEM 2): the suggested package names the Packages
            // cell even when the repo proof says unavailable.
            if (capability.available) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("network-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0) {
                    probePackage = sideChannel.left(separator).trimmed();
                    capability.installable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available");
                    repoUnavailable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("unavailable");
                }
            }
        } else if (key == QStringLiteral("bootstack")) {
            capability.command = evidence.value(key);
            capability.installable = false;
            // Round 39 (Ubuntu #4): the missing initramfs/GRUB prerequisite
            // Install suggestion (the dkms/display pattern).  The helper
            // derives the package list from the DETECTED initramfs/bootloader
            // backends + firmware mode and proves every package present in the
            // scope's repositories (`bootstack-installable: <pkg> [<pkg>]|
            // available`).  Fail closed without it.  Round 106 (ITEM 2): the
            // suggested package list names the Packages cell even when the
            // repo proof says unavailable.
            const QString reasonKey = capabilityReasonKey(state);
            if (!capability.available
                && reasonKey == QStringLiteral("bootstack-needs-initramfs-grub")) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("bootstack-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0) {
                    probePackage = sideChannel.left(separator).trimmed();
                    capability.installable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available");
                    repoUnavailable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("unavailable");
                }
            }
        } else if (key == QStringLiteral("efi")) {
            capability.command = evidence.value(key);
            capability.installable = false;
            // Round 141 (ITEM O1/O2): the missing-GRUB-tooling Install
            // suggestion for the EFI row (the round-39/42 pattern).  The
            // helper derives the package(s) from the DETECTED bootloader
            // backend + firmware mode (efi_install_suggestion — probe, don't
            // prefilter) and proves every package present in the scope's
            // repositories; `Repair capability evidence efi-installable:
            // <pkg> [<pkg>]|available` is the single source of truth.  Fail
            // closed without it.  Only the missing-tooling reasons are
            // installable: a layout/firmware reason (legacy BIOS, no ESP, an
            // EFI-stub gap) is never a package fix.
            const QString reasonKey = capabilityReasonKey(state);
            const bool installableReason = reasonKey.startsWith(QStringLiteral("missing-grub"))
                || reasonKey == QStringLiteral("missing-grub2-mkconfig")
                || reasonKey == QStringLiteral("missing-grub2-install")
                || reasonKey == QStringLiteral("missing-grub-config-tooling")
                || reasonKey == QStringLiteral("missing-tuxedo-uki-builder");
            if (!capability.available && installableReason) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("efi-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0) {
                    probePackage = sideChannel.left(separator).trimmed();
                    capability.installable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available");
                    repoUnavailable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("unavailable");
                }
            }
        } else if (key == QStringLiteral("filesystem")) {
            capability.command = evidence.value(key);
            capability.installable = false;
            // Round 143 (ITEM T4): the missing check tool(s) get a
            // RECOVERY-HOST Install through this row — the helper derives the
            // providing packages from the scope's missing tools and proves
            // them in the recovery host's repositories
            // (`fs-tools-installable: <pkg> [<pkg>]|available`); the Install
            // routes to install-host-package regardless of the row's scope
            // (installOnRecoveryHost).  Fail closed without the channel.
            const QString reasonKey = capabilityReasonKey(state);
            if (!capability.available
                && reasonKey == QStringLiteral("no-filesystem-check-tool")) {
                const QString sideChannel = evidence.value(
                    QStringLiteral("fs-tools-installable")).trimmed();
                const int separator = sideChannel.indexOf(QLatin1Char('|'));
                if (separator > 0) {
                    probePackage = sideChannel.left(separator).trimmed();
                    capability.installable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("available");
                    repoUnavailable = !probePackage.isEmpty()
                        && sideChannel.mid(separator + 1).trimmed() == QStringLiteral("unavailable");
                    capability.installOnRecoveryHost = capability.installable;
                }
            }
        } else {
            capability.command = evidence.value(key);
            capability.installable = false;
        }
        // Round 106 (ITEM 2): the Packages column names a package for EVERY
        // row — the probe-derived suggestion above when one exists, else the
        // shared capability-packages table's static per-family name (the
        // legacy capabilitySpecs wording; the parity contract locks both
        // sources to the same data file).
        capability.packageName = probePackage.isEmpty()
            ? capabilityStaticPackages(key, distributionFamily)
            : probePackage;
        // Notes (round 105, ITEM 3): the concise per-row note (the legacy
        // content style) for EVERY row, with the machine-readable "why missing"
        // folded in for an unavailable tool so the reason stays visible.  The
        // dkms row additionally folds its evidence line (its Command column
        // shows only the short backend token).
        capability.note = capabilityNoteForTool(key);
        QString noteDetail;
        if (unavailable) {
            noteDetail = state.startsWith(QStringLiteral("unavailable|"))
                ? state.mid(QStringLiteral("unavailable|").size())
                : state;
        } else if (key == QStringLiteral("dkms")) {
            noteDetail = evidence.value(key);
        }
        if (!noteDetail.isEmpty()) {
            capability.note += capability.note.isEmpty()
                ? noteDetail
                : QStringLiteral(" (") + noteDetail + QStringLiteral(")");
        }
        // Round 106 (ITEM 2): the honest not-available caveat — a missing row
        // whose Packages cell names a package the helper proved unavailable in
        // this system's repositories says so in the Notes (the shared string
        // is locked by the ui-parity-qt3 capability-packages block).
        if (unavailable && !capability.installable && repoUnavailable
            && !capability.packageName.isEmpty()) {
            capability.note += QStringLiteral(" (")
                + QCoreApplication::translate("CapabilityChecker",
                                              "Packages not available in this system's repositories")
                + QStringLiteral(")");
        }
        capabilities.append(capability);
    }

    // Storage-stack markers.  LVM and software-RAID are scope-neutral: the
    // helper emits them for both the repair target and the running host, so
    // both scopes surface the same two rows with the caller's scope label and
    // targetScope routing.  Round 141 (ITEM O1/O2): the Install affordance
    // reads the helper's proven `lvm-installable`/`mdadm-installable`
    // side-channels (the fail-closed dkms pattern); the not-available caveat
    // is folded into the row by storageCapability itself.
    const Capability lvm = storageCapability(
        capabilitiesPreamble, QStringLiteral("Storage lvm inspection"),
        QCoreApplication::translate("CapabilityChecker", "LVM inspection"),
        QStringLiteral("lvs"), QStringLiteral("missing-lvm2"),
        QStringLiteral("lvm2"), QStringLiteral("lvm"),
        QCoreApplication::translate("CapabilityChecker", "Optional LVM storage-stack support"),
        scopeLabel, targetScope, evidence, nullptr);
    if (!lvm.feature.isEmpty()) {
        capabilities.append(lvm);
    }
    const Capability raid = storageCapability(
        capabilitiesPreamble, QStringLiteral("Storage software raid"),
        QCoreApplication::translate("CapabilityChecker", "Software RAID"),
        QStringLiteral("mdadm"), QStringLiteral("missing-mdadm"),
        QStringLiteral("mdadm"), QStringLiteral("mdadm"),
        QCoreApplication::translate("CapabilityChecker", "Optional Linux MD RAID support"),
        scopeLabel, targetScope, evidence, nullptr);
    if (!raid.feature.isEmpty()) {
        capabilities.append(raid);
    }

    // Snapshot rollback.  The host scope surfaces `Host snapshot rollback:` and
    // the repair-target scope `Snapshot rollback:`; the row appears in BOTH
    // available and unavailable states (never filtered out when available) so
    // the user always sees whether snapshots are usable and why.  Round 143
    // (ITEM T2): the Install/Enable affordance reads the helper's proven
    // full-enable side-channels (`snapshot-enable-packages` +
    // `snapshot-config-enableable`), and the Packages cell names the COMPLETE
    // derived package set the enablement needs.
    const Capability snapshot = snapshotRollbackCapability(
        capabilitiesPreamble, scopeLabel, targetScope, evidence, nullptr);
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

