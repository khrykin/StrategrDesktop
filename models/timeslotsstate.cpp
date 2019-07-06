//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include "timeslotsstate.h"

TimeSlotsState::Time TimeSlotsState::beginTime() const {
    return _beginTime;
}

void TimeSlotsState::setBeginTime(TimeSlotsState::Time beginTime) {
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

void TimeSlotsState::setSlotDuration(TimeSlotsState::Duration slotDuration) {
    _slotDuration = slotDuration;
    // TODO: recalculate
    onChangeEvent();
}

TimeSlotsState::StateSize TimeSlotsState::numberOfSlots() const {
    return _vector.size();
}

void TimeSlotsState::setNumberOfSlots(TimeSlotsState::StateSize newNumberOfSlots) {
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

TimeSlotsState::TimeSlotsState(TimeSlotsState::Time startTime,
                               TimeSlotsState::Duration slotDuration,
                               TimeSlotsState::StateSize numberOfSlots)
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

void TimeSlotsState::fillSlots(TimeSlotsState::Index fromIndex,
                               TimeSlotsState::Index tillIndex) {
    if (!hasIndices(fromIndex, tillIndex)) {
        return;
    }

    if (tillIndex < fromIndex) {
        std::swap(tillIndex, fromIndex);
    }

    for (auto copyIndex = fromIndex; copyIndex <= tillIndex; copyIndex++) {
        _vector[copyIndex].activity = _vector[fromIndex].activity;
    }

    onChangeEvent();
}

bool TimeSlotsState::hasIndex(TimeSlotsState::Index slotIndex) {
    return slotIndex >= 0 && slotIndex < _vector.size();
}

bool TimeSlotsState::hasIndices(TimeSlotsState::Index slotIndex) {
    return hasIndex(slotIndex);
}

void TimeSlotsState::setActivityAtIndex(const Activity *activity,
                                        PrivateList::Index slotIndex) {
    _vector[slotIndex].activity = activity;
}

void TimeSlotsState::setActivityAtIndices(const Activity *activity,
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

void TimeSlotsState::populateVector(TimeSlotsState::Time startTime,
                                    TimeSlotsState::StateSize numberOfSlots) {
    for (auto slotIndex = 0; slotIndex < numberOfSlots; ++slotIndex) {
        auto timeSlot = TimeSlot(
                slotBeginTime(startTime, slotIndex),
                slotDuration()
        );

        _vector.emplace_back(timeSlot);
    }
}

TimeSlotsState::Time
TimeSlotsState::slotBeginTime(TimeSlotsState::Time globalBeginTime,
                              TimeSlotsState::Index slotIndex) {
    return globalBeginTime + slotIndex * slotDuration();
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

void TimeSlotsState::removeActivity(const Activity *activity) {
    editActivity(activity, TimeSlot::NoActivity);
}

void TimeSlotsState::editActivity(const Activity *oldActivity,
                                  const Activity *newActivity) {
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
