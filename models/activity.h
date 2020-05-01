#ifndef STRATEGR_ACTIVITY_H
#define STRATEGR_ACTIVITY_H

#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <utility>

#include "color.h"

namespace stg {
    struct activity {
        using color_t = stg::color;
        using name_t = std::string;
        using color_info_t = std::pair<color_t, std::string>;

        class invalid_property_exception;

        static constexpr auto default_color = "#000000";
        static auto default_colors() -> const std::vector<color_info_t> &;

        explicit activity(name_t name, color_t color = default_color) noexcept(false);

        auto name() const -> const name_t &;
        auto color() const -> const color_t &;

        auto light_color() const -> color_t {
            auto clr = color();
            clr.set_alpha_component(0.15);

            return clr;
        }

        auto desaturated_light_color() const -> color_t {
            auto clr = color();
            clr.set_hsl(clr.hue(), 0.3, 0.75);
            clr.set_alpha_component(0.2);

            return clr;
        }

        auto desaturated_dark_color() const -> color_t {
            auto clr = color_t();

            if (color().lightness() < 0.2)
                clr = color_t(0xffffffff);

            clr.set_alpha_component(0.1);
            return clr;
        }

        auto with_name(const name_t &name) const -> activity;
        auto with_color(const color_t &color) const -> activity;

        friend auto operator==(const activity &lhs, const activity &rhs) -> bool;
        friend auto operator!=(const activity &lhs, const activity &rhs) -> bool;

        friend auto operator<<(std::ostream &os,
                               const activity &activity) -> std::ostream &;

        auto to_json() const -> nlohmann::json;
        static auto from_json(const nlohmann::json &j) -> activity;

    private:
        name_t _name;
        color_t _color;

        struct keys {
            static constexpr auto name = "name";
            static constexpr auto color = "color";
        };

        static auto empty_name_exception() -> invalid_property_exception;

        static auto is_valid(const name_t &name) -> bool;
    };
}

#endif // STRATEGR_ACTIVITY_H
