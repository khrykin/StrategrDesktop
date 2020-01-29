//
// Created by Dmitry Khrykin on 2019-11-23.
//

#ifndef STRATEGR_JSON_H
#define STRATEGR_JSON_H

#include <string>
#include <nlohmann/json.hpp>

#include "Strategy.h"

class JSON {
public:
    static std::string serialize(const Strategy &strategy);
    static std::unique_ptr<Strategy> parse(const std::string &jsonString);

    struct Keys {
        static constexpr auto slotDuration = "slotDuration";
        static constexpr auto startTime = "startTime";
        static constexpr auto activities = "activities";
        static constexpr auto slots = "slots";
    };
private:
    ActivityList activities;

    static ActivityList::RawData
    parseActivities(const nlohmann::json &json);

    static TimeSlotsState::RawData
    parseTimeSlots(const nlohmann::json &json,
                   const ActivityList::RawData &activities);
};


#endif //STRATEGR_JSON_H


