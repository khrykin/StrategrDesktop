//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEW_H
#define STRATEGR_OVERVIEW_H

#include <vector>
#include <cmath>
#include <optional>

#include "strategy.h"

class Overview {
public:
    struct OverviewItem {
        int originX = 0;
        int width = 0;

        Activity::Color color;
    };

    explicit Overview(const Strategy &strategy);

    std::vector<const OverviewItem> elementsForWidth(int width);
    std::vector<const OverviewItem> elementsForWidth(float width = 0.0);

private:
    const Strategy &strategy;
};


#endif //STRATEGR_OVERVIEW_H
