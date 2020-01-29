//
// created by dmitry khrykin on 2019-08-15.
//

#include "overview.h"
#include "strategy.h"
#include "time_utils.h"

stg::overview::overview(const stg::strategy &strategy, float width) :
        strategy(strategy), width(width) {
}

stg::overview::overview(const stg::strategy &strategy, int width) :
        strategy(strategy), width(static_cast<float>(width)) {
}

std::vector<stg::overview::overview_item>
stg::overview::elements() {
    auto &activity_sessions = strategy.sessions();
    std::vector<overview_item> result;

    auto prev_origin_x = 0;
    for (auto &item : activity_sessions.overview()) {
        auto current_width = std::lround(item.duration_percentage * width);
        auto origin_x = std::lround(item.begin_percentage * width);

        if (origin_x != prev_origin_x) {
            current_width += origin_x - prev_origin_x;
            origin_x = prev_origin_x;
        }

        if (item.color) {
            result.push_back(overview_item{static_cast<int>(origin_x),
                                           static_cast<int>(current_width),
                                           *item.color});
        }

        prev_origin_x = origin_x + current_width;
    }

    return result;

}

stg::overview::viewport_marker stg::overview::viewport_marker_for(int viewport_height,
                                                                  int slotboard_height,
                                                                  int viewport_top_offset) {

    return viewport_marker_for(static_cast<float>(viewport_height),
                               static_cast<float>(slotboard_height),
                               static_cast<float>(viewport_top_offset));
}

stg::overview::viewport_marker stg::overview::viewport_marker_for(float viewport_height,
                                                                  float slotboard_height,
                                                                  float viewport_top_offset) {
    auto viewport_marker_origin_x = std::roundl(viewport_top_offset / slotboard_height * width);
    auto viewport_marker_width = std::roundl(viewport_height / slotboard_height * width);

    return viewport_marker{
            static_cast<int>(viewport_marker_origin_x),
            static_cast<int>(viewport_marker_width)};
}

int stg::overview::current_time_position() {
    auto minutes = stg::time_utils::current_minutes();
    auto rel = static_cast<float>(minutes - strategy.begin_time()) / strategy.duration();

    return static_cast<int>(std::roundl(rel * width));
}


