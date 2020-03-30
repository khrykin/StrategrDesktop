//
// Created by Dmitry Khrykin on 2019-07-03.
//

#ifndef STRATEGR_ACTIVITYLIST_H
#define STRATEGR_ACTIVITYLIST_H

#include <optional>
#include <vector>
#include <functional>
#include <memory>

#include "notifiableonchange.h"
#include "activity.h"
#include "privatelist.h"
#include "streamablelist.h"

namespace stg {
    class strategy;

    using activity_index = unsigned int;
    using activity_list_base = private_list<std::shared_ptr<activity>>;

    class activity_list :
            public activity_list_base,
            public notifiable_on_change,
            public streamable_list<activity_list> {
    public:
        class already_present_exception;
        using const_iterator = std::vector<activity>::const_iterator;

        explicit activity_list(const std::vector<activity> &from_vector = {});
        explicit activity_list(const std::vector<std::shared_ptr<activity>> &from_vector);

        const activity &operator[](activity_index item_index) const;
        activity *at(activity_index item_index) const;

        std::optional<index_t> index_of(const activity *activity) const;
        std::optional<index_t> index_of(const activity &activity) const;

        activity_list search(std::string query) const;

        std::string class_print_name() const override;
    private:
        friend strategy;

        void silently_add(const activity &activity) noexcept(false);
        void add(const activity &activity) noexcept(false);

        void silently_remove_at_index(activity_index index);
        void remove_at_index(activity_index index);

        void silently_edit_at_index(activity_index index, const activity &new_activity) noexcept(false);
        void edit_at_index(activity_index index, const activity &new_activity) noexcept(false);

        void silently_drag(activity_index from_index, activity_index to_index);
        void drag(activity_index from_index, activity_index to_index);

        bool has(const activity &searched_activity) const;
    };

    class activity_list::already_present_exception : public std::exception {
        static constexpr auto message = "this activity already exists";
        const char *what() const noexcept override;
    };
}

#endif //STRATEGR_ACTIVITYLIST_H
