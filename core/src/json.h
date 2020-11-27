//
// Created by Dmitry Khrykin on 2019-11-23.
//

#ifndef STRATEGR_JSON_H
#define STRATEGR_JSON_H

#include <string>

#include "activitylist.h"
#include "timeslotsstate.h"

namespace stg {
    class strategy;

    namespace json {
        namespace keys {
            inline constexpr auto slot_duration = "slotDuration";
            inline constexpr auto start_time = "startTime";
            inline constexpr auto activities = "activities";
            inline constexpr auto slots = "slots";
            inline constexpr auto version = "version";

            namespace activity {
                inline constexpr auto name = "name";
                inline constexpr auto color = "color";
            }
        }

        auto serialize(const strategy &strategy) -> std::string;
        auto parse(const std::string &json_string) -> std::unique_ptr<strategy>;
    }
}

#endif//STRATEGR_JSON_H
