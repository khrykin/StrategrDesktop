//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_SESSIONSLIST_H
#define MODELS_SESSIONSLIST_H

#include <iostream>

#include "Session.h"
#include "PrivateList.hpp"
#include "NotifiableOnChange.h"
#include "TimeSlotsState.h"
#include "StreamableList.h"
#include "SessionsCalculator.h"

using ActivitySessionsListBase = PrivateList<Session>;
class Strategy;

class SessionsList :
        public ActivitySessionsListBase,
        public NotifiableOnChange,
        public StreamableList<SessionsList> {
public:
    struct OverviewItem {
        float durationPercentage = 0.0;
        float beginPercentage = 0.0;
        std::optional<Activity::Color> color = std::nullopt;
    };

    std::vector<Session> nonEmpty() const;

    std::string classPrintName() const override;

    std::optional<Index>
    sessionIndexForTimeSlotIndex(TimeSlotsState::Index timeSlotIndex) const;

    const Session *
    sessionAfter(const Session &activitySession) const;

    std::optional<Session>
    sessionBefore(const Session &activitySession) const;

    std::vector<OverviewItem> overview() const;

private:
    using ActivitySessionsListBase::ActivitySessionsListBase;

    void recalculateForTimeSlotsState(const TimeSlotsState &timeSlotsState);

    friend Strategy;
};


#endif //MODELS_SESSIONSLIST_H
