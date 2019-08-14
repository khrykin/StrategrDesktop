//
// Created by Dmitry Khrykin on 2019-07-15.
//

#include <QFontDatabase>

#include "application.h"

auto Application::openedFiles = QStringList();

Application::Application(int &argc, char **argv)
        : QApplication(argc, argv) {
    QTimer::singleShot(0, [=]() {
        if (!launchedByOpenEvent) {
            auto window = MainWindow::createLastOpened();
            window->show();
        }
    });

    Application::setAttribute(Qt::AA_EnableHighDpiScaling);
    Application::setAttribute(Qt::AA_UseHighDpiPixmaps);

    setupFonts();

#ifdef Q_OS_MAC
    setupCocoaDelegate();
#endif
}

bool Application::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        launchedByOpenEvent = true;

        auto openEvent = static_cast<QFileOpenEvent *>(event);

        auto window = new MainWindow();
        window->loadFile(openEvent->file(), MainWindow::LoadInCurrentWindow);
        window->show();
    }

    return QApplication::event(event);
}

void Application::setupFonts() {
    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    if (QFontDatabase::addApplicationFont(":/fonts/ionicons.ttf") < 0)
        qWarning() << "FontAwesome cannot be loaded !";
}
