//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QApplication>
#include <QTimer>

#include "application.h"
#include "mainwindow.h"
#include "windowgeometrymanager.h"

QWidgetList WindowGeometryManager::windows;

void WindowGeometryManager::setInitialGeometry(MainWindow *window) {
    window->setMinimumWidth(ApplicationSettings::windowMinimumWidth);
    window->setMinimumHeight(ApplicationSettings::windowMinimumHeight);

    auto &settings = Application::currentSettings();

    auto storedRect = settings.value(windowGeometrySetting).toRect();

    if (storedRect.isValid()) {
        window->setGeometry(storedRect);
    } else {
        window->setGeometry(defaultInitialRect(window));
    }

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

#ifdef Q_OS_MAC
    // Attaching the window to the native run loop (which happens
    // asynchronously right after this call returns, before the window is
    // shown) silently resets the vertical position -- macOS's native
    // toolbar/titlebar layout pass overrides it once, discarding the
    // geometry set above. Re-apply on the next event loop iteration to
    // correct for it; this sticks permanently once done.
    auto target = window->geometry();
    QTimer::singleShot(0, window, [window, target]() {
        window->setGeometry(target);
    });
#endif
}

void WindowGeometryManager::saveGeometry(MainWindow *window) {
    windows.removeAll(window);

    Application::currentSettings().setValue(windowGeometrySetting, window->geometry());
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
    return widget->screen()->availableGeometry();
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
