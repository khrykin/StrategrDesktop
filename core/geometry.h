//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_GEOMETRY_H
#define STRATEGR_GEOMETRY_H

#include <ostream>
#include <type_traits>
#include <utility>

namespace stg {
    using gfloat = float;

#pragma mark - Type Traits

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

#pragma mark - Point

    struct point {
        static const point zero;

        gfloat x = 0;
        gfloat y = 0;

        constexpr explicit point(gfloat x = 0, gfloat y = 0) : x{x}, y{y} {}

        template<typename T, std::enable_if_t<is_qpoint_like<T>::value, int> = 0>
        point(const T &point) {
            x = (gfloat) point.x();
            y = (gfloat) point.y();
        }

        template<typename T, std::enable_if_t<is_cg_point_like<T>::value, int> = 0>
        point(const T &point) {
            x = (gfloat) point.x;
            y = (gfloat) point.y;
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

#pragma mark - Rectangle

    struct rect {
        static const rect zero;

        gfloat left = 0;
        gfloat top = 0;
        gfloat width = 0;
        gfloat height = 0;

        constexpr explicit rect(gfloat left = 0,
                                gfloat top = 0,
                                gfloat width = 0,
                                gfloat height = 0)
            : left{left},
              top{top},
              width{width},
              height{height} {}

        template<typename T, std::enable_if_t<is_qrect_like<T>::value, int> = 0>
        rect(const T &rect) {
            left = (gfloat) rect.x();
            top = (gfloat) rect.y();
            width = (gfloat) rect.width();
            height = (gfloat) rect.height();
        }

        template<typename T, std::enable_if_t<is_cg_rect_like<T>::value, int> = 0>
        rect(const T &rect) {
            left = (gfloat) rect.origin.x;
            top = (gfloat) rect.origin.y;
            width = (gfloat) rect.size.width;
            height = (gfloat) rect.size.height;
        }

        template<typename T, std::enable_if_t<is_qrect_like<T>::value, int> = 0>
        operator T() const {
            return T(left, top, width, height);
        }

        template<typename T, std::enable_if_t<is_cg_rect_like<T>::value, int> = 0>
        operator T() const {
            return {(gfloat) left,
                    (gfloat) top,
                    {(gfloat) width,
                     (gfloat) height}};
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

#endif//STRATEGR_GEOMETRY_H
