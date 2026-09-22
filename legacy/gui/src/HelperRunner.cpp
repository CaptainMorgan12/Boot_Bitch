// HelperRunner implementation. Qt3. See HelperRunner.h.

#include "HelperRunner.h"

#include <qprocess.h>

#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace legacy {

namespace {

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
bool probeCommand(const char *const argv[])
{
    const pid_t pid = ::fork();
    if (pid < 0) {
        return false;
    }
    if (pid == 0) {
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
}

void HelperRunner::setNoElevate(bool noElevate)
{
    m_noElevate = noElevate;
    m_resolved = false;
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
        m_description = QString::fromLatin1("sudo (may need a terminal)");
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
        emitErrorLine(QString::fromLatin1("ERROR: ") + elevationDescription);
        emit finished(false, -1);
        return false;
    }
    if (m_helperPath.isEmpty()) {
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
        emitErrorLine(QString::fromLatin1("ERROR: failed to start helper: ")
                      + m_helperPath);
        delete m_process;
        m_process = 0;
        emit finished(false, -1);
        return false;
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
