//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEW_H
#define STRATEGR_OVERVIEW_H

#include <vector>
#include <cmath>
#include <optional>

#include "color.h"
#include "geometry.h"

namespace stg {
    class strategy;

    class overview {
    public:
        struct overview_item {
            stg::gfloat origin_x = 0;
            stg::gfloat width = 0;

            stg::color color;
        };

        struct viewport_marker {
            stg::gfloat origin_x = 0;
            stg::gfloat width = 0;
        };

        explicit overview(const strategy &strategy, stg::gfloat width);

        auto elements() -> std::vector<overview_item>;

        auto viewport_marker_for(rect slots_bounds, rect viewport_rect_in_slots) const -> viewport_marker;

        auto current_time_position() -> stg::gfloat;

    private:
        const strategy &strategy;
        stg::gfloat width = 0.0;
    };

}

#endif //STRATEGR_OVERVIEW_H
