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
    _vector = ActivitySessionsCalculator::
    calculateForTimeSlotsState(timeSlotsState);
//    using namespace std;
//    vector<ActivitySession> result;
//    optional<ActivitySession> cachedActivityGroup;
//
//    for (unsigned int index = 0; index < timeSlotsState.size(); index++) {
//        const auto &timeSlot = timeSlotsState[index];
//        auto activity = timeSlot.activity;
//
//        auto defaultActivityGroup = ActivitySession();
//        defaultActivityGroup.activity = activity;
//        defaultActivityGroup.timeSlots.push_back(&timeSlot);
//
//        auto prevActivity = index > 0
//                            ? timeSlotsState[index - 1].activity
//                            : TimeSlot::NoActivity;
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
//            if (!prevActivity || prevActivity != activity) {
//                if (cachedActivityGroup) {
//                    result.push_back(cachedActivityGroup.value());
//                }
//
//                cachedActivityGroup = defaultActivityGroup;
//            } else {
//                if (cachedActivityGroup) {
//                    cachedActivityGroup->timeSlots.push_back(&timeSlot);
//                }
//            }
//
//            if (cachedActivityGroup && index == timeSlotsState.size() - 1) {
//                result.push_back(cachedActivityGroup.value());
//            }
//        }
//    }
//
//    _vector = result;

    onChangeEvent();
}
