//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include "timeslot.h"


std::ostream &operator<<(std::ostream &os, const TimeSlot &slot) {
    os << "TimeSlot(";
    if (slot.activity) {
        os << *slot.activity;
    } else {
        os << "None";
    }
    os << ", beginTime: " << slot.beginTime;
    os << ", duration: " << slot.duration;

    os << ")";
    return os;
}

TimeSlot::Time TimeSlot::endTime() const {
    return beginTime + duration;
}

bool operator==(const TimeSlot &lhs, const TimeSlot &rhs) {
    return lhs.activity == rhs.activity
           && lhs.beginTime == rhs.beginTime
           && lhs.duration == rhs.duration;
}

bool operator!=(const TimeSlot &lhs, const TimeSlot &rhs) {
    return !(lhs == rhs);
}