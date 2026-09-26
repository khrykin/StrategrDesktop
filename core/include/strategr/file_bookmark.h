//
// Created by Dmitry Khrykin on 26.08.2020.
//

#ifndef STRATEGR_FILE_BOOKMARK_H
#define STRATEGR_FILE_BOOKMARK_H

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

#include "utility.h"

#if defined(__APPLE__) && defined(__OBJC__)
#include <Foundation/Foundation.h>
#endif

namespace stg {

    class file_bookmark {
    public:
#pragma mark - Construction

        explicit file_bookmark(raw_buffer data);
        file_bookmark(const std::string &path);

#if defined(__APPLE__) && defined(__OBJC__)
        file_bookmark(NSURL *url);
#endif

#pragma mark - Conversions

        auto to_string() const -> std::string;

#if defined(__APPLE__) && defined(__OBJC__)
        auto to_nsurl(bool *is_stale) const -> NSURL *;
#endif

#pragma mark - Getting Hash

        auto hash() const -> size_t;

    private:
        std::vector<uint8_t> data;

        friend auto operator==(const file_bookmark &lhs, const file_bookmark &rhs) -> bool;
        friend auto operator!=(const file_bookmark &lhs, const file_bookmark &rhs) -> bool;
        friend auto operator<<(std::ostream &os, const file_bookmark &bookmark) -> std::ostream &;

        friend auto serialize(const file_bookmark &bookmark) -> raw_buffer;
    };
}

namespace std {
    template<>
    class hash<stg::file_bookmark> {
    public:
        auto operator()(const stg::file_bookmark &bookmark) const -> size_t {
            return bookmark.hash();
        }
    };
}


#endif//STRATEGR_FILE_BOOKMARK_H
