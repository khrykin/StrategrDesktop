//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include "ActivityList.h"
#include <iostream>
#include <algorithm>
#include <regex>

void ActivityList::add(const Activity &activity) {
    if (has(activity)) {
        throw AlreadyPresentException();
    }


    _vector.push_back(std::make_shared<Activity>(activity));

    onChangeEvent();
}

void ActivityList::removeAtIndex(ActivityIndex index) {
    _vector.erase(_vector.begin() + index);

    onChangeEvent();
}

void ActivityList::editAtIndex(ActivityIndex index, const Activity &newActivity) {
    if (has(newActivity)) {
        throw AlreadyPresentException();
    }

    _vector[index] = std::make_shared<Activity>(newActivity);

    onChangeEvent();
}

bool ActivityList::has(const Activity &searchedActivity) const {
    for (const auto &activity : _vector) {
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
    rotate(_vector.begin() + toIndex,
           _vector.begin() + fromIndex,
           _vector.end());
}

void ActivityList::rotateRight(ActivityIndex fromIndex, ActivityIndex toIndex) {
    auto lastIndex = _vector.size() - 1;
    auto fromIndexRight = lastIndex - fromIndex;
    auto toIndexRight = lastIndex - toIndex;

    rotate(_vector.rbegin() + toIndexRight,
           _vector.rbegin() + fromIndexRight,
           _vector.rend());
}

std::string ActivityList::classPrintName() const {
    return "ActivityList";
}

ActivityList::ActivityList(const std::vector<Activity> &fromVector) {
    std::transform(fromVector.begin(),
                   fromVector.end(),
                   std::back_inserter(_vector), [](auto &activity) {
                return std::make_shared<Activity>(activity);
            });
}

ActivityList::ActivityList(const std::vector<std::shared_ptr<Activity>> &fromVector) {
    std::transform(fromVector.begin(),
                   fromVector.end(),
                   std::back_inserter(_vector), [](auto &activity) {
                return activity;
            });
}


const Activity &ActivityList::operator[](ActivityIndex itemIndex) const {
    return *_vector[itemIndex];
}

Activity *ActivityList::at(ActivityIndex itemIndex) const {
    return _vector.at(itemIndex).get();
}

ActivityList &ActivityList::operator=(const ActivityList &newList) {
    _vector = newList._vector;

    onChangeEvent();

    return *this;
}

std::optional<ActivityList::Index> ActivityList::indexOf(const Activity *activity) const {
    for (auto activityIndex = 0; activityIndex < size(); activityIndex++) {
        if (_vector[activityIndex].get() == activity) {
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

    std::transform(query.begin(),
                   query.end(),
                   query.begin(),
                   [](auto &c) {
                       return std::tolower(c);
                   });

    std::vector<std::shared_ptr<Activity>> results;
    std::copy_if(begin(),
                 end(),
                 std::back_inserter(results), [&query](auto activity) {
                auto name = activity->name();

                std::transform(name.begin(),
                               name.end(),
                               name.begin(),
                               [](auto &c) {
                                   return std::tolower(c);
                               });

                return name.find(query) != std::string::npos;
            });

    return ActivityList({results});
}

const char *ActivityList::AlreadyPresentException::what() const noexcept {
    return message;
}
