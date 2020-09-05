//
// Created by Dmitry Khrykin on 30.07.2020.
//

#include <algorithm>

#include <catch2/catch.hpp>

#include "strategy.h"
#include "notifier.h"
#include "time_utils.h"

constexpr auto immediate_seconds_interval = stg::notifier::immediate_seconds_interval;
constexpr auto prepare_seconds_interval = stg::notifier::prepare_seconds_interval;

inline auto require_notification_type(const stg::user_notifications::notification &notification,
                                      stg::notification_type type) {
    REQUIRE(notification.user_info_as<stg::notification_type>() != nullptr);
    REQUIRE(*notification.user_info_as<stg::notification_type>() == type);
}

TEST_CASE("Notifier immediate notifications", "[notifier][immediate]") {
    using namespace stg;
    using namespace user_notifications;

    // Timer backend mock
    std::unordered_map<size_t, std::function<void()>> timers;
    stg::timer::backend::set_scheduler([&timers](auto seconds, auto callback) {
        auto timer_id = (void *) timers.size();
        auto timer_callback = [=]() { callback(timer_id); };

        timers[(size_t) timer_id] = timer_callback;

        return timer_id;
    });

    stg::timer::backend::set_invalidator([&timers](void *timer_impl_ptr) {
        auto timer_id = (size_t) timer_impl_ptr;
        timers.erase(timer_id);
    });

    // Time source mock
    auto current_seconds = 0;
    stg::time_utils::set_time_source([&current_seconds]() {
        return current_seconds;
    });

    // Notifier backend mock
    std::unique_ptr<notification> sent_notification = nullptr;
    backend::set_immediate_sender([&sent_notification](const notification &n) {
        sent_notification = std::make_unique<notification>(n);
    });

    auto strategy = stg::strategy();

    const auto polling_interval = 5;
    const auto test_immediate_notification = [&](auto type,
                                                 auto delivery_time,
                                                 unsigned int margin = 1) {
        // This is the time at which we've "opened" the strategy
        // (Just before the first upcoming notification time)
        current_seconds = delivery_time - margin;

        auto notifier = stg::notifier(strategy);

        notifier.start_polling(polling_interval);

        // This is the time at which the first polling callback will be executed
        // (Just after the first upcoming notification time)
        current_seconds = delivery_time + margin;
        timers[0]();

        REQUIRE(sent_notification != nullptr);
        require_notification_type(*sent_notification, type);
    };

    SECTION("strategy end") {
        SECTION("prepare") {
            test_immediate_notification(stg::notification_type::prepare_strategy_end,
                                        strategy.end_time() * 60 - prepare_seconds_interval);
        }

        SECTION("now") {
            test_immediate_notification(stg::notification_type::strategy_end,
                                        strategy.end_time() * 60 - immediate_seconds_interval);
        }
    }

    SECTION("session begin") {
        strategy.add_activity(stg::activity("Some"));
        strategy.place_activity(0, {3, 4});

        SECTION("prepare") {
            auto delivery_time = strategy.time_slots()[3].begin_time * 60 - prepare_seconds_interval;
            test_immediate_notification(stg::notification_type::prepare_start, delivery_time);
        }

        SECTION("now") {
            auto delivery_time = strategy.time_slots()[3].begin_time * 60 - immediate_seconds_interval;
            test_immediate_notification(stg::notification_type::start, delivery_time);
        }
    }

    SECTION("session end") {
        strategy.add_activity(stg::activity("Some"));
        strategy.place_activity(0, {3, 4});

        SECTION("prepare") {
            auto delivery_time = strategy.time_slots()[4].end_time() * 60 - prepare_seconds_interval;
            test_immediate_notification(stg::notification_type::prepare_end, delivery_time);
        }

        SECTION("now") {
            auto delivery_time = strategy.time_slots()[4].end_time() * 60 - immediate_seconds_interval;
            test_immediate_notification(stg::notification_type::end, delivery_time);
        }
    }

    SECTION("session change") {
        strategy.add_activity(stg::activity("Some 1"));
        strategy.add_activity(stg::activity("Some 2"));

        strategy.place_activity(0, {3, 4});
        strategy.place_activity(1, {5, 6});

        SECTION("prepare") {
            auto delivery_time = strategy.time_slots()[5].begin_time * 60 - prepare_seconds_interval;
            test_immediate_notification(stg::notification_type::prepare_start, delivery_time);
        }

        SECTION("now") {
            auto delivery_time = strategy.time_slots()[5].begin_time * 60 - immediate_seconds_interval;
            test_immediate_notification(notification_type::start, delivery_time);
        }
    }

    SECTION("updates on slots change") {
        strategy.add_activity(stg::activity("Some 1"));
        strategy.place_activity(0, {1, 2});

        current_seconds = strategy.begin_time() * 60 - prepare_seconds_interval - 5;

        auto notifier = stg::notifier(strategy);

        notifier.start_polling(polling_interval);

        // We made some changes after the polling started,
        // but before the first polling timer fires:
        strategy.edit_activity(0, stg::activity("Some 2"));
        strategy.place_activity(0, {0});

        current_seconds = strategy.begin_time() * 60 - prepare_seconds_interval + 1;
        timers[0]();


        REQUIRE(sent_notification != nullptr);
        require_notification_type(*sent_notification, notification_type::prepare_start);
        REQUIRE(sent_notification->title.find("Some 2") != std::string::npos);
    }

    SECTION("doesn't send stale notification") {
        strategy.add_activity(stg::activity("Some"));
        strategy.place_activity(0, {0});

        // This is the time at which we've "opened" the strategy
        // (Just before the first upcoming notification time)
        current_seconds = strategy.begin_time() * 60 - prepare_seconds_interval - 1;

        auto notifier = stg::notifier(strategy);

        notifier.start_polling(polling_interval);

        // Here, the first notification is sent
        current_seconds = strategy.begin_time() * 60 - prepare_seconds_interval + 1;
        timers[0]();

        REQUIRE(sent_notification != nullptr);
        require_notification_type(*sent_notification, stg::notification_type::prepare_start);

        REQUIRE(sent_notification->title.find("Some") != std::string::npos);

        // Imitate run loop running timer callback until
        // the new notification's delivery time
        while (current_seconds <= strategy.begin_time() * 60 - immediate_seconds_interval) {
            current_seconds += polling_interval;

            timers[0]();
        }

        REQUIRE(sent_notification != nullptr);
        require_notification_type(*sent_notification, notification_type::start);
        REQUIRE(sent_notification->title.find("Some") != std::string::npos);
    }

    SECTION("handles system time changes") {
        strategy.add_activity(stg::activity("Some"));
        strategy.place_activity(0, {1});

        // This is the time at which we've "opened" the strategy.
        current_seconds = strategy.end_time() * 60 - 1;

        auto notifier = stg::notifier(strategy);

        notifier.start_polling(polling_interval);

        // First timer callback since polling has started.
        current_seconds += polling_interval;
        timers[0]();

        // Imitation of system time change:
        current_seconds = strategy.time_slots()[1].begin_time * 60 - prepare_seconds_interval - 1;

        // First timer callback after the change that occurs before next delivery time.
        timers[0]();

        // Second timer callback after the change,should send expected notification.
        current_seconds += polling_interval;
        timers[0]();

        REQUIRE(sent_notification != nullptr);
        require_notification_type(*sent_notification, notification_type::prepare_start);
    }

    stg::time_utils::set_time_source(nullptr);
}