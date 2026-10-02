// Host-side contract test for the legacy GUI's Qt-free evidence parser and
// device-inventory parsers. Compiled by legacy/tests/test-gui-contract.sh with
// plain g++ -std=c++98 (no Qt3 required), so the parser semantics are covered
// on any development host while the GUI itself only builds on Etch.
//
// Fixtures: legacy/tests/fixtures/gui-host-diagnose.txt (captured-style
// running-host transcript).

#include "DeviceInventory.h"
#include "EvidenceParser.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace {

int failures = 0;

void check(bool condition, const std::string &what)
{
    if (condition) {
        std::printf("ok - %s\n", what.c_str());
        return;
    }
    std::printf("FAIL - %s\n", what.c_str());
    ++failures;
}

std::string readFile(const char *path)
{
    std::ifstream input(path);
    if (!input) {
        std::fprintf(stderr, "cannot read fixture: %s\n", path);
        std::exit(2);
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool contains(const std::string &text, const std::string &needle)
{
    return text.find(needle) != std::string::npos;
}

void testTranscriptParsing(const std::string &fixture)
{
    const legacy::ParsedTranscript parsed = legacy::parseTranscript(fixture);
    check(parsed.capabilityLineCount == 13, "13 Repair tool lines parsed");
    check(parsed.unknownCapabilityLineCount == 0, "no unknown capability keys");
    check(parsed.capabilities.size() == 13, "all capability lines retained");
    check(parsed.capabilityEvidence.size() == 13, "all evidence lines retained");
    check(parsed.legacyFeatureLineCount == 6, "6 Legacy feature lines parsed");
    check(parsed.unknownLegacyFeatureLineCount == 0, "no unknown legacy feature keys");
    check(parsed.legacyFeatures.size() == 6, "all legacy feature lines retained");
    check(parsed.configFileLineCount == 0,
          "running-host fixture carries no target configuration probe");
    check(parsed.changeStatuses.size() == 1, "change status parsed");

    std::string reason;
    check(legacy::capabilityIsAvailable("available", &reason), "available state");
    check(!legacy::capabilityIsAvailable("unavailable|probe failed", &reason),
          "unavailable state rejected");
    check(reason == "probe failed", "unavailable reason extracted verbatim");
    check(!legacy::capabilityIsAvailable("mystery", &reason),
          "unknown state rejected (fail closed)");
    check(!legacy::capabilityIsAvailable("", &reason),
          "missing state rejected (fail closed)");

    check(legacy::legacyFeatureIsAvailable("available", &reason),
          "legacy feature available state");
    check(!legacy::legacyFeatureIsAvailable("unavailable|probe failed", &reason),
          "legacy feature unavailable state rejected");
    check(reason == "probe failed",
          "legacy feature reason extracted verbatim");
    check(!legacy::legacyFeatureIsAvailable("mystery", &reason),
          "unknown legacy feature state rejected (fail closed)");
    check(!legacy::legacyFeatureIsAvailable("", &reason),
          "missing legacy feature state rejected (fail closed)");

    check(legacy::diagnosticKeys().size() == 16, "16 diagnostic keys");
    check(legacy::legacyFeatureKeys().size() == 6, "6 legacy feature keys");

    check(legacy::changeStatusIsUnchanged("unchanged"), "unchanged status");
    check(legacy::changeStatusIsUnchanged("unchanged|read-only"),
          "unchanged|reason status");
    check(!legacy::changeStatusIsUnchanged("changed"), "changed status invalidates");
    check(!legacy::changeStatusIsUnchanged(""), "missing status invalidates");

    bool sawDrive = false;
    bool sawComponent = false;
    bool sawRoot = false;
    for (std::size_t i = 0; i < parsed.devicePaths.size(); ++i) {
        if (parsed.devicePaths[i] == "/dev/hda") sawDrive = true;
        if (parsed.devicePaths[i] == "/dev/mapper/root") sawComponent = true;
    }
    sawRoot = sawComponent;
    check(sawDrive, "Physical drive extracted as a device path");
    check(sawComponent, "Detected component extracted as a device path");
    check(sawRoot, "root mount source deduplicated with the component");

    bool sawSystem = false;
    bool sawFilesystem = false;
    for (std::size_t i = 0; i < parsed.targetFacts.size(); ++i) {
        if (contains(parsed.targetFacts[i], "System: Debian (legacy root")) sawSystem = true;
        if (contains(parsed.targetFacts[i], "Filesystem: ext3")) sawFilesystem = true;
    }
    check(sawSystem, "System fact parsed");
    check(sawFilesystem, "Filesystem fact parsed");
}

void testCapabilityModel(const std::string &fixture)
{
    const std::string identity = "host|/dev/hda|/dev/mapper/root";
    legacy::CapabilityModel model;
    std::string reason;

    check(!model.isAvailable("grub", identity, &reason),
          "no diagnostics -> gated action stays disabled");
    check(contains(reason, "Run diagnostics"), "no-diagnostics reason is explicit");

    model.applyDiagnosticTranscript(identity, fixture, true);
    check(model.hasDiagnostics(identity), "diagnostics cached for the identity");
    check(model.isAvailable("grub", identity, &reason),
          "available capability unlocks the action");
    check(model.isAvailable("validate", identity, &reason),
          "validate capability unlocks");
    check(model.isAvailable("display", identity, &reason),
          "legacy SysV display capability unlocks the display stage");
    check(model.evidence("display")
              == "legacy SysV: sysvinit display manager kdm (/usr/bin/kdm, init script /etc/init.d/kdm)",
          "display evidence line cached verbatim");
    check(!model.isAvailable("dkms", identity, &reason),
          "unavailable capability stays disabled");
    check(reason == "DKMS is not installed in the target",
          "unavailable reason is the helper's probe reason");
    check(!model.isAvailable("mystery", identity, &reason),
          "unknown key fails closed");
    check(!model.isAvailable("grub", "target|/dev/hdb|/dev/hdb1", &reason),
          "diagnostics for another identity never unlock this one");
    check(model.evidence("grub") == "update-grub present", "evidence line cached");

    // Legacy feature lines gate the legacy-only workflows with the same
    // fail-closed semantics as the capability lines.
    check(!model.legacyFeatureAvailable("shell", identity, &reason),
          "unavailable legacy feature stays disabled");
    check(reason == "the installed timeout does not support --foreground/--kill-after",
          "legacy feature reason is the helper's probe reason");
    check(!model.legacyFeatureAvailable("mystery", identity, &reason),
          "unknown legacy feature key fails closed");
    check(model.legacyFeatureState("host-maintenance") == "available",
          "the plain-chroot fallback unlocks host-maintenance on the Etch host");
    check(model.legacyFeatureAvailable("host-maintenance", identity, &reason),
          "host-maintenance unlocks through the guarded plain-chroot fallback");
    check(!model.legacyFeatureAvailable("host-shell", identity, &reason),
          "host-shell keeps its strict unshare gate");
    check(reason == "unshare is not installed in the recovery environment",
          "host-shell reason stays the helper's probe reason");
    check(!model.legacyFeatureAvailable("shell", "target|/dev/hdb|/dev/hdb1", &reason),
          "legacy features never unlock for another identity");

    legacy::CapabilityModel noFeatures;
    noFeatures.applyDiagnosticTranscript(identity, "Repair tool validate: available\n", true);
    check(!noFeatures.legacyFeatureAvailable("shell", identity, &reason),
          "missing Legacy feature line fails closed");
    check(contains(reason, "No 'Legacy feature shell:' line was cached"),
          "missing legacy feature reason names the exact line");

    model.applyCommandTranscript("Repair change status fixbroken: changed\n");
    check(model.diagnosticsStale(), "changed status marks diagnostics stale");
    check(!model.isAvailable("grub", identity, &reason),
          "stale diagnostics keep actions disabled");
    check(!model.legacyFeatureAvailable("shell", identity, &reason),
          "stale diagnostics keep legacy features disabled");
    check(contains(reason, "stale"), "stale reason is explicit");

    legacy::CapabilityModel clean;
    clean.applyDiagnosticTranscript(identity, fixture, true);
    clean.applyCommandTranscript(
        "Repair change status fixbroken: unchanged|simulated transaction proposed no package changes\n");
    check(!clean.diagnosticsStale(),
          "proven-unchanged status keeps diagnostics fresh");
    check(clean.isAvailable("grub", identity, &reason),
          "actions stay enabled after a proven no-op");

    legacy::CapabilityModel failed;
    failed.applyDiagnosticTranscript(identity, fixture, false);
    check(!failed.isAvailable("grub", identity, &reason),
          "failed diagnostic run clears capabilities (fail closed)");

    legacy::CapabilityModel empty;
    empty.applyDiagnosticTranscript(identity, "no capability lines here\n", true);
    check(!empty.hasDiagnostics(identity),
          "a transcript without capability lines is not diagnostics evidence");
    check(!empty.isAvailable("grub", identity, &reason),
          "missing capability lines keep actions disabled");
}

// The three-way classification behind the Settings -> Full Repair presentation
// (mirrors MainWindow::capabilityState). Available = exact cached `available`
// line; Unavailable = explicit unavailable|reason (or stale/unknown key);
// NoEvidence = no cached line yet (identity mismatch or no completed
// diagnostics). The fail-closed gate (isAvailable) stays binary: only
// Available unlocks an action.
void testCapabilityState(const std::string &fixture)
{
    const std::string identity = "host|/dev/hda";
    legacy::CapabilityModel model;
    std::string reason;

    // Before any diagnostics: every known key is NoEvidence, never Unavailable.
    check(model.capabilityState("grub", identity, &reason)
              == legacy::CapabilityModel::CapabilityNoEvidence,
          "no diagnostics -> three-way state is NoEvidence");
    check(contains(reason, "Run diagnostics"),
          "no-evidence reason names the missing diagnostics");
    check(!model.isAvailable("grub", identity, &reason),
          "no diagnostics keeps the fail-closed gate disabled");

    // Unknown keys fail closed as Unavailable, never NoEvidence.
    check(model.capabilityState("mystery", identity, &reason)
              == legacy::CapabilityModel::CapabilityUnavailable,
          "unknown key is Unavailable (fail closed)");

    model.applyDiagnosticTranscript(identity, fixture, true);

    check(model.capabilityState("grub", identity, &reason)
              == legacy::CapabilityModel::CapabilityAvailable,
          "available capability line -> Available");
    check(model.capabilityState("dkms", identity, &reason)
              == legacy::CapabilityModel::CapabilityUnavailable,
          "unavailable capability line -> Unavailable");
    check(reason == "DKMS is not installed in the target",
          "three-way unavailable reason is the helper's probe reason");

    // Evidence belongs to another scope: NoEvidence, so a persisted Settings
    // selection can stay visible instead of being greyed out as "unavailable".
    check(model.capabilityState("grub", "target|/dev/hdb", &reason)
              == legacy::CapabilityModel::CapabilityNoEvidence,
          "diagnostics for another identity -> NoEvidence");
    check(!model.isAvailable("grub", "target|/dev/hdb", &reason),
          "identity mismatch keeps the fail-closed gate disabled");

    // A diagnostic run that simply did not emit this key's line is NoEvidence,
    // not a hard unavailable, so the plan checkbox can await a later run.
    legacy::CapabilityModel partial;
    partial.applyDiagnosticTranscript(identity, "Repair tool validate: available\n", true);
    check(partial.capabilityState("validate", identity, &reason)
              == legacy::CapabilityModel::CapabilityAvailable,
          "partial run still reports the emitted key available");
    check(partial.capabilityState("grub", identity, &reason)
              == legacy::CapabilityModel::CapabilityNoEvidence,
          "missing key line -> NoEvidence");
    check(contains(reason, "No 'Repair tool grub:' line was cached"),
          "missing-key NoEvidence reason names the exact line");
    check(!partial.isAvailable("grub", identity, &reason),
          "missing key line keeps the fail-closed gate disabled");

    // A stale cache is a hard Unavailable (never NoEvidence): the repair was
    // not proven unchanged, so the saved selection must not stay checkable.
    model.applyCommandTranscript("Repair change status fixbroken: changed\n");
    check(model.capabilityState("grub", identity, &reason)
              == legacy::CapabilityModel::CapabilityUnavailable,
          "stale diagnostics -> Unavailable");
    check(contains(reason, "stale"),
          "stale three-way reason is explicit");
    check(!model.isAvailable("grub", identity, &reason),
          "stale diagnostics keep the fail-closed gate disabled");

    // A failed diagnostic run carries no evidence (NoEvidence), never a hard
    // unavailable, matching the cleared-capabilities fail-closed state.
    legacy::CapabilityModel failed;
    failed.applyDiagnosticTranscript(identity, fixture, false);
    check(failed.capabilityState("grub", identity, &reason)
              == legacy::CapabilityModel::CapabilityNoEvidence,
          "failed diagnostic run -> NoEvidence");
}

// The Etch target configuration probe: `Legacy config <key>:` lines from a
// target diagnostic run decide which files the GUI may offer for editing.
void testConfigFileProbe()
{
    const std::vector<std::string> keys = legacy::configFileKeys();
    check(keys.size() == 8, "8 Etch configuration keys");
    bool sawInittab = false;
    bool sawMenuLst = false;
    bool sawAptConf = false;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == "inittab") sawInittab = true;
        if (keys[i] == "menu-lst") sawMenuLst = true;
        if (keys[i] == "apt-conf") sawAptConf = true;
    }
    check(sawInittab && sawMenuLst && sawAptConf,
          "Etch-specific keys present (inittab, menu-lst, apt-conf)");

    const std::string transcript =
        "Repair tool validate: available\n"
        "Legacy config fstab: available\n"
        "Legacy config inittab: unavailable|/etc/inittab is not a readable "
        "regular file in the selected target\n"
        "Legacy config menu-lst: available\n"
        "Legacy config crypttab: available\n"
        "Legacy config modules: unavailable|/etc/modules is not a readable "
        "regular file in the selected target\n"
        "Legacy config interfaces: available\n"
        "Legacy config sources-list: available\n"
        "Legacy config apt-conf: unavailable|/etc/apt/apt.conf is not a "
        "readable regular file in the selected target\n"
        "Legacy config mystery: available\n";
    const legacy::ParsedTranscript parsed = legacy::parseTranscript(transcript);
    check(parsed.configFileLineCount == 9, "9 Legacy config lines parsed");
    check(parsed.unknownConfigFileLineCount == 1, "one unknown config key counted");
    check(parsed.configFiles.size() == 9, "all config lines retained");

    const std::string identity = "target|/dev/hdb|/dev/hdb1";
    legacy::CapabilityModel model;
    std::string reason;
    check(!model.configFileAvailable("fstab", identity, &reason),
          "no diagnostics -> config file stays unavailable (fail closed)");
    check(contains(reason, "Run diagnostics"), "config no-diagnostics reason is explicit");

    model.applyDiagnosticTranscript(identity, transcript, true);
    check(model.configFileAvailable("fstab", identity, &reason),
          "available config probe unlocks the file");
    check(!model.configFileAvailable("inittab", identity, &reason),
          "absent config file stays unavailable");
    check(reason == "/etc/inittab is not a readable regular file in the selected target",
          "config reason is the helper's probe reason verbatim");
    check(!model.configFileAvailable("mystery", identity, &reason),
          "unknown config key fails closed");
    check(!model.configFileAvailable("fstab", "host|/dev/hda|/dev/mapper/root", &reason),
          "config probe for another identity never unlocks this one");
    check(model.configFileState("menu-lst") == "available",
          "config state cached verbatim");

    model.applyCommandTranscript("Repair change status fixbroken: changed\n");
    check(!model.configFileAvailable("fstab", identity, &reason),
          "stale diagnostics keep the config editor closed");
    check(contains(reason, "stale"), "stale config reason is explicit");

    legacy::CapabilityModel noProbe;
    noProbe.applyDiagnosticTranscript(identity, "Repair tool validate: available\n", true);
    check(!noProbe.configFileAvailable("fstab", identity, &reason),
          "missing Legacy config line fails closed");
    check(contains(reason, "No 'Legacy config fstab:' line was cached"),
          "missing config reason names the exact line");
}

void testDeviceParsers()
{
    const std::string partitions =
        "major minor  #blocks  name\n"
        "\n"
        "   3     0    8388608 hda\n"
        "   3     1     104391 hda1\n"
        "   3     2    2097152 hda2\n"
        "   3     5    4194304 hda5\n"
        " 253     0    2097152 dm-0\n"
        " 253     1    4194304 dm-1\n"
        "  11     0    2097152 sr0\n"
        "   7     0      10240 loop0\n";
    const std::vector<legacy::PartitionRecord> records =
        legacy::parseProcPartitions(partitions);
    check(records.size() == 8, "partition records parsed");
    check(records[0].name == "hda" && records[0].blocks == 8388608ULL,
          "disk record parsed");
    check(records[1].name == "hda1" && records[1].majorNumber == 3,
          "partition record parsed");

    const std::string mounts =
        "rootfs / rootfs rw 0 0\n"
        "/dev/hda1 /boot ext3 rw 0 0\n"
        "/dev/mapper/root / ext3 rw,data=ordered 0 0\n"
        "/dev/hda1 /boot ext3 rw,noatime 0 0\n";
    const std::map<std::string, legacy::MountRecord> mountMap =
        legacy::parseProcMounts(mounts);
    check(mountMap.size() == 3, "duplicate sources collapsed to one entry");
    check(mountMap.find("/dev/mapper/root") != mountMap.end()
              && mountMap.find("/dev/mapper/root")->second.target == "/",
          "root mapper mount parsed");
    check(mountMap.find("/dev/hda1") != mountMap.end()
              && mountMap.find("/dev/hda1")->second.options == "rw",
          "first (real) mount entry kept over later bind mounts");

    // Joined mount targets for the details panel: distinct targets per
    // source, primary first, bind mounts appended.
    const std::string mountTargetsText =
        "/dev/hda1 /boot ext3 rw 0 0\n"
        "/dev/hda1 /boot ext3 rw,noatime 0 0\n"
        "/dev/hda1 /mnt/boot ext3 ro 0 0\n"
        "/dev/mapper/root / ext3 rw 0 0\n";
    const std::map<std::string, std::string> mountTargets =
        legacy::parseMountTargets(mountTargetsText);
    check(mountTargets.find("/dev/hda1") != mountTargets.end()
              && mountTargets.find("/dev/hda1")->second == "/boot, /mnt/boot",
          "distinct mount targets joined (primary first, duplicates dropped)");
    check(mountTargets.find("/dev/mapper/root") != mountTargets.end()
              && mountTargets.find("/dev/mapper/root")->second == "/",
          "single mount target kept verbatim");

    const std::string swaps =
        "Filename                                Type            Size    Used    Priority\n"
        "/dev/hda2                               partition       2097144 0       -1\n";
    const std::map<std::string, std::string> swapMap =
        legacy::parseProcSwaps(swaps);
    check(swapMap.size() == 1 && swapMap.find("/dev/hda2") != swapMap.end(),
          "swap device parsed");

    check(legacy::looksLikeWholeDisk("hda"), "hda is a whole disk");
    check(legacy::looksLikeWholeDisk("sda"), "sda is a whole disk");
    check(legacy::looksLikeWholeDisk("md0"), "md0 is a whole disk");
    check(legacy::looksLikeWholeDisk("sr0"), "sr0 (optical) is a whole device");
    check(!legacy::looksLikeWholeDisk("hda1"), "hda1 is not a whole disk");
    check(!legacy::looksLikeWholeDisk("dm-0"), "dm-0 is not a whole disk");
    check(!legacy::looksLikeWholeDisk("loop0"), "loop0 is filtered out");
    check(!legacy::looksLikeWholeDisk("ram0"), "ram0 is filtered out");

    std::vector<std::string> diskNames;
    diskNames.push_back("hda");
    diskNames.push_back("sr0");
    std::map<std::string, std::string> attributes;
    attributes["hda/size"] = "16777216\n";      // 8 GiB in 512-byte sectors
    attributes["hda/device/model"] = "GENERIC DISK\n";
    attributes["hda/device-link"] = "../../../ide0/ide0.0\n";
    attributes["hda1/size"] = "208782\n";       // ~102 MiB
    attributes["sr0/removable"] = "1\n";
    attributes["dm-0/size"] = "4194304\n";
    attributes["dm-0/slave"] = "hda1\n";
    attributes["dm-1/size"] = "4194304\n";
    attributes["dm-1/slave"] = "dm-0\n";
    std::map<std::string, std::string> mapperLinks;
    mapperLinks["/dev/mapper/crypt"] = "dm-0";
    mapperLinks["/dev/mapper/root"] = "dm-1";
    std::map<std::string, std::string> uuidByPath;
    uuidByPath["/dev/hda1"] = "1111-2222";
    uuidByPath["/dev/mapper/root"] = "uuid-root";
    std::map<std::string, std::string> labelByPath;
    labelByPath["/dev/hda1"] = "boot";
    // udev metadata: hda1 is an unmounted ext3 /boot and hda5 a locked LUKS
    // container; hda2 is swap and the mappers are mounted.
    std::map<std::string, std::string> probedFsByPath;
    probedFsByPath["/dev/hda1"] = "ext3";
    probedFsByPath["/dev/hda5"] = "crypto_LUKS";
    std::map<std::string, legacy::MountRecord> mountRows;
    legacy::MountRecord rootMount;
    rootMount.source = "/dev/mapper/root";
    rootMount.target = "/";
    rootMount.fstype = "ext3";
    mountRows["/dev/mapper/root"] = rootMount;

    const std::vector<legacy::DeviceRow> rows = legacy::buildDeviceRows(
        records, mountRows, swapMap, diskNames, attributes, mapperLinks,
        uuidByPath, labelByPath, probedFsByPath, mountTargets);

    bool sawDisk = false;
    bool sawPart = false;
    bool sawMapper = false;
    bool sawCrypt = false;
    bool sawOptical = false;
    bool sawLoop = false;
    bool sawDm = false;
    bool sawLockedLuks = false;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const legacy::DeviceRow &row = rows[i];
        if (row.path == "/dev/hda") {
            sawDisk = true;
            check(row.disk && !row.mapper, "/dev/hda is a disk row");
            check(row.size == "8.0G", "/dev/hda size formatted");
            check(row.model == "GENERIC DISK", "/dev/hda model read");
            check(row.transport == "IDE", "/dev/hda transport probed");
        }
        if (row.path == "/dev/sr0") {
            sawOptical = true;
            check(row.optical, "/dev/sr0 is optical");
            check(row.transport == "removable", "/dev/sr0 removable transport");
        }
        if (row.path == "/dev/hda1") {
            sawPart = true;
            check(!row.disk && row.parent == "hda", "/dev/hda1 parent resolved");
            check(row.uuid == "1111-2222", "/dev/hda1 UUID linked");
            check(row.label == "boot", "/dev/hda1 label linked");
            check(row.probedFstype == "ext3" && !row.encrypted,
                  "/dev/hda1 udev filesystem probed");
        }
        if (row.path == "/dev/hda5") {
            sawLockedLuks = true;
            check(row.probedFstype == "crypto_LUKS" && row.encrypted,
                  "/dev/hda5 locked LUKS container probed");
            check(row.mountpoint.empty(), "/dev/hda5 is not mounted");
        }
        if (row.path == "/dev/mapper/root") {
            sawMapper = true;
            check(row.mapper && row.parent == "hda1",
                  "mapper row resolves the dm chain to the backing partition");
            check(row.kernelName == "dm-1", "mapper row keeps the dm kernel name");
            check(row.uuid == "uuid-root", "mapper UUID linked by path");
            check(row.mountpoint == "/" && row.fstype == "ext3",
                  "mapper row carries the mount");
            check(row.mountpoints == "/", "mapper row carries the joined mounts");
        }
        if (row.path == "/dev/mapper/crypt") {
            sawCrypt = true;
            check(row.parent == "hda1", "crypt mapper resolves to its partition");
        }
        if (row.path == "/dev/loop0") sawLoop = true;
        if (row.path == "/dev/dm-0") sawDm = true;
    }
    check(sawDisk, "/dev/hda listed");
    check(sawPart, "/dev/hda1 listed");
    check(sawMapper, "/dev/mapper/root listed from /dev/mapper");
    check(sawCrypt, "/dev/mapper/crypt listed");
    check(sawOptical, "/dev/sr0 listed");
    check(sawLockedLuks, "/dev/hda5 listed as a locked LUKS container");
    check(!sawLoop, "loop devices are not offered");
    check(!sawDm, "raw dm-N nodes are not duplicated as rows");

    check(legacy::formatSizeKb(8388608ULL) == "8.0G", "formatSizeKb GiB");
    check(legacy::formatSizeKb(208782ULL) == "203.9M", "formatSizeKb MiB");
    check(legacy::formatSizeKb(512ULL) == "512K", "formatSizeKb KiB");

    // /dev/mapper symlink layout (modern udev): the target names dm-N.
    char dir[128];
    std::snprintf(dir, sizeof(dir), "/tmp/legacy-mapper-test-%ld",
                  static_cast<long>(::getpid()));
    ::rmdir(dir);
    check(::mkdir(dir, 0700) == 0, "mapper fixture directory created");
    const std::string link = std::string(dir) + "/root";
    check(::symlink("dm-7", link.c_str()) == 0, "mapper symlink fixture created");
    std::map<std::string, std::string> emptyMap;
    check(legacy::mapperKernelName(link, emptyMap) == "dm-7",
          "mapper symlink target resolved");
    check(legacy::mapperKernelName("/no/such/mapper", emptyMap).empty(),
          "missing mapper entry resolves to empty");
    ::unlink(link.c_str());
    ::rmdir(dir);

    // Live detection is best-effort (skipped on hosts without a /dev-backed
    // root); when it resolves, both values must be populated.
    std::string liveRoot;
    std::string liveDisk;
    if (legacy::detectRunningHostTarget(&liveRoot, &liveDisk)) {
        check(!liveRoot.empty() && !liveDisk.empty(),
              "live running-host detection returns a consistent pair");
    } else {
        std::printf("skip - no /dev-backed root mount on this host\n");
    }
}

// The dm-name-collision / VG-mismatch regression (Etch legacy user test):
// the running host's LVs (host VG `vghost`) and the unlocked peer's
// LVs (VG `vgtarget`) are active at the same time, and the helper's
// UNLOCKED_ROOT probe (LVM2 2.02.07 lacks `lv_path`) can mis-attribute the
// running host's `vghost-root` LV as the unlocked root. The GUI re-resolves
// from the freshly-rescanned inventory, so the inventory's dm parent-chain
// must attribute each LV to its own disk: vghost-root -> hda5 (hda) and
// vgtarget-root -> hdb5 (hdb). This fixture reproduces the real Etch topology
// (major:minor 254:N == dm-N, slaves captured from /sys/block/dm-N/slaves).
void testUnlockedRootDiskAttribution()
{
    const std::string partitions =
        "major minor  #blocks  name\n"
        "\n"
        "   3     0    8388608 hda\n"
        "   3     1     104391 hda1\n"
        "   3     2          1 hda2\n"
        "   3     5    8135863 hda5\n"
        "   3    64    8388608 hdb\n"
        "   3    65     104391 hdb1\n"
        "   3    66          1 hdb2\n"
        "   3    69    8135863 hdb5\n"
        "  22     0      65536 hdc\n"
        " 254     0    8135863 dm-0\n"
        " 254     1    2379776 dm-1\n"
        " 254     2     323584 dm-2\n"
        " 254     3    5431296 dm-3\n"
        " 254     4    8135863 dm-4\n"
        " 254     5     282624 dm-5\n"
        " 254     6    2891776 dm-6\n"
        " 254     7    1384448 dm-7\n"
        " 254     8     483328 dm-8\n"
        " 254     9     253952 dm-9\n"
        " 254    10    2838528 dm-10\n";
    const std::vector<legacy::PartitionRecord> records =
        legacy::parseProcPartitions(partitions);

    std::vector<std::string> diskNames;
    diskNames.push_back("hda");
    diskNames.push_back("hdb");
    diskNames.push_back("hdc");

    std::map<std::string, std::string> attributes;
    attributes["hda/size"] = "16777216\n";
    attributes["hdb/size"] = "16777216\n";
    attributes["hdc/size"] = "131072\n";
    // dm-N/slaves as observed on the Etch 2.6.18 kernel after unlocking hdb5:
    // the host stack sits on hda5_crypt (dm-0) -> hda5, the peer stack sits
    // on luks-<uuid> (dm-4) -> hdb5.
    attributes["dm-0/slave"] = "hda5\n";
    attributes["dm-1/slave"] = "dm-0\n";
    attributes["dm-2/slave"] = "dm-0\n";
    attributes["dm-3/slave"] = "dm-0\n";
    attributes["dm-4/slave"] = "hdb5\n";
    attributes["dm-5/slave"] = "dm-4\n";
    attributes["dm-6/slave"] = "dm-4\n";
    attributes["dm-7/slave"] = "dm-4\n";
    attributes["dm-8/slave"] = "dm-4\n";
    attributes["dm-9/slave"] = "dm-4\n";
    attributes["dm-10/slave"] = "dm-4\n";
    attributes["dm-0/size"] = "8135863\n";
    attributes["dm-1/size"] = "2379776\n";
    attributes["dm-2/size"] = "323584\n";
    attributes["dm-3/size"] = "5431296\n";
    attributes["dm-4/size"] = "8135863\n";
    attributes["dm-5/size"] = "282624\n";
    attributes["dm-6/size"] = "2891776\n";
    attributes["dm-7/size"] = "1384448\n";
    attributes["dm-8/size"] = "483328\n";
    attributes["dm-9/size"] = "253952\n";
    attributes["dm-10/size"] = "2838528\n";

    std::map<std::string, std::string> mapperLinks;
    mapperLinks["/dev/mapper/hda5_crypt"] = "dm-0";
    mapperLinks["/dev/mapper/vghost-root"] = "dm-1";
    mapperLinks["/dev/mapper/vghost-swap_1"] = "dm-2";
    mapperLinks["/dev/mapper/vghost-home"] = "dm-3";
    mapperLinks["/dev/mapper/luks-11111111-2222-3333-4444-555555555555"] = "dm-4";
    mapperLinks["/dev/mapper/vgtarget-root"] = "dm-5";
    mapperLinks["/dev/mapper/vgtarget-usr"] = "dm-6";
    mapperLinks["/dev/mapper/vgtarget-var"] = "dm-7";
    mapperLinks["/dev/mapper/vgtarget-swap_1"] = "dm-8";
    mapperLinks["/dev/mapper/vgtarget-tmp"] = "dm-9";
    mapperLinks["/dev/mapper/vgtarget-home"] = "dm-10";

    std::map<std::string, legacy::MountRecord> mounts;
    legacy::MountRecord rootMount;
    rootMount.source = "/dev/mapper/vghost-root";
    rootMount.target = "/";
    rootMount.fstype = "ext3";
    mounts["/dev/mapper/vghost-root"] = rootMount;
    legacy::MountRecord homeMount;
    homeMount.source = "/dev/mapper/vghost-home";
    homeMount.target = "/home";
    homeMount.fstype = "ext3";
    mounts["/dev/mapper/vghost-home"] = homeMount;

    std::map<std::string, std::string> swaps;
    swaps["/dev/mapper/vghost-swap_1"] = "partition";

    std::map<std::string, std::string> probedFsByPath;
    probedFsByPath["/dev/hda5"] = "crypto_LUKS";
    probedFsByPath["/dev/hdb5"] = "crypto_LUKS";
    probedFsByPath["/dev/hda1"] = "ext3";
    probedFsByPath["/dev/hdb1"] = "ext3";

    const std::vector<legacy::DeviceRow> rows = legacy::buildDeviceRows(
        records, mounts, swaps, diskNames, attributes, mapperLinks,
        std::map<std::string, std::string>(),
        std::map<std::string, std::string>(), probedFsByPath);

    std::map<std::string, legacy::DeviceRow> byPath;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        byPath[rows[i].path] = rows[i];
    }
    const std::map<std::string, legacy::DeviceRow>::const_iterator hroot =
        byPath.find("/dev/mapper/vghost-root");
    const std::map<std::string, legacy::DeviceRow>::const_iterator proot =
        byPath.find("/dev/mapper/vgtarget-root");
    check(hroot != byPath.end(), "vghost-root row present");
    check(proot != byPath.end(), "vgtarget-root row present");
    check(hroot != byPath.end() && hroot->second.parent == "hda5",
          "vghost-root resolves through hda5_crypt to its own hda5 partition");
    check(proot != byPath.end() && proot->second.parent == "hdb5",
          "vgtarget-root resolves through luks-<uuid> to the peer hdb5 partition");

    // The GUI's rootBelongsToDisk / resolvedUnlockedRoot decisions key on the
    // resolved parent chain: the mis-attributed running-host root must NOT
    // belong to hdb, and the real unlocked root must. Re-check the exact
    // predicate the GUI uses so the attribution cannot silently cross disks.
    const std::map<std::string, legacy::DeviceRow>::const_iterator hdb5 =
        byPath.find("/dev/hdb5");
    const std::map<std::string, legacy::DeviceRow>::const_iterator hda5 =
        byPath.find("/dev/hda5");
    check(hdb5 != byPath.end() && !hdb5->second.disk && hdb5->second.parent == "hdb",
          "hdb5 is a partition of hdb");
    check(hda5 != byPath.end() && !hda5->second.disk && hda5->second.parent == "hda",
          "hda5 is a partition of hda");
}

// The world-readable udev metadata probe: Etch's /dev/.udev/db records and
// the modern /run/udev/data format share the "E:<KEY>=<value>" lines. The
// filesystem-name helpers mirror the modern Linux-capable list and the
// crypto_LUKS detection used by the Unlock control.
void testUdevMetadata()
{
    const std::string record =
        "N:hda5\n"
        "S:disk/by-uuid/11111111-2222-3333-4444-555555555555\n"
        "M:3:5\n"
        "E:ID_TYPE=disk\n"
        "E:ID_FS_USAGE=crypto\n"
        "E:ID_FS_TYPE=crypto_LUKS\n"
        "E:ID_FS_UUID=11111111-2222-3333-4444-555555555555\n"
        "E:ID_FS_LABEL=\n";
    const std::map<std::string, std::string> properties =
        legacy::parseUdevDatabase(record);
    check(properties.size() == 5, "udev E: properties parsed");
    check(properties.find("ID_FS_TYPE") != properties.end()
              && properties.find("ID_FS_TYPE")->second == "crypto_LUKS",
          "udev ID_FS_TYPE parsed");
    check(properties.find("ID_FS_UUID") != properties.end()
              && properties.find("ID_FS_UUID")->second
                     == "11111111-2222-3333-4444-555555555555",
          "udev ID_FS_UUID parsed");
    check(legacy::parseUdevDatabase("").empty(), "empty udev record yields no properties");
    check(legacy::parseUdevDatabase("garbage\nN:hda\n").empty(),
          "malformed udev lines are ignored");

    check(legacy::isLinuxFileSystemName("ext3"), "ext3 is Linux-capable");
    check(legacy::isLinuxFileSystemName("EXT4"), "ext4 is case-insensitive");
    check(legacy::isLinuxFileSystemName("btrfs"), "btrfs is Linux-capable");
    check(legacy::isLinuxFileSystemName("xfs"), "xfs is Linux-capable");
    check(legacy::isLinuxFileSystemName("f2fs"), "f2fs is Linux-capable");
    check(!legacy::isLinuxFileSystemName("ntfs"), "ntfs is not a Linux root candidate");
    check(!legacy::isLinuxFileSystemName("crypto_LUKS"), "crypto_LUKS is not a filesystem");
    check(!legacy::isLinuxFileSystemName(""), "empty filesystem is not Linux-capable");

    check(legacy::looksLikeLuks("crypto_LUKS"), "crypto_LUKS detected");
    check(legacy::looksLikeLuks("CRYPTO_LUKS"), "crypto_LUKS detection is case-insensitive");
    check(!legacy::looksLikeLuks("ext3"), "ext3 is not LUKS");
    check(!legacy::looksLikeLuks(""), "empty filesystem is not LUKS");
}

void testUnlockHelpers()
{
    const std::string okTranscript =
        "Unlocking LUKS target /dev/hda5\n"
        "UNLOCKED=/dev/mapper/luks-etchroot\n"
        "UNLOCKED_ROOT=/dev/mapper/vgtarget-root\n"
        "UNLOCKED_ROOT_FSTYPE=ext3\n"
        "UNLOCKED_ROOT_UUID=smoke-uuid-value\n";
    check(legacy::unlockMapper(okTranscript) == "/dev/mapper/luks-etchroot",
          "unlock mapper path extracted");
    check(legacy::unlockRoot(okTranscript) == "/dev/mapper/vgtarget-root",
          "unlocked root component extracted");
    check(legacy::unlockRootFstype(okTranscript) == "ext3",
          "unlocked root fstype extracted");
    check(legacy::unlockRootUuid(okTranscript) == "smoke-uuid-value",
          "unlocked root uuid extracted");
    check(legacy::unlockRoot("UNLOCKED=/dev/mapper/x\n").empty(),
          "missing UNLOCKED_ROOT marker yields an empty root");
    check(!legacy::unlockAuthFailed(okTranscript),
          "successful unlock is not an auth failure");
    check(legacy::unlockMapper("no marker here").empty(),
          "missing unlock marker yields an empty mapper");

    const std::string failedTranscript =
        "UNLOCK_AUTH_FAILED=1\n"
        "ERROR: LUKS passphrase was not accepted.\n";
    check(legacy::unlockAuthFailed(failedTranscript),
          "auth-failure marker detected");
    check(legacy::unlockMapper(failedTranscript).empty(),
          "failed unlock carries no mapper");
    check(legacy::unlockErrorLine(failedTranscript) == "LUKS passphrase was not accepted.",
          "last error line extracted for the unlock status");

    const std::string twoErrors =
        "ERROR: first failure\n"
        "some progress\n"
        "ERROR: the exact failure\n";
    check(legacy::unlockErrorLine(twoErrors) == "the exact failure",
          "last ERROR line wins");
    check(legacy::unlockErrorLine("no errors here").empty(),
          "missing error line yields an empty message");

    // The marker must be an exact line: prose that merely mentions it (for
    // example the log line the GUI writes) must not trigger a retry.
    check(!legacy::unlockAuthFailed("the UNLOCK_AUTH_FAILED=1 marker was not seen"),
          "auth-failure marker requires an exact line");
}

void testStageMapping()
{
    check(legacy::repairToolKeyForStage("fix-broken") == "fixbroken",
          "fix-broken maps to fixbroken");
    check(legacy::repairToolKeyForStage("dpkg-configure") == "dpkg",
          "dpkg-configure maps to dpkg");
    check(legacy::repairToolKeyForStage("apt-update") == "aptupdate",
          "apt-update maps to aptupdate");
    check(legacy::repairToolKeyForStage("apt-upgrade") == "upgrade",
          "apt-upgrade maps to upgrade");
    check(legacy::repairToolKeyForStage("initramfs") == "initramfs",
          "initramfs maps to initramfs");
    check(legacy::repairToolKeyForStage("fs-inspect") == "fs-inspect",
          "unknown stages pass through");
    check(legacy::capabilityKeys().size() == 13, "13 capability keys");
}

} // namespace

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <fixture-transcript>\n", argv[0]);
        return 2;
    }
    const std::string fixture = readFile(argv[1]);
    testTranscriptParsing(fixture);
    testCapabilityModel(fixture);
    testCapabilityState(fixture);
    testConfigFileProbe();
    testDeviceParsers();
    testUnlockedRootDiskAttribution();
    testUdevMetadata();
    testUnlockHelpers();
    testStageMapping();

    if (failures > 0) {
        std::printf("gui parser contract: FAIL (%d)\n", failures);
        return 1;
    }
    std::printf("gui parser contract: PASS\n");
    return 0;
}
