//
// Created by Dmitry Khrykin on 2019-07-03.
//

#ifndef STRATEGR_ACTIVITYLIST_H
#define STRATEGR_ACTIVITYLIST_H

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "activity.h"
#include "notifiableonchange.h"
#include "privatelist.h"
#include "streamablelist.h"

namespace stg {
    class strategy;

    using activity_index_t = unsigned int;
    using activity_list_base = private_list<std::shared_ptr<activity>>;

    class activity_list : public activity_list_base,
                          public notifiable_on_change,
                          public streamable_list<activity_list> {
    public:
        class already_present_exception;
        using const_iterator = std::vector<activity>::const_iterator;

        explicit activity_list(const std::vector<activity> &from_vector = {});
        explicit activity_list(const data_t &from_vector);

        auto operator[](activity_index_t item_index) const -> const activity &;
        auto at(activity_index_t item_index) const -> activity *;

        auto index_of(const activity *activity) const -> std::optional<index_t>;
        auto index_of(const activity &activity) const -> std::optional<index_t>;

        auto search(std::string query) const -> bool;
        auto filtered() const -> const data_t &;
        auto index_from_filtered(index_t index_in_filtered) const -> std::optional<index_t>;
        auto index_in_filtered(index_t activity_index) const -> std::optional<index_t>;

        void reset_with(data_t data) override;

        auto class_print_name() const -> std::string override;

    private:
        friend strategy;

        mutable std::string search_query;
        mutable data_t search_results;

        void silently_add(const activity &activity) noexcept(false);
        void add(const activity &activity) noexcept(false);

        void silently_remove_at_index(activity_index_t index);
        void remove_at_index(activity_index_t index);

        void silently_edit_at_index(activity_index_t index, const activity &new_activity) noexcept(false);
        void edit_at_index(activity_index_t index, const activity &new_activity) noexcept(false);

        void silently_drag(activity_index_t from_index, activity_index_t to_index);
        void drag(activity_index_t from_index, activity_index_t to_index);

        auto has(const activity &searched_activity) const -> bool;
    };

    class activity_list::already_present_exception : public std::exception {
        static constexpr auto message = "this activity already exists";
        auto what() const noexcept -> const char * override;
    };
}

#endif//STRATEGR_ACTIVITYLIST_H
