#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>

#include "activitygroup.h"

using namespace std;
using Slot = optional<Activity>;
using SlotsState = vector<Slot>;
using ActivityGroupsState = vector<ActivityGroup>;
using Duration = int;

struct Strategy
{
    optional<string> title;
    vector<Activity> activities;

    ActivityGroupsState group();

    void appendActivity(const Activity activity);
    void removeActivity(const Activity &activity);
    void editActivity(const Activity &from, const Activity &to);
    optional<unsigned int> indexOfActivity(const Activity &activity) const;

    Slot slotAtIndex(int index);
    void setSlotAtIndex(int index, Slot slot);
    void setSlotAtIndices(vector<int> indices, Slot slot);
    void copySlot(int fromIndex, int toIndex);
    void fillSlots(int fromIndex, int toIndex);

    optional<int> startSlotIndexForGroupIndex(int groupIndex);
    optional<int> groupIndexForSlotIndex(int slotIndex);

    static Strategy *createEmpty();

    SlotsState slotsState() const;
    void setSlotsState(const SlotsState &slotsState);

    string debugSlots();
    string debugGroups();

    bool hasSlotIndex(int index);
    bool hasActivity(const Activity &activity);

    void commitToHistory(SlotsState state, vector<Activity> activities);
    void undo();
    void redo();

    struct HistoryEntry {
        HistoryEntry(Strategy *strategy);
        HistoryEntry(SlotsState slotsState, vector<Activity> activities);

        SlotsState slotsState;
        vector<Activity> activities;
    };

    void commitToHistory(HistoryEntry entry);

    unsigned int numberOfSlots() const;
    void setNumberOfSlots(unsigned int numberOfSlots);

    Duration startTime() const;
    void setStartTime(const Duration &startTime);

    Duration slotDuration() const;
    void setSlotDuration(const Duration &slotDuration);

    Duration startTimeForSlotIndex(int index);
    Duration endTime();

    vector<Duration> startTimes();
private:
    SlotsState _slotsState;
    Duration _slotDuration = 15;
    Duration _startTime = 6 * 60; // 6:00
    unsigned int _numberOfSlots = 68;

    vector<HistoryEntry> undoStack;
    vector<HistoryEntry> redoStack;
};

#endif // STRATEGY_H
