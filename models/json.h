//
// Created by Dmitry Khrykin on 2019-11-23.
//

#ifndef STRATEGR_JSON_H
#define STRATEGR_JSON_H

#include <string>
#include <nlohmann/json.hpp>

#include "activitylist.h"
#include "timeslotsstate.h"

namespace stg {
    class strategy;

    class json {
    public:
        static std::string serialize(const strategy &strategy);
        static std::unique_ptr<strategy> parse(const std::string &json_string);

        struct keys {
            static constexpr auto slot_duration = "slotDuration";
            static constexpr auto start_time = "startTime";
            static constexpr auto activities = "activities";
            static constexpr auto slots = "slots";
        };
    private:
        activity_list activities;

        static activity_list::data_t
        parse_activities(const nlohmann::json &json);

        static time_slots_state::data_t
        parse_time_slots(const nlohmann::json &json,
                         const activity_list::data_t &activities);
    };
}

#endif //STRATEGR_JSON_H


