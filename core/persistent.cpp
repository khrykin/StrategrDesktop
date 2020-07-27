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
               "You've called a function that requires stg::persistent_storage, but it's backend hasn't been set up."
               "You have to call stg::persistent_storage::backend::set_setter and stg::persistent_storage::backend::set_getter with proper backend implementations before using this function.");
    }

#pragma mark - Serialization

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

#pragma mark - Deserialization

    template<>
    auto deserialize<std::string>(const uint8_t *data) -> std::string {
        auto size = deserialize<uint32_t>(data);
        data += sizeof(uint32_t);

        auto *begin = reinterpret_cast<const char *>(data);

        return std::string(begin, begin + size);
    }

    template<>
    auto deserialize<std::vector<std::string>>(const uint8_t *data) -> std::vector<std::string> {
        auto vec_size = deserialize<uint32_t>(data);
        data += sizeof(uint32_t);

        std::vector<std::string> vec(vec_size);
        for (auto &element : vec) {
            auto str = deserialize<std::string>(data);
            element = str;

            data += sizeof(uint32_t) + str.size() + 1;
        }

        return vec;
    }


    void persistent_storage::test() {
        auto *key = "stg::persistent_storage::test";
        {
            std::vector<std::string> strs = {"One", "Two"};
            set(key, strs);
        }

        auto strs = get<std::vector<std::string>>(key);
        std::vector<std::string> expected_strs = {"One", "Two"};
        assert(strs && "stg::persistent_storage::get returned nullptr for test data, check backend implementation");
        assert(*strs == expected_strs);
    }
}
