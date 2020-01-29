//
// Created by Dmitry Khrykin on 2020-01-29.
//

#include "mousehandler.h"
#include "mousehandleroperations.h"

#define make_operation(operation) (std::make_unique<operation>(this))

stg::mousehandler::mousehandler(stg::strategy &strategy,
                                std::function<int()> slot_height_getter,
                                std::function<rect()> bounds_getter,
                                const mousehandler_parameters &settings)
        : strategy(strategy),
          get_slot_height(std::move(slot_height_getter)),
          get_bounds(std::move(bounds_getter)),
          settings(settings),
          current_operaion(make_operation(none_operation)) {
    assert(slot_height_getter != nullptr && "slot_height_getter must be provided");
    assert(bounds_getter != nullptr && "bounds_getter must be provided");
}

stg::mousehandler::~mousehandler() = default;

void stg::mousehandler::mouse_press(const stg::mouse_event &event) {
//    std::cout << "mouse_press: " << event << "\n";
//    std::cout << "bounds: " << get_bounds() << "\n";
//    std::cout << "slot_index: " << get_slot_index(event) << "\n";

//    current_mouse_zone = get_mouse_zone(event);
    current_operaion = get_operation(event);
    current_operaion->init(event);

    std::cout << "current_operaion: " << current_operaion->type() << "\n";

    update_cursor(event);
}

void stg::mousehandler::mouse_move(const stg::mouse_event &event) {
    current_operaion->perform(event);

    update_cursor(event);
}

void stg::mousehandler::mouse_release(const stg::mouse_event &event) {
    current_operaion->teardown(event);
    current_operaion = make_operation(none_operation);

    update_cursor(event);
}

std::unique_ptr<stg::mousehandler::operation>
stg::mousehandler::get_operation(const mouse_event &event) {
    auto zone = get_mouse_zone(event);
    auto current_slot_index = get_slot_index(event);
    auto &time_slots = strategy.time_slots();
    auto &current_slot = time_slots[current_slot_index];
    auto next_empty = time_slots.next_slot_empty(current_slot_index);
    auto prev_empty = time_slots.previous_slot_empty(current_slot_index);

    if (event.is(mouse_event::left_key)) {
        if (zone == mouse_zone::drag) {
            if (current_slot.empty())
                return make_operation(select_operation);
            else
                return make_operation(drag_operation);
        } else if (zone == mouse_zone::stretch_bottom) {
            if (current_slot.empty() && !next_empty)
                return make_operation(stretch_bottom_operation);
            else if (current_slot.empty())
                return make_operation(select_operation);
            else
                return make_operation(stretch_bottom_operation);
        } else if (zone == mouse_zone::stretch_top) {
            if (current_slot.empty() && !prev_empty)
                return make_operation(stretch_top_operation);
            else if (current_slot.empty())
                return make_operation(select_operation);
            else
                return make_operation(stretch_top_operation);
        }
    } else if (event.is(mouse_event::right_key)) {

    } else if (event.is(mouse_event::left_key | mouse_event::ctrl_key)) {
        return make_operation(select_operation);
    }

    return make_operation(none_operation);
}

stg::mousehandler::index_t
stg::mousehandler::get_session_index(stg::mousehandler::index_t slot_index) {
    return strategy.sessions().session_index_for_time_slot_index(slot_index);
}

stg::mousehandler::range stg::mousehandler::get_session_range(const stg::mouse_event &event) {
    auto slot_index = get_slot_index(event);
    auto session_index = get_session_index(slot_index);
    const auto &session = strategy.sessions()[session_index];
    auto top = static_cast<int>(strategy.sessions().relative_time(session) * px_in_time());
    auto height = static_cast<int>(session.duration() * px_in_time());

//    std::cout << "slot_index:  " << slot_index << "\n";
//    std::cout << "session_index:  " << session_index << "\n";

    return range{.top = top, .bottom = top + height};
}

stg::mousehandler::mouse_zone stg::mousehandler::get_mouse_zone(stg::mouse_event event) {
    auto session_range = get_session_range(event);

    auto top_stretch_zone = range{
            .top = session_range.top,
            .bottom = session_range.top + settings.stretch_zone_height
    };

    auto bottom_stretch_zone = range{
            .top = session_range.bottom - settings.stretch_zone_height,
            .bottom = session_range.bottom
    };

//    std::cout << "pos:  " << event.position << "\n";
//    std::cout << "session_range:  " << session_range << "\n";
//    std::cout << "top_stretch_zone:  " << top_stretch_zone << "\n";
//    std::cout << "bottom_stretch_zone:  " << bottom_stretch_zone << "\n";

    if (!session_range.contains(event.position.y)) {
        return mouse_zone::out_of_bounds;
    }

    if (top_stretch_zone.contains(event.position.y)) {
        return mouse_zone::stretch_top;
    } else if (bottom_stretch_zone.contains(event.position.y)) {
        return mouse_zone::stretch_bottom;
    } else {
        return mouse_zone::drag;
    }
}

float stg::mousehandler::px_in_time() {
    return get_slot_height() / static_cast<float>(strategy.time_slot_duration());
}

stg::mousehandler::index_t stg::mousehandler::get_slot_index(const stg::mouse_event &event) {
    return event.position.y / get_slot_height();
}

void stg::mousehandler::update_cursor(const mouse_event &event) {
    auto new_cursor = get_cursor(event);
    if (new_cursor == current_cursor)
        return;

    current_cursor = new_cursor;

    if (on_cursor_change)
        on_cursor_change(new_cursor);
}

void stg::mousehandler::set_selected(bool selected) {
    if (on_set_selected)
        on_set_selected(selected);
}

stg::mousehandler::cursor stg::mousehandler::get_cursor(const mouse_event &event) {
    auto mouse_zone = get_mouse_zone(event);
    auto current_slot_index = get_slot_index(event);
    auto &time_slots = strategy.time_slots();
    auto &current_slot = time_slots[current_slot_index];
    auto next_empty = time_slots.next_slot_empty(current_slot_index);
    auto prev_empty = time_slots.previous_slot_empty(current_slot_index);

    switch (current_operaion->type()) {
        case none:
            switch (mouse_zone) {
                case mouse_zone::out_of_bounds:
                    return cursor::pointer;
                case mouse_zone::drag:
                    return current_slot.empty()
                           ? cursor::pointer
                           : cursor::open_hand;
                case mouse_zone::stretch_top:
                    return !current_slot.empty() ||
                           (current_slot.empty() && !prev_empty)
                           ? cursor::resize
                           : cursor::pointer;
                case mouse_zone::stretch_bottom:
                    return !current_slot.empty() ||
                           (current_slot.empty() && !next_empty)
                           ? cursor::resize
                           : cursor::pointer;

            }
        case drag:
            return cursor::closed_hand;
        case stretch_top:
        case stretch_bottom:
            return cursor::resize;
        case select:
            return cursor::pointer;
    }
}

std::ostream &stg::operator<<(std::ostream &os, stg::mousehandler::mouse_zone zone) {
    switch (zone) {
        case mousehandler::mouse_zone::stretch_top:
            os << "stretch_top";
            break;
        case mousehandler::mouse_zone::stretch_bottom:
            os << "stretch_bottom";
            break;
        case mousehandler::mouse_zone::drag :
            os << "drag";
            break;
        case mousehandler::mouse_zone::out_of_bounds :
            os << "out_of_bounds";
            break;
    }

    return os;
}

std::ostream &stg::operator<<(std::ostream &os, const stg::mousehandler::range &range) {
    os << "range [ " << range.top << " " << range.bottom << " ]";
    return os;
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

stg::mouse_event::mouse_event(
        const stg::point &position, stg::mouse_event::key_modifiers
modifiers)
        : position(position), modifiers(modifiers) {
}

bool stg::mouse_event::is(stg::mouse_event::key_modifiers mod) const {
    return modifiers == mod;
}

bool stg::mouse_event::with(stg::mouse_event::key_modifiers mod) const {
    return (modifiers & mod) == mod;
}
