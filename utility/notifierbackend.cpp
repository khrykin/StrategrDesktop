//
// Created by Dmitry Khrykin on 2019-07-13.
//

#include "notifierbackend.h"

#ifndef Q_OS_MAC
NotifierBackend::NotifierBackend(QSystemTrayIcon *trayIcon, QObject *parent)
        : QObject(parent), trayIcon(trayIcon) {
}

void NotifierBackend::sendMessage(const QString &title, const QString &message) {
  if (QSystemTrayIcon::supportsMessages()) {
    trayIcon->showMessage(title, message, QIcon(), 10000);
  }
}
#endif