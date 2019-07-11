#include "strategy.h"

#include <algorithm>
#include <string>
#include <vector>

Strategy::Strategy(Time beginTime,
                   Duration timeSlotDuration,
                   StateSize numberOfTimeSlots)
        : _timeSlots(TimeSlotsState(beginTime,
                                    timeSlotDuration,
                                    numberOfTimeSlots)),
          history(makeHistoryEntry()) {
    setupTimeSlotsCallback();
}

Strategy::Strategy(const TimeSlotsState &timeSlots,
                   const ActivityList &activities) :
        _timeSlots(timeSlots),
        _activities(activities),
        history(makeHistoryEntry()) {
    setupTimeSlotsCallback();
}

const ActivityList &Strategy::activities() const {
    return _activities;
}

void Strategy::addActivity(const Activity &activity) {
    _activities.add(activity);
    commitToHistory();
}

void Strategy::removeActivityAtIndex(ActivityIndex activityIndex) {
    _timeSlots.removeActivity(_activities.at(activityIndex));
    _activities.removeAtIndex(activityIndex);

    commitToHistory();
}

void Strategy::editActivityAtIndex(ActivityIndex activityIndex,
                                   const Activity &newActivity) {
    const auto oldActivity = _activities.at(activityIndex);
    _activities.editAtIndex(activityIndex, newActivity);

    const auto updatedActivity = _activities.at(activityIndex);
    _timeSlots.editActivity(oldActivity, updatedActivity);

    commitToHistory();
}

Strategy::Time Strategy::beginTime() const {
    return _timeSlots.beginTime();
}

void Strategy::setBeginTime(Time beginTime) {
    _timeSlots.setBeginTime(beginTime);
}

Strategy::Duration Strategy::timeSlotDuration() const {
    return _timeSlots.slotDuration();
}

void Strategy::setTimeSlotDuration(Duration timeSlotDuration) {
    _timeSlots.setSlotDuration(timeSlotDuration);
}


void Strategy::timeSlotsChanged() {
    _activitySessions.recalculateForTimeSlotsState(_timeSlots);
}

void Strategy::putActivityInTimeSlotsAtIndices(ActivityIndex activityIndex,
                                               const std::vector<TimeSlotIndex> &timeSlotIndices) {
    if (!activities().hasIndex(activityIndex)) {
        return;
    }

    auto activity = activities().at(activityIndex);
    _timeSlots.setActivityAtIndices(activity, timeSlotIndices);

    commitToHistory();
}

void Strategy::emptyTimeSlotsAtIndices(
        const std::vector<TimeSlotIndex> &timeSlotIndices) {
    _timeSlots.setActivityAtIndices(Strategy::NoActivity, timeSlotIndices);

    commitToHistory();
}

void Strategy::dragActivity(ActivityIndex fromIndex, ActivityIndex toIndex) {
    _activities.drag(fromIndex, toIndex);
}

void Strategy::fillTimeSlots(TimeSlotIndex fromIndex,
                             TimeSlotIndex tillIndex) {
    _timeSlots.fillSlots(fromIndex, tillIndex);
    // NB! We don't want to commit to history here,
    // because user may continue to drag session to next slot index!
    // So we'll commit from UI side.
}

Strategy::StateSize Strategy::numberOfTimeSlots() const {
    return _timeSlots.numberOfSlots();
}

void Strategy::setNumberOfTimeSlots(StateSize numberOfTimeSlots) {
    _timeSlots.setNumberOfSlots(numberOfTimeSlots);
}

const ActivitySessionsList &Strategy::activitySessions() const {
    return _activitySessions;
}

Strategy::Time Strategy::endTime() const {
    return _timeSlots.last().endTime();
}

void Strategy::setupTimeSlotsCallback() {
    timeSlotsChanged();
    _timeSlots.setOnChangeCallback(this, &Strategy::timeSlotsChanged);
}

StrategyHistory::Entry Strategy::makeHistoryEntry() {
    return StrategyHistory::Entry{_activities, _timeSlots};
}

void Strategy::commitToHistory() {
    history.commit(makeHistoryEntry());
    onChangeEvent();
}

void Strategy::undo() {
    auto historyEntry = history.undo();
    if (historyEntry) {
        _activities = historyEntry->activities;
        _timeSlots = historyEntry->timeSlots;
    }

    onChangeEvent();
}

void Strategy::redo() {
    auto historyEntry = history.redo();
    if (historyEntry) {
        _activities = historyEntry->activities;
        _timeSlots = historyEntry->timeSlots;
    }

    onChangeEvent();
}

const TimeSlotsState &Strategy::timeSlots() const {
    return _timeSlots;
}
