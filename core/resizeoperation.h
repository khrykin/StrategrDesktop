//
// Created by Dmitry Khrykin on 2019-08-01.
//

#ifndef STRATEGR_RESIZEOPERATION_H
#define STRATEGR_RESIZEOPERATION_H

#include "timeslotsstate.h"

namespace stg {
    class resize_operation {
    public:
        using index_t = time_slots_state::index_t;

        explicit resize_operation(time_slots_state *time_slots);

        void fill_slots(index_t from_index, index_t till_index);
        void fill_slots_shifting(index_t from_index, index_t till_index);

        auto state_changed() -> bool;

    private:
        time_slots_state *time_slots;
        time_slots_state::data_t initial_time_slots = time_slots->data();
    };
}

#endif//STRATEGR_RESIZEOPERATION_H
