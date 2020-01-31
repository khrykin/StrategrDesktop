//
// Created by Dmitry Khrykin on 2020-01-29.
//

#include "mouse_handler.h"
#include "mousehandleroperations.h"

#define stg_make_operation(operation) (std::make_unique<operation>(this))

stg::mouse_handler::mouse_handler(stg::strategy &strategy,
                                  stg::selection &selection,
                                  std::function<int()> slot_height_getter,
                                  std::function<rect()> bounds_getter,
                                  const mouse_parameters &settings)
        : strategy(strategy),
          selection(selection),
          get_slot_height(std::move(slot_height_getter)),
          get_bounds(std::move(bounds_getter)),
          settings(settings),
          current_operaion(stg_make_operation(none_operation)) {
    assert(slot_height_getter != nullptr && "slot_height_getter must be provided");
    assert(bounds_getter != nullptr && "bounds_getter must be provided");
}

stg::mouse_handler::~mouse_handler() = default;

void stg::mouse_handler::mouse_press(const stg::mouse_event &event) {
//    std::cout << "mouse_press: " << event << "\n";
//    std::cout << "bounds: " << get_bounds() << "\n";
//    std::cout << "slot_index: " << get_slot_index(event) << "\n";

    current_slot_index = get_slot_index(event);
    current_session_index = get_session_index(current_slot_index);
    current_mouse_zone = get_mouse_zone(current_session_index, event.position);

    current_operaion = get_operation(event.modifiers);
    current_operaion->init(event);

    std::cout << "current_operaion: " << current_operaion->type() << "\n";

    update_cursor(event.modifiers);
}

void stg::mouse_handler::mouse_move(const stg::mouse_event &event) {
    current_slot_index = get_slot_index(event);
    current_session_index = get_session_index(current_slot_index);
    current_mouse_zone = get_mouse_zone(current_session_index, event.position);

    current_operaion->perform(event);

    update_cursor(event.modifiers);
}

void stg::mouse_handler::mouse_release(const stg::mouse_event &event) {
    std::cout << "mouse_release" << "\n";

    current_operaion->teardown(event);
    current_operaion = stg_make_operation(none_operation);

    update_cursor(event.modifiers);

    // Reset these after we update cursor
    current_mouse_zone = mouse_zone::out_of_bounds;
    current_slot_index = -1;
    current_session_index = -1;
}

void stg::mouse_handler::key_down(const event &event) {
//    std::cout << "key_down" << "\n";
    update_cursor(event.modifiers);
}

void stg::mouse_handler::key_up(const event &event) {
//    std::cout << "key_up" << "\n";
    update_cursor(event.modifiers);
}

std::unique_ptr<stg::mouse_handler::operation>
stg::mouse_handler::get_operation(event::key_modifiers modifiers) {
    auto zone = current_mouse_zone;
    auto &time_slots = strategy.time_slots();
    auto &current_slot = time_slots[current_slot_index];

    auto next_empty = time_slots.next_slot_empty(current_slot_index);
    auto prev_empty = time_slots.previous_slot_empty(current_slot_index);

    auto current_selected = selection.has_selected(current_slot_index);

    if (modifiers == mouse_event::left_key) {
        if (!selection.empty() && current_selected) {
            return stg_make_operation(select_operation);
        }

        if (zone == mouse_zone::drag) {
            if (current_slot.empty())
                return stg_make_operation(select_operation);
            else
                return stg_make_operation(drag_operation);
        } else if (zone == mouse_zone::stretch_bottom) {
            if (current_slot.empty() && !next_empty)
                return stg_make_operation(resize_operation);
            else if (current_slot.empty())
                return stg_make_operation(select_operation);
            else
                return stg_make_operation(resize_operation);
        } else if (zone == mouse_zone::stretch_top) {
            if (current_slot.empty() && !prev_empty)
                return stg_make_operation(resize_operation);
            else if (current_slot.empty())
                return stg_make_operation(select_operation);
            else
                return stg_make_operation(resize_operation);
        }
    } else if (modifiers == mouse_event::right_key) {

    } else if (modifiers == (mouse_event::left_key | mouse_event::ctrl_key)) {
        return stg_make_operation(select_operation);
    }

    return stg_make_operation(none_operation);
}

stg::mouse_handler::index_t
stg::mouse_handler::get_session_index(index_t slot_index) {
    return strategy.sessions().session_index_for_time_slot_index(slot_index);
}

stg::mouse_handler::range stg::mouse_handler::get_session_range(index_t session_index) {
    const auto &session = strategy.sessions()[session_index];
    auto top = static_cast<int>(strategy.sessions().relative_time(session) * px_in_time());
    auto height = static_cast<int>(session.duration() * px_in_time());

//    std::cout << "slot_index:  " << slot_index << "\n";
//    std::cout << "session_index:  " << session_index << "\n";

    return range{.top = top, .bottom = top + height};
}

stg::mouse_handler::mouse_zone stg::mouse_handler::get_mouse_zone(int session_index,
                                                                  const point &mouse_pos) {
    auto session_range = get_session_range(session_index);

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

    if (!session_range.contains(mouse_pos.y)) {
        return mouse_zone::out_of_bounds;
    }

    if (top_stretch_zone.contains(mouse_pos.y)) {
        return mouse_zone::stretch_top;
    } else if (bottom_stretch_zone.contains(mouse_pos.y)) {
        return mouse_zone::stretch_bottom;
    } else {
        return mouse_zone::drag;
    }
}

float stg::mouse_handler::px_in_time() {
    return get_slot_height() / static_cast<float>(strategy.time_slot_duration());
}

stg::mouse_handler::index_t stg::mouse_handler::get_slot_index(const stg::mouse_event &event) {
    auto index = event.position.y / get_slot_height();

    if (index < 0)
        index = 0;
    else if (index > strategy.number_of_time_slots() - 1)
        index = strategy.number_of_time_slots() - 1;

    return index;
}

void stg::mouse_handler::update_cursor(event::key_modifiers modifiers) {
    auto new_cursor = get_cursor(modifiers);
    if (new_cursor == current_cursor)
        return;

    current_cursor = new_cursor;

    if (on_cursor_change)
        on_cursor_change(new_cursor);
}

stg::mouse_handler::cursor
stg::mouse_handler::get_cursor(event::key_modifiers modifiers) {
    auto mouse_zone = current_mouse_zone;
    auto &time_slots = strategy.time_slots();
    auto &current_slot = time_slots[current_slot_index];
    auto next_empty = time_slots.next_slot_empty(current_slot_index);
    auto prev_empty = time_slots.previous_slot_empty(current_slot_index);

    if (selection.has_selected(current_slot_index)) {
        return cursor::pointer;
    }

    switch (current_operaion->type()) {
        case none:
            if ((modifiers & mouse_event::ctrl_key) == mouse_event::ctrl_key) {
                return cursor::pointer;
            }

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
        case resize:
            return cursor::resize;
        case select:
            return cursor::pointer;
    }
}

std::ostream &stg::operator<<(std::ostream &os, stg::mouse_handler::mouse_zone zone) {
    switch (zone) {
        case mouse_handler::mouse_zone::stretch_top:
            os << "stretch_top";
            break;
        case mouse_handler::mouse_zone::stretch_bottom:
            os << "stretch_bottom";
            break;
        case mouse_handler::mouse_zone::drag :
            os << "drag";
            break;
        case mouse_handler::mouse_zone::out_of_bounds :
            os << "out_of_bounds";
            break;
    }

    return os;
}

std::ostream &stg::operator<<(std::ostream &os, const stg::mouse_handler::range &range) {
    os << "range [ " << range.top << " " << range.bottom << " ]";
    return os;
}

bool stg::mouse_handler::range::contains(int coord) {
    return coord >= top && coord <= bottom;
}

std::ostream &stg::operator<<(std::ostream &os, mouse_handler::operation_type op) {
    switch (op) {
        case mouse_handler::none:
            os << "none";
            break;
        case mouse_handler::drag:
            os << "drag";
            break;
        case mouse_handler::resize:
            os << "resize";
            break;
        case mouse_handler::select:
            os << "select";
            break;
    }

    return os;
}

