//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include "activitysessionscalculator.h"


ActivitySessionsCalculator::ResultType
ActivitySessionsCalculator::
calculateForTimeSlotsState(const TimeSlotsState &timeSlotsState) {
    return ActivitySessionsCalculator(timeSlotsState).calculate();
}

ActivitySessionsCalculator::
ActivitySessionsCalculator(const TimeSlotsState &timeSlotsState)
        : timeSlotsState(timeSlotsState) {}

ActivitySessionsCalculator::ResultType
ActivitySessionsCalculator::calculate() {
    ResultType result;

    for (timeSlotIndex = 0;
         timeSlotIndex < timeSlotsState.numberOfSlots();
         timeSlotIndex++) {
        if (currentSlotHasActivity()) {
            processSlotWithActivity(result);
        } else {
            processEmptySlot(result);
        }
    }

    return result;
}

void ActivitySessionsCalculator::
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

void ActivitySessionsCalculator::
processEmptySlot(ResultType &result) {
    pushCachedSessionIfExists(result);
    pushDefaultSession(result);

    cachedSession = std::nullopt;
}

ActivitySession ActivitySessionsCalculator::defaultSession() {
    const auto &timeSlot = timeSlotsState[timeSlotIndex];

    auto defaultActivitySession = ActivitySession();
    defaultActivitySession.activity = timeSlot.activity;
    defaultActivitySession.timeSlots.push_back(&timeSlot);

    return defaultActivitySession;
}

const Activity *ActivitySessionsCalculator::currentActivity() {
    return currentTimeSlot()->activity;
}

const Activity *ActivitySessionsCalculator::previousActivity() {
    return timeSlotIndex > 0
           ? timeSlotsState[timeSlotIndex - 1].activity
           : NoActivity;
}

const TimeSlot *ActivitySessionsCalculator::currentTimeSlot() {
    return &timeSlotsState[timeSlotIndex];
}

bool ActivitySessionsCalculator::currentSlotIsLast() {
    return timeSlotIndex == timeSlotsState.numberOfSlots() - 1;
}

bool ActivitySessionsCalculator::currentSlotHasActivity() {
    return currentActivity() != NoActivity;
}

void ActivitySessionsCalculator::pushCachedSessionIfExists(ResultType &result) {
    if (cachedSession) {
        result.push_back(*cachedSession);
    }
}

void ActivitySessionsCalculator::resetCacheWithDefault() {
    cachedSession = defaultSession();
}

void ActivitySessionsCalculator::pushCurrentSlotToCache() {
    if (cachedSession) {
        cachedSession->timeSlots.push_back(currentTimeSlot());
    }
}

void ActivitySessionsCalculator::pushDefaultSession(ResultType &result) {
    result.push_back(defaultSession());
}

bool ActivitySessionsCalculator::activityChanged() {
    return !previousActivity() || previousActivity() != currentActivity();
}
