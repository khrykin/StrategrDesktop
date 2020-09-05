//
// Created by Dmitry Khrykin on 2020-01-29.
//

#include "mousehandler.h"
#include "mousehandleroperations.h"
#include "actioncenter.h"

namespace stg {
    mouse_handler::mouse_handler(stg::strategy &strategy,
                                 stg::selection &selection,
                                 std::function<gfloat()> slot_height_getter,
                                 std::function<rect()> viewport_getter,
                                 const mouse_parameters &settings)
            : strategy(strategy),
              selection(selection),
              get_slot_height(std::move(slot_height_getter)),
              get_viewport(std::move(viewport_getter)),
              settings(settings),
              current_operaion(make_operation<none_operation>()) {
        assert(get_slot_height != nullptr && "slot_height_getter must be provided");
    }

    mouse_handler::~mouse_handler() = default;

    void mouse_handler::mouse_press(const mouse_event &event) {
        current_slot_index = get_slot_index(event);
        current_session_index = get_session_index(current_slot_index);
        current_mouse_zone = get_mouse_zone(current_session_index, event.position);
        current_key_modifiers = event.modifiers;

        current_operaion = get_operation(current_key_modifiers);
        current_operaion->start(event);

        if (event.has(event::right_key))
            show_context_menu(event);

        previous_position = event.position;

        update_cursor(event.modifiers);
    }

    void mouse_handler::show_context_menu(const mouse_event &event) {
        if (on_show_context_menu)
            on_show_context_menu({event.position,
                                  current_slot_index,
                                  current_session_index,
                                  context_actions()});
    }

    void mouse_handler::mouse_move(const mouse_event &event) {
        current_slot_index = get_slot_index(event);
        current_session_index = get_session_index(current_slot_index);
        current_mouse_zone = get_mouse_zone(current_session_index, event.position);
        current_key_modifiers = event.modifiers;

        if (current_operaion->type() != none) {
            update_direction(event);
            handle_autoscroll(event);
        }

        current_operaion->perform(event);

        update_cursor(event.modifiers);
    }

    void mouse_handler::mouse_release(const mouse_event &event) {
        current_operaion->stop(event);
        current_operaion = make_operation<none_operation>();

        update_cursor(event.modifiers);

        stop_autoscroll();

        // We need to reset these after we update the cursor
        current_mouse_zone = mouse_zone::out_of_bounds;
        current_direction = direction::none;
        previous_position = point::zero;
        current_key_modifiers = 0;

        current_slot_index = -1;
        current_session_index = -1;
    }

    void mouse_handler::key_down(const event &event) {
        current_key_modifiers |= event.modifiers;

        if (current_operaion->type() == drag &&
            current_operaion->state() == operation::state::started) {
            if (event.has(event::alt_key)) {
                auto fake_event = mouse_event(previous_position, event.modifiers);
                current_operaion->stop(fake_event);
                current_operaion = make_operation<copy_drag_operation>();
                current_operaion->start(fake_event);
            }
        }

        update_cursor(event.modifiers);
    }

    void mouse_handler::key_up(const event &event) {
        current_key_modifiers |= event.modifiers;

        if (current_operaion->type() == copy_drag &&
            current_operaion->state() == operation::state::started) {
            if (!event.has(event::alt_key)) {
                auto fake_event = mouse_event(previous_position, event.modifiers);
                current_operaion->stop(fake_event);
                current_operaion = make_operation<drag_operation>();
                current_operaion->start(fake_event);
            }
        }

        update_cursor(event.modifiers);
    }

    void mouse_handler::auto_scroll_frame(const point &new_mouse_position) {
        auto new_event = mouse_event(new_mouse_position, current_key_modifiers);
        mouse_move(new_event);
    }

    void mouse_handler::handle_autoscroll(const mouse_event &event) {
        auto viewport = get_viewport();

        auto autoscroll_zone_size = settings.autoscroll_zone_size;
        auto pos_in_viewport = event.position - viewport.origin();

        auto top_autoscroll_zone = range{0, autoscroll_zone_size};
        auto slotboard_height = (strategy.number_of_time_slots() + 1.0) * get_slot_height();

        auto bottom_autoscroll_zone = range{viewport.height - autoscroll_zone_size,
                                            viewport.height};

        auto needs_autoscroll_top = top_autoscroll_zone.contains(pos_in_viewport.y) &&
                                    viewport.top > 0 &&
                                    event.position.y > 0;

        auto needs_autoscroll_bottom = bottom_autoscroll_zone.contains(pos_in_viewport.y) &&
                                       viewport.top + viewport.height < slotboard_height &&
                                       event.position.y < slotboard_height;

        auto needs_autoscroll = needs_autoscroll_top || needs_autoscroll_bottom;

        if (!autoscroll_timer && needs_autoscroll) {
            auto direction = scroll_direction::down;
            if (needs_autoscroll_top) {
                direction = scroll_direction::up;
            } else if (needs_autoscroll_bottom) {
                direction = scroll_direction::down;
            }

            start_autoscroll(direction);
        } else if (autoscroll_timer && !needs_autoscroll) {
            stop_autoscroll();
        }
    }

    auto mouse_handler::get_operation(event::key_modifiers modifiers) -> std::unique_ptr<operation> {
        auto zone = current_mouse_zone;
        auto &time_slots = strategy.time_slots();
        auto &current_slot = time_slots[current_slot_index];
        auto &current_session = strategy.sessions()[current_session_index];

        auto next_empty = time_slots.next_slot_empty(current_slot_index);
        auto prev_empty = time_slots.previous_slot_empty(current_slot_index);

        auto current_selected = selection.has_selected(current_slot_index);

        if (modifiers == mouse_event::left_key ||
            modifiers == (mouse_event::left_key | mouse_event::shift_key)) {
            if (!selection.empty() && current_selected) {
                return make_operation<select_operation>();
            }

            if (zone == mouse_zone::drag) {
                if (current_slot.empty())
                    return make_operation<select_operation>();
                else
                    return make_operation<drag_operation>();
            } else if (zone == mouse_zone::stretch_bottom) {
                if (current_slot.empty() && next_empty)
                    return make_operation<select_operation>();
                else
                    return make_operation<resize_operation>();
            } else if (zone == mouse_zone::stretch_top) {
                if (current_slot.empty() && prev_empty)
                    return make_operation<select_operation>();
                else
                    return make_operation<resize_operation>();
            }
        } else if (modifiers == mouse_event::right_key) {
            if (zone == mouse_zone::drag) {
                if (selection.empty() && !current_session.empty()) {
                    auto &sessions = strategy.sessions();
                    auto first_slot_index = sessions.get_bounds_for(current_session_index).start_index;
                    auto slot_indexes = std::vector<index_t>(current_session.length());

                    std::iota(slot_indexes.begin(), slot_indexes.end(), first_slot_index);

                    selection.reset_with(slot_indexes);
                }
            }
        } else if (modifiers == (mouse_event::left_key | mouse_event::ctrl_key)) {
            return make_operation<select_operation>();
        } else if (modifiers == (mouse_event::left_key | mouse_event::alt_key) &&
                   !current_slot.empty() &&
                   zone == mouse_zone::drag) {
            return make_operation<copy_drag_operation>();
        }

        return make_operation<none_operation>();
    }

    auto mouse_handler::get_session_index(index_t slot_index) -> index_t {
        return strategy.sessions().session_index_for_time_slot_index(slot_index);
    }

    auto mouse_handler::get_session_range(index_t session_index) -> range {
        if (session_index < 0)
            return range{0, get_slot_height() / 2};

        const auto &session = strategy.sessions()[session_index];
        auto top = strategy.sessions().relative_begin_time(session) * px_in_time() + get_slot_height() / 2;
        auto height = session.duration() * px_in_time();

        return range{top, top + height};
    }

    auto mouse_handler::get_mouse_zone(int session_index, const point &mouse_pos) -> mouse_zone {
        auto session_range = get_session_range(session_index);

        auto top_stretch_zone = range{session_range.top,
                                      session_range.top + settings.stretch_zone_size};

        auto bottom_stretch_zone = range{session_range.bottom - settings.stretch_zone_size,
                                         session_range.bottom};

//    std::cout << "pos:  " << mouse_pos << "\n";
//    std::cout << "slot_height:  " << get_slot_height() << "\n";
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

    auto mouse_handler::get_direction(int delta) const -> direction {
        if (std::abs(delta) < settings.direction_change_resolution)
            return direction::none;

        return delta > 0 ? direction::down : direction::up;
    }

    auto mouse_handler::px_in_time() -> float {
        return get_slot_height() / (float) strategy.time_slot_duration();
    }

    auto mouse_handler::get_slot_index(const mouse_event &event) -> index_t {
        auto index = (event.position.y - get_slot_height() / 2) / get_slot_height();

        if (index < 0)
            index = -1;
        else if (index >= strategy.number_of_time_slots())
            index = strategy.number_of_time_slots();

        return index;
    }

    auto mouse_handler::get_cursor(event::key_modifiers modifiers) -> cursor {
        auto mouse_zone = current_mouse_zone;
        auto &time_slots = strategy.time_slots();

        if (!time_slots.has_index(current_slot_index))
            return cursor::pointer;

        auto &current_slot = time_slots[current_slot_index];
        auto next_empty = time_slots.next_slot_empty(current_slot_index);
        auto prev_empty = time_slots.previous_slot_empty(current_slot_index);


        auto alt_key_is_pressed = modifiers == (mouse_event::alt_key | mouse_event::left_key) ||
                                  modifiers == mouse_event::alt_key;

        if (alt_key_is_pressed &&
            !current_slot.empty() &&
            mouse_zone == mouse_zone::drag) {
            return cursor::drag_copy;
        }

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
            case copy_drag:
                return cursor::drag_copy;
        }
    }

    void mouse_handler::update_cursor(event::key_modifiers modifiers) {
        if (!on_cursor_change)
            return;

        auto new_cursor = get_cursor(modifiers);
        if (new_cursor == current_cursor)
            return;

        current_cursor = new_cursor;

        on_cursor_change(new_cursor);
    }

    void mouse_handler::update_direction(const mouse_event &event) {
        auto delta = event.position.y - this->previous_position.y;
        auto new_direction = this->get_direction(delta);
        auto prev_direction = this->current_direction;

        if (abs(delta) > this->settings.direction_change_resolution)
            this->previous_position = event.position;

        if (new_direction != mouse_handler::direction::none &&
            new_direction != prev_direction) {
            this->current_direction = new_direction;
            this->current_operaion->handle_direction_change();
        }
    }

    void mouse_handler::start_autoscroll(scroll_direction direction) {
        if (!on_auto_scroll_frame)
            return;

        double constexpr frame_seconds_duration = 1.0 / 30;
        gfloat constexpr pixels_in_second = 200;

        auto setup_auto_scroll_frame = [=] {
            auto scroll_offset_increment = direction == mouse_handler::scroll_direction::down
                                           ? pixels_in_second * frame_seconds_duration
                                           : -pixels_in_second * frame_seconds_duration;

            auto mouse_position = on_auto_scroll_frame(scroll_offset_increment);
            auto_scroll_frame(mouse_position);
        };

        autoscroll_timer = timer::schedule(frame_seconds_duration, true, setup_auto_scroll_frame);
    }

    void mouse_handler::stop_autoscroll() {
        assert(autoscroll_timer.use_count() <= 1);

        autoscroll_timer = nullptr;
    }

    auto mouse_handler::resize_boundary() const -> const resize_boundary_configuration & {
        return _resize_boundary;
    }

    auto mouse_handler::context_actions() -> std::vector<const action *> {
        if (!action_center)
            return {};

        auto actions = std::vector<const action *>();

        actions.push_back(&action_center->set_activity);
        actions.push_back(&action::separator_action());
        actions.push_back(&action_center->empty_selection);
        actions.push_back(&action_center->make_room);
        actions.push_back(&action::separator_action());
        actions.push_back(&action_center->select_all);
        actions.push_back(&action_center->deselect);

        return actions;
    }

    auto operator<<(std::ostream &os, mouse_handler::mouse_zone zone) -> std::ostream & {
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

    auto operator<<(std::ostream &os, const mouse_handler::range &range) -> std::ostream & {
        os << "range [ " << range.top << " " << range.bottom << " ]";
        return os;
    }

    auto operator<<(std::ostream &os, mouse_handler::operation_type op) -> std::ostream & {
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
            case mouse_handler::copy_drag:
                os << "copy_drag";
                break;
        }

        return os;
    }

    auto operator<<(std::ostream &os, mouse_handler::direction dir) -> std::ostream & {
        switch (dir) {
            case mouse_handler::direction::none:
                os << "none";
                break;
            case mouse_handler::direction::up:
                os << "up";
                break;
            case mouse_handler::direction::down:
                os << "down";
                break;
        }

        return os;
    }

    auto operator<<(std::ostream &os, mouse_handler::scroll_direction dir) -> std::ostream & {
        switch (dir) {
            case mouse_handler::scroll_direction::down:
                os << "down";
                break;
            case mouse_handler::scroll_direction::up:
                os << "up";
                break;
        }

        return os;
    }

    auto mouse_handler::range::contains(gfloat coord) -> bool {
        return coord >= top && coord <= bottom;
    }
}
