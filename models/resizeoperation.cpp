//
// Created by Dmitry Khrykin on 2019-08-01.
//

#include "resizeoperation.h"

stg::resize_operation::resize_operation(time_slots_state *time_slots) :
        time_slots(time_slots) {
}

void stg::resize_operation::fill_slots(time_slots_state::index_t from_index,
                                       time_slots_state::index_t till_index) {
    time_slots->fill_slots(from_index, till_index);
}

void stg::resize_operation::fill_slots_shifting(time_slots_state::index_t from_index,
                                                time_slots_state::index_t till_index) {
    time_slots->fill_slots_shifting(from_index, till_index);
}

bool stg::resize_operation::state_changed() {
    return time_slots->data() != initial_time_slots;
}

