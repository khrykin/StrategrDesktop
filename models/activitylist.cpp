//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include "activitylist.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include "utility.h"

#include <iostream>
#include <algorithm>
#include <regex>
#include "utility.h"

void stg::activity_list::silently_add(const activity &activity) {
    if (has(activity)) {
        throw already_present_exception();
    }

    std::cout << "add_activity: " << activity << "\n";

    _data.push_back(std::make_shared<stg::activity>(activity));
}

void stg::activity_list::add(const activity &activity) {
    silently_add(activity);

    on_change_event();
}

void stg::activity_list::silently_remove_at_index(stg::activity_index index) {
    _data.erase(_data.begin() + index);
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
                   std::back_inserter(_data), [](auto &activity) {
                return std::make_shared<stg::activity>(activity);
            });
}

stg::activity_list::activity_list(const std::vector<std::shared_ptr<activity>> &from_vector) {
    std::transform(from_vector.begin(),
                   from_vector.end(),
                   std::back_inserter(_data), [](auto &activity) {
                return activity;
            });
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

stg::activity_list
stg::activity_list::search(std::string query) const {
    query = std::regex_replace(query, std::regex("^\\s*"), "");
    query = std::regex_replace(query, std::regex("\\s*$"), "");

    if (query.empty()) {
        return activity_list{};
    }

    query = text::utf8_fold_case(query);

    std::vector<std::shared_ptr<activity>> results;
    std::copy_if(begin(),
                 end(),
                 std::back_inserter(results), [&query](auto activity) {
                auto name = activity->name();

                name = text::utf8_fold_case(name);

                return name.find(query) != std::string::npos;
            });

    return activity_list(results);
}

const char *stg::activity_list::already_present_exception::what() const noexcept {
    return message;
}
