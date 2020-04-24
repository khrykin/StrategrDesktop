//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_CURRENTTIMEMARKER_H
#define STRATEGR_CURRENTTIMEMARKER_H

#include "time_utils.h"
#include "geometry.h"

namespace stg {
    class strategy;
    class current_time_marker {
    public:
        explicit current_time_marker(const strategy &strategy);

        auto is_visible() const -> bool;
        auto is_hidden() const -> bool;

        auto rect_in_parent(const rect &parent_rect,
                            int marker_radius = 0) const -> rect;

        auto scroll_offset_in_parent(const rect &parent_rect,
                                     int window_height) const -> int;
    private:
        const strategy &strategy;

        auto top_offset_in(int total_height) const -> int;
        auto relative_position() const -> float;
    };
}


#endif //STRATEGR_CURRENTTIMEMARKER_H
