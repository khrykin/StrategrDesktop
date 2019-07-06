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

    explicit StrategyHistory(Entry currentState)
            : currentState(std::move(currentState)) {}


    void commit(const Entry &newState) {
        if (newState != currentState) {
            undoStack.push_back(currentState);
            redoStack.clear();

            currentState = newState;
        }
    }

    std::optional<Entry> undo() {
        if (hasPrevoiusState()) {
            redoStack.push_back(currentState);

            currentState = undoStack.back();
            undoStack.pop_back();

            return currentState;
        }

        return std::nullopt;
    }

    std::optional<Entry> redo() {
        if (hasNextState()) {
            undoStack.push_back(currentState);

            currentState = redoStack.back();
            redoStack.pop_back();

            return currentState;
        }

        return std::nullopt;
    }

    bool hasPrevoiusState() {
        return !undoStack.empty();
    }

    bool hasNextState() {
        return !redoStack.empty();
    }

private:
    Entry currentState;
    std::vector<Entry> undoStack;
    std::vector<Entry> redoStack;
};

#endif //MODELS_STRATEGYHISTORY_H
