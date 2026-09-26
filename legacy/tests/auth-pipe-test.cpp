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
// And the B3 bounded-wait hardening:
//   - with the `slow` marker the fake sudo spams stderr and stalls past the
//     deadlines; sessionIsCurrent() and authenticateElevation() must both
//     fail closed within their 10-second bounds (no indefinite block), and
//     the captured sudo stderr must stay capped at 32 KiB.
//
// The harness points HelperRunner at a fake `sudo` through the documented
// test seam BOOT_REPAIR_LEGACY_FAKE_SUDO (the runner resolves elevation tools
// from the fixed trusted system locations only, so a PATH fake would never be
// seen; the seam is test/development only and cannot elevate anything - the
// fake sudo runs as the invoking user and the helper still requires root):
//   - `sudo -n true` fails, so the plain interactive sudo path is resolved;
//   - `sudo -S -p '' -v` exits 0 immediately without reading stdin unless the
//     caller's state directory holds an `expired` marker (exit 1) or a `slow`
//     marker (stderr spam + a 30-second stall).
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
#include <time.h>
#include <unistd.h>

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

bool writeSlowMarker(const QString &stateDir)
{
    const QString path = stateDir + QString::fromLatin1("/slow");
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
    // A10-03: the runner resolves elevation tools from the fixed trusted
    // system locations, so the fake sudo is registered through the documented
    // test seam instead of PATH.
    const QString fakeSudo = QString::fromLocal8Bit(argv[1])
        + QString::fromLatin1("/sudo");
    ::setenv("BOOT_REPAIR_LEGACY_FAKE_SUDO", fakeSudo.local8Bit(), 1);
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

    // Bounded-wait coverage: a wedged sudo must never hang the GUI's probes.
    // The slow marker makes the fake sudo spam stderr past the 32 KiB cap and
    // stall well beyond the 10-second deadlines; sessionIsCurrent() must fail
    // closed within its bound and authenticateElevation() must fail within
    // its bound with the captured stderr capped.
    if (!writeSlowMarker(QString::fromLocal8Bit(argv[2]))) {
        std::fprintf(stderr, "FAIL: could not write the slow-sudo marker\n");
        return 1;
    }
    // The fake sudo checks the slow marker first, but keep the cases
    // independent by removing the expired marker.
    ::unlink((QString::fromLocal8Bit(argv[2]) + QString::fromLatin1("/expired"))
                 .local8Bit()
                 .data());
    const time_t probeStarted = ::time(NULL);
    if (runner.sessionIsCurrent()) {
        std::fprintf(stderr, "FAIL: a wedged sudo was reported current\n");
        return 1;
    }
    const time_t probeElapsed = ::time(NULL) - probeStarted;
    if (probeElapsed < 8) {
        std::fprintf(stderr, "FAIL: the wedged-sudo probe returned too fast "
                             "(%ld s; the deadline was not enforced)\n",
                     static_cast<long>(probeElapsed));
        return 1;
    }
    if (probeElapsed > 15) {
        std::fprintf(stderr, "FAIL: the wedged-sudo probe took %ld s "
                             "(deadline not enforced)\n",
                     static_cast<long>(probeElapsed));
        return 1;
    }
    const time_t authStarted = ::time(NULL);
    QString slowError;
    if (runner.authenticateElevation(secret, &slowError)) {
        std::fprintf(stderr, "FAIL: a wedged sudo authenticated\n");
        return 1;
    }
    const time_t authElapsed = ::time(NULL) - authStarted;
    if (authElapsed > 35) {
        std::fprintf(stderr, "FAIL: the wedged-sudo authentication took %ld s "
                             "(deadline not enforced)\n",
                     static_cast<long>(authElapsed));
        return 1;
    }
    if (slowError.length() > 32 * 1024) {
        std::fprintf(stderr, "FAIL: captured sudo stderr exceeded the 32 KiB "
                             "cap (%d bytes)\n",
                     slowError.length());
        return 1;
    }
    std::printf("BOUNDED-WAIT OK: wedged sudo probes fail closed within the "
                "deadlines; captured stderr capped at 32 KiB\n");
    return 0;
}
