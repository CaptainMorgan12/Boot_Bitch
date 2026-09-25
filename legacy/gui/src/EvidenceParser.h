// EvidenceParser - legacy helper transcript parsing and fail-closed capability
// gating for the Qt3 legacy frontend.
//
// This module is deliberately free of Qt dependencies (C++98 + libc/libstdc++)
// so it can be compiled and tested on a modern development host while the GUI
// itself only builds with Qt3 on the Etch guest. The semantics mirror
// src/MainWindow.cpp:
//   - `Repair tool <key>: available|unavailable|<reason>` decides availability,
//   - a missing, unknown or malformed line keeps the action disabled,
//   - `Legacy feature <feature>: available|unavailable|<reason>` decides the
//     legacy-only workflows (file copy, shell, host-shell, host-maintenance,
//     snapshots, host-default) with the same fail-closed rule,
//   - `Legacy config <key>: available|unavailable|<reason>` decides which
//     Etch target configuration files the GUI may offer for editing,
//   - `Repair change status <key>: unchanged|...` is a proven no-op, anything
//     else invalidates the cached diagnostics.
//
// Contract: Development/docs/debian-etch-legacy-plan.md sections 3.2 and 3.4.

#ifndef LEGACY_EVIDENCE_PARSER_H
#define LEGACY_EVIDENCE_PARSER_H

#include <map>
#include <string>
#include <vector>

namespace legacy {

// The 13 capability keys emitted by the shared helper (and gated by
// MainWindow::repairToolAvailable). Kept here as the single legacy-side list.
std::vector<std::string> capabilityKeys();

// The 16 read-only diagnostic keys accepted by `diagnose <key>` and
// `host-diagnose <key>` (the `report` key is the combined report).
std::vector<std::string> diagnosticKeys();

// The legacy feature keys emitted by the helper's read-only
// `Legacy feature <feature>:` gating report. Kept here as the single
// legacy-side list.
std::vector<std::string> legacyFeatureKeys();

// The Etch-era target configuration keys accepted by the legacy helper's
// guarded `config-read`/`config-write` verbs. The helper reports their
// per-target availability with read-only `Legacy config <key>:` lines during
// target diagnostics (never for the running host); the GUI uses that probe to
// grey/omit absent files with the helper's exact reason.
std::vector<std::string> configFileKeys();

// Stable mapping from helper repair stage to capability key, mirroring
// MainWindow::repairToolKeyForStage.
std::string repairToolKeyForStage(const std::string &stage);

struct ParsedTranscript {
    ParsedTranscript();

    // `Repair tool <key>: <state>` lines in order; state is the verbatim text
    // after the colon ("available" or "unavailable|<reason>").
    std::vector<std::pair<std::string, std::string> > capabilities;
    // `Repair capability evidence <key>: <text>` lines.
    std::vector<std::pair<std::string, std::string> > capabilityEvidence;
    // `Legacy feature <feature>: <state>` lines in order; state is the
    // verbatim text after the colon ("available" or "unavailable|<reason>").
    std::vector<std::pair<std::string, std::string> > legacyFeatures;
    // `Legacy config <key>: <state>` lines in order; state is the verbatim
    // text after the colon ("available" or "unavailable|<reason>").
    std::vector<std::pair<std::string, std::string> > configFiles;
    // `Repair change status <key>: <state>` lines in order.
    std::vector<std::pair<std::string, std::string> > changeStatuses;
    // Recognised target/validation fact lines, verbatim (trimmed).
    std::vector<std::string> targetFacts;
    // Unique /dev paths referenced by the helper output (device discovery).
    std::vector<std::string> devicePaths;
    // Number of `Repair tool` lines seen and how many had an unknown key.
    int capabilityLineCount;
    int unknownCapabilityLineCount;
    // Number of `Legacy feature` lines seen and how many had an unknown key.
    int legacyFeatureLineCount;
    int unknownLegacyFeatureLineCount;
    // Number of `Legacy config` lines seen and how many had an unknown key.
    int configFileLineCount;
    int unknownConfigFileLineCount;
};

// Parses a helper transcript; tolerant, never throws.
ParsedTranscript parseTranscript(const std::string &text);

// True only for the exact state "available". Otherwise the reason is the text
// after "unavailable|" (or the whole state for unrecognised states).
bool capabilityIsAvailable(const std::string &state, std::string *reason);

// Legacy feature state helper with the same fail-closed semantics as
// capabilityIsAvailable: true only for the exact state "available".
bool legacyFeatureIsAvailable(const std::string &state, std::string *reason);

// True for "unchanged" and "unchanged|<reason>" (proven no-op).
bool changeStatusIsUnchanged(const std::string &status);


// LUKS unlock result helpers (legacy helper `unlock` command):
//   `UNLOCKED=<mapper-path>` on success (the mapper path is returned, "" when
//   the transcript carries no such line);
//   `UNLOCK_AUTH_FAILED=1` on a rejected passphrase (retry without treating it
//   as a generic failure). Neither helper ever sees a passphrase: the helper
//   prints only these markers.
std::string unlockMapper(const std::string &text);
std::string unlockRoot(const std::string &text);
std::string unlockRootFstype(const std::string &text);
std::string unlockRootUuid(const std::string &text);
bool unlockAuthFailed(const std::string &text);
// Last "ERROR:" line of a transcript, trimmed and with the prefix removed
// ("" when the transcript carries no error line). Used by the Systems tab's
// unlock status to name the helper's exact failure without a passphrase ever
// being present in the transcript.
std::string unlockErrorLine(const std::string &text);

// Cached capability/evidence model for one selected scope identity.
// Fail closed: availability requires a completed diagnostic run for the same
// identity, an exact `available` line and no invalidating change status since.
class CapabilityModel {
public:
    CapabilityModel();

    // Identity string for the currently selected scope, e.g.
    // "host|/dev/hda|/dev/mapper/root". Diagnostics for another
    // identity never unlock actions for this one.
    void beginDiagnostics(const std::string &identity);

    // Applies a completed diagnostic transcript. When `processOk` is false the
    // cached capabilities are cleared (fail closed).
    void applyDiagnosticTranscript(const std::string &identity,
                                   const std::string &text,
                                   bool processOk);

    // Records the change statuses of any completed helper command. A status
    // that is not proven `unchanged` invalidates the cached diagnostics.
    void applyCommandTranscript(const std::string &text);

    // Records one change status directly (used by tests).
    void noteChangeStatus(const std::string &key, const std::string &status);

    bool hasDiagnostics(const std::string &identity) const;

    // Fail-closed availability. `reason` always receives an explanation.
    bool isAvailable(const std::string &key, const std::string &identity,
                     std::string *reason) const;

    // Fail-closed availability for a legacy feature line (`file-copy`,
    // `shell`, `host-shell`, `host-maintenance`, `snapshots`,
    // `host-default`). A missing, unknown or unrecognised line keeps the
    // feature unavailable, exactly like isAvailable().
    bool legacyFeatureAvailable(const std::string &feature,
                                const std::string &identity,
                                std::string *reason) const;

    // Fail-closed availability for an Etch target configuration key from the
    // helper's read-only `Legacy config <key>:` probe. A missing, unknown or
    // unrecognised line keeps the file out of the editable list.
    bool configFileAvailable(const std::string &key, const std::string &identity,
                             std::string *reason) const;

    // Verbatim state line for a key ("" when absent).
    std::string state(const std::string &key) const;
    // Verbatim evidence line for a key ("" when absent).
    std::string evidence(const std::string &key) const;
    // Verbatim legacy feature state line ("" when absent).
    std::string legacyFeatureState(const std::string &feature) const;
    // Verbatim legacy configuration state line ("" when absent).
    std::string configFileState(const std::string &key) const;
    // Capability keys with an invalidating change status, in first-seen order.
    std::vector<std::string> invalidatingKeys() const;
    // Diagnostics must be regenerated (a non-unchanged status was seen).
    bool diagnosticsStale() const;

    void reset();

private:
    void clearCapabilities();
    void noteChangeStatusLocked(const std::string &key, const std::string &status);

    std::string m_identity;
    bool m_ran;
    bool m_stale;
    std::map<std::string, std::string> m_capabilities;
    std::map<std::string, std::string> m_evidence;
    std::map<std::string, std::string> m_legacyFeatures;
    std::map<std::string, std::string> m_configFiles;
    std::map<std::string, std::string> m_changes;
    std::vector<std::string> m_invalidatingKeys;
};

// Formats a human-readable size from a 1024-byte block count ("8.0G").
std::string formatSizeKb(unsigned long long blocks);

} // namespace legacy

#endif // LEGACY_EVIDENCE_PARSER_H
