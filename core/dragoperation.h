//
// Created by Dmitry Khrykin on 2019-07-29.
//

#ifndef STRATEGR_DRAGOPERATION_H
#define STRATEGR_DRAGOPERATION_H

#include <map>
#include <tuple>
#include <vector>

#include "session.h"
#include "timeslotsstate.h"
#include <iostream>

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

        auto record_drag(const std::vector<time_slot> &time_slots_to_drag,
                         int distance) -> std::vector<index_t>;

        auto state_changed() -> bool;

        auto initial_state() -> time_slots_state::data_t &;

    private:
        struct indices_range {
            indices_range(index_t frst, index_t lst);

            index_t first = 0;
            index_t last = 0;

            auto size() const -> index_t;

            friend auto operator<<(std::ostream &os,
                                   const indices_range &r) -> std::ostream & {
                os << "[" << r.first << ", " << r.last << "]";
                return os;
            }
        };

        using ranges_tuple = std::tuple<indices_range, indices_range, indices_range>;

        static const unsigned int initial_index_key = 0;

        time_slots_state *time_slots;
        time_slots_state::data_t initial_time_slots = time_slots->_data;

        indices_vector initial_dragged_indices;

        history_state history = history_state();

        auto silently_drag(const indices_range &range_to_drag, int distance) -> indices_vector;

        void restore_cache(const indices_range &restore_cache_range,
                           const indices_cache &cache,
                           movements_state &movements) const;

        auto get_ranges(const indices_range &indices_to_drag,
                        index_t destination_index,
                        int distance) const -> ranges_tuple;

        auto make_cache(indices_range cache_indices) const -> indices_cache;

        void apply_movements_to_history(const movements_state &movements);

        void validate_drag(const indices_vector &new_dragged_indices);

        auto find_session_range_for(index_t time_slot_index) -> indices_range;
        auto find_avaliable_movement_index(indices_range session_range,
                                           index_t target_index) -> index_t;

        auto get_initial(index_t index) -> index_t;

        static auto get_new_dragging_indices(const indices_range &dragging_indices,
                                             index_t destination_index,
                                             int distance) -> indices_range;

        static auto get_cache_range(const indices_range &dragging_indices,
                                    index_t destination_index,
                                    int distance) -> indices_range;

        static void print_history(const std::string &name,
                                  const history_state &history_state);
        static void print_indices(const std::string &name,
                                  const indices_vector &indices_state);
        static void print_movements(const movements_state &movements);
    };
}
#endif//STRATEGR_DRAGOPERATION_H
