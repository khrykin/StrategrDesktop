//
// Created by Dmitry Khrykin on 2019-07-05.
//

#ifndef MODELS_TIMESLOT_H
#define MODELS_TIMESLOT_H

#include <optional>
#include <iostream>

#include "activity.h"

namespace stg {
    class strategy;

    struct time_slot {
        using minutes = unsigned;
        using time_t = minutes;
        using duration_t = minutes;

        static constexpr auto no_activity = nullptr;

        time_t begin_time = 0;
        duration_t duration = 0;

        stg::activity *activity = no_activity;

        time_slot(time_t begin_time,
                  duration_t duration,
                  stg::activity *activity = no_activity) :
                begin_time(begin_time),
                duration(duration),
                activity(activity) {}

        time_t end_time() const;

        friend std::ostream &operator<<(std::ostream &os,
                                        const time_slot &slot);

        friend bool operator==(const time_slot &lhs, const time_slot &rhs);
        friend bool operator!=(const time_slot &lhs, const time_slot &rhs);
    private:
        struct keys {
            static constexpr auto begin_time = "start_time";
        };
    };
}

#endif //MODELS_TIMESLOT_H
