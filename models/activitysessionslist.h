//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_SESSIONSLIST_H
#define MODELS_SESSIONSLIST_H

#include <iostream>

#include "activitysession.h"
#include "privatelist.hpp"
#include "notifiableonchange.h"
#include "timeslotsstate.h"
#include "streamablelist.h"
#include "activitysessionscalculator.h"

using ActivitySessionsListBase = PrivateList<ActivitySession>;
class Strategy;

class ActivitySessionsList :
        public ActivitySessionsListBase,
        public NotifiableOnChange,
        public StreamableList<ActivitySessionsList> {
public:
    std::string classPrintName() const override {
        return "ActivitySessionsList";
    }

private:
    using ActivitySessionsListBase::ActivitySessionsListBase;

    void recalculateForTimeSlotsState(const TimeSlotsState &timeSlotsState) {
        _vector = ActivitySessionsCalculator::
        calculateForTimeSlotsState(timeSlotsState);

        onChangeEvent();
    };


    friend Strategy;
};


#endif //MODELS_SESSIONSLIST_H
