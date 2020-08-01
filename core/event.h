//
// Created by Dmitry Khrykin on 2020-01-30.
//

#ifndef STRATEGR_EVENT_H
#define STRATEGR_EVENT_H

#include "geometry.h"

namespace stg {
    struct event {
        using key_modifiers = uint16_t;

        static constexpr key_modifiers left_key = 1u << 0u;
        static constexpr key_modifiers right_key = 1u << 1u;
        static constexpr key_modifiers ctrl_key = 1u << 2u;
        static constexpr key_modifiers alt_key = 1u << 3u;
        static constexpr key_modifiers shift_key = 1u << 4u;

        explicit event(key_modifiers modifiers);

        template<class QtLikeEvent>
        event(QtLikeEvent *evt) {
            if ((evt->modifiers() & 0x04000000) == 0x04000000)
                modifiers |= ctrl_key;

            if ((evt->modifiers() & 0x08000000) == 0x08000000)
                modifiers |= alt_key;

            if ((evt->modifiers() & 0x02000000) == 0x02000000)
                modifiers |= shift_key;
        }

        auto has_only(key_modifiers mod) const -> bool;
        auto has(key_modifiers mod) const -> bool;

        key_modifiers modifiers = 0;
    };

    struct mouse_event : public event {
        point position;

        explicit mouse_event(const point &position, key_modifiers modifiers = left_key);

        template<class QtLikeEvent>
        mouse_event(QtLikeEvent *evt) : event(evt), position(evt->pos()) {
            if (evt->buttons() == 1) {
                modifiers |= left_key;
            } else if (evt->buttons() == 2) {
                modifiers |= right_key;
            }
        }

        friend auto operator<<(std::ostream &os, const mouse_event &e) -> std::ostream &;
    };
};


#endif //STRATEGR_EVENT_H
