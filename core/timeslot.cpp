//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include "timeslot.h"
#include "activity.h"
#include "time_utils.h"

namespace stg {
    time_slot::time_slot(minutes begin_time, minutes duration, stg::activity *activity) : begin_time(begin_time),
                                                                                          duration(duration),
                                                                                          activity(activity) {}

    auto time_slot::end_time() const -> time_slot::minutes {
        return begin_time + duration;
    }

    auto time_slot::empty() const -> bool {
        return activity == no_activity;
    }

    auto time_slot::calendar_begin_time() const -> std::time_t {
        return time_utils::calendar_time_from_minutes(begin_time);
    }

    auto time_slot::calendar_end_time() const -> std::time_t {
        return time_utils::calendar_time_from_minutes(end_time());
    }

    auto operator==(const time_slot &lhs, const time_slot &rhs) -> bool {
        return lhs.activity == rhs.activity && lhs.begin_time == rhs.begin_time && lhs.duration == rhs.duration;
    }

    auto operator!=(const time_slot &lhs, const time_slot &rhs) -> bool {
        return !(lhs == rhs);
    }

    auto operator<<(std::ostream &os, const time_slot &slot) -> std::ostream & {
        os << "time_slot(";
        if (slot.activity) {
            os << *slot.activity;
        } else {
            os << "none";
        }
        os << ", begin_time: " << slot.begin_time;
        os << ", duration: " << slot.duration;

        os << ")";
        return os;
    }
}