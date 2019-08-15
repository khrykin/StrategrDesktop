//
// Created by Dmitry Khrykin on 2019-07-11.
//

#ifndef STRATEGR_CURRENTACTIVITYSESSION_H
#define STRATEGR_CURRENTACTIVITYSESSION_H


#include "activitysession.h"

class Strategy;
class CurrentActivitySession {
public:
    /// Returns current session if it's non-empty.
    static std::optional<ActivitySession>
    forStrategy(const Strategy &strategy);

    /// Returns upcoming session if it's non-empty.
    static std::optional<ActivitySession>
    upcomingForStrategy(const Strategy &strategy);

private:
    static std::optional<ActivitySession>
    currentSessionForStrategy(const Strategy &strategy);
};


#endif //STRATEGR_CURRENTACTIVITYSESSION_H
