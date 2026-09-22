// EvidenceParser implementation. C++98, no Qt. See EvidenceParser.h.

#include "EvidenceParser.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>

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

std::vector<std::string> splitLines(const std::string &text)
{
    std::vector<std::string> lines;
    std::string current;
    for (std::string::size_type i = 0; i < text.size(); ++i) {
        if (text[i] == '\n') {
            lines.push_back(current);
            current.clear();
        } else if (text[i] != '\r') {
            current += text[i];
        }
    }
    if (!current.empty()) {
        lines.push_back(current);
    }
    return lines;
}

// True for the key alphabet of the evidence contract ([A-Za-z0-9._-]).
bool validKey(const std::string &key)
{
    if (key.empty()) {
        return false;
    }
    for (std::string::size_type i = 0; i < key.size(); ++i) {
        const char c = key[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
              || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) {
            return false;
        }
    }
    return true;
}

bool startsWith(const std::string &text, const char *prefix)
{
    const std::size_t length = std::strlen(prefix);
    return text.size() >= length && text.compare(0, length, prefix) == 0;
}

bool parseEvidenceLine(const std::string &line, const char *prefix,
                       std::string *key, std::string *value)
{
    if (!startsWith(line, prefix)) {
        return false;
    }
    const std::string rest = line.substr(std::strlen(prefix));
    const std::string::size_type colon = rest.find(": ");
    if (colon == std::string::npos) {
        return false;
    }
    const std::string candidate = rest.substr(0, colon);
    if (!validKey(candidate)) {
        return false;
    }
    *key = candidate;
    *value = trim(rest.substr(colon + 2));
    return true;
}

bool isDevicePathStart(const std::string &token)
{
    if (startsWith(token, "/dev/")) {
        return true;
    }
    // lsblk NAME columns print kernel names without the /dev prefix.
    static const char *const prefixes[] = {
        "hd", "sd", "vd", "xvd", "nvme", "mmcblk", "dm-", "md", "cciss!"
    };
    for (std::size_t i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i) {
        if (startsWith(token, prefixes[i])) {
            // Require a digit so prose words ("hard", "sda" is fine) cannot
            // match; kernel names always carry an index.
            for (std::string::size_type j = std::strlen(prefixes[i]); j < token.size(); ++j) {
                if (token[j] >= '0' && token[j] <= '9') {
                    return true;
                }
                if (!((token[j] >= 'a' && token[j] <= 'z') || token[j] == '!')) {
                    break;
                }
            }
            return false;
        }
    }
    return false;
}

void addDevicePath(std::vector<std::string> *paths, std::set<std::string> *seen,
                   const std::string &token)
{
    std::string path = token;
    // Trim punctuation that can trail a path in prose.
    while (!path.empty()) {
        const char last = path[path.size() - 1];
        if (last == ',' || last == ')' || last == '.' || last == ';') {
            path.erase(path.size() - 1);
        } else {
            break;
        }
    }
    if (path.empty()) {
        return;
    }
    if (!startsWith(path, "/dev/")) {
        path = "/dev/" + path;
    }
    if (seen->insert(path).second) {
        paths->push_back(path);
    }
}

bool isFactLabel(const std::string &line)
{
    static const char *const labels[] = {
        "System:", "Physical drive:", "Detected component:", "Root subvolume:",
        "Filesystem:", "Inspection mode:", "Detected target OS:",
        "Target root mount:", "Distribution family:", "Package manager backend:",
        "Package manager backends:", "Initramfs backend:", "Initramfs backends:",
        "Bootloader backend:", "Service manager:", "ESP mount candidate:",
        "Supported modifying backend:", "Repair capability:", "Kernel layout:",
        "Logging backend:", "Display manager backend:", "Backend note:",
        "OS:", "Root:", "Root fs:", "Root filesystem:", "Root mount source:",
        "GRUB config:", "GRUB tools:"
    };
    for (std::size_t i = 0; i < sizeof(labels) / sizeof(labels[0]); ++i) {
        if (startsWith(line, labels[i])) {
            return true;
        }
    }
    // "<Scope> root mount source:" / "<Scope> root mount options:".
    if (line.find(" root mount source:") != std::string::npos
        || line.find(" root mount options:") != std::string::npos) {
        return true;
    }
    return false;
}

} // namespace

std::vector<std::string> capabilityKeys()
{
    static const char *const keys[] = {
        "validate", "filesystem", "dpkg", "fixbroken", "aptupdate", "upgrade",
        "dkms", "display", "initramfs", "efi", "grub", "extlinux", "bootstack"
    };
    std::vector<std::string> result;
    for (std::size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        result.push_back(keys[i]);
    }
    return result;
}

std::string repairToolKeyForStage(const std::string &stage)
{
    if (stage == "dpkg-configure") return "dpkg";
    if (stage == "fix-broken") return "fixbroken";
    if (stage == "apt-update") return "aptupdate";
    if (stage == "apt-upgrade") return "upgrade";
    if (stage == "display-manager") return "display";
    if (stage == "boot-stack") return "bootstack";
    return stage;
}

ParsedTranscript::ParsedTranscript()
    : capabilityLineCount(0), unknownCapabilityLineCount(0)
{
}

ParsedTranscript parseTranscript(const std::string &text)
{
    ParsedTranscript parsed;
    const std::vector<std::string> lines = splitLines(text);
    const std::vector<std::string> keys = capabilityKeys();
    std::set<std::string> knownKeys(keys.begin(), keys.end());
    std::set<std::string> seenPaths;

    for (std::size_t i = 0; i < lines.size(); ++i) {
        const std::string line = trim(lines[i]);
        if (line.empty()) {
            continue;
        }
        std::string key;
        std::string value;
        if (parseEvidenceLine(line, "Repair tool ", &key, &value)) {
            ++parsed.capabilityLineCount;
            if (knownKeys.find(key) == knownKeys.end()) {
                ++parsed.unknownCapabilityLineCount;
            }
            parsed.capabilities.push_back(std::make_pair(key, value));
            continue;
        }
        if (parseEvidenceLine(line, "Repair capability evidence ", &key, &value)) {
            parsed.capabilityEvidence.push_back(std::make_pair(key, value));
            continue;
        }
        if (parseEvidenceLine(line, "Repair change status ", &key, &value)) {
            parsed.changeStatuses.push_back(std::make_pair(key, value));
            continue;
        }
        if (isFactLabel(line)) {
            parsed.targetFacts.push_back(line);
            const std::string::size_type colon = line.find(": ");
            if (colon != std::string::npos) {
                std::string token = trim(line.substr(colon + 2));
                const std::string::size_type space = token.find_first_of(" \t");
                if (space != std::string::npos) {
                    token = token.substr(0, space);
                }
                if (isDevicePathStart(token)) {
                    addDevicePath(&parsed.devicePaths, &seenPaths, token);
                }
            }
            continue;
        }
        // Device rows from findmnt/lsblk output: first token starts with a
        // device path (only when the line looks like a listing, i.e. has more
        // than one whitespace-separated token).
        const std::string::size_type space = line.find_first_of(" \t");
        if (space != std::string::npos) {
            const std::string token = line.substr(0, space);
            if (isDevicePathStart(token)
                && (line.find('/') == 0 || startsWith(line, "/dev/"))) {
                addDevicePath(&parsed.devicePaths, &seenPaths, token);
            }
        }
    }
    return parsed;
}

bool capabilityIsAvailable(const std::string &state, std::string *reason)
{
    if (state == "available") {
        if (reason) {
            reason->clear();
        }
        return true;
    }
    std::string text;
    if (startsWith(state, "unavailable|")) {
        text = state.substr(std::strlen("unavailable|"));
    } else if (!state.empty()) {
        text = "unrecognised capability state '" + state + "'";
    } else {
        text = "no capability line was emitted";
    }
    if (reason) {
        *reason = text;
    }
    return false;
}

bool changeStatusIsUnchanged(const std::string &status)
{
    return status == "unchanged" || startsWith(status, "unchanged|");
}

CapabilityModel::CapabilityModel()
    : m_ran(false), m_stale(false)
{
}

void CapabilityModel::clearCapabilities()
{
    m_capabilities.clear();
    m_evidence.clear();
}

void CapabilityModel::reset()
{
    m_identity.clear();
    m_ran = false;
    m_stale = false;
    clearCapabilities();
    m_changes.clear();
    m_invalidatingKeys.clear();
}

void CapabilityModel::beginDiagnostics(const std::string &identity)
{
    m_identity = identity;
}

void CapabilityModel::applyDiagnosticTranscript(const std::string &identity,
                                                const std::string &text,
                                                bool processOk)
{
    m_identity = identity;
    m_ran = false;
    clearCapabilities();
    if (!processOk) {
        return;
    }
    const ParsedTranscript parsed = parseTranscript(text);
    if (parsed.capabilityLineCount == 0) {
        return;
    }
    for (std::size_t i = 0; i < parsed.capabilities.size(); ++i) {
        // A later line for the same key overrides an earlier one, exactly like
        // the modern GUI's cached capability scan.
        m_capabilities[parsed.capabilities[i].first] = parsed.capabilities[i].second;
    }
    for (std::size_t i = 0; i < parsed.capabilityEvidence.size(); ++i) {
        m_evidence[parsed.capabilityEvidence[i].first] = parsed.capabilityEvidence[i].second;
    }
    m_ran = true;
    m_stale = false;
}

void CapabilityModel::noteChangeStatusLocked(const std::string &key,
                                             const std::string &status)
{
    m_changes[key] = status;
    if (!changeStatusIsUnchanged(status)) {
        m_stale = true;
        for (std::size_t i = 0; i < m_invalidatingKeys.size(); ++i) {
            if (m_invalidatingKeys[i] == key) {
                return;
            }
        }
        m_invalidatingKeys.push_back(key);
    }
}

void CapabilityModel::applyCommandTranscript(const std::string &text)
{
    const ParsedTranscript parsed = parseTranscript(text);
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        noteChangeStatusLocked(parsed.changeStatuses[i].first,
                               parsed.changeStatuses[i].second);
    }
}

void CapabilityModel::noteChangeStatus(const std::string &key,
                                       const std::string &status)
{
    noteChangeStatusLocked(key, status);
}

bool CapabilityModel::hasDiagnostics(const std::string &identity) const
{
    return m_ran && m_identity == identity;
}

bool CapabilityModel::isAvailable(const std::string &key,
                                  const std::string &identity,
                                  std::string *reason) const
{
    const std::vector<std::string> keys = capabilityKeys();
    bool known = false;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == key) {
            known = true;
            break;
        }
    }
    if (!known) {
        if (reason) {
            *reason = "Unknown repair tool key '" + key + "'; fail closed.";
        }
        return false;
    }
    if (!m_ran || m_identity != identity) {
        if (reason) {
            *reason = "Run diagnostics for the selected scope first.";
        }
        return false;
    }
    if (m_stale) {
        if (reason) {
            std::string names;
            for (std::size_t i = 0; i < m_invalidatingKeys.size(); ++i) {
                if (!names.empty()) {
                    names += ", ";
                }
                names += m_invalidatingKeys[i];
            }
            *reason = "Diagnostics are stale after a repair that was not proven "
                      "unchanged";
            if (!names.empty()) {
                *reason += " (" + names + ")";
            }
            *reason += "; run diagnostics again.";
        }
        return false;
    }
    const std::map<std::string, std::string>::const_iterator it =
        m_capabilities.find(key);
    if (it == m_capabilities.end()) {
        if (reason) {
            *reason = "No 'Repair tool " + key + ":' line was cached; run "
                      "diagnostics for the selected scope.";
        }
        return false;
    }
    return capabilityIsAvailable(it->second, reason);
}

std::string CapabilityModel::state(const std::string &key) const
{
    const std::map<std::string, std::string>::const_iterator it =
        m_capabilities.find(key);
    return it == m_capabilities.end() ? std::string() : it->second;
}

std::string CapabilityModel::evidence(const std::string &key) const
{
    const std::map<std::string, std::string>::const_iterator it =
        m_evidence.find(key);
    return it == m_evidence.end() ? std::string() : it->second;
}

std::vector<std::string> CapabilityModel::invalidatingKeys() const
{
    return m_invalidatingKeys;
}

bool CapabilityModel::diagnosticsStale() const
{
    return m_stale;
}

std::string formatSizeKb(unsigned long long blocks)
{
    char buffer[64];
    const double kb = static_cast<double>(blocks);
    if (kb >= 1024.0 * 1024.0) {
        std::snprintf(buffer, sizeof(buffer), "%.1fG", kb / (1024.0 * 1024.0));
    } else if (kb >= 1024.0) {
        std::snprintf(buffer, sizeof(buffer), "%.1fM", kb / 1024.0);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%lluK", blocks);
    }
    return std::string(buffer);
}

} // namespace legacy
