//
// Created by Dmitry Khrykin on 2019-12-25.
//

#ifndef STRATEGR_DYNAMICPALETTE_H
#define STRATEGR_DYNAMICPALETTE_H

#include <functional>
#include <map>

#include <QBrush>
#include <QPalette>

#include "colorprovider.h"

class DynamicPalette : public QPalette, public ColorProvider {
public:
    explicit DynamicPalette(const QPalette &palette) : QPalette(palette) {
    }

    const QBrush text() const {
        return textColorJustLighter();
    }

private:
    std::map<QPalette::ColorRole, std::function<QColor()>> colorGetters;
};


#endif//STRATEGR_DYNAMICPALETTE_H
