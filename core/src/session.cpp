//
// Created by Dmitry Khrykin on 2019-07-12.
//

#include "session.h"
#include "activity.h"
#include "time_utils.h"

namespace stg {
    auto session::length() const -> length_t {
        return static_cast<length_t>(time_slots.size());
    }

    auto session::begin_time() const -> minutes {
        if (time_slots.empty()) {
            return 0;
        }

        return time_slots.front().begin_time;
    }

    auto session::end_time() const -> minutes {
        if (time_slots.empty()) {
            return 0;
        }

        return time_slots.back().end_time();
    }

    auto session::duration() const -> minutes {
        return end_time() - begin_time();
    }

    auto session::progress() const -> double {
        if (is_future()) return 0;
        if (is_past()) return 1;

        auto seconds_passed = (double) (current_seconds() - begin_time() * 60);
        auto seconds_duration = 60 * duration();

        return seconds_passed / seconds_duration;
    }

    auto session::passed_minutes() const -> minutes {
        return current_minutes() - begin_time();
    }

    auto session::left_minutes() const -> minutes {
        return duration() - passed_minutes();
    }

    auto session::is_current() const -> bool {
        return current_minutes() >= begin_time() && current_minutes() <= end_time() - 1;
    }

    auto session::is_past() const -> bool {
        return current_minutes() > end_time();
    }

    auto session::is_future() const -> bool {
        return current_minutes() < begin_time();
    }

    auto session::empty() const -> bool {
        return activity == time_slot::no_activity;
    }

    auto session::current_seconds() const -> unsigned {
        auto current_seconds = time_utils::current_seconds();
        auto begin_time_seconds = begin_time() * 60;

        if (end_time() > 24 * 60 && current_seconds < begin_time_seconds)
            current_seconds += 24 * 3600;

        return current_seconds;
    }

    auto session::current_minutes() const -> unsigned {
        return std::round(current_seconds() / 60);
    }

    auto operator<<(std::ostream &os, const session &session) -> std::ostream & {
        os << "session(";

        if (session.activity) {
            os << *session.activity;
        } else {
            os << "none";
        }

        os << ", length: " << session.length();
        os << ", begin_time: " << session.begin_time();
        os << ", end_time: " << session.end_time();
        os << ", duration: " << session.duration();

        os << ")";

        return os;
    }

    auto operator==(const session &lhs, const session &rhs) -> bool {
        return lhs.activity == rhs.activity && lhs.begin_time() == rhs.begin_time() && lhs.length() == rhs.length() && lhs.duration() == rhs.duration();
    }

    auto operator!=(const session &lhs, const session &rhs) -> bool {
        return !(lhs == rhs);
    }
}
