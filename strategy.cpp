#include <algorithm>
#include <vector>
#include <string>
#include "strategy.h"
#include <iostream>

ActivityGroupsState Strategy::group()
{
    auto slotsState = this->slotsState();
    ActivityGroupsState result;
    optional<ActivityGroup> cachedActivityGroup;

    for (unsigned int index = 0; index < slotsState.size(); index++) {
        auto activity = slotsState[index];
        auto defaultActivityGroup = ActivityGroup(slotsState[index]);
        auto prevActivity = index > 0
                ? slotsState[index - 1]
                : nullopt;

        if (!activity.has_value()) {
            if (cachedActivityGroup.has_value()) {
                result.push_back(cachedActivityGroup.value());
            }

            result.push_back(defaultActivityGroup);

            cachedActivityGroup = nullopt;
        } else {
            if (!prevActivity.has_value() || prevActivity.value() != activity.value()) {
                if (cachedActivityGroup.has_value()) {
                    result.push_back(cachedActivityGroup.value());
                }

                cachedActivityGroup = defaultActivityGroup;
            } else {
                if (cachedActivityGroup.has_value()) {
                    cachedActivityGroup.value().length++;
                }
            }

            if (cachedActivityGroup.has_value() && index == slotsState.size() - 1) {
                result.push_back(cachedActivityGroup.value());
            }
        }
    }

    return result;
}

Slot Strategy::slotAtIndex(int index)
{
    return _slotsState[static_cast<unsigned int>(index)];
}

void Strategy::setSlotAtIndex(int index, Slot slot)
{
    _slotsState[static_cast<unsigned int>(index)] = slot;
}

void Strategy::copySlot(int fromIndex, int toIndex)
{
    _slotsState[static_cast<unsigned int>(toIndex)] = _slotsState[static_cast<unsigned int>(fromIndex)];
}

Strategy *Strategy::createEmtpty()
{
    auto strategy = new Strategy();
    strategy->activities = {
        make_shared<Activity>("Training"),
        make_shared<Activity>("Work 1"),
        make_shared<Activity>("Nap"),
        make_shared<Activity>("Commute")
    };

    SlotsState slotsState;

    for (unsigned int i = 0; i < strategy->numberOfSlots; i++) {
        if (i < strategy->numberOfSlots / 4) {
            slotsState.push_back(strategy->activities[0]);
        } else if (i < 2 * strategy->numberOfSlots / 4) {
            slotsState.push_back(strategy->activities[1]);
        } else {
            slotsState.push_back(nullopt);
        }
    }

    strategy->setSlotsState(slotsState);

    return strategy;
}

SlotsState Strategy::slotsState() const
{
    return _slotsState;
}

void Strategy::setSlotsState(const SlotsState &slotsState)
{
    _slotsState = slotsState;
}

void Strategy::debug()
{
    for (unsigned int i = 0; i < slotsState().size(); i++) {
        auto slot = slotsState()[i];
        if (slot.has_value()) {
            cout << "Slot " + to_string(i) << "\t" << slot.value()->name << "\n";
        } else {
            cout << "Slot " + to_string(i) << "\t" << "None" << "\n";
        }
    }
}
