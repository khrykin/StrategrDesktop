#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>

#include "activitygroup.h"

using namespace std;
using Slot = optional<shared_ptr<Activity>>;
using SlotsState = vector<Slot>;
using ActivityGroupsState = vector<ActivityGroup>;

struct Strategy
{
    optional<string> title;
    vector<shared_ptr<Activity>> activities;
    unsigned int numberOfSlots = 20;

    ActivityGroupsState group();

    Slot slotAtIndex(int index);
    void setSlotAtIndex(int index, Slot slot);
    void copySlot(int fromIndex, int toIndex);
    void fillSlots(int fromIndex, int toIndex);

    optional<int> startSlotIndexForGroupIndex(int groupIndex);
    optional<int> groupIndexForSlotIndex(int slotIndex);

    static Strategy *createEmtpty();

    SlotsState slotsState() const;
    void setSlotsState(const SlotsState &slotsState);

    string debugSlots();
    string debugGroups();

    bool hasSlotIndex(int index);

private:
    SlotsState _slotsState;

};

#endif // STRATEGY_H
