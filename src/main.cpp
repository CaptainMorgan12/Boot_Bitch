#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QIcon>

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

    MainWindow window;
    window.show();

    return application.exec();
}
