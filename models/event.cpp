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

bool stg::event::has_only(stg::event::key_modifiers mod) const {
    return modifiers == mod;
}

bool stg::event::with(stg::event::key_modifiers mod) const {
    return (modifiers & mod) == mod;
}

std::ostream &stg::operator<<(std::ostream &os, const stg::mouse_event &e) {
    os << "mouse_event {\n";
    os << "  position: [" << e.position.x << ", " << e.position.y << "]\n";
    os << "  modifiers: [";
    if (e.with(mouse_event::left_key)) {
        os << " left_key ";
    }
    if (e.with(mouse_event::right_key)) {
        os << " right_key ";
    }
    if (e.with(mouse_event::ctrl_key)) {
        os << " ctrl_key ";
    }
    os << "]\n";
    os << "}\n";

    return os;
}