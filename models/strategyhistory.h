//
// Created by Dmitry Khrykin on 2019-07-07.
//

#ifndef MODELS_STRATEGYHISTORY_H
#define MODELS_STRATEGYHISTORY_H

#include <vector>
#include <optional>

#include "activitylist.h"
#include "timeslotsstate.h"

class StrategyHistory {
public:
    struct Entry {
        ActivityList activities;
        TimeSlotsState timeSlots;

        friend bool operator==(const Entry &lhs, const Entry &rhs) {
            return lhs.activities == rhs.activities
                   && lhs.timeSlots == rhs.timeSlots;
        }

        friend bool operator!=(const Entry &lhs, const Entry &rhs) {
            return !(lhs == rhs);
        }
    };

    explicit StrategyHistory(Entry currentState);

    void commit(const Entry &newState);

    std::optional<Entry> undo();
    std::optional<Entry> redo();

    bool hasPrevoiusState();
    bool hasNextState();

private:
    Entry currentState;
    std::vector<Entry> undoStack;
    std::vector<Entry> redoStack;
};

#endif //MODELS_STRATEGYHISTORY_H
