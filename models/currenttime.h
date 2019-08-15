//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_CURRENTTIME_H
#define STRATEGR_CURRENTTIME_H

#include <ctime>
#include <chrono>
#include <iostream>

class CurrentTime {
public:
    using Seconds = unsigned;
    using Minutes = unsigned;

    using Timestamp = std::time_t;
    using DayComponents = std::tm;
    using Duration = std::chrono::duration<double>;

    constexpr static auto dayComponentsFromTimestamp = std::localtime;
    constexpr static auto timestampFromDayComponents = std::mktime;

    static Timestamp startOfADayFromTimestamp(Timestamp timestamp);
    static Duration currentDayDuration();
    static Minutes currentMinutes();
};


#endif //STRATEGR_CURRENTTIME_H
