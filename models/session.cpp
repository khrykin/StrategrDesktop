//
// Created by Dmitry Khrykin on 2019-07-12.
//

#include "session.h"
#include "time_utils.h"
#include "activity.h"

stg::session::length_t stg::session::length() const {
    return static_cast<length_t>(time_slots.size());
}

stg::session::time_t stg::session::begin_time() const {
    if (time_slots.empty()) {
        return 0;
    }

    return time_slots.front().begin_time;
}

stg::session::time_t stg::session::end_time() const {
    if (time_slots.empty()) {
        return 0;
    }

    return time_slots.back().end_time();
}

stg::session::duration_t stg::session::duration() const {
    return end_time() - begin_time();
}

std::ostream &stg::operator<<(std::ostream &os, const stg::session &session) {
    os << "session(";

    if (session.activity) {
        os << *session.activity;
    } else {
        os << "none";
    }

    os << ", length: " << session.length();
    os << ", begin_time: " << session.begin_time();
    os << ", duration: " << session.duration();

    os << ")";

    return os;
}

bool stg::operator==(const session &lhs,
                     const session &rhs) {
    return lhs.activity == rhs.activity
           && lhs.begin_time() == rhs.begin_time()
           && lhs.duration() == rhs.duration();
}

bool stg::operator!=(const session &lhs,
                     const session &rhs) {
    return !(lhs == rhs);
}

double stg::session::progress() const {
    auto current_seconds = stg::time_utils::current_seconds();
    auto begin_time_seconds = begin_time() * 60;

    if (is_future()) return 0;
    if (is_past()) return 1;

    auto seconds_passed = static_cast<double>(current_seconds - begin_time_seconds);
    auto seconds_duration = 60 * duration();

    return seconds_passed / seconds_duration;
}

stg::session::duration_t stg::session::passed_minutes() const {
    return std::round(static_cast<float>(stg::time_utils::current_seconds() - 60 * begin_time()) / 60.0f);
}

stg::session::duration_t stg::session::left_minutes() const {
    return duration() - passed_minutes();
}

bool stg::session::is_current() const {
    auto current_minutes = stg::time_utils::current_minutes();
    return current_minutes >= begin_time() && current_minutes <= end_time() - 1;
}

bool stg::session::is_past() const {
    return stg::time_utils::current_minutes() > end_time();
}

bool stg::session::is_future() const {
    return stg::time_utils::current_minutes() < begin_time();
}

