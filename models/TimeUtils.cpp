//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include "TimeUtils.h"

TimeUtils::Minutes TimeUtils::currentMinutes() {
    return currentSeconds() / 60;
}

TimeUtils::Seconds TimeUtils::currentSeconds() {
    using namespace std::chrono;
    auto duration = currentDayDuration();
    return duration_cast<seconds>(duration).count();
}

TimeUtils::Timestamp
TimeUtils::startOfADayFromTimestamp(TimeUtils::Timestamp timestamp) {
    auto dayComponents = *dayComponentsFromTimestamp(&timestamp);

    dayComponents.tm_hour = 0;
    dayComponents.tm_min = 0;
    dayComponents.tm_sec = 0;

    timestamp = timestampFromDayComponents(&dayComponents);

    return timestamp;
}

TimeUtils::Duration TimeUtils::currentDayDuration() {
    using namespace std::chrono;

    auto clockNow = system_clock::now();
    auto currentTimestamp = system_clock::to_time_t(clockNow);

    auto startOfADayTimestamp = startOfADayFromTimestamp(currentTimestamp);
    auto clockStartOfToday = system_clock::from_time_t(startOfADayTimestamp);

    return clockNow - clockStartOfToday;
}

std::string TimeUtils::humanTimeForMinutes(TimeUtils::Minutes minutes) {
    if (minutes < 1) {
        return "Less than 1 min";
    }

    unsigned int hours = minutes / 60;
    unsigned int minsRemainder = minutes - hours * 60;

    std::string result;

    if (hours > 0) {
        result += std::to_string(hours);
    }

    if (hours > 0 && minsRemainder == 30) {
        result += ".5 h";
    } else if (hours > 0) {
        result += " h";
    }

    if (minsRemainder != 0) {
        if (hours > 0) {
            if (minsRemainder == 30) {
                return result;
            }

            result += " ";
        }

        result += std::to_string(minsRemainder) + " min";
    }

    return result;
}
