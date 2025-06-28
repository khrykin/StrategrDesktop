#include "application.h"
#include "backends.h"
#include "mainwindow.h"
#include "utils.h"
#include <QStyleFactory>

void setupCredentials() {
    QCoreApplication::setOrganizationName("Dmitry Khrykin");
    QCoreApplication::setOrganizationDomain("khrykin.com");
    QCoreApplication::setApplicationName("Strategr");
}

QString getNativeStyle() {
#ifdef Q_OS_MAC
    return "macos";
#elif defined(Q_OS_WIN)
    return "windows";
#else
    // On Linux, try to use the desktop environment's native style
    const QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("gtk3")) {
        return "gtk3";
    } else if (availableStyles.contains("gtk2")) {
        return "gtk2";
    } else if (availableStyles.contains("fusion")) {
        return "fusion";
    }
    return "fusion";
#endif
}

int main(int argc, char *argv[]) {
    setupCredentials();
    setupBackends();

    Application app(argc, argv);
    app.setStyle(getNativeStyle());

    return app.exec();
}
