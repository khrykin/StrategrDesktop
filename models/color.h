//
// Created by Dmitry Khrykin on 2020-02-18.
//

#ifndef STRATEGR_COLOR_H
#define STRATEGR_COLOR_H

#include <string>
#include <algorithm>
#include <array>
#include <ostream>

#if __APPLE__
struct CGColor;
#endif

namespace stg {
    struct color {
        template<typename T,
                typename = int,
                typename = int,
                typename = int,
                typename = int>
        struct is_qcolor_like : std::false_type {
        };

        template<typename T>
        struct is_qcolor_like<T,
                decltype(std::declval<T>().red()),
                decltype(std::declval<T>().green()),
                decltype(std::declval<T>().blue()),
                decltype(std::declval<T>().alpha())> : std::true_type {
        };

        explicit color(uint32_t data = 0xff000000);

        color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);

        color(const char *str);
        color(const std::string &str);

        template<typename T, std::enable_if_t<is_qcolor_like<T>::value, int> = 0>
        color(const T &q_color_like) {
            set_rgba(q_color_like.red(),
                     q_color_like.green(),
                     q_color_like.blue(),
                     q_color_like.alpha());
        }

        auto operator=(const std::string &str) -> color &;
        operator std::string() const;

        template<typename T, std::enable_if_t<is_qcolor_like<T>::value, int> = 0>
        operator T() const {
            return T(red(), green(), blue(), alpha());
        }

        static auto clear_color() -> color;
        static auto black_color() -> color;
        static auto white_color() -> color;

        auto red() const -> uint8_t;
        auto green() const -> uint8_t;
        auto blue() const -> uint8_t;
        auto alpha() const -> uint8_t;

        auto red_component() const -> float;
        auto green_component() const -> float;
        auto blue_component() const -> float;
        auto alpha_component() const -> float;

        void set_red(uint8_t value);
        void set_green(uint8_t value);
        void set_blue(uint8_t value);
        void set_alpha(uint8_t value);

        void set_red_component(float value);
        void set_green_component(float value);
        void set_blue_component(float value);
        void set_alpha_component(float value);

        void set_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
        void set_hsl(float hue, float saturation, float lightness);
        auto with_hsl(float hue, float saturation, float lightness) const -> color;

        auto hue() const -> float;
        auto saturation() const -> float;
        auto lightness() const -> float;
        auto brightness() const -> float;

        void blend_with(const color &overlay_color);
        auto blended_with(const color &overlay_color) const -> color;
        auto with_alpha_component(float value) const -> color;

        void invert();
        auto inverted() -> color;

        auto components() const -> std::array<double, 4>;
        auto info() const -> std::string;

        auto to_hex_string() const -> std::string;

#if __APPLE__
        auto to_cg_color() const -> CGColor *;
        static auto from_cg_color(CGColor *cg_color) -> color;
#endif

    private:
        uint32_t data = 0;

        auto chroma_range() const -> float;

        static auto parse_hex_string(std::string color_string) -> uint32_t;

        friend auto operator<<(std::ostream &os, const color &color) -> std::ostream &;
        friend auto operator==(const stg::color &lhs, const stg::color &rhs) -> bool;
        friend auto operator!=(const stg::color &lhs, const stg::color &rhs) -> bool;
    };
}

#endif //STRATEGR_COLOR_H
