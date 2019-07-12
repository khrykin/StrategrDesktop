//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_CURRENTTIMESESSION_H
#define STRATEGR_CURRENTTIMESESSION_H

#include <ctime>
#include <chrono>
#include <iostream>
#include "activitysession.h"

class Strategy;
class CurrentTimeSession {
public:
    using Minutes = unsigned;

    static std::optional<ActivitySession> forStrategy(const Strategy &strategy);
    static std::optional<ActivitySession> upcomingForStrategy(const Strategy &strategy, Minutes inMinutes);

private:
    using Timestamp = std::time_t;
    using DayComponents = std::tm;
    using Duration = std::chrono::duration<double>;

    constexpr static auto dayComponentsFromTimestamp = std::localtime;
    constexpr static auto timestampFromDayComponents = std::mktime;

    static Timestamp startOfADayFromTimestamp(Timestamp timestamp);

    static Duration currentDayDuration();

    static Minutes currentMinutes();
};


#endif //STRATEGR_CURRENTTIMESESSION_H
