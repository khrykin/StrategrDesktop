//
// Created by Dmitry Khrykin on 2019-07-07.
//

#include "StrategyHistory.h"

void StrategyHistory::commit(const StrategyHistory::Entry &newState) {
    if (newState != currentState) {
        undoStack.push_back(currentState);
        redoStack.clear();

        currentState = newState;
    }
}

std::optional<StrategyHistory::Entry> StrategyHistory::undo() {
    if (hasPrevoiusState()) {
        redoStack.push_back(currentState);

        currentState = undoStack.back();
        undoStack.pop_back();

        return currentState;
    }

    return std::nullopt;
}

std::optional<StrategyHistory::Entry> StrategyHistory::redo() {
    if (hasNextState()) {
        undoStack.push_back(currentState);

        currentState = redoStack.back();
        redoStack.pop_back();

        return currentState;
    }

    return std::nullopt;
}

bool StrategyHistory::hasPrevoiusState() {
    return !undoStack.empty();
}

bool StrategyHistory::hasNextState() {
    return !redoStack.empty();
}

StrategyHistory::StrategyHistory(StrategyHistory::Entry currentState)
        : currentState(std::move(currentState)) {}
