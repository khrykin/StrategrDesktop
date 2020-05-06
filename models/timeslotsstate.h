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
    class strategy;
    class sessions_calculator;
    class drag_operation;
    class resize_operation;

    using time_slots_state_base = private_list<time_slot>;
    class time_slots_state :
            public time_slots_state_base,
            public notifiable_on_change,
            public streamable_list<time_slots_state> {
    public:
        using time_t = time_slot::time_t;
        using duration_t = time_slot::duration_t;
        using size_t = int;

        auto next_slot_empty(index_t index) const -> bool;
        auto previous_slot_empty(index_t index) const -> bool;

        auto begin_time() const -> time_t;
        auto end_time() const -> time_t;
        auto slot_duration() const -> duration_t;
        auto number_of_slots() const -> size_t;

        auto has_activity(const activity *activity) -> bool;

        auto times() const -> std::vector<time_t>;
    private:
        friend strategy;
        friend sessions_calculator;
        friend drag_operation;
        friend resize_operation;

        time_t _begin_time = 0;
        duration_t _slot_duration = 0;

        time_slots_state(time_t start_time,
                         duration_t slot_duration,
                         size_t number_of_slots);

        // NB! you can't create time_slots_state from empty vector,
        // since there would be no way to find out slot_duration
        // and begin_time
        explicit time_slots_state(std::vector<time_slot> from_vector);

        auto operator=(const time_slots_state &new_state) -> time_slots_state &;

        void set_begin_time(time_t begin_time);
        void set_slot_duration(duration_t slot_duration);
        void set_number_of_slots(size_t new_number_of_slots);
        void set_end_time(time_t end_time);
        void set_activity_at_indices(activity *activity,
                                     const std::vector<index_t> &indices);

        void silently_set_activity_at_indices(activity *activity, const std::vector<index_t> &indices);
        void silently_set_activity_at_index(index_t slot_index, activity *activity);
        void silently_fill_slots(index_t from_index, index_t till_index);
        void fill_slots(index_t from_index, index_t till_index);
        void fill_slots_shifting(index_t from_index, index_t till_index);
        void shift_below(index_t from_index, size_t length);
        void copy_slots(index_t from_index, index_t till_index, index_t destination_index);
        void populate(time_t start_time, size_t number_of_slots);

        auto find_slot_with_activity(const activity *activity) -> iterator;
        void remove_activity(activity *activity);
        void edit_activity(activity *old_activity, activity *new_activity);

        void swap(index_t first_index, index_t second_index);
        void silently_swap(index_t first_index, index_t second_index);

        auto at(index_t index) -> const time_slot &;

        auto first_activity_in_time_window(time_t begin_time, time_t end_time) const -> activity *;
        auto slots_in_time_window(time_slots_state::time_t begin_time,
                                  time_slots_state::time_t end_time,
                                  bool round) -> std::vector<time_slot *>;

        auto make_slot_begin_time(time_t global_begin_time, index_t slot_index) const -> time_t;
        void update_begin_times();
        void reset_times();

        void make_safe_index(index_t &index);
        void reset_with(data_t raw_data) override;

        auto class_print_name() const -> std::string override;
    };
}

#endif //MODELS_TIMESLOTSSTATE_H
