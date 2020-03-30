//
// Created by Dmitry Khrykin on 2020-02-18.
//

#ifndef STRATEGR_COLOR_H
#define STRATEGR_COLOR_H

#include <string>
#include <algorithm>
#include <ctype.h>
#include <ostream>

namespace stg {
    struct color {
        color() {
            init("#000000");
        }

        color(const char *str) {
            init(str);
        }

        color(const std::string &str) {
            init(str);
        }

        color &operator=(const std::string &str) {
            init(str);
            return *this;
        }

        operator const char *() const {
            return color_string.c_str();
        }

        operator const std::string &() const {
            return color_string;
        }

    private:
        friend std::ostream &operator<<(std::ostream &os, const color &color) {
            os << color.color_string;
            return os;
        }

        friend bool operator==(const stg::color &lhs, const stg::color &rhs) {
            return lhs.color_string == rhs.color_string;
        }

        friend bool operator!=(const stg::color &lhs, const stg::color &rhs) {
            return !(lhs == rhs);
        }

        std::string color_string;

        void init(const std::string &str) {
            color_string = str;
            std::transform(color_string.begin(),
                           color_string.end(),
                           color_string.begin(),
                           tolower);
        }
    };
}

#endif //STRATEGR_COLOR_H
