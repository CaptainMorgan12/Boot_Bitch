#pragma once

#include <QList>
#include <QString>

// One host capability probe: the feature it represents, the command that must
// be present, the distribution package that provides it and where it is needed
// (host, repair target or both). Commands are located with a portable PATH
// fallback so AppImage and minimal hosts behave consistently.
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
};

// Read-only inventory of the tools the running host and the repair workflows
// need. The scan never executes a probed command; it only locates it on disk.
class CapabilityChecker
{
public:
    static QList<Capability> scanHost();
    static QString distributionLabel();
    static QString packageManagerLabel();

private:
    static QString distributionId();
    static QString packageForCommand(const QString &command, const QString &distributionId);
};
