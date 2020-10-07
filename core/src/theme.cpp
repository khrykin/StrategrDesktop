//
// Created by Dmitry Khrykin on 26/04/2020.
//

#include "theme.h"

namespace stg {

    auto theme::text_color() const -> color {
        return get_text_color();
    }

    auto theme::base_color() const -> color {
        return get_base_color();
    }

    auto theme::session_background_color(const session &session, bool is_selected) const -> color {
        if (!session.activity)
            return color::clear_color;

        auto activity_color = session.activity->color();

        if (activity_color.lightness() < 0.5 &&
            is_dark_mode()) {
            activity_color.set_hsl(activity_color.hue(), activity_color.saturation(), 0.2);
        }

        return is_selected
                   ? activity_color
                   : activity_color.with_alpha_component(0.15);
    }

    auto theme::session_ruler_color(const session &session, bool is_selected) const -> color {
        return is_selected
                   ? session.activity->desaturated_dark_color()
                   : session.activity->desaturated_light_color();
    }

    auto theme::session_duration_color(const session &session, bool is_selected) const -> color {
        if (!session.activity)
            return color::clear_color;

        const auto activity_color = session.activity->color();
        auto default_duration_color = text_color()
                                          .blended_with(activity_color
                                                            .with_hsl(activity_color.hue(), 0.2, 0.7)
                                                            .with_alpha_component(0.6));

        auto selected_duration_color = base_color()
                                           .blended_with(activity_color

                                                             .with_alpha_component(0.2));

        auto duration_color = is_selected
                                  ? selected_duration_color
                                  : default_duration_color;


        if (session.activity->color().lightness() < 0.2 && is_selected) {
            duration_color = color::white_color
                                 .blended_with(session.activity->color().with_alpha_component(0.5));
        }

        return duration_color;
    }

    auto theme::session_title_color(const session &session, bool is_selected) const -> color {
        auto activity_color = session.activity->color();
        auto lightened_activity_color = activity_color.with_hsl(activity_color.hue(),
                                                                activity_color.saturation(),
                                                                1 - activity_color.lightness());

        if (is_selected) {
            return color::white_color;
        } else {
            if (activity_color.lightness() < 0.5 && is_dark_mode())
                return lightened_activity_color;

            return session.activity->color();
        }
    }

    auto theme::is_dark_mode() const -> bool {
        return text_color().lightness() > 0.2;
    }

}
