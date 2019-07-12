//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include "currenttimesession.h"
#include "strategy.h"

CurrentTimeSession::Timestamp CurrentTimeSession::startOfADayFromTimestamp(CurrentTimeSession::Timestamp timestamp) {
    auto dayComponents = *dayComponentsFromTimestamp(&timestamp);

    dayComponents.tm_hour = 0;
    dayComponents.tm_min = 0;
    dayComponents.tm_sec = 0;

    timestamp = timestampFromDayComponents(&dayComponents);

    return timestamp;
}

CurrentTimeSession::Duration CurrentTimeSession::currentDayDuration() {
    using namespace std::chrono;

    auto clockNow = system_clock::now();
    auto currentTimestamp = system_clock::to_time_t(clockNow);

    auto startOfADayTimestamp = startOfADayFromTimestamp(currentTimestamp);
    auto clockStartOfToday = system_clock::from_time_t(startOfADayTimestamp);

    return clockNow - clockStartOfToday;
}

std::optional<ActivitySession>
CurrentTimeSession::forStrategy(const Strategy &strategy) {
    auto currentMinutes = CurrentTimeSession::currentMinutes();
    for (auto &session : strategy.activitySessions()) {
        if (currentMinutes >= session.beginTime()
            && currentMinutes <= session.endTime()) {
            return session;
        }
    }

    return std::nullopt;
}

std::optional<ActivitySession>
CurrentTimeSession::upcomingForStrategy(const Strategy &strategy,
                                        Minutes inMinutes) {
    auto currentMinutes = CurrentTimeSession::currentMinutes();
    auto targetTime = currentMinutes + inMinutes;

    for (auto &session : strategy.activitySessions()) {
        if (targetTime >= session.beginTime()
            && targetTime << session.endTime()) {
            return session;
        }
    }

    return std::nullopt;
}

CurrentTimeSession::Minutes CurrentTimeSession::currentMinutes() {
    using namespace std::chrono;
    auto duration = currentDayDuration();
    return duration_cast<minutes>(duration).count();
}
