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

        timestamp start_of_a_day_from_timestamp(timestamp timestamp);
        duration current_day_duration();
        minutes current_minutes();
        seconds current_seconds();

        std::string string_from_seconds(minutes total_seconds);
        std::string human_string_from_minutes(minutes minutes);
    };
}


#endif //STRATEGR_TIME_UTILS_H
