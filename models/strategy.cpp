#include "strategy.h"

#include <algorithm>
#include <string>
#include <vector>

//
//ActivityGroupsState Strategy::calculateGroups() {
//    auto slotsState = this->slotsState();
//    ActivityGroupsState result;
//    optional <ActivityGroup> cachedActivityGroup;
//
//    for (unsigned int index = 0; index < slotsState.size(); index++) {
//        auto activity = slotsState[index];
//        auto defaultActivityGroup = ActivityGroup(slotsState[index]);
//        auto prevActivity = index > 0 ? slotsState[index - 1] : nullopt;
//
//        if (!activity) {
//            if (cachedActivityGroup) {
//                result.push_back(cachedActivityGroup.value());
//            }
//
//            result.push_back(defaultActivityGroup);
//
//            cachedActivityGroup = nullopt;
//        } else {
//            if (!prevActivity || prevActivity.value() != activity.value()) {
//                if (cachedActivityGroup) {
//                    result.push_back(cachedActivityGroup.value());
//                }
//
//                cachedActivityGroup = defaultActivityGroup;
//            } else {
//                if (cachedActivityGroup) {
//                    cachedActivityGroup.value().length++;
//                }
//            }
//
//            if (cachedActivityGroup && index == slotsState.size() - 1) {
//                result.push_back(cachedActivityGroup.value());
//            }
//        }
//    }
//
//    return result;
//}
//
//void Strategy::appendActivity(const Activity &activity) {
//    if (!hasActivity(activity)) {
//        activities.push_back(activity);
//    }
//}
//
//void Strategy::removeActivity(const Activity &activity) {
//    commitToHistory(_slotsState, activities);
//
//    activities.erase(std::remove(activities.begin(), activities.end(), activity),
//                     activities.end());
//
//    for (unsigned int i = 0; i < slotsState().size(); i++) {
//        auto slot = slotAtIndex(static_cast<int>(i));
//
//        if (slot && slot.value() == activity) {
//            _slotsState[i] = nullopt;
//        }
//    }
//}
//
//void Strategy::editActivity(const Activity &from, const Activity &to) {
//    if (hasActivity(to)) {
//        return;
//    }
//
//    auto it = std::find(activities.begin(), activities.end(), from);
//    if (it == activities.end()) {
//        return;
//    } else {
//        commitToHistory(_slotsState, activities);
//
//        auto index =
//                static_cast<unsigned int>(std::distance(activities.begin(), it));
//        activities[index] = to;
//
//        for (unsigned int i = 0; i < slotsState().size(); i++) {
//            if (slotsState()[i] && slotsState()[i].value() == from) {
//                _slotsState[i] = to;
//            }
//        }
//    }
//}
//
//void Strategy::editActivityAtIndex(int index, const Activity &to) {
//    if (hasActivity(to)) {
//        return;
//    }
//
//    auto uindex = static_cast<unsigned int>(index);
//    if (index < 0 || uindex >= activities.size()) {
//        return;
//    }
//
//    commitToHistory(_slotsState, activities);
//
//    auto oldActivity = activities[uindex];
//    activities[uindex] = to;
//    for (unsigned int i = 0; i < slotsState().size(); i++) {
//        if (slotsState()[i] && slotsState()[i].value() == oldActivity) {
//            _slotsState[i] = to;
//        }
//    }
//}
//
//optional<unsigned int>
//Strategy::indexOfActivity(const Activity &activity) const {
//    for (unsigned int i = 0; i < activities.size(); i++) {
//        if (activities[i] == activity) {
//            return i;
//        }
//    }
//
//    return nullopt;
//}
//
//Slot Strategy::slotAtIndex(int index) {
//    if (!hasSlotIndex(index)) {
//        return nullopt;
//    }
//
//    return _slotsState[static_cast<unsigned int>(index)];
//}
//
//void Strategy::setSlotAtIndex(int index, Slot slot) {
//    _slotsState[static_cast<unsigned int>(index)] = slot;
//}
//
//void Strategy::setSlotAtIndices(vector<int> indices, Slot slot) {
//    commitToHistory(_slotsState, activities);
//    for (int index : indices) {
//        setSlotAtIndex(index, slot);
//    }
//}
//
//void Strategy::copySlot(int fromIndex, int toIndex) {
//    if (!hasSlotIndex(fromIndex) || !hasSlotIndex(toIndex)) {
//        return;
//    }
//
//    _slotsState[static_cast<unsigned int>(toIndex)] =
//            _slotsState[static_cast<unsigned int>(fromIndex)];
//}
//
//void Strategy::fillSlots(int fromIndex, int toIndex) {
//    if (!hasSlotIndex(fromIndex) || !hasSlotIndex(toIndex)) {
//        return;
//    }
//
//    auto sourceIndex = fromIndex;
//
//    if (toIndex < fromIndex) {
//        auto tempIndex = fromIndex;
//        fromIndex = toIndex;
//        toIndex = tempIndex;
//    }
//
//    for (auto i = fromIndex; i <= toIndex; i++) {
//        copySlot(sourceIndex, i);
//    }
//}
//
//optional<int> Strategy::startSlotIndexForGroupIndex(int groupIndex) {
//    auto activityGroups = calculateGroups();
//    if (groupIndex < 0 ||
//        static_cast<unsigned int>(groupIndex) >= activityGroups.size()) {
//        return nullopt;
//    }
//
//    int lastSlotIndex = -1;
//    for (unsigned int i = 0; i <= static_cast<unsigned int>(groupIndex); i++) {
//        auto activityGroup = activityGroups[i];
//        auto startSlotIndex = lastSlotIndex + 1;
//        auto endSlotIndex =
//                startSlotIndex + static_cast<int>(activityGroup.length) - 1;
//
//        lastSlotIndex = endSlotIndex;
//    }
//
//    int lastSlotLength = static_cast<int>(
//            activityGroups[static_cast<unsigned int>(groupIndex)].length);
//    return lastSlotIndex - (lastSlotLength - 1);
//}
//
//optional<int> Strategy::groupIndexForSlotIndex(int slotIndex) {
//    auto activityGroups = calculateGroups();
//    int lastSlotIndex = -1;
//    for (unsigned int i = 0; i < activityGroups.size(); i++) {
//        auto activityGroup = activityGroups[i];
//        auto startSlotIndex = lastSlotIndex + 1;
//        auto endSlotIndex =
//                startSlotIndex + static_cast<int>(activityGroup.length) - 1;
//
//        if (slotIndex >= startSlotIndex && slotIndex <= endSlotIndex) {
//            return make_optional(i);
//        }
//
//        lastSlotIndex = endSlotIndex;
//    }
//
//    return nullopt;
//}
//
//optional<int> Strategy::findSlotIndexForTime(int time) {
//    auto startTimes = this->startTimes();
//    for (unsigned int i = 0; i < numberOfSlots(); i++) {
//        auto beginTime = startTimes[i];
//        auto endTime = beginTime + slotDuration();
//
//        if (time >= beginTime && time < endTime) {
//            return static_cast<int>(i);
//        }
//    }
//
//    return nullopt;
//}
//
//Strategy *Strategy::createEmpty() {
//    auto strategy = new Strategy();
//    strategy->activities = {};
//
//    SlotsState slotsState;
//    for (unsigned int i = 0; i < defaultNumberOfSlots; i++) {
//        slotsState.push_back(nullopt);
//    }
//
//    strategy->setSlotsState(slotsState);
//
//    return strategy;
//}
//
//SlotsState Strategy::slotsState() const { return _slotsState; }
//
//void Strategy::setSlotsState(const SlotsState &slotsState) {
//    _slotsState = slotsState;
//}
//
//string Strategy::debugSlots() {
//    string result = "-Slots------------------\n";
//    for (unsigned int i = 0; i < slotsState().size(); i++) {
//        auto slot = slotsState()[i];
//        if (slot) {
//            result += static_cast<string>("Slot " + to_string(i) + "\t" +
//                                          slot.value()._name + "\n");
//        } else {
//            result +=
//                    static_cast<string>("Slot " + to_string(i) + "\t" + "None" + "\n");
//        }
//        result += "---------------------------\n";
//    }
//
//    return result;
//}
//
//string Strategy::debugGroups() {
//    string result = "-Groups-------------------------\n";
//    auto groups = calculateGroups();
//    for (unsigned int i = 0; i < groups.size(); i++) {
//        auto group = groups[i];
//        if (group.activity) {
//            result += static_cast<string>("Group " + to_string(i) + "\t" +
//                                          group.activity->_name + "\t" +
//                                          group.activity->_color + "\n");
//        } else {
//            result +=
//                    static_cast<string>("Group " + to_string(i) + "\t" + "None" + "\n");
//        }
//        result += "--------------------------------\n";
//    }
//
//    return result;
//}
//
//bool Strategy::hasSlotIndex(int index) {
//    return index >= 0 && static_cast<unsigned int>(index) < slotsState().size();
//}
//
//bool Strategy::hasActivity(const Activity &activity) {
//    if (std::find(activities.begin(), activities.end(), activity) !=
//        activities.end()) {
//        return true;
//    } else {
//        return false;
//    }
//}
//
//void Strategy::commitToHistory(SlotsState state, vector <Activity> activities) {
//    undoStack.push_back(HistoryEntry(state, activities));
//    redoStack = {};
//}
//
//void Strategy::commitToHistory(Strategy::HistoryEntry entry) {
//    undoStack.push_back(entry);
//    redoStack = {};
//}
//
//unsigned int Strategy::numberOfSlots() const {
//    return static_cast<unsigned int>(_slotsState.size());
//}
//
//void Strategy::setNumberOfSlots(unsigned int numberOfSlots) {
//    if (_slotsState.size() > numberOfSlots) {
//        while (_slotsState.size() != numberOfSlots) {
//            _slotsState.pop_back();
//        }
//    } else if (_slotsState.size() < numberOfSlots) {
//        while (_slotsState.size() != numberOfSlots) {
//            _slotsState.push_back(Slot());
//        }
//    }
//}
//
//Duration Strategy::beginTime() const { return _beginTime; }
//
//void Strategy::setBeginTime(const Duration &beginTime) {
//    _beginTime = beginTime;
//}
//
//Duration Strategy::slotDuration() const { return _slotDuration; }
//
//void Strategy::setSlotDuration(const Duration &slotDuration) {
//    _slotDuration = slotDuration;
//}
//
//Duration Strategy::startTimeForSlotIndex(int index) {
//    return index * slotDuration() + beginTime();
//}
//
//Duration Strategy::startTimeForGroupIndex(int groupIndex) {
//    auto startSlotIndex = startSlotIndexForGroupIndex(groupIndex);
//    if (!startSlotIndex) {
//        return 0;
//    }
//
//    return startTimeForSlotIndex(startSlotIndex.value());
//}
//
//Duration Strategy::endTime() { return startTimes().back() + slotDuration(); }
//
//vector <Duration> Strategy::startTimes() {
//    vector <Duration> result;
//
//    for (int i = 0; i < static_cast<int>(numberOfSlots()); i++) {
//        result.push_back(startTimeForSlotIndex(i));
//    }
//
//    return result;
//}
//
//void Strategy::undo() {
//    if (undoStack.empty()) {
//        return;
//    }
//
//    auto newEntry = HistoryEntry(this);
//    redoStack.push_back(newEntry);
//
//    auto prevEntry = undoStack.back();
//
//    _slotsState = prevEntry.slotsState;
//    activities = prevEntry.activities;
//
//    undoStack.pop_back();
//}
//
//void Strategy::redo() {
//    if (redoStack.empty()) {
//        return;
//    }
//
//    undoStack.push_back(HistoryEntry(this));
//
//    auto historyEntry = redoStack.back();
//
//    _slotsState = historyEntry.slotsState;
//    activities = historyEntry.activities;
//
//    redoStack.pop_back();
//}
//
//Strategy::HistoryEntry::HistoryEntry(Strategy *strategy) {
//    this->slotsState = strategy->slotsState();
//    this->activities = strategy->activities;
//}
//
//Strategy::HistoryEntry::HistoryEntry(SlotsState slotsState,
//                                     vector <Activity> activities)
//        : slotsState(slotsState), activities(activities) {}

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

void Strategy::setBeginTime(Strategy::Time beginTime) {
    timeSlots.setBeginTime(beginTime);
}

Strategy::Duration Strategy::timeSlotDuration() const {
    return timeSlots.slotDuration();
}

void Strategy::setTimeSlotDuration(Strategy::Duration timeSlotDuration) {
    timeSlots.setSlotDuration(timeSlotDuration);
}

Strategy::Strategy(Strategy::Time beginTime,
                   Strategy::Duration timeSlotDuration,
                   Strategy::StateSize numberOfTimeSlots)
        : timeSlots(TimeSlotsState(beginTime,
                                   timeSlotDuration,
                                   numberOfTimeSlots)),
          history(makeHistoryEntry()) {
    setupTimeSlotsCallback();
}

void Strategy::timeSlotsChanged() {
    _activitySessions.recalculateForTimeSlotsState(timeSlots);
}

void Strategy::putActivityInTimeSlotsAtIndices(const Activity *activity,
                                               const std::vector<TimeSlotIndex> &timeSlotIndices) {
    timeSlots.setActivityAtIndices(activity, timeSlotIndices);
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

void Strategy::fillTimeSlots(Strategy::TimeSlotIndex fromIndex, Strategy::TimeSlotIndex tillIndex) {
    timeSlots.fillSlots(fromIndex, tillIndex);
}

Strategy::StateSize Strategy::numberOfTimeSlots() const {
    return timeSlots.numberOfSlots();
}

void Strategy::setNumberOfTimeSlots(Strategy::StateSize numberOfTimeSlots) {
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
