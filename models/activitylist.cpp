//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include <algorithm>
#include <regex>

#include "activitylist.h"
#include "utility.h"

void stg::activity_list::silently_add(const activity &activity) {
    if (has(activity)) {
        throw already_present_exception();
    }

    _data.push_back(std::make_shared<stg::activity>(activity));

    if (!search_query.empty())
        search(search_query);
}

void stg::activity_list::add(const activity &activity) {
    silently_add(activity);

    on_change_event();
}

void stg::activity_list::silently_remove_at_index(stg::activity_index index) {
    _data.erase(_data.begin() + index);

    if (!search_query.empty())
        search(search_query);
}

void stg::activity_list::remove_at_index(activity_index index) {
    silently_remove_at_index(index);
    on_change_event();
}

void stg::activity_list::silently_edit_at_index(activity_index index, const activity &new_activity) {
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

void stg::activity_list::edit_at_index(activity_index index, const activity &new_activity) {
    if (*_data[index] == new_activity) {
        return;
    }

    silently_edit_at_index(index, new_activity);

    on_change_event();
}

bool stg::activity_list::has(const activity &searched_activity) const {
    for (const auto &activity : _data) {
        if (*activity == searched_activity) {
            return true;
        }
    }

    return false;
}

void stg::activity_list::silently_drag(activity_index from_index, activity_index to_index) {
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

void stg::activity_list::drag(activity_index from_index, activity_index to_index) {
    silently_drag(from_index, to_index);

    on_change_event();
}

std::string stg::activity_list::class_print_name() const {
    return "activity_list";
}

stg::activity_list::activity_list(const std::vector<activity> &from_vector) {
    std::transform(from_vector.begin(),
                   from_vector.end(),
                   std::back_inserter(_data),
                   [](auto &activity) {
                       return std::make_shared<stg::activity>(activity);
                   });

    if (!search_query.empty())
        search(search_query);
}

stg::activity_list::activity_list(const std::vector<std::shared_ptr<activity>> &from_vector) {
    std::transform(from_vector.begin(),
                   from_vector.end(),
                   std::back_inserter(_data),
                   [](auto &activity) {
                       return activity;
                   });

    if (!search_query.empty())
        search(search_query);
}

const stg::activity &stg::activity_list::operator[](activity_index item_index) const {
    return *_data[item_index];
}

stg::activity *stg::activity_list::at(activity_index item_index) const {
    return _data.at(item_index).get();
}

std::optional<stg::activity_list::index_t>
stg::activity_list::index_of(const activity *activity) const {
    auto it = std::find_if(_data.begin(), _data.end(), [=](auto &a) {
        return a.get() == activity;
    });

    if (it == _data.end()) {
        return std::nullopt;
    }

    return std::distance(_data.begin(), it);
}

std::optional<stg::activity_list::index_t>
stg::activity_list::index_of(const activity &activity) const {
    auto it = std::find_if(_data.begin(), _data.end(), [&](auto &a) {
        return *a == activity;
    });

    if (it == _data.end()) {
        return std::nullopt;
    }

    return std::distance(_data.begin(), it);
}

bool stg::activity_list::search(std::string query) const {
    text::strip_bounding_whitespaces(query);

    search_query = query;

    if (query.empty()) {
        auto was_updated = !search_results.empty();
        search_results.clear();

        return was_updated;
    }

    query = text::utf8_fold_case(query);

    activity_list::data_t results;
    std::copy_if(begin(),
                 end(),
                 std::back_inserter(results),
                 [&query](auto activity) {
                     auto name = text::utf8_fold_case(activity->name());
                     return name.find(query) != std::string::npos;
                 });

    auto was_updated = search_results != results;

    search_results = results;

    return was_updated;
}

const stg::activity_list::data_t &stg::activity_list::filtered() const {
    if (search_query.empty())
        return _data;

    return search_results;
}

std::optional<stg::activity_list::index_t>
stg::activity_list::index_from_filtered(index_t index_in_filtered) const {
    auto *activity = filtered().at(index_in_filtered).get();
    return index_of(activity);
}

std::optional<stg::activity_list::index_t>
stg::activity_list::index_in_filtered(index_t activity_index) const {
    auto activity = _data[activity_index];
    auto it = std::find(filtered().begin(),
                        filtered().end(),
                        activity);

    if (it == filtered().end())
        return std::nullopt;

    return std::distance(filtered().begin(), it);
}


void stg::activity_list::reset_with(data_t data) {
    activity_list_base::reset_with(data);

    if (!search_query.empty())
        search(search_query);
}

const char *stg::activity_list::already_present_exception::what() const noexcept {
    return message;
}
