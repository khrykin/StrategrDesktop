//
// Created by Dmitry Khrykin on 2019-08-12.
//

#include <QApplication>

#include "applicationicon.h"

#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
QPixmap ApplicationIcon::defaultIcon() {
    return QApplication::windowIcon().pixmap(QSize(size, size));
}
#endif

#ifdef Q_OS_WIN
#include <QIcon>

QPixmap ApplicationIcon::defaultIcon() {
    auto icon = QIcon(QCoreApplication::applicationDirPath() + "\\Strategr.ico");
    return icon.pixmap(QSize(size, size));
}
#endif

