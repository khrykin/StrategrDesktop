#ifndef ACTIVITYSESSION_H
#define ACTIVITYSESSION_H

#include <vector>
#include <memory>
#include <optional>
#include <iostream>

#include "activity.h"
#include "timeslot.h"

struct ActivitySession {
    using Length = size_t;
    using Time = TimeSlot::Time;
    using Duration = TimeSlot::Duration;

    std::vector<const TimeSlot *> timeSlots;
    const Activity *activity = nullptr;

    Length length() const {
        return timeSlots.size();
    };

    Time beginTime() const {
        return timeSlots.front()->beginTime;
    }

    Time endTime() const {
        return timeSlots.back()->endTime();
    }

    Duration duration() const {
        return endTime() - beginTime();
    }

    friend std::ostream &operator<<(std::ostream &os, const ActivitySession &session) {
        os << "ActivitySession(";

        if (session.activity) {
            os << *session.activity;
        } else {
            os << "None";
        }

        os << ", length: " << session.length();
        os << ", beginTime: " << session.beginTime();

        os << ")";

        return os;
    }
};

#endif // ACTIVITYSESSION_H
