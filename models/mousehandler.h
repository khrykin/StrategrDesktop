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

namespace stg {
    struct mouse_event {
        using key_modifiers = uint8_t;

        static constexpr key_modifiers left_key = 1u << 0u;
        static constexpr key_modifiers right_key = 1u << 1u;
        static constexpr key_modifiers ctrl_key = 1u << 2u;

        mouse_event(const point &position, key_modifiers modifiers);

        template<class QtLikeEvent>
        mouse_event(QtLikeEvent *evt) : position(evt->pos()) {
            if (evt->buttons() == 1) {
                modifiers |= left_key;
            } else if (evt->buttons() == 2) {
                modifiers |= right_key;
            }

            if (evt->modifiers() == 0x04000000) {
                modifiers |= ctrl_key;
            };
        }

        bool is(key_modifiers mod) const;
        bool with(key_modifiers mod) const;

        friend std::ostream &operator<<(std::ostream &os, const mouse_event &e);

        point position;
        key_modifiers modifiers = 0;
    };

    struct mousehandler_parameters {
        int stretch_zone_height = 5;
    };

    class mousehandler {
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

            bool contains(int coord) {
                return coord >= top && coord <= bottom;
            }

            friend std::ostream &operator<<(std::ostream &os, const range &range);
        };

        std::function<void(cursor)> on_cursor_change = nullptr;
        std::function<void(bool)> on_set_selected = nullptr;

        explicit mousehandler(stg::strategy &strategy,
                              std::function<int()> slot_height_getter,
                              std::function<rect()> bounds_getter,
                              const mousehandler_parameters &settings = mousehandler_parameters());

        ~mousehandler();

        void mouse_press(const mouse_event &event);
        void mouse_move(const mouse_event &event);
        void mouse_release(const mouse_event &event);

        void update_cursor(const mouse_event &event);
    private:
        enum class mouse_zone {
            out_of_bounds = 0,
            drag,
            stretch_top,
            stretch_bottom
        };

        enum class direction {
            up,
            down,
        };

        enum operation_type {
            none = 0,
            drag,
            stretch_top,
            stretch_bottom,
            select
        };

        struct operation;
        struct none_operation;
        struct drag_operation;
        struct stretch_top_operation;
        struct stretch_bottom_operation;
        struct select_operation;

        using index_t = stg::strategy::time_slot_index_t;

        std::function<rect()> get_bounds = nullptr;
        std::function<int()> get_slot_height = nullptr;

        stg::strategy &strategy;
        mousehandler_parameters settings;

        cursor current_cursor = cursor::pointer;
        std::unique_ptr<operation> current_operaion;

        float px_in_time();

        index_t get_slot_index(const mouse_event &event);
        index_t get_session_index(index_t slot_index);
        range get_session_range(const mouse_event &event);
        mouse_zone get_mouse_zone(mouse_event event);
        cursor get_cursor(const mouse_event &event);
        std::unique_ptr<stg::mousehandler::operation> get_operation(const mouse_event &event);

        void set_selected(bool selected);

        friend std::ostream &operator<<(std::ostream &os, mouse_zone zone);
    };

}


#endif //STRATEGR_MOUSEHANDLER_H
