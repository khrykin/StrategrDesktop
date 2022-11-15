//
// Created by Dmitry Khrykin on 2019-07-13.
//

#include <iostream>

#include <QCoreApplication>

#include "notifierbackend.h"
#include "application.h"
#include "applicationicon.h"


#ifndef Q_OS_MAC
#include <QSystemTrayIcon>
#endif

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#ifdef Q_OS_UNIX
#include <QIcon>
#endif

#ifndef Q_OS_MAC
NotifierBackend::NotifierBackend() {

  if (!Application::trayIcon) {
    Application::trayIcon = new QSystemTrayIcon();

    // auto pmap = QPixmap(22, 22);
    // pmap.fill();
    // auto icon = QIcon(pmap);
    auto icon = QIcon("Strategr.ico");

    Application::trayIcon->setIcon(icon);
    Application::trayIcon->show();
  }

}
#endif


#ifndef Q_OS_MAC

void NotifierBackend::sendMessage(const QString &title, const QString &message) {
  if (Application::trayIcon && QSystemTrayIcon::supportsMessages()) {
    Application::trayIcon->showMessage(title, message, ApplicationIcon::defaultIcon(), 10000);
  } else {
    std::cout << "Error: Can't send notification\n";
  }
}

#endif
