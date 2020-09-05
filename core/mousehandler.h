
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
#include "timer.h"
#include "action.hpp"

namespace stg {
    class action_center;

    struct mouse_parameters {
        stg::gfloat stretch_zone_size = 5;
        stg::gfloat direction_change_resolution = 2;
        stg::gfloat autoscroll_zone_size = 40;
    };

    class mouse_handler {
    public:
        using index_t = stg::strategy::time_slot_index_t;

        enum class cursor {
            pointer,
            closed_hand,
            open_hand,
            resize,
            drag_copy
        };

        enum class scroll_direction {
            up,
            down
        };

        struct range {
            stg::gfloat top = 0;
            stg::gfloat bottom = 0;

            auto contains(stg::gfloat coord) -> bool;

            friend auto operator<<(std::ostream &os, const range &range) -> std::ostream &;
        };

        struct resize_boundary_configuration {
            static constexpr index_t none = -2;
            
            index_t slot_index = resize_boundary_configuration::none;
            index_t session_index = resize_boundary_configuration::none;
        };

        struct context_menu_configuration {
            point position;
            index_t slot_index;
            index_t session_index;
            std::vector<const stg::action *> actions;
        };

        mouse_parameters settings;
        stg::action_center *action_center = nullptr;

        std::function<void(cursor)> on_cursor_change = nullptr;
        std::function<void(const std::vector<index_t> &)> on_select_sessions = nullptr;
        std::function<void()> on_resize_boundary_change = nullptr;
        std::function<point(gfloat scroll_offset_increment)> on_auto_scroll_frame = nullptr;
        std::function<void(index_t session_index, index_t first_slot_index)> on_draw_dragged_session = nullptr;
        std::function<void(const context_menu_configuration &)> on_show_context_menu = nullptr;

        explicit mouse_handler(stg::strategy &strategy,
                               stg::selection &selection,
                               std::function<gfloat()> slot_height_getter = nullptr,
                               std::function<rect()> viewport_getter = nullptr,
                               const mouse_parameters &settings = mouse_parameters());

        ~mouse_handler();

        auto resize_boundary() const -> const resize_boundary_configuration &;

        // Note: all event positions must be in slotboard view's coordinate space
        void mouse_press(const mouse_event &event);
        void mouse_move(const mouse_event &event);
        void mouse_release(const mouse_event &event);

        void key_down(const event &event);
        void key_up(const event &event);
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
            select,
            copy_drag
        };

        struct operation;
        struct none_operation;

        struct drag_operation;
        friend struct drag_operation;

        struct resize_operation;
        friend struct resize_operation;

        struct select_operation;
        friend struct select_operation;

        struct copy_drag_operation;
        friend struct copy_drag_operation;

        // Scroll view viewport frame relative to slotboard
        std::function<rect()> get_viewport = nullptr;
        std::function<gfloat()> get_slot_height = nullptr;

        stg::strategy &strategy;
        stg::selection &selection;

        std::unique_ptr<operation> current_operaion;
        std::shared_ptr<timer> autoscroll_timer = nullptr;

        cursor current_cursor = cursor::pointer;
        mouse_zone current_mouse_zone = mouse_zone::out_of_bounds;
        direction current_direction = direction::none;
        point previous_position = point::zero;
        event::key_modifiers current_key_modifiers;

        index_t current_slot_index = -1;
        index_t current_session_index = -1;

        resize_boundary_configuration _resize_boundary;

        auto px_in_time() -> float;

        template<class Type>
        auto make_operation() -> std::unique_ptr<Type> {
            return std::make_unique<Type>(this);
        }

        void handle_autoscroll(const stg::mouse_event &event);

        auto get_slot_index(const mouse_event &event) -> index_t;
        auto get_session_index(index_t slot_index) -> index_t;
        auto get_session_range(index_t session_index) -> range;
        auto get_direction(int delta) const -> direction;
        auto get_cursor(event::key_modifiers modifiers) -> cursor;

        auto get_mouse_zone(int session_index, const point &mouse_pos) -> mouse_zone;
        auto get_operation(event::key_modifiers modifiers) -> std::unique_ptr<operation>;

        void update_cursor(event::key_modifiers modifiers);
        void update_direction(const mouse_event &event);

        void start_autoscroll(scroll_direction direction);
        void stop_autoscroll();
        void auto_scroll_frame(const point &new_mouse_position);

        void show_context_menu(const mouse_event &event);

        auto context_actions() -> std::vector<const stg::action *>;

        friend auto operator<<(std::ostream &os, operation_type op) -> std::ostream &;
        friend auto operator<<(std::ostream &os, direction direction) -> std::ostream &;
        friend auto operator<<(std::ostream &os, scroll_direction direction) -> std::ostream &;
        friend auto operator<<(std::ostream &os, mouse_zone zone) -> std::ostream &;
    };
}


#endif //STRATEGR_MOUSEHANDLER_H
