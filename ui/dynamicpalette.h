//
// Created by Dmitry Khrykin on 2019-12-25.
//

#ifndef STRATEGR_DYNAMICPALETTE_H
#define STRATEGR_DYNAMICPALETTE_H

#include <map>
#include <functional>

#include <QPalette>
#include <QBrush>

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


#endif //STRATEGR_DYNAMICPALETTE_H
