//
// Created by Dmitry Khrykin on 2019-11-26.
//

#ifndef STRATEGR_NOTIFIER_H
#define STRATEGR_NOTIFIER_H

#include <optional>
#include <ostream>
#include <string>
#include <unordered_set>

#include "notifications.h"
#include "stgstring.h"
#include "strategy.h"
#include "time_utils.h"
#include "timer.h"

namespace stg {
    class strategy;

#pragma mark - Notification

    enum class notification_type {
        prepare_start,
        start,
        prepare_end,
        end,
        prepare_strategy_end,
        strategy_end
    };

    auto session_notification(const session &session,
                              notification_type type) -> user_notifications::notification;

#pragma mark - Notifier

    // This class supports two methods for generating notifications:
    // polling (more suitable for desktop) and scheduling (more suitable for mobile).

    class notifier {
    public:
#pragma mark - Notifier Type Aliases

        using seconds = time_utils::seconds;
        using minutes = time_utils::minutes;
        using notifications_list = std::vector<user_notifications::notification>;

        using dictionary = user_notifications::storage;

#pragma mark - Getting Delivery Intervals from Current Time

        static constexpr seconds prepare_seconds_interval = 5 * 60;
        static constexpr seconds immediate_seconds_interval = 20;

        static auto immediate_delivery_seconds(minutes minutes_time) -> seconds;
        static auto prepare_delivery_seconds(minutes minutes_time) -> seconds;

#pragma mark - Construction

        explicit notifier(const strategy &strategy,
                          std::optional<file_bookmark> file = std::nullopt);
        ~notifier();

#pragma mark - Polling For Notifications

        void start_polling(timer::seconds interval);
        void stop_polling();

#pragma mark - Manually Scheduling Notifications

        void schedule();
        auto scheduled_identifiers() const -> std::vector<std::string>;

#pragma mark - Manually Persisting Scheduled Notifications Identifiers

        void persist_scheduled_identifiers();

#pragma mark - Getting & Updating Represented File Path

        auto file_path() const -> const std::optional<file_bookmark> &;
        void set_file(const std::optional<file_bookmark> &file_path);

        static void note_file_removed(const file_bookmark &file_path);
        static void note_file_moved(const file_bookmark &from, const file_bookmark &to);

#pragma mark - Getting Active Files

        static auto active_files() -> std::unordered_set<file_bookmark>;
        static void reset_active_files();

    private:
        static void remove_stale_from(notifications_list &notifications);

        const strategy &strategy;
        std::optional<file_bookmark> _file;

        std::shared_ptr<timer> polling_timer;
        std::shared_ptr<timer> on_change_timer;

        seconds last_poll_time = 0;

        notifications_list _scheduled_notifications;
        notifications_list _upcoming_notifications;

#pragma mark - Responding To Strategy Changes

        void on_sessions_change();

#pragma mark - Sending Immeadiate Notification

        void send_now_if_needed(seconds polling_seconds_interval);

#pragma mark - Reading & Writing to Persistent Dictionary

        auto persisted_notifications_identifiers() const -> std::vector<std::string>;
    };
}

#endif//STRATEGR_NOTIFIER_H
