//
// created by dmitry khrykin on 2019-08-15.
//

#include "overview.h"
#include "strategy.h"
#include "time_utils.h"

namespace stg {
    overview::overview(const stg::strategy &strategy, stg::float_t width) :
            strategy(strategy), width(width) {
    }

    auto overview::elements() -> std::vector<overview::overview_item> {
        const auto &activity_sessions = strategy.sessions();
        std::vector<overview_item> result;

        auto prev_origin_x = 0;
        for (auto &item : activity_sessions.overview()) {
            auto current_width = std::round(item.duration_percentage * width);
            auto origin_x = std::round(item.begin_percentage * width);

            if (origin_x != prev_origin_x) {
                current_width += origin_x - prev_origin_x;
                origin_x = prev_origin_x;
            }

            if (item.color) {
                result.push_back(overview_item{origin_x, current_width, *item.color});
            }

            prev_origin_x = origin_x + current_width;
        }

        return result;

    }

    auto overview::viewport_marker_for(stg::float_t viewport_height,
                                       stg::float_t slotboard_height,
                                       stg::float_t viewport_top_offset) const -> viewport_marker {
        auto marker_origin_x = std::round(viewport_top_offset / slotboard_height * width);
        auto marker_width = std::round(viewport_height / slotboard_height * width);

        return viewport_marker{marker_origin_x, marker_width};
    }

    auto overview::current_time_position() -> stg::float_t {
        auto minutes = time_utils::current_minutes();
        auto rel = (float) (minutes - strategy.begin_time()) / strategy.duration();

        return std::round(rel * width);
    }
}
