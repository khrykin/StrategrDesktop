//
// Created by Dmitry Khrykin on 2019-08-12.
//

#ifndef STRATEGR_APPLICATIONICON_H
#define STRATEGR_APPLICATIONICON_H

#include <QPixmap>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class ApplicationIcon {
public:
    static const auto size = 64;
    static QPixmap defaultIcon();

#ifdef Q_OS_WIN
    // QtWinExtras (and QtWin::fromHICON with it) was removed in Qt6, so
    // HICON -> QPixmap needs to go through GDI manually.
    static QPixmap fromHICON(HICON hicon);
#endif
};


#endif//STRATEGR_APPLICATIONICON_H
