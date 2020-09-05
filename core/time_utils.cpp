//
// Created by Dmitry Khrykin on 2019-08-15.
//

#include <string>
#include <functional>

#include "time_utils.h"

namespace stg::time_utils {

#pragma mark - Type & Function Aliases

    using day_components = std::tm;

    constexpr static auto day_components_from_timestamp = std::localtime;
    constexpr static auto timestamp_from_day_components = std::mktime;

#pragma mark - Time Source

    struct time_source {
        static auto current_seconds() -> seconds;
        static std::function<seconds()> seconds_getter;
    };

    std::function<seconds()> time_source::seconds_getter = nullptr;

    void set_time_source(std::function<seconds()> fn) {
        time_source::seconds_getter = std::move(fn);
    }

    auto default_time_source() -> seconds;

    auto time_source::current_seconds() -> seconds {
        if (seconds_getter)
            return seconds_getter();
        else
            return default_time_source();
    }

#pragma mark - Default Time Source Implementation

    auto start_of_a_day_from_timestamp(time_t timestamp) -> time_t {
        auto day_components = *day_components_from_timestamp(&timestamp);

        day_components.tm_hour = 0;
        day_components.tm_min = 0;
        day_components.tm_sec = 0;

        return timestamp_from_day_components(&day_components);;
    }

    auto today_timestamp() -> time_t {
        using namespace std::chrono;

        auto clock_now = system_clock::now();
        auto current_timestamp = system_clock::to_time_t(clock_now);

        return start_of_a_day_from_timestamp(current_timestamp);
    }

    auto current_day_duration() -> duration {
        using namespace std::chrono;

        auto clock_now = system_clock::now();
        auto clock_start_of_today = system_clock::from_time_t(today_timestamp());

        return clock_now - clock_start_of_today;
    }

    auto default_time_source() -> seconds {
        auto duration = current_day_duration();
        return static_cast<seconds>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
    }

#pragma mark - Getting Current Time

    auto current_seconds() -> seconds {
        return time_source::current_seconds();
    }

    auto current_minutes() -> time_utils::minutes {
        return current_seconds() / 60;
    }

#pragma mark - Getting Calendar Time from Relative Time

    auto calendar_time_from_seconds(seconds seconds_in_today) -> time_t {
        return today_timestamp() + seconds_in_today;
    }

    auto calendar_time_from_minutes(minutes minutes_in_today) -> time_t {
        return calendar_time_from_seconds(minutes_in_today * 60);
    }

#pragma mark - Getting Relative Time from Calendar Time

    auto seconds_from_calendar_time(time_t calendar_time) -> seconds {
        auto start_of_a_day = start_of_a_day_from_timestamp(calendar_time);
        return static_cast<seconds>(calendar_time - start_of_a_day);
    }

#pragma mark - Getting String Representation of Time

    auto string_from_seconds(minutes total_seconds) -> std::string {
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

    auto human_string_from_minutes(minutes minutes) -> std::string {
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

    auto to_string(time_t calendar_time, const char *format) -> std::string {
        std::tm *tm = std::localtime(&calendar_time);
        char buffer[255];

        std::strftime(buffer, 255, format, tm);

        return buffer;
    }
}
