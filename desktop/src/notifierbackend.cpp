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

#if !defined(Q_OS_MAC) || defined(STG_UI_TEST)

NotifierBackend::NotifierBackend() {
    if (!Application::trayIcon) {
        Application::trayIcon = new QSystemTrayIcon();

        auto icon = ApplicationIcon::defaultIcon();

#ifdef Q_OS_WIN

        auto hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));
        auto hicon = static_cast<HICON>(LoadIcon(hInstance,"IDI_ICON1"));
        icon = QtWin::fromHICON(hicon);

#endif
        Application::trayIcon->setIcon(icon);
        Application::trayIcon->show();
    }
}

void NotifierBackend::sendMessage(const QString &title, const QString &message) const {
    if (Application::trayIcon && QSystemTrayIcon::supportsMessages()) {
        Application::trayIcon->showMessage(title, message, ApplicationIcon::defaultIcon(), 10000);
    } else {
        std::cout << "Error: Can't send notification\n";
    }
}

#endif
