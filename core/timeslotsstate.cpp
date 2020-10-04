//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include <algorithm>
#include <cassert>
#include <numeric>

#include "timeslotsstate.h"

namespace stg {
    auto time_slots_state::begin_time() const -> minutes {
        return _begin_time;
    }

    void time_slots_state::set_begin_time(minutes begin_time) {
        if (begin_time == _begin_time)
            return;

        auto old_begin_time = _begin_time;
        auto old_data = _data;

        _begin_time = begin_time;

        auto new_end_time = end_time();
        if (begin_time >= end_time()) {
            new_end_time += 24 * 60;
        }

        auto new_number_of_slots = (new_end_time - begin_time) / slot_duration();

        _data.resize(new_number_of_slots, time_slot(0, slot_duration()));

        update_begin_times();

        int slot_difference = ((int) begin_time - (int) old_begin_time) / (int) slot_duration();

        for (const auto &slot : _data) {
            auto slot_index = &slot - &_data[0];
            auto old_index = slot_index + slot_difference;

            stg::activity *activity = time_slot::no_activity;
            if (old_index >= 0 && old_index < old_data.size()) {
                const auto &old_slot = old_data[old_index];
                activity = old_slot.activity;
            }

            _data[slot_index].activity = activity;
        }

        on_change_event();
    }

    void time_slots_state::update_begin_times() {
        for (auto slot_index = 0; slot_index < number_of_slots(); slot_index++) {
            _data[slot_index].begin_time = make_slot_begin_time(_begin_time, slot_index);
        }
    }

    auto time_slots_state::slot_duration() const -> minutes {
        return _slot_duration;
    }

    void time_slots_state::set_slot_duration(minutes slot_duration) {
        if (slot_duration == _slot_duration)
            return;

        auto old_state = time_slots_state(_data);

        _slot_duration = slot_duration;

        auto new_number_of_slots = (end_time() - begin_time()) / slot_duration;

        _data.resize(new_number_of_slots, time_slot(0, slot_duration));

        for (auto &slot : _data) {
            auto slot_index = &slot - &_data[0];
            slot.duration = slot_duration;
            slot.begin_time = make_slot_begin_time(begin_time(), (index_t) slot_index);

            slot.activity = old_state.first_activity_in_time_window(slot.begin_time, slot.end_time());
        }

        on_change_event();
    }

    auto time_slots_state::number_of_slots() const -> time_slots_state::size_t {
        return (time_slots_state::size_t) _data.size();
    }

    void time_slots_state::set_number_of_slots(size_t new_number_of_slots) {
        if (new_number_of_slots == number_of_slots()) {
            return;
        }

        if (new_number_of_slots < number_of_slots()) {
            _data.erase(_data.begin() + new_number_of_slots, _data.end());
        } else {
            populate(last().end_time(), new_number_of_slots - number_of_slots());
        }

        on_change_event();
    }

    time_slots_state::time_slots_state(minutes start_time,
                                       minutes slot_duration,
                                       size_t number_of_slots)
        : _begin_time(start_time),
          _slot_duration(slot_duration) {
        populate(start_time, number_of_slots);
    }

    time_slots_state::time_slots_state(std::vector<time_slot> from_vector) {
        assert(!from_vector.empty() && "Can't create time slots from empty vector");

        _data = std::move(from_vector);
        reset_times();
    }

    void time_slots_state::set_end_time(minutes end_time) {
        if (end_time == this->end_time())
            return;

        if (end_time <= begin_time())
            end_time += 24 * 60;

        auto number_of_slots = (end_time - begin_time()) / slot_duration();

        if (number_of_slots < 1)
            number_of_slots = 1;

        set_number_of_slots(number_of_slots);
    }

    auto time_slots_state::end_time() const -> minutes {
        return last().end_time();
    }

    void time_slots_state::reset_times() {
        auto &first_slot = _data.front();
        _begin_time = first_slot.begin_time;
        _slot_duration = first_slot.duration;
    }

    void time_slots_state::silently_fill_slots(index_t from_index, index_t till_index) {
        auto source_index = from_index;

        if (till_index < from_index) {
            std::swap(till_index, from_index);
        }

        make_safe_index(till_index);
        make_safe_index(from_index);

        auto *source_activity = has_index(source_index)
                                    ? _data[source_index].activity
                                    : time_slot::no_activity;

        for (auto i = from_index; i <= till_index; i++) {
            _data[i].activity = source_activity;
        }
    }

    void time_slots_state::fill_slots(index_t from_index, index_t till_index) {
        silently_fill_slots(from_index, till_index);

        on_change_event();
    }

    void time_slots_state::fill_slots_shifting(index_t from_index, index_t till_index) {
        auto result = _data;

        if (from_index == till_index)
            return;

        auto *source_activity = has_index(from_index)
                                    ? _data[from_index].activity
                                    : time_slot::no_activity;

        auto activities_not_equal = [&](const auto &v) {
            return v.activity != source_activity;
        };

        if (till_index > from_index) {
            auto movable_begin = from_index < 0
                                     ? _data.begin()
                                     : std::find_if(_data.begin() + from_index,
                                                    std::next(_data.begin() + till_index),
                                                    activities_not_equal);
            if (movable_begin != _data.end()) {
                auto movable_begin_index = std::distance(_data.begin(), movable_begin);
                auto move_to_index = till_index + 1;

                if (move_to_index < _data.size()) {
                    auto move_distance = move_to_index - movable_begin_index;
                    auto movable_last_index = _data.size() - 1 - move_distance;

                    for (auto i = movable_begin_index; i <= movable_last_index; i++) {
                        result[i + move_distance].activity = _data[i].activity;
                    }
                }
            }
        } else {
            auto movable_rbegin = from_index >= _data.size()
                                      ? _data.rbegin()
                                      : std::find_if(_data.rbegin() + (_data.size() - 1 - from_index),
                                                     std::next(_data.rbegin() + (_data.size() - 1 - till_index)),
                                                     activities_not_equal);

            if (movable_rbegin != _data.rend()) {
                auto movable_last_index = std::distance(movable_rbegin, _data.rend()) - 1;
                auto move_to_index = till_index - 1;

                if (move_to_index >= 0) {
                    auto move_distance = movable_last_index - move_to_index;
                    auto movable_begin_index = move_distance;

                    for (auto i = movable_begin_index; i <= movable_last_index; i++) {
                        result[i - move_distance].activity = _data[i].activity;
                    }
                }
            }
        }

        make_safe_index(till_index);
        make_safe_index(from_index);

        if (till_index < from_index)
            std::swap(till_index, from_index);

        for (auto i = from_index; i <= till_index; i++) {
            result[i].activity = source_activity;
        }

        _data = result;

        on_change_event();
    }


    void time_slots_state::silently_set_activity_at_index(index_t slot_index, activity *activity) {
        if (!has_index(slot_index)) {
            return;
        }

        _data[slot_index].activity = activity;
    }

    void time_slots_state::set_activity_at_indices(activity *activity,
                                                   const std::vector<index_t> &indices) {
        auto activity_changed = false;
        for (auto slot_index : indices) {
            auto current_activity_changed = _data[slot_index].activity != activity;
            if (current_activity_changed) {
                silently_set_activity_at_index(slot_index, activity);
            }

            if (!activity_changed && current_activity_changed) {
                activity_changed = true;
            }
        }

        if (activity_changed) {
            on_change_event();
        }
    }


    void time_slots_state::silently_set_activity_at_indices(activity *activity,
                                                            const std::vector<index_t> &indices) {
        for (auto slot_index : indices) {
            silently_set_activity_at_index(slot_index, activity);
        }
    }


    auto time_slots_state::class_print_name() const -> std::string {
        return "time_slots_state";
    }

    void time_slots_state::populate(minutes start_time, size_t number_of_slots) {
        for (auto slot_index = 0; slot_index < number_of_slots; ++slot_index) {
            auto begin_time = make_slot_begin_time(start_time, slot_index);

            auto time_slot = stg::time_slot(begin_time, _slot_duration);
            _data.emplace_back(time_slot);
        }
    }

    auto time_slots_state::make_slot_begin_time(minutes global_begin_time,
                                                index_t slot_index) const -> minutes {
        return global_begin_time + slot_index * _slot_duration;
    }

    auto time_slots_state::has_activity(const activity *activity) const -> bool {
        return std::find_if(_data.begin(), _data.end(), [activity](const auto &time_slot) {
                   return time_slot.activity == activity;
               }) != _data.end();
    }

    void time_slots_state::remove_activity(activity *activity) {
        edit_activity(activity, time_slot::no_activity);
    }

    void time_slots_state::edit_activity(activity *old_activity,
                                         activity *new_activity) {
        auto activity_changed = false;
        for (auto &time_slot : _data) {
            if (time_slot.activity == old_activity) {
                activity_changed = old_activity != new_activity;
                time_slot.activity = new_activity;
            }
        }

        if (activity_changed) {
            on_change_event();
        }
    }

    auto time_slots_state::operator=(const time_slots_state &new_state) -> time_slots_state & {
        if (this == &new_state)
            return *this;

        _data = new_state._data;
        _begin_time = new_state._begin_time;
        _slot_duration = new_state._slot_duration;

        on_change_event();

        return *this;
    }

    void time_slots_state::shift_below(index_t from_index,
                                       size_t length) {
        auto actual_number_of_slots = number_of_slots();

        _data.insert(_data.begin() + from_index, length, time_slot(0, slot_duration()));
        _data.resize(actual_number_of_slots, time_slot(0, 0));

        update_begin_times();

        on_change_event();
    }

    void time_slots_state::copy_slots(index_t from_index, index_t till_index, index_t destination_index) {
        data_t copied_slots;
        std::copy(_data.begin() + from_index,
                  _data.begin() + till_index,
                  std::back_inserter(copied_slots));

        auto copied_length = till_index - from_index;
        auto destination_end_index = destination_index + copied_length;
        if (destination_end_index > _data.size() - 1)
            destination_end_index = (index_t) _data.size() - 1;

        auto index_in_copied = 0;
        std::for_each(_data.begin() + destination_index,
                      _data.begin() + destination_end_index,
                      [&index_in_copied, copied_slots](auto &slot) {
                          slot.activity = copied_slots[index_in_copied].activity;
                          index_in_copied++;
                      });

        on_change_event();
    }

    void time_slots_state::swap(index_t first_index, index_t second_index) {
        silently_swap(first_index, second_index);
        on_change_event();
    }

    void time_slots_state::silently_swap(index_t first_index,
                                         index_t second_index) {
        auto *activity = _data[first_index].activity;

        _data[first_index].activity = _data[second_index].activity;
        _data[second_index].activity = activity;
    }

    auto time_slots_state::at(index_t index) -> const time_slot & {
        return _data.at(index);
    }

    auto time_slots_state::next_slot_empty(index_t index) const -> bool {
        if (index >= (index_t) _data.size() - 1) {
            return true;
        } else {
            return _data[index + 1].empty();
        }
    }

    auto time_slots_state::previous_slot_empty(index_t index) const -> bool {
        if (index <= 0) {
            return true;
        } else {
            return _data[index - 1].empty();
        }
    }

    void time_slots_state::reset_with(data_t raw_data) {
        time_slots_state_base::reset_with(raw_data);
        reset_times();
    }

    auto time_slots_state::make_ruler_times() const -> std::vector<std::time_t> {
        if (empty())
            return {};

        std::vector<std::time_t> result;

        for (const auto &slot : _data) {
            result.push_back(slot.calendar_begin_time());
        }

        result.push_back(_data.back().calendar_end_time());

        return result;
    }

    void time_slots_state::make_safe_index(index_t &index) {
        if (empty())
            index = 0;
        else if (index < 0)
            index = 0;
        else if (index >= size())
            index = size() - 1;
    }

    auto time_slots_state::first_activity_in_time_window(minutes begin_time,
                                                         minutes end_time) const -> activity * {
        assert(end_time >= begin_time && "end_time must be greater than begin_time");

        auto range = end_time - begin_time;

        auto it = std::find_if(_data.begin(), _data.end(), [=](const time_slot &slot) {
            auto slot_overlaps_with_range = slot.begin_time >= begin_time && slot.end_time() >= begin_time;

            auto found = range >= slot_duration()
                             ? (slot.begin_time >= begin_time && slot.end_time() <= end_time) || slot_overlaps_with_range
                             : (slot.begin_time <= begin_time && slot.end_time() >= end_time) || slot_overlaps_with_range;

            return found;
        });

        return it != _data.end() ? it->activity : nullptr;
    }

    auto time_slots_state::slots_in_time_window(minutes begin_time,
                                                minutes end_time) -> std::vector<time_slot *> {
        assert(end_time >= begin_time && "end_time must be greater than begin_time");

        std::vector<time_slot *> result;

        for (auto &slot : _data) {
            auto fits_inside = slot.begin_time >= begin_time && slot.end_time() <= end_time;

            auto overlaps_with_beginning = begin_time >= slot.begin_time && begin_time <= slot.end_time() && (float) (slot.end_time() - begin_time) >= 0.5f * slot_duration();

            auto overlaps_with_end = end_time >= slot.begin_time && end_time <= slot.end_time() && (float) (end_time - slot.begin_time) >= 0.5f * slot_duration();

            if (fits_inside || overlaps_with_end || overlaps_with_beginning) {
                result.push_back(&slot);
            }

            if (slot.end_time() > end_time)
                break;

            if (slot.end_time() < begin_time)
                continue;
        }

        return result;
    }

    void time_slots_state::on_change_event() const {
        if (_ruler_times.empty() && !on_ruler_change) {
            notifiable_on_change::on_change_event();

            return;
        }

        auto new_calendar_times = make_ruler_times();
        bool times_changed = new_calendar_times != _ruler_times;

        if (times_changed) {
            _ruler_times = new_calendar_times;
        }

        notifiable_on_change::on_change_event();

        if (times_changed && on_ruler_change)
            on_ruler_change();
    }

    auto time_slots_state::ruler_times() const -> const std::vector<std::time_t> & {
        if (_ruler_times.empty()) {
            _ruler_times = make_ruler_times();
        }

        return _ruler_times;
    }

    void time_slots_state::add_on_ruler_change_callback(const std::function<void()> &callback) const {
        auto old_on_ruler_change = on_ruler_change;

        on_ruler_change = [=] {
            if (old_on_ruler_change)
                old_on_ruler_change();

            callback();
        };
    }

    auto time_slots_state::duration_for_activity(const activity *activity) const -> minutes {
        return std::accumulate(_data.begin(), _data.end(), 0, [activity](minutes duration, const time_slot &time_slot) {
            auto duration_in_slot = time_slot.activity == activity ? time_slot.duration : 0;
            return duration + duration_in_slot;
        });
    }
}
