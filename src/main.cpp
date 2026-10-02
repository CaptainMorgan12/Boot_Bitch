#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QIcon>
#include <QLocale>
#include <QTranslator>

namespace {

// The Boot Bitch UI translator. It must outlive the QApplication (the window
// and the application destructors can still resolve tr() while tearing down),
// so it lives in function-local static storage rather than on main()'s stack.
QTranslator &applicationTranslator()
{
    static QTranslator translator;
    return translator;
}

// Installs the best-matching Boot Bitch UI translation for the current desktop
// locale, falling back to English (no translator) when none is found. The
// helper and application log are intentionally left English by the separate
// LC_ALL=C backend contract; this only translates the Qt GUI chrome.
bool installUiTranslator(QApplication *application)
{
#ifdef BOOT_REPAIR_UI_TEST
    // The offscreen UI regression suite asserts English strings, so it must
    // never pick up a translation from the developer's desktop locale.
    Q_UNUSED(application);
    return false;
#else
    // Installed packages ship translations under
    // <prefix>/share/boot-repair/translations; source-tree/build runs resolve
    // them from a translations/ directory beside the executable instead.
    const QStringList translationDirs = {
        QDir(QCoreApplication::applicationDirPath())
            .absoluteFilePath(QStringLiteral("../share/boot-repair/translations")),
        QDir(QCoreApplication::applicationDirPath())
            .absoluteFilePath(QStringLiteral("translations"))
    };

    QTranslator &translator = applicationTranslator();
    for (const QString &language : QLocale::system().uiLanguages()) {
        const QString fileName =
            QStringLiteral("boot-repair_%1.qm").arg(QLocale(language).name());
        for (const QString &dir : translationDirs) {
            if (translator.load(fileName, dir)) {
                application->installTranslator(&translator);
                return true;
            }
        }
    }
    return false;
#endif
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    // Make the bundled hicolor application assets discoverable when running
    // from an AppImage, while leaving the host desktop theme in control of
    // widget styling and any icons it provides.
    const QString appShare = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../share"));
    QIcon::setThemeSearchPaths({QDir(appShare).absoluteFilePath(QStringLiteral("icons")),
                                QStringLiteral("/usr/share/icons")});

    QCoreApplication::setOrganizationName(QStringLiteral("BootRepair"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("bootrepair.org"));
    QCoreApplication::setApplicationName(QStringLiteral("BootRepair"));
    QCoreApplication::setApplicationVersion(QString::fromLatin1(BOOT_REPAIR_VERSION));
    QApplication::setDesktopFileName(QStringLiteral("org.bootrepair.BootRepair"));

    QIcon applicationIcon = QIcon::fromTheme(QStringLiteral("org.bootrepair.BootRepair"));
    if (applicationIcon.isNull()) {
        applicationIcon = QIcon(QStringLiteral(":/icons/org.bootrepair.BootRepair.png"));
    }
    QApplication::setWindowIcon(applicationIcon);

    // Load the UI translation before any window is constructed, so every
    // widget's tr() call resolves against the installed translator.
    installUiTranslator(&application);

    MainWindow window;
    window.show();

    return application.exec();
}
