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

    // True when the resolved elevation is a plain `sudo` that will ask for a
    // password. The GUI prompts for it in a modal hidden-input dialog and calls
    // authenticateElevation(); `sudo -n`, gksu/gksudo, su and direct/root
    // execution never need the password here (gksu/gksudo prompt themselves).
    bool elevationNeedsPassword(QString *description = 0);

    // True when a privileged command can start right now without a password
    // prompt: direct execution, gksu/gksudo (they prompt themselves), a
    // `sudo -n` prefix that still passes its own non-interactive probe, or a
    // plain-sudo session whose timestamp is still valid. The plain-sudo case
    // probes `sudo -S -v` with stdin closed so an expired timestamp fails fast
    // (EOF) instead of blocking on a password read; it never prompts and never
    // logs a secret. This is the GUI's pre-command expiry check.
    bool sessionIsCurrent();

    // Best-effort `sudo -k`: drops the cached sudo timestamp without ever
    // prompting or blocking (stdin is closed and output discarded). Returns
    // false when sudo is not installed or the call failed; callers must not
    // treat that as a session state.
    bool clearSudoTimestamp();

    // Authenticates the cached interactive `sudo` with `secret` by running
    // `sudo -S -p '' -v`; the password travels over a pipe, never argv/env and
    // never into the helper transcript. On success the cached prefix becomes
    // `sudo -n` when the installed sudo supports it (modern hosts), otherwise
    // plain `sudo`, which runs non-interactively while the cached timestamp is
    // valid (Etch's sudo 1.6.8 has no -n). `error` receives sudo's own stderr
    // line on failure. SIGPIPE is ignored only for the password write: sudo
    // exits without reading stdin when its timestamp is already valid, and the
    // resulting EPIPE must not terminate the GUI.
    bool authenticateElevation(const QByteArray &secret, QString *error = 0);

    // Drops the cached elevation decision so the next resolveElevation()/run()
    // probes the environment again (the GUI's "re-check elevation" control).
    void resetElevation();

    // Secret input for the next run(): written to the helper's standard input
    // and closed immediately after start. Used for the LUKS passphrase, which
    // is never placed in command arguments or logs. Cleared after use.
    void setInputData(const QByteArray &data);

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
    // True after authenticateElevation() succeeded, so a plain-sudo prefix is
    // not prompted again while its cached timestamp is valid.
    bool m_authenticated;
    QStringList m_prefix;
    QString m_description;
    QString m_buffer;
    QByteArray m_input;
    bool m_reported;
};

} // namespace legacy

#endif // LEGACY_HELPER_RUNNER_H
