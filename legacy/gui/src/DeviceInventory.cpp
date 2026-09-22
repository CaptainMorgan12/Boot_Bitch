// DeviceInventory implementation. C++98 + POSIX. See DeviceInventory.h.

#include "DeviceInventory.h"
#include "EvidenceParser.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>

namespace legacy {

namespace {

std::string trim(const std::string &text)
{
    std::string::size_type begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return std::string();
    }
    std::string::size_type end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

std::vector<std::string> splitWhitespace(const std::string &text)
{
    std::vector<std::string> fields;
    std::istringstream stream(text);
    std::string field;
    while (stream >> field) {
        fields.push_back(field);
    }
    return fields;
}

bool readFile(const std::string &path, std::string *content)
{
    std::ifstream input(path.c_str());
    if (!input) {
        return false;
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    *content = buffer.str();
    return true;
}

bool isSkippedName(const std::string &name)
{
    static const char *const prefixes[] = { "ram", "loop", "fd", "zram" };
    for (std::size_t i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i) {
        const std::size_t length = std::strlen(prefixes[i]);
        if (name.size() >= length && name.compare(0, length, prefixes[i]) == 0) {
            return true;
        }
    }
    return false;
}

bool isOpticalName(const std::string &name)
{
    return name.size() >= 2 && name.compare(0, 2, "sr") == 0;
}

unsigned long long parseULL(const std::string &text)
{
    if (text.empty()) {
        return 0;
    }
    return std::strtoull(text.c_str(), 0, 10);
}

std::string attributeValue(const std::map<std::string, std::string> &attributes,
                           const std::string &key)
{
    const std::map<std::string, std::string>::const_iterator it =
        attributes.find(key);
    return it == attributes.end() ? std::string() : trim(it->second);
}

// Strips the trailing partition index from a kernel name (hda1 -> hda,
// cciss/c0d0p1 -> cciss/c0d0, nvme0n1p1 -> nvme0n1). Returns "" when the name
// has no index.
std::string parentOf(const std::string &name)
{
    if (name.empty()) {
        return std::string();
    }
    std::string::size_type end = name.size();
    while (end > 0 && name[end - 1] >= '0' && name[end - 1] <= '9') {
        --end;
    }
    if (end == name.size()) {
        return std::string();
    }
    // "nvme0n1p1" / "mmcblk0p1": drop the trailing 'p' separator.
    if (end > 0 && (name[end - 1] == 'p' || name[end - 1] == '!')
        && end < name.size()) {
        --end;
    }
    if (end == 0) {
        return std::string();
    }
    return name.substr(0, end);
}

std::string mapperTargetFromIds(unsigned int majorNumber, unsigned int minorNumber,
                                const std::map<std::string, std::string> &deviceMap)
{
    char key[64];
    std::snprintf(key, sizeof(key), "%u:%u", majorNumber, minorNumber);
    const std::map<std::string, std::string>::const_iterator it =
        deviceMap.find(key);
    if (it == deviceMap.end() || it->second.compare(0, 3, "dm-") != 0) {
        return std::string();
    }
    return it->second;
}

std::vector<std::string> readMapperLinks(
    const std::map<std::string, std::string> &deviceMap,
    std::map<std::string, std::string> *targets)
{
    std::vector<std::string> names;
    DIR *dir = ::opendir("/dev/mapper");
    if (!dir) {
        return names;
    }
    struct dirent *entry = 0;
    while ((entry = ::readdir(dir)) != 0) {
        const std::string name = entry->d_name;
        if (name == "." || name == ".." || name == "control") {
            continue;
        }
        const std::string link = "/dev/mapper/" + name;
        const std::string target = mapperKernelName(link, deviceMap);
        if (target.empty()) {
            continue;
        }
        names.push_back(name);
        if (targets) {
            (*targets)[link] = target;
        }
    }
    ::closedir(dir);
    return names;
}

} // namespace

std::string mapperKernelName(const std::string &path,
                             const std::map<std::string, std::string> &deviceMap)
{
    struct stat info;
    if (::lstat(path.c_str(), &info) == 0 && S_ISLNK(info.st_mode)) {
        char buffer[512];
        const ssize_t length = ::readlink(path.c_str(), buffer, sizeof(buffer) - 1);
        if (length > 0) {
            buffer[length] = '\0';
            std::string target(buffer);
            const std::string::size_type slash = target.find_last_of('/');
            if (slash != std::string::npos) {
                target = target.substr(slash + 1);
            }
            if (target.compare(0, 3, "dm-") == 0) {
                return target;
            }
        }
        return std::string();
    }
    // Debian Etch udev creates /dev/mapper/<name> as a block device node; map
    // its major:minor through /sys/block/dm-N/dev.
    if (::stat(path.c_str(), &info) == 0 && S_ISBLK(info.st_mode)) {
        return mapperTargetFromIds(::major(info.st_rdev), ::minor(info.st_rdev),
                                   deviceMap);
    }
    return std::string();
}

PartitionRecord::PartitionRecord()
    : majorNumber(0), minorNumber(0), blocks(0)
{
}

MountRecord::MountRecord()
{
}

DeviceRow::DeviceRow()
    : disk(false), mapper(false), optical(false), blocks(0)
{
}

std::vector<PartitionRecord> parseProcPartitions(const std::string &text)
{
    std::vector<PartitionRecord> records;
    std::istringstream stream(text);
    std::string line;
    bool headerSeen = false;
    while (std::getline(stream, line)) {
        const std::vector<std::string> fields = splitWhitespace(line);
        if (fields.empty()) {
            continue;
        }
        if (!headerSeen) {
            // First non-empty line is the "major minor #blocks name" header.
            headerSeen = true;
            if (fields[0] == "major") {
                continue;
            }
        }
        if (fields.size() < 4) {
            continue;
        }
        PartitionRecord record;
        record.majorNumber = std::atoi(fields[0].c_str());
        record.minorNumber = std::atoi(fields[1].c_str());
        record.blocks = parseULL(fields[2]);
        record.name = fields[3];
        if (record.name.empty()) {
            continue;
        }
        records.push_back(record);
    }
    return records;
}

std::map<std::string, MountRecord> parseProcMounts(const std::string &text)
{
    std::map<std::string, MountRecord> mounts;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        const std::vector<std::string> fields = splitWhitespace(line);
        if (fields.size() < 4) {
            continue;
        }
        MountRecord record;
        record.source = fields[0];
        record.target = fields[1];
        record.fstype = fields[2];
        record.options = fields[3];
        // Keep the first mount for a source: later entries are usually bind
        // mounts (for example /dev/.static/dev) that must not shadow the real
        // root mount when the GUI looks for "/".
        if (mounts.find(record.source) == mounts.end()) {
            mounts[record.source] = record;
        }
    }
    return mounts;
}

std::map<std::string, std::string> parseProcSwaps(const std::string &text)
{
    std::map<std::string, std::string> swaps;
    std::istringstream stream(text);
    std::string line;
    bool headerSeen = false;
    while (std::getline(stream, line)) {
        const std::vector<std::string> fields = splitWhitespace(line);
        if (fields.empty()) {
            continue;
        }
        if (!headerSeen) {
            headerSeen = true;
            if (fields[0] == "Filename") {
                continue;
            }
        }
        if (fields.size() < 2 || fields[0] == "Filename") {
            continue;
        }
        swaps[fields[0]] = fields[1];
    }
    return swaps;
}

bool isMdName(const std::string &name)
{
    if (name.size() < 3 || name.compare(0, 2, "md") != 0) {
        return false;
    }
    for (std::string::size_type i = 2; i < name.size(); ++i) {
        if (name[i] < '0' || name[i] > '9') {
            return false;
        }
    }
    return true;
}

bool looksLikeWholeDisk(const std::string &name)
{
    if (name.empty() || isSkippedName(name)) {
        return false;
    }
    if (name.compare(0, 3, "dm-") == 0) {
        return false;
    }
    if (isMdName(name) || isOpticalName(name)) {
        return true;
    }
    return parentOf(name).empty();
}

std::vector<DeviceRow> buildDeviceRows(
    const std::vector<PartitionRecord> &partitions,
    const std::map<std::string, MountRecord> &mounts,
    const std::map<std::string, std::string> &swaps,
    const std::vector<std::string> &diskNames,
    const std::map<std::string, std::string> &attributes,
    const std::map<std::string, std::string> &mapperLinks)
{
    std::vector<DeviceRow> rows;
    std::map<std::string, bool> diskSet;
    for (std::size_t i = 0; i < diskNames.size(); ++i) {
        diskSet[diskNames[i]] = true;
    }

    for (std::size_t i = 0; i < partitions.size(); ++i) {
        const PartitionRecord &record = partitions[i];
        if (isSkippedName(record.name)) {
            continue;
        }
        const bool isDisk = diskSet.find(record.name) != diskSet.end()
            || looksLikeWholeDisk(record.name);
        if (record.name.compare(0, 3, "dm-") == 0 && !isDisk) {
            continue; // mapper devices are listed from /dev/mapper below
        }
        DeviceRow row;
        row.name = record.name;
        row.path = "/dev/" + record.name;
        row.disk = isDisk;
        row.optical = isOpticalName(record.name);
        if (!isDisk) {
            row.parent = parentOf(record.name);
        }
        const std::string sectors =
            attributeValue(attributes, record.name + "/size");
        if (!sectors.empty()) {
            row.blocks = parseULL(sectors) / 2; // 512-byte sectors -> KiB
        } else {
            row.blocks = record.blocks;
        }
        row.size = formatSizeKb(row.blocks);
        if (isDisk) {
            row.model = attributeValue(attributes, record.name + "/device/model");
        }
        const std::map<std::string, MountRecord>::const_iterator mount =
            mounts.find(row.path);
        if (mount != mounts.end()) {
            row.fstype = mount->second.fstype;
            row.mountpoint = mount->second.target;
        } else {
            const std::map<std::string, std::string>::const_iterator swap =
                swaps.find(row.path);
            if (swap != swaps.end()) {
                row.fstype = "swap";
                row.mountpoint = "[swap]";
            }
        }
        rows.push_back(row);
    }

    for (std::map<std::string, std::string>::const_iterator it = mapperLinks.begin();
         it != mapperLinks.end(); ++it) {
        DeviceRow row;
        row.mapper = true;
        row.disk = false;
        row.name = it->first.substr(std::strlen("/dev/mapper/"));
        row.path = it->first;
        // Kernel 2.6.18 has no dm/name but exposes the backing device through
        // dm-N/slaves; use it to name the parent partition when available.
        row.parent = attributeValue(attributes, it->second + "/slave");
        const std::string sectors = attributeValue(attributes, it->second + "/size");
        if (!sectors.empty()) {
            row.blocks = parseULL(sectors) / 2;
        }
        row.size = formatSizeKb(row.blocks);
        const std::map<std::string, MountRecord>::const_iterator mount =
            mounts.find(row.path);
        if (mount != mounts.end()) {
            row.fstype = mount->second.fstype;
            row.mountpoint = mount->second.target;
        } else {
            const std::map<std::string, std::string>::const_iterator swap =
                swaps.find(row.path);
            if (swap != swaps.end()) {
                row.fstype = "swap";
                row.mountpoint = "[swap]";
            }
        }
        rows.push_back(row);
    }
    return rows;
}

std::vector<DeviceRow> scanDevices()
{
    std::vector<PartitionRecord> partitions;
    std::string content;
    if (readFile("/proc/partitions", &content)) {
        partitions = parseProcPartitions(content);
    }
    std::map<std::string, MountRecord> mounts;
    if (readFile("/proc/mounts", &content)) {
        mounts = parseProcMounts(content);
    }
    std::map<std::string, std::string> swaps;
    if (readFile("/proc/swaps", &content)) {
        swaps = parseProcSwaps(content);
    }

    std::vector<std::string> diskNames;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> deviceMap;
    DIR *dir = ::opendir("/sys/block");
    if (dir) {
        struct dirent *entry = 0;
        while ((entry = ::readdir(dir)) != 0) {
            const std::string name = entry->d_name;
            if (name == "." || name == ".." || isSkippedName(name)
                || name.compare(0, 3, "dm-") == 0) {
                // Record dm-N major:minor so /dev/mapper device nodes (the
                // Debian Etch udev layout) can be resolved.
                if (name.compare(0, 3, "dm-") == 0) {
                    std::string devValue;
                    if (readFile("/sys/block/" + name + "/dev", &devValue)) {
                        deviceMap[trim(devValue)] = name;
                    }
                }
                continue;
            }
            diskNames.push_back(name);
            std::string value;
            if (readFile("/sys/block/" + name + "/size", &value)) {
                attributes[name + "/size"] = value;
            }
            if (readFile("/sys/block/" + name + "/device/model", &value)) {
                attributes[name + "/device/model"] = value;
            }
            // Partitions are not separate /sys/block entries on 2.6.18, so
            // read their size attributes from the parent directory.
            DIR *partDir = ::opendir(("/sys/block/" + name).c_str());
            if (partDir) {
                struct dirent *partEntry = 0;
                while ((partEntry = ::readdir(partDir)) != 0) {
                    const std::string part = partEntry->d_name;
                    if (part == "." || part == "..") {
                        continue;
                    }
                    if (part.compare(0, 3, "dm-") == 0) {
                        continue;
                    }
                    if (readFile("/sys/block/" + name + "/" + part + "/size", &value)) {
                        attributes[part + "/size"] = value;
                    }
                }
                ::closedir(partDir);
            }
        }
        ::closedir(dir);
    }

    std::map<std::string, std::string> mapperLinks;
    const std::vector<std::string> mapperNames =
        readMapperLinks(deviceMap, &mapperLinks);
    for (std::size_t i = 0; i < mapperNames.size(); ++i) {
        std::string value;
        const std::map<std::string, std::string>::const_iterator target =
            mapperLinks.find("/dev/mapper/" + mapperNames[i]);
        if (target == mapperLinks.end()) {
            continue;
        }
        if (readFile("/sys/block/" + target->second + "/size", &value)) {
            attributes[target->second + "/size"] = value;
        }
        DIR *slaveDir =
            ::opendir(("/sys/block/" + target->second + "/slaves").c_str());
        if (slaveDir) {
            struct dirent *slave = 0;
            while ((slave = ::readdir(slaveDir)) != 0) {
                const std::string name = slave->d_name;
                if (name == "." || name == "..") {
                    continue;
                }
                attributes[target->second + "/slave"] = name;
                break;
            }
            ::closedir(slaveDir);
        }
    }

    return buildDeviceRows(partitions, mounts, swaps, diskNames, attributes,
                           mapperLinks);
}

bool detectRunningHostTarget(std::string *rootPath, std::string *diskPath)
{
    std::string content;
    if (!readFile("/proc/mounts", &content)) {
        return false;
    }
    // Scan the raw lines for the effective "/" mount; the source-keyed map
    // keeps only one entry per source and a bind mount can hide the root one.
    std::string root;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        const std::vector<std::string> fields = splitWhitespace(line);
        if (fields.size() >= 3 && fields[1] == "/"
            && fields[0].compare(0, 5, "/dev/") == 0) {
            root = fields[0];
            break;
        }
    }
    if (root.empty()) {
        return false;
    }

    const std::vector<DeviceRow> rows = scanDevices();
    std::string disk;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].path != root) {
            continue;
        }
        if (rows[i].disk) {
            disk = rows[i].name;
        } else if (!rows[i].parent.empty()) {
            const std::string parent = rows[i].parent;
            for (std::size_t j = 0; j < rows.size(); ++j) {
                if (rows[j].disk && parent.compare(0, rows[j].name.size(),
                                                   rows[j].name) == 0) {
                    disk = rows[j].name;
                    break;
                }
            }
            if (disk.empty()) {
                // The parent may itself be a partition row (hda5 -> hda).
                for (std::size_t j = 0; j < rows.size(); ++j) {
                    if (rows[j].name == parent && !rows[j].parent.empty()) {
                        const std::string grandParent = rows[j].parent;
                        for (std::size_t k = 0; k < rows.size(); ++k) {
                            if (rows[k].disk
                                && grandParent.compare(0, rows[k].name.size(),
                                                       rows[k].name) == 0) {
                                disk = rows[k].name;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
        break;
    }
    if (disk.empty()) {
        for (std::size_t i = 0; i < rows.size(); ++i) {
            if (rows[i].disk && !rows[i].optical) {
                disk = rows[i].name;
                break;
            }
        }
    }
    if (disk.empty()) {
        return false;
    }
    if (rootPath) {
        *rootPath = root;
    }
    if (diskPath) {
        *diskPath = "/dev/" + disk;
    }
    return true;
}

} // namespace legacy
