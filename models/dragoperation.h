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
    using OldIndex = TimeSlotIndex;
    using NewIndex = TimeSlotIndex;
    using CurrentIndex = TimeSlotIndex;

    using IndicesVector = std::vector<TimeSlotIndex>;
    using InitialAndPreviousIndices = IndicesVector;

    using HistoryState = std::map<CurrentIndex, InitialAndPreviousIndices>;
    using MovementsState = std::map<OldIndex, NewIndex>;

    using IndicesCacheEntry = std::tuple<OldIndex, Activity *>;
    using IndicesCache = std::vector<IndicesCacheEntry>;

    explicit DragOperation(TimeSlotsState &timeSlots, IndicesVector initialIndices)
            : timeSlots(timeSlots),
              initialDraggedIndices(std::move(initialIndices)) {
        printIndices("initialDraggedIndices", initialDraggedIndices);
    }

    void recordDrag(const ActivitySession::TimeSlotsState &timeSlotsToDrag, int distance) {
        if (distance == 0) {
            return;
        }

        auto newDraggedIndices = silentlyDrag(timeSlotsToDrag, distance);

        invalidateDrag(newDraggedIndices);
    }

private:
    struct IndicesRange {
        TimeSlotIndex first = 0;
        TimeSlotIndex last = 0;

        unsigned int size() {
            return last - first + 1;
        }
    };

    TimeSlotsState &timeSlots;
    IndicesVector initialDraggedIndices;
    IndicesVector draggedIndices = {};

    HistoryState history = HistoryState();

    IndicesVector silentlyDrag(const ActivitySession::TimeSlotsState &timeSlotsToDrag, int distance) {
        auto indicesToDrag = IndicesRange{timeSlots.indexOf(timeSlotsToDrag.front()),
                                          timeSlots.indexOf(timeSlotsToDrag.back())};

        auto destinationIndex = distance < 0
                                ? indicesToDrag.first + distance
                                : indicesToDrag.last + distance;

        if (destinationIndex > timeSlots.size() - 1) {
            return {};
        }

        auto[cacheRange, restoreCacheRange, newDragRange]
        = getRanges(indicesToDrag, destinationIndex, distance);

        auto cache = makeCache(cacheRange);

        MovementsState movements;
        IndicesVector newDraggedIndices;

        for (auto i = 0; i < timeSlotsToDrag.size(); i++) {
            auto insertAtIndex = newDragRange.first + i;

            movements[indicesToDrag.first + i] = insertAtIndex;

            newDraggedIndices.push_back(insertAtIndex);
            timeSlots.silentlySetActivityAtIndex(timeSlotsToDrag[i]->activity, insertAtIndex);
        }

        restoreCache(restoreCacheRange, cache, movements);

        applyMovementsToHistory(movements);

        return newDraggedIndices;
    }

    void restoreCache(const IndicesRange &restoreCacheRange,
                      const IndicesCache &cache,
                      MovementsState &movements) const {
        for (auto i = 0; i < cache.size(); i++) {
            auto insertAtIndex = restoreCacheRange.first + i;
            auto[historyIndex, activity] = cache[i];

            movements[historyIndex] = insertAtIndex;

            timeSlots.silentlySetActivityAtIndex(activity, insertAtIndex);
        }
    }

    std::tuple<IndicesRange, IndicesRange, IndicesRange>
    getRanges(IndicesRange &draggingIndices,
              TimeSlotIndex destinationIndex,
              int distance) const {
        auto cacheRange = getCacheRange(draggingIndices,
                                        destinationIndex,
                                        distance);

        auto restoreCacheFirstIndex = distance < 0
                                      ? destinationIndex + draggingIndices.size()
                                      : draggingIndices.first;

        auto restoreCacheRange = IndicesRange{restoreCacheFirstIndex,
                                              restoreCacheFirstIndex + cacheRange.size()};

        auto newDragRange = getNewDraggingIndices(draggingIndices,
                                                  destinationIndex,
                                                  distance);

        return std::make_tuple(cacheRange, restoreCacheRange, newDragRange);
    }

    IndicesRange getNewDraggingIndices(IndicesRange &draggingIndices,
                                       TimeSlotIndex destinationIndex,
                                       int distance) const {
        auto newFirstIndex = distance < 0
                             ? destinationIndex
                             : destinationIndex - draggingIndices.size() + 1;

        return IndicesRange{newFirstIndex,
                            newFirstIndex + draggingIndices.size()};
    }

    IndicesRange getCacheRange(const IndicesRange &draggingIndices,
                               TimeSlotIndex destinationIndex,
                               int distance) const {
        return distance < 0
               ? IndicesRange{destinationIndex,
                              draggingIndices.first - 1}
               : IndicesRange{draggingIndices.last + 1,
                              destinationIndex};
    }

    IndicesCache makeCache(IndicesRange cacheIndices) const {
        IndicesCache cache;
        for (auto i = cacheIndices.first; i <= cacheIndices.last; i++) {
            auto cacheEntry = std::make_tuple(i, timeSlots[i].activity);
            cache.push_back(cacheEntry);
        }

        return cache;
    }

    void invalidateDrag(const IndicesVector &newDraggedIndices) {
        if (newDraggedIndices.empty()) {
            return;
        }

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

};


#endif //STRATEGR_DRAGOPERATION_H
