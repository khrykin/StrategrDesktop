//
// Created by Dmitry Khrykin on 15.06.2020.
//

#include "actioncenter.h"

#include <utility>
#include "mousehandler.h"

namespace stg {

#pragma mark - Construction

    action_center::action_center(stg::strategy &strategy,
                                 std::function<gfloat()> slot_height_getter,
                                 std::function<rect()> viewport_getter)
            : strategy(strategy),
              _mouse_handler(strategy,
                             _selection,
                             std::move(slot_height_getter),
                             std::move(viewport_getter)) {
        _mouse_handler.action_center = this;

        strategy.sessions().add_on_change_callback([this] {
            stg::timer::schedule(1, false, [this] { lazily_update_current_session(); });
        });

        auto timer_callback = [this] {
            if (on_update_current_time_marker)
                on_update_current_time_marker();

            if (!this->strategy.is_dragging() && !this->strategy.is_resizing()) {
                update_current_session();
            }
        };

        strategy.time_slots().add_on_ruler_change_callback([=] {
            timer_callback();
        });

        auto timer_interval = 5;
        timer = stg::timer::schedule(timer_interval, true, timer_callback);

        timer_callback();
    }

#pragma mark - Acessing Depenent Structures

    auto action_center::selection() -> stg::selection & {
        return _selection;
    }

    auto action_center::mouse_handler() -> stg::mouse_handler & {
        return _mouse_handler;
    }

#pragma mark - Current Session

    auto action_center::current_session_is_shown() const -> bool {
        return _current_session_is_shown;
    }

    void action_center::update_current_session() {
        auto *active_session = this->strategy.active_session();

        if (active_session) {
            auto now = std::chrono::system_clock::now();
            if (now - last_current_time_update_time < std::chrono::seconds{1})
                return;

            last_current_time_update_time = now;

            if (on_reload_current_session)
                on_reload_current_session();
        }

        if (!active_session && _current_session_is_shown) {
            _current_session_is_shown = false;

            if (on_toggle_current_session)
                on_toggle_current_session(false);
        } else if (active_session && !_current_session_is_shown) {
            _current_session_is_shown = true;

            if (on_toggle_current_session)
                on_toggle_current_session(true);
        }
    }

    void action_center::lazily_update_current_session() {
        if (!this->strategy.is_dragging() && !this->strategy.is_resizing()) {
            update_current_session();
        } else {
            stg::timer::schedule(1, false, [this] { lazily_update_current_session(); });
        }
    }
}
