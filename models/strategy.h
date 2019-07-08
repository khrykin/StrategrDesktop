#ifndef STRATEGY_H
#define STRATEGY_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "activity.h"
#include "activitylist.h"
#include "timeslotsstate.h"
#include "activitysessionslist.h"
#include "strategyhistory.h"

struct Strategy {
public:
    struct Defaults {
        static constexpr auto timeSlotDuration = 15;
        static constexpr auto beginTime = 6 * 60;
        static constexpr auto numberOfTimeSlots = (22 - 6) * 60 / timeSlotDuration;
    };

    using Duration = TimeSlot::Duration;
    using Time = TimeSlot::Time;
    using TimeSlotIndex = TimeSlotsState::Index;
    using StateSize = TimeSlotsState::StateSize;

    static constexpr auto NoActivity = TimeSlot::NoActivity;

    const ActivityList &activities() const;
    const ActivitySessionsList &activitySessions() const;

    explicit Strategy(Time beginTime = Defaults::beginTime,
                      Duration timeSlotDuration = Defaults::timeSlotDuration,
                      StateSize numberOfTimeSlots = Defaults::numberOfTimeSlots);

    Strategy(const std::vector<TimeSlot> &timeSlotsVector,
             const std::vector<Activity> &activitiesVector);

    Time beginTime() const;
    void setBeginTime(Time beginTime);

    Duration timeSlotDuration() const;
    void setTimeSlotDuration(Duration timeSlotDuration);

    StateSize numberOfTimeSlots() const;
    void setNumberOfTimeSlots(StateSize numberOfTimeSlots);

    Time endTime() const;

    void addActivity(const Activity &activity);

    void removeActivityAtIndex(ActivityIndex activityIndex);

    void editActivityAtIndex(ActivityIndex activityIndex,
                             const Activity &newActivity);

    void dragActivity(ActivityIndex fromIndex, ActivityIndex toIndex);

    void putActivityInTimeSlotsAtIndices(ActivityIndex activityIndex,
                                         const std::vector<TimeSlotIndex> &timeSlotIndices);

    void emptyTimeSlotsAtIndices(const std::vector<TimeSlotIndex> &timeSlotIndices);;

    void fillTimeSlots(TimeSlotIndex fromIndex, TimeSlotIndex tillIndex);

    void commitToHistory();
    void undo();
    void redo();

private:
    ActivityList _activities;
    TimeSlotsState timeSlots;
    ActivitySessionsList _activitySessions;
    StrategyHistory history;

    void timeSlotsChanged();
    void setupTimeSlotsCallback();

    StrategyHistory::Entry makeHistoryEntry();
};

//
//struct Strategy {
//  static const auto defaultSlotDuration = 15;  // 15 min.
//  static const auto defaultStartTime = 6 * 60; // 6:00 AM
//  static const auto defaultNumberOfSlots = // (22:00 PM - 60.00 AM) / 15 min.
//      (22 * 60 - defaultStartTime) / defaultSlotDuration;
//
//  optional<string> title;
//  vector<Activity> activities;
//
//  ActivityGroupsState calculateGroups();
//
//  void appendActivity(const Activity &activity);
//  void removeActivity(const Activity &activity);
//  void editActivity(const Activity &from, const Activity &to);
//  void editActivityAtIndex(int index, const Activity &to);
//  optional<unsigned int> indexOfActivity(const Activity &activity) const;
//
//  Slot slotAtIndex(int index);
//  void setSlotAtIndex(int index, Slot slot);
//  void setSlotAtIndices(vector<int> indices, Slot slot);
//  void copySlot(int fromIndex, int toIndex);
//  void fillSlots(int fromIndex, int toIndex);
//
//  optional<int> startSlotIndexForGroupIndex(int groupIndex);
//  optional<int> groupIndexForSlotIndex(int slotIndex);
//  optional<int> findSlotIndexForTime(int time);
//
//  static Strategy *createEmpty();
//
//  SlotsState slotsState() const;
//  void setSlotsState(const SlotsState &slotsState);
//
//  string debugSlots();
//  string debugGroups();
//
//  bool hasSlotIndex(int index);
//  bool hasActivity(const Activity &activity);
//
//  void commitToHistory(SlotsState state, vector<Activity> activities);
//  void undo();
//  void redo();
//
//  struct HistoryEntry {
//    explicit HistoryEntry(Strategy *strategy);
//    HistoryEntry(SlotsState slotsState, vector<Activity> activities);
//
//    SlotsState slotsState;
//    vector<Activity> activities;
//  };
//
//  void commitToHistory(HistoryEntry entry);
//
//  unsigned int numberOfSlots() const;
//  void setNumberOfSlots(unsigned int numberOfSlots);
//
//  Duration beginTime() const;
//  void setBeginTime(const Duration &beginTime);
//
//  Duration slotDuration() const;
//  void setSlotDuration(const Duration &slotDuration);
//
//  Duration startTimeForSlotIndex(int index);
//  Duration startTimeForGroupIndex(int groupIndex);
//  Duration endTime();
//
//  vector<Duration> startTimes();
//
//private:
//  SlotsState _slotsState;
//  Duration _slotDuration = defaultSlotDuration;
//  Duration _beginTime = defaultStartTime;
//
//  vector<HistoryEntry> undoStack;
//  vector<HistoryEntry> redoStack;
//};
//
#endif // STRATEGY_H
