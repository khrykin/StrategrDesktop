//
// Created by Dmitry Khrykin on 2020-01-29.
//

#ifndef STRATEGR_MOUSEHANDLEROPERATIONS_H
#define STRATEGR_MOUSEHANDLEROPERATIONS_H

#include "mousehandler.h"

struct stg::mouse_handler::operation {
    explicit operation(mouse_handler *handler) : handler(*handler) {}

    virtual ~operation() = default;

    virtual operation_type type() = 0;

    virtual void init(const mouse_event &event) = 0;
    virtual void perform(const mouse_event &event) = 0;
    virtual void teardown(const mouse_event &event) = 0;

    virtual void handle_direction_change() {};
protected:
    mouse_handler &handler;
    stg::strategy &strategy = handler.strategy;
};

struct stg::mouse_handler::none_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return none;
    }

    void init(const mouse_event &event) override {};

    void perform(const mouse_event &event) override {};

    void teardown(const mouse_event &event) override {};
};

struct stg::mouse_handler::drag_operation : public operation {
    using operation::operation;

    operation_type type() override {
        return drag;
    }

    void init(const mouse_event &event) override {
        handler.selection.deselect_all();

        if (handler.on_select_sessions)
            handler.on_select_sessions({handler.current_session_index});

        strategy.begin_dragging(handler.current_session_index);

        previous_session_index = handler.current_session_index;
        previous_slot_index = handler.current_slot_index;
        previous_position = event.position;
    };

    void perform(const mouse_event &event) override {
        auto current_session = strategy.sessions()[handler.current_session_index];
        auto session_slots_indices = strategy.sessions()
                .get_bounds_for(previous_session_index);

        auto boundary_delta = get_delta(previous_position.y, event.position.y);
        if (boundary_delta < handler.get_slot_height() / 2) {
            return;
        }

        int distance = handler.current_slot_index - previous_slot_index;

        if (!distance)
            return;

        strategy.drag_session(previous_session_index, distance);

        previous_session_index = strategy.sessions()
                .session_index_for_time_slot_index(handler.current_slot_index);
        previous_slot_index = handler.current_slot_index;
        previous_position = event.position;

        if (handler.on_select_sessions)
            handler.on_select_sessions({previous_session_index});
    };

    void teardown(const mouse_event &event) override {
        strategy.end_dragging();

        if (handler.on_select_sessions)
            handler.on_select_sessions({});
    };
private:
    point previous_position;

    index_t previous_slot_index = -1;
    index_t previous_session_index = -1;

    int get_delta(int prev_pos, int current_pos) {
        auto slot_height = handler.get_slot_height();
        auto closest_slot_boundary = std::round(prev_pos / slot_height) * slot_height;

        return std::abs(static_cast<int>(current_pos - closest_slot_boundary));
    }
};

struct stg::mouse_handler::resize_operation : public operation {
    using operation::operation;
    static constexpr auto nothing_selected_index = -2;

    operation_type type() override {
        return resize;
    }

    void init(const mouse_event &event) override {
        handler.selection.deselect_all();

        handle_index = handler.current_slot_index;
        initial_mouse_zone = handler.current_mouse_zone;
        initial_position = event.position;

        select_sessions(handle_index, initial_mouse_zone);

        strategy.begin_resizing();
    }

    void perform(const mouse_event &event) override {
        if (finished)
            return;

        auto boundary_delta = get_boundary_delta(initial_position.y, event.position.y);
        if (boundary_delta < handler.get_slot_height() / 2) {
            return;
        }

        index_t source_slot_index = get_source_slot_index(handler.current_direction, initial_mouse_zone);
        if (source_slot_index == handler.current_slot_index) {
            return;
        }

        strategy.fill_time_slots(source_slot_index, handler.current_slot_index);

        auto new_mouse_zone = handler.current_direction == direction::down
                              ? mouse_zone::stretch_bottom
                              : mouse_zone::stretch_top;

        handle_index = handler.current_slot_index;
        initial_position = event.position;
        initial_mouse_zone = new_mouse_zone;

        select_sessions(handle_index, initial_mouse_zone);
    }

    void teardown(const mouse_event &event) override {
        if (handler.on_select_sessions)
            handler.on_select_sessions({});

        if (handler.on_resize_boundary_change)
            handler.on_resize_boundary_change(nothing_selected_index,
                                              nothing_selected_index);

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

private:
    bool finished = false;

    index_t handle_index = -1;
    index_t boundary_slot_index = -1;

    point initial_position;
    mouse_zone initial_mouse_zone = mouse_zone::out_of_bounds;

    int get_boundary_delta(int prev_pos, int current_pos) {
        auto boundary_pos = get_boundary_position();
        auto delta = static_cast<int>(current_pos - boundary_pos);

        return std::abs(delta);
    }

    int get_boundary_position() const {
        return handler.get_slot_height() * (boundary_slot_index + 1);
    }

    void select_sessions(index_t source_index,
                         enum mouse_zone mouse_zone) {
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

        if (!strategy.sessions()[first_selection_index].activity &&
            !strategy.sessions()[second_selection_index].activity) {

            teardown(mouse_event(point(), 0));
            return;
        }

        if (handler.on_select_sessions) {
            auto selected = get_selected(first_selection_index, second_selection_index);
            handler.on_select_sessions(selected);
        }

        if (handler.on_resize_boundary_change)
            handler.on_resize_boundary_change(first_session_index, boundary_slot_index);
    }

    std::vector<index_t> get_selected(index_t first_selection_index, index_t second_selection_index) const {
        std::__1::vector<index_t> selected;
        if (strategy.sessions().has_index(first_selection_index))
            selected.push_back(first_selection_index);

        if (strategy.sessions().has_index(second_selection_index))
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

    void init(const mouse_event &event) override {
        initial_event = std::make_unique<mouse_event>(event);

        if (current_slot_is_selected() && !event.with(event::ctrl_key)) {
            handler.selection.set_is_clicked(true);
            return;
        }

        handler.selection.toggle_at(handler.current_slot_index);
    };

    void perform(const mouse_event &event) override {
        if (current_slot_is_selected()) {
            return;
        }

        handler.selection.toggle_at(handler.current_slot_index);
    };

    void teardown(const mouse_event &event) override {
        if (handler.selection.is_clicked()) {
            handler.selection.set_is_clicked(false);

            if (initial_event->with(stg::event::left_key)) {
                if (handler.on_open_activities)
                    handler.on_open_activities();

            } else if (initial_event->with(stg::event::right_key)) {
                if (handler.on_show_context_menu)
                    handler.on_show_context_menu(event.position);
            }
        }
    };

private:
    std::unique_ptr<event> initial_event;

    bool current_slot_is_selected() {
        return handler.selection.has_selected(handler.current_slot_index);
    }
};

#endif //STRATEGR_MOUSEHANDLEROPERATIONS_H
