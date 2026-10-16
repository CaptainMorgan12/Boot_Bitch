#pragma once

#include <QList>
#include <QString>

// One capability row in the Boot capabilities table: the feature it represents,
// the command/tool it names, the suggested package that provides it and where
// it is needed (host, repair target or both). Rows are parsed from the cached
// diagnostic log (single source of truth); nothing here re-probes the
// filesystem. executablePath is retained for struct compatibility but is no
// longer populated (availability comes from the cache, not a PATH lookup).
struct Capability
{
    QString feature;
    QString command;
    QString packageName;
    QString scope;
    QString note;
    QString executablePath;
    bool available = false;
    bool optional = true;
    // True when the scope involves the repair target (any scope other than the
    // host-only "Host"), which routes the Boot capabilities table Install
    // action to the guarded install-target-package helper verb; false routes
    // it to the guarded install-host-package verb (Host Maintenance only).
    bool targetScope = false;
    // True when the "Suggested package" column names a concrete package that
    // can actually be installed (false for "Unknown" / "Distribution-specific"
    // / "Not available on Alpine" placeholders).
    bool installable = false;
};

// Read-only inventory of the tools the running host and the repair workflows
// need. Every capability row is now parsed from the cached diagnostic log
// (single source of truth): the Boot capabilities table never re-probes the
// filesystem, and a probe is never executed against a probed command.
class CapabilityChecker
{
public:
    // Parses the cached capability preamble (the `Repair tool <key>` gating
    // lines plus the `Repair capability evidence <key>` lines) and the cached
    // backend section's `Module build system:` line into one Capability per
    // repair tool. Availability comes from the cache; the module-build row's
    // suggested package maps to dkms/akmods/akms per the detected backend
    // (never the host distro). Both the repair-target and the running-host
    // scopes derive from the same preamble: scopeLabel overrides the "Repair
    // target" scope text (e.g. "Host") and targetScope selects the install
    // routing (guarded install into the selected target vs. guarded install
    // on the running host).
    static QList<Capability> capabilitiesFromRepairEvidence(const QString &capabilitiesPreamble,
                                                            const QString &backendSection,
                                                            const QString &scopeLabel = QString(),
                                                            bool targetScope = true);
    static QString distributionLabel();
    static QString packageManagerLabel();

private:
    // Names the package providing the module-build system for a detected
    // backend token (dkms/akmods/akms), defaulting to dkms.
    static QString moduleBuildSystemPackageForBackend(const QString &backend);
    // Names the native module-build system package for a target distribution
    // family (fedora/rhel -> akmods, alpine -> akms, else dkms).
    static QString moduleBuildSystemPackageForFamily(const QString &family);
};
