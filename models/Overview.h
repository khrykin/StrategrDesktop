//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEW_H
#define STRATEGR_OVERVIEW_H

#include <vector>
#include <cmath>
#include <optional>

#include "Strategy.h"

class Overview {
public:
    struct OverviewItem {
        int originX = 0;
        int width = 0;

        Activity::Color color;
    };

    struct ViewportMarker {
        int originX = 0;
        int width = 0;
    };

    explicit Overview(const Strategy &strategy, float width);
    explicit Overview(const Strategy &strategy, int width);

    std::vector<Overview::OverviewItem> elements();

    ViewportMarker viewportMarkerFor(int viewportHeight,
                                     int slotboardHeight,
                                     int viewportTopOffset);
    ViewportMarker viewportMarkerFor(float viewportHeight,
                                     float slotboardHeight,
                                     float viewportTopOffset);

    int currentTimePosition();;

private:
    const Strategy &strategy;
    float width = 0.0;
};


#endif //STRATEGR_OVERVIEW_H
