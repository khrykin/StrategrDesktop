//
// Created by Dmitry Khrykin on 2019-11-23.
//

#ifndef STRATEGR_JSON_H
#define STRATEGR_JSON_H

#include <nlohmann/json.hpp>
#include <string>

#include "activitylist.h"
#include "timeslotsstate.h"

namespace stg {
    class strategy;

    class json {
    public:
        static auto serialize(const strategy &strategy) -> std::string;
        static auto parse(const std::string &json_string) -> std::unique_ptr<strategy>;

        struct keys {
            static constexpr auto slot_duration = "slotDuration";
            static constexpr auto start_time = "startTime";
            static constexpr auto activities = "activities";
            static constexpr auto slots = "slots";
            static constexpr auto version = "version";
        };

    private:
        activity_list activities;

        static auto parse_activities(const nlohmann::json &json) -> activity_list::data_t;
        static auto parse_time_slots(const nlohmann::json &json,
                                     const activity_list::data_t &activities) -> time_slots_state::data_t;
    };
}

#endif//STRATEGR_JSON_H
