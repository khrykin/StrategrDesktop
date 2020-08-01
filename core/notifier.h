//
// Created by Dmitry Khrykin on 2019-11-26.
//

#ifndef STRATEGR_NOTIFIER_H
#define STRATEGR_NOTIFIER_H

#include <thread>
#include <stdexcept>
#include <ostream>
#include <string>
#include <unordered_map>

#include "strategy.h"
#include "timer.h"
#include "stgstring.h"

namespace stg {
    class strategy;

#pragma mark - Notification

    struct notification {
        using seconds = unsigned int;
        using minutes = unsigned int;

        enum class type {
            prepare_start,
            start,
            prepare_end,
            end,
            prepare_strategy_end,
            strategy_end
        };

        notification(const session &session, type type);

        type type;
        std::string identifier = make_string_uuid();
        std::string title;
        std::string message;
        seconds delivery_time;

    private:
        auto make_title(const session &session) const -> std::string;
        auto make_sub_title(const session &session) const -> std::string;
        auto make_delivery_time(const session &session) const -> seconds;

        friend bool operator==(const notification &lhs, const notification &rhs);

        friend std::ostream &operator<<(std::ostream &os, const notification &notification);
    };

    // This class supports two methods for generating notifications:
    // polling (more suitable for desktop) and scheduling (more suitable for mobile).

    class notifier {
    public:

#pragma mark - Notifier Type Aliases

        using seconds = notification::seconds;
        using minutes = notification::minutes;

        using scheduler_t = std::function<void(const std::vector<notification> &)>;
        using resetter_t = std::function<void(const std::vector<std::string> &)>;
        using sender_t = std::function<void(const notification &)>;

        using dictionary = std::unordered_map<std::string, std::vector<std::string>>;

#pragma mark - Getting Delivery Intervals from Current Time

        static constexpr seconds prepare_seconds_interval = 5 * 60;
        static constexpr seconds immediate_seconds_interval = 20;

        static auto immediate_delivery_seconds(minutes minutes_time) -> seconds;
        static auto prepare_delivery_seconds(minutes minutes_time) -> seconds;

#pragma mark - Callbacks

        // Asks delegate to send notification immediately.
        // Called when using polling.
        sender_t on_send_notification = nullptr;

#pragma mark - Construction

        explicit notifier(const strategy &strategy,
                          std::string filename = "",
                          scheduler_t on_schedule_notifications = nullptr,
                          resetter_t on_delete_notifications = nullptr);
        ~notifier();

#pragma mark - Scheduling Notifications

        void schedule();
        auto scheduled_identifiers() const -> std::vector<std::string>;

#pragma mark - Polling For Notifications

        void start_polling(timer::seconds interval);
        void stop_polling();

#pragma mark - Getting & Updating Represented Filename

        auto filename() const -> const std::string &;
        void set_filename(const std::string &filename);

    private:
        static void remove_stale_from(std::vector<notification> &notifications);

        const strategy &strategy;
        std::string _filename;
        std::unique_ptr<stg::timer> timer = nullptr;
        seconds last_poll_time = 0;

        std::vector<stg::notification> _scheduled_notifications;
        std::vector<stg::notification> _upcoming_notifications;

#pragma mark - Private callbacks

        // Asks delegate to schedule notifications.
        // Called when using scheduling method.
        scheduler_t on_schedule_notifications = nullptr;
        // Asks delegate to delete previously scheduled notifications with identifiers.
        // Called when using scheduling method.
        resetter_t on_delete_notifications = nullptr;

#pragma mark - Sending Immeadiate Notification

        void send_now_if_needed(seconds polling_seconds_interval);

#pragma mark - Reading & Writing to Persistent Dictionary

        static auto get_notifications_dictionary() -> dictionary;
        auto persisted_notifications_identifiers() const -> std::vector<std::string>;
        void persist_scheduled_identifiers();
    };
}

#endif //STRATEGR_NOTIFIER_H
