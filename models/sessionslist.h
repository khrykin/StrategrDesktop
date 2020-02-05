//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef STRATEGR_SESSIONSLIST_H
#define STRATEGR_SESSIONSLIST_H

#include <iostream>
#include <optional>

#include "session.h"
#include "privatelist.h"
#include "notifiableonchange.h"
#include "timeslotsstate.h"
#include "streamablelist.h"
#include "sessionscalculator.h"

namespace stg {
    using activity_sessions_list_base = private_list<session>;
    class strategy;

    class sessions_list :
            public activity_sessions_list_base,
            public notifiable_on_change,
            public streamable_list<sessions_list> {
    public:
        struct overview_item {
            float duration_percentage = 0.0;
            float begin_percentage = 0.0;
            std::optional<activity::color_t> color = std::nullopt;
        };

        struct bounds {
            index_t start_index = 0;
            index_t end_index = 0;

            bool contain(index_t index) {
                return index >= start_index && index <= end_index;
            }
        };

        std::vector<session> get_non_empty() const;

        bounds get_bounds_for(index_t session_index) const;

        stg::sessions_list::index_t session_index_for_time_slot_index(index_t time_slot_index) const;

        const session *session_after(const session &activity_session) const;
        const session *session_before(const session &activity_session) const;

        time_slot::time_t relative_begin_time(const session &session) const;

        std::vector<overview_item> overview() const;

        std::string class_print_name() const override;
    private:
        using activity_sessions_list_base::activity_sessions_list_base;

        void recalculate(const time_slots_state &time_slots_state);

        friend strategy;
    };
}

#endif //STRATEGR_SESSIONSLIST_H
