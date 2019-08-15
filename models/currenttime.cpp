//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include "currenttime.h"

CurrentTime::Minutes CurrentTime::currentMinutes() {
    using namespace std::chrono;
    auto duration = currentDayDuration();
    return duration_cast<minutes>(duration).count();
}

CurrentTime::Timestamp
CurrentTime::startOfADayFromTimestamp(CurrentTime::Timestamp timestamp) {
    auto dayComponents = *dayComponentsFromTimestamp(&timestamp);

    dayComponents.tm_hour = 0;
    dayComponents.tm_min = 0;
    dayComponents.tm_sec = 0;

    timestamp = timestampFromDayComponents(&dayComponents);

    return timestamp;
}

CurrentTime::Duration CurrentTime::currentDayDuration() {
    using namespace std::chrono;

    auto clockNow = system_clock::now();
    auto currentTimestamp = system_clock::to_time_t(clockNow);

    auto startOfADayTimestamp = startOfADayFromTimestamp(currentTimestamp);
    auto clockStartOfToday = system_clock::from_time_t(startOfADayTimestamp);

    return clockNow - clockStartOfToday;
}
