//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef MODELS_ACTIVITYSESSIONSCALCULATOR_H
#define MODELS_ACTIVITYSESSIONSCALCULATOR_H

#include "session.h"
#include "timeslotsstate.h"

namespace stg {
    class time_slots_state;

    class sessions_calculator {
    public:
        using result_t = std::vector<session>;
        static constexpr auto no_activity = time_slot::no_activity;
        static result_t calculate(const time_slots_state &time_slots_state);

    private:
        using index_t = time_slots_state::index_t;

        const time_slots_state &time_slots;
        index_t time_slot_index = 0;

        explicit sessions_calculator(const time_slots_state &time_slots_state);

        std::optional<session> cached_session;

        result_t calculate();

        void process_slot_with_activity(result_t &result);
        void process_empty_slot(result_t &result);

        session default_session();

        const activity *current_activity();
        const activity *previous_activity();

        const time_slot &current_time_slot();

        bool current_slot_is_last();
        bool current_slot_has_activity();
        bool activity_changed();

        void push_cached_session_if_exists(result_t &result);
        void push_current_slot_to_cache();
        void push_default_session(result_t &result);

        void reset_cache_with_default();
    };
}
#endif //MODELS_ACTIVITYSESSIONSCALCULATOR_H
