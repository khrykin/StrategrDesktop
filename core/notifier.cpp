//
// Created by Dmitry Khrykin on 2019-11-26.
//

#include <algorithm>
#include <cassert>
#include <utility>
#include <cstdlib>

#include "notifier.h"
#include "strategy.h"
#include "time_utils.h"
#include "persistent.h"

namespace stg {

    constexpr auto *notifications_dictionary_key = "activeURLs";

#pragma mark - Notification

    notification::notification(const session &session, enum type type) :
            type(type),
            title(make_title(session)),
            message(make_sub_title(session)),
            delivery_time(make_delivery_time(session)) {}

    auto notification::make_title(const session &session) const -> std::string {
        if (type == type::prepare_strategy_end ||
            type == type::strategy_end) {
            return "End Of A Strategy";
        }

        if (!session.activity)
            throw std::invalid_argument("session must have an activity for this type of notification");

        return session.activity->name()
               + " ("
               + time_utils::human_string_from_minutes(session.duration())
               + ")";
    }

    auto notification::make_sub_title(const session &session) const -> std::string {
        const auto minutes_interval = notifier::prepare_seconds_interval / 60;
        const auto time_string = time_utils::human_string_from_minutes(minutes_interval);

        switch (type) {
            case type::prepare_start:
                return "Coming up in " + time_string;
            case type::start:
                return "Starts right now";
            case type::prepare_end:
                return "Ends in " + time_string;
            case type::end:
                return "Ends right now";
            case type::prepare_strategy_end:
                return "Strategy ends in " + time_string;
            case type::strategy_end:
                return "Strategy ends right now";
            default:
                return "";
        }
    }

    auto notification::make_delivery_time(const session &session) const -> seconds {
        switch (type) {
            case type::prepare_start:
                return notifier::prepare_delivery_seconds(session.begin_time());
            case type::start:
                return notifier::immediate_delivery_seconds(session.begin_time());
            case type::prepare_end:
            case type::prepare_strategy_end:
                return notifier::prepare_delivery_seconds(session.end_time());
            case type::end:
            case type::strategy_end:
                return notifier::immediate_delivery_seconds(session.end_time());
            default:
                return 0;
        }
    }

    auto operator==(const notification &lhs, const notification &rhs) -> bool {
        // Two notifications are considered equal if all properties other than id are equal,
        return lhs.title == rhs.title &&
               lhs.message == rhs.message &&
               lhs.delivery_time == rhs.delivery_time;
    }

    auto operator<<(std::ostream &os, const notification &notification) -> std::ostream & {
        os << "notification: [ ";
        os << "id: \"" << notification.identifier << "\", ";
        os << "title: \"" << notification.title << "\", ";
        os << "message: \"" << notification.message << "\", ";
        os << "delivery_time: \"" << time_utils::string_from_seconds(notification.delivery_time) << "\"";
        os << "]";

        return os;
    }

#pragma mark - Getting Delivery Intervals from Current Time

    auto notifier::immediate_delivery_seconds(minutes minutes_time) -> seconds {
        return minutes_time * 60 - immediate_seconds_interval;
    }

    auto notifier::prepare_delivery_seconds(minutes minutes_time) -> seconds {
        return minutes_time * 60 - prepare_seconds_interval;
    }

#pragma mark - Construction

    notifier::notifier(const stg::strategy &strategy,
                       std::string filename,
                       scheduler_t on_schedule_notifications,
                       resetter_t on_delete_notifications)
            : strategy(strategy),
              _filename(std::move(filename)),
              on_schedule_notifications(std::move(on_schedule_notifications)),
              on_delete_notifications(std::move(on_delete_notifications)) {
        schedule();
        strategy.sessions().add_on_change_callback(this, &notifier::schedule);
    }

    notifier::~notifier() {
        if (timer)
            stop_polling();

        if (!_filename.empty())
            persist_scheduled_identifiers();
    }

#pragma mark - Scheduling Notifications

    void notifier::schedule() {
        // Ask delegate to remove previous notifications
        if (on_delete_notifications)
            on_delete_notifications(scheduled_identifiers());

        std::vector<notification> notifications;

        for (const auto &session : strategy.sessions()) {
            auto session_index = &session - &strategy.sessions()[0];
            auto next_session_it = strategy.sessions().begin() + session_index + 1;

            if (session.activity) {
                notifications.emplace_back(session, notification::type::prepare_start);
                notifications.emplace_back(session, notification::type::start);

                if (next_session_it != strategy.sessions().end() && !next_session_it->activity) {
                    notifications.emplace_back(session, notification::type::prepare_end);
                    notifications.emplace_back(session, notification::type::end);
                }
            }

            if (next_session_it == strategy.sessions().end()) {
                notifications.emplace_back(session, notification::type::prepare_strategy_end);
                notifications.emplace_back(session, notification::type::strategy_end);
            }
        }

        _scheduled_notifications = notifications;

        if (on_schedule_notifications)
            on_schedule_notifications(notifications);

        remove_stale_from(notifications);

        _upcoming_notifications = notifications;
    }

    void notifier::remove_stale_from(std::vector<notification> &notifications) {
        auto current_seconds = stg::time_utils::current_seconds();
        notifications.erase(std::remove_if(notifications.begin(),
                                           notifications.end(),
                                           [current_seconds](const stg::notification &notification) {
                                               return notification.delivery_time < current_seconds;
                                           }), notifications.end());
    }

    auto notifier::scheduled_identifiers() const -> std::vector<std::string> {
        if (!_filename.empty() && _scheduled_notifications.empty())
            return persisted_notifications_identifiers();

        std::vector<std::string> result;
        std::transform(_scheduled_notifications.begin(),
                       _scheduled_notifications.end(),
                       std::back_inserter(result),
                       [](const auto &notification) {
                           return notification.identifier;
                       });

        return result;
    }

#pragma mark - Polling For Notifications

    void notifier::start_polling(timer::seconds interval) {
        assert("Already polling" && timer == nullptr);

        timer = stg::timer::schedule(interval, true, [=] {
            send_now_if_needed(interval);
        });
    }

    void notifier::send_now_if_needed(seconds polling_seconds_interval) {
        auto current_time = time_utils::current_seconds();

        if (last_poll_time && std::abs((int) current_time - (int) last_poll_time) > 4 * polling_seconds_interval) {
            // If time difference between two calls of this function was too big,
            // this probably means that the system time had changed, we need to reschedule.
            schedule();
        }

        last_poll_time = current_time;

        if (_upcoming_notifications.empty() ||
            current_time < _upcoming_notifications.front().delivery_time)
            return;

        // We have to send only last notification for which current time >= delivery time.
        // The first is guaranteed to be so, we need to check the others

        // This points to the first notification after the one we need:
        auto next_notification_it = std::find_if(_upcoming_notifications.begin() + 1,
                                                 _upcoming_notifications.end(),
                                                 [current_time](auto &notification) {
                                                     return current_time < notification.delivery_time;
                                                 });

        auto current_notification_it = std::prev(next_notification_it);
        auto &notification = *current_notification_it;

        if (on_send_notification)
            on_send_notification(notification);

        // Remove just sent and stale notifications
        _upcoming_notifications.erase(_upcoming_notifications.begin(), next_notification_it);
    }

    void notifier::stop_polling() {
        timer = nullptr;
    }

#pragma mark - Getting & Updating Represented Filename

    auto notifier::filename() const -> const std::string & {
        return _filename;
    }

    void notifier::set_filename(const std::string &filename) {
        auto old_filename = _filename;
        _filename = filename;

        auto dict = get_notifications_dictionary();

        auto old_filename_it = dict.find(old_filename);
        if (old_filename_it != dict.end()) {
            auto scheduled_notification_ids = old_filename_it->second;
            if (filename.empty() && on_delete_notifications) {
                on_delete_notifications(scheduled_notification_ids);
            } else {
                dict[filename] = scheduled_notification_ids;
            }

            dict.erase(old_filename_it);

            persistent_storage::set(notifications_dictionary_key, dict);
        }
    }

#pragma mark - Reading & Writing to Persistent Dictionary

    auto notifier::get_notifications_dictionary() -> dictionary {
        auto dictionary_ptr = persistent_storage::get<dictionary>(notifications_dictionary_key);

        return dictionary_ptr ? *dictionary_ptr : dictionary();
    }

    auto notifier::persisted_notifications_identifiers() const -> std::vector<std::string> {
        assert("Attempted to read notification identifiers for empty filename" && !_filename.empty());

        auto dict = get_notifications_dictionary();

        if (dict.find(_filename) != dict.end())
            return dict[_filename];

        return {};
    }

    void notifier::persist_scheduled_identifiers() {
        assert("Attempted to save notification identifiers for empty filename" && !_filename.empty());

        if (!_scheduled_notifications.empty()) {
            auto dict = get_notifications_dictionary();
            dict[_filename] = scheduled_identifiers();

            persistent_storage::set(notifications_dictionary_key, dict);
        }
    }

}
