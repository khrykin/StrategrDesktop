#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("Dmitry Khrykin");
  QCoreApplication::setOrganizationDomain("khrykin.com");
  QCoreApplication::setApplicationName("Strategr");
  QApplication a(argc, argv);

  if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") < 0)
    qWarning() << "FontAwesome cannot be loaded !";

  if (QFontDatabase::addApplicationFont(":/fonts/ionicons.ttf") < 0)
    qWarning() << "FontAwesome cannot be loaded !";

  MainWindow w;
  w.show();

  return a.exec();
}
