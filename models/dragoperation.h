//
// Created by Dmitry Khrykin on 2019-07-29.
//

#ifndef STRATEGR_DRAGOPERATION_H
#define STRATEGR_DRAGOPERATION_H

#include <vector>
#include <map>
#include <tuple>

#include <iostream>
#include "timeslotsstate.h"
#include "session.h"

namespace stg {
    class drag_operation {
    public:
        using index_t = time_slots_state::index_t;
        using old_index = index_t;
        using new_index = index_t;
        using current_index = index_t;

        using indices_vector = std::vector<index_t>;
        using initial_and_previous_indices = indices_vector;

        using history_state = std::map<current_index, initial_and_previous_indices>;
        using movements_state = std::map<old_index, new_index>;

        using indices_cache_entry = std::tuple<old_index, activity *>;
        using indices_cache = std::vector<indices_cache_entry>;

        explicit drag_operation(time_slots_state *time_slots,
                                indices_vector initial_indices);

        void record_drag(const std::vector<time_slot> &time_slots_to_drag,
                         int distance);

        bool state_changed();

        time_slots_state::data_t &initial_state();
    private:
        static const unsigned int initial_index_key = 0;

        struct indices_range {
            indices_range(index_t frst, index_t lst);

            index_t first = 0;
            index_t last = 0;

            index_t size() const;

            friend std::ostream &operator<<(std::ostream &os,
                                            const indices_range &r) {
                os << "[" << r.first << ", " << r.last << "]";
                return os;
            }
        };

        time_slots_state *time_slots;
        time_slots_state::data_t initial_time_slots = time_slots->data();

        indices_vector initial_dragged_indices;

        history_state history = history_state();

        indices_vector silently_drag(const indices_range &range_to_drag,
                                     int distance);

        void restore_cache(const indices_range &restore_cache_range,
                           const indices_cache &cache,
                           movements_state &movements) const;

        std::tuple<indices_range, indices_range, indices_range>
        get_ranges(const indices_range &indices_to_drag,
                   index_t destination_index,
                   int distance) const;

        indices_range get_new_dragging_indices(const indices_range &dragging_indices,
                                               index_t destination_index,
                                               int distance) const;

        indices_range get_cache_range(const indices_range &dragging_indices,
                                      index_t destination_index,
                                      int distance) const;

        indices_cache make_cache(indices_range cache_indices) const;

        void apply_movements_to_history(const movements_state &movements);

        void invalidate_drag(const indices_vector &new_dragged_indices);

        indices_range find_session_range_for(index_t time_slot_index);
        index_t find_avaliable_movement_index(indices_range session_range,
                                              index_t target_index);

        index_t get_initial(index_t index);

        static void print_indices(const std::string &name,
                                  const indices_vector &indices_state);


        void print_history(const std::string &name,
                           const history_state &history_state);
        static void print_movements(const movements_state &movements);
    };
}
#endif //STRATEGR_DRAGOPERATION_H
