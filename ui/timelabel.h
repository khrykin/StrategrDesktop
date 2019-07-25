//
// Created by Dmitry Khrykin on 2019-07-25.
//

#ifndef STRATEGR_TIMELABEL_H
#define STRATEGR_TIMELABEL_H

#include <QString>
#include "strategy.h"

struct TimeLabel {
    QString label = "";
    Strategy::Time time = 0;

    friend bool operator==(const TimeLabel &lhs, const TimeLabel &rhs) {
        return lhs.time == rhs.time;
    }

    friend bool operator!=(const TimeLabel &lhs, const TimeLabel &rhs) {
        return !(lhs == rhs);
    }
};

#endif //STRATEGR_TIMELABEL_H
