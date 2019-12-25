//
// Created by Dmitry Khrykin on 2019-07-29.
//

#include "DragOperation.h"
#include "Strategy.h"

DragOperation::DragOperation(TimeSlotsState *timeSlots, IndicesVector initialIndices)
        : timeSlots(timeSlots),
          initialDraggedIndices(std::move(initialIndices)) {
}

void DragOperation::recordDrag(const Session::TimeSlotsState &timeSlotsToDrag, int distance) {
    if (distance == 0) {
        return;
    }

    auto rangeToDrag = IndicesRange{*timeSlots->indexOf(timeSlotsToDrag.front()),
                                    *timeSlots->indexOf(timeSlotsToDrag.back())};

    auto newDraggedIndices = silentlyDrag(rangeToDrag, distance);

    invalidateDrag(newDraggedIndices);
}

DragOperation::IndicesVector
DragOperation::silentlyDrag(const IndicesRange &rangeToDrag, int distance) {
    auto destinationIndex = distance < 0
                            ? rangeToDrag.first + distance
                            : rangeToDrag.last + distance;

    if (destinationIndex > timeSlots->size() - 1) {
        return {};
    }

    auto[cacheRange, restoreCacheRange, newDragRange]
    = getRanges(rangeToDrag, destinationIndex, distance);

    auto cache = makeCache(cacheRange);

    MovementsState movements;
    IndicesVector newDraggedIndices;

    for (auto i = 0; i < rangeToDrag.size(); i++) {
        auto insertAtIndex = newDragRange.first + i;
        auto oldIndex = rangeToDrag.first + i;

        movements[oldIndex] = insertAtIndex;

        newDraggedIndices.push_back(insertAtIndex);

        timeSlots->silentlySetActivityAtIndex(
                timeSlots->at(oldIndex).activity,
                insertAtIndex
        );
    }

    restoreCache(restoreCacheRange, cache, movements);

    applyMovementsToHistory(movements);

    return newDraggedIndices;
}

void DragOperation::restoreCache(const IndicesRange &restoreCacheRange,
                                 const IndicesCache &cache,
                                 MovementsState &movements) const {
    for (auto i = 0; i < cache.size(); i++) {
        auto insertAtIndex = restoreCacheRange.first + i;
        auto[historyIndex, activity] = cache[i];

        movements[historyIndex] = insertAtIndex;

        timeSlots->silentlySetActivityAtIndex(activity, insertAtIndex);
    }
}

std::tuple<
        DragOperation::IndicesRange,
        DragOperation::IndicesRange,
        DragOperation::IndicesRange
>
DragOperation::getRanges(const IndicesRange &indicesToDrag,
                         TimeSlotIndex destinationIndex,
                         int distance) const {
    auto cacheRange = getCacheRange(indicesToDrag,
                                    destinationIndex,
                                    distance);

    auto restoreCacheFirstIndex = distance < 0
                                  ? destinationIndex + indicesToDrag.size()
                                  : indicesToDrag.first;

    auto restoreCacheRange = IndicesRange{restoreCacheFirstIndex,
                                          restoreCacheFirstIndex + cacheRange.size()};

    auto newDragRange = getNewDraggingIndices(indicesToDrag,
                                              destinationIndex,
                                              distance);

    return std::make_tuple(cacheRange, restoreCacheRange, newDragRange);
}

DragOperation::IndicesRange
DragOperation::getNewDraggingIndices(const IndicesRange &draggingIndices,
                                     TimeSlotIndex destinationIndex,
                                     int distance) const {
    auto newFirstIndex = distance < 0
                         ? destinationIndex
                         : destinationIndex - draggingIndices.size() + 1;

    return IndicesRange{newFirstIndex,
                        newFirstIndex + draggingIndices.size()};
}

DragOperation::IndicesRange
DragOperation::getCacheRange(const IndicesRange &draggingIndices,
                             TimeSlotIndex destinationIndex,
                             int distance) const {
    return distance < 0
           ? IndicesRange{destinationIndex,
                          draggingIndices.first - 1}
           : IndicesRange{draggingIndices.last + 1,
                          destinationIndex};
}

DragOperation::IndicesCache
DragOperation::makeCache(IndicesRange cacheIndices) const {
    IndicesCache cache;
    for (auto i = cacheIndices.first; i <= cacheIndices.last; i++) {
        auto cacheEntry = std::make_tuple(i, timeSlots->at(i).activity);
        cache.push_back(cacheEntry);
    }

    return cache;
}

void DragOperation::invalidateDrag(const IndicesVector &newDraggedIndices) {
    if (newDraggedIndices.empty()) {
        return;
    }

    draggedIndices = newDraggedIndices;

    auto nobodyCanMove = false;

    while (!nobodyCanMove) {
        auto cantMoveCount = 0;

        for (auto const&[currentIndex, pastIndices] : history) {
            auto slotIsDragged = std::find(draggedIndices.begin(),
                                           draggedIndices.end(),
                                           currentIndex) != draggedIndices.end();
            auto slotIsEmpty = timeSlots->at(currentIndex).activity == Strategy::NoActivity;

            if (slotIsDragged || slotIsEmpty) {
                cantMoveCount++;
                continue;
            }

            auto sessionRange = findSessionRangeFor(currentIndex);
            auto initialSessionBeginIndex = getInitial(sessionRange.first);

            if (initialSessionBeginIndex != sessionRange.first) {
                auto canMoveTo = findAvaliableMovementIndex(sessionRange,
                                                            initialSessionBeginIndex);

                if (canMoveTo != sessionRange.first) {
                    auto distance = canMoveTo - sessionRange.first;
                    silentlyDrag(sessionRange, distance);
                    break;
                }
            }

            cantMoveCount++;
        }

        if (cantMoveCount == history.size()) {
            nobodyCanMove = true;
        }
    }
}

DragOperation::TimeSlotIndex
DragOperation::getInitial(TimeSlotIndex index) {
    return history.count(index)
           ? history[index][InitialIndexKey]
           : index;
}

void DragOperation::printIndices(const std::string &name,
                                 const IndicesVector &indicesState) {
    std::cout << name << ": [ ";

    for (auto index : indicesState) {
        std::cout << index << " ";
    }

    std::cout << "]" << std::endl;
}

void DragOperation::printHistory(const std::string &name,
                                 const HistoryState &historyState) {
    std::cout << name << std::endl;
    for (auto const&[currentIndex, pastIndices] : historyState) {
        if (!pastIndices.empty()) {
            printIndices(std::to_string(currentIndex), pastIndices);
        }
    }
}

void DragOperation::printMovements(const MovementsState &movements) {
    std::cout << "Movements:" << std::endl;
    for (auto const&[pastIndex, currentIndex] : movements) {
        std::cout << pastIndex << " -> " << currentIndex << std::endl;
    }
}

void DragOperation::applyMovementsToHistory(const MovementsState &movements) {
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
}

DragOperation::IndicesRange
DragOperation::findSessionRangeFor(TimeSlotIndex timeSlotIndex) {
    auto beginIndex = timeSlotIndex;
    auto endIndex = timeSlotIndex;

    if (timeSlotIndex > 0) {
        for (auto i = timeSlotIndex - 1; i < timeSlots->size(); i--) {
            if (timeSlots->at(i).activity == timeSlots->at(timeSlotIndex).activity) {
                beginIndex = i;
            } else {
                break;
            }
        }
    }

    if (timeSlotIndex < timeSlots->size() - 1) {
        for (auto i = timeSlotIndex + 1; i < timeSlots->size(); i++) {
            if (timeSlots->at(i).activity == timeSlots->at(timeSlotIndex).activity) {
                endIndex = i;
            } else {
                break;
            }
        }
    }

    return IndicesRange{beginIndex, endIndex};
}

DragOperation::TimeSlotIndex
DragOperation::findAvaliableMovementIndex(IndicesRange sessionRange,
                                          TimeSlotIndex targetIndex) {
    TimeSlotIndex result = sessionRange.first;

    auto wantsUp = sessionRange.first > targetIndex;
    auto wantsDown = sessionRange.first < targetIndex;

    if (wantsUp) {
        if (sessionRange.first == 0) {
            return result;
        }

        for (auto i = sessionRange.first - 1; i >= targetIndex; i--) {
            if (timeSlots->at(i).activity == Strategy::NoActivity ||
                timeSlots->at(i).activity == timeSlots->at(sessionRange.first).activity) {
                result = i;
            } else {
                break;
            }
        }
    }

    if (wantsDown) {
        if (sessionRange.last == timeSlots->size() - 1 ||
            targetIndex + sessionRange.size() >= timeSlots->size()) {
            return result;
        }

        for (auto i = sessionRange.last + 1; i <= targetIndex + sessionRange.size() - 1; i++) {
            if (timeSlots->at(i).activity == Strategy::NoActivity ||
                timeSlots->at(i).activity == timeSlots->at(sessionRange.last).activity) {
                result = i - sessionRange.size() + 1;
            } else {
                break;
            }
        }
    }

    return result;
}

bool DragOperation::stateChanged() {
    return *timeSlots != initialTimeSlotsState;
}

unsigned int DragOperation::IndicesRange::size() const {
    return last - first + 1;
}
