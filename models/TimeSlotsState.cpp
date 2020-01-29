//
// Created by Dmitry Khrykin on 2019-07-05.
//
#include <algorithm>

#include "TimeSlotsState.h"

TimeSlotsState::Time TimeSlotsState::beginTime() const {
    return _beginTime;
}

void TimeSlotsState::setBeginTime(Time beginTime) {
    _beginTime = beginTime;

    updateBeginTimes();

    onChangeEvent();
}

void TimeSlotsState::updateBeginTimes() {
    for (auto slotIndex = 0;
         slotIndex < numberOfSlots();
         slotIndex++) {
        _data[slotIndex].beginTime
                = slotBeginTime(_beginTime, slotIndex);
    }
}

TimeSlotsState::Duration TimeSlotsState::slotDuration() const {
    return _slotDuration;
}

void TimeSlotsState::setSlotDuration(Duration slotDuration) {
    _slotDuration = slotDuration;

    for (auto slotIndex = 0; slotIndex < numberOfSlots(); slotIndex++) {
        auto &slot = _data[slotIndex];
        slot.duration = slotDuration;
        slot.beginTime = slotBeginTime(beginTime(), slotIndex);
    }

    onChangeEvent();
}

TimeSlotsState::StateSize TimeSlotsState::numberOfSlots() const {
    return _data.size();
}

void TimeSlotsState::setNumberOfSlots(StateSize newNumberOfSlots) {
    if (newNumberOfSlots == numberOfSlots()) {
        return;
    }

    if (newNumberOfSlots < numberOfSlots()) {
        _data.erase(_data.begin() + newNumberOfSlots,
                    _data.end());
    } else {
        populateVector(last().endTime(),
                       newNumberOfSlots - numberOfSlots());
    }

    onChangeEvent();
}

TimeSlotsState::TimeSlotsState(Time startTime,
                               Duration slotDuration,
                               StateSize numberOfSlots)
        : _beginTime(startTime),
          _slotDuration(slotDuration) {
    populateVector(startTime, numberOfSlots);
}

TimeSlotsState::TimeSlotsState(std::vector<TimeSlot> fromVector) {
    auto firstSlot = fromVector.front();

    _beginTime = firstSlot.beginTime;
    _slotDuration = firstSlot.duration;

    _data = std::move(fromVector);
}

void TimeSlotsState::fillSlots(Index fromIndex,
                               Index tillIndex) {
    if (fromIndex == tillIndex || !hasIndices(fromIndex, tillIndex)) {
        return;
    }

    auto sourceIndex = fromIndex;

    if (tillIndex < fromIndex) {
        std::swap(tillIndex, fromIndex);
    }

    for (auto copyIndex = fromIndex; copyIndex <= tillIndex; copyIndex++) {
        _data[copyIndex].activity = _data[sourceIndex].activity;
    }

    onChangeEvent();
}

void TimeSlotsState::silentlySetActivityAtIndex(Activity *activity,
                                                Index slotIndex) {
    if (!hasIndex(slotIndex)) {
        return;
    }

    _data[slotIndex].activity = activity;
}

void TimeSlotsState::setActivityAtIndices(Activity *activity,
                                          const std::vector<Index> &indices) {
    auto activityChanged = false;
    for (auto slotIndex : indices) {
        auto currentActivityChanged = _data[slotIndex].activity != activity;
        if (currentActivityChanged) {
            silentlySetActivityAtIndex(activity, slotIndex);
        }

        if (!activityChanged && currentActivityChanged) {
            activityChanged = true;
        }
    }

    if (activityChanged) {
        onChangeEvent();
    }
}


void TimeSlotsState::silentlySetActivityAtIndices(Activity *activity,
                                                  const std::vector<Index> &indices) {
    for (auto slotIndex : indices) {
        silentlySetActivityAtIndex(activity, slotIndex);
    }
}


std::string TimeSlotsState::classPrintName() const {
    return "TimeSlotsState";
}

void TimeSlotsState::populateVector(Time startTime,
                                    StateSize numberOfSlots) {
    for (auto slotIndex = 0; slotIndex < numberOfSlots; ++slotIndex) {
        auto beginTime = slotBeginTime(startTime, slotIndex);
        auto timeSlot = TimeSlot(beginTime, _slotDuration);

        _data.emplace_back(timeSlot);
    }
}

TimeSlotsState::Time
TimeSlotsState::slotBeginTime(Time globalBeginTime,
                              Index slotIndex) {
    return globalBeginTime + slotIndex * _slotDuration;
}

bool TimeSlotsState::hasActivity(const Activity *activity) {
    return findSlotWithActivity(activity) != _data.end();
}

TimeSlotsState::iterator
TimeSlotsState::findSlotWithActivity(const Activity *activity) {
    return std::find_if(_data.begin(), _data.end(),
                        [activity](const auto &timeSlot) {
                            return timeSlot.activity == activity;
                        });
}

void TimeSlotsState::removeActivity(Activity *activity) {
    editActivity(activity, TimeSlot::NoActivity);
}

void TimeSlotsState::editActivity(Activity *oldActivity,
                                  Activity *newActivity) {
    auto activityChanged = false;
    for (auto &timeSlot : _data) {
        if (timeSlot.activity == oldActivity) {
            activityChanged = oldActivity != newActivity;
            timeSlot.activity = newActivity;
        }
    }

    if (activityChanged) {
        onChangeEvent();
    }
}

TimeSlotsState &TimeSlotsState::operator=(const TimeSlotsState &newState) {
    _data = newState._data;
    _beginTime = newState._beginTime;
    _slotDuration = newState._slotDuration;

    onChangeEvent();
    return *this;
}

void TimeSlotsState::shiftBelow(PrivateList::Index fromIndex,
                                TimeSlotsState::StateSize length) {
    auto actualNumberOfSlots = numberOfSlots();
    _data.insert(_data.begin() + fromIndex,
                 length,
                 TimeSlot(0, slotDuration()));

    _data.resize(actualNumberOfSlots, TimeSlot(0, 0));

    updateBeginTimes();

    onChangeEvent();
}

//TimeSlotsState::Index TimeSlotsState::indexOf(const TimeSlot *slot) const {
//    return slot - &_vector[0];
//}

void TimeSlotsState::swap(Index firstIndex,
                          Index secondIndex) {

    silentlySwap(firstIndex, secondIndex);
    onChangeEvent();
}

void TimeSlotsState::silentlySwap(Index firstIndex,
                                  Index secondIndex) {
    auto activity = _data[firstIndex].activity;
    _data[firstIndex].activity = _data[secondIndex].activity;
    _data[secondIndex].activity = activity;
}

const TimeSlot &TimeSlotsState::at(TimeSlotsState::Index index) {
    return _data.at(index);
}


