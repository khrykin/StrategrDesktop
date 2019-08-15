//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include "currentactivitysession.h"
#include "strategy.h"
#include "currenttime.h"

std::optional<ActivitySession>
CurrentActivitySession::forStrategy(const Strategy &strategy) {
    auto currentSession = currentSessionForStrategy(strategy);
    return currentSession && currentSession->activity
           ? currentSession
           : std::nullopt;
}

std::optional<ActivitySession>
CurrentActivitySession::currentSessionForStrategy(const Strategy &strategy) {
    auto currentMinutes = CurrentTime::currentMinutes();
    for (auto &session : strategy.activitySessions()) {
        if (currentMinutes >= session.beginTime()
            && currentMinutes <= session.endTime()) {
            return session;
        }
    }

    return std::nullopt;
}

std::optional<ActivitySession>
CurrentActivitySession::upcomingForStrategy(const Strategy &strategy) {
    auto currentSession =
            CurrentActivitySession::currentSessionForStrategy(strategy);

    if (!currentSession) {
        // We're out of strategy's time bounds.
        return std::nullopt;
    }

    auto nextSession = strategy.activitySessions().sessionAfter(*currentSession);
    return nextSession && nextSession->activity
           ? nextSession
           : std::nullopt;
}
