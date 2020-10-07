//
// Created by Dmitry Khrykin on 26.08.2020.
//

#include "file_bookmark.h"

#include <utility>


namespace stg {
#pragma mark - Construction

    file_bookmark::file_bookmark(raw_buffer data)
        : data(std::move(data)) {}

    file_bookmark::file_bookmark(const std::string &path)
        : data(path.begin(), path.end()) {}

    file_bookmark::file_bookmark(const char *data)
        : file_bookmark(std::string(data)) {}

#pragma mark - Conversions

    auto file_bookmark::to_string() const -> std::string {
        return {data.begin(), data.end()};
    }

#pragma mark - Getting Hash

    auto file_bookmark::hash() const -> size_t {
        return std::hash<std::string>()(to_string());
    }

#pragma mark - Operators

    auto operator==(const file_bookmark &lhs, const file_bookmark &rhs) -> bool {
        return lhs.data == rhs.data;
    }

    auto operator!=(const file_bookmark &lhs, const file_bookmark &rhs) -> bool {
        return !(lhs == rhs);
    }

    auto operator<<(std::ostream &os, const file_bookmark &bookmark) -> std::ostream & {
        os << "bookmark(\"" << bookmark.to_string() << "\")";

        return os;
    }
}
