//
// Created by Dmitry Khrykin on 2020-01-29.
//

#ifndef STRATEGR_MOUSE_HANDLER_H
#define STRATEGR_MOUSE_HANDLER_H

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
        int stretch_zone_height = 5;
        unsigned int resolution = 2;
    };

    class mouse_handler {
    public:
        enum class cursor {
            pointer,
            closed_hand,
            open_hand,
            resize
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

        explicit mouse_handler(stg::strategy &strategy,
                               stg::selection &selection,
                               std::function<int()> slot_height_getter,
                               std::function<rect()> bounds_getter,
                               const mouse_parameters &settings = mouse_parameters());

        ~mouse_handler();

        void mouse_press(const mouse_event &event);
        void mouse_move(const mouse_event &event);
        void mouse_release(const mouse_event &event);
        void key_down(const event &event);
        void key_up(const event &event);
    private:
        enum class mouse_zone {
            out_of_bounds = 0,
            drag,
            stretch_top,
            stretch_bottom
        };

        enum operation_type {
            none = 0,
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
        std::function<int()> get_slot_height = nullptr;

        stg::strategy &strategy;
        stg::selection &selection;

        mouse_parameters settings;

        cursor current_cursor = cursor::pointer;
        mouse_zone current_mouse_zone = mouse_zone::out_of_bounds;
        index_t current_slot_index = -1;
        index_t current_session_index = -1;

        std::unique_ptr<operation> current_operaion;

        float px_in_time();

        index_t get_slot_index(const mouse_event &event);
        index_t get_session_index(index_t slot_index);

        range get_session_range(index_t session_index);

        stg::mouse_handler::mouse_zone get_mouse_zone(int session_index, const point &mouse_pos);
        std::unique_ptr<operation> get_operation(event::key_modifiers modifiers);

        cursor get_cursor(event::key_modifiers modifiers);
        void update_cursor(event::key_modifiers modifiers);

        friend std::ostream &operator<<(std::ostream &os, mouse_zone zone);
        friend std::ostream &operator<<(std::ostream &os, operation_type op);
    };
}


#endif //STRATEGR_MOUSE_HANDLER_H
