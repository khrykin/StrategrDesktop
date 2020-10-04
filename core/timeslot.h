//
// Created by Dmitry Khrykin on 2019-07-05.
//

#ifndef MODELS_TIMESLOT_H
#define MODELS_TIMESLOT_H

#include <ctime>
#include <iostream>

namespace stg {
    struct activity;

    struct time_slot {
        using minutes = unsigned;

        static constexpr auto no_activity = nullptr;

        minutes begin_time = 0;
        minutes duration = 0;

        stg::activity *activity = no_activity;

        time_slot(minutes begin_time, minutes duration, stg::activity *activity = no_activity);

        auto end_time() const -> minutes;

        auto calendar_begin_time() const -> std::time_t;
        auto calendar_end_time() const -> std::time_t;

        auto empty() const -> bool;

        friend auto operator==(const time_slot &lhs, const time_slot &rhs) -> bool;
        friend auto operator!=(const time_slot &lhs, const time_slot &rhs) -> bool;
        friend auto operator<<(std::ostream &os, const time_slot &slot) -> std::ostream &;

    private:
        struct keys {
            static constexpr auto begin_time = "start_time";
        };
    };
}

#endif//MODELS_TIMESLOT_H
