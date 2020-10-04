//
// Created by Dmitry Khrykin on 2019-08-01.
//

#ifndef STRATEGR_CURSORPROVIDER_H
#define STRATEGR_CURSORPROVIDER_H

#include <QCursor>

class CursorProvider {
protected:
    static QCursor resizeVerticalCursor();
    static QCursor closedHandCursor();
    static QCursor openHandCursor();
    static QCursor dragCopyCursor();
    static QFont iconFont();
    static QString searchIconText();
};


#endif//STRATEGR_CURSORPROVIDER_H
