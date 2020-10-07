#ifndef STRATEGR_ACTIVITYSESSION_H
#define STRATEGR_ACTIVITYSESSION_H

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "timeslot.h"

namespace stg {
    struct activity;

    struct session {
        using length_t = int;
        using minutes = time_slot::minutes;

        std::vector<time_slot> time_slots{};
        activity *activity = time_slot::no_activity;

        auto length() const -> length_t;
        auto begin_time() const -> minutes;
        auto end_time() const -> minutes;
        auto duration() const -> minutes;

        auto progress() const -> double;

        auto is_current() const -> bool;
        auto is_past() const -> bool;
        auto is_future() const -> bool;

        auto empty() const -> bool;

        auto passed_minutes() const -> minutes;
        auto left_minutes() const -> minutes;

        friend auto operator==(const session &lhs, const session &rhs) -> bool;
        friend auto operator!=(const session &lhs, const session &rhs) -> bool;
        friend auto operator<<(std::ostream &os, const session &session) -> std::ostream &;

    private:
        auto current_seconds() const -> unsigned;
        auto current_minutes() const -> unsigned;
    };

};

#endif// STRATEGR_ACTIVITYSESSION_H
