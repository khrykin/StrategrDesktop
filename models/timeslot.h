//
// Created by Dmitry Khrykin on 2019-07-05.
//

#ifndef MODELS_TIMESLOT_H
#define MODELS_TIMESLOT_H

#include <optional>
#include <iostream>

#include "activity.h"

class Strategy;
struct TimeSlot {
    using Time = unsigned;
    using Duration = unsigned;

    static constexpr auto NoActivity = nullptr;

    Time beginTime;
    Duration duration;

    Activity *activity = NoActivity;

    TimeSlot(Time beginTime,
             Duration duration,
             Activity *activity = NoActivity) :
            beginTime(beginTime),
            duration(duration),
            activity(activity) {}

    Time endTime() const;

    friend std::ostream &operator<<(std::ostream &os,
                                    const TimeSlot &slot);

    friend bool operator==(const TimeSlot &lhs, const TimeSlot &rhs);
    friend bool operator!=(const TimeSlot &lhs, const TimeSlot &rhs);

};


#endif //MODELS_TIMESLOT_H
