//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include "timeslot.h"
#include "activity.h"

std::ostream &stg::operator<<(std::ostream &os, const stg::time_slot &slot) {
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

stg::time_slot::time_t stg::time_slot::end_time() const {
    return begin_time + duration;
}

bool stg::operator==(const stg::time_slot &lhs, const stg::time_slot &rhs) {
    return lhs.activity == rhs.activity
           && lhs.begin_time == rhs.begin_time
           && lhs.duration == rhs.duration;
}

bool stg::operator!=(const stg::time_slot &lhs, const stg::time_slot &rhs) {
    return !(lhs == rhs);
}

bool stg::time_slot::empty() const {
    return activity == no_activity;
}
