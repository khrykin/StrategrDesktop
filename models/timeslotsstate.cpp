//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include <algorithm>
#include <cassert>

#include "timeslotsstate.h"

stg::time_slots_state::time_t stg::time_slots_state::begin_time() const {
    return _begin_time;
}

void stg::time_slots_state::set_begin_time(time_t begin_time) {
    if (begin_time == _begin_time)
        return;

    _begin_time = begin_time;

    auto max_number_of_slots = 24 * 60 / slot_duration();
    while (number_of_slots() > max_number_of_slots) {
        _data.pop_back();
    }

    update_begin_times();

    on_change_event();
}

void stg::time_slots_state::update_begin_times() {
    for (auto slot_index = 0; slot_index < number_of_slots(); slot_index++) {
        _data[slot_index].begin_time
                = make_slot_begin_time(_begin_time, slot_index);
    }
}

stg::time_slots_state::duration_t stg::time_slots_state::slot_duration() const {
    return _slot_duration;
}

void stg::time_slots_state::set_slot_duration(duration_t slot_duration) {
    if (slot_duration == _slot_duration)
        return;

    _slot_duration = slot_duration;

    auto desired_number_of_slots = (end_time() - begin_time()) / slot_duration;
    while (number_of_slots() > desired_number_of_slots) {
        _data.pop_back();
    }

    while (number_of_slots() < desired_number_of_slots) {
        _data.push_back(time_slot(0, slot_duration));
    }

    for (auto slot_index = 0; slot_index < number_of_slots(); slot_index++) {
        auto &slot = _data[slot_index];
        slot.duration = slot_duration;
        slot.begin_time = make_slot_begin_time(begin_time(), slot_index);
    }

    on_change_event();
}

stg::time_slots_state::size_t stg::time_slots_state::number_of_slots() const {
    return static_cast<time_slots_state::size_t>(_data.size());
}

void stg::time_slots_state::set_number_of_slots(size_t new_number_of_slots) {
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

stg::time_slots_state::time_slots_state(time_t start_time,
                                        duration_t slot_duration,
                                        size_t number_of_slots)
        : _begin_time(start_time),
          _slot_duration(slot_duration) {
    populate(start_time, number_of_slots);
}

stg::time_slots_state::time_slots_state(std::vector<time_slot> from_vector) {
    assert(!from_vector.empty() && "Can't create time slots from empty vector");

    _data = std::move(from_vector);
    reset_times();
}

void stg::time_slots_state::set_end_time(time_t end_time) {
    if (end_time <= begin_time()) {
        end_time += 24 * 60;
    }

    auto number_of_slots = (end_time - begin_time()) / slot_duration();
    set_number_of_slots(number_of_slots);
}

auto stg::time_slots_state::end_time() const -> time_t {
    return last().end_time();
}

void stg::time_slots_state::reset_times() {
    auto first_slot = _data.front();
    _begin_time = first_slot.begin_time;
    _slot_duration = first_slot.duration;
}

void stg::time_slots_state::silently_fill_slots(index_t from_index, index_t till_index) {
    auto source_index = from_index;

    if (till_index < from_index) {
        std::swap(till_index, from_index);
    }

    make_safe(till_index);
    make_safe(from_index);

    auto source_activity = has_index(source_index)
                           ? _data[source_index].activity
                           : time_slot::no_activity;

    for (auto i = from_index; i <= till_index; i++) {
        _data[i].activity = source_activity;
    }
}

void stg::time_slots_state::fill_slots(index_t from_index, index_t till_index) {
    silently_fill_slots(from_index, till_index);

    on_change_event();
}

void stg::time_slots_state::fill_slots_shifting(index_t from_index, index_t till_index) {
    auto result = _data;

    if (from_index == till_index)
        return;

    auto source_activity = has_index(from_index)
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

    make_safe(till_index);
    make_safe(from_index);

    if (till_index < from_index)
        std::swap(till_index, from_index);

    for (auto i = from_index; i <= till_index; i++) {
        result[i].activity = source_activity;
    }

    _data = result;

    on_change_event();
}


void stg::time_slots_state::silently_set_activity_at_index(index_t slot_index, activity *activity) {
    if (!has_index(slot_index)) {
        return;
    }

    _data[slot_index].activity = activity;
}

void stg::time_slots_state::set_activity_at_indices(activity *activity,
                                                    const std::vector<index_t> &indices) {
    auto activity_changed = false;
    for (auto slot_index: indices) {
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


void stg::time_slots_state::silently_set_activity_at_indices(activity *activity,
                                                             const std::vector<index_t> &indices) {
    for (auto slot_index: indices) {
        silently_set_activity_at_index(slot_index, activity);
    }
}


std::string stg::time_slots_state::class_print_name() const {
    return "time_slots_state";
}

void stg::time_slots_state::populate(time_t start_time, size_t number_of_slots) {
    for (auto slot_index = 0; slot_index < number_of_slots; ++slot_index) {
        auto begin_time = make_slot_begin_time(start_time, slot_index);

        auto time_slot = stg::time_slot(begin_time, _slot_duration);
        _data.emplace_back(time_slot);
    }
}

stg::time_slots_state::time_t
stg::time_slots_state::make_slot_begin_time(time_t global_begin_time, index_t slot_index) {
    return global_begin_time + slot_index * _slot_duration;
}

bool stg::time_slots_state::has_activity(const activity *activity) {
    return find_slot_with_activity(activity) != _data.end();
}

stg::time_slots_state::iterator
stg::time_slots_state::find_slot_with_activity(const activity *activity) {
    return std::find_if(_data.begin(),
                        _data.end(),
                        [activity](const auto &time_slot) {
                            return time_slot.activity == activity;
                        });
}

void stg::time_slots_state::remove_activity(activity *activity) {
    edit_activity(activity, time_slot::no_activity);
}

void stg::time_slots_state::edit_activity(activity *old_activity,
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

stg::time_slots_state &stg::time_slots_state::operator=(const time_slots_state &new_state) {
    _data = new_state._data;
    _begin_time = new_state._begin_time;
    _slot_duration = new_state._slot_duration;

    on_change_event();

    return *this;
}

void stg::time_slots_state::shift_below(index_t from_index,
                                        size_t length) {
    auto actual_number_of_slots = number_of_slots();

    _data.insert(_data.begin() + from_index, length, time_slot(0, slot_duration()));
    _data.resize(actual_number_of_slots, time_slot(0, 0));

    update_begin_times();

    on_change_event();
}


void stg::time_slots_state::swap(index_t first_index, index_t second_index) {
    silently_swap(first_index, second_index);
    on_change_event();
}

void stg::time_slots_state::silently_swap(index_t first_index,
                                          index_t second_index) {
    auto activity = _data[first_index].activity;

    _data[first_index].activity = _data[second_index].activity;
    _data[second_index].activity = activity;
}

const stg::time_slot &stg::time_slots_state::at(index_t index) {
    return _data.at(index);
}

bool stg::time_slots_state::next_slot_empty(index_t index) const {
    if (index >= static_cast<index_t>(_data.size() - 1)) {
        return true;
    } else {
        return _data[index + 1].empty();
    }
}

bool stg::time_slots_state::previous_slot_empty(index_t index) const {
    if (index <= 0) {
        return true;
    } else {
        return _data[index - 1].empty();
    }
}

void stg::time_slots_state::reset_with(data_t raw_data) {
    time_slots_state_base::reset_with(raw_data);
    reset_times();
}

std::vector<stg::time_slots_state::time_t> stg::time_slots_state::times() const {
    if (empty())
        return {};

    std::vector<time_t> result;

    for (auto &slot : _data) {
        result.push_back(slot.begin_time);
    }

    result.push_back(_data.back().end_time());

    return result;
}

void stg::time_slots_state::make_safe(index_t &index) {
    if (empty())
        index = 0;
    else if (index < 0)
        index = 0;
    else if (index >= size())
        index = size() - 1;
}


