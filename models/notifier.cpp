//
// Created by Dmitry Khrykin on 2019-11-26.
//

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>
#include <algorithm>

#include "notifier.h"
#include "strategy.h"
#include "time_utils.h"

namespace stg {
    notifier::notifier(const stg::strategy &strategy) : strategy(strategy) {
        strategy.add_on_change_callback(this, &stg::notifier::schedule);
    }

    notifier::~notifier() {
        if (timer)
            stop_polling();
    }

    void notifier::schedule() {
        std::vector<notification> notifications;

        for (auto it = strategy.sessions().begin(); it != strategy.sessions().end(); ++it) {
            const auto &session = *it;
            auto next_it = std::next(it);

            if (session.activity) {
                notifications.emplace_back(session, notification::type::prepare_start);
                notifications.emplace_back(session, notification::type::start);

                if (next_it != strategy.sessions().end() && !next_it->activity) {
                    notifications.emplace_back(session, notification::type::prepare_end);
                    notifications.emplace_back(session, notification::type::end);
                }
            }

            if (next_it == strategy.sessions().end()) {
                notifications.emplace_back(session, notification::type::prepare_strategy_end);
                notifications.emplace_back(session, notification::type::strategy_end);
            }
        }

        if (on_delete_notifications)
            on_delete_notifications(scheduled_identifiers());

        _scheduled_notifications = notifications;

        if (on_schedule_notifications)
            on_schedule_notifications(_scheduled_notifications);

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

    auto notification::make_string_uuid() -> std::string {
        auto uuid = boost::uuids::random_generator()();

        std::stringstream sstream;
        sstream << uuid;

        return sstream.str();
    }

    auto notifier::scheduled_notifications() const -> const std::vector<notification> & {
        return _scheduled_notifications;
    }

    auto notifier::scheduled_identifiers() const -> std::vector<std::string> {
        std::vector<std::string> result;
        std::transform(_scheduled_notifications.begin(),
                       _scheduled_notifications.end(),
                       std::back_inserter(result),
                       [](const auto &notification) {
                           return notification.identifier;
                       });

        return result;
    }

    auto notifier::immediate_delivery_seconds(minutes minutes_time) -> seconds {
        return minutes_time * 60 - immediate_seconds_interval;
    }

    auto notifier::prepare_delivery_seconds(minutes minutes_time) -> seconds {
        return minutes_time * 60 - prepare_seconds_interval;
    }

    void notifier::send_now_if_needed(seconds polling_seconds_interval) {
        std::cout << "notifier::send_now_if_needed, polling_seconds_interval: " << polling_seconds_interval << ".\n";

        if (_scheduled_notifications.empty()) {
            schedule();
        }

        auto current_time = time_utils::current_seconds();

        if (last_poll_time && current_time - last_poll_time > 4 * polling_seconds_interval) {
            std::cout << "system time has changed, notifications rescheduled.\n";

            // If time difference between two calls of this function was too big,
            // this probably means that the system time had changed, we need to reschedule.
            schedule();
        }

        last_poll_time = current_time;

        if (_upcoming_notifications.empty() ||
            current_time < _upcoming_notifications.front().delivery_time)
            return;

//    std::cout << "current_time: " << current_time << "\n";
//    std::cout << "delivery_time: " << next_notification.delivery_time << "\n";

        // We have to send only last notification for which delivery time is less than the current.
        // The first is guaranteed to be so, we need to check the others:

        auto next_notification_it = _upcoming_notifications.begin();
        for (auto it = std::next(_upcoming_notifications.begin());
             it != _upcoming_notifications.end();
             ++it) {
            auto &notification = *it;

            if (current_time < notification.delivery_time) {
                next_notification_it = std::prev(it);
                break;
            }
        }

        auto &next_notification = *next_notification_it;

        if (on_send_notification)
            on_send_notification(next_notification);

//    std::cout << "notification sent: " << next_notification << "\n";

        // Remove sent and stale notifications
        _upcoming_notifications.erase(_upcoming_notifications.begin(),
                                      std::next(next_notification_it));
    }

    void notifier::start_polling(timer::seconds interval) {
        timer = stg::timer::schedule(interval, true, [=] {
            send_now_if_needed(interval);
        });
    }

    void notifier::stop_polling() {
        timer = nullptr;
    }

    auto notification::make_title(const session &session, type type) -> std::string {
        if (type == type::prepare_strategy_end ||
            type == type::strategy_end) {
            return "End Of A Strategy";
        }

        if (!session.activity) {
            throw std::invalid_argument("session must have an activity for this type of notification");
        }

        return session.activity->name()
               + " ("
               + time_utils::human_string_from_minutes(session.duration())
               + ")";
    }

    auto notification::make_delivery_time(const session &session, type type) -> seconds {
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

    auto notification::make_sub_title(const session &session, type type) -> std::string {
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

    notification::notification(const session &session, type type) :
            title(make_title(session, type)),
            message(make_sub_title(session, type)),
            delivery_time(make_delivery_time(session, type)) {}

    auto operator<<(std::ostream &os, const notification &notification) -> std::ostream & {
        os << "notification: [ ";
        os << "id: \"" << notification.identifier << "\", ";
        os << "title: \"" << notification.title << "\", ";
        os << "message: \"" << notification.message << "\", ";
        os << "delivery_time: \"" << time_utils::string_from_seconds(notification.delivery_time) << "\"";
        os << "]";

        return os;
    }

    auto operator==(const notification &lhs, const notification &rhs) -> bool {
        // Two notifications are considered equal if all properties other than id are equal,
        return lhs.title == rhs.title &&
               lhs.message == rhs.message &&
               lhs.delivery_time == rhs.delivery_time;
    }
}
