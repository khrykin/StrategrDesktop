#include "ui/mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QWidget>
#include <QLayout>
#include <QLabel>

void setupCredentials() {
    QCoreApplication::setOrganizationName("Dmitry Khrykin");
    QCoreApplication::setOrganizationDomain("khrykin.com");
    QCoreApplication::setApplicationName("Strategr");
}

void setupFonts() {
    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    if (QFontDatabase::addApplicationFont(":/fonts/ionicons.ttf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";
}


int main(int argc, char *argv[]) {
    setupCredentials();
    QApplication a(argc, argv);

    // NB! Must be called after QApplication()
    setupFonts();

    MainWindow w;
    w.openRecentOrCreateNew();
    w.show();

    return a.exec();
}
