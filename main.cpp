#include "mainwindow.h"
#include "application.h"

void setupCredentials() {
    QCoreApplication::setOrganizationName("Dmitry Khrykin");
    QCoreApplication::setOrganizationDomain("khrykin.com");
    QCoreApplication::setApplicationName("Strategr");
}

int main(int argc, char *argv[]) {
    setupCredentials();
    Application a(argc, argv);

    return QApplication::exec();
}
