#include "mainwindow.h"
#include "application.h"
#include "backends.h"

void setupCredentials() {
    QCoreApplication::setOrganizationName("Dmitry Khrykin");
    QCoreApplication::setOrganizationDomain("khrykin.com");
    QCoreApplication::setApplicationName("Strategr");
}

int main(int argc, char *argv[]) {
    setupCredentials();
    setupBackends();

    Application::setAttribute(Qt::AA_EnableHighDpiScaling);
    Application::setAttribute(Qt::AA_UseHighDpiPixmaps);

    Application a(argc, argv);

    return QApplication::exec();
}
