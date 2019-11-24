//
// Created by Dmitry Khrykin on 2019-07-12.
//

#include "activitysession.h"


ActivitySession::Length ActivitySession::length() const {
    return timeSlots.size();
}

ActivitySession::Time ActivitySession::beginTime() const {
    if (timeSlots.empty()) {
        return 0;
    }

    return timeSlots.front().beginTime;
}

ActivitySession::Time ActivitySession::endTime() const {
    if (timeSlots.empty()) {
        return 0;
    }

    return timeSlots.back().endTime();
}

ActivitySession::Duration ActivitySession::duration() const {
    return endTime() - beginTime();
}

std::ostream &operator<<(std::ostream &os, const ActivitySession &session) {
    os << "ActivitySession(";

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

bool operator==(const ActivitySession &lhs,
                const ActivitySession &rhs) {
    return lhs.activity == rhs.activity
           && lhs.beginTime() == rhs.beginTime()
           && lhs.duration() == rhs.duration();
}

bool operator!=(const ActivitySession &lhs,
                const ActivitySession &rhs) {
    return !(lhs == rhs);
}
