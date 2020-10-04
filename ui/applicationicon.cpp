//
// Created by Dmitry Khrykin on 2019-08-12.
//

#include <QApplication>
#include <QIcon>

#ifdef Q_OS_WIN
#include <QtWinExtras>
#endif

#include "applicationicon.h"
#include "utils.h"

#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)

QPixmap ApplicationIcon::defaultIcon() {
    auto pixmap = QApplication::windowIcon().pixmap(QSize(size, size));
    pixmap.setDevicePixelRatio(devicePixelRatio());

    return pixmap;
}

#endif

#ifdef Q_OS_WIN

QPixmap ApplicationIcon::defaultIcon() {
    auto hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));
    auto hicon = static_cast<HICON>(LoadImage(hInstance,
                                              "IDI_ICON1",
                                              IMAGE_ICON,
                                              devicePixelRatio() * size,
                                              devicePixelRatio() * size,
                                              LR_DEFAULTCOLOR));
    auto icon = QtWin::fromHICON(hicon);
    icon.setDevicePixelRatio(devicePixelRatio());

    return icon;
}

#endif
