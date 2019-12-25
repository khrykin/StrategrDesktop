#ifndef ACTIVITYSESSION_H
#define ACTIVITYSESSION_H

#include <vector>
#include <memory>
#include <optional>
#include <iostream>

#include "Activity.h"
#include "TimeSlot.h"

struct Session {
    using Length = unsigned int;
    using Time = TimeSlot::Time;
    using Duration = TimeSlot::Duration;
    using TimeSlotsState  = std::vector<TimeSlot>;

    TimeSlotsState timeSlots{};
    Activity *activity = TimeSlot::NoActivity;

    Length length() const;
    Time beginTime() const;
    Time endTime() const;
    Duration duration() const;

    double progress() const;

    bool isCurrent() const;
    bool isPast() const;
    bool isFuture() const;

    Duration passedMinutes() const;

    Duration leftMinutes() const;

    friend bool operator==(const Session &lhs,
                           const Session &rhs);

    friend bool operator!=(const Session &lhs,
                           const Session &rhs);

    friend std::ostream &operator<<(std::ostream &os,
                                    const Session &session);
};


#endif // ACTIVITYSESSION_H
