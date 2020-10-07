//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef STRATEGR_SESSIONSLIST_H
#define STRATEGR_SESSIONSLIST_H

#include <iostream>
#include <optional>

#include "activity.h"
#include "notifiableonchange.h"
#include "privatelist.h"
#include "session.h"
#include "streamablelist.h"
#include "timeslotsstate.h"

namespace stg {
    using sessions_list_base = private_list<session>;
    class strategy;

    class sessions_list : public sessions_list_base,
                          public notifiable_on_change,
                          public streamable_list<sessions_list> {
    public:
        struct overview_item {
            float duration_percentage = 0.0;
            float begin_percentage = 0.0;
            std::optional<stg::color> color = std::nullopt;
        };

        struct bounds {
            index_t start_index = 0;
            index_t end_index = 0;

            auto contain(index_t index) const -> bool {
                return index >= start_index && index <= end_index;
            }
        };

        auto get_non_empty() const -> std::vector<session>;
        auto get_bounds_for(index_t session_index) const -> bounds;

        auto session_index_for_time_slot_index(index_t time_slot_index) const -> stg::sessions_list::index_t;

        auto session_after(const session &activity_session) const -> const session *;
        auto session_before(const session &activity_session) const -> const session *;

        auto relative_begin_time(const session &session) const -> time_slot::minutes;
        auto duration() const -> time_slot::minutes;

        auto overview() const -> std::vector<overview_item>;

        auto class_print_name() const -> std::string override;

    private:
        using sessions_list_base::sessions_list_base;

        void recalculate(const time_slots_state &time_slots);

        friend strategy;
    };
}

#endif//STRATEGR_SESSIONSLIST_H
