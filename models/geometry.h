//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_GEOMETRY_H
#define STRATEGR_GEOMETRY_H

#include <ostream>
#include <type_traits>
#include <utility>

namespace stg {
    using float_t = float;

    template<typename T, typename = int, typename = int>
    struct is_cg_point_like : std::false_type {
    };

    template<typename T>
    struct is_cg_point_like<T,
            decltype((void) (T::x), 0),
            decltype((void) (T::y), 0)> : std::true_type {
    };

    template<typename T, typename = int, typename = int>
    struct is_cg_rect_like : std::false_type {
    };

    template<typename T>
    struct is_cg_rect_like<T,
            decltype((void) (T::origin), 0),
            decltype((void) (T::size), 0)> : std::true_type {
    };

    template<typename T, typename = int, typename = int>
    struct is_qpoint_like : std::false_type {
    };

    template<typename T>
    struct is_qpoint_like<T,
            decltype(std::declval<T>().x()),
            decltype(std::declval<T>().y())> : std::true_type {
    };

    template<typename T, typename = int, typename = int>
    struct is_qrect_like : std::false_type {
    };

    template<typename T>
    struct is_qrect_like<T,
            decltype(std::declval<T>().x()),
            decltype(std::declval<T>().width())> : std::true_type {
    };

    struct point {
        static const point zero;

        float_t x = 0;
        float_t y = 0;

        constexpr explicit point(float_t x = 0, float_t y = 0) : x(x), y(y) {}

        template<typename T, std::enable_if_t<is_qpoint_like<T>::value, int> = 0>
        point(const T &point) {
            x = (float_t) point.x();
            y = (float_t) point.y();
        }

        template<typename T, std::enable_if_t<is_cg_point_like<T>::value, int> = 0>
        point(const T &point) {
            x = (float_t) point.x;
            y = (float_t) point.y;
        }

        template<typename T, std::enable_if_t<is_qpoint_like<T>::value, int> = 0>
        operator T() const {
            return T(x, y);
        }

        template<typename T, std::enable_if_t<is_cg_point_like<T>::value, int> = 0>
        operator T() const {
            return {(double) x,
                    (double) y};
        }

        auto operator+(const point &other) const -> point {
            return point{x + other.x, y + other.y};
        }

        auto operator-(const point &other) const -> point {
            return point{x - other.x, y - other.y};
        }

        friend std::ostream &operator<<(std::ostream &os, const point &point) {
            os << "point [ " << point.x << " " << point.y << " ]";
            return os;
        }
    };

    inline constexpr point point::zero = point();

    struct rect {
        static const rect zero;

        stg::float_t left = 0;
        stg::float_t top = 0;
        stg::float_t width = 0;
        stg::float_t height = 0;

        constexpr rect(std::initializer_list<stg::float_t> l = {}) noexcept {
            if (l.size() == 4) {
                const auto *it = l.begin();
                left = *it++;
                top = *it++;
                width = *it++;
                height = *it;
            }
        }

        constexpr rect(std::initializer_list<int> l = {}) noexcept {
            if (l.size() == 4) {
                const auto *it = l.begin();
                left = *it++;
                top = *it++;
                width = *it++;
                height = *it;
            }
        }

        template<typename T, std::enable_if_t<is_qrect_like<T>::value, int> = 0>
        rect(const T &rect) {
            left = (float_t) rect.x();
            top = (float_t) rect.y();
            width = (float_t) rect.width();
            height = (float_t) rect.height();
        }

        template<typename T, std::enable_if_t<is_cg_rect_like<T>::value, int> = 0>
        rect(const T &rect) {
            left = (float_t) rect.origin.x;
            top = (float_t) rect.origin.y;
            width = (float_t) rect.size.width;
            height = (float_t) rect.size.height;
        }

        template<typename T, std::enable_if_t<is_qrect_like<T>::value, int> = 0>
        operator T() const {
            return T(left, top, width, height);
        }

        template<typename T, std::enable_if_t<is_cg_rect_like<T>::value, int> = 0>
        operator T() const {
            return {(float_t) left,
                    (float_t) top, {
                            (float_t) width,
                            (float_t) height
                    }};
        }

        auto origin() const -> point {
            return point{left, top};
        }

        explicit operator bool() const {
            return *this != rect::zero;
        }

        friend auto operator==(const rect &lhs, const rect &rhs) -> bool {
            return lhs.left == rhs.left &&
                   lhs.top == rhs.top &&
                   lhs.width == rhs.width &&
                   lhs.height == rhs.height;
        }

        friend auto operator!=(const rect &lhs, const rect &rhs) -> bool {
            return !(lhs == rhs);
        }

        friend auto operator<<(std::ostream &os, const rect &r) -> std::ostream & {
            os << "rect [ "
               << r.left << " "
               << r.top << " "
               << r.width << " "
               << r.height << " ]";
            return os;
        }
    };

    inline constexpr rect rect::zero = rect{0, 0, 0, 0};
}

#endif //STRATEGR_GEOMETRY_H
