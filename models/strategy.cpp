#include "strategy.h"

#include <algorithm>
#include <string>
#include <vector>

const ActivityList &Strategy::activities() const {
    return _activities;
}

void Strategy::addActivity(const Activity &activity) {
    _activities.add(activity);
    commitToHistory();
}

void Strategy::removeActivityAtIndex(ActivityIndex activityIndex) {
    timeSlots.removeActivity(&_activities[activityIndex]);
    _activities.removeAtIndex(activityIndex);

    commitToHistory();
}

void Strategy::editActivityAtIndex(ActivityIndex activityIndex,
                                   const Activity &newActivity) {
    const auto oldActivity = &_activities[activityIndex];
    _activities.editAtIndex(activityIndex, newActivity);

    const auto updatedActivity = &_activities[activityIndex];
    timeSlots.editActivity(oldActivity, updatedActivity);

    commitToHistory();
}

Strategy::Time Strategy::beginTime() const {
    return timeSlots.beginTime();
}

void Strategy::setBeginTime(Time beginTime) {
    timeSlots.setBeginTime(beginTime);
}

Strategy::Duration Strategy::timeSlotDuration() const {
    return timeSlots.slotDuration();
}

void Strategy::setTimeSlotDuration(Duration timeSlotDuration) {
    timeSlots.setSlotDuration(timeSlotDuration);
}

Strategy::Strategy(Time beginTime,
                   Duration timeSlotDuration,
                   StateSize numberOfTimeSlots)
        : timeSlots(TimeSlotsState(beginTime,
                                   timeSlotDuration,
                                   numberOfTimeSlots)),
          history(makeHistoryEntry()) {
    setupTimeSlotsCallback();
}

void Strategy::timeSlotsChanged() {
    _activitySessions.recalculateForTimeSlotsState(timeSlots);
}

void Strategy::putActivityInTimeSlotsAtIndices(ActivityIndex activityIndex,
                                               const std::vector<TimeSlotIndex> &timeSlotIndices) {
    if (!activities().hasIndex(activityIndex)) {
        return;
    }

    auto activity = activities().at(activityIndex);
    timeSlots.setActivityAtIndices(activity, timeSlotIndices);

    commitToHistory();
}

void Strategy::emptyTimeSlotsAtIndices(
        const std::vector<TimeSlotIndex> &timeSlotIndices) {
    timeSlots.setActivityAtIndices(Strategy::NoActivity, timeSlotIndices);

    commitToHistory();
}

void Strategy::dragActivity(ActivityIndex fromIndex, ActivityIndex toIndex) {
    _activities.drag(fromIndex, toIndex);
}

Strategy::Strategy(const std::vector<TimeSlot> &timeSlotsVector,
                   const std::vector<Activity> &activitiesVector) :
        timeSlots(TimeSlotsState(timeSlotsVector)),
        _activities(ActivityList(activitiesVector)),
        history(makeHistoryEntry()) {
    setupTimeSlotsCallback();
}

void Strategy::fillTimeSlots(TimeSlotIndex fromIndex,
                             TimeSlotIndex tillIndex) {
    timeSlots.fillSlots(fromIndex, tillIndex);
    // NB! We don't want to commit to history here,
    // because user may continue to drag session to next slot index!
    // So we'll commit from UI side.
}

Strategy::StateSize Strategy::numberOfTimeSlots() const {
    return timeSlots.numberOfSlots();
}

void Strategy::setNumberOfTimeSlots(StateSize numberOfTimeSlots) {
    timeSlots.setNumberOfSlots(numberOfTimeSlots);
}

const ActivitySessionsList &Strategy::activitySessions() const {
    return _activitySessions;
}

Strategy::Time Strategy::endTime() const {
    return timeSlots.last().endTime();
}

void Strategy::setupTimeSlotsCallback() {
    timeSlotsChanged();
    timeSlots.setOnChangeCallback(this, &Strategy::timeSlotsChanged);
}

StrategyHistory::Entry Strategy::makeHistoryEntry() {
    return StrategyHistory::Entry{_activities, timeSlots};
}

void Strategy::commitToHistory() {
    history.commit(makeHistoryEntry());
}

void Strategy::undo() {
    auto historyEntry = history.undo();
    if (historyEntry) {
        _activities = historyEntry->activities;
        timeSlots = historyEntry->timeSlots;
    }
}

void Strategy::redo() {
    auto historyEntry = history.redo();
    if (historyEntry) {
        _activities = historyEntry->activities;
        timeSlots = historyEntry->timeSlots;
    }
}