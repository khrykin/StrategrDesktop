//
// Created by Dmitry Khrykin on 2019-08-01.
//

#include "resizeoperation.h"

namespace stg {
    resize_operation::resize_operation(time_slots_state *time_slots) : time_slots(time_slots) {
    }

    void resize_operation::fill_slots(index_t from_index, index_t till_index) {
        time_slots->fill_slots(from_index, till_index);
    }

    void resize_operation::fill_slots_shifting(index_t from_index, index_t till_index) {
        time_slots->fill_slots_shifting(from_index, till_index);
    }

    auto resize_operation::state_changed() -> bool {
        return time_slots->data() != initial_time_slots;
    }
}
