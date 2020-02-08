//
// Created by Dmitry Khrykin on 2019-07-15.
//

#include <QFontDatabase>

#include "application.h"

QStringList Application::openedFiles = QStringList();

SelfUpdater Application::_updater = SelfUpdater();

AboutWindow *Application::aboutWindow = nullptr;

UpdateDialog *Application::updateDialog = nullptr;

Application::Application(int &argc, char **argv)
        : QApplication(argc, argv) {
    QTimer::singleShot(0, [=]() {
        if (!launchedByOpenEvent) {
            auto window = MainWindow::createLastOpened();
            window->show();
        }
    });

    QSettings().setProperty("lastLaunchedVersion",
                            ApplicationSettings::version);

    setupFonts();

#ifdef Q_OS_MAC
    setupCocoaDelegate();
#endif
}

bool Application::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        launchedByOpenEvent = true;

        auto openEvent = dynamic_cast<QFileOpenEvent *>(event);

        auto window = new MainWindow();
        window->loadFile(openEvent->file(), MainWindow::loadInCurrentWindow);
        window->show();
    }

    return QApplication::event(event);
}

void Application::setupFonts() {
    if (QFontDatabase::addApplicationFont(":/fonts/FontAwesome.otf") < 0)
        qWarning() << "FontAwesome cannot be loaded!";

    if (QFontDatabase::addApplicationFont(":/fonts/ionicons.ttf") < 0)
        qWarning() << "Ionicons cannot be loaded!";
}

const SelfUpdater &Application::updater() {
    return Application::_updater;
}

void Application::registerOpenedFile(const QString &filePath) {
    openedFiles.append(filePath);

#ifdef Q_OS_MAC
    registerCocoaRecentFile(filePath);
#endif
}

void Application::markFileClosed(const QString &filePath) {
    openedFiles.removeAll(filePath);
}

bool Application::fileIsOpened(const QString &filePath) {
    return openedFiles.indexOf(filePath) >= 0;
}
