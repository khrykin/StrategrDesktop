#include <algorithm>
#include <vector>
#include <string>
#include "strategy.h"

#include <QDebug>

ActivityGroupsState Strategy::group()
{
    auto slotsState = this->slotsState();
    ActivityGroupsState result;
    optional<ActivityGroup> cachedActivityGroup;

    for (unsigned int index = 0; index < slotsState.size(); index++) {
        auto activity = slotsState[index];
        auto defaultActivityGroup = ActivityGroup(slotsState[index]);
        auto prevActivity = index > 0
                ? slotsState[index - 1]
                : nullopt;

        if (!activity.has_value()) {
            if (cachedActivityGroup.has_value()) {
                result.push_back(cachedActivityGroup.value());
            }

            result.push_back(defaultActivityGroup);

            cachedActivityGroup = nullopt;
        } else {
            if (!prevActivity.has_value() || prevActivity.value() != activity.value()) {
                if (cachedActivityGroup.has_value()) {
                    result.push_back(cachedActivityGroup.value());
                }

                cachedActivityGroup = defaultActivityGroup;
            } else {
                if (cachedActivityGroup.has_value()) {
                    cachedActivityGroup.value().length++;
                }
            }

            if (cachedActivityGroup.has_value() && index == slotsState.size() - 1) {
                result.push_back(cachedActivityGroup.value());
            }
        }
    }

    return result;
}

void Strategy::appendActivity(const Activity activity)
{
    activities.push_back(activity);
}

void Strategy::removeActivity(const Activity &activity)
{
    commitToHistory(_slotsState, activities);

    activities.erase(std::remove(activities.begin(), activities.end(), activity), activities.end());
    for (unsigned int i = 0; i < slotsState().size(); i++) {
        auto slot = slotAtIndex(static_cast<int>(i));

        if (slot.has_value() && slot.value() == activity) {
            _slotsState[i] = nullopt;
        }
    }
}

void Strategy::editActivity(const Activity &from, const Activity &to)
{
    auto it = std::find(activities.begin(), activities.end(), from);
    if (it == activities.end()) {
        return;
    } else {
        commitToHistory(_slotsState, activities);

        auto index = static_cast<unsigned int>(std::distance(activities.begin(), it));
        activities[index] = to;

        for (unsigned int i = 0; i < slotsState().size(); i++) {
            if (slotsState()[i].has_value() && slotsState()[i].value() == from) {
                _slotsState[i] = to;
            }
        }
    }
}

Slot Strategy::slotAtIndex(int index)
{
    if (!hasSlotIndex(index)) {
        return nullopt;
    }

    return _slotsState[static_cast<unsigned int>(index)];
}

void Strategy::setSlotAtIndex(int index, Slot slot)
{
    _slotsState[static_cast<unsigned int>(index)] = slot;
}

void Strategy::setSlotAtIndices(vector<int> indices, Slot slot)
{
    commitToHistory(_slotsState, activities);
    for (int index : indices) {
        setSlotAtIndex(index, slot);
    }
}

void Strategy::copySlot(int fromIndex, int toIndex)
{
    if (!hasSlotIndex(fromIndex) || !hasSlotIndex(toIndex)) {
        return;
    }

    _slotsState[static_cast<unsigned int>(toIndex)] = _slotsState[static_cast<unsigned int>(fromIndex)];
}

void Strategy::fillSlots(int fromIndex, int toIndex)
{
    if (!hasSlotIndex(fromIndex) || !hasSlotIndex(toIndex)) {
        return;
    }

    auto sourceIndex = fromIndex;

    if (toIndex < fromIndex) {
        auto tempIndex = fromIndex;
        fromIndex = toIndex;
        toIndex = tempIndex;
    }

    for (auto i = fromIndex; i <= toIndex; i++) {
        copySlot(sourceIndex, i);
    }
}

optional<int> Strategy::startSlotIndexForGroupIndex(int groupIndex)
{
    auto activityGroups = group();
    if (groupIndex < 0 || static_cast<unsigned int>(groupIndex) >= activityGroups.size()) {
        return nullopt;
    }

    int lastSlotIndex = -1;
    for (unsigned int i = 0; i <= static_cast<unsigned int>(groupIndex); i++) {
        auto activityGroup = activityGroups[i];
        auto startSlotIndex = lastSlotIndex + 1;
        auto endSlotIndex  = startSlotIndex + static_cast<int>(activityGroup.length) - 1;

        lastSlotIndex = endSlotIndex;
    }

    int lastSlotLength = static_cast<int>(activityGroups[static_cast<unsigned int>(groupIndex)].length);
    return lastSlotIndex - (lastSlotLength - 1);
}

optional<int> Strategy::groupIndexForSlotIndex(int slotIndex)
{
    auto activityGroups = group();
    int lastSlotIndex = -1;
    for (unsigned int i = 0; i < activityGroups.size(); i++) {
        auto activityGroup = activityGroups[i];
        auto startSlotIndex = lastSlotIndex + 1;
        auto endSlotIndex  = startSlotIndex + static_cast<int>(activityGroup.length) - 1;

        if (slotIndex >= startSlotIndex && slotIndex <= endSlotIndex) {
            return make_optional(i);
        }

        lastSlotIndex = endSlotIndex;
    }

    return nullopt;
}

Strategy *Strategy::createEmtpty()
{
    auto strategy = new Strategy();
    strategy->activities = {
        Activity("Training"),
        Activity("Work 1"),
        Activity("Nap"),
        Activity("Commute")
    };

    SlotsState slotsState;

    for (unsigned int i = 0; i < strategy->_numberOfSlots; i++) {
        if (i < strategy->_numberOfSlots / 4) {
            slotsState.push_back(strategy->activities[0]);
        } else if (i < 2 * strategy->_numberOfSlots / 4) {
            slotsState.push_back(strategy->activities[1]);
        } else if (i < 3 * strategy->_numberOfSlots / 4) {
            slotsState.push_back(strategy->activities[2]);
        } else {
            slotsState.push_back(nullopt);
        }
    }

    strategy->setSlotsState(slotsState);

    return strategy;
}

SlotsState Strategy::slotsState() const
{
    return _slotsState;
}

void Strategy::setSlotsState(const SlotsState &slotsState)
{
    _slotsState = slotsState;
}

string Strategy::debugSlots()
{
    string result = "-Slots------------------\n";
    for (unsigned int i = 0; i < slotsState().size(); i++) {
        auto slot = slotsState()[i];
        if (slot.has_value()) {
            result += static_cast<string>("Slot " + to_string(i) + "\t" + slot.value().name + "\n");
        } else {
            result += static_cast<string>("Slot " + to_string(i) + "\t" + "None" + "\n");
        }
        result += "---------------------------\n";
    }

    return result;
}

string Strategy::debugGroups()
{
    string result = "-Groups--------------------\n";
    auto groups = group();
    for (unsigned int i = 0; i < groups.size(); i++) {
        auto group = groups[i];
        if (group.activity.has_value()) {
            result += static_cast<string>("Group " + to_string(i) + "\t" + group.activity.value().name + "\n");
        } else {
            result += static_cast<string>("Group " + to_string(i) + "\t" + "None" + "\n");
        }
        result += "---------------------------\n";
    }

    return result;
}

bool Strategy::hasSlotIndex(int index)
{
    return  index >= 0 && static_cast<unsigned int>(index) < slotsState().size();
}

bool Strategy::hasActivity(const Activity &activity)
{
    if (std::find(activities.begin(), activities.end(), activity) != activities.end()) {
        return true;
    } else {
        return false;
    }
}

void Strategy::commitToHistory(SlotsState state, vector<Activity> activities)
{
    undoStack.push_back(HistoryEntry(state, activities));
    redoStack = {};
}

void Strategy::commitToHistory(Strategy::HistoryEntry entry)
{
    undoStack.push_back(entry);
    redoStack = {};
}

unsigned int Strategy::numberOfSlots() const
{
    return _numberOfSlots;
}

void Strategy::setNumberOfSlots(unsigned int numberOfSlots)
{
    _numberOfSlots = numberOfSlots;
}

Duration Strategy::startTime() const
{
    return _startTime;
}

void Strategy::setStartTime(const Duration &startTime)
{
    _startTime = startTime;
}

Duration Strategy::slotDuration() const
{
    return _slotDuration;
}

void Strategy::setSlotDuration(const Duration &slotDuration)
{
    _slotDuration = slotDuration;
}

Duration Strategy::startTimeForSlotIndex(int index)
{
    return index * slotDuration() + startTime();
}

Duration Strategy::endTime()
{
    return startTimes().back() + slotDuration();
}

vector<Duration> Strategy::startTimes()
{
    vector<Duration> result;

    for (int i = 0; i < static_cast<int>(numberOfSlots()); i++) {
        result.push_back(startTimeForSlotIndex(i));
    }

    return result;
}

void Strategy::undo()
{
    if (undoStack.empty()) {
        return;
    }
    
    auto newEntry = HistoryEntry(this);
    redoStack.push_back(newEntry);
    
    auto prevEntry = undoStack.back();

    _slotsState = prevEntry.slotsState;
    activities = prevEntry.activities;

    undoStack.pop_back();
}

void Strategy::redo()
{
    if (redoStack.empty()) {
        return;
    }

    undoStack.push_back(HistoryEntry(this));

    auto historyEntry = redoStack.back();

    _slotsState = historyEntry.slotsState;
    activities = historyEntry.activities;

    redoStack.pop_back();
}


Strategy::HistoryEntry::HistoryEntry(Strategy *strategy)
{
    this->slotsState = strategy->slotsState();
    this->activities = strategy->activities;
}

Strategy::HistoryEntry::HistoryEntry(SlotsState slotsState, vector<Activity> activities): slotsState(slotsState), activities(activities)
{

}
