//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_GEOMETRY_H
#define STRATEGR_GEOMETRY_H

#include <ostream>

namespace stg {
    struct rect {
        int left = 0;
        int top = 0;
        int width = 0;
        int height = 0;

        rect(std::initializer_list<int> l = {}) {
            if (l.size() == 4) {
                auto it = l.begin();
                left = *it++;
                top = *it++;
                width = *it++;
                height = *it++;
            }
        }

        /**
         * Implicit constructor for Qt-like rect
         */

        template<typename T>
        rect(const T &rect) {
            left = rect.x();
            top = rect.y();
            width = rect.width();
            height = rect.height();
        }

        template<typename T>
        operator T() const {
            return T(left, top, width, height);
        }

        friend std::ostream &operator<<(std::ostream &os, const rect &r) {
            os << "rect [ "
               << r.left << " "
               << r.top << " "
               << r.width << " "
               << r.height << " ]";
            return os;
        }
    };

    struct point {
        int x = 0;
        int y = 0;

        explicit point(int x = 0, int y = 0) : x(x), y(y) {}

        /**
        * Implicit constructor for Qt-like point
        */

        template<typename T>
        point(const T &point) {
            x = point.x();
            y = point.y();
        }

        template<typename T>
        operator T() const {
            return T(x, y);
        }

        friend std::ostream &operator<<(std::ostream &os, const point &point) {
            os << "point [ " << point.x << " " << point.y << " ]";
            return os;
        }
    };
}

#endif //STRATEGR_GEOMETRY_H
