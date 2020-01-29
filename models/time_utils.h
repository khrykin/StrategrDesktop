//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_TIME_UTILS_H
#define STRATEGR_TIME_UTILS_H

#include <ctime>
#include <chrono>
#include <iostream>

namespace stg {
    class time_utils {
    public:
        using seconds = unsigned;
        using minutes = unsigned;

        using timestamp = std::time_t;
        using day_components = std::tm;
        using duration = std::chrono::duration<double>;

        constexpr static auto day_components_from_timestamp = std::localtime;
        constexpr static auto timestamp_from_day_components = std::mktime;

        static timestamp start_of_a_day_from_timestamp(timestamp timestamp);
        static duration current_day_duration();
        static minutes current_minutes();
        static seconds current_seconds();

        static std::string human_time_for_minutes(minutes minutes);
    };
}


#endif //STRATEGR_TIME_UTILS_H
