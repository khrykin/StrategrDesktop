//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_CURRENTTIMEMARKER_H
#define STRATEGR_CURRENTTIMEMARKER_H

#include "geometry.h"
#include "time_utils.h"

namespace stg {
    class strategy;
    class current_time_marker {
    public:
        gfloat marker_radius;

        explicit current_time_marker(const stg::strategy &strategy, gfloat marker_radius);

        auto is_visible() const -> bool;
        auto is_hidden() const -> bool;

        auto rect_in_slots_rect(const rect &slotboard_rect) const -> rect;

        auto scroll_offset(const rect &slots_rect, rect viewport_rect) const -> stg::gfloat;

    private:
        const strategy &strategy;

        auto top_offset_in_slots(stg::gfloat total_height) const -> stg::gfloat;
    };
}


#endif//STRATEGR_CURRENTTIMEMARKER_H
