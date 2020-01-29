//
// Created by Dmitry Khrykin on 2019-11-23.
//

#include <algorithm>

#include "JSON.h"

std::string JSON::serialize(const Strategy &strategy) {
    auto json = nlohmann::json();

    json[Keys::slotDuration] = strategy.timeSlotDuration();
    json[Keys::startTime] = strategy.beginTime();

    std::transform(strategy.activities().begin(),
                   strategy.activities().end(),
                   std::back_inserter(json[Keys::activities]),
                   [](auto activity) {
                       return activity->toJson();
                   });

    std::transform(strategy.timeSlots().begin(),
                   strategy.timeSlots().end(),
                   std::back_inserter(json[Keys::slots]),
                   [&strategy](auto &timeSlot) {
                       auto activityIndex = strategy
                               .activities()
                               .indexOf(timeSlot.activity);

                       if (activityIndex) {
                           return nlohmann::json(*activityIndex);
                       }

                       return nlohmann::json();
                   });

    return json.dump();
}

std::unique_ptr<Strategy> JSON::parse(const std::string &jsonString) {
    using namespace nlohmann;

    try {
        auto json = json::parse(jsonString);

        auto activities = parseActivities(json);
        auto timeSlots = parseTimeSlots(json, activities);

        return std::make_unique<Strategy>(timeSlots, activities);
    } catch (...) {
        return nullptr;
    }
}

TimeSlotsState::RawData
JSON::parseTimeSlots(const nlohmann::json &json,
                     const ActivityList::RawData &activities) {
    TimeSlotsState::RawData timeSlotsVector;

    auto timeSlotDuration = Strategy::Defaults::timeSlotDuration;
    if (!json[Keys::slotDuration].is_null())
        timeSlotDuration = json[Keys::slotDuration];

    auto beginTime = Strategy::Defaults::beginTime;
    if (!json[Keys::startTime].is_null())
        beginTime = json[Keys::startTime];

    if (!json[Keys::slots].is_null()) {
        for (auto it = json[Keys::slots].begin();
             it != json[Keys::slots].end();
             ++it) {

            auto slotIndex = it - json[Keys::slots].begin();
            auto timeSlotBeginTime = beginTime + slotIndex * timeSlotDuration;

            auto timeSlot = TimeSlot(timeSlotBeginTime,
                                     timeSlotDuration);

            if (!it->is_null()) {
                auto activityIndex = static_cast<ActivityList::Index>(*it);

                try {
                    auto *activity = activities.at(activityIndex).get();
                    timeSlot.activity = activity;
                } catch (const std::out_of_range &) {
                    // Activity is present in time slots, but not present in
                    // strategy.activities(), so we won't preserve it.
                }
            }

            timeSlotsVector.push_back(timeSlot);
        }
    }

    return timeSlotsVector;
}

ActivityList::RawData JSON::parseActivities(const nlohmann::json &json) {
    ActivityList::RawData activitiesVector;

    if (!json[Keys::activities].is_null()) {
        for (const auto &activityJson : json[Keys::activities]) {
            auto activity = std::make_shared<Activity>(Activity::fromJson(activityJson));
            activitiesVector.emplace_back(activity);
        }
    }

    return activitiesVector;
}