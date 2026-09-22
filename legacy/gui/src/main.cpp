// boot-repair-legacy-gui - Qt3 frontend entry point.
//
// CLI (kept usable without X for --help/--version/--print-config):
//   boot-repair-legacy-gui [--helper PATH] [--elevate PREFIX|none]
//                          [--no-elevate] [--host-disk PATH] [--host-root PATH]
//                          [--log-dir DIR] [--smoke-test]
//   boot-repair-legacy-gui --help | --version | --print-config
//
// The helper path discovery mirrors the boot-repair-legacy launcher:
// --helper, BOOT_REPAIR_LEGACY_HELPER, /usr/sbin/boot-repair-legacy-helper,
// /usr/libexec/boot-repair/boot-repair-helper-legacy, source-tree paths.

#include <qapplication.h>
#include <qfile.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "DeviceInventory.h"
#include "HelperRunner.h"
#include "LegacyMainWindow.h"

#ifndef LEGACY_VERSION
#define LEGACY_VERSION "unknown"
#endif

namespace {

void printUsage(FILE *out, const char *program)
{
    std::fprintf(out,
        "Usage: %s [options]\n"
        "\n"
        "Legacy Boot Bitch Qt3 frontend (Debian Etch / KDE 3.5 era). The GUI is a\n"
        "thin client for the ported privileged helper and only enables commands\n"
        "whose cached 'Repair tool' capability line says available.\n"
        "\n"
        "Options:\n"
        "  --helper PATH        helper to run (default /usr/sbin/boot-repair-legacy-helper)\n"
        "  --elevate PREFIX     elevation prefix, e.g. 'sudo -n' or 'gksu --sudo-mode';\n"
        "                       'none' runs the helper directly\n"
        "  --no-elevate         do not elevate (must already be root)\n"
        "  --host-disk PATH     running-host physical disk (smoke test/auto-fill)\n"
        "  --host-root PATH     running-host root component (smoke test/auto-fill)\n"
        "  --log-dir DIR        directory for the per-session helper log\n"
        "  --smoke-test         run host-diagnose all + host-validate, verify the\n"
        "                       unlock/elevation/filter controls and the 1024x768\n"
        "                       layout, print the result, then exit (needs a display)\n"
        "  --print-config       print the resolved helper, host target and read-only\n"
        "                       device inventory without starting a GUI\n"
        "  -V, --version        print the legacy frontend version\n"
        "  -h, --help           show this help\n"
        "\n"
        "TUI fallback (no X session): boot-repair-legacy --tui\n",
        program);
}

bool isExecutable(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }
    return ::access(path.latin1(), X_OK) == 0;
}

QString discoverHelper(const QString &override, const QString &argv0)
{
    if (!override.isEmpty()) {
        return override;
    }
    const char *env = ::getenv("BOOT_REPAIR_LEGACY_HELPER");
    if (env && *env) {
        return QString::fromLocal8Bit(env);
    }
    static const char *const installed[] = {
        "/usr/sbin/boot-repair-legacy-helper",
        "/usr/libexec/boot-repair/boot-repair-helper-legacy",
        0
    };
    for (int i = 0; installed[i]; ++i) {
        const QString candidate = QString::fromLatin1(installed[i]);
        if (isExecutable(candidate)) {
            return candidate;
        }
    }
    // Source-tree fallbacks relative to the binary location.
    QString selfDir;
    const int slash = argv0.findRev(QChar('/'));
    if (slash >= 0) {
        selfDir = argv0.left(slash);
    }
    static const char *const relative[] = {
        "/boot-repair-legacy-helper",
        "/../legacy/boot-repair-helper.sh",
        "/../boot-repair-helper.sh",
        "/../../legacy/boot-repair-helper.sh",
        0
    };
    for (int i = 0; relative[i]; ++i) {
        const QString candidate = selfDir + QString::fromLatin1(relative[i]);
        if (isExecutable(candidate)) {
            return candidate;
        }
    }
    return QString::fromLatin1("/usr/sbin/boot-repair-legacy-helper");
}

class SmokeReporter : public QObject
{
    Q_OBJECT
public:
    explicit SmokeReporter(QApplication *application)
        : QObject(0, "smoke-reporter"), m_application(application), m_ok(false)
    {
    }

    bool ok() const
    {
        return m_ok;
    }

public slots:
    void finished(bool ok, const QString &summary)
    {
        m_ok = ok;
        std::printf("SMOKE %s: %s\n", ok ? "OK" : "FAIL", summary.latin1());
        std::fflush(stdout);
        m_application->quit();
    }

private:
    QApplication *m_application;
    bool m_ok;
};

} // namespace

int main(int argc, char **argv)
{
    QString helperOverride;
    QString elevationOverride;
    QString hostDisk;
    QString hostRoot;
    QString logDir;
    bool noElevate = false;
    bool smokeTest = false;
    bool printConfig = false;

    QStringList arguments;
    for (int i = 1; i < argc; ++i) {
        arguments.append(QString::fromLocal8Bit(argv[i]));
    }
    for (QStringList::ConstIterator it = arguments.begin(); it != arguments.end(); ++it) {
        const QString &arg = *it;
        if (arg == QString::fromLatin1("-h") || arg == QString::fromLatin1("--help")) {
            printUsage(stdout, argv[0]);
            return 0;
        } else if (arg == QString::fromLatin1("-V") || arg == QString::fromLatin1("--version")) {
            std::printf("boot-repair-legacy-gui %s\n", LEGACY_VERSION);
            return 0;
        } else if (arg == QString::fromLatin1("--helper")) {
            ++it;
            if (it == arguments.end()) {
                std::fprintf(stderr, "%s: --helper requires a path\n", argv[0]);
                return 2;
            }
            helperOverride = *it;
        } else if (arg.startsWith(QString::fromLatin1("--helper="))) {
            helperOverride = arg.mid(9);
        } else if (arg == QString::fromLatin1("--elevate")) {
            ++it;
            if (it == arguments.end()) {
                std::fprintf(stderr, "%s: --elevate requires a value\n", argv[0]);
                return 2;
            }
            elevationOverride = *it;
        } else if (arg.startsWith(QString::fromLatin1("--elevate="))) {
            elevationOverride = arg.mid(10);
        } else if (arg == QString::fromLatin1("--no-elevate")) {
            noElevate = true;
        } else if (arg == QString::fromLatin1("--host-disk")) {
            ++it;
            if (it == arguments.end()) {
                std::fprintf(stderr, "%s: --host-disk requires a path\n", argv[0]);
                return 2;
            }
            hostDisk = *it;
        } else if (arg.startsWith(QString::fromLatin1("--host-disk="))) {
            hostDisk = arg.mid(12);
        } else if (arg == QString::fromLatin1("--host-root")) {
            ++it;
            if (it == arguments.end()) {
                std::fprintf(stderr, "%s: --host-root requires a path\n", argv[0]);
                return 2;
            }
            hostRoot = *it;
        } else if (arg.startsWith(QString::fromLatin1("--host-root="))) {
            hostRoot = arg.mid(12);
        } else if (arg == QString::fromLatin1("--log-dir")) {
            ++it;
            if (it == arguments.end()) {
                std::fprintf(stderr, "%s: --log-dir requires a directory\n", argv[0]);
                return 2;
            }
            logDir = *it;
        } else if (arg.startsWith(QString::fromLatin1("--log-dir="))) {
            logDir = arg.mid(10);
        } else if (arg == QString::fromLatin1("--smoke-test")) {
            smokeTest = true;
        } else if (arg == QString::fromLatin1("--print-config")) {
            printConfig = true;
        } else {
            std::fprintf(stderr, "%s: unknown option: %s\n", argv[0], arg.latin1());
            printUsage(stderr, argv[0]);
            return 2;
        }
    }

    const QString helper = discoverHelper(helperOverride, QString::fromLocal8Bit(argv[0]));

    if (printConfig) {
        std::printf("boot-repair-legacy-gui %s\n", LEGACY_VERSION);
        std::printf("helper: %s (%s)\n", helper.latin1(),
                    isExecutable(helper) ? "executable" : "missing or not executable");
        legacy::HelperRunner runner;
        runner.setHelperPath(helper);
        runner.setElevationOverride(elevationOverride);
        runner.setNoElevate(noElevate);
        QString elevation;
        if (runner.resolveElevation(&elevation)) {
            std::printf("elevation: %s\n", elevation.latin1());
        } else {
            std::printf("elevation: unavailable (%s)\n", elevation.latin1());
        }
        std::string root;
        std::string disk;
        if (legacy::detectRunningHostTarget(&root, &disk)) {
            std::printf("host root: %s\n", root.c_str());
            std::printf("host disk: %s\n", disk.c_str());
        } else {
            std::printf("host root: unavailable\n");
            std::printf("host disk: unavailable\n");
        }
        const std::vector<legacy::DeviceRow> rows = legacy::scanDevices();
        std::printf("devices: %lu\n", static_cast<unsigned long>(rows.size()));
        for (std::size_t i = 0; i < rows.size(); ++i) {
            std::printf("  %-22s %-8s %-10s %-8s %-12s %s\n",
                        rows[i].path.c_str(), rows[i].size.c_str(),
                        rows[i].disk ? (rows[i].optical ? "optical" : "disk")
                                     : (rows[i].mapper ? "mapper" : "partition"),
                        rows[i].fstype.c_str(), rows[i].mountpoint.c_str(),
                        rows[i].model.c_str());
        }
        return 0;
    }

    QApplication application(argc, argv);
    application.setName(QString::fromLatin1("boot-repair-legacy-gui"));
    legacy::LegacyMainWindow window;
    window.setHelperPath(helper);
    window.setElevationOverride(elevationOverride);
    window.setNoElevate(noElevate);
    if (!logDir.isEmpty()) {
        window.setLogDirectory(logDir);
    }
    if (!hostDisk.isEmpty() || !hostRoot.isEmpty()) {
        window.setTarget(hostDisk, hostRoot);
    }
    window.show();

    if (smokeTest) {
        SmokeReporter reporter(&application);
        QObject::connect(&window, SIGNAL(smokeFinished(bool, const QString &)),
                         &reporter, SLOT(finished(bool, const QString &)));
        window.startSmokeTest();
        application.exec();
        return reporter.ok() ? 0 : 1;
    }

    return application.exec();
}

#include "main.moc"
