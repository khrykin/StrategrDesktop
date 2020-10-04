//
// created by dmitry khrykin on 2019-08-15.
//

#include "overview.h"
#include "strategy.h"

namespace stg {
    overview::overview(const stg::strategy &strategy, std::function<gfloat()> width_getter)
        : strategy(strategy),
          width_getter(std::move(width_getter)) {
    }

    auto overview::elements() const -> std::vector<overview::overview_item> {
        const auto &activity_sessions = strategy.sessions();
        std::vector<overview_item> result;

        auto prev_origin_x = 0;
        for (auto &item : activity_sessions.overview()) {
            auto current_width = std::round(item.duration_percentage * width());
            auto origin_x = std::round(item.begin_percentage * width());

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

    auto overview::viewport_marker_for(const rect &slots_bounds,
                                       const rect &viewport_rect) const -> viewport_marker {
        //        std::cout << "slots_bounds: " << slots_bounds << "\n";
        //        std::cout << "viewport_rect: " << viewport_rect << "\n";

        auto relative_top = viewport_rect.top;
        auto relative_bottom = slots_bounds.height - viewport_rect.top - viewport_rect.height;
        auto marker_origin_x = std::round((float) relative_top / slots_bounds.height * width());
        auto width_in_viewport = viewport_rect.height;

        if (relative_top < 0) {
            width_in_viewport = viewport_rect.height + relative_top;
            marker_origin_x = 0;
        } else if (relative_bottom < 0) {
            width_in_viewport = viewport_rect.height + relative_bottom;
        }

        auto marker_width = std::round((float) width_in_viewport / slots_bounds.height * width());

        return viewport_marker{marker_origin_x, marker_width};
    }

    auto overview::current_time_position() const -> gfloat {
        return std::round(strategy.progress() * width());
    }

    auto overview::scroll_offset_for(point mouse_position,
                                     const rect &slots_bounds,
                                     const rect &viewport_rect) const -> gfloat {
        auto percentage = (gfloat) mouse_position.x / (gfloat) width();
        auto relative_distance = percentage * slots_bounds.height;

        return relative_distance - viewport_rect.height / 2;
    }

    gfloat overview::width() const {
        return width_getter();
    }
}
