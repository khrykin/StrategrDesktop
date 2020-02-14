//
// Created by Dmitry Khrykin on 2019-07-13.
//

#include <iostream>

#include <QCoreApplication>

#include "notifierbackend.h"
#include "application.h"
#include "applicationicon.h"

#ifdef Q_OS_WIN

#include <Windows.h>
#include <QtWinExtras>

#endif

NotifierBackend::NotifierBackend() {
#ifdef Q_OS_WIN

    if (!Application::trayIcon) {
        Application::trayIcon = new QSystemTrayIcon();

        auto hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));
        auto hicon = static_cast<HICON>(LoadIcon(hInstance,"IDI_ICON1"));
        auto icon = QtWin::fromHICON(hicon);

        Application::trayIcon->setIcon(icon);
        Application::trayIcon->show();
    }

#endif
}

#ifndef Q_OS_MAC

void NotifierBackend::sendMessage(const QString &title, const QString &message) {
#ifdef Q_OS_WIN

    if (Application::trayIcon && QSystemTrayIcon::supportsMessages()) {
        Application::trayIcon->showMessage(title, message, ApplicationIcon::defaultIcon(), 10000);
    } else {
        std::cout << "Error: Can't send no tification\n";
    }

#endif
}

#endif
