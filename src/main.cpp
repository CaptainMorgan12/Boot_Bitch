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
#ifdef BOOT_BITCH_UI_TEST
    // The offscreen UI regression suite asserts English strings, so it must
    // never pick up a translation from the developer's desktop locale.
    Q_UNUSED(application);
    return false;
#else
    // Installed packages ship translations under
    // <prefix>/share/boot-bitch/translations; source-tree/build runs resolve
    // them from a translations/ directory beside the executable instead.
    const QStringList translationDirs = {
        QDir(QCoreApplication::applicationDirPath())
            .absoluteFilePath(QStringLiteral("../share/boot-bitch/translations")),
        QDir(QCoreApplication::applicationDirPath())
            .absoluteFilePath(QStringLiteral("translations"))
    };

    QTranslator &translator = applicationTranslator();
    for (const QString &language : QLocale::system().uiLanguages()) {
        const QString fileName =
            QStringLiteral("boot-bitch_%1.qm").arg(QLocale(language).name());
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

    QCoreApplication::setOrganizationName(QStringLiteral("BootBitch"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("bootbitch.org"));
    QCoreApplication::setApplicationName(QStringLiteral("BootBitch"));
    QCoreApplication::setApplicationVersion(QString::fromLatin1(BOOT_BITCH_VERSION));
    QApplication::setDesktopFileName(QStringLiteral("org.bootbitch.BootBitch"));

    QIcon applicationIcon = QIcon::fromTheme(QStringLiteral("org.bootbitch.BootBitch"));
    if (applicationIcon.isNull()) {
        applicationIcon = QIcon(QStringLiteral(":/icons/org.bootbitch.BootBitch.png"));
    }
    QApplication::setWindowIcon(applicationIcon);

    // Load the UI translation before any window is constructed, so every
    // widget's tr() call resolves against the installed translator.
    installUiTranslator(&application);

    MainWindow window;
    window.show();

    return application.exec();
}
