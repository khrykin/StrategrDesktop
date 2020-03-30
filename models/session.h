#ifndef STRATEGR_ACTIVITYSESSION_H
#define STRATEGR_ACTIVITYSESSION_H

#include <vector>
#include <memory>
#include <optional>
#include <iostream>

#include "timeslot.h"

namespace stg {
    struct activity;

    struct session {
        using length_t = int;
        using time_t = time_slot::time_t;
        using duration_t = time_slot::duration_t;

        std::vector<time_slot> time_slots{};
        activity *activity = time_slot::no_activity;

        length_t length() const;
        time_t begin_time() const;
        time_t end_time() const;
        duration_t duration() const;

        double progress() const;

        bool is_current() const;
        bool is_past() const;
        bool is_future() const;

        bool empty() const;

        duration_t passed_minutes() const;
        duration_t left_minutes() const;

        friend bool operator==(const session &lhs,
                               const session &rhs);

        friend bool operator!=(const session &lhs,
                               const session &rhs);

        friend std::ostream &operator<<(std::ostream &os,
                                        const session &session);
    };

};

#endif // STRATEGR_ACTIVITYSESSION_H
