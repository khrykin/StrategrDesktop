//
// Created by Dmitry Khrykin on 2019-08-01.
//

#include "ResizeOperation.h"

ResizeOperation::ResizeOperation(TimeSlotsState *timeSlots) :
        timeSlots(timeSlots) {
}

void ResizeOperation::fillSlots(TimeSlotsState::Index fromIndex,
                                TimeSlotsState::Index tillIndex) {
    timeSlots->fillSlots(fromIndex, tillIndex);
}

bool ResizeOperation::stateChanged() {
    return timeSlots->data() != initialTimeSlots;
}
