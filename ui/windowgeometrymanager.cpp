//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QApplication>

#include "windowgeometrymanager.h"
#include "mainwindow.h"
#include "macoswindow.h"

QWidgetList WindowGeometryManager::windows;

void WindowGeometryManager::setInitialGeometry(MainWindow *window) {
    windows.append(window);

    window->setMinimumWidth(ApplicationSettings::windowMinimumWidth);
    window->setMinimumHeight(ApplicationSettings::windowMinimumHeight);

    auto initValue = QSettings().value(windowGeometrySetting);
    auto initRect = initValue.isValid()
                    ? initValue.toRect()
                    : defaultInitialRect(window);

    if (windows.count() > 1) {
        initRect = initRectForExtraWindows(initRect, window);
    }

    window->setGeometry(initRect);
}

void WindowGeometryManager::saveGeometry(MainWindow *window) {
    windows.removeAll(window);

    auto geometry = window->geometry();

#ifdef Q_OS_MAC
    geometry = MacOSWindow::geometry(window);
#endif

    QSettings().setValue(windowGeometrySetting, geometry);
}

void WindowGeometryManager::resetSavedGeometry() {
    QSettings().remove(windowGeometrySetting);
}

QRect WindowGeometryManager::defaultInitialRect(QWidget *window) {
    using namespace ApplicationSettings;

    const auto availableSize = avaliableDesktopSize(window);

    const auto windowInitialHeight = availableSize.height();
    const auto windowInitialLeft =
            (availableSize.width() - windowInitialWidth) / 2;

    return QRect(windowInitialLeft,
                 0,
                 windowInitialWidth,
                 windowInitialHeight);

}

QSize WindowGeometryManager::avaliableDesktopSize(QWidget *widget) {
    return QDesktopWidget().availableGeometry(widget).size();
}

QRect WindowGeometryManager::initRectForExtraWindows(const QRect &defaultRect,
                                                     QWidget *window) {
    auto adjustFactor = defaultAdjustFactor * (windows.count() - 1);
    auto initRect = QRect(defaultRect.x() + adjustFactor,
                          defaultRect.y(),
                          defaultRect.width(),
                          defaultRect.height()
    );

    if (initRect.right() > avaliableDesktopSize(window).width()) {
        initRect.setLeft(0);
    }

    if (initRect.left() < 0) {
        initRect.setLeft(0);
    }


    return initRect;
}
