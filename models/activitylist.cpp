//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include "activitylist.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include "utility.h"

#include "activitylist.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include "utility.h"

void stg::activity_list::add(const activity &activity) {
    if (has(activity)) {
        throw already_present_exception();
    }


    _data.push_back(std::make_shared<stg::activity>(activity));

    on_change_event();
}

void stg::activity_list::remove_at_index(activity_index index) {
    _data.erase(_data.begin() + index);

    on_change_event();
}

void stg::activity_list::edit_at_index(activity_index index, const activity &new_activity) {
    if (has(new_activity)) {
        throw already_present_exception();
    }

    _data[index] = std::make_shared<activity>(new_activity);

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

void stg::activity_list::drag(activity_index from_index, activity_index to_index) {
    if (from_index == to_index) {
        return;
    }

    if (to_index < from_index) {
        rotate_left(from_index, to_index);
    } else {
        rotate_right(from_index, to_index);
    }

    on_change_event();
}

void stg::activity_list::rotate_left(activity_index from_index, activity_index to_index) {
    rotate(_data.begin() + to_index,
           _data.begin() + from_index,
           _data.end());
}

void stg::activity_list::rotate_right(activity_index from_index, activity_index to_index) {
    auto last_index = _data.size() - 1;
    auto from_index_right = last_index - from_index;
    auto to_index_right = last_index - to_index;

    rotate(_data.rbegin() + to_index_right,
           _data.rbegin() + from_index_right,
           _data.rend());
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
    for (auto activity_index = 0; activity_index < size(); activity_index++) {
        if (_data[activity_index].get() == activity) {
            return activity_index;
        }
    }

    return std::nullopt;
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