//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include "currentactivitysession.h"
#include "strategy.h"

CurrentActivitySession::Timestamp
CurrentActivitySession::startOfADayFromTimestamp(CurrentActivitySession::Timestamp timestamp) {
    auto dayComponents = *dayComponentsFromTimestamp(&timestamp);

    dayComponents.tm_hour = 0;
    dayComponents.tm_min = 0;
    dayComponents.tm_sec = 0;

    timestamp = timestampFromDayComponents(&dayComponents);

    return timestamp;
}

CurrentActivitySession::Duration CurrentActivitySession::currentDayDuration() {
    using namespace std::chrono;

    auto clockNow = system_clock::now();
    auto currentTimestamp = system_clock::to_time_t(clockNow);

    auto startOfADayTimestamp = startOfADayFromTimestamp(currentTimestamp);
    auto clockStartOfToday = system_clock::from_time_t(startOfADayTimestamp);

    return clockNow - clockStartOfToday;
}

std::optional<ActivitySession>
CurrentActivitySession::forStrategy(const Strategy &strategy) {
    auto currentMinutes = CurrentActivitySession::currentMinutes();
    for (auto &session : strategy.activitySessions()) {
        if (currentMinutes >= session.beginTime()
            && currentMinutes <= session.endTime()) {
            return session.activity
                   ? std::make_optional(session)
                   : std::nullopt;
        }
    }

    return std::nullopt;
}

std::optional<ActivitySession>
CurrentActivitySession::upcomingForStrategy(const Strategy &strategy,
                                            Minutes inMinutes) {
    auto currentMinutes = CurrentActivitySession::currentMinutes();
    auto targetTime = currentMinutes + inMinutes;

    for (auto &session : strategy.activitySessions()) {
        if (targetTime >= session.beginTime()
            && targetTime << session.endTime()) {
            return session.activity
                   ? std::make_optional(session)
                   : std::nullopt;
        }
    }

    return std::nullopt;
}

CurrentActivitySession::Minutes CurrentActivitySession::currentMinutes() {
    using namespace std::chrono;
    auto duration = currentDayDuration();
    return duration_cast<minutes>(duration).count();
}
