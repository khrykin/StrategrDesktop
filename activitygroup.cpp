#include "activitygroup.h"

ActivityGroup::ActivityGroup(optional<shared_ptr<Activity>> activity, int length)
    : activity(activity),
      length(length)
{}
