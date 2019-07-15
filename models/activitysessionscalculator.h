//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_ACTIVITYSESSIONSCALCULATOR_H
#define MODELS_ACTIVITYSESSIONSCALCULATOR_H

#include "activitysession.h"
#include "timeslotsstate.h"

class ActivitySessionsCalculator {
public:
    using ResultType = std::vector<ActivitySession>;

    static constexpr auto NoActivity = TimeSlot::NoActivity;

    static ResultType calculateForTimeSlotsState(
            const TimeSlotsState &timeSlotsState);

private:
    using TimeSlotIndex = TimeSlotsState::Index;

    const TimeSlotsState &timeSlotsState;
    TimeSlotIndex timeSlotIndex = 0;

    explicit ActivitySessionsCalculator(const TimeSlotsState &timeSlotsState);

    std::optional<ActivitySession> cachedSession;

    ResultType calculate();

    void processSlotWithActivity(ResultType &result);

    void processEmptySlot(ResultType &result);

    ActivitySession defaultSession();

    const Activity *currentActivity();

    const Activity *previousActivity();

    const TimeSlot *currentTimeSlot();

    bool currentSlotIsLast();

    bool currentSlotHasActivity();

    void pushCachedSessionIfExists(ResultType &result);

    void resetCacheWithDefault();

    void pushCurrentSlotToCache();

    void pushDefaultSession(ResultType &result);

    bool activityChanged();

};

#endif //MODELS_ACTIVITYSESSIONSCALCULATOR_H
