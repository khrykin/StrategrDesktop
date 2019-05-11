#include "activitygroup.h"

ActivityGroup::ActivityGroup(optional<shared_ptr<Activity>> activity, unsigned int length)
    : activity(activity),
      length(length)
{}
