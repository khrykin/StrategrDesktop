//
// Created by Dmitry Khrykin on 2019-07-03.
//

#ifndef MODELS_ACTIVITYLIST_H
#define MODELS_ACTIVITYLIST_H

#include <optional>
#include <vector>
#include <functional>
#include <memory>

#include "NotifiableOnChange.h"
#include "Activity.h"
#include "PrivateList.hpp"
#include "StreamableList.h"

class Strategy;

using ActivityIndex = unsigned int;
using ActivityListBase = PrivateList<std::shared_ptr<Activity>>;

class ActivityList :
        public ActivityListBase,
        public NotifiableOnChange,
        public StreamableList<ActivityList> {
public:
    class AlreadyPresentException;
    using const_iterator = std::vector<Activity>::const_iterator;

    explicit ActivityList(const std::vector<Activity> &fromVector = {});
    explicit ActivityList(const std::vector<std::shared_ptr<Activity>> &fromVector);

    const Activity &operator[](ActivityIndex itemIndex) const;

    Activity *at(ActivityIndex itemIndex) const;

    std::optional<Index> indexOf(const Activity *activity) const;

    std::string classPrintName() const override;

    ActivityList search(std::string query) const;
private:
    friend Strategy;

    void add(const Activity &activity);
    void removeAtIndex(ActivityIndex index);
    void editAtIndex(ActivityIndex index, const Activity &newActivity);

    void drag(ActivityIndex fromIndex, ActivityIndex toIndex);

    bool has(const Activity &searchedActivity) const;

    void rotateLeft(ActivityIndex fromIndex, ActivityIndex toIndex);
    void rotateRight(ActivityIndex fromIndex, ActivityIndex toIndex);
};

class ActivityList::AlreadyPresentException : public std::exception {
    static constexpr auto message = "this activity already exists";
    const char *what() const noexcept override;
};


#endif //MODELS_ACTIVITYLIST_H
