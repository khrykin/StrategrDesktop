//
// Created by Dmitry Khrykin on 2019-07-04.
//

#ifndef MODELS_PRIVATELIST_HPP
#define MODELS_PRIVATELIST_HPP

#include <functional>
#include <vector>
#include <optional>
#include <sstream>

template<class ItemType>
class PrivateList {
public:
    using Index = unsigned;
    using Item = ItemType;
    using value_type = Item;
    using RawData = std::vector<Item>;

    using const_iterator = typename RawData::const_iterator;

    explicit PrivateList(RawData data = {})
            : _data(std::move(data)) {}

    void resetWith(RawData data = {}) {
        _data = data;
    }

    const RawData &data() const {
        return _data;
    }

    const_iterator begin() const {
        return _data.cbegin();
    };

    const_iterator end() const {
        return _data.cend();
    };

    std::optional<Index> indexOf(const Item &item) const {
        auto it = findConst(item);
        auto itemIsPresent = it != _data.end();

        if (itemIsPresent) {
            return std::distance(_data.begin(), it);
        } else {
            return std::nullopt;
        }
    };

    size_t size() const {
        return _data.size();
    }

    const Item &operator[](Index itemIndex) const {
        return _data[itemIndex];
    }

    const Item first() const {
        return _data.front();
    }

    const Item last() const {
        return _data.back();
    }

    bool isEmpty() const {
        return _data.empty();
    }

    bool hasIndex(Index itemIndex) const {
        if (_data.empty()) {
            return false;
        }

        return itemIndex >= 0
               && itemIndex < _data.size();
    }

    bool hasIndices(Index itemIndex) const {
        return hasIndex(itemIndex);
    }

    template<typename ...Indices>
    bool hasIndices(Index itemIndex, Indices... indices) const {
        return hasIndex(itemIndex) && hasIndices(indices...);
    }

    friend bool operator==(const PrivateList<ItemType> &lhs,
                           const PrivateList<ItemType> &rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (auto itemIndex = 0;
             itemIndex < lhs.size();
             itemIndex++) {
            if (lhs[itemIndex] != rhs[itemIndex]) {
                return false;
            }
        }


        return true;
    };

    friend bool operator!=(const PrivateList<ItemType> &lhs,
                           const PrivateList<ItemType> &rhs) {
        return !(lhs == rhs);
    }

protected:
    using iterator = typename RawData::iterator;

    RawData _data = {};

    iterator find(const Item &item) {
        return std::find(_data.begin(), _data.end(), item);
    };

    const_iterator findConst(const Item &item) const {
        return std::find(_data.begin(), _data.end(), item);
    };
};

#endif //MODELS_PRIVATELIST_HPP
