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
        std::cout << "is_serializable const char *: " << is_serializable<const char *>::value << "\n";
        std::cout << "is_serializable string: " << is_serializable<std::string>::value << "\n";
        std::cout << "is_serializable vec<string>: " << is_serializable<std::vector<std::string>>::value << "\n";
        std::cout << "is_serializable vec<int>: " << is_serializable<std::vector<int>>::value << "\n";

        std::cout << "is_deserializable string: " << is_deserializable<std::string>::value << "\n";
        std::cout << "is_deserializable vec<string>: " << is_deserializable<std::vector<std::string>>::value
                  << "\n";
        std::cout << "is_deserializable vec<int>: " << is_deserializable<std::vector<int>>::value << "\n";
        std::cout << "is_plain? const char *: " << is_plain_type<char const *>::value << "\n";

        {
            std::vector<std::string> strs = {"One", "Two", "Three"};

            set("strs", strs);
            set("char", "Просто текст");
            set("double", 2.03);
        }

        auto vec = get<std::vector<std::string>>("strs");

        if (!vec) {
            std::cout << "key is empty\n";
            return;
        }

        std::cout << "values:\n";

        for (auto &str : *vec) std::cout << "\"" << str << "\"\n";

        std::cout << "char: \"" << *get<std::string>("char") << "\"\n";
        std::cout << "double: \"" << *get<double>("double") << "\"\n";

    }
}
