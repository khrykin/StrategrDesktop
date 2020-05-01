//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_GEOMETRY_H
#define STRATEGR_GEOMETRY_H

#include <ostream>
#include <type_traits>
#include <utility>

namespace stg {
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

        int x = 0;
        int y = 0;

        constexpr explicit point(int x = 0, int y = 0) : x(x), y(y) {}

        template<typename T, std::enable_if_t<is_qpoint_like<T>::value, int> = 0>
        point(const T &point) {
            x = point.x();
            y = point.y();
        }

        template<typename T, std::enable_if_t<is_cg_point_like<T>::value, int> = 0>
        point(const T &point) {
            x = static_cast<int>(point.x);
            y = static_cast<int>(point.y);
        }

        template<typename T, std::enable_if_t<is_qpoint_like<T>::value, int> = 0>
        operator T() const {
            return T(x, y);
        }

        template<typename T, std::enable_if_t<is_cg_point_like<T>::value, int> = 0>
        operator T() const {
            return {static_cast<double>(x),
                    static_cast<double>(y)};
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

        int left = 0;
        int top = 0;
        int width = 0;
        int height = 0;

        constexpr rect(std::initializer_list<int> l = {}) noexcept {
            if (l.size() == 4) {
                const auto *it = l.begin();
                left = *it++;
                top = *it++;
                width = *it++;
                height = *it++;
            }
        }

        template<typename T, std::enable_if_t<is_qrect_like<T>::value, int> = 0>
        rect(const T &rect) {
            left = rect.x();
            top = rect.y();
            width = rect.width();
            height = rect.height();
        }

        template<typename T, std::enable_if_t<is_cg_rect_like<T>::value, int> = 0>
        rect(const T &rect) {
            left = static_cast<int>(rect.origin.x);
            top = static_cast<int>(rect.origin.y);
            width = static_cast<int>(rect.size.width);
            height = static_cast<int>(rect.size.height);
        }

        template<typename T, std::enable_if_t<is_qrect_like<T>::value, int> = 0>
        operator T() const {
            return T(left, top, width, height);
        }

        template<typename T, std::enable_if_t<is_cg_rect_like<T>::value, int> = 0>
        operator T() const {
            return {static_cast<float>(left),
                    static_cast<float>(top), {
                            static_cast<float>(width),
                            static_cast<float>(height)
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
