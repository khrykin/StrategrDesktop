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

stg::notifier::notifier(const stg::strategy &strategy) : strategy(strategy) {
    strategy.add_on_change_callback(this,
                                    &stg::notifier::schedule);

    schedule();
}

void stg::notifier::start_watching() {
    is_watching = true;

    // schedule();
}

void stg::notifier::stop_watching() {
    is_watching = false;

    // teardown();
}

void stg::notifier::schedule() {
//    std::cout << "scheduled notifications: \n";

    std::vector<notification> notifications;
    for (auto it = strategy.sessions().begin(); it != strategy.sessions().end(); ++it) {
        const auto &session = *it;
        auto next_it = std::next(it);

        if (session.activity) {
            notifications.emplace_back(session,
                                       notification::type::prepare_start);
            notifications.emplace_back(session,
                                       notification::type::start);

            if (next_it != strategy.sessions().end() && !next_it->activity) {
                notifications.emplace_back(session,
                                           notification::type::prepare_end);
                notifications.emplace_back(session,
                                           notification::type::end);
            }
        }

        if (next_it == strategy.sessions().end()) {
            notifications.emplace_back(session,
                                       notification::type::prepare_strategy_end);
            notifications.emplace_back(session,
                                       notification::type::strategy_end);
        }
    }

    remove_stale(notifications);

//    for (auto &n : notifications) {
//        std::cout << n << "\n";
//    }

    if (on_delete_notifications)
        on_delete_notifications(scheduled_identifiers());

    _scheduled_notifications = notifications;

    if (on_schedule_notifications)
        on_schedule_notifications(_scheduled_notifications);
}

void stg::notifier::remove_stale(std::vector<notification> &notifications) {
    auto current_seconds = stg::time_utils::current_seconds();
    notifications.erase(std::remove_if(notifications.begin(),
                                       notifications.end(),
                                       [current_seconds](const stg::notification &notification) {
                                           return notification.delivery_time < current_seconds;
                                       }), notifications.end());
}

std::string stg::notification::make_string_uuid() {
    auto uuid = boost::uuids::random_generator()();

    std::stringstream sstream;
    sstream << uuid;

    return sstream.str();
}

const std::vector<stg::notification>
&stg::notifier::scheduled_notifications() const {
    return _scheduled_notifications;
}

std::vector<std::string> stg::notifier::scheduled_identifiers() const {
    std::vector<std::string> result;
    std::transform(_scheduled_notifications.begin(),
                   _scheduled_notifications.end(),
                   std::back_inserter(result),
                   [](const auto &notification) {
                       return notification.identifier;
                   });

    return result;
}

stg::notifier::seconds stg::notifier::immediate_delivery_seconds(stg::notifier::minutes minutes_time) {
    return minutes_time * 60 - immediate_seconds_interval;
}

stg::notifier::seconds stg::notifier::prepare_delivery_seconds(stg::notifier::minutes minutes_time) {
    return minutes_time * 60 - prepare_seconds_interval;
}

void stg::notifier::send_now_if_needed(seconds polling_seconds_interval) {
//    std::cout << "send notification, if needed => \n";
    auto current_time = time_utils::current_seconds();

    if (last_poll_time && current_time - last_poll_time > 4 * polling_seconds_interval) {
//        std::cout << "system time changed\n";

        // If time difference between two calls of this function was too big,
        // this probably means that the system time had changed, we need to reschedule.
        schedule();
    }

    last_poll_time = current_time;

    if (_scheduled_notifications.empty() ||
        current_time < _scheduled_notifications.front().delivery_time)
        return;

//    std::cout << "current_time: " << current_time << "\n";
//    std::cout << "delivery_time: " << next_notification.delivery_time << "\n";

    // We have to send only last notification for which delivery time is less than the current.
    // The first is guaranteed to be so, we need to check the others:

    auto next_notification_it = _scheduled_notifications.begin();
    for (auto it = std::next(_scheduled_notifications.begin());
         it != _scheduled_notifications.end();
         ++it) {
        auto &notification = *it;

        if (current_time < notification.delivery_time) {
            next_notification_it = std::prev(it);
            break;
        }
    }

    auto &next_notification = *next_notification_it;

    if (on_send_notiifcation)
        on_send_notiifcation(next_notification);

//    std::cout << "notification sent: " << next_notification << "\n";

    // Remove sent and stale notifications
    _scheduled_notifications.erase(_scheduled_notifications.begin(),
                                   std::next(next_notification_it));
}

std::string stg::notification::make_title(const session &session, type type) {
    if (type == type::prepare_strategy_end ||
        type == type::strategy_end) {
        return "End Of A Strategy";
    }

    if (!session.activity) {
        throw std::invalid_argument("session must have an activity for this type of notification");
    }

    return session.activity->name()
           + " ("
           + stg::time_utils::human_string_from_minutes(session.duration())
           + ")";
}

stg::notification::seconds stg::notification::make_delivery_time(const session &session, type type) {
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

std::string stg::notification::make_sub_title(const session &session, type type) {
    switch (type) {
        case type::prepare_start:
            return "Coming up in "
                   + time_utils::human_string_from_minutes(notifier::prepare_seconds_interval / 60);
        case type::start:
            return "Starts right now";
        case type::prepare_end:
            return "Ends in "
                   + time_utils::human_string_from_minutes(notifier::prepare_seconds_interval / 60);
        case type::end:
            return "Ends right now";
        case type::prepare_strategy_end:
            return "Strategy ends in "
                   + time_utils::human_string_from_minutes(notifier::prepare_seconds_interval / 60);
        case type::strategy_end:
            return "Strategy ends right now";
        default:
            return "";
    }
}

stg::notification::notification(const session &session, type type) :
        title(make_title(session, type)),
        message(make_sub_title(session, type)),
        delivery_time(make_delivery_time(session, type)) {}

std::ostream &stg::operator<<(std::ostream &os, const stg::notification &notification) {
    os << "notification: [ ";
    os << "id: \"" << notification.identifier << "\", ";
    os << "title: \"" << notification.title << "\", ";
    os << "message: \"" << notification.message << "\", ";
    os << "delivery_time: \"" << time_utils::string_from_seconds(notification.delivery_time) << "\"";
    os << "]";

    return os;
}

bool stg::operator==(const stg::notification &lhs, const stg::notification &rhs) {
    // Two notifications are considered equal if all properties other than id are equal,
    return lhs.title == rhs.title &&
           lhs.message == rhs.message &&
           lhs.delivery_time == rhs.delivery_time;
}
