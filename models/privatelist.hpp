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

    using const_iterator = typename std::vector<Item>::const_iterator;

    explicit PrivateList(std::vector<Item> vectorImpl = {})
            : _vector(std::move(vectorImpl)) {}

    const_iterator begin() const {
        return _vector.cbegin();
    };

    const_iterator end() const {
        return _vector.cend();
    };

    std::optional<Index> indexOf(const Item &item) const {
        auto it = findConst(item);
        auto itemIsPresent = it != _vector.end();

        if (itemIsPresent) {
            return std::distance(_vector.begin(), it);
        } else {
            return std::nullopt;
        }
    };

    size_t size() const {
        return _vector.size();
    }

    const Item &operator[](Index itemIndex) const {
        return _vector[itemIndex];
    }

    const Item first() const {
        return _vector.front();
    }

    const Item last() const {
        return _vector.back();
    }

    bool empty() const {
        return _vector.empty();
    }

    bool hasIndex(Index itemIndex) const {
        if (_vector.empty()) {
            return false;
        }

        return itemIndex >= 0
               && itemIndex < _vector.size();
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
    using iterator = typename std::vector<Item>::iterator;

    std::vector<Item> _vector = {};

    iterator find(const Item &item) {
        return std::find(_vector.begin(), _vector.end(), item);
    };

    const_iterator findConst(const Item &item) const {
        return std::find(_vector.begin(), _vector.end(), item);
    };
};

#endif //MODELS_PRIVATELIST_HPP
