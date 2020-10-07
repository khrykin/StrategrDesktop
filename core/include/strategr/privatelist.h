//
// Created by Dmitry Khrykin on 2019-07-04.
//

#ifndef PRIVATELIST
#define PRIVATELIST

#include <functional>
#include <optional>
#include <sstream>
#include <vector>

namespace stg {
    template<class Item>
    class private_list {
    public:
        using index_t = int;
        using item_t = Item;
        using data_t = std::vector<item_t>;

        using const_iterator = typename data_t::const_iterator;

        explicit private_list(data_t data = {})
            : _data(std::move(data)) {}

        virtual void reset_with(data_t data) {
            _data = std::move(data);
        }

        const data_t &data() const {
            return _data;
        }

        const_iterator begin() const {
            return _data.cbegin();
        };

        const_iterator end() const {
            return _data.cend();
        };

        std::optional<index_t> index_of(const item_t &item) const {
            auto it = find_const(item);
            auto item_is_present = it != _data.end();

            if (item_is_present) {
                return std::distance(_data.begin(), it);
            } else {
                return std::nullopt;
            }
        };

        index_t size() const {
            return static_cast<index_t>(_data.size());
        }

        const item_t &operator[](index_t item_index) const {
            return _data[item_index];
        }

        const item_t first() const {
            return _data.front();
        }

        const item_t last() const {
            return _data.back();
        }

        bool empty() const {
            return _data.empty();
        }

        bool has_index(index_t item_index) const {
            if (_data.empty()) {
                return false;
            }

            return item_index >= 0 && item_index < static_cast<index_t>(_data.size());
        }

        bool has_indices(index_t item_index) const {
            return has_index(item_index);
        }

        template<typename... Indices>
        bool has_indices(index_t item_index, Indices... indices) const {
            return has_index(item_index) && has_indices(indices...);
        }

        friend bool operator==(const private_list<Item> &lhs,
                               const private_list<Item> &rhs) {
            if (lhs.size() != rhs.size()) {
                return false;
            }

            for (auto item_index = 0; item_index < lhs.size(); item_index++) {
                if (lhs[item_index] != rhs[item_index]) {
                    return false;
                }
            }

            return true;
        };

        friend bool operator!=(const private_list<Item> &lhs,
                               const private_list<Item> &rhs) {
            return !(lhs == rhs);
        }

    protected:
        using iterator = typename data_t::iterator;

        data_t _data = {};

        iterator find(const item_t &item) {
            return std::find(_data.begin(), _data.end(), item);
        };

        const_iterator find_const(const item_t &item) const {
            return std::find(_data.begin(), _data.end(), item);
        };
    };
}

#endif//PRIVATELIST
