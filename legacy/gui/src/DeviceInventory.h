// DeviceInventory - read-only block-device inventory for the Qt3 legacy GUI.
//
// The GUI runs unprivileged and Etch keeps /dev nodes root-only, so this module
// reads only world-readable kernel metadata: /proc/partitions, /proc/mounts,
// /proc/swaps, /sys/block/* attributes and the /dev/mapper symlink directory.
// It never opens a block device and never writes anything. The authoritative
// target identity still comes from the helper's read-only diagnostics
// (`Physical drive:`, `Detected component:`, root mount source); this list only
// provides selectable candidates.
//
// C++98 + POSIX only (no Qt) so the parsers are unit-testable on a modern host.

#ifndef LEGACY_DEVICE_INVENTORY_H
#define LEGACY_DEVICE_INVENTORY_H

#include <map>
#include <string>
#include <vector>

namespace legacy {

struct PartitionRecord {
    PartitionRecord();
    int majorNumber;
    int minorNumber;
    unsigned long long blocks; // 1024-byte blocks as reported by the kernel
    std::string name;          // hda, hda1, dm-0, ...
};

struct MountRecord {
    MountRecord();
    std::string source;
    std::string target;
    std::string fstype;
    std::string options;
};

struct DeviceRow {
    DeviceRow();
    std::string name;       // hda / hda1 / root (mapper name)
    std::string path;       // /dev/hda (mapper: /dev/mapper/<name>)
    std::string kernelName; // dm-0 for mapper rows, "" otherwise
    std::string parent;     // "" for a whole disk; mapper rows keep the
                            // resolved backing partition (hda5), walking
                            // through intermediate dm devices when possible
    bool disk;
    bool mapper;
    bool optical;
    bool encrypted;         // udev reports ID_FS_TYPE=crypto_LUKS
    unsigned long long blocks;
    std::string size;       // human-readable, from formatSizeKb
    std::string model;      // disks only, from sysfs
    std::string fstype;     // mounted or swapped filesystem, else ""
    std::string mountpoint; // mount target, else "" / "[swap]"
    std::string probedFstype; // udev ID_FS_TYPE (world-readable metadata), else ""
    std::string uuid;       // /dev/disk/by-uuid link value, else ""
    std::string label;      // /dev/disk/by-label link value, else ""
    std::string transport;  // IDE/SATA/USB/... best-effort, else ""
};

// Parsers for the world-readable kernel files (tested on the host).
std::vector<PartitionRecord> parseProcPartitions(const std::string &text);
std::map<std::string, MountRecord> parseProcMounts(const std::string &text);
std::map<std::string, std::string> parseProcSwaps(const std::string &text);

// Parses a udev database record (Etch's /dev/.udev/db/block@<name> or a
// modern /run/udev/data/b<major>:<minor> file): lines of the form
// "E:<KEY>=<value>" become map entries with the E: prefix stripped. Tolerant
// and never throws; unknown/malformed lines are ignored. This is
// world-readable metadata, never a block-device read.
std::map<std::string, std::string> parseUdevDatabase(const std::string &text);

// True for the Linux root-capable filesystem names (mirrors the modern
// SystemScanner::isLinuxCapableFileSystem list). Case-insensitive.
bool isLinuxFileSystemName(const std::string &fstype);

// True when a probed filesystem name identifies a LUKS container.
bool looksLikeLuks(const std::string &fstype);

// Builds the display rows from already-read inputs. `diskNames` are the entries
// of /sys/block that are whole disks; `attributes` maps "<name>/<attr>" to the
// file contents (size in 512-byte sectors, device/model, removable,
// device-link = the /sys/block/<name>/device symlink target).
// `mapperLinks` maps "/dev/mapper/<name>" to the dm kernel name (dm-0).
// `uuidByPath`/`labelByPath` map a resolved /dev path (/dev/hda1,
// /dev/mapper/root, /dev/dm-0) to its /dev/disk/by-uuid or /dev/disk/by-label
// entry value; they may be empty. `probedFsByPath` maps a /dev path to the
// udev-reported ID_FS_TYPE; it is read-only metadata and may be empty.
std::vector<DeviceRow> buildDeviceRows(
    const std::vector<PartitionRecord> &partitions,
    const std::map<std::string, MountRecord> &mounts,
    const std::map<std::string, std::string> &swaps,
    const std::vector<std::string> &diskNames,
    const std::map<std::string, std::string> &attributes,
    const std::map<std::string, std::string> &mapperLinks,
    const std::map<std::string, std::string> &uuidByPath =
        std::map<std::string, std::string>(),
    const std::map<std::string, std::string> &labelByPath =
        std::map<std::string, std::string>(),
    const std::map<std::string, std::string> &probedFsByPath =
        std::map<std::string, std::string>());

// Reads the live kernel metadata and returns the rows (read-only).
std::vector<DeviceRow> scanDevices();

// Best-effort read-only detection of the running host's root component and the
// physical disk that backs it (/proc/mounts + the /sys/block tree). Returns
// false when no /dev-backed root mount can be resolved. The helper's
// host-validate/host-diagnose output remains the authoritative confirmation.
bool detectRunningHostTarget(std::string *rootPath, std::string *diskPath);

// True when the kernel name looks like a whole disk (no trailing partition
// index, not a mapper/loop/ram device).
bool looksLikeWholeDisk(const std::string &name);

// Resolves a /dev/mapper entry to its dm kernel name (dm-0). Handles both the
// symlink layout and the Debian Etch udev layout, where /dev/mapper/<name> is
// a block device node whose major:minor maps to /sys/block/dm-N/dev
// (`deviceMap` maps "major:minor" to "dm-N"). Returns "" when unresolvable.
std::string mapperKernelName(const std::string &path,
                             const std::map<std::string, std::string> &deviceMap);

} // namespace legacy

#endif // LEGACY_DEVICE_INVENTORY_H
