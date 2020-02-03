//
// Created by Dmitry Khrykin on 2019-11-25.
//

#include "currenttimemarker.h"
#include "strategy.h"

stg::current_time_marker::current_time_marker(const stg::strategy &strategy)
        : strategy(strategy) {}

int stg::current_time_marker::top_offset_in(int total_height) const {
    auto relative = relative_position();
    if (relative < 0)
        return 0;

    if (relative > 1) {
        return 1;
    }

    return static_cast<int>(total_height * relative_position());
}

bool stg::current_time_marker::is_visible() const {
    auto relative = relative_position();
    return relative >= 0 && relative <= 1;
}

bool stg::current_time_marker::is_hidden() const {
    return !is_visible();
}

stg::rect stg::current_time_marker::rect_in_parent(const rect &parent_rect,
                                                   int marker_radius) const {
    return rect{
            parent_rect.left - marker_radius,
            parent_rect.top + top_offset_in(parent_rect.height) - marker_radius,
            parent_rect.width + marker_radius,
            2 * marker_radius
    };
}

int stg::current_time_marker::scroll_offset_in_parent(const rect &parent_rect,
                                                      int window_height) const {
    auto rect = rect_in_parent(parent_rect);
    auto top_offset = rect.top - window_height / 2;

    if (top_offset < 0) {
        top_offset = 0;
    } else if (top_offset > parent_rect.height) {
        top_offset = parent_rect.height;
    }

    return top_offset;
}

float stg::current_time_marker::relative_position() const {
    auto strategy_duration_seconds = strategy.duration() * 60;
    auto start_of_strategy_seconds = strategy.begin_time() * 60;
    return static_cast<float>(stg::time_utils::current_seconds() - start_of_strategy_seconds)
           / strategy_duration_seconds;
}
