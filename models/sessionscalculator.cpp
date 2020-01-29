//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include "sessionscalculator.h"

stg::sessions_calculator::result_t
stg::sessions_calculator::calculate(const time_slots_state &time_slots_state) {
    return sessions_calculator(time_slots_state).calculate();
}

stg::sessions_calculator::
sessions_calculator(const time_slots_state &time_slots_state)
        : time_slots(time_slots_state) {}

stg::sessions_calculator::result_t
stg::sessions_calculator::calculate() {
    result_t result;

    auto number_of_slots = time_slots.number_of_slots();
    for (time_slot_index = 0; time_slot_index < number_of_slots; time_slot_index++) {
        if (current_slot_has_activity()) {
            process_slot_with_activity(result);
        } else {
            process_empty_slot(result);
        }
    }

    return result;
}

void stg::sessions_calculator::
process_slot_with_activity(result_t &result) {
    if (activity_changed()) {
        push_cached_session_if_exists(result);
        reset_cache_with_default();
    } else {
        push_current_slot_to_cache();
    }

    if (current_slot_is_last()) {
        push_cached_session_if_exists(result);
    }
}

void stg::sessions_calculator::
process_empty_slot(result_t &result) {
    push_cached_session_if_exists(result);
    push_default_session(result);

    cached_session = std::nullopt;
}

stg::session stg::sessions_calculator::default_session() {
    auto &time_slot = time_slots[time_slot_index];

    auto default_activity_session = session();
    default_activity_session.activity = time_slot.activity;
    default_activity_session.time_slots.push_back(time_slot);

    return default_activity_session;
}

const stg::activity *stg::sessions_calculator::current_activity() {
    return current_time_slot().activity;
}

const stg::activity *stg::sessions_calculator::previous_activity() {
    return time_slot_index > 0
           ? time_slots[time_slot_index - 1].activity
           : no_activity;
}

const stg::time_slot &stg::sessions_calculator::current_time_slot() {
    return time_slots[time_slot_index];
}

bool stg::sessions_calculator::current_slot_is_last() {
    return time_slot_index == time_slots.number_of_slots() - 1;
}

bool stg::sessions_calculator::current_slot_has_activity() {
    return current_activity() != no_activity;
}

void stg::sessions_calculator::push_cached_session_if_exists(result_t &result) {
    if (cached_session) {
        result.push_back(*cached_session);
    }
}

void stg::sessions_calculator::reset_cache_with_default() {
    cached_session = default_session();
}

void stg::sessions_calculator::push_current_slot_to_cache() {
    if (cached_session) {
        cached_session->time_slots.push_back(current_time_slot());
    }
}

void stg::sessions_calculator::push_default_session(result_t &result) {
    result.push_back(default_session());
}

bool stg::sessions_calculator::activity_changed() {
    return !previous_activity() || previous_activity() != current_activity();
}
