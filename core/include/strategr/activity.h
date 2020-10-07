#ifndef STRATEGR_ACTIVITY_H
#define STRATEGR_ACTIVITY_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

#include "color.h"

namespace stg {
    struct activity {
        using color_info = std::pair<stg::color, std::string>;

        class invalid_property_exception;

        static constexpr auto default_color = stg::color::black_color;
        static auto default_colors() -> const std::vector<color_info> &;

        explicit activity(std::string name,
                          stg::color color = default_color) noexcept(false);

        auto name() const -> const std::string &;
        auto color() const -> const color &;

        auto with_name(const std::string &name) const -> activity;
        auto with_color(const stg::color &color) const -> activity;

        auto light_color() const -> stg::color;
        auto desaturated_light_color() const -> stg::color;
        auto desaturated_dark_color() const -> stg::color;

        auto to_json() const -> nlohmann::json;
        static auto from_json(const nlohmann::json &j) -> activity;

    private:
        std::string _name;
        stg::color _color;

        struct keys {
            static constexpr auto name = "name";
            static constexpr auto color = "color";
        };

        static auto empty_name_exception() -> invalid_property_exception;

        static auto is_valid(const std::string &name) -> bool;

        friend auto operator==(const activity &lhs, const activity &rhs) -> bool;
        friend auto operator!=(const activity &lhs, const activity &rhs) -> bool;
        friend auto operator<<(std::ostream &os, const activity &activity) -> std::ostream &;
    };
}

#endif// STRATEGR_ACTIVITY_H
