// HelperRunner - QProcess wrapper for the ported legacy helper.
//
// Runs /usr/sbin/boot-repair-legacy-helper (or a configured path) with an
// optional elevation prefix (sudo/gksu), streams merged stdout+stderr as lines
// and reports the exit status. No secret is ever placed on the command line.
//
// Elevation trust (A10-03/A10-04): sudo/gksu/gksudo resolve ONLY from the
// fixed trusted system locations (/usr/bin/sudo, /usr/local/bin/sudo,
// /bin/sudo, /usr/bin/gksu, /usr/bin/gksudo); every candidate must be an
// lstat-verified regular file owned by root, not group/world-writable, in a
// root-owned, non-group/world-writable directory. The verified absolute path
// is used in every probe and exec (never execvp with a bare name, never
// PATH). Before any elevation the resolved helper is verified the same way
// (regular, no symlink, uid 0, not group/world-writable, realpath == path),
// so only an installed helper can be elevated; --no-elevate/root runs keep
// the previous semantics.
//
// Cancellation (A10-05): when setCancelFile() was given a path, run() passes
// --cancel-file <path> to the helper and cancel() touches that file
// (existence signal) before the existing direct kill; the helper aborts at
// its next stage boundary or shell-command tick.
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

    // A10-04: verifies a helper candidate for ELEVATION: an absolute path
    // that lstats as a regular file (symlinks fail), owned by root, not
    // group/world-writable, whose resolved realpath equals the raw path.
    // `reason` receives the first failing property.  Used by run() before
    // every elevated exec and printed by --print-config.
    static bool verifyHelperForElevation(const QString &path, QString *reason = 0);

    // A10-03: verifies one elevation tool candidate: an absolute path that
    // lstats as a regular file (symlinks fail), owned by root, not
    // group/world-writable, with a root-owned parent directory that is not
    // group/world-writable.
    static bool verifyTrustedToolPath(const QString &path, QString *reason = 0);

    // A10-05: cancel-token file.  When set, run() passes --cancel-file <path>
    // to the helper and cancel() touches the file (existence signal) before
    // the direct-kill fallback.
    void setCancelFile(const QString &path);
    QString cancelFile() const;

    // Detects the elevation method once; fills `description` with the human
    // readable choice ("root", "/usr/bin/sudo -n", "/usr/bin/gksu
    // --sudo-mode", ...). Fails closed with
    // "no trusted sudo/gksu found in the fixed system locations" when no
    // verified tool exists.
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
    // false when the trusted sudo is not available or the call failed;
    // callers must not treat that as a session state.
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
    // line) when the helper cannot be started or cannot be verified for
    // elevation.
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
    // Resolves the verified absolute paths of the elevation tools once
    // (m_sudoPath/m_gksuPath/m_gksudoPath, empty = not verifiable).
    void resolveTrustedTools();

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
    // A10-03: verified absolute elevation tool paths (empty when no fixed
    // system location verifies).
    QString m_sudoPath;
    QString m_gksuPath;
    QString m_gksudoPath;
    bool m_toolsResolved;
    // A10-05: the cancel-token file handed to the helper (--cancel-file).
    QString m_cancelFile;
};

} // namespace legacy

#endif // LEGACY_HELPER_RUNNER_H
