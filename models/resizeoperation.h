//
// Created by Dmitry Khrykin on 2019-08-01.
//

#ifndef STRATEGR_RESIZEOPERATION_H
#define STRATEGR_RESIZEOPERATION_H

#include "timeslotsstate.h"

namespace stg {
    class resize_operation {
    public:
        explicit resize_operation(time_slots_state *time_slots);

        void fill_slots(time_slots_state::index_t from_index,
                        time_slots_state::index_t till_index);

        void fill_slots_shifting(time_slots_state::index_t from_index,
                                 time_slots_state::index_t till_index);

        bool state_changed();
    private:
        time_slots_state *time_slots;
        time_slots_state::data_t initial_time_slots = time_slots->data();
    };
}

#endif //STRATEGR_RESIZEOPERATION_H
