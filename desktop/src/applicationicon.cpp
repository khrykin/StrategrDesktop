//
// Created by Dmitry Khrykin on 2019-08-12.
//

#include <QApplication>
#include <QIcon>
#include <QImage>

#include "applicationicon.h"
#include "utils.h"

#ifdef Q_OS_WIN

QPixmap ApplicationIcon::fromHICON(HICON hicon) {
    ICONINFO iconInfo;
    if (!GetIconInfo(hicon, &iconInfo))
        return {};

    BITMAP bitmap;
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bitmap);

    QImage image(bitmap.bmWidth, bitmap.bmHeight, QImage::Format_ARGB32);

    BITMAPINFOHEADER header = {};
    header.biSize = sizeof(BITMAPINFOHEADER);
    header.biWidth = bitmap.bmWidth;
    header.biHeight = -bitmap.bmHeight;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;

    HDC hdc = GetDC(nullptr);
    GetDIBits(hdc,
              iconInfo.hbmColor,
              0,
              bitmap.bmHeight,
              image.bits(),
              reinterpret_cast<BITMAPINFO *>(&header),
              DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);

    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);

    return QPixmap::fromImage(image);
}

#endif

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
                                              L"IDI_ICON1",
                                              IMAGE_ICON,
                                              devicePixelRatio() * size,
                                              devicePixelRatio() * size,
                                              LR_DEFAULTCOLOR));
    auto icon = fromHICON(hicon);
    icon.setDevicePixelRatio(devicePixelRatio());

    return icon;
}

#endif
