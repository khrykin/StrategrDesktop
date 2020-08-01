//
// Created by Dmitry Khrykin on 31.07.2020.
//

#include <unordered_map>
#include <algorithm>

#include <catch2/catch.hpp>

#include "strategy.h"
#include "notifier.h"
#include "persistent.h"

TEST_CASE("Notifier scheduled notifications", "[notifier][scheduled]") {
    std::unordered_map<std::string, std::vector<uint8_t>> storage_mock;

    stg::persistent_storage::backend::set_setter([&](const std::string &key,
                                                     const void *data,
                                                     size_t size) {
        storage_mock[key] = std::vector<uint8_t>((const uint8_t *) data,
                                                 (const uint8_t *) data + size);
    });

    stg::persistent_storage::backend::set_getter([&](const std::string &key,
                                                     const auto &result) {
        result(storage_mock[key].data());
    });

    std::unique_ptr<std::vector<stg::notification>> scheduled_notifications = nullptr;
    std::unique_ptr<std::vector<std::string>> deleted_identifiers = nullptr;

    auto schedule_notifications = [&](const auto &notifications) {
        scheduled_notifications = std::make_unique<std::vector<stg::notification>>(notifications);
    };

    auto delete_notifications = [&](const auto &identifiers) {
        deleted_identifiers = std::make_unique<std::vector<std::string>>(identifiers);
    };

    SECTION("initial scheduling") {
        {
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy,
                                          "file.stg",
                                          schedule_notifications,
                                          delete_notifications);

            REQUIRE(deleted_identifiers->size() == 0);
            REQUIRE(scheduled_notifications->size() == 2);
        }

        auto strategy = stg::strategy();
        auto notifier = stg::notifier(strategy,
                                      "file.stg",
                                      schedule_notifications,
                                      delete_notifications);

        REQUIRE(deleted_identifiers->size() == 2);
        REQUIRE(scheduled_notifications->size() == 2);
    }

    SECTION("scheduling on change") {
        auto strategy = stg::strategy();
        auto notifier = stg::notifier(strategy,
                                      "file.stg",
                                      schedule_notifications,
                                      delete_notifications);

        auto expected_deleted_ids = std::vector<std::string>();
        std::transform(scheduled_notifications->begin(),
                       scheduled_notifications->end(),
                       std::back_inserter(expected_deleted_ids),
                       [](const stg::notification &notification) {
                           return notification.identifier;
                       });

        strategy.add_activity(stg::activity("Some"));
        strategy.place_activity(0, {0});

        REQUIRE(*deleted_identifiers == expected_deleted_ids);
        REQUIRE(scheduled_notifications->size() == 6);
    }

    SECTION("handles file rename") {
        {
            // At the end of this scope, notifications for file.stg
            // will be persisted.
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy,
                                          "file.stg",
                                          schedule_notifications,
                                          delete_notifications);

        }

        {
            // At the end of this scope, notifications for file2.stg
            // will be persisted and notifications for file.stg must be removed
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy,
                                          "file.stg",
                                          schedule_notifications,
                                          delete_notifications);

            notifier.set_filename("file2.stg");
        }

        {
            // Check normal re-scheduling scenario for file2.stg
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy,
                                          "file2.stg",
                                          schedule_notifications,
                                          delete_notifications);

            REQUIRE(deleted_identifiers->size() == 2);
            REQUIRE(scheduled_notifications->size() == 2);
        }

        {
            // Since file.stg has been renamed to file2.stg, it should
            // behave just like initial scheduling (no previous notificaions to delete)
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy,
                                          "file.stg",
                                          schedule_notifications,
                                          delete_notifications);

            REQUIRE(deleted_identifiers->size() == 0);
            REQUIRE(scheduled_notifications->size() == 2);
        }
    }

}