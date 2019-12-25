//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <algorithm>
#include "SessionsList.h"

std::optional<SessionsList::Index>
SessionsList::sessionIndexForTimeSlotIndex(
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

void SessionsList::recalculateForTimeSlotsState(const TimeSlotsState &timeSlotsState) {
    _vector = SessionsCalculator::calculateForTimeSlotsState(timeSlotsState);

    onChangeEvent();
}

const Session *
SessionsList::sessionAfter(const Session &activitySession) const {
    auto it = findConst(activitySession);
    return it < _vector.end() - 1 ? &*(it + 1) : nullptr;
}

std::optional<Session>
SessionsList::sessionBefore(const Session &activitySession) const {
    auto it = findConst(activitySession);
    return it > _vector.begin()
           ? std::make_optional(*(it - 1))
           : std::nullopt;
}

std::string SessionsList::classPrintName() const {
    return "SessionsList";
}

std::vector<SessionsList::OverviewItem>
SessionsList::overview() const {
    if (_vector.empty()) {
        return {};
    }

    auto overallBeginTime = _vector.front().beginTime();
    auto totalDuraion = _vector.back().endTime() - overallBeginTime;

    std::vector<OverviewItem> result;
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

std::vector<Session> SessionsList::nonEmpty() const {
    std::vector<Session> result;
    std::copy_if(_vector.begin(),
                 _vector.end(),
                 std::back_inserter(result),
                 [](auto &session) {
                     return session.activity != TimeSlot::NoActivity;
                 });

    return result;
}
