#ifndef STRATEGY_H
#define STRATEGY_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "notifiableonchange.h"
#include "activity.h"
#include "activitylist.h"
#include "timeslotsstate.h"
#include "activitysessionslist.h"
#include "strategyhistory.h"
#include "dragoperation.h"
#include "resizeoperation.h"

class Strategy : public NotifiableOnChange {
public:
    struct Defaults {
        static constexpr auto timeSlotDuration = 15;
        static constexpr auto beginTime = 6 * 60;
        static constexpr auto numberOfTimeSlots = (22 - 6) * 60 / timeSlotDuration;
    };

    using Duration = TimeSlot::Duration;
    using Time = TimeSlot::Time;
    using TimeSlotIndex = TimeSlotsState::Index;
    using ActivitySessionIndex = ActivitySessionsList::Index;
    using StateSize = TimeSlotsState::StateSize;

    static constexpr auto NoActivity = TimeSlot::NoActivity;

    const ActivityList &activities() const;
    const ActivitySessionsList &activitySessions() const;
    const TimeSlotsState &timeSlots() const;

    explicit Strategy(Time beginTime = Defaults::beginTime,
                      Duration timeSlotDuration = Defaults::timeSlotDuration,
                      StateSize numberOfTimeSlots = Defaults::numberOfTimeSlots);

    Strategy(const TimeSlotsState &timeSlots, const ActivityList &activities);

    static std::optional<Strategy> fromJsonString(const std::string &jsonString);
    std::string toJsonString() const;

    Time beginTime() const;
    void setBeginTime(Time beginTime);

    Duration timeSlotDuration() const;
    void setTimeSlotDuration(Duration timeSlotDuration);

    StateSize numberOfTimeSlots() const;
    void setNumberOfTimeSlots(StateSize numberOfTimeSlots);

    Time endTime() const;
    Duration duration() const;

    void addActivity(const Activity &activity);
    void removeActivityAtIndex(ActivityIndex activityIndex);
    void editActivityAtIndex(ActivityIndex activityIndex,
                             const Activity &newActivity);

    void dragActivity(ActivityIndex fromIndex, ActivityIndex toIndex);

    void putActivityInTimeSlotsAtIndices(ActivityIndex activityIndex,
                                         const std::vector<TimeSlotIndex> &timeSlotIndices);
    void emptyTimeSlotsAtIndices(const std::vector<TimeSlotIndex> &timeSlotIndices);

    void beginResizing();
    void fillTimeSlots(TimeSlotIndex fromIndex, TimeSlotIndex tillIndex);
    void endResizing();

    void beginDragging(ActivitySessionIndex sessionIndex);
    void dragActivitySession(ActivitySessionIndex sessionIndex, int distance);
    void endDragging();

    void shiftBelowTimeSlot(TimeSlotIndex fromIndex, int length);

    void commitToHistory();
    void undo();
    void redo();

private:
    ActivityList _activities;
    TimeSlotsState _timeSlots;
    ActivitySessionsList _activitySessions;
    StrategyHistory history;

    std::shared_ptr<DragOperation> currentDragOperation = nullptr;
    std::shared_ptr<ResizeOperation> currentResizeOperation = nullptr;

    void timeSlotsChanged();
    void setupTimeSlotsCallback();

    StrategyHistory::Entry makeHistoryEntry();

    std::vector<TimeSlotIndex>
    globalSlotIndicesFromSession(const ActivitySession &session) const;
};

#endif // STRATEGY_H