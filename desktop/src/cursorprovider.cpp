//
// Created by Dmitry Khrykin on 2019-08-01.
//

#include "cursorprovider.h"
#include "applicationsettings.h"

#include <QFont>
#include <iostream>

#ifdef Q_OS_MAC

#include "macoswindow.h"
#include "utils.h"

#endif


QCursor CursorProvider::resizeVerticalCursor() {
#ifdef Q_OS_MAC
    auto pixmap = MacOSWindow::resizeCursor();
    pixmap.setDevicePixelRatio(devicePixelRatio());

    return QCursor(pixmap);
#else
    return QCursor(Qt::SizeVerCursor);
#endif
}

QCursor CursorProvider::closedHandCursor() {
#ifdef Q_OS_MAC
    auto pixmap = MacOSWindow::closedHandCursor();
    pixmap.setDevicePixelRatio(devicePixelRatio());

    return QCursor(pixmap);
#else
    return QCursor(Qt::ClosedHandCursor);
#endif
}

QCursor CursorProvider::openHandCursor() {
#ifdef Q_OS_MAC
    auto pixmap = MacOSWindow::openHandCursor();
    pixmap.setDevicePixelRatio(devicePixelRatio());

    return QCursor(pixmap);
#else
    return QCursor(Qt::OpenHandCursor);
#endif
}

QCursor CursorProvider::dragCopyCursor() {
    return QCursor(Qt::DragCopyCursor);
}

QFont CursorProvider::iconFont() {
    QFont iconFont;
    iconFont.setFamily(ApplicationSettings::fontResourcePath);

    return iconFont;
}

QString CursorProvider::searchIconText() {
    return u8"\uf002";
}
