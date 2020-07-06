//
// Created by Dmitry Khrykin on 2020-01-30.
//

#include "event.h"

stg::event::event(stg::event::key_modifiers modifiers)
        : modifiers(modifiers) {
}

stg::mouse_event::mouse_event(const stg::point &position,
                              stg::event::key_modifiers modifiers)
        : event(modifiers),
          position(position) {
}

auto stg::event::has_only(stg::event::key_modifiers mod) const -> bool {
    return modifiers == mod;
}

auto stg::event::has(stg::event::key_modifiers mod) const -> bool {
    return (modifiers & mod) == mod;
}

auto stg::operator<<(std::ostream &os, const stg::mouse_event &e) -> std::ostream & {
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