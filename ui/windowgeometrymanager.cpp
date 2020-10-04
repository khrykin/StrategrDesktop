//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QApplication>
#include <QDesktopWidget>

#include "application.h"
#include "macoswindow.h"
#include "mainwindow.h"
#include "windowgeometrymanager.h"

QWidgetList WindowGeometryManager::windows;

void WindowGeometryManager::setInitialGeometry(MainWindow *window) {
    window->setMinimumWidth(ApplicationSettings::windowMinimumWidth);
    window->setMinimumHeight(ApplicationSettings::windowMinimumHeight);

    auto &settings = Application::currentSettings();

    if (settings.value(windowGeometrySetting).isNull()) {
        window->setGeometry(defaultInitialRect(window));
    } else {
        auto storedData = settings.value(windowGeometrySetting).toByteArray();
        window->restoreGeometry(storedData);
    }

#ifdef Q_OS_MAC
    window->setGeometry(MacOSWindow::adjustedGeometry(window));
#endif

    if (windows.count() > 0) {
        auto fixedGeometry = window->geometry();
        auto width = window->geometry().width();
        fixedGeometry.setLeft(minLeft() - width);

        if (fixedGeometry.left() < 0)
            fixedGeometry.setLeft(maxRight());

        fixedGeometry.setWidth(width);

        window->setGeometry(fixedGeometry);
    }

    windows.append(window);
}

void WindowGeometryManager::saveGeometry(MainWindow *window) {
    windows.removeAll(window);

    Application::currentSettings().setValue(windowGeometrySetting, window->saveGeometry());
}

void WindowGeometryManager::resetSavedGeometry() {
    Application::currentSettings().remove(windowGeometrySetting);
}

QRect WindowGeometryManager::defaultInitialRect(QWidget *window) {
    using namespace ApplicationSettings;

    const auto desktopGeometry = avaliableGeometry(window);

    const auto titlebarHeight = 30;
    const auto windowInitialHeight = desktopGeometry.height() - titlebarHeight;
    const auto windowInitialLeft = (desktopGeometry.width() - windowInitialWidth) / 2;

    return {windowInitialLeft,
            titlebarHeight,
            windowInitialWidth,
            windowInitialHeight};
}

QRect WindowGeometryManager::avaliableGeometry(QWidget *widget) {
    return QDesktopWidget().availableGeometry(widget);
}

int WindowGeometryManager::minLeft() {
    auto leftWindow = *std::min_element(windows.begin(), windows.end(),
                                        [](QWidget *a, QWidget *b) {
                                            return a->geometry().left() < b->geometry().left();
                                        });

    return leftWindow->geometry().left();
}

int WindowGeometryManager::maxRight() {
    auto rightWindow = *std::max_element(windows.begin(), windows.end(),
                                         [](QWidget *a, QWidget *b) {
                                             return a->geometry().right() < b->geometry().right();
                                         });

    return rightWindow->geometry().right();
}
