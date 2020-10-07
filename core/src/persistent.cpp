//
// Created by Dmitry Khrykin on 22.07.2020.
//

#include "persistent.h"

namespace stg {
    persistent_storage::backend::setter_t persistent_storage::backend::set = nullptr;
    persistent_storage::backend::getter_t persistent_storage::backend::get = nullptr;

    void persistent_storage::backend::set_setter(setter_t fn) {
        set = std::move(fn);
    }

    void persistent_storage::backend::set_getter(getter_t fn) {
        get = std::move(fn);
    }

    void persistent_storage::assert_setup() {
        assert((backend::set && backend::get) &&
               "You've called a function that requires stg::persistent_storage, but it's backend hasn't been set up. You have to call stg::persistent_storage::backend::set_setter and stg::persistent_storage::backend::set_getter with proper backend implementations before using this function.");
    }

#pragma mark - Serialization

    auto serialize(const raw_buffer &buff) -> raw_buffer {
        raw_buffer result;
        auto size_buff = serialize((uint32_t) buff.size());
        result.insert(result.end(), size_buff.begin(), size_buff.end());

        result.insert(result.end(), buff.begin(), buff.end());

        return result;
    }

    auto serialize(const std::string &str) -> raw_buffer {
        raw_buffer buff(sizeof(uint32_t) + str.size() + 1);

        auto output_it = buff.begin();

        auto size_buff = serialize((uint32_t) str.size());
        std::move(size_buff.begin(), size_buff.end(), output_it);
        output_it += sizeof(uint32_t);

        std::move(str.begin(), str.end(), output_it);

        return buff;
    }

    auto serialize(const std::vector<std::string> &vec) -> raw_buffer {
        raw_buffer buff;

        auto size_buff = serialize((uint32_t) vec.size());
        buff.insert(buff.end(), size_buff.begin(), size_buff.end());

        for (auto &str : vec) {
            auto str_buf = serialize(str);
            buff.insert(buff.end(), str_buf.begin(), str_buf.end());
        }

        return buff;
    }

    auto serialize(const user_notifications::storage &storage) -> raw_buffer {
        raw_buffer buff;

        auto size_buff = serialize((uint32_t) storage.size());
        buff.insert(buff.end(), size_buff.begin(), size_buff.end());

        for (auto &[bookmark, ids] : storage) {
            auto bookmark_buff = serialize(bookmark);
            buff.insert(buff.end(), bookmark_buff.begin(), bookmark_buff.end());

            auto ids_buff = serialize(ids);
            buff.insert(buff.end(), ids_buff.begin(), ids_buff.end());
        }

        return buff;
    }

    auto serialize(const file_bookmark &bookmark) -> raw_buffer {
        return serialize(bookmark.data);
    }

#pragma mark - Deserialization

    template<>
    auto deserialize(const uint8_t *&data) -> raw_buffer {
        auto size = deserialize<uint32_t>(data);
        auto *begin = reinterpret_cast<const uint8_t *>(data);

        data += size;

        return {begin, begin + size};
    }

    template<>
    auto deserialize(const uint8_t *&data) -> std::string {
        auto size = deserialize<uint32_t>(data);
        auto *begin = reinterpret_cast<const char *>(data);

        auto str = std::string(begin, begin + size);
        data += str.size() + 1;

        return str;
    }

    template<>
    auto deserialize(const uint8_t *&data) -> std::vector<std::string> {
        auto vec_size = deserialize<uint32_t>(data);

        std::vector<std::string> vec(vec_size);
        for (auto &element : vec) {
            element = deserialize<std::string>(data);
        }

        return vec;
    }

    template<>
    auto deserialize(const uint8_t *&data) -> user_notifications::storage {
        auto dict_size = deserialize<uint32_t>(data);

        user_notifications::storage storage;

        for (auto i = 0; i < dict_size; i++) {
            auto bookmark = deserialize<file_bookmark>(data);
            auto identifiers = deserialize<std::vector<std::string>>(data);

            storage.insert(bookmark, identifiers);
        }

        return storage;
    }

    template<>
    auto deserialize(const uint8_t *&data) -> file_bookmark {
        return deserialize<raw_buffer>(data);
    }

    void persistent_storage::test() {
        auto *key = "stg::persistent_storage::test";
        {
            std::vector<std::string> strs = {"One", "Two"};
            set(key, strs);
        }

        auto strs = get<std::vector<std::string>>(key);
        std::vector<std::string> expected_strs = {"One", "Two"};
        assert("stg::persistent_storage::get returned nullptr for test data, check backend implementation" && strs);

        if (strs) {
            assert("stg::persistent_storage::get was unable to get back valid test data from storage" &&
                   *strs == expected_strs);
        }
    }
}
