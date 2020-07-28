//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_TIME_UTILS_H
#define STRATEGR_TIME_UTILS_H

#include <ctime>
#include <chrono>
#include <iostream>

namespace stg::time_utils {

#pragma mark - Type Aliases

    using seconds = unsigned;
    using minutes = unsigned;
    using duration = std::chrono::duration<double>;

#pragma mark - Setting Time Source

    // Time source is a function that returns current seconds since beginning of a day.
    // Default time source uses local system time.
    void set_time_source(std::function<seconds()> seconds_getter);

#pragma mark - Getting Current Time

    auto current_seconds() -> seconds;
    auto current_minutes() -> minutes;

#pragma mark - Getting Calendar Time from Relative Time

    auto calendar_time_from_minutes(minutes minutes_in_today) -> time_t;

#pragma mark - Getting String Representation of Time

    auto string_from_seconds(minutes total_seconds) -> std::string;
    auto human_string_from_minutes(minutes minutes) -> std::string;
}


#endif //STRATEGR_TIME_UTILS_H
