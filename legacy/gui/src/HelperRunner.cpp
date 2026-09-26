// HelperRunner implementation. Qt3. See HelperRunner.h.

#include "HelperRunner.h"

#include <qprocess.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <vector>

namespace legacy {

namespace {

// A10-03: the fixed trusted system locations for the elevation tools.  Only
// these paths - each lstat-verified as a root-owned, non-group/world-writable
// regular file inside a root-owned, non-group/world-writable directory - may
// ever be executed by the elevation machinery: never a PATH lookup, never
// execvp with a bare name.  The fail-closed wording is
// "no trusted sudo/gksu found in the fixed system locations".
const char *const kTrustedSudoPaths[] = {
    "/usr/bin/sudo",
    "/usr/local/bin/sudo",
    "/bin/sudo",
    0
};
const char *const kTrustedGksuPaths[] = {
    "/usr/bin/gksu",
    0
};
const char *const kTrustedGksudoPaths[] = {
    "/usr/bin/gksudo",
    0
};

// A password write to a sudo that exits without reading its stdin (for example
// when sudo already holds a valid timestamp, so `sudo -S -v` succeeds
// immediately) must never kill the GUI with SIGPIPE. Ignore SIGPIPE only for
// the duration of the write and restore the previous disposition afterwards so
// the helper processes spawned through QProcess keep the default behaviour.
class ScopedSigPipeIgnore
{
public:
    ScopedSigPipeIgnore()
        : m_valid(false)
    {
        struct sigaction ignore;
        ::memset(&ignore, 0, sizeof(ignore));
        ignore.sa_handler = SIG_IGN;
        ::sigemptyset(&ignore.sa_mask);
        m_valid = ::sigaction(SIGPIPE, &ignore, &m_old) == 0;
    }

    ~ScopedSigPipeIgnore()
    {
        if (m_valid) {
            ::sigaction(SIGPIPE, &m_old, 0);
        }
    }

private:
    struct sigaction m_old;
    bool m_valid;
};

// Splits a whitespace-separated elevation override ("sudo -n") into arguments.
QStringList splitPrefix(const QString &text)
{
    QStringList parts;
    const QStringList fields = QStringList::split(QChar(' '), text, true);
    for (QStringList::ConstIterator it = fields.begin(); it != fields.end(); ++it) {
        parts.append(*it);
    }
    return parts;
}

// Short poll sleep (100 ms) so the bounded WNOHANG wait loops are never busy
// loops and the GUI thread stays responsive; nanosleep is interrupted by
// signals and retried with the remaining time.
void boundedPollSleep()
{
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 100 * 1000 * 1000; // 100 ms
    while (::nanosleep(&delay, &delay) != 0 && errno == EINTR) {
        // retry the remainder on EINTR
    }
}

// Waits for `pid` with a WNOHANG poll until the child exits or the 10-second
// deadline (time(NULL) granularity) passes. On timeout the child is
// SIGKILL'ed, the reap retries for another 10 s, and the function returns
// false: the GUI never blocks indefinitely on a wedged sudo/helper child.
bool boundedWaitPid(pid_t pid, int *status)
{
    const time_t deadline = ::time(NULL) + 10;
    for (;;) {
        const pid_t result = ::waitpid(pid, status, WNOHANG);
        if (result == pid) {
            return true;
        }
        if (result < 0 && errno != EINTR) {
            return false;
        }
        if (::time(NULL) >= deadline) {
            break;
        }
        boundedPollSleep();
    }
    ::kill(pid, SIGKILL);
    const time_t reapDeadline = ::time(NULL) + 10;
    for (;;) {
        const pid_t result = ::waitpid(pid, status, WNOHANG);
        if (result == pid) {
            return false;
        }
        if (result < 0 && errno != EINTR) {
            return false;
        }
        if (::time(NULL) >= reapDeadline) {
            return false;
        }
        boundedPollSleep();
    }
}

// A10-03: returns the first verified candidate for `name` from the fixed
// trusted system locations, or QString::null when none verifies (fail
// closed).  A bare name is never resolved through PATH.
QString trustedToolPath(const QString &name)
{
    // Test seam for the Qt3 auth-pipe harness (test/development only): the
    // fake sudo exercises the password pipe and the bounded waits without a
    // root-owned sudo.  A fake sudo runs as the invoking user, so it cannot
    // elevate anything (the helper still requires root) and the helper
    // verification (A10-04) is never bypassed; the GUI never sets this
    // variable.
    if (name == QString::fromLatin1("sudo")) {
        const char *fake = ::getenv("BOOT_REPAIR_LEGACY_FAKE_SUDO");
        if (fake && *fake && ::access(fake, X_OK) == 0) {
            return QString::fromLocal8Bit(fake);
        }
    }
    const char *const *candidates = 0;
    if (name == QString::fromLatin1("sudo")) {
        candidates = kTrustedSudoPaths;
    } else if (name == QString::fromLatin1("gksu")) {
        candidates = kTrustedGksuPaths;
    } else if (name == QString::fromLatin1("gksudo")) {
        candidates = kTrustedGksudoPaths;
    } else {
        return QString::null;
    }
    for (int i = 0; candidates[i]; ++i) {
        const QString candidate = QString::fromLatin1(candidates[i]);
        if (HelperRunner::verifyTrustedToolPath(candidate, 0)) {
            return candidate;
        }
    }
    return QString::null;
}

// Runs a fixed probe command (no shell, verified absolute path, no PATH
// resolution) and returns true for exit code 0.  stdout/stderr are discarded
// so an unsupported probe option (Etch's sudo has no -n) can never print its
// usage text into the GUI's own output.  The argument pointers are built from
// the stable QString storage before the fork and stay valid in the child's
// copy-on-write address space; the exec uses execv (absolute path), never
// execvp with a bare name.
bool probeCommand(const QStringList &arguments)
{
    if (arguments.isEmpty()) {
        return false;
    }
    std::vector<const char *> argv;
    argv.reserve(static_cast<std::size_t>(arguments.count()) + 1);
    for (int i = 0; i < arguments.count(); ++i) {
        argv.push_back(arguments[i].latin1());
    }
    argv.push_back(0);
    const pid_t pid = ::fork();
    if (pid < 0) {
        return false;
    }
    if (pid == 0) {
        const int devnull = ::open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            ::dup2(devnull, 1);
            ::dup2(devnull, 2);
            if (devnull > 2) {
                ::close(devnull);
            }
        }
        ::execv(argv[0], const_cast<char *const *>(&argv[0]));
        ::_exit(127);
    }
    int status = 0;
    if (!boundedWaitPid(pid, &status)) {
        return false;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

// Same as probeCommand, but the child's stdin is /dev/null. `sudo -S -v`
// reads its password from stdin, so an invalid/expired timestamp gets an
// immediate EOF and fails fast instead of blocking the GUI forever on a
// password read; a valid timestamp exits 0 without reading.
bool probeCommandWithStdinNull(const QStringList &arguments)
{
    if (arguments.isEmpty()) {
        return false;
    }
    std::vector<const char *> argv;
    argv.reserve(static_cast<std::size_t>(arguments.count()) + 1);
    for (int i = 0; i < arguments.count(); ++i) {
        argv.push_back(arguments[i].latin1());
    }
    argv.push_back(0);
    const pid_t pid = ::fork();
    if (pid < 0) {
        return false;
    }
    if (pid == 0) {
        const int devnullIn = ::open("/dev/null", O_RDONLY);
        if (devnullIn >= 0) {
            ::dup2(devnullIn, 0);
            if (devnullIn > 2) {
                ::close(devnullIn);
            }
        }
        const int devnull = ::open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            ::dup2(devnull, 1);
            ::dup2(devnull, 2);
            if (devnull > 2) {
                ::close(devnull);
            }
        }
        ::execv(argv[0], const_cast<char *const *>(&argv[0]));
        ::_exit(127);
    }
    int status = 0;
    if (!boundedWaitPid(pid, &status)) {
        return false;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

} // namespace

HelperRunner::HelperRunner(QObject *parent, const char *name)
    : QObject(parent, name),
      m_process(0),
      m_noElevate(false),
      m_resolved(false),
      m_direct(false),
      m_authenticated(false),
      m_reported(false),
      m_toolsResolved(false)
{
}

HelperRunner::~HelperRunner()
{
    if (m_process) {
        delete m_process;
        m_process = 0;
    }
}

void HelperRunner::setHelperPath(const QString &path)
{
    m_helperPath = path;
}

QString HelperRunner::helperPath() const
{
    return m_helperPath;
}

void HelperRunner::setElevationOverride(const QString &override)
{
    m_elevationOverride = override;
    m_resolved = false;
    m_authenticated = false;
}

void HelperRunner::setNoElevate(bool noElevate)
{
    m_noElevate = noElevate;
    m_resolved = false;
    m_authenticated = false;
}

void HelperRunner::setCancelFile(const QString &path)
{
    m_cancelFile = path;
}

QString HelperRunner::cancelFile() const
{
    return m_cancelFile;
}

void HelperRunner::resetElevation()
{
    m_resolved = false;
    m_authenticated = false;
}

void HelperRunner::setInputData(const QByteArray &data)
{
    m_input = data;
}

// A10-03: verify one elevation tool candidate.  lstat (so a symlink never
// passes), regular file, uid 0, not group/world-writable, and the containing
// directory root-owned and not group/world-writable (a writable directory
// lets anyone swap the tool for a symlink between the check and the exec).
bool HelperRunner::verifyTrustedToolPath(const QString &path, QString *reason)
{
    if (reason) {
        *reason = QString::null;
    }
    if (path.isEmpty() || !path.startsWith(QChar('/'))) {
        if (reason) {
            *reason = QString::fromLatin1("not an absolute path");
        }
        return false;
    }
    struct stat info;
    if (::lstat(path.latin1(), &info) != 0) {
        if (reason) {
            *reason = QString::fromLatin1("missing");
        }
        return false;
    }
    if (!S_ISREG(info.st_mode)) {
        if (reason) {
            *reason = QString::fromLatin1("not a regular file");
        }
        return false;
    }
    if (info.st_uid != 0) {
        if (reason) {
            *reason = QString::fromLatin1("not owned by root");
        }
        return false;
    }
    if ((info.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
        if (reason) {
            *reason = QString::fromLatin1("group or world writable");
        }
        return false;
    }
    const int slash = path.findRev(QChar('/'));
    const QString dirPath = slash > 0
        ? path.left(slash)
        : QString::fromLatin1("/");
    struct stat dirInfo;
    if (::lstat(dirPath.latin1(), &dirInfo) != 0) {
        if (reason) {
            *reason = QString::fromLatin1("parent directory missing");
        }
        return false;
    }
    if (!S_ISDIR(dirInfo.st_mode)) {
        if (reason) {
            *reason = QString::fromLatin1("parent is not a directory");
        }
        return false;
    }
    if (dirInfo.st_uid != 0) {
        if (reason) {
            *reason = QString::fromLatin1("parent directory not owned by root");
        }
        return false;
    }
    if ((dirInfo.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
        if (reason) {
            *reason = QString::fromLatin1("parent directory group or world writable");
        }
        return false;
    }
    return true;
}

// A10-04: verify a helper candidate for ELEVATION.  lstat (so a symlink never
// passes), regular file, uid 0, not group/world-writable, and the resolved
// realpath must equal the raw path.  An env-override or source-tree-relative
// (user-owned, symlinked or non-canonical) helper therefore never gets
// elevated; only the installed helper passes.  Root-run / --no-elevate
// execution never calls this and keeps its previous semantics.
bool HelperRunner::verifyHelperForElevation(const QString &path, QString *reason)
{
    if (reason) {
        *reason = QString::null;
    }
    if (path.isEmpty() || !path.startsWith(QChar('/'))) {
        if (reason) {
            *reason = QString::fromLatin1("not an absolute path");
        }
        return false;
    }
    struct stat info;
    if (::lstat(path.latin1(), &info) != 0) {
        if (reason) {
            *reason = QString::fromLatin1("missing");
        }
        return false;
    }
    if (!S_ISREG(info.st_mode)) {
        if (reason) {
            *reason = QString::fromLatin1("not a regular file");
        }
        return false;
    }
    if (info.st_uid != 0) {
        if (reason) {
            *reason = QString::fromLatin1("not owned by root");
        }
        return false;
    }
    if ((info.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
        if (reason) {
            *reason = QString::fromLatin1("group or world writable");
        }
        return false;
    }
    char resolved[PATH_MAX];
    if (!::realpath(path.latin1(), resolved)) {
        if (reason) {
            *reason = QString::fromLatin1("cannot resolve the real path");
        }
        return false;
    }
    if (QString::fromLocal8Bit(resolved) != path) {
        if (reason) {
            *reason = QString::fromLatin1(
                "resolved path differs (symlinked or non-canonical)");
        }
        return false;
    }
    return true;
}

void HelperRunner::resolveTrustedTools()
{
    if (m_toolsResolved) {
        return;
    }
    m_toolsResolved = true;
    m_sudoPath = trustedToolPath(QString::fromLatin1("sudo"));
    m_gksuPath = trustedToolPath(QString::fromLatin1("gksu"));
    m_gksudoPath = trustedToolPath(QString::fromLatin1("gksudo"));
}

bool HelperRunner::resolveElevation(QString *description)
{
    if (m_resolved) {
        if (description) {
            *description = m_description;
        }
        return true;
    }
    m_prefix.clear();
    m_direct = false;
    resolveTrustedTools();

    if (m_noElevate || ::geteuid() == 0) {
        m_direct = true;
        m_description = (::geteuid() == 0)
            ? QString::fromLatin1("root (no elevation needed)")
            : QString::fromLatin1("no elevation requested (--no-elevate)");
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }

    if (!m_elevationOverride.isEmpty()) {
        if (m_elevationOverride == QString::fromLatin1("none")) {
            m_direct = true;
            m_description = QString::fromLatin1("no elevation (override)");
            m_resolved = true;
            if (description) {
                *description = m_description;
            }
            return true;
        }
        // A10-03: BOOT_REPAIR_LEGACY_ELEVATE stays the explicit escape
        // hatch, but its tool is verified exactly like an auto-detected one:
        // a bare name must resolve to a verified fixed system location, and
        // an explicit path override is lstat-verified as-is.
        const QStringList parts = splitPrefix(m_elevationOverride);
        QString verified;
        QString reason;
        if (parts.isEmpty() || parts.first().isEmpty()) {
            m_description = QString::fromLatin1(
                "invalid elevation override (no command)");
        } else if (parts.first().find(QChar('/')) >= 0) {
            if (verifyTrustedToolPath(parts.first(), &reason)) {
                verified = parts.first();
            } else {
                m_description = QString::fromLatin1(
                    "untrusted elevation override %1 (%2)")
                    .arg(parts.first()).arg(reason);
            }
        } else {
            verified = trustedToolPath(parts.first());
            if (verified.isEmpty()) {
                m_description = QString::fromLatin1(
                    "no trusted %1 found in the fixed system locations")
                    .arg(parts.first());
            }
        }
        if (!verified.isEmpty()) {
            m_prefix.append(verified);
            QString visible = verified;
            for (int i = 1; i < static_cast<int>(parts.count()); ++i) {
                m_prefix.append(parts[i]);
                visible += QString::fromLatin1(" ");
                visible += parts[i];
            }
            m_description = visible;
            m_resolved = true;
            if (description) {
                *description = m_description;
            }
            return true;
        }
        if (description) {
            *description = m_description;
        }
        return false;
    }

    // gksu asks for the password in a GUI dialog; the verified `sudo -n` is
    // probed first so the GUI only uses it when it is already authorized and
    // never hangs on an unseen password prompt.
    if (!m_sudoPath.isEmpty()) {
        QStringList sudoProbe;
        sudoProbe << m_sudoPath << QString::fromLatin1("-n")
                  << QString::fromLatin1("true");
        if (probeCommand(sudoProbe)) {
            m_prefix.append(m_sudoPath);
            m_prefix.append(QString::fromLatin1("-n"));
            m_description = m_sudoPath + QString::fromLatin1(" -n");
            m_resolved = true;
            if (description) {
                *description = m_description;
            }
            return true;
        }
    }
    if (!m_gksuPath.isEmpty()) {
        m_prefix.append(m_gksuPath);
        m_prefix.append(QString::fromLatin1("--sudo-mode"));
        m_description = m_gksuPath + QString::fromLatin1(" --sudo-mode");
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }
    if (!m_gksudoPath.isEmpty()) {
        m_prefix.append(m_gksudoPath);
        m_description = m_gksudoPath;
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }
    if (!m_sudoPath.isEmpty()) {
        m_prefix.append(m_sudoPath);
        m_description = m_sudoPath + QString::fromLatin1(" (password required)");
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }
    m_description = QString::fromLatin1(
        "no trusted sudo/gksu found in the fixed system locations "
        "(/usr/bin/sudo, /usr/local/bin/sudo, /bin/sudo, /usr/bin/gksu, "
        "/usr/bin/gksudo)");
    if (description) {
        *description = m_description;
    }
    return false;
}

bool HelperRunner::elevationNeedsPassword(QString *description)
{
    if (!m_resolved) {
        resolveElevation(description);
    } else if (description) {
        *description = m_description;
    }
    if (m_authenticated || m_direct || m_prefix.isEmpty()) {
        return false;
    }
    if (m_prefix.first() != m_sudoPath) {
        return false;
    }
    for (QStringList::ConstIterator it = m_prefix.begin(); it != m_prefix.end(); ++it) {
        if (*it == QString::fromLatin1("-n")) {
            return false;
        }
    }
    return true;
}

bool HelperRunner::sessionIsCurrent()
{
    if (!m_resolved) {
        resolveElevation(0);
    }
    if (m_direct || m_prefix.isEmpty()) {
        return true;
    }
    // gksu/gksudo/su provide their own prompt (or the caller is root); only
    // sudo carries a cacheable, non-interactive session here.
    if (m_prefix.first() != m_sudoPath) {
        return true;
    }
    bool nonInteractive = false;
    for (QStringList::ConstIterator it = m_prefix.begin(); it != m_prefix.end(); ++it) {
        if (*it == QString::fromLatin1("-n")) {
            nonInteractive = true;
            break;
        }
    }
    if (nonInteractive) {
        QStringList sudoProbe;
        sudoProbe << m_sudoPath << QString::fromLatin1("-n")
                  << QString::fromLatin1("true");
        return probeCommand(sudoProbe);
    }
    // Etch's sudo 1.6.8 has no -n: `sudo -S -v` validates the cached timestamp
    // without reading stdin when it is still valid, and gets an immediate EOF
    // when it expired.
    QStringList sudoTimestampProbe;
    sudoTimestampProbe << m_sudoPath << QString::fromLatin1("-S")
                       << QString::fromLatin1("-v");
    return probeCommandWithStdinNull(sudoTimestampProbe);
}

bool HelperRunner::clearSudoTimestamp()
{
    resolveTrustedTools();
    if (m_sudoPath.isEmpty()) {
        return false;
    }
    // `sudo -k` only removes the cached timestamp; it never prompts. Closing
    // stdin keeps even a broken sudo from blocking on a password read.
    QStringList sudoKill;
    sudoKill << m_sudoPath << QString::fromLatin1("-k");
    return probeCommandWithStdinNull(sudoKill);
}

bool HelperRunner::authenticateElevation(const QByteArray &secret, QString *error)
{
    if (error) {
        *error = QString::null;
    }
    if (m_direct || !elevationNeedsPassword(0)) {
        return true;
    }
    if (secret.isEmpty()) {
        if (error) {
            *error = QString::fromLatin1("no password was provided");
        }
        return false;
    }

    int inputPipe[2];
    int errorPipe[2];
    if (::pipe(inputPipe) != 0) {
        if (error) {
            *error = QString::fromLatin1("cannot create the sudo authentication pipe");
        }
        return false;
    }
    if (::pipe(errorPipe) != 0) {
        ::close(inputPipe[0]);
        ::close(inputPipe[1]);
        if (error) {
            *error = QString::fromLatin1("cannot create the sudo authentication pipe");
        }
        return false;
    }

    const pid_t pid = ::fork();
    if (pid < 0) {
        ::close(inputPipe[0]);
        ::close(inputPipe[1]);
        ::close(errorPipe[0]);
        ::close(errorPipe[1]);
        if (error) {
            *error = QString::fromLatin1("cannot start sudo");
        }
        return false;
    }
    if (pid == 0) {
        ::signal(SIGPIPE, SIG_DFL);
        ::close(inputPipe[1]);
        ::close(errorPipe[0]);
        ::dup2(inputPipe[0], 0);
        ::dup2(errorPipe[1], 2);
        const int devnull = ::open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            ::dup2(devnull, 1);
            if (devnull > 2) {
                ::close(devnull);
            }
        }
        ::close(inputPipe[0]);
        ::close(errorPipe[1]);
        // The password is read from the inherited stdin pipe; the empty prompt
        // keeps sudo from writing a "Password:" line into the GUI.  The exec
        // uses the verified absolute sudo path (execv, never execvp).
        const char *const sudoArgv[] = {
            m_sudoPath.latin1(), "-S", "-p", "", "-v", 0
        };
        ::execv(sudoArgv[0], const_cast<char *const *>(sudoArgv));
        ::_exit(127);
    }

    ::close(inputPipe[0]);
    ::close(errorPipe[1]);
    // Write the password and its newline directly, without building a
    // secret-bearing QByteArray (Qt3's QByteArray has no append/+= helpers).
    // SIGPIPE stays ignored for the write: when sudo already holds a valid
    // timestamp it exits without reading stdin and the write returns EPIPE
    // instead of terminating the GUI.
    {
        ScopedSigPipeIgnore ignoreSigPipe;
        std::size_t offset = 0;
        while (offset < static_cast<std::size_t>(secret.size())) {
            const ssize_t written = ::write(inputPipe[1], secret.data() + offset,
                                            static_cast<std::size_t>(secret.size()) - offset);
            if (written < 0) {
                if (errno == EINTR) {
                    continue;
                }
                break;
            }
            offset += static_cast<std::size_t>(written);
        }
        {
            const char newline = '\n';
            if (::write(inputPipe[1], &newline, 1) != 1) {
                // Closing the pipe below makes sudo fail closed on EOF.
            }
        }
        ::close(inputPipe[1]);
    }

    // Read sudo's stderr with a poll()-bounded 10-second deadline and a
    // 32 KiB capture cap: a wedged sudo can neither block the GUI forever nor
    // exhaust its memory; bytes past the cap are discarded.
    QByteArray captured;
    const uint capturedLimit = 32 * 1024;
    const time_t stderrDeadline = ::time(NULL) + 10;
    char buffer[256];
    struct pollfd pollFd;
    pollFd.fd = errorPipe[0];
    pollFd.events = POLLIN;
    for (;;) {
        if (::time(NULL) >= stderrDeadline) {
            break;
        }
        pollFd.revents = 0;
        const int ready = ::poll(&pollFd, 1, 250);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        if (ready == 0) {
            continue; // the deadline check at the top bounds the loop
        }
        const ssize_t count = ::read(errorPipe[0], buffer, sizeof(buffer));
        if (count > 0) {
            if (captured.size() < capturedLimit) {
                const uint room = capturedLimit - captured.size();
                const uint take = static_cast<uint>(count) < room
                    ? static_cast<uint>(count) : room;
                const uint oldSize = captured.size();
                captured.resize(oldSize + take);
                ::memcpy(captured.data() + oldSize, buffer,
                         static_cast<std::size_t>(take));
            }
            continue;
        }
        if (count < 0 && errno == EINTR) {
            continue;
        }
        break; // EOF (sudo closed stderr) or a read error
    }
    ::close(errorPipe[0]);

    int status = 0;
    const bool reaped = boundedWaitPid(pid, &status);
    if (reaped && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // Pick the least-prompting prefix the installed sudo actually supports:
        // modern sudo uses `-n` after the cached timestamp, while Etch's sudo
        // 1.6.8 has no `-n` and runs non-interactively on the valid timestamp.
        bool nonInteractive = false;
        if (!m_sudoPath.isEmpty()) {
            QStringList sudoProbe;
            sudoProbe << m_sudoPath << QString::fromLatin1("-n")
                      << QString::fromLatin1("true");
            nonInteractive = probeCommand(sudoProbe);
        }
        m_prefix.clear();
        m_prefix.append(m_sudoPath);
        if (nonInteractive) {
            m_prefix.append(QString::fromLatin1("-n"));
            m_description = m_sudoPath
                + QString::fromLatin1(" -n (authenticated for this session)");
        } else {
            m_description = m_sudoPath
                + QString::fromLatin1(
                    " (authenticated for this session; this sudo has no -n)");
        }
        m_resolved = true;
        m_authenticated = true;
        return true;
    }
    QString text = QString::fromLocal8Bit(captured.data(), captured.size())
                       .stripWhiteSpace();
    captured.fill('\0');
    if (text.isEmpty()) {
        text = reaped
            ? QString::fromLatin1("sudo did not accept the password")
            : QString::fromLatin1("sudo did not respond within the timeout");
    }
    if (error) {
        *error = text;
    }
    return false;
}

bool HelperRunner::isRunning() const
{
    return m_process && m_process->isRunning();
}

void HelperRunner::emitErrorLine(const QString &text)
{
    emit outputLine(text);
}

bool HelperRunner::run(const QStringList &helperArgs)
{
    if (isRunning()) {
        emitErrorLine(QString::fromLatin1("ERROR: a helper command is already running."));
        return false;
    }
    QString elevationDescription;
    if (!resolveElevation(&elevationDescription)) {
        m_input.fill('\0');
        m_input = QByteArray();
        emitErrorLine(QString::fromLatin1("ERROR: ") + elevationDescription);
        emit finished(false, -1);
        return false;
    }
    if (m_helperPath.isEmpty()) {
        m_input.fill('\0');
        m_input = QByteArray();
        emitErrorLine(QString::fromLatin1("ERROR: no legacy helper path is configured."));
        emit finished(false, -1);
        return false;
    }
    if (!m_direct) {
        // A10-04: before any elevation the resolved helper must verify as the
        // installed helper (regular, no symlink, uid 0, not group/world
        // writable, realpath == path).  An env-override or source-tree-
        // relative helper is refused for elevation; root-run / --no-elevate
        // execution keeps its previous semantics.
        QString trustReason;
        if (!verifyHelperForElevation(m_helperPath, &trustReason)) {
            m_input.fill('\0');
            m_input = QByteArray();
            emitErrorLine(QString::fromLatin1(
                "ERROR: refusing to elevate the helper %1: %2. The helper "
                "must be an installed, root-owned helper; run the GUI as root "
                "or with --no-elevate to use an unverified helper.")
                .arg(m_helperPath).arg(trustReason));
            emit finished(false, -1);
            return false;
        }
    }

    m_buffer = QString::null;
    m_reported = false;
    m_process = new QProcess(this);
    // stdin stays unused by design: Qt 3.3.7's QProcess cannot deliver stdin
    // reliably (without QProcess::Stdin the written bytes never reach the
    // child; with it the child inherits the parent's stdin on some setups).
    // The LUKS passphrase therefore travels through a mode-600 keyfile
    // argument (never argv-processable content) and setInputData is only
    // used to clear the buffer. Stderr is duplicated onto stdout (DupStderr)
    // so one read channel carries the whole transcript; the admin sudo -S -v
    // authentication uses its own raw fork/pipe and is unaffected.
    m_process->setCommunication(QProcess::Stdout | QProcess::DupStderr);

    if (!m_direct) {
        for (QStringList::ConstIterator it = m_prefix.begin(); it != m_prefix.end(); ++it) {
            m_process->addArgument(*it);
        }
    }
    m_process->addArgument(m_helperPath);
    if (!m_cancelFile.isEmpty()) {
        // A10-05: the helper accepts --cancel-file before its command verb
        // and polls the file between repair stages and on shell-command
        // ticks; cancel() touches the file before the direct kill.
        m_process->addArgument(QString::fromLatin1("--cancel-file"));
        m_process->addArgument(m_cancelFile);
    }
    for (QStringList::ConstIterator it = helperArgs.begin(); it != helperArgs.end(); ++it) {
        m_process->addArgument(*it);
    }

    connect(m_process, SIGNAL(readyReadStdout()), this, SLOT(readOutput()));
    connect(m_process, SIGNAL(processExited()), this, SLOT(processExited()));

    if (!m_process->start()) {
        m_input.fill('\0');
        m_input = QByteArray();
        emitErrorLine(QString::fromLatin1("ERROR: failed to start helper: ")
                      + m_helperPath);
        delete m_process;
        m_process = 0;
        emit finished(false, -1);
        return false;
    }

    // LUKS passphrases (and only those) travel over the helper's standard
    // input; the pipe is closed immediately so the helper reads exactly the
    // submitted bytes and never waits for a newline. The buffer is wiped
    // right after the write. SIGPIPE is ignored for the write so a helper that
    // exited before reading cannot kill the GUI.
    if (!m_input.isEmpty()) {
        ScopedSigPipeIgnore ignoreSigPipe;
        m_process->writeToStdin(m_input);
        m_process->closeStdin();
        m_input.fill('\0');
        m_input = QByteArray();
    }
    return true;
}

void HelperRunner::cancel()
{
    if (!m_process || !m_process->isRunning()) {
        return;
    }
    if (!m_cancelFile.isEmpty()) {
        // A10-05: signal the helper-side cancel token first.  The helper
        // polls the file between repair stages and on shell-command ticks
        // and aborts with its own bounded TERM -> KILL escalation and full
        // session cleanup.  O_NOFOLLOW refuses a symlinked cancel file.
        const int fd = ::open(m_cancelFile.latin1(),
                              O_WRONLY | O_CREAT | O_NOFOLLOW, 0600);
        if (fd >= 0) {
            ::close(fd);
        }
        // Short grace (200 ms) before the existing direct-kill fallback so
        // the helper's watcher can observe the token; the kill only ever
        // reaches the elevation wrapper (sudo), which is the documented
        // Etch deviation.
        boundedPollSleep();
        boundedPollSleep();
    }
    m_process->kill();
}

void HelperRunner::readOutput()
{
    if (!m_process) {
        return;
    }
    const QByteArray chunk = m_process->readStdout();
    m_buffer += QString::fromLocal8Bit(chunk.data(), chunk.size());
    flushBufferedLines();
}

void HelperRunner::flushBufferedLines()
{
    int newline = m_buffer.find('\n');
    while (newline >= 0) {
        QString line = m_buffer.left(newline);
        m_buffer = m_buffer.mid(newline + 1);
        if (!line.isEmpty() && line[line.length() - 1] == QChar('\r')) {
            line = line.left(line.length() - 1);
        }
        emit outputLine(line);
        newline = m_buffer.find('\n');
    }
}

void HelperRunner::processExited()
{
    if (!m_process || m_reported) {
        return;
    }
    // Drain whatever the process wrote just before exiting.
    const QByteArray chunk = m_process->readStdout();
    m_buffer += QString::fromLocal8Bit(chunk.data(), chunk.size());
    flushBufferedLines();
    if (!m_buffer.isEmpty()) {
        QString line = m_buffer;
        m_buffer = QString::null;
        if (!line.isEmpty() && line[line.length() - 1] == QChar('\r')) {
            line = line.left(line.length() - 1);
        }
        emit outputLine(line);
    }

    const bool normal = m_process->normalExit();
    const int code = m_process->exitStatus();
    m_reported = true;
    QProcess *finishedProcess = m_process;
    m_process = 0;
    finishedProcess->deleteLater();
    emit finished(normal && code == 0, normal ? code : -1);
}

} // namespace legacy
