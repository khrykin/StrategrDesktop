//
// Created by Dmitry Khrykin on 2019-07-05.
//

#ifndef MODELS_TIMESLOTSSTATE_H
#define MODELS_TIMESLOTSSTATE_H

#include <vector>

#include "timeslot.h"
#include "privatelist.hpp"
#include "notifiableonchange.h"
#include "streamablelist.h"

class TimeSlotsState :
        public PrivateList<TimeSlot>,
        public NotifiableOnChange,
        public StreamableList<TimeSlotsState> {
public:
    using Time = TimeSlot::Time;
    using Duration = TimeSlot::Duration;
    using StateSize = unsigned;

    TimeSlotsState(Time startTime,
                   Duration slotDuration,
                   StateSize numberOfSlots);

    // NB! You can't create TimeSlotsState from empty vector,
    // since there would be no way to find out slotDuration
    // and beginTime
    explicit TimeSlotsState(std::vector<TimeSlot> fromVector);

    Time beginTime() const;
    void setBeginTime(Time beginTime);

    Duration slotDuration() const;
    void setSlotDuration(Duration slotDuration);

    StateSize numberOfSlots() const;
    void setNumberOfSlots(StateSize newNumberOfSlots);

    void setActivityAtIndices(Activity *activity,
                              const std::vector<Index> &indices);

    void silentlySetActivityAtIndices(Activity *activity,
                                      const std::vector<Index> &indices);

    void silentlySetActivityAtIndex(Activity *activity,
                                    Index slotIndex);

    void fillSlots(Index fromIndex, Index tillIndex);

    void shiftBelow(Index fromIndex, StateSize length);

    void populateVector(Time startTime, StateSize numberOfSlots);

    iterator findSlotWithActivity(const Activity *activity);

    void removeActivity(Activity *activity);

    void editActivity(Activity *oldActivity,
                      Activity *newActivity);

    void swap(Index firstIndex, Index secondIndex);
    void silentlySwap(Index firstIndex, Index secondIndex);


    bool hasActivity(const Activity *activity);

    TimeSlotsState &operator=(const TimeSlotsState &newState);

    std::string classPrintName() const override;

    Index indexOf(const TimeSlot *slot) const;

private:
    Time _beginTime = 0;
    Duration _slotDuration = 0;


    Time slotBeginTime(Time globalBeginTime, Index slotIndex);
    void updateBeginTimes();
};


#endif //MODELS_TIMESLOTSSTATE_H
