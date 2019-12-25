//
// Created by Dmitry Khrykin on 2019-07-12.
//

#include "Session.h"
#include "TimeUtils.h"

Session::Length Session::length() const {
    return timeSlots.size();
}

Session::Time Session::beginTime() const {
    if (timeSlots.empty()) {
        return 0;
    }

    return timeSlots.front().beginTime;
}

Session::Time Session::endTime() const {
    if (timeSlots.empty()) {
        return 0;
    }

    return timeSlots.back().endTime();
}

Session::Duration Session::duration() const {
    return endTime() - beginTime();
}

std::ostream &operator<<(std::ostream &os, const Session &session) {
    os << "Session(";

    if (session.activity) {
        os << *session.activity;
    } else {
        os << "None";
    }

    os << ", length: " << session.length();
    os << ", beginTime: " << session.beginTime();
    os << ", duration: " << session.duration();

    os << ")";

    return os;
}

bool operator==(const Session &lhs,
                const Session &rhs) {
    return lhs.activity == rhs.activity
           && lhs.beginTime() == rhs.beginTime()
           && lhs.duration() == rhs.duration();
}

bool operator!=(const Session &lhs,
                const Session &rhs) {
    return !(lhs == rhs);
}

double Session::progress() const {
    auto currentSeconds = TimeUtils::currentMinutes() * 60;
    auto beginTimeSeconds = beginTime() * 60;

    if (isFuture()) return 0;
    if (isPast()) return 1;

    auto secondsPassed = static_cast<double>(currentSeconds - beginTimeSeconds);
    auto secondsDuration = 60 * duration();

    return secondsPassed / secondsDuration;
}

Session::Duration Session::passedMinutes() const {
    return TimeUtils::currentMinutes() - beginTime();
}

Session::Duration Session::leftMinutes() const {
    return duration() - passedMinutes();
}

bool Session::isCurrent() const {
    auto currentSeconds = TimeUtils::currentMinutes();
    return currentSeconds >= beginTime() && currentSeconds <= endTime();
}

bool Session::isPast() const {
    return TimeUtils::currentMinutes() > endTime();
}

bool Session::isFuture() const {
    return TimeUtils::currentMinutes() < beginTime();
}
