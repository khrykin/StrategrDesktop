#ifndef ACTIVITYGROUP_H
#define ACTIVITYGROUP_H

#include <vector>
#include <memory>
#include <optional>

#include "activity.h"

using namespace std;

struct ActivityGroup
{
    ActivityGroup(optional<shared_ptr<Activity>> activity, unsigned int length = 1);
    optional<shared_ptr<Activity>> activity;
    unsigned int length;
};

#endif // ACTIVITYGROUP_H
