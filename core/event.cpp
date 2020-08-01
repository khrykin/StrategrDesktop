//
// Created by Dmitry Khrykin on 2020-01-30.
//

#include "event.h"

namespace stg {
    event::event(key_modifiers modifiers)
            : modifiers(modifiers) {}

    mouse_event::mouse_event(const point &position, key_modifiers modifiers)
            : event(modifiers),
              position(position) {}

    auto event::has_only(key_modifiers mod) const -> bool {
        return modifiers == mod;
    }

    auto event::has(key_modifiers mod) const -> bool {
        return (modifiers & mod) == mod;
    }

    auto operator<<(std::ostream &os, const mouse_event &e) -> std::ostream & {
        os << "mouse_event {\n";
        os << "  position: [" << e.position.x << ", " << e.position.y << "]\n";
        os << "  modifiers: [";

        if (e.has(mouse_event::left_key))
            os << " left_key ";

        if (e.has(mouse_event::right_key))
            os << " right_key ";

        if (e.has(mouse_event::ctrl_key))
            os << " ctrl_key ";

        os << "]\n";
        os << "}\n";

        return os;
    }
}