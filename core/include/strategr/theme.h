//
// Created by Dmitry Khrykin on 26/04/2020.
//

#ifndef STRATEGR_THEME_H
#define STRATEGR_THEME_H

#include <functional>
#include <utility>

#include "activity.h"
#include "color.h"
#include "session.h"

namespace stg {
    struct theme {
        std::function<color()> get_text_color = [] { return color::black_color; };
        std::function<color()> get_base_color = [] { return color::white_color; };

        auto text_color() const -> color;
        auto base_color() const -> color;
        auto is_dark_mode() const -> bool;

        auto session_background_color(const session &session, bool is_selected) const -> color;
        auto session_ruler_color(const session &session, bool is_selected) const -> color;
        auto session_duration_color(const session &session, bool is_selected) const -> color;
        auto session_title_color(const session &session, bool is_selected) const -> color;
    };
}

#endif//STRATEGR_THEME_H
