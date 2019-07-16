#include "ui/mainwindow.h"

#include <QFontDatabase>
#include <QDebug>
#include <QWidget>
#include <QLayout>
#include <QLabel>

#include "application.h"
#include "backtrace.h"

void setupCredentials() {
    QCoreApplication::setOrganizationName("Dmitry Khrykin");
    QCoreApplication::setOrganizationDomain("khrykin.com");
    QCoreApplication::setApplicationName("Strategr");
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, handler);

    setupCredentials();
    Application a(argc, argv);

    return QApplication::exec();
}
