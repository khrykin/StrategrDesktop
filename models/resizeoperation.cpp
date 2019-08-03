//
// Created by Dmitry Khrykin on 2019-08-01.
//

#include "resizeoperation.h"

ResizeOperation::ResizeOperation(TimeSlotsState &timeSlots) :
        timeSlots(timeSlots) {
}

void ResizeOperation::fillSlots(TimeSlotsState::Index fromIndex, TimeSlotsState::Index tillIndex) {
    timeSlots.fillSlots(fromIndex, tillIndex);
    // NB! We don't want to commit to history here,
    // because user may continue to drag session to next slot index!
    // So we'll commit from UI side.
}

bool ResizeOperation::stateChanged() {
    return timeSlots != initialTimeSlotsState;
}
