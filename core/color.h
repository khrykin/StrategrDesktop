//
// Created by Dmitry Khrykin on 2020-02-18.
//

#ifndef STRATEGR_COLOR_H
#define STRATEGR_COLOR_H

#include <algorithm>
#include <array>
#include <ostream>
#include <string>

#if __APPLE__

#import <CoreGraphics/CGColor.h>

#endif

namespace stg {
    struct color {

#pragma mark - SFINAE helpers

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

#pragma mark - Default Colors

        static const color clear_color;
        static const color black_color;
        static const color white_color;

#pragma mark - Constructors

        explicit constexpr color(uint32_t data = 0u);
        constexpr color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);

#pragma mark - Conversion Constructors & Operators

        color(const char *str);
        color(const std::string &str);

        template<typename T, std::enable_if_t<is_qcolor_like<T>::value, int> = 0>
        constexpr color(const T &q_color_like) {
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

#if __APPLE__
        color(CGColorRef cg_color);
        operator CGColorRef() const;
#endif

#pragma mark - Managing RGB Components

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

        void set_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255u);
        auto rgb_components() const -> std::array<double, 4>;

#pragma mark - Managing HSL Components

        void set_hsl(float hue, float saturation, float lightness);
        auto with_hsl(float hue, float saturation, float lightness) const -> color;

        auto hue() const -> float;
        auto saturation() const -> float;
        auto lightness() const -> float;
        auto brightness() const -> float;

#pragma mark - Color Operations

        void blend_with(const color &overlay_color);
        auto blended_with(const color &overlay_color) const -> color;
        auto with_alpha_component(float value) const -> color;

        void invert();
        auto inverted() -> color;

#pragma mark - Explicit Conversions

        auto to_hex_string() const -> std::string;

#pragma mark - Debugging

        auto info() const -> std::string;

    private:
        uint32_t data = 0;

        auto chroma_range() const -> float;

        static auto parse_hex_string(std::string color_string) -> uint32_t;

        friend auto operator<<(std::ostream &os, const color &color) -> std::ostream &;
        friend auto operator==(const stg::color &lhs, const stg::color &rhs) -> bool;
        friend auto operator!=(const stg::color &lhs, const stg::color &rhs) -> bool;
    };

    constexpr color::color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        set_rgba(red, green, blue, alpha);
    }

    constexpr color::color(uint32_t data) : data(data) {}

    inline constexpr const color color::clear_color = color(0x00'00'00'00u);
    inline constexpr const color color::black_color = color(0x00'00'00'ffu);
    inline constexpr const color color::white_color = color(0xff'ff'ff'ffu);
}

#endif//STRATEGR_COLOR_H
