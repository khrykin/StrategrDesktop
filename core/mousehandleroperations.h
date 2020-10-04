//
// Created by Dmitry Khrykin on 2020-01-29.
//

#ifndef STRATEGR_MOUSEHANDLEROPERATIONS_H
#define STRATEGR_MOUSEHANDLEROPERATIONS_H

#include <vector>

#include "actioncenter.h"
#include "mousehandler.h"

struct stg::mouse_handler::operation {
    enum class state {
        started,
        performing,
        idle
    };

    explicit operation(mouse_handler *handler) : handler(*handler) {}

    virtual ~operation() = default;

    virtual operation_type type() = 0;

    state state() const {
        return _state;
    }

    void start(const mouse_event &event) {
        init(event);
        _state = state::started;
    }

    void perform(const mouse_event &event) {
        _state = state::performing;
        change(event);
    }

    void stop(const mouse_event &event) {
        teardown(event);
        _state = state::idle;
    }

    virtual void handle_direction_change(){};

protected:
    mouse_handler &handler;
    stg::strategy &strategy = handler.strategy;
    enum state _state = state::idle;

    virtual void init(const mouse_event &event) = 0;
    virtual void change(const mouse_event &event) = 0;
    virtual void teardown(const mouse_event &event) = 0;
};

struct stg::mouse_handler::none_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return none;
    }

private:
    void init(const mouse_event &event) override{};

    void change(const mouse_event &event) override{};

    void teardown(const mouse_event &event) override{};
};

struct stg::mouse_handler::copy_drag_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return copy_drag;
    }

private:
    point initial_position;
    index_t dragged_session_index = -1;
    int global_distance = 0;

    void init(const mouse_event &event) override {
        handler.selection.deselect_all();

        dragged_session_index = handler.current_session_index;
        initial_position = event.position;

        if (handler.on_draw_dragged_session)
            handler.on_draw_dragged_session(dragged_session_index, get_first_slot_index());
    };

    void change(const mouse_event &event) override {
        if (!event.has(mouse_event::alt_key)) {
            dragged_session_index = -1;
            stop(event);
            handler.current_operaion = handler.make_operation<none_operation>();
            return;
        }

        auto get_global_distance = [this](const mouse_event &event) -> int {
            auto new_global_distance = static_cast<int>(
                (event.position.y - initial_position.y - 0.5 * handler.get_slot_height()) / handler.get_slot_height());

            if (event.position.y - initial_position.y > 0.5 * handler.get_slot_height()) {
                new_global_distance++;
            }

            return new_global_distance;
        };

        auto new_global_distance = get_global_distance(event);
        if (new_global_distance == global_distance)
            return;

        global_distance = new_global_distance;

        if (handler.on_draw_dragged_session)
            handler.on_draw_dragged_session(dragged_session_index, get_first_slot_index());
    }

    void teardown(const mouse_event &event) override {
        if (dragged_session_index >= 0) {
            strategy.copy_session(dragged_session_index, get_first_slot_index());
        }

        handler.current_mouse_zone = mouse_zone::out_of_bounds;

        if (handler.on_draw_dragged_session)
            handler.on_draw_dragged_session(-1, -1);
    };

    auto get_first_slot_index() -> index_t {
        auto &current_session = strategy.sessions()[dragged_session_index];

        auto first_slot_index = *strategy.time_slots().index_of(current_session.time_slots.front()) +
                                global_distance;
        auto last_slot_index = first_slot_index + current_session.length() - 1;

        if (first_slot_index < 0) {
            first_slot_index = 0;
        } else if (last_slot_index > strategy.time_slots().size() - 1) {
            first_slot_index = strategy.time_slots().size() - current_session.length();
        }

        return first_slot_index;
    }
};

struct stg::mouse_handler::drag_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return drag;
    }

    void switch_to_copy(const mouse_event &event) const {
        strategy.cancel_dragging();

        if (handler.on_select_sessions)
            handler.on_select_sessions({});

        auto new_current_operation = handler.make_operation<copy_drag_operation>();

        auto fake_initial_event = mouse_event(initial_position, event.modifiers);
        handler.current_slot_index = initial_slot_index;
        handler.current_session_index = initial_session_index;

        new_current_operation->start(fake_initial_event);
        new_current_operation->perform(event);

        handler.current_operaion = std::move(new_current_operation);
    };

private:
    point initial_position;

    index_t initial_session_index = -1;
    index_t dragged_session_index = -1;
    index_t initial_slot_index = -1;

    int global_distance = 0;

    void init(const mouse_event &event) override {
        handler.selection.deselect_all();

        initial_session_index = handler.current_session_index;
        initial_slot_index = handler.current_slot_index;

        dragged_session_index = initial_session_index;

        if (handler.on_select_sessions)
            handler.on_select_sessions({initial_session_index});

        strategy.begin_dragging(handler.current_session_index);

        initial_position = event.position;
    };

    void change(const mouse_event &event) override {
        if (event.has(mouse_event::alt_key)) {
            switch_to_copy(event);
            return;
        }

        auto get_global_distance = [this](const mouse_event &event) -> int {
            auto new_global_distance = static_cast<int>(
                (event.position.y - initial_position.y - 0.5 * handler.get_slot_height()) / handler.get_slot_height());

            if (event.position.y - initial_position.y > 0.5 * handler.get_slot_height()) {
                new_global_distance++;
            }

            return new_global_distance;
        };

        auto new_global_distance = get_global_distance(event);
        if (new_global_distance == global_distance) {
            if (global_distance == 0)
                _state = state::started;

            return;
        }

        auto new_local_distance = new_global_distance - global_distance;
        if (new_local_distance < 0 && dragged_session_index == 0)
            return;

        if (new_local_distance > 0 && dragged_session_index == strategy.sessions().size() - 1)
            return;

        global_distance = new_global_distance;

        dragged_session_index = strategy.drag_session(dragged_session_index, new_local_distance);

        if (handler.on_select_sessions)
            handler.on_select_sessions({dragged_session_index});
    }

    void teardown(const mouse_event &event) override {
        strategy.end_dragging();

        if (handler.on_select_sessions)
            handler.on_select_sessions({});
    };
};

struct stg::mouse_handler::resize_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return resize;
    }

private:
    bool finished = false;

    index_t handle_index = -1;
    index_t boundary_slot_index = -1;

    mouse_zone initial_mouse_zone = mouse_zone::out_of_bounds;

    void init(const mouse_event &event) override {
        handler.selection.deselect_all();

        handle_index = handler.current_slot_index;
        initial_mouse_zone = handler.current_mouse_zone;

        select_sessions(event, handle_index, initial_mouse_zone);

        strategy.begin_resizing();
    }

    void change(const mouse_event &event) override {
        if (finished)
            return;

        auto boundary_delta = get_boundary_delta(event.position.y);
        if (boundary_delta < handler.get_slot_height() / 2) {
            return;
        }

        index_t source_slot_index = get_source_slot_index(handler.current_direction, initial_mouse_zone);
        if (source_slot_index == handler.current_slot_index) {
            return;
        }

        if (event.has(event::shift_key)) {
            strategy.fill_time_slots_shifting(source_slot_index, handler.current_slot_index);
        } else {
            strategy.fill_time_slots(source_slot_index, handler.current_slot_index);
        }

        auto new_mouse_zone = handler.current_direction == direction::down
                                  ? mouse_zone::stretch_bottom
                                  : mouse_zone::stretch_top;

        handle_index = handler.current_slot_index;
        initial_mouse_zone = new_mouse_zone;

        select_sessions(event, handle_index, initial_mouse_zone);
    }

    void teardown(const mouse_event &event) override {
        if (handler.on_select_sessions)
            handler.on_select_sessions({});

        handler._resize_boundary = resize_boundary_configuration();

        if (handler.on_resize_boundary_change)
            handler.on_resize_boundary_change();

        strategy.end_resizing();
        finished = true;
    };

    void handle_direction_change() override {
        if (initial_mouse_zone == mouse_zone::stretch_top &&
            handler.current_direction == direction::down) {
            if (handle_index != boundary_slot_index + 1)
                handle_index = boundary_slot_index + 1;
        }

        if (initial_mouse_zone == mouse_zone::stretch_bottom &&
            handler.current_direction == direction::down) {
            if (handle_index != boundary_slot_index)
                handle_index = boundary_slot_index;
        }
    }

    gfloat get_boundary_delta(gfloat current_pos) {
        auto boundary_pos = get_boundary_position();
        auto delta = static_cast<int>(current_pos - boundary_pos);

        return std::abs(delta);
    }

    gfloat get_boundary_position() const {
        return handler.get_slot_height() / 2 + handler.get_slot_height() * ((gfloat) boundary_slot_index + 1);
    }

    void select_sessions(const mouse_event &event, index_t source_index, enum mouse_zone mouse_zone) {
        auto first_selection_index = strategy.sessions()
                                         .session_index_for_time_slot_index(source_index);

        if (mouse_zone == mouse_zone::stretch_top &&
            handler.current_direction == direction::down) {
            first_selection_index++;
        }

        if (mouse_zone == mouse_zone::stretch_bottom &&
            handler.current_direction == direction::up) {
            first_selection_index--;
        }

        auto second_selection_index = first_selection_index + 1;
        if (mouse_zone == mouse_zone::stretch_top) {
            second_selection_index = first_selection_index - 1;
        }

        auto first_session_index = std::min(first_selection_index, second_selection_index);
        if (first_session_index < 0) {
            boundary_slot_index = -1;
        } else {
            auto border_slot = strategy.sessions()[first_session_index].time_slots.back();
            boundary_slot_index = strategy.time_slots().index_of(border_slot).value_or(-1);
        }

        auto first_session_empty = !strategy.sessions().has_index(first_selection_index) ||
                                   !strategy.sessions()[first_selection_index].activity;

        auto second_session_empty = !strategy.sessions().has_index(second_selection_index) ||
                                    !strategy.sessions()[second_selection_index].activity;

        if (first_session_empty && second_session_empty) {
            teardown(mouse_event(point(), 0));
            handler.current_operaion = handler.make_operation<none_operation>();
            return;
        }

        if (handler.on_select_sessions) {
            auto selected = get_selected(first_selection_index, second_selection_index);

            if (event.has(event::shift_key)) {
                if (mouse_zone == mouse_zone::stretch_top) {
                    if (handler.current_direction == direction::down)
                        selected.erase(selected.begin());

                    if (handler.current_direction == direction::up)
                        selected.erase(selected.end() - 1);
                }

                if (mouse_zone == mouse_zone::stretch_bottom) {
                    if (handler.current_direction == direction::up)
                        selected.erase(selected.end() - 1);

                    if (handler.current_direction == direction::down)
                        selected.erase(selected.end() - 1);
                }
            }

            handler.on_select_sessions(selected);
        }

        handler._resize_boundary.slot_index = boundary_slot_index;
        handler._resize_boundary.session_index = first_session_index;

        if (handler.on_resize_boundary_change)
            handler.on_resize_boundary_change();
    }

    std::vector<index_t> get_selected(index_t first_selection_index, index_t second_selection_index) const {
        std::vector<index_t> selected;
        if (strategy.sessions().has_index(first_selection_index) &&
            strategy.sessions()[first_selection_index].activity != strategy::no_activity)
            selected.push_back(first_selection_index);

        if (strategy.sessions().has_index(second_selection_index) &&
            strategy.sessions()[second_selection_index].activity != strategy::no_activity)
            selected.push_back(second_selection_index);

        return selected;
    }

    index_t get_source_slot_index(const direction &direction, mouse_zone mouse_zone) const {
        auto source_slot_index = handle_index;
        if (mouse_zone == mouse_zone::stretch_top &&
            direction == direction::down) {
            source_slot_index--;
        } else if (mouse_zone == mouse_zone::stretch_bottom &&
                   direction == direction::up) {
            source_slot_index++;
        }

        return source_slot_index;
    };
};

struct stg::mouse_handler::select_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return select;
    }

private:
    std::unique_ptr<event> initial_event;
    stg::mouse_handler::index_t initial_slot_index = -1;

    void init(const mouse_event &event) override {
        initial_event = std::make_unique<mouse_event>(event);
        initial_slot_index = handler.current_slot_index;

        if (current_slot_is_selected() && !event.has(event::ctrl_key)) {
            handler.selection.set_is_clicked(true);
            return;
        }

        if (!is_safe(handler.current_slot_index))
            return;

        handler.selection.toggle_at(handler.current_slot_index);
    };

    void change(const mouse_event &event) override {
        if (current_slot_is_selected()) {
            return;
        }

        if (!is_safe(handler.current_slot_index)) {
            return;
        }

        handler.selection.fill(initial_slot_index, handler.current_slot_index);
    };

    void teardown(const mouse_event &event) override {
        if (handler.selection.is_clicked()) {
            handler.selection.set_is_clicked(false);

            if (initial_event->has(stg::event::left_key)) {
                if (handler.action_center)
                    handler.action_center->show_activities();

            } else if (initial_event->has(stg::event::right_key)) {
                handler.show_context_menu(event);
            }
        }
    };

    bool current_slot_is_selected() {
        return handler.selection.has_selected(handler.current_slot_index);
    }

    bool is_safe(int slot_index) {
        return slot_index >= 0 && slot_index < strategy.number_of_time_slots();
    }
};

#endif//STRATEGR_MOUSEHANDLEROPERATIONS_H
