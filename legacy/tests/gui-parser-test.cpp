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
    check(!model.isAvailable("dkms", identity, &reason),
          "unavailable capability stays disabled");
    check(reason == "DKMS is not installed in the target",
          "unavailable reason is the helper's probe reason");
    check(!model.isAvailable("mystery", identity, &reason),
          "unknown key fails closed");
    check(!model.isAvailable("grub", "target|/dev/hdb|/dev/hdb1", &reason),
          "diagnostics for another identity never unlock this one");
    check(model.evidence("grub") == "update-grub present", "evidence line cached");

    model.applyCommandTranscript("Repair change status fixbroken: changed\n");
    check(model.diagnosticsStale(), "changed status marks diagnostics stale");
    check(!model.isAvailable("grub", identity, &reason),
          "stale diagnostics keep actions disabled");
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

void testDeviceParsers()
{
    const std::string partitions =
        "major minor  #blocks  name\n"
        "\n"
        "   3     0    8388608 hda\n"
        "   3     1     104391 hda1\n"
        "   3     2    2097152 hda2\n"
        " 253     0    2097152 dm-0\n"
        "   7     0      10240 loop0\n";
    const std::vector<legacy::PartitionRecord> records =
        legacy::parseProcPartitions(partitions);
    check(records.size() == 5, "partition records parsed");
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
    attributes["hda1/size"] = "208782\n";       // ~102 MiB
    attributes["dm-0/size"] = "4194304\n";
    attributes["dm-0/slave"] = "hda1\n";
    std::map<std::string, std::string> mapperLinks;
    mapperLinks["/dev/mapper/root"] = "dm-0";
    std::map<std::string, legacy::MountRecord> mountRows;
    legacy::MountRecord rootMount;
    rootMount.source = "/dev/mapper/root";
    rootMount.target = "/";
    rootMount.fstype = "ext3";
    mountRows["/dev/mapper/root"] = rootMount;

    const std::vector<legacy::DeviceRow> rows = legacy::buildDeviceRows(
        records, mountRows, swapMap, diskNames, attributes, mapperLinks);

    bool sawDisk = false;
    bool sawPart = false;
    bool sawMapper = false;
    bool sawLoop = false;
    bool sawDm = false;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const legacy::DeviceRow &row = rows[i];
        if (row.path == "/dev/hda") {
            sawDisk = true;
            check(row.disk && !row.mapper, "/dev/hda is a disk row");
            check(row.size == "8.0G", "/dev/hda size formatted");
            check(row.model == "GENERIC DISK", "/dev/hda model read");
        }
        if (row.path == "/dev/hda1") {
            sawPart = true;
            check(!row.disk && row.parent == "hda", "/dev/hda1 parent resolved");
        }
        if (row.path == "/dev/mapper/root") {
            sawMapper = true;
            check(row.mapper && row.parent == "hda1",
                  "mapper row keeps the dm slave parent");
            check(row.mountpoint == "/" && row.fstype == "ext3",
                  "mapper row carries the mount");
        }
        if (row.path == "/dev/loop0") sawLoop = true;
        if (row.path == "/dev/dm-0") sawDm = true;
    }
    check(sawDisk, "/dev/hda listed");
    check(sawPart, "/dev/hda1 listed");
    check(sawMapper, "/dev/mapper/root listed from /dev/mapper");
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

void testUnlockHelpers()
{
    const std::string okTranscript =
        "Unlocking LUKS target /dev/hda5\n"
        "UNLOCKED=/dev/mapper/luks-etchroot\n";
    check(legacy::unlockMapper(okTranscript) == "/dev/mapper/luks-etchroot",
          "unlock mapper path extracted");
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
    testDeviceParsers();
    testUnlockHelpers();
    testStageMapping();

    if (failures > 0) {
        std::printf("gui parser contract: FAIL (%d)\n", failures);
        return 1;
    }
    std::printf("gui parser contract: PASS\n");
    return 0;
}
