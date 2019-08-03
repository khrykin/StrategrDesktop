#ifndef ACTIVITYSESSION_H
#define ACTIVITYSESSION_H

#include <vector>
#include <memory>
#include <optional>
#include <iostream>

#include "activity.h"
#include "timeslot.h"

struct ActivitySession {
    using Length = unsigned int;
    using Time = TimeSlot::Time;
    using Duration = TimeSlot::Duration;
    using TimeSlotsState  = std::vector<const TimeSlot *>;

    TimeSlotsState timeSlots{};
    Activity *activity = TimeSlot::NoActivity;

    Length length() const;

    Time beginTime() const;

    Time endTime() const;

    Duration duration() const;

    friend bool operator==(const ActivitySession &lhs,
                           const ActivitySession &rhs);

    friend bool operator!=(const ActivitySession &lhs,
                           const ActivitySession &rhs);

    friend std::ostream &operator<<(std::ostream &os,
                                    const ActivitySession &session);
};


#endif // ACTIVITYSESSION_H
