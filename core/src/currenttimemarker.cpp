//
// Created by Dmitry Khrykin on 2019-11-25.
//

#include "currenttimemarker.h"
#include "strategy.h"

namespace stg {
    current_time_marker::current_time_marker(const stg::strategy &strategy, gfloat marker_radius)
        : strategy(strategy),
          marker_radius(marker_radius) {}

    auto current_time_marker::top_offset_in_slots(gfloat total_height) const -> gfloat {
        auto slot_height = total_height / (gfloat)(strategy.number_of_time_slots() + 1);

        return (total_height - slot_height) * strategy.progress() + slot_height / 2;
    }

    auto current_time_marker::is_visible() const -> bool {
        auto relative = strategy.progress();

        return relative > 0 && relative < 1;
    }

    auto current_time_marker::is_hidden() const -> bool {
        return !is_visible();
    }

    auto current_time_marker::rect_in_slots_rect(const rect &slotboard_rect) const -> rect {
        return rect{
            slotboard_rect.left - marker_radius,
            slotboard_rect.top + top_offset_in_slots(slotboard_rect.height) - marker_radius,
            slotboard_rect.width + marker_radius,
            2 * marker_radius};
    }

    auto current_time_marker::scroll_offset(const rect &slots_rect, rect viewport_rect) const -> gfloat {
        auto offset_in_slots = top_offset_in_slots(slots_rect.height);
        auto top_offset = offset_in_slots - viewport_rect.height / 2;
        auto max_top_offset = slots_rect.height - viewport_rect.height;

        if (top_offset < 0) {
            top_offset = 0;
        } else if (top_offset > max_top_offset) {
            top_offset = max_top_offset;
        }

        return top_offset;
    }
}
