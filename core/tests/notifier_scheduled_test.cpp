//
// Created by Dmitry Khrykin on 31.07.2020.
//

#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <catch2/catch.hpp>

#include "strategy.h"
#include "notifier.h"
#include "persistent.h"

TEST_CASE("Notifier scheduled notifications", "[notifier][scheduled]") {
    using namespace stg;
    using namespace user_notifications;

    std::unordered_map<std::string, std::vector<uint8_t>> storage_mock;

    persistent_storage::backend::set_setter([&](const std::string &key,
                                                const void *data,
                                                size_t size) {
        storage_mock[key] = std::vector<uint8_t>((const uint8_t *) data,
                                                 (const uint8_t *) data + size);
    });

    persistent_storage::backend::set_getter([&](const std::string &key,
                                                const auto &result) {
        result(storage_mock[key].data());
    });

    std::unique_ptr<notifier::notifications_list> scheduled_notifications = nullptr;
    backend::set_scheduler([&](const auto &notifications) {
        scheduled_notifications = std::make_unique<notifier::notifications_list>(notifications);
    });

    std::unique_ptr<std::vector<std::string>> deleted_identifiers = nullptr;
    backend::set_resetter([&](const auto &identifiers) {
        deleted_identifiers = std::make_unique<std::vector<std::string>>(identifiers);
    });

    SECTION("initial scheduling") {
        {
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file.stg");

            REQUIRE(deleted_identifiers == nullptr);
            REQUIRE(scheduled_notifications->size() == 2);
        }

        auto strategy = stg::strategy();
        auto notifier = stg::notifier(strategy, "file.stg");

        // Should reschedule on every construction of notifier:
        REQUIRE(deleted_identifiers->size() == 2);
        REQUIRE(scheduled_notifications->size() == 2);
    }

    SECTION("scheduling on change") {
        auto strategy = stg::strategy();
        auto notifier = stg::notifier(strategy, "file.stg");

        auto expected_deleted_ids = std::vector<std::string>();
        std::transform(scheduled_notifications->begin(),
                       scheduled_notifications->end(),
                       std::back_inserter(expected_deleted_ids),
                       [](const user_notifications::notification &notification) {
                           return notification.identifier;
                       });

        strategy.add_activity(activity("Some"));
        strategy.place_activity(0, {0});

        REQUIRE(*deleted_identifiers == expected_deleted_ids);
        REQUIRE(scheduled_notifications->size() == 6);
    }

    SECTION("handles file renaming") {
        {
            // At the end of this scope, notifications for file.stg
            // will be persisted.
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file.stg");

        }

        {
            // At the end of this scope, notifications for file2.stg
            // will be persisted and notifications for file.stg must be removed
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file.stg");

            notifier.set_file("file2.stg");
        }

        {
            // Check normal re-scheduling scenario for file2.stg
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file2.stg");

            REQUIRE(deleted_identifiers->size() == 2);
            REQUIRE(scheduled_notifications->size() == 2);
        }

        {
            // Since file.stg has been renamed to file2.stg,
            // it should behave just like the initial scheduling
            // (no previous notificaions to delete)

            deleted_identifiers = nullptr;
            scheduled_notifications = nullptr;

            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file.stg");

            REQUIRE(deleted_identifiers == nullptr);
            REQUIRE(scheduled_notifications->size() == 2);
        }
    }

    SECTION("counts active strategies") {
        {
            // At the end of this scope, notifications for file.stg
            // will be persisted.
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file.stg");
        }

        {
            // At the end of this scope, notifications for file2.stg
            // will be persisted.
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file2.stg");
        }


        // Below are the cases that shouldn't appear as active:

        {
            // At the end of this scope, notifications for file3.stg
            // will be persisted.
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file3.stg");
        }

        notifier::note_file_removed("file3.stg");

        {
            // At the end of this scope, notifications for file4.stg
            // SHOULD NOT be persisted.
            auto strategy = stg::strategy();
            auto notifier = stg::notifier(strategy, "file4.stg");
            notifier.set_file(std::nullopt);
        }

        std::unordered_set<file_bookmark> expected_active_files = {
                "file.stg",
                "file2.stg"
        };

        REQUIRE(notifier::active_files() == expected_active_files);
    }

}