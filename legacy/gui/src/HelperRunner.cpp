// HelperRunner implementation. Qt3. See HelperRunner.h.

#include "HelperRunner.h"

#include <qprocess.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace legacy {

namespace {

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

bool commandOnPath(const QString &name)
{
    const char *path = ::getenv("PATH");
    if (!path) {
        path = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    }
    const QStringList dirs =
        QStringList::split(QChar(':'), QString::fromLocal8Bit(path), true);
    for (QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it) {
        const QString candidate = (*it) + "/" + name;
        if (::access(candidate.latin1(), X_OK) == 0) {
            return true;
        }
    }
    return false;
}

// Runs a fixed probe command (no shell) and returns true for exit code 0.
// stdout/stderr are discarded so an unsupported probe option (Etch's sudo has
// no -n) can never print its usage text into the GUI's own output.
bool probeCommand(const char *const argv[])
{
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
        ::execvp(argv[0], const_cast<char *const *>(argv));
        ::_exit(127);
    }
    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) {
        // retry on EINTR
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
      m_reported(false)
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

void HelperRunner::resetElevation()
{
    m_resolved = false;
    m_authenticated = false;
}

void HelperRunner::setInputData(const QByteArray &data)
{
    m_input = data;
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
        } else {
            m_prefix = splitPrefix(m_elevationOverride);
            m_description = m_elevationOverride;
        }
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }

    // gksu asks for the password in a GUI dialog; sudo -n is used only when it
    // is already authorized so the GUI never hangs on an unseen password
    // prompt.
    if (commandOnPath(QString::fromLatin1("sudo"))) {
        static const char *const sudoProbe[] = { "sudo", "-n", "true", 0 };
        if (probeCommand(sudoProbe)) {
            m_prefix.append(QString::fromLatin1("sudo"));
            m_prefix.append(QString::fromLatin1("-n"));
            m_description = QString::fromLatin1("sudo -n");
            m_resolved = true;
            if (description) {
                *description = m_description;
            }
            return true;
        }
    }
    if (commandOnPath(QString::fromLatin1("gksu"))) {
        m_prefix.append(QString::fromLatin1("gksu"));
        m_prefix.append(QString::fromLatin1("--sudo-mode"));
        m_description = QString::fromLatin1("gksu --sudo-mode");
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }
    if (commandOnPath(QString::fromLatin1("gksudo"))) {
        m_prefix.append(QString::fromLatin1("gksudo"));
        m_description = QString::fromLatin1("gksudo");
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }
    if (commandOnPath(QString::fromLatin1("sudo"))) {
        m_prefix.append(QString::fromLatin1("sudo"));
        m_description = QString::fromLatin1("sudo (password required)");
        m_resolved = true;
        if (description) {
            *description = m_description;
        }
        return true;
    }
    m_description = QString::fromLatin1(
        "unavailable: install gksu or sudo to run the helper as root");
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
    if (m_prefix.first() != QString::fromLatin1("sudo")) {
        return false;
    }
    for (QStringList::ConstIterator it = m_prefix.begin(); it != m_prefix.end(); ++it) {
        if (*it == QString::fromLatin1("-n")) {
            return false;
        }
    }
    return true;
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
        // keeps sudo from writing a "Password:" line into the GUI.
        static const char *const sudoArgv[] = { "sudo", "-S", "-p", "", "-v", 0 };
        ::execvp(sudoArgv[0], const_cast<char *const *>(sudoArgv));
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

    QByteArray captured;
    char buffer[256];
    for (;;) {
        const ssize_t count = ::read(errorPipe[0], buffer, sizeof(buffer));
        if (count > 0) {
            const uint oldSize = captured.size();
            captured.resize(oldSize + static_cast<uint>(count));
            ::memcpy(captured.data() + oldSize, buffer,
                     static_cast<std::size_t>(count));
            continue;
        }
        if (count < 0 && errno == EINTR) {
            continue;
        }
        break;
    }
    ::close(errorPipe[0]);

    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            break;
        }
    }
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // Pick the least-prompting prefix the installed sudo actually supports:
        // modern sudo uses `-n` after the cached timestamp, while Etch's sudo
        // 1.6.8 has no `-n` and runs non-interactively on the valid timestamp.
        bool nonInteractive = false;
        if (commandOnPath(QString::fromLatin1("sudo"))) {
            static const char *const sudoProbe[] = { "sudo", "-n", "true", 0 };
            nonInteractive = probeCommand(sudoProbe);
        }
        m_prefix.clear();
        m_prefix.append(QString::fromLatin1("sudo"));
        if (nonInteractive) {
            m_prefix.append(QString::fromLatin1("-n"));
            m_description = QString::fromLatin1("sudo -n (authenticated for this session)");
        } else {
            m_description = QString::fromLatin1(
                "sudo (authenticated for this session; this sudo has no -n)");
        }
        m_resolved = true;
        m_authenticated = true;
        return true;
    }
    QString text = QString::fromLocal8Bit(captured.data(), captured.size())
                       .stripWhiteSpace();
    captured.fill('\0');
    if (text.isEmpty()) {
        text = QString::fromLatin1("sudo did not accept the password");
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

    m_buffer = QString::null;
    m_reported = false;
    m_process = new QProcess(this);
    m_process->setCommunication(QProcess::Stdout | QProcess::DupStderr);

    if (!m_direct) {
        for (QStringList::ConstIterator it = m_prefix.begin(); it != m_prefix.end(); ++it) {
            m_process->addArgument(*it);
        }
    }
    m_process->addArgument(m_helperPath);
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
    if (m_process && m_process->isRunning()) {
        m_process->kill();
    }
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
