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

        // The member below intentionally shares its name with the `activity`
        // type above; GCC warns about that (-Wchanges-meaning) but this is a
        // long-established part of the public API, referenced throughout the
        // codebase as `session.activity`, so it's not worth renaming.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wchanges-meaning"
#endif
        activity *activity = time_slot::no_activity;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

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
        auto current_seconds() const -> minutes;
        auto current_minutes() const -> minutes;
    };

};

#endif// STRATEGR_ACTIVITYSESSION_H
