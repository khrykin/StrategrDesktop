//
// Created by Dmitry Khrykin on 2020-01-29.
//

#ifndef STRATEGR_MOUSEHANDLER_H
#define STRATEGR_MOUSEHANDLER_H

#include <cstdint>
#include <ostream>
#include <iostream>
#include <functional>
#include <utility>
#include <optional>
#include <memory>

#include "geometry.h"
#include "strategy.h"
#include "event.h"
#include "selection.h"

namespace stg {
    struct mouse_parameters {
        int stretch_zone_size = 5;
        int direction_change_resolution = 2;
        int autoscroll_zone_size = 70;
    };

    class mouse_handler {
    public:
        enum class cursor {
            pointer,
            closed_hand,
            open_hand,
            resize
        };

        enum class scroll_direction {
            up,
            down
        };

        struct range {
            int top = 0;
            int bottom = 0;

            bool contains(int coord);

            friend std::ostream &operator<<(std::ostream &os, const range &range);
        };

        std::function<void(cursor)> on_cursor_change = nullptr;
        std::function<void()> on_open_activities = nullptr;
        std::function<void(const point &)> on_show_context_menu = nullptr;
        std::function<void(const std::vector<index_t> &)> on_select_sessions = nullptr;
        std::function<void(index_t, index_t)> on_resize_boundary_change = nullptr;
        std::function<void(scroll_direction)> on_start_auto_scroll = nullptr;
        std::function<void()> on_stop_auto_scroll = nullptr;
        std::function<void(const point &, index_t, index_t)> on_context_menu_event = nullptr;

        explicit mouse_handler(stg::strategy &strategy,
                               stg::selection &selection,
                               std::function<int()> slot_height_getter,
                               std::function<rect()> bounds_getter,
                               std::function<rect()> viewport_getter,
                               const mouse_parameters &settings = mouse_parameters());

        ~mouse_handler();

        void mouse_press(const mouse_event &event);
        void mouse_move(const mouse_event &event);
        void mouse_release(const mouse_event &event);
        void key_down(const event &event);
        void key_up(const event &event);

        void auto_scroll_frame(const point &new_mouse_position);
    private:
        enum class mouse_zone {
            out_of_bounds,
            drag,
            stretch_top,
            stretch_bottom
        };

        enum class direction {
            none,
            up,
            down,
        };

        enum operation_type {
            none,
            drag,
            resize,
            select
        };

        struct operation;
        struct none_operation;

        struct drag_operation;
        friend struct drag_operation;

        struct resize_operation;
        friend struct resize_operation;

        struct select_operation;
        friend struct select_operation;

        using index_t = stg::strategy::time_slot_index_t;

        std::function<rect()> get_bounds = nullptr;
        std::function<rect()> get_viewport = nullptr;
        std::function<int()> get_slot_height = nullptr;

        stg::strategy &strategy;
        stg::selection &selection;

        mouse_parameters settings;

        std::unique_ptr<operation> current_operaion;

        cursor current_cursor = cursor::pointer;
        mouse_zone current_mouse_zone = mouse_zone::out_of_bounds;
        direction current_direction = direction::none;
        point previous_position = point::zero;
        event::key_modifiers current_key_modifiers;

        index_t current_slot_index = -1;
        index_t current_session_index = -1;

        bool autoscroll_is_on = false;

        float px_in_time();

        template<class Type>
        std::unique_ptr<Type> make_operation() {
            return std::make_unique<Type>(this);
        }

        void handle_autoscroll(const stg::mouse_event &event);

        index_t get_slot_index(const mouse_event &event);
        index_t get_session_index(index_t slot_index);
        range get_session_range(index_t session_index);
        direction get_direction(int delta);
        cursor get_cursor(event::key_modifiers modifiers);

        stg::mouse_handler::mouse_zone get_mouse_zone(int session_index, const point &mouse_pos);
        std::unique_ptr<operation> get_operation(event::key_modifiers modifiers);

        void update_cursor(event::key_modifiers modifiers);
        void update_direction(const mouse_event &event);

        friend std::ostream &operator<<(std::ostream &os, operation_type op);
        friend std::ostream &operator<<(std::ostream &os, direction direction);
        friend std::ostream &operator<<(std::ostream &os, scroll_direction direction);
        friend std::ostream &operator<<(std::ostream &os, mouse_zone zone);
    };
}


#endif //STRATEGR_MOUSEHANDLER_H
