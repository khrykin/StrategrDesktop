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

    auto overview::viewport_marker_for(rect viewport_rect, rect slotboard_rect) const -> viewport_marker {
        auto relative_top = viewport_rect.top - slotboard_rect.top;
        auto relative_bottom = slotboard_rect.top + slotboard_rect.height - viewport_rect.top - viewport_rect.height;
        auto marker_origin_x = std::round((float) relative_top / slotboard_rect.height * width);
        auto width_in_viewport = viewport_rect.height;

        auto viewport_end = viewport_rect.top + viewport_rect.height;
        auto slotboard_end = slotboard_rect.top + slotboard_rect.height;

        if (relative_top < 0)
            width_in_viewport = viewport_rect.height + relative_top;
        else if (relative_bottom < 0)
            width_in_viewport = viewport_rect.height + relative_bottom;

        auto marker_width = std::round((float) width_in_viewport / slotboard_rect.height * width);

        return viewport_marker{marker_origin_x, marker_width};
    }

    auto overview::current_time_position() -> stg::float_t {
        auto minutes = time_utils::current_minutes();
        auto rel = (float) (minutes - strategy.begin_time()) / strategy.duration();

        return std::round(rel * width);
    }
}
