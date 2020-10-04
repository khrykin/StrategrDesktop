//
// Created by Dmitry Khrykin on 2019-07-07.
//

#ifndef STRATEGR_STRATEGYHISTORY_H
#define STRATEGR_STRATEGYHISTORY_H

#include <optional>
#include <vector>

#include "activitylist.h"
#include "timeslotsstate.h"

namespace stg {
    class strategy_history {
    public:
        struct entry {
            activity_list::data_t activities;
            time_slots_state::data_t time_slots;

            friend auto operator==(const entry &lhs, const entry &rhs) -> bool {
                auto activities_are_equal = std::equal(lhs.activities.begin(),
                                                       lhs.activities.end(),
                                                       rhs.activities.begin(),
                                                       rhs.activities.end(),
                                                       [](const auto &l, const auto &r) {
                                                           return *l == *r;
                                                       });
                return activities_are_equal &&
                       lhs.time_slots == rhs.time_slots;
            }

            friend bool operator!=(const entry &lhs, const entry &rhs) {
                return !(lhs == rhs);
            }
        };

        explicit strategy_history(entry current_state);

        bool commit(const entry &new_state);

        std::optional<entry> undo();
        std::optional<entry> redo();

        bool has_prevoius_state();
        bool has_next_state();

        bool has_prevoius_activities_state();
        bool has_next_activities_state();

    private:
        entry current_state;
        std::vector<entry> undo_stack;
        std::vector<entry> redo_stack;
    };
}

#endif//STRATEGR_STRATEGYHISTORY_H
