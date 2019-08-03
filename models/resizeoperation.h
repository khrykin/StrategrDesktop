//
// Created by Dmitry Khrykin on 2019-08-01.
//

#ifndef STRATEGR_RESIZEOPERATION_H
#define STRATEGR_RESIZEOPERATION_H


#include "timeslotsstate.h"

class ResizeOperation {
public:
    explicit ResizeOperation(TimeSlotsState &timeSlots);

    void fillSlots(TimeSlotsState::Index fromIndex,
                   TimeSlotsState::Index tillIndex);

    bool stateChanged();
private:
    TimeSlotsState &timeSlots;
    TimeSlotsState initialTimeSlotsState = timeSlots;
};


#endif //STRATEGR_RESIZEOPERATION_H
