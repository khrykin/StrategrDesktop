//
// Created by Dmitry Khrykin on 2019-08-01.
//

#ifndef STRATEGR_CURSORPROVIDER_H
#define STRATEGR_CURSORPROVIDER_H

#include <QCursor>

class CursorProvider {
public:
    QCursor resizeVerticalCursor();
    QCursor closedHandCursor();
    QCursor openHandCursor();
    QCursor dragCopyCursor();
};


#endif //STRATEGR_CURSORPROVIDER_H
