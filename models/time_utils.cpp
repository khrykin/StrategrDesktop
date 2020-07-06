//
// Created by Dmitry Khrykin on 2019-08-15.
//
#include <string>

#include "time_utils.h"

namespace stg {
    auto time_utils::current_minutes() -> time_utils::minutes {
        return current_seconds() / 60;
    }

    auto time_utils::current_seconds() -> time_utils::seconds {
        auto duration = current_day_duration();
        return static_cast<seconds>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
    }

    auto time_utils::start_of_a_day_from_timestamp(time_t timestamp) -> time_t {
        auto day_components = *day_components_from_timestamp(&timestamp);

        day_components.tm_hour = 0;
        day_components.tm_min = 0;
        day_components.tm_sec = 0;

        timestamp = timestamp_from_day_components(&day_components);

        return timestamp;
    }

    auto time_utils::current_day_duration() -> duration {
        using namespace std::chrono;
        auto clock_now = system_clock::now();
        auto clock_start_of_today = system_clock::from_time_t(today_timestamp());

        return clock_now - clock_start_of_today;
    }

    auto time_utils::human_string_from_minutes(minutes minutes) -> std::string {
        if (minutes < 1) {
            return "0 min";
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

    auto time_utils::string_from_seconds(minutes total_seconds) -> std::string {
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

    auto time_utils::calendar_time_from_minutes(minutes minutes_in_today) -> time_t {
        return today_timestamp() + minutes_in_today * 60;
    }

    auto time_utils::today_timestamp() -> time_t {
        using namespace std::chrono;

        auto clock_now = system_clock::now();
        auto current_timestamp = system_clock::to_time_t(clock_now);

        return start_of_a_day_from_timestamp(current_timestamp);
    }
};
