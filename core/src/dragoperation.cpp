//
// Created by Dmitry Khrykin on 2019-07-29.
//

#include "dragoperation.h"
#include "strategy.h"

namespace stg {
    drag_operation::drag_operation(time_slots_state *time_slots, indices_vector initial_indices)
        : time_slots(time_slots),
          initial_dragged_indices(std::move(initial_indices)) {
    }

    auto drag_operation::record_drag(const std::vector<time_slot> &time_slots_to_drag,
                                     int distance) -> std::vector<index_t> {
        if (distance == 0) {
            return {};
        }

        auto range_to_drag = indices_range{*time_slots->index_of(time_slots_to_drag.front()),
                                           *time_slots->index_of(time_slots_to_drag.back())};

        // Drag operation_type is divided into two phases:
        // 1. Drag selected slots to their new positions, switching the nearby slots;
        auto new_dragged_indices = silently_drag(range_to_drag, distance);
        // 2. Try to restore nearby sessions' initial positions.
        validate_drag(new_dragged_indices);

        return new_dragged_indices;
    }

    auto drag_operation::silently_drag(const indices_range &range_to_drag,
                                       int distance) -> indices_vector {
        auto destination_index = distance < 0
                                     ? range_to_drag.first + distance
                                     : range_to_drag.last + distance;

        if (destination_index > time_slots->size() - 1) {
            return {};
        }

        auto [cache_range, restore_cache_range, new_drag_range] = get_ranges(range_to_drag, destination_index, distance);

        auto cache = make_cache(cache_range);

        movements_state movements;
        indices_vector new_dragged_indices;

        for (auto i = 0; i < range_to_drag.size(); i++) {
            auto insert_at_index = new_drag_range.first + i;
            auto old_index = range_to_drag.first + i;

            movements[old_index] = insert_at_index;

            new_dragged_indices.push_back(insert_at_index);

            auto activity_at_old_index = time_slots->at(old_index).activity;
            time_slots->silently_set_activity_at_index(insert_at_index,
                                                       activity_at_old_index);
        }

        restore_cache(restore_cache_range, cache, movements);

        apply_movements_to_history(movements);

        return new_dragged_indices;
    }

    void drag_operation::restore_cache(const indices_range &restore_cache_range,
                                       const indices_cache &cache,
                                       movements_state &movements) const {
        for (size_t i = 0; i < cache.size(); i++) {
            auto insert_at_index = static_cast<index_t>(restore_cache_range.first + i);
            auto [history_index, activity] = cache[i];

            movements[history_index] = insert_at_index;

            time_slots->silently_set_activity_at_index(insert_at_index, activity);
        }
    }

    auto drag_operation::get_ranges(const indices_range &indices_to_drag,
                                    index_t destination_index,
                                    int distance) const -> ranges_tuple {
        auto cache_range = get_cache_range(indices_to_drag,
                                           destination_index,
                                           distance);

        auto restore_cache_first_index = distance < 0
                                             ? destination_index + indices_to_drag.size()
                                             : indices_to_drag.first;

        auto restore_cache_range = indices_range{restore_cache_first_index,
                                                 restore_cache_first_index + cache_range.size()};

        auto new_drag_range = get_new_dragging_indices(indices_to_drag,
                                                       destination_index,
                                                       distance);

        return std::make_tuple(cache_range, restore_cache_range, new_drag_range);
    }

    auto drag_operation::get_new_dragging_indices(const indices_range &dragging_indices,
                                                  index_t destination_index,
                                                  int distance) -> indices_range {
        auto new_first_index = distance < 0
                                   ? destination_index
                                   : destination_index - dragging_indices.size() + 1;

        return indices_range{new_first_index,
                             new_first_index + dragging_indices.size()};
    }

    auto drag_operation::get_cache_range(const indices_range &dragging_indices,
                                         index_t destination_index,
                                         int distance) -> indices_range {
        return distance < 0
                   ? indices_range{destination_index,
                                   dragging_indices.first - 1}
                   : indices_range{dragging_indices.last + 1,
                                   destination_index};
    }

    auto drag_operation::make_cache(indices_range cache_indices) const -> indices_cache {
        indices_cache cache = {};

        for (auto i = cache_indices.first; i <= cache_indices.last; i++) {
            auto cache_entry = std::make_tuple(i, time_slots->at(i).activity);
            cache.push_back(cache_entry);
        }

        return cache;
    }

    void stg::drag_operation::validate_drag(const indices_vector &new_dragged_indices) {
        if (new_dragged_indices.empty()) {
            return;
        }

        auto nobody_can_move = false;

        while (!nobody_can_move) {
            auto cant_move_count = 0;

            for (auto const &[current_index, past_indices] : history) {
                auto slot_is_dragged = std::find(new_dragged_indices.begin(),
                                                 new_dragged_indices.end(),
                                                 current_index) != new_dragged_indices.end();

                auto slot_is_empty = time_slots->at(current_index).activity == strategy::no_activity;

                if (slot_is_dragged || slot_is_empty) {
                    cant_move_count++;
                    continue;
                }

                auto session_range = find_session_range_for(current_index);

                auto initial_session_begin_index = get_initial(session_range.first);

                if (initial_session_begin_index != session_range.first) {
                    auto can_move_to = find_avaliable_movement_index(session_range,
                                                                     initial_session_begin_index);

                    if (can_move_to != session_range.first) {
                        auto distance = can_move_to - session_range.first;
                        silently_drag(session_range, distance);
                        break;
                    }
                }

                cant_move_count++;
            }

            if (cant_move_count == history.size()) {
                nobody_can_move = true;
            }
        }
    }

    auto drag_operation::get_initial(index_t index) -> index_t {
        return history.count(index)
                   ? history[index][initial_index_key]
                   : index;
    }

    void drag_operation::print_indices(const std::string &name,
                                       const indices_vector &indices_state) {
        std::cout << name << ": [ ";

        for (auto index : indices_state) {
            std::cout << index << " ";
        }

        std::cout << "]" << std::endl;
    }

    void drag_operation::print_history(const std::string &name,
                                       const history_state &history_state) {
        std::cout << name << std::endl;
        for (auto const &[current_index, past_indices] : history_state) {
            if (!past_indices.empty()) {
                print_indices(std::to_string(current_index), past_indices);
            }
        }
    }

    void drag_operation::print_movements(const movements_state &movements) {
        std::cout << "movements:" << std::endl;
        for (auto const &[past_index, current_index] : movements) {
            std::cout << past_index << " -> " << current_index << std::endl;
        }
    }

    void drag_operation::apply_movements_to_history(const movements_state &movements) {
        history_state new_history_stacks;

        for (auto const &[past_index, current_index] : movements) {
            auto new_history_stack = history[past_index];
            new_history_stack.push_back(past_index);

            new_history_stacks[current_index] = new_history_stack;
        }

        for (auto const &[index, stack] : new_history_stacks) {
            if (index == stack.front()) {
                history.erase(index);
            } else {
                indices_vector history_entry{stack.front()};

                // history entry doesn't need more than two values:
                // only initial index and previous index if they're different
                if (stack.back() != stack.front()) {
                    history_entry.push_back(stack.back());
                }

                history[index] = history_entry;
            }
        }
    }

    auto drag_operation::find_session_range_for(index_t time_slot_index) -> indices_range {
        if (time_slot_index < 0)
            time_slot_index = 0;
        if (time_slot_index > time_slots->size() - 1) {
            time_slot_index = time_slots->size() - 1;
        }

        auto begin_index = time_slot_index;
        auto end_index = time_slot_index;

        for (auto i = time_slot_index; i > 0; i--) {
            if (time_slots->at(i).activity == time_slots->at(time_slot_index).activity) {
                begin_index = i;
            } else {
                break;
            }
        }

        for (auto i = time_slot_index; i < time_slots->size(); i++) {
            if (time_slots->at(i).activity == time_slots->at(time_slot_index).activity) {
                end_index = i;
            } else {
                break;
            }
        }

        return indices_range{begin_index, end_index};
    }

    auto drag_operation::find_avaliable_movement_index(indices_range session_range,
                                                       index_t target_index) -> index_t {
        index_t result = session_range.first;

        auto wants_up = session_range.first > target_index;
        auto wants_down = session_range.first < target_index;

        if (wants_up) {
            if (session_range.first == 0) {
                return result;
            }

            for (auto i = session_range.first - 1; i >= target_index; i--) {
                if (time_slots->at(i).activity == strategy::no_activity ||
                    time_slots->at(i).activity == time_slots->at(session_range.first).activity) {
                    result = i;
                } else {
                    break;
                }
            }
        }

        if (wants_down) {
            if (session_range.last == time_slots->size() - 1 ||
                target_index + session_range.size() >= time_slots->size()) {
                return result;
            }

            for (auto i = session_range.last + 1; i <= target_index + session_range.size() - 1; i++) {
                if (time_slots->at(i).activity == strategy::no_activity ||
                    time_slots->at(i).activity == time_slots->at(session_range.last).activity) {
                    result = i - session_range.size() + 1;
                } else {
                    break;
                }
            }
        }

        return result;
    }

    auto drag_operation::state_changed() -> bool {
        return time_slots->data() != initial_time_slots;
    }

    auto drag_operation::initial_state() -> time_slots_state::data_t & {
        return initial_time_slots;
    }

    drag_operation::indices_range::indices_range(index_t frst, index_t lst) {
        first = frst < 0 ? 0 : frst;
        last = lst < 0 ? 0 : lst;
    }

    auto drag_operation::indices_range::size() const -> index_t {
        return last - first + 1;
    }
}