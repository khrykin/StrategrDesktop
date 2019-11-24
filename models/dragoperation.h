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

    explicit DragOperation(TimeSlotsState *timeSlots,
                           IndicesVector initialIndices);

    void recordDrag(const ActivitySession::TimeSlotsState &timeSlotsToDrag,
                    int distance);

    bool stateChanged();

private:
    static const unsigned int InitialIndexKey = 0;

    struct IndicesRange {
        TimeSlotIndex first = 0;
        TimeSlotIndex last = 0;

        unsigned int size() const;
    };

    TimeSlotsState *timeSlots;
    TimeSlotsState initialTimeSlotsState = *timeSlots;

    IndicesVector initialDraggedIndices;
    IndicesVector draggedIndices = {};

    HistoryState history = HistoryState();

    IndicesVector silentlyDrag(const IndicesRange &rangeToDrag,
                               int distance);

    void restoreCache(const IndicesRange &restoreCacheRange,
                      const IndicesCache &cache,
                      MovementsState &movements) const;

    std::tuple<IndicesRange, IndicesRange, IndicesRange>
    getRanges(const IndicesRange &indicesToDrag,
              TimeSlotIndex destinationIndex,
              int distance) const;

    IndicesRange getNewDraggingIndices(const IndicesRange &draggingIndices,
                                       TimeSlotIndex destinationIndex,
                                       int distance) const;

    IndicesRange getCacheRange(const IndicesRange &draggingIndices,
                               TimeSlotIndex destinationIndex,
                               int distance) const;

    IndicesCache makeCache(IndicesRange cacheIndices) const;

    void applyMovementsToHistory(const MovementsState &movements);

    void invalidateDrag(const IndicesVector &newDraggedIndices);

    IndicesRange findSessionRangeFor(TimeSlotIndex timeSlotIndex);
    TimeSlotIndex findAvaliableMovementIndex(IndicesRange sessionRange,
                                             TimeSlotIndex targetIndex);

    TimeSlotIndex getInitial(TimeSlotIndex index);

    void printIndices(const std::string &name,
                      const IndicesVector &indicesState);


    void printHistory(const std::string &name,
                      const HistoryState &historyState);
    static void printMovements(const MovementsState &movements);
};


#endif //STRATEGR_DRAGOPERATION_H
