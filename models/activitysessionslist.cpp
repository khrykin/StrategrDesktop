//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <algorithm>
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

std::vector<const ActivitySessionsList::OverviewItem>
ActivitySessionsList::overview() const {
    if (_vector.empty()) {
        return {};
    }

    auto overallBeginTime = _vector.front().beginTime();
    auto totalDuraion = _vector.back().endTime() - overallBeginTime;

    std::vector<const OverviewItem> result;
    std::transform(_vector.begin(),
                   _vector.end(),
                   std::back_inserter(result),
                   [totalDuraion, overallBeginTime](auto &session) {
                       auto durationPercentage = static_cast<float>(session.duration())
                                                 / totalDuraion;
                       auto beginPercentage = static_cast<float>(session.beginTime()
                                                                 - overallBeginTime)
                                              / totalDuraion;

                       auto color = session.activity
                                    ? std::make_optional(session.activity->color())
                                    : std::nullopt;

                       return OverviewItem{
                               durationPercentage,
                               beginPercentage,
                               color
                       };
                   });

    return result;
}
