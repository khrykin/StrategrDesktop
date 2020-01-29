//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <algorithm>
#include "SessionsList.h"

std::optional<SessionsList::Index>
SessionsList::sessionIndexForTimeSlotIndex(
        TimeSlotsState::Index timeSlotIndex) const {
    auto slotsCount = 0;
    for (auto &session : _data) {
        auto firstSlotIndex = slotsCount;
        auto nextSlotIndex = slotsCount + session.length();

        if (firstSlotIndex <= timeSlotIndex
            && timeSlotIndex < nextSlotIndex) {
            auto index = static_cast<Index>(&session - &_data[0]);
            return index;
        }

        slotsCount = nextSlotIndex;
    }

    return std::nullopt;
}

void SessionsList::recalculateForTimeSlotsState(const TimeSlotsState &timeSlotsState) {
    _data = SessionsCalculator::calculateForTimeSlotsState(timeSlotsState);

    onChangeEvent();
}

const Session *
SessionsList::sessionAfter(const Session &activitySession) const {
    auto it = findConst(activitySession);
    return it < std::prev(_data.end()) ? &*std::next(it) : nullptr;
}

std::optional<Session>
SessionsList::sessionBefore(const Session &activitySession) const {
    auto it = findConst(activitySession);
    return it > _data.begin()
           ? std::make_optional(*std::prev(it))
           : std::nullopt;
}

std::string SessionsList::classPrintName() const {
    return "SessionsList";
}

std::vector<SessionsList::OverviewItem>
SessionsList::overview() const {
    if (_data.empty()) {
        return {};
    }

    auto overallBeginTime = _data.front().beginTime();
    auto totalDuraion = _data.back().endTime() - overallBeginTime;

    std::vector<OverviewItem> result;
    std::transform(_data.begin(),
                   _data.end(),
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
    std::copy_if(_data.begin(),
                 _data.end(),
                 std::back_inserter(result),
                 [](auto &session) {
                     return session.activity != TimeSlot::NoActivity;
                 });

    return result;
}
