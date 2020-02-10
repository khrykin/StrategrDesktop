//
// Created by Dmitry Khrykin on 2019-08-15.
//
#include <string>

#include "time_utils.h"

namespace stg {
    time_utils::minutes time_utils::current_minutes() {
        return current_seconds() / 60;
    }

    time_utils::seconds time_utils::current_seconds() {
        auto duration = current_day_duration();
        return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }

    time_utils::timestamp
    time_utils::start_of_a_day_from_timestamp(time_utils::timestamp timestamp) {
        auto day_components = *day_components_from_timestamp(&timestamp);

        day_components.tm_hour = 0;
        day_components.tm_min = 0;
        day_components.tm_sec = 0;

        timestamp = timestamp_from_day_components(&day_components);

        return timestamp;
    }

    time_utils::duration time_utils::current_day_duration() {
        using namespace std::chrono;

        auto clock_now = system_clock::now();
        auto current_timestamp = system_clock::to_time_t(clock_now);

        auto start_of_a_day_timestamp = start_of_a_day_from_timestamp(current_timestamp);
        auto clock_start_of_today = system_clock::from_time_t(start_of_a_day_timestamp);

        return clock_now - clock_start_of_today;
    }

    std::string time_utils::human_string_from_minutes(time_utils::minutes minutes) {
        if (minutes < 1) {
            return "Less than 1 min";
        }

        unsigned int hours = minutes / 60;
        unsigned int mins_remainder = minutes - hours * 60;

        std::string result;

        if (hours > 0) {
            result += std::to_string(hours);
        }

        if (hours > 0 && mins_remainder == 30) {
            result += ".5 h";
        } else if (hours > 0) {
            result += " h";
        }

        if (mins_remainder != 0) {
            if (hours > 0) {
                if (mins_remainder == 30) {
                    return result;
                }

                result += " ";
            }

            result += std::to_string(mins_remainder) + " min";
        }

        return result;
    }

    std::string time_utils::string_from_seconds(time_utils::minutes total_seconds) {
        auto hours = total_seconds / 3600;
        auto minutes = (total_seconds - 3600 * hours) / 60;
        auto seconds = total_seconds - 3600 * hours - 60 * minutes;

        std::string result = std::to_string(hours) + " h";

        if (minutes)
            result += " " + std::to_string(minutes) + " m";

        if (seconds)
            result += " " + std::to_string(seconds) + "s";

        return result;
    }
};
