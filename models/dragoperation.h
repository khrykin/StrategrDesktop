//
// Created by Dmitry Khrykin on 2019-07-29.
//

#ifndef STRATEGR_DRAGOPERATION_H
#define STRATEGR_DRAGOPERATION_H

#include <vector>
#include <map>
#include <tuple>

#include <iostream>
#include "timeslotsstate.h"
#include "activitysession.h"

class DragOperation {
public:
    using TimeSlotIndex = TimeSlotsState::Index;
    using IndicesVector = std::vector<TimeSlotIndex>;

    using HistoryState = std::map<TimeSlotIndex, IndicesVector>;
    using MovementsState = std::map<TimeSlotIndex, TimeSlotIndex>;

    using IndicesCacheEntry = std::tuple<TimeSlotIndex, Activity *>;
    using IndicesCache = std::vector<IndicesCacheEntry>;

    explicit DragOperation(TimeSlotsState &timeSlots, IndicesVector initialIndices)
            : timeSlots(timeSlots),
              initialDraggedIndices(std::move(initialIndices)) {
        printIndices("initialDraggedIndices", initialDraggedIndices);
    }

    void initHistory() {
        for (auto i = 0; i < timeSlots.size(); i++) {
            history[i] = {};
        }
    }

    void recordDrag(const ActivitySession::TimeSlotsState &timeSlotsToDrag, int distance) {
        if (distance == 0) {
            return;
        }

        IndicesCache cache;
        DragOperation::IndicesVector newDraggedIndices;
        MovementsState movements;

        auto firstDraggingSlotIndex = timeSlots.indexOf(timeSlotsToDrag.front());
        auto lastDraggingSlotIndex = timeSlots.indexOf(timeSlotsToDrag.back());
        
        if (distance < 0) {

            int previousSlotIndex = firstDraggingSlotIndex + distance;
            if (previousSlotIndex < 0)
                return;

            for (auto i = previousSlotIndex; i != firstDraggingSlotIndex; i++) {
                auto cacheEntry = std::make_tuple(i, timeSlots[i].activity);
                cache.push_back(cacheEntry);
            }

            for (auto i = 0; i < timeSlotsToDrag.size(); i++) {
                auto insertAtIndex = previousSlotIndex + i;

                movements[firstDraggingSlotIndex + i] = insertAtIndex;

                newDraggedIndices.push_back(insertAtIndex);
                timeSlots.silentlySetActivityAtIndex(timeSlotsToDrag[i]->activity, insertAtIndex);
            }

            for (auto i = 0; i < cache.size(); i++) {
                auto insertAtIndex = previousSlotIndex + timeSlotsToDrag.size() + i;
                auto[historyIndex, activity] = cache[i];

                movements[historyIndex] = insertAtIndex;

                timeSlots.silentlySetActivityAtIndex(activity, insertAtIndex);
            }
        } else {
            auto nextSlotIndex = lastDraggingSlotIndex + distance;

            if (nextSlotIndex > timeSlots.size() - 1)
                return;

            for (auto i = nextSlotIndex; i != lastDraggingSlotIndex; i--) {
                auto cacheEntry = std::make_tuple(i, timeSlots[i].activity);
                cache.push_back(cacheEntry);
            }

            for (auto i = 0; i < timeSlotsToDrag.size(); i++) {
                auto insertAtIndex = nextSlotIndex - i;
                movements[lastDraggingSlotIndex - i] = insertAtIndex;

                newDraggedIndices.insert(newDraggedIndices.begin(), insertAtIndex);
                timeSlots.silentlySetActivityAtIndex(timeSlotsToDrag[i]->activity, insertAtIndex);
            }

            for (auto i = 0; i < cache.size(); i++) {
                auto insertAtIndex = nextSlotIndex - timeSlotsToDrag.size() - i;
                auto[historyIndex, activity] = cache[i];

                movements[historyIndex] = insertAtIndex;

                timeSlots.silentlySetActivityAtIndex(activity, insertAtIndex);
            }
        }


        applyMovementsToHistory(movements);

        invalidateDrag(newDraggedIndices);
    }

    void applyMovementsToHistory(const MovementsState &movements) {
        HistoryState newHistoryStacks;

        for (auto const&[pastIndex, currentIndex] : movements) {
            auto newHistoryStack = history[pastIndex];
            newHistoryStack.push_back(pastIndex);

            newHistoryStacks[currentIndex] = newHistoryStack;
        }

        for (auto const&[index, stack] : newHistoryStacks) {
            if (index == stack.front()) {
                history.erase(index);
            } else {
                IndicesVector historyEntry{stack.front()};

                // History entry doesn't need more than two values:
                // only initial index and previous index if they're different
                if (stack.back() != stack.front()) {
                    historyEntry.push_back(stack.back());
                }

                history[index] = historyEntry;
            }
        }

        printHistory("History", history);
    }

    void invalidateDrag(const IndicesVector &newDraggedIndices) {
        draggedIndices = newDraggedIndices;
        printIndices("draggedIndices", draggedIndices);
    }

    void printIndices(const std::string &name,
                      const IndicesVector &indicesState) {
        std::cout << name << ": [ ";

        for (auto index : indicesState) {
            std::cout << index << " ";
        }

        std::cout << "]" << std::endl;
    }

    void printHistory(const std::string &name, const HistoryState &historyState) {
        std::cout << name << std::endl;
        for (auto const&[currentIndex, pastIndices] : historyState) {
            if (!pastIndices.empty()) {
                printIndices(std::to_string(currentIndex), pastIndices);
            }
        }
    }

    void printMovements(const MovementsState &movements) {
        std::cout << "Movements:" << std::endl;
        for (auto const&[pastIndex, currentIndex] : movements) {
            std::cout << pastIndex << " -> " << currentIndex << std::endl;
        }
    }

private:
    TimeSlotsState &timeSlots;
    TimeSlotsState initialState = timeSlots;
    IndicesVector initialDraggedIndices;
    IndicesVector draggedIndices = {};

    HistoryState history = HistoryState();

    TimeSlotIndex firstIndex() {
        return draggedIndices.front();
    }

    TimeSlotIndex lastIndex() {
        return draggedIndices.back();
    }
};


#endif //STRATEGR_DRAGOPERATION_H
