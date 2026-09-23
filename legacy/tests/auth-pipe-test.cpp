// Qt3 console harness for the HelperRunner sudo authentication pipe.
//
// Reproduces the Etch crash: when sudo already holds a valid timestamp,
// `sudo -S -p '' -v` exits 0 *without reading its stdin*. Writing the password
// to that pipe must not terminate the GUI with SIGPIPE; the write returns
// EPIPE and the authentication still succeeds because sudo exited 0.
//
// The harness prepends a directory holding a fake `sudo` to PATH:
//   - `sudo -n true` fails, so the plain interactive sudo path is resolved;
//   - `sudo -S -p '' -v` exits 0 immediately without reading stdin.
// It must run as a non-root user (root/direct execution never prompts).
//
// Build/run through legacy/tests/test-auth-pipe.sh (qmake-qt3 required; the
// modern host skips it, the Etch guest runs it).
#include <qstring.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "HelperRunner.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "usage: %s <fake-sudo-directory>\n", argv[0]);
        return 2;
    }
    const char *oldPath = ::getenv("PATH");
    QString path = QString::fromLocal8Bit(argv[1]);
    if (oldPath && *oldPath) {
        path += QString::fromLatin1(":");
        path += QString::fromLocal8Bit(oldPath);
    }
    ::setenv("PATH", path.local8Bit(), 1);

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
    return 0;
}
