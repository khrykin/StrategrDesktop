//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include <algorithm>
#include <regex>

#include "activitylist.h"
#include "stgstring.h"

namespace stg {
    void activity_list::silently_add(const activity &activity) {
        if (has(activity)) {
            throw already_present_exception();
        }

        _data.push_back(std::make_shared<stg::activity>(activity));

        if (!search_query.empty())
            search(search_query);
    }

    void activity_list::add(const activity &activity) {
        silently_add(activity);

        on_change_event();
    }

    void activity_list::silently_remove_at_index(activity_index_t index) {
        _data.erase(_data.begin() + index);

        if (!search_query.empty())
            search(search_query);
    }

    void activity_list::remove_at_index(activity_index_t index) {
        silently_remove_at_index(index);

        on_change_event();
    }

    void activity_list::silently_edit_at_index(activity_index_t index, const activity &new_activity) {
        if (*_data[index] == new_activity) {
            return;
        }

        if (has(new_activity)) {
            throw already_present_exception();
        }

        _data[index] = std::make_shared<activity>(new_activity);

        if (!search_query.empty()) {
            search(search_query);
        }
    }

    void activity_list::edit_at_index(activity_index_t index, const activity &new_activity) {
        if (*_data[index] == new_activity) {
            return;
        }

        silently_edit_at_index(index, new_activity);

        on_change_event();
    }

    auto activity_list::has(const activity &searched_activity) const -> bool {
        for (const auto &activity : _data) {
            if (*activity == searched_activity) {
                return true;
            }
        }

        return false;
    }

    void activity_list::silently_drag(activity_index_t from_index, activity_index_t to_index) {
        if (from_index == to_index) {
            return;
        }

        if (from_index > to_index)
            std::rotate(_data.rend() - from_index - 1,
                        _data.rend() - from_index,
                        _data.rend() - to_index);
        else
            std::rotate(_data.begin() + from_index,
                        _data.begin() + from_index + 1,
                        _data.begin() + to_index + 1);
    }

    void activity_list::drag(activity_index_t from_index, activity_index_t to_index) {
        silently_drag(from_index, to_index);

        on_change_event();
    }

    auto activity_list::class_print_name() const -> std::string {
        return "activity_list";
    }

    activity_list::activity_list(const std::vector<activity> &from_vector) {
        std::transform(from_vector.begin(),
                       from_vector.end(),
                       std::back_inserter(_data),
                       [](auto &activity) {
                           return std::make_shared<stg::activity>(activity);
                       });

        if (!search_query.empty())
            search(search_query);
    }

    activity_list::activity_list(const data_t &from_vector) {
        _data = from_vector;

        if (!search_query.empty())
            search(search_query);
    }

    auto activity_list::operator[](activity_index_t item_index) const -> const activity & {
        return *_data[item_index];
    }

    auto activity_list::at(activity_index_t item_index) const -> stg::activity * {
        return _data.at(item_index).get();
    }

    auto activity_list::index_of(const activity *activity) const -> std::optional<index_t> {
        auto it = std::find_if(_data.begin(), _data.end(), [=](auto &a) {
            return a.get() == activity;
        });

        if (it == _data.end()) {
            return std::nullopt;
        }

        return std::distance(_data.begin(), it);
    }

    auto activity_list::index_of(const activity &activity) const -> std::optional<index_t> {
        auto it = std::find_if(_data.begin(), _data.end(),
                               [&](auto &a) { return *a == activity; });

        if (it == _data.end()) {
            return std::nullopt;
        }

        return std::distance(_data.begin(), it);
    }

    auto activity_list::search(std::string query) const -> bool {
        stg::string::strip_bounding_whitespaces(query);

        auto old_query = search_query;
        search_query = query;

        if (query.empty()) {
            auto was_updated = !search_results.empty() || !old_query.empty();
            search_results.clear();

            return was_updated;
        }

        query = string::utf8_fold_case(query);

        data_t results;
        std::for_each(begin(), end(), [&query, &results](auto &activity) {
            auto name = string::utf8_fold_case(activity->name());

            if (name.find(query) != std::string::npos)
                results.push_back(activity);
        });

        auto was_updated = old_query.empty() || search_results != results;

        search_results = results;

        return was_updated;
    }

    auto activity_list::filtered() const -> const data_t & {
        if (search_query.empty())
            return _data;

        return search_results;
    }

    auto activity_list::index_from_filtered(index_t index_in_filtered) const -> std::optional<index_t> {
        auto *activity = filtered().at(index_in_filtered).get();
        return index_of(activity);
    }

    auto activity_list::index_in_filtered(index_t activity_index) const -> std::optional<index_t> {
        const auto &activity = _data[activity_index];
        auto it = std::find(filtered().begin(), filtered().end(), activity);

        if (it == filtered().end())
            return std::nullopt;

        return std::distance(filtered().begin(), it);
    }


    void activity_list::reset_with(data_t data) {
        activity_list_base::reset_with(data);

        if (!search_query.empty())
            search(search_query);
    }

    auto activity_list::already_present_exception::what() const noexcept -> const char * {
        return message;
    }
}