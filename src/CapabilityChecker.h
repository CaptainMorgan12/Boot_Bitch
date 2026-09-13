#pragma once

#include <QString>
#include <QList>

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
