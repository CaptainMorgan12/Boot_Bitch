// Qt3 console harness for the HelperRunner sudo authentication pipe and the
// cached-session expiry probe.
//
// Reproduces the Etch crash: when sudo already holds a valid timestamp,
// `sudo -S -p '' -v` exits 0 *without reading its stdin*. Writing the password
// to that pipe must not terminate the GUI with SIGPIPE; the write returns
// EPIPE and the authentication still succeeds because sudo exited 0.
//
// It also covers the deferred-authorization carry-forward fix:
//   - after authenticateElevation() the cached plain-sudo session reports
//     sessionIsCurrent() == true (the next privileged command may run);
//   - once the fake sudo's timestamp is expired (state file), the same probe
//     reports false without blocking, so the GUI fails closed with the
//     Authorize remedy instead of hanging on sudo's password read;
//   - resetElevation() restores the interactive-password path, which is what
//     the explicit Authorize control needs.
//
// The harness prepends a directory holding a fake `sudo` to PATH:
//   - `sudo -n true` fails, so the plain interactive sudo path is resolved;
//   - `sudo -S -p '' -v` exits 0 immediately without reading stdin unless the
//     caller's state directory holds an `expired` marker.
// It must run as a non-root user (root/direct execution never prompts).
//
// Build/run through legacy/tests/test-auth-pipe.sh (qmake-qt3 required; the
// modern host skips it, the Etch guest runs it).
#include <qstring.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include "HelperRunner.h"

namespace {

bool writeExpiredMarker(const QString &stateDir)
{
    const QString path = stateDir + QString::fromLatin1("/expired");
    FILE *file = std::fopen(path.latin1(), "w");
    if (!file) {
        return false;
    }
    std::fclose(file);
    return true;
}

} // namespace

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::fprintf(stderr, "usage: %s <fake-sudo-directory> <state-directory>\n", argv[0]);
        return 2;
    }
    const char *oldPath = ::getenv("PATH");
    QString path = QString::fromLocal8Bit(argv[1]);
    if (oldPath && *oldPath) {
        path += QString::fromLatin1(":");
        path += QString::fromLocal8Bit(oldPath);
    }
    ::setenv("PATH", path.local8Bit(), 1);
    // The fake sudo consults this directory for the `expired` marker.
    ::setenv("FAKE_SUDO_STATE", argv[2], 1);

    legacy::HelperRunner runner;
    QString description;
    if (!runner.elevationNeedsPassword(&description)) {
        std::fprintf(stderr,
                     "FAIL: the fake sudo did not force the interactive path "
                     "(elevation resolved as '%s')\n",
                     description.latin1());
        return 1;
    }
    QString error;
    // Qt 3.3's QByteArray is a QMemArray<char> typedef with no const char*
    // constructor; build the throwaway secret explicitly.
    const char *secretText = "not-read-by-sudo";
    const int secretSize = static_cast<int>(std::strlen(secretText));
    QByteArray secret(secretSize);
    std::memcpy(secret.data(), secretText, static_cast<std::size_t>(secretSize));
    if (!runner.authenticateElevation(secret, &error)) {
        std::fprintf(stderr, "FAIL: authentication failed: %s\n", error.latin1());
        return 1;
    }
    std::printf("AUTH-PIPE OK: sudo exited without reading stdin; the password "
                "write handled EPIPE without SIGPIPE\n");

    // The cached session must carry forward to the next privileged command.
    if (!runner.sessionIsCurrent()) {
        std::fprintf(stderr, "FAIL: the authenticated session was reported expired\n");
        return 1;
    }
    // Expire the fake sudo timestamp: the probe must fail closed without
    // blocking, and the cached decision must not be silently dropped (the
    // window's Authorize control is what re-establishes it).
    if (!writeExpiredMarker(QString::fromLocal8Bit(argv[2]))) {
        std::fprintf(stderr, "FAIL: could not write the expiry marker\n");
        return 1;
    }
    if (runner.sessionIsCurrent()) {
        std::fprintf(stderr, "FAIL: an expired sudo timestamp was reported current\n");
        return 1;
    }
    if (runner.elevationNeedsPassword(&description)) {
        std::fprintf(stderr,
                     "FAIL: the cached decision was dropped before the window "
                     "could re-authorize (elevation resolved as '%s')\n",
                     description.latin1());
        return 1;
    }
    // Re-authorization path: after the window resets the elevation decision
    // (its Authorize control), the interactive password path is available.
    runner.resetElevation();
    if (!runner.elevationNeedsPassword(&description)) {
        std::fprintf(stderr,
                     "FAIL: resetElevation() did not restore the interactive "
                     "password path (elevation resolved as '%s')\n",
                     description.latin1());
        return 1;
    }
    std::printf("SESSION-EXPIRY OK: expired cached sudo detected without "
                "blocking; resetElevation() restores the Authorize path\n");
    return 0;
}
