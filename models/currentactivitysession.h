//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_CURRENTACTIVITYSESSION_H
#define STRATEGR_CURRENTACTIVITYSESSION_H

#include <ctime>
#include <chrono>
#include <iostream>
#include "activitysession.h"

class Strategy;
class CurrentActivitySession {
public:
    using Seconds = unsigned;

    /// Returns current session if it's non-empty.
    static std::optional<ActivitySession>
    forStrategy(const Strategy &strategy);

    /// Returns upcoming session if it's non-empty.
    static std::optional<ActivitySession>
    upcomingForStrategy(const Strategy &strategy);

private:
    using Timestamp = std::time_t;
    using DayComponents = std::tm;
    using Duration = std::chrono::duration<double>;

    constexpr static auto dayComponentsFromTimestamp = std::localtime;
    constexpr static auto timestampFromDayComponents = std::mktime;

    static Timestamp startOfADayFromTimestamp(Timestamp timestamp);

    static Duration currentDayDuration();

    static Seconds currentMinutes();
    static std::optional<ActivitySession> currentSessionForStrategy(const Strategy &strategy);
};


#endif //STRATEGR_CURRENTACTIVITYSESSION_H
