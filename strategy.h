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

    static Strategy *createEmtpty();

    SlotsState slotsState() const;
    void setSlotsState(const SlotsState &slotsState);
    void debug();

private:
    SlotsState _slotsState;

};

#endif // STRATEGY_H
