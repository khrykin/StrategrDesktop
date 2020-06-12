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
        auto relative = relative_position();
        if (relative < 0)
            return 0;

        if (relative > 1) {
            return 1;
        }

        return (gfloat) total_height * relative_position();
    }

    auto current_time_marker::is_visible() const -> bool {
        auto relative = relative_position();
        return relative >= 0 && relative <= 1;
    }

    auto current_time_marker::is_hidden() const -> bool {
        return !is_visible();
    }

    auto current_time_marker::rect_in_parent(const rect &slotboard_rect) const -> rect {
        return rect{
                slotboard_rect.left - marker_radius,
                slotboard_rect.top + top_offset_in_slots(slotboard_rect.height) - marker_radius,
                slotboard_rect.width + marker_radius,
                2 * marker_radius
        };
    }

    auto current_time_marker::scroll_offset(const rect &slots_rect, rect viewport_rect) const -> gfloat {
        auto offset_in_slots = top_offset_in_slots(slots_rect.height);
        auto top_offset =
                offset_in_slots + slots_rect.top - viewport_rect.height / 2;

        auto max_top_offset = slots_rect.top + slots_rect.height - viewport_rect.height;

        if (top_offset < 0) {
            top_offset = 0;
        } else if (top_offset > max_top_offset) {
            top_offset = max_top_offset;
        }

        return top_offset;
    }

    auto current_time_marker::relative_position() const -> gfloat {
        auto strategy_duration_seconds = strategy.duration() * 60;
        auto start_of_strategy_seconds = strategy.begin_time() * 60;

        return static_cast<float>(time_utils::current_seconds() - start_of_strategy_seconds)
               / strategy_duration_seconds;
    }
}
