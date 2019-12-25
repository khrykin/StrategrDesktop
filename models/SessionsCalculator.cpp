//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include "SessionsCalculator.h"


SessionsCalculator::ResultType
SessionsCalculator::
calculateForTimeSlotsState(const TimeSlotsState &timeSlotsState) {
    return SessionsCalculator(timeSlotsState).calculate();
}

SessionsCalculator::
SessionsCalculator(const TimeSlotsState &timeSlotsState)
        : timeSlotsState(timeSlotsState) {}

SessionsCalculator::ResultType
SessionsCalculator::calculate() {
    ResultType result;

    auto numberOfSlots = timeSlotsState.numberOfSlots();
    for (timeSlotIndex = 0; timeSlotIndex < numberOfSlots; timeSlotIndex++) {
        if (currentSlotHasActivity()) {
            processSlotWithActivity(result);
        } else {
            processEmptySlot(result);
        }
    }

    return result;
}

void SessionsCalculator::
processSlotWithActivity(ResultType &result) {
    if (activityChanged()) {
        pushCachedSessionIfExists(result);
        resetCacheWithDefault();
    } else {
        pushCurrentSlotToCache();
    }

    if (currentSlotIsLast()) {
        pushCachedSessionIfExists(result);
    }
}

void SessionsCalculator::
processEmptySlot(ResultType &result) {
    pushCachedSessionIfExists(result);
    pushDefaultSession(result);

    cachedSession = std::nullopt;
}

Session SessionsCalculator::defaultSession() {
    auto &timeSlot = timeSlotsState[timeSlotIndex];

    auto defaultActivitySession = Session();
    defaultActivitySession.activity = timeSlot.activity;
    defaultActivitySession.timeSlots.push_back(timeSlot);

    return defaultActivitySession;
}

const Activity *SessionsCalculator::currentActivity() {
    return currentTimeSlot().activity;
}

const Activity *SessionsCalculator::previousActivity() {
    return timeSlotIndex > 0
           ? timeSlotsState[timeSlotIndex - 1].activity
           : NoActivity;
}

const TimeSlot &SessionsCalculator::currentTimeSlot() {
    return timeSlotsState[timeSlotIndex];
}

bool SessionsCalculator::currentSlotIsLast() {
    return timeSlotIndex == timeSlotsState.numberOfSlots() - 1;
}

bool SessionsCalculator::currentSlotHasActivity() {
    return currentActivity() != NoActivity;
}

void SessionsCalculator::pushCachedSessionIfExists(ResultType &result) {
    if (cachedSession) {
        result.push_back(*cachedSession);
    }
}

void SessionsCalculator::resetCacheWithDefault() {
    cachedSession = defaultSession();
}

void SessionsCalculator::pushCurrentSlotToCache() {
    if (cachedSession) {
        cachedSession->timeSlots.push_back(currentTimeSlot());
    }
}

void SessionsCalculator::pushDefaultSession(ResultType &result) {
    result.push_back(defaultSession());
}

bool SessionsCalculator::activityChanged() {
    return !previousActivity() || previousActivity() != currentActivity();
}
