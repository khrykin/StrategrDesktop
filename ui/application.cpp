//
// Created by Dmitry Khrykin on 2019-07-15.
//

#include "application.h"
#include <QFontDatabase>

auto Application::openedFiles = QStringList();

Application::Application(int &argc, char **argv)
        : QApplication(argc, argv) {
    QTimer::singleShot(0, [=]() {
        if (!launchedByOpenEvent) {
            auto window = MainWindow::createLastOpened();
            window->show();
        }
    });
}

bool Application::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        launchedByOpenEvent = true;

        auto openEvent = static_cast<QFileOpenEvent *>(event);
        auto window = new MainWindow();

        window->loadFile(openEvent->file(), MainWindow::LoadInCurrentWindow);
    }

    return QApplication::event(event);
}

void Application::setupFonts() {
    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    if (QFontDatabase::addApplicationFont(":/fonts/ionicons.ttf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";
}
