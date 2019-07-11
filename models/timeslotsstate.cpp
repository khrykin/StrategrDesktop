//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include "timeslotsstate.h"

TimeSlotsState::Time TimeSlotsState::beginTime() const {
    return _beginTime;
}

void TimeSlotsState::setBeginTime(Time beginTime) {
    _beginTime = beginTime;

    for (auto slotIndex = 0;
         slotIndex < numberOfSlots();
         slotIndex++) {
        _vector[slotIndex].beginTime
                = slotBeginTime(beginTime, slotIndex);
    }

    onChangeEvent();
}

TimeSlotsState::Duration TimeSlotsState::slotDuration() const {
    return _slotDuration;
}

void TimeSlotsState::setSlotDuration(Duration slotDuration) {
    _slotDuration = slotDuration;

    for (auto slotIndex = 0; slotIndex < numberOfSlots(); slotIndex++) {
        auto &slot = _vector[slotIndex];
        slot.duration = slotDuration;
        slot.beginTime = slotBeginTime(beginTime(), slotIndex);
    }

    onChangeEvent();
}

TimeSlotsState::StateSize TimeSlotsState::numberOfSlots() const {
    return _vector.size();
}

void TimeSlotsState::setNumberOfSlots(StateSize newNumberOfSlots) {
    if (newNumberOfSlots == numberOfSlots()) {
        return;
    }

    if (newNumberOfSlots < numberOfSlots()) {
        _vector.erase(_vector.begin() + newNumberOfSlots,
                      _vector.end());
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

    _vector = std::move(fromVector);
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
        _vector[copyIndex].activity = _vector[sourceIndex].activity;
    }

    onChangeEvent();
}

void TimeSlotsState::setActivityAtIndex(Activity *activity,
                                        Index slotIndex) {
    if (!hasIndex(slotIndex)) {
        return;
    }

    _vector[slotIndex].activity = activity;
}

void TimeSlotsState::setActivityAtIndices(Activity *activity,
                                          const std::vector<Index> &indices) {
    auto activityChanged = false;
    for (auto slotIndex : indices) {
        activityChanged = _vector[slotIndex].activity != activity;
        if (activityChanged) {
            setActivityAtIndex(activity, slotIndex);
        }
    }

    if (activityChanged) {
        onChangeEvent();
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

        _vector.emplace_back(timeSlot);
    }
}

TimeSlotsState::Time
TimeSlotsState::slotBeginTime(Time globalBeginTime,
                              Index slotIndex) {
    return globalBeginTime + slotIndex * _slotDuration;
}

bool TimeSlotsState::hasActivity(const Activity *activity) {
    return findSlotWithActivity(activity) != _vector.end();
}

TimeSlotsState::iterator
TimeSlotsState::findSlotWithActivity(const Activity *activity) {
    return std::find_if(_vector.begin(), _vector.end(),
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
    for (auto &timeSlot : _vector) {
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
    _vector = newState._vector;
    _beginTime = newState._beginTime;
    _slotDuration = newState._slotDuration;

    onChangeEvent();
    return *this;
}
