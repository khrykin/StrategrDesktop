//
// Created by Dmitry Khrykin on 2019-11-25.
//

#ifndef STRATEGR_RECT_HPP
#define STRATEGR_RECT_HPP

namespace stg {
    struct Rect {
        int left = 0;
        int top = 0;
        int width = 0;
        int height = 0;

        Rect(std::initializer_list<int> l = {}) {
            if (l.size() == 4) {
                auto it = l.begin();
                left = *it++;
                top = *it++;
                width = *it++;
                height = *it++;
            }
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

        template<typename T>
        Rect(const T &rect) {
            left = rect.x();
            top = rect.y();
            width = rect.width();
            height = rect.height();
        }

#pragma clang diagnostic pop

        template<typename T>
        T to() {
            return T(left, top, width, height);
        }
    };
}

#endif //STRATEGR_RECT_HPP
