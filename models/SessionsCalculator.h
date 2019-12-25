//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_ACTIVITYSESSIONSCALCULATOR_H
#define MODELS_ACTIVITYSESSIONSCALCULATOR_H

#include "Session.h"
#include "TimeSlotsState.h"

class SessionsCalculator {
public:
    using ResultType = std::vector<Session>;

    static constexpr auto NoActivity = TimeSlot::NoActivity;

    static ResultType calculateForTimeSlotsState(
            const TimeSlotsState &timeSlotsState);

private:
    using TimeSlotIndex = TimeSlotsState::Index;

    const TimeSlotsState &timeSlotsState;
    TimeSlotIndex timeSlotIndex = 0;

    explicit SessionsCalculator(const TimeSlotsState &timeSlotsState);

    std::optional<Session> cachedSession;

    ResultType calculate();

    void processSlotWithActivity(ResultType &result);
    void processEmptySlot(ResultType &result);

    Session defaultSession();

    const Activity *currentActivity();
    const Activity *previousActivity();

    const TimeSlot &currentTimeSlot();

    bool currentSlotIsLast();
    bool currentSlotHasActivity();
    bool activityChanged();

    void pushCachedSessionIfExists(ResultType &result);
    void pushCurrentSlotToCache();
    void pushDefaultSession(ResultType &result);

    void resetCacheWithDefault();
};

#endif //MODELS_ACTIVITYSESSIONSCALCULATOR_H
