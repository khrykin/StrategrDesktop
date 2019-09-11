//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_SESSIONSLIST_H
#define MODELS_SESSIONSLIST_H

#include <iostream>

#include "activitysession.h"
#include "privatelist.hpp"
#include "notifiableonchange.h"
#include "timeslotsstate.h"
#include "streamablelist.h"
#include "activitysessionscalculator.h"

using ActivitySessionsListBase = PrivateList<ActivitySession>;
class Strategy;

class ActivitySessionsList :
        public ActivitySessionsListBase,
        public NotifiableOnChange,
        public StreamableList<ActivitySessionsList> {
public:
    struct OverviewItem {
        float durationPercentage = 0.0;
        float beginPercentage = 0.0;
        std::optional<Activity::Color> color = std::nullopt;
    };

    std::vector<ActivitySession> nonEmpty() const;

    std::string classPrintName() const override;

    std::optional<Index>
    sessionIndexForTimeSlotIndex(TimeSlotsState::Index timeSlotIndex) const;

    std::optional<ActivitySession>
    sessionAfter(const ActivitySession &activitySession) const;

    std::optional<ActivitySession>
    sessionBefore(const ActivitySession &activitySession) const;

    std::vector<OverviewItem> overview() const;

private:
    using ActivitySessionsListBase::ActivitySessionsListBase;

    void recalculateForTimeSlotsState(const TimeSlotsState &timeSlotsState);

    friend Strategy;
};


#endif //MODELS_SESSIONSLIST_H
