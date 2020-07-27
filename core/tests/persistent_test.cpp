//
// Created by Dmitry Khrykin on 27.07.2020.
//

#include <map>
#include <vector>
#include <algorithm>

#include <catch2/catch.hpp>

#include "persistent.h"

TEST_CASE("Persistent Storage", "[persistent]") {
    std::map<std::string, std::vector<uint8_t>> storage_mock;

    stg::persistent_storage::backend::set_setter([&](const std::string &key,
                                                     const void *data,
                                                     size_t size) {
        storage_mock[key] = std::vector<uint8_t>((const uint8_t *) data, (const uint8_t *) data + size);
    });

    stg::persistent_storage::backend::set_getter([&](const std::string &key,
                                                     const auto &result) {
        result(storage_mock[key].data());
    });

    stg::persistent_storage::test();

    SECTION("non-existent key") {
        REQUIRE(stg::persistent_storage::get<std::string>("some") == nullptr);
    }

    SECTION("std::string") {
        {
            std::string test_string = "test std::string";
            stg::persistent_storage::set("std::string", test_string);
            stg::persistent_storage::set("const char *", "test const char *");
        }

        REQUIRE(*stg::persistent_storage::get<std::string>("std::string") == "test std::string");
        REQUIRE(*stg::persistent_storage::get<std::string>("const char *") == "test const char *");
    }

    SECTION("std::vector<std::string>") {
        {
            std::vector<std::string> test_strings = {"Test 1", "Тест 2"};
            stg::persistent_storage::set("std::vector", test_strings);
        }

        std::vector<std::string> expected_strings = {"Test 1", "Тест 2"};
        REQUIRE(*stg::persistent_storage::get<std::vector<std::string>>("std::vector") == expected_strings);
    }

}
