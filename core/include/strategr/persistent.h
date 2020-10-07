//
// Created by Dmitry Khrykin on 22.07.2020.
//

#ifndef STRATEGR_PERSISTENT_H
#define STRATEGR_PERSISTENT_H

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "file_bookmark.h"
#include "notifications.h"
#include "utility"

namespace stg {
    template<typename T>
    struct is_plain_type {
        static constexpr bool value = std::is_standard_layout<T>::value && std::is_trivial<T>::value && !std::is_pointer<T>::value;
    };

#pragma mark - Serialization

    template<typename POD, std::enable_if_t<is_plain_type<POD>::value, int> = 0>
    auto serialize(POD i) -> raw_buffer {
        return raw_buffer((uint8_t *) &i, (uint8_t *) &i + sizeof(POD));
    }

    auto serialize(const raw_buffer &buff) -> raw_buffer;
    auto serialize(const std::string &str) -> raw_buffer;
    auto serialize(const std::vector<std::string> &vec) -> raw_buffer;
    auto serialize(const user_notifications::storage &storage) -> raw_buffer;

    template<typename T = void, typename = void>
    struct is_serializable : std::false_type {
    };

    template<typename T>
    struct is_serializable<T,
                           std::void_t<decltype(serialize(std::declval<T>()))>> : std::true_type {
    };

#pragma mark - Deserialization

    template<typename POD,
             std::enable_if_t<is_plain_type<POD>::value, int> = 0>
    auto deserialize(const uint8_t *&data) -> POD {
        auto pod = *reinterpret_cast<const POD *>(data);
        data += sizeof(POD);

        return pod;
    }

    template<typename T,
             std::enable_if_t<!is_plain_type<T>::value, int> = 0>
    auto deserialize(const uint8_t *&data) -> T;

    template<>
    auto deserialize(const uint8_t *&data) -> raw_buffer;

    template<>
    auto deserialize(const uint8_t *&data) -> std::string;

    template<>
    auto deserialize(const uint8_t *&data) -> std::vector<std::string>;

    template<>
    auto deserialize(const uint8_t *&data) -> user_notifications::storage;

    template<>
    auto deserialize(const uint8_t *&data) -> file_bookmark;

    template<typename T = void, typename = void>
    struct is_deserializable : std::false_type {
    };

    template<typename T>
    struct is_deserializable<T, std::void_t<decltype(deserialize<T>(std::declval<const uint8_t *&>()))>>
        : std::true_type {
    };

#pragma mark - Storage

    class persistent_storage {
    public:
        class backend {
        public:
            using result_callback_t = std::function<void(const void *)>;
            using setter_t = std::function<void(const std::string &key,
                                                void *data, size_t size)>;
            using getter_t = std::function<void(const std::string &key,
                                                const result_callback_t &result_callback)>;

            static void set_setter(setter_t setter);
            static void set_getter(getter_t getter);

        private:
            friend persistent_storage;

            static setter_t set;
            static getter_t get;
        };

        struct keys {
            static constexpr auto *default_strategy = "defaultStrategy";
        };

        static void assert_setup();

        template<typename T>
        static void set(const std::string &name, const T &data);

        template<typename T>
        static auto get(const std::string &name) -> std::unique_ptr<T>;

        static void test();
    };

    template<typename T>
    void persistent_storage::set(const std::string &name, const T &data) {
        assert_setup();

        static_assert(is_serializable<T>::value,
                      "stg::persistent_storage::set template parameter must be serializable");

        auto buffer = serialize(data);

        backend::set(name, static_cast<void *>(buffer.data()), buffer.size());
    }

    template<typename T>
    auto persistent_storage::get(const std::string &name) -> std::unique_ptr<T> {
        assert_setup();

        static_assert(is_deserializable<T>::value,
                      "stg::persistent_storage::get template parameter must be deserializable");

        std::unique_ptr<T> object_ptr = nullptr;
        backend::get(name, [&](const void *data) {
            if (data) {
                auto *bytes_ptr = static_cast<const uint8_t *>(data);
                auto object = deserialize<T>(bytes_ptr);

                object_ptr = std::make_unique<T>(std::move(object));
            }
        });

        return std::move(object_ptr);
    }
}

#endif//STRATEGR_PERSISTENT_H
