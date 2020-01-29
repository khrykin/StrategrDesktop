//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include "ActivityList.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include "utility.h"

void ActivityList::add(const Activity &activity) {
    if (has(activity)) {
        throw AlreadyPresentException();
    }


    _data.push_back(std::make_shared<Activity>(activity));

    onChangeEvent();
}

void ActivityList::removeAtIndex(ActivityIndex index) {
    _data.erase(_data.begin() + index);

    onChangeEvent();
}

void ActivityList::editAtIndex(ActivityIndex index, const Activity &newActivity) {
    if (has(newActivity)) {
        throw AlreadyPresentException();
    }

    _data[index] = std::make_shared<Activity>(newActivity);

    onChangeEvent();
}

bool ActivityList::has(const Activity &searchedActivity) const {
    for (const auto &activity : _data) {
        if (*activity == searchedActivity) {
            return true;
        }
    }

    return false;
}

void ActivityList::drag(ActivityIndex fromIndex, ActivityIndex toIndex) {
    if (fromIndex == toIndex) {
        return;
    }

    if (toIndex < fromIndex) {
        rotateLeft(fromIndex, toIndex);
    } else {
        rotateRight(fromIndex, toIndex);
    }

    onChangeEvent();
}

void ActivityList::rotateLeft(ActivityIndex fromIndex, ActivityIndex toIndex) {
    rotate(_data.begin() + toIndex,
           _data.begin() + fromIndex,
           _data.end());
}

void ActivityList::rotateRight(ActivityIndex fromIndex, ActivityIndex toIndex) {
    auto lastIndex = _data.size() - 1;
    auto fromIndexRight = lastIndex - fromIndex;
    auto toIndexRight = lastIndex - toIndex;

    rotate(_data.rbegin() + toIndexRight,
           _data.rbegin() + fromIndexRight,
           _data.rend());
}

std::string ActivityList::classPrintName() const {
    return "ActivityList";
}

ActivityList::ActivityList(const std::vector<Activity> &fromVector) {
    std::transform(fromVector.begin(),
                   fromVector.end(),
                   std::back_inserter(_data), [](auto &activity) {
                return std::make_shared<Activity>(activity);
            });
}

ActivityList::ActivityList(const std::vector<std::shared_ptr<Activity>> &fromVector) {
    std::transform(fromVector.begin(),
                   fromVector.end(),
                   std::back_inserter(_data), [](auto &activity) {
                return activity;
            });
}

const Activity &ActivityList::operator[](ActivityIndex itemIndex) const {
    return *_data[itemIndex];
}

Activity *ActivityList::at(ActivityIndex itemIndex) const {
    return _data.at(itemIndex).get();
}

std::optional<ActivityList::Index> ActivityList::indexOf(const Activity *activity) const {
    for (auto activityIndex = 0; activityIndex < size(); activityIndex++) {
        if (_data[activityIndex].get() == activity) {
            return activityIndex;
        }
    }

    return std::nullopt;
}

ActivityList ActivityList::search(std::string query) const {
    query = std::regex_replace(query, std::regex("^\\s*"), "");
    query = std::regex_replace(query, std::regex("\\s*$"), "");

    if (query.empty()) {
        return ActivityList{};
    }

    query = stg::utf8_fold_case(query);

    std::vector<std::shared_ptr<Activity>> results;
    std::copy_if(begin(),
                 end(),
                 std::back_inserter(results), [&query](auto activity) {
                auto name = activity->name();

                name = stg::utf8_fold_case(name);

                return name.find(query) != std::string::npos;
            });

    return ActivityList(results);
}

const char *ActivityList::AlreadyPresentException::what() const noexcept {
    return message;
}
