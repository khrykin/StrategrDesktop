//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include "activitysessionslist.h"

std::optional<ActivitySessionsList::Index>
ActivitySessionsList::sessionIndexForTimeSlotIndex(
        TimeSlotsState::Index timeSlotIndex) const {
    auto slotsCount = 0;
    for (auto &session : _vector) {
        auto firstSlotIndex = slotsCount;
        auto nextSlotIndex = slotsCount + session.length();

        if (firstSlotIndex <= timeSlotIndex
            && timeSlotIndex < nextSlotIndex) {
            auto index = static_cast<Index>(&session - &_vector[0]);
            return index;
        }

        slotsCount = nextSlotIndex;
    }

    return std::nullopt;
}

void ActivitySessionsList::recalculateForTimeSlotsState(const TimeSlotsState &timeSlotsState) {
    _vector = ActivitySessionsCalculator::calculateForTimeSlotsState(timeSlotsState);

    onChangeEvent();
}

std::optional<ActivitySession>
ActivitySessionsList::sessionAfter(const ActivitySession &activitySession) const {
    auto it = findConst(activitySession);
    return it < _vector.end() - 1
           ? std::make_optional(*(it + 1))
           : std::nullopt;
}

std::optional<ActivitySession>
ActivitySessionsList::sessionBefore(const ActivitySession &activitySession) const {
    auto it = findConst(activitySession);
    return it > _vector.begin()
           ? std::make_optional(*(it - 1))
           : std::nullopt;
}

std::string ActivitySessionsList::classPrintName() const {
    return "ActivitySessionsList";
}
