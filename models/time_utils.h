//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_TIME_UTILS_H
#define STRATEGR_TIME_UTILS_H

#include <ctime>
#include <chrono>
#include <iostream>

namespace stg {
    namespace time_utils {
        using seconds = unsigned;
        using minutes = unsigned;

        using timestamp = std::time_t;
        using day_components = std::tm;
        using duration = std::chrono::duration<double>;

        constexpr static auto day_components_from_timestamp = std::localtime;
        constexpr static auto timestamp_from_day_components = std::mktime;

        auto start_of_a_day_from_timestamp(timestamp timestamp) -> time_utils::timestamp;
        auto current_day_duration() -> duration;
        auto current_minutes() -> minutes;
        auto current_seconds() -> seconds;

        auto string_from_seconds(minutes total_seconds) -> std::string;
        auto human_string_from_minutes(minutes minutes) -> std::string;
    };
}


#endif //STRATEGR_TIME_UTILS_H
