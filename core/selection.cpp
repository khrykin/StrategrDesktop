//
// Created by Dmitry Khrykin on 2020-01-30.
//

#include <algorithm>

#include "selection.h"

stg::selection::selection(const stg::strategy &strategy)
    : strategy(strategy) {
    add_on_change_callback([this] { reload(); });
}

void stg::selection::set_selected_at(index_t slot_index, bool is_selected) {
    make_safe(slot_index);

    auto found_it = std::find(begin(), end(), slot_index);
    auto already_selected = found_it != end();

    if (already_selected && is_selected) {
        return;
    } else if (already_selected && !is_selected) {
        erase(found_it);
    } else if (is_selected) {
        push_back(slot_index);
        std::sort(begin(), end());
    }

    on_change_event();
}

void stg::selection::reset_with(std::vector<index_t> slot_indices) {
    assign(slot_indices.begin(), slot_indices.end());
    std::sort(begin(), end());

    on_change_event();
}


void stg::selection::toggle_at(index_t slot_index) {
    make_safe(slot_index);

    auto found_it = std::find(begin(), end(), slot_index);
    auto already_selected = found_it != end();

    if (already_selected) {
        erase(found_it);
    } else {
        push_back(slot_index);
        std::sort(begin(), end());
    }

    on_change_event();
}

void stg::selection::deselect_all() {
    clear();
    _is_clicked = false;

    on_change_event();
}

void stg::selection::select_all() {
    resize(strategy.number_of_time_slots());

    std::iota(begin(), end(), 0);

    on_change_event();
}

void stg::selection::fill(index_t from_index, index_t to_index) {
    make_safe(from_index);
    make_safe(to_index);

    if (from_index > to_index) {
        std::swap(from_index, to_index);
    }

    for (auto i = from_index; i <= to_index; i++) {
        if (!has_selected(i))
            push_back(i);
    }

    std::sort(begin(), end());

    on_change_event();
}

bool stg::selection::is_continuous() const {
    return _grouped.size() == 1;
}

bool stg::selection::only_empty_selected() const {
    return std::find_if(begin(), end(), [this](auto index) {
               return strategy.time_slots()[index].activity != stg::strategy::no_activity;
           }) == end();
}


bool stg::selection::only_non_empty_selected() const {
    return std::find_if(begin(), end(), [this](auto index) {
               return strategy.time_slots()[index].activity == stg::strategy::no_activity;
           }) == end();
}

bool stg::selection::has_selected(index_t slot_index) const {
    return std::find(begin(), end(), slot_index) != end();
}

void stg::selection::reload() {
    grouped_selection new_grouped;
    grouped_selection_element current_item;

    for (auto i = begin(); i != end(); ++i) {
        auto current_index = *i;
        auto previous = i != begin()
                            ? std::make_optional(*prev(i))
                            : std::nullopt;

        if (!previous || current_index == *previous + 1) {
            current_item.push_back(current_index);
        } else {
            new_grouped.push_back(current_item);
            current_item = {current_index};
        }

        if (i == prev(end())) {
            new_grouped.push_back(current_item);
        }
    }

    _grouped = new_grouped;
}

bool stg::selection::is_clicked() const {
    return _is_clicked;
}

void stg::selection::set_is_clicked(bool is_clicked) {
    _is_clicked = is_clicked;

    on_change_event();
}

const stg::grouped_selection &stg::selection::grouped() const {
    return _grouped;
}

bool stg::selection::is_all_selected() const {
    return size() == strategy.number_of_time_slots();
}

bool stg::selection::is_boundary(stg::index_t slot_index) const {
    return (!has_selected(slot_index - 1) && has_selected(slot_index)) || (has_selected(slot_index - 1) && !has_selected(slot_index));
}

void stg::selection::make_safe(int &index) {
    if (index < 0) {
        index = 0;
    } else if (index > strategy.number_of_time_slots() - 1) {
        index = strategy.number_of_time_slots() - 1;
    }
}
