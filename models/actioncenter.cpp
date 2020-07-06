//
// Created by Dmitry Khrykin on 15.06.2020.
//

#include "actioncenter.h"
#include "mousehandler.h"

namespace stg {
    action_center::action_center(stg::strategy &strategy,
                                 stg::mouse_handler &mouse_handler,
                                 stg::selection &selection)
            : strategy(strategy),
              mouse_handler(mouse_handler),
              selection(selection) {
        mouse_handler.action_center = this;

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

        auto timer_interval = 5;
        timer = stg::timer::schedule(timer_interval, true, timer_callback);

        timer_callback();
    }

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
