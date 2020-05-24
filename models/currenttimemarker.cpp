//
// Created by Dmitry Khrykin on 2019-11-25.
//

#include "currenttimemarker.h"
#include "strategy.h"

stg::current_time_marker::current_time_marker(const stg::strategy &strategy)
        : strategy(strategy) {}

auto stg::current_time_marker::top_offset_in(stg::float_t total_height) const -> stg::float_t {
    auto relative = relative_position();
    if (relative < 0)
        return 0;

    if (relative > 1) {
        return 1;
    }

    return (float_t) total_height * relative_position();
}

auto stg::current_time_marker::is_visible() const -> bool {
    auto relative = relative_position();
    return relative >= 0 && relative <= 1;
}

auto stg::current_time_marker::is_hidden() const -> bool {
    return !is_visible();
}

auto stg::current_time_marker::rect_in_parent(const rect &parent_rect,
                                              stg::float_t marker_radius) const -> stg::rect {
    return rect{
            parent_rect.left - marker_radius,
            parent_rect.top + top_offset_in(parent_rect.height) - marker_radius,
            parent_rect.width + marker_radius,
            2 * marker_radius
    };
}

auto stg::current_time_marker::scroll_offset(const rect &slots_rect,
                                             stg::float_t viewport_height) const -> stg::float_t {
    auto rect = rect_in_parent(slots_rect);
    auto top_offset = rect.top - viewport_height / 2;

    if (top_offset < 0) {
        top_offset = 0;
    } else if (top_offset > slots_rect.height - viewport_height) {
        top_offset = slots_rect.height - viewport_height;
    }

    return top_offset;
}

auto stg::current_time_marker::relative_position() const -> stg::float_t {
    auto strategy_duration_seconds = strategy.duration() * 60;
    auto start_of_strategy_seconds = strategy.begin_time() * 60;

    return static_cast<float>(stg::time_utils::current_seconds() - start_of_strategy_seconds)
           / strategy_duration_seconds;
}
