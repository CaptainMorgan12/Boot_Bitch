// HelperRunner - QProcess wrapper for the ported legacy helper.
//
// Runs /usr/sbin/boot-repair-legacy-helper (or a configured path) with an
// optional elevation prefix (sudo/gksu), streams merged stdout+stderr as lines
// and reports the exit status. No secret is ever placed on the command line.
//
// Qt3 (qprocess.h) notes: the program is the first argument added to the
// process; there is no separate setProgram(); `launchFinished()` only fires for
// a successful spawn, so a false `start()` return is reported directly.

#ifndef LEGACY_HELPER_RUNNER_H
#define LEGACY_HELPER_RUNNER_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

class QProcess;

namespace legacy {

class HelperRunner : public QObject
{
    Q_OBJECT
public:
    HelperRunner(QObject *parent = 0, const char *name = 0);
    ~HelperRunner();

    // Installed helper path, BOOT_REPAIR_LEGACY_HELPER and the source-tree
    // fallbacks are resolved by the caller (main.cpp / LegacyMainWindow).
    void setHelperPath(const QString &path);
    QString helperPath() const;

    // Explicit elevation prefix override (BOOT_REPAIR_LEGACY_ELEVATE). Empty
    // means auto-detect. The literal "none" forces direct execution.
    void setElevationOverride(const QString &override);
    void setNoElevate(bool noElevate);

    // Detects the elevation method once; fills `description` with the human
    // readable choice ("root", "sudo -n", "gksu --sudo-mode", ...).
    bool resolveElevation(QString *description);

    bool isRunning() const;

    // Starts the helper with `helperArgs`. Returns false (and emits an error
    // line) when the helper cannot be started.
    bool run(const QStringList &helperArgs);

    void cancel();

signals:
    // One complete output line (merged stdout/stderr, newline stripped).
    void outputLine(const QString &line);
    // Process finished. ok is true only for a normal exit with code 0.
    void finished(bool ok, int exitCode);

private slots:
    void readOutput();
    void processExited();

private:
    void flushBufferedLines();
    void emitErrorLine(const QString &text);

    QProcess *m_process;
    QString m_helperPath;
    QString m_elevationOverride;
    bool m_noElevate;
    bool m_resolved;
    bool m_direct;
    QStringList m_prefix;
    QString m_description;
    QString m_buffer;
    bool m_reported;
};

} // namespace legacy

#endif // LEGACY_HELPER_RUNNER_H
