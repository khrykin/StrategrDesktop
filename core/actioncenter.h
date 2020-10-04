//
// Created by Dmitry Khrykin on 15.06.2020.
//

#ifndef STRATEGR_ACTIONCENTER_H
#define STRATEGR_ACTIONCENTER_H

#include <chrono>

#include "action.hpp"
#include "mousehandler.h"
#include "selection.h"
#include "strategy.h"
#include "timer.h"

namespace stg {
    class action_center {
    public:
#pragma mark - Delegate Callbacks

        std::function<void()> on_show_activities = nullptr;
        std::function<void()> on_show_sessions = nullptr;
        std::function<void(bool)> on_toggle_current_session = nullptr;
        std::function<void()> on_reload_current_session = nullptr;
        std::function<void()> on_update_current_time_marker = nullptr;
        std::function<void(bool animated)> on_scroll_to_current_time = nullptr;

#pragma mark - Construction

        action_center(stg::strategy &strategy,
                      std::function<gfloat()> slot_height_getter,
                      std::function<rect()> viewport_getter);

#pragma mark - Acessing Depenent Structures

        auto selection() -> stg::selection &;
        auto mouse_handler() -> stg::mouse_handler &;

#pragma mark - Actions

        const action show_activities{"Show Activities",
                                     [this] {
                                         if (on_show_activities)
                                             on_show_activities();
                                     }};

        const action show_sessions{"Show Sessions",
                                   [this] {
                                       if (on_show_sessions)
                                           on_show_sessions();
                                   }};

        const action undo{"Undo",
                          [this] { this->strategy.undo(); },// Note: Explicit this here and below due to MSVC++ bug.
                          [this] { return strategy.has_undo(); }};

        const action redo{"Redo",
                          [this] { this->strategy.redo(); },
                          [this] { return strategy.has_redo(); }};

        const action activities_undo{"Undo",
                                     [this] { this->strategy.undo(); },
                                     [this] { return strategy.has_activities_undo(); }};

        const action activities_redo{"Redo",
                                     [this] { this->strategy.redo(); },
                                     [this] { return strategy.has_activities_redo(); }};

        const action set_activity{"Set Activity",
                                  [this] { if (on_show_activities) on_show_activities(); },
                                  [this] { return !_selection.empty(); }};

        const action empty_selection{"Delete",
                                     [this] {
                                         this->strategy.make_empty_at(_selection);
                                         this->_selection.deselect_all();
                                     },
                                     [this] {
                                         return !_selection.empty() && !_selection.only_empty_selected();
                                     },
                                     action::destructive};

        const action make_room{"Make Room",
                               [this] {
                                   this->strategy.shift_below_time_slot(_selection.front(),
                                                                        (stg::index_t) _selection.size());
                                   this->_selection.deselect_all();
                               },
                               [this] { return _selection.is_continuous() && !_selection.only_empty_selected(); }};

        const action deselect{"Deselect", [this] { this->_selection.deselect_all(); },
                              [this] { return !_selection.empty(); }};

        const action select_all{"Select All",
                                [this] { this->_selection.select_all(); },
                                [this] { return !_selection.is_all_selected(); }};

        const action go_to_current_time{"Go To Current Time",
                                        [this] {
                                            if (on_scroll_to_current_time)
                                                on_scroll_to_current_time(true);
                                        }};

        const action reorder_activities_by_usage{"Reorder Activities By Usage",
                                                 [this] { this->strategy.reorder_activities_by_usage(); }};

        void place_activity_in_selection(index_t actvity_index) {
            strategy.place_activity(actvity_index, _selection);
            _selection.deselect_all();

            if (on_show_sessions) on_show_sessions();
        }

#pragma mark - Current Session Visibility

        auto current_session_is_shown() const -> bool;

    private:
        stg::strategy &strategy;
        stg::mouse_handler _mouse_handler;
        stg::selection _selection{strategy};

        std::shared_ptr<stg::timer> timer;

        bool _current_session_is_shown = strategy.active_session() != nullptr;
        std::chrono::time_point<std::chrono::system_clock> last_current_time_update_time{};

        void update_current_session();
        void lazily_update_current_session();
    };
}


#endif//STRATEGR_ACTIONCENTER_H
