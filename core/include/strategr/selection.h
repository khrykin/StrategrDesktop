//
// Created by Dmitry Khrykin on 2020-01-30.
//

#ifndef STRATEGR_SELECTION_H
#define STRATEGR_SELECTION_H

#include <functional>
#include <optional>
#include <vector>

#include "notifiableonchange.h"
#include "strategy.h"

namespace stg {
    using index_t = strategy::time_slot_index_t;
    using flat_selection = std::vector<index_t>;
    using grouped_selection = std::vector<flat_selection>;

    class selection : public notifiable_on_change {
    public:
        explicit selection(const stg::strategy &strategy);

        void set_selected_at(index_t slot_index, bool is_selected);
        void reset_with(std::vector<index_t> slot_indices);

        void toggle_at(index_t slot_index);
        void deselect_all();
        void select_all();
        void fill(index_t from_index, index_t to_index);

        bool is_continuous() const;

        bool only_empty_selected() const;
        bool only_non_empty_selected() const;

        bool has_selected(index_t slot_index) const;
        bool is_all_selected() const;

        bool is_boundary(index_t slot_index) const;

        void reload();

        bool is_clicked() const;
        void set_is_clicked(bool is_clicked);

        auto size() const -> time_slots_state::size_t;
        auto empty() const -> bool;

        auto flat() const -> const flat_selection &;
        auto grouped() const -> const grouped_selection &;

        auto front() const -> const index_t &;
        auto back() const -> const index_t &;

    private:
        const stg::strategy &strategy;

        flat_selection _flat;
        grouped_selection _grouped;

        void make_safe(int &index);

        std::function<void(int)> get_slot_height;

        bool _is_clicked = false;
    };
}

#endif//STRATEGR_SELECTION_H
