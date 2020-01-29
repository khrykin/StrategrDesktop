//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_RECT_H
#define STRATEGR_RECT_H

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
    };

    template<typename T>
    inline T rect_cast(const rect &r) {
        return T(r.left, r.top, r.width, r.height);
    };

    // Qt-like cast
    template<typename T>
    inline rect rect_cast(const T &r) {
        return rect(r.left(), r.y(), r.width(), r.height());
    };
}

#endif //STRATEGR_RECT_H
