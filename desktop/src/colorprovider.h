//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_COLORPROVIDER_H
#define STRATEGR_COLORPROVIDER_H


#include <QApplication>
#include <QColor>
#include <QPalette>

#include "colorutils.h"

class ColorProvider {
public:
    static QColor baseColor();
    static QColor windowColor();
    static QColor panelColor();
    static QColor darkPanelColor();
    static QColor borderColor();
    static QColor hardBorderColor();
    static QColor highlightColor();
    static QColor highlightedTextColor();
    static QColor selectionColor();
    static QColor controlColor();
    static QColor textColor();
    static QColor textColorJustLighter();
    static QColor secondaryTextColor();
    static QColor tertiaryTextColor();
};


#endif//STRATEGR_COLORPROVIDER_H
