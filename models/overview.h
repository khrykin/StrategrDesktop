//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_OVERVIEW_H
#define STRATEGR_OVERVIEW_H

#include <vector>
#include <cmath>
#include <optional>

#include "color.h"

namespace stg {
    class strategy;

    class overview {
    public:
        struct overview_item {
            int origin_x = 0;
            int width = 0;

            stg::color color;
        };

        struct viewport_marker {
            int origin_x = 0;
            int width = 0;
        };

        explicit overview(const strategy &strategy, float width);
        explicit overview(const strategy &strategy, int width);

        std::vector<overview_item> elements();

        viewport_marker viewport_marker_for(int viewport_height,
                                            int slotboard_height,
                                            int viewport_top_offset);
        viewport_marker viewport_marker_for(float viewport_height,
                                            float slotboard_height,
                                            float viewport_top_offset);

        int current_time_position();

    private:
        const strategy &strategy;
        float width = 0.0;
    };

}

#endif //STRATEGR_OVERVIEW_H
