//
// Created by Dmitry Khrykin on 2020-02-18.
//

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <regex>
#include <sstream>

#include "color.h"

namespace stg {

#pragma mark - Conversion Constructors & Operators

    color::color(const char *str) : data(parse_hex_string(str)) {}

    color::color(const std::string &str) : data(parse_hex_string(str)) {}

    auto color::operator=(const std::string &str) -> color & {
        data = parse_hex_string(str);

        return *this;
    }

    color::operator std::string() const {
        return to_hex_string();
    }

#pragma mark - Managing RGB Components

    auto color::red() const -> uint8_t {
        return static_cast<uint8_t>(data >> 24u) & 255u;
    }

    auto color::green() const -> uint8_t {
        return static_cast<uint8_t>(data >> 16u) & 255u;
    }

    auto color::blue() const -> uint8_t {
        return static_cast<uint8_t>(data >> 8u) & 255u;
    }

    auto color::alpha() const -> uint8_t {
        return data & 255u;
    }

    auto color::red_component() const -> float {
        return static_cast<float>(red()) / 255u;
    }

    auto color::green_component() const -> float {
        return static_cast<float>(green()) / 255u;
    }

    auto color::blue_component() const -> float {
        return static_cast<float>(blue()) / 255u;
    }

    auto color::alpha_component() const -> float {
        return static_cast<float>(alpha()) / 255u;
    }

    void color::set_red(uint8_t value) {
        data = (data & 0x00'ff'ff'ffu) | static_cast<uint32_t>(value << 24u);
    }

    void color::set_green(uint8_t value) {
        data = (data & 0xff'00'ff'ffu) | static_cast<uint32_t>(value << 16u);
    }

    void color::set_blue(uint8_t value) {
        data = (data & 0xff'ff'00'ffu) | static_cast<uint32_t>(value << 8u);
    }

    void color::set_alpha(uint8_t value) {
        data = (data & 0xff'ff'ff'00u) | static_cast<uint32_t>(value);
    }

    void color::set_red_component(float value) {
        set_red(std::round(value * 255u));
    }

    void color::set_green_component(float value) {
        set_green(std::round(value * 255u));
    }

    void color::set_blue_component(float value) {
        set_blue(std::round(value * 255u));
    }

    void color::set_alpha_component(float value) {
        set_alpha(std::round(value * 255u));
    }

    void color::set_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        data = 0u;
        data = data | static_cast<uint32_t>(red << 24u);
        data = data | static_cast<uint32_t>(green << 16u);
        data = data | static_cast<uint32_t>(blue << 8u);
        data = data | static_cast<uint32_t>(alpha);
    }

    auto color::rgb_components() const -> std::array<double, 4> {
        return {red_component(),
                green_component(),
                blue_component(),
                alpha_component()};
    }

#pragma mark - Managing HSL Components

    void color::set_hsl(float hue, float saturation, float lightness) {
        hue = hue * 360 / 60;

        auto chroma = (1 - std::abs(2 * lightness - 1)) * saturation;
        auto x = chroma * (1 - std::abs(std::fmodf(hue, 2) - 1));

        auto rgb = std::array<float, 3>();

        if (std::ceilf(hue) == 1) {
            rgb = {chroma, x, 0};
        } else if (std::ceilf(hue) == 2) {
            rgb = {x, chroma, 0};
        } else if (std::ceilf(hue) == 3) {
            rgb = {0, chroma, x};
        } else if (std::ceilf(hue) == 4) {
            rgb = {0, x, chroma};
        } else if (std::ceilf(hue) == 5) {
            rgb = {x, 0, chroma};
        } else if (std::ceilf(hue) == 6) {
            rgb = {chroma, 0, x};
        }

        auto m = lightness - 0.5 * chroma;
        std::transform(rgb.begin(), rgb.end(),
                       rgb.begin(), [&m](auto &c) {
                           return c + m;
                       });

        set_red_component(rgb[0]);
        set_green_component(rgb[1]);
        set_blue_component(rgb[2]);
    }

    auto color::with_hsl(float hue, float saturation, float lightness) const -> color {
        auto clr = *this;
        clr.set_hsl(hue, saturation, lightness);

        return clr;
    }

    auto color::hue() const -> float {
        auto chroma = chroma_range();
        auto value = brightness();

        auto result = 0.0f;
        if (chroma == 0) {
            return result;
        } else if (value == red_component()) {
            auto shift = blue_component() > green_component() ? 6 : 0;
            result = 1.0f / 6 * (shift + (green_component() - blue_component()) / chroma);
        } else if (value == green_component()) {
            result = 1.0f / 6 * (2 + (blue_component() - red_component()) / chroma);
        } else if (value == blue_component()) {
            result = 1.0f / 6 * (4 + (red_component() - green_component()) / chroma);
        }

        return result;
    }

    auto color::saturation() const -> float {
        if (lightness() == 0 || lightness() == 1) {
            return 0;
        } else {
            auto chroma = chroma_range();
            return chroma / (1 - std::abs(2 * brightness() - chroma - 1));
        }
    }

    auto color::lightness() const -> float {
        auto x_max = std::max({red_component(),
                               green_component(),
                               blue_component()});
        auto x_min = std::min({red_component(),
                               green_component(),
                               blue_component()});

        return 0.5f * (x_max + x_min);
    }

    auto color::brightness() const -> float {
        return std::max({red_component(),
                         green_component(),
                         blue_component()});
    }

    auto color::chroma_range() const -> float {
        auto x_max = brightness();
        auto x_min = std::min({red_component(),
                               green_component(),
                               blue_component()});
        return x_max - x_min;
    }

#pragma mark - Color Operations

    void color::blend_with(const color &overlay_color) {
        auto new_red_component = red_component() + (overlay_color.red_component() - red_component()) * overlay_color.alpha_component();

        set_red_component(new_red_component);

        auto new_green_component = green_component() + (overlay_color.green_component() - green_component()) * overlay_color.alpha_component();

        set_green_component(new_green_component);

        auto new_blue_component = blue_component() + (overlay_color.blue_component() - blue_component()) * overlay_color.alpha_component();

        set_blue_component(new_blue_component);
    }

    auto color::blended_with(const color &overlay_color) const -> color {
        auto clr = *this;
        clr.blend_with(overlay_color);

        return clr;
    }

    auto color::with_alpha_component(float value) const -> color {
        auto clr = *this;
        clr.set_alpha_component(value);

        return clr;
    }

    void color::invert() {
        data = ~data;
    }

    auto color::inverted() -> color {
        auto clr = *this;
        clr.invert();

        return clr;
    }

#pragma mark - Explicit Conversions

    auto color::to_hex_string() const -> std::string {
        auto component_string = [](uint8_t value) -> std::string {
            std::stringstream stream;
            stream << std::hex
                   << std::setw(2)
                   << std::right
                   << std::setfill('0')
                   << (unsigned) value;

            return stream.str();
        };

        std::string result = "#";

        result = result + component_string(red()) + component_string(green()) + component_string(blue());

        if (alpha() < 255u)
            result += component_string(alpha());

        return result;
    }

    auto color::parse_hex_string(std::string color_string) -> uint32_t {
        uint32_t result = 0;

        std::transform(color_string.begin(),
                       color_string.end(),
                       color_string.begin(),
                       tolower);

        color_string = std::regex_replace(color_string, std::regex("^#"), "");

        if (color_string.length() == 3)
            color_string = {color_string[0],
                            color_string[0],
                            color_string[1],
                            color_string[1],
                            color_string[2],
                            color_string[2]};

        if (color_string.length() == 4)
            color_string = {color_string[0],
                            color_string[0],
                            color_string[1],
                            color_string[1],
                            color_string[2],
                            color_string[2],
                            color_string[3],
                            color_string[3]};

        while (color_string.length() < 6)
            color_string += "0";

        if (color_string.length() == 6)
            color_string = color_string + "ff";

        std::stringstream stream;
        stream << std::hex << color_string;
        stream >> result;

        return result;
    }

#pragma mark - Debugging

    auto color::info() const -> std::string {
        std::stringstream s;
        s << "color: " << *this << " "
          << "rgba(" << (int) red()
          << ", " << (int) green()
          << ", " << (int) blue()
          << ", " << (int) alpha() << ") "
          << "hslb(" << hue() * 360
          << "°, " << saturation()
          << ", " << lightness()
          << ", " << brightness() << ")";

        return s.str();
    }

    auto operator<<(std::ostream &os, const color &color) -> std::ostream & {
        os << color.to_hex_string();
        return os;
    }

    auto operator==(const color &lhs, const color &rhs) -> bool {
        return lhs.data == rhs.data;
    }

    auto operator!=(const color &lhs, const color &rhs) -> bool {
        return !(lhs == rhs);
    }
}
