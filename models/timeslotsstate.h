//
// Created by Dmitry Khrykin on 2019-07-05.
//

#ifndef MODELS_TIMESLOTSSTATE_H
#define MODELS_TIMESLOTSSTATE_H

#include <vector>

#include "timeslot.h"
#include "privatelist.h"
#include "notifiableonchange.h"
#include "streamablelist.h"

namespace stg {
    using time_slots_state_base = private_list<time_slot>;
    class time_slots_state :
            public time_slots_state_base,
            public notifiable_on_change,
            public streamable_list<time_slots_state> {
    public:
        using time_t = time_slot::time_t;
        using duration_t = time_slot::duration_t;
        using size_t = int;

        time_slots_state(time_t start_time,
                         duration_t slot_duration,
                         size_t number_of_slots);

        // nb! you can't create time_slots_state from empty vector,
        // since there would be no way to find out slot_duration
        // and begin_time
        explicit time_slots_state(std::vector<time_slot> from_vector);

        time_t begin_time() const;
        void set_begin_time(time_t begin_time);

        duration_t slot_duration() const;
        void set_slot_duration(duration_t slot_duration);

        size_t number_of_slots() const;
        void set_number_of_slots(size_t new_number_of_slots);

        void set_activity_at_indices(activity *activity,
                                     const std::vector<index_t> &indices);

        void silently_set_activity_at_indices(activity *activity,
                                              const std::vector<index_t> &indices);

        void silently_set_activity_at_index(index_t slot_index, activity *activity);

        void fill_slots(index_t from_index, index_t till_index);

        void shift_below(index_t from_index, size_t length);

        void populate(time_t start_time, size_t number_of_slots);

        iterator find_slot_with_activity(const activity *activity);

        void remove_activity(activity *activity);

        void edit_activity(activity *old_activity,
                           activity *new_activity);

        void swap(index_t first_index, index_t second_index);
        void silently_swap(index_t first_index, index_t second_index);

        bool next_slot_empty(index_t index) const;
        bool previous_slot_empty(index_t index) const;

        bool has_activity(const activity *activity);

        time_slots_state &operator=(const time_slots_state &new_state);

        std::string class_print_name() const override;

        const time_slot &at(index_t index);
    private:
        friend strategy;

        time_t _begin_time = 0;
        duration_t _slot_duration = 0;

        time_t slot_begin_time(time_t global_begin_time, index_t slot_index);
        void update_begin_times();
    };
}

#endif //MODELS_TIMESLOTSSTATE_H
