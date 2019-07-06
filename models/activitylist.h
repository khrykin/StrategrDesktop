//
// Created by Dmitry Khrykin on 2019-07-03.
//

#ifndef MODELS_ACTIVITYLIST_H
#define MODELS_ACTIVITYLIST_H

#include <optional>
#include <vector>
#include <functional>
#include <memory>

#include "notifiableonchange.h"
#include "activity.h"
#include "privatelist.hpp"
#include "streamablelist.h"

class Strategy;

using ActivityIndex = unsigned int;
using BasePrivateList = PrivateList<std::shared_ptr<Activity>>;

class ActivityList :
        public BasePrivateList,
        public NotifiableOnChange,
        public StreamableList<ActivityList> {
public:
    class AlreadyPresentException;
    using const_iterator = std::vector<Activity>::const_iterator;

    explicit ActivityList(const std::vector<Activity> &fromVector = {});

    const Activity &operator[](Index itemIndex) const;

private:
    friend Strategy;

    void add(const Activity &activity);
    void removeAtIndex(ActivityIndex index);
    void editAtIndex(ActivityIndex index, const Activity &newActivity);

    void drag(ActivityIndex fromIndex, ActivityIndex toIndex);

    bool has(const Activity &searchedActivity) const;

    void rotateLeft(ActivityIndex fromIndex, ActivityIndex toIndex);
    void rotateRight(ActivityIndex fromIndex, ActivityIndex toIndex);

    std::string classPrintName() const
    override;
};

struct ActivityList::AlreadyPresentException : public std::exception {
    static constexpr auto message = "Already present";
    const char *what() const noexcept override;
};


#endif //MODELS_ACTIVITYLIST_H
