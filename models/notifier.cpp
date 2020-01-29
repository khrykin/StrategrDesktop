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

void stg::notifier::set_strategy(const stg::strategy *new_strategy) {
    strategy = new_strategy;
    strategy->add_on_change_callback(this,
                                     &stg::notifier::schedule_notifications);
}

stg::notifier::notifier(const stg::strategy *strategy) : strategy(strategy) {
    strategy->add_on_change_callback(this,
                                     &stg::notifier::schedule_notifications);
}

void stg::notifier::start_watching() {
    is_watching = true;

    // reset() -> schedule();
}

void stg::notifier::stop_watching() {
    is_watching = false;

    // reset();
}

void stg::notifier::on_schedule_notifications(const scheduler_t &callback) {
    scheduler = callback;
}

void stg::notifier::on_reset_notifications(const resetter_t &callback) {
    resetter = callback;
}

stg::notifier::~notifier() {
}

void stg::notifier::schedule_notifications() {
    std::cout << "schedule notifications" << std::endl;

    scheduled_notifications_t notifications;
    for (auto it = strategy->sessions().begin();
         it != strategy->sessions().end();
         ++it) {
        const auto &session = *it;
        auto next_it = std::next(it);

        if (next_it == strategy->sessions().end()) {
            notifications.push_back(notification(session,
                                                 notification::type::prepare_strategy_end));
            notifications.push_back(notification(session,
                                                 notification::type::strategy_end));
        }

        if (!session.activity) {
            return;
        }

        notifications.push_back(notification(session,
                                             notification::type::prepare_start));
        notifications.push_back(notification(session,
                                             notification::type::start));


        if (next_it != strategy->sessions().end() && !next_it->activity) {
            notifications.push_back(notification(session,
                                                 notification::type::prepare_end));
            notifications.push_back(notification(session,
                                                 notification::type::end));
        }
    }

}

void stg::notifier::reset_notifications() {
    if (resetter) {
        resetter();
    }
}

void stg::notifier::on_send_notification(const sender_t &callback) {
    sender = callback;
}

std::string stg::notifier::make_string_uuid() {
    auto uuid = boost::uuids::random_generator()();

    std::stringstream sstream;
    sstream << uuid;

    return sstream.str();
}

const stg::notifier::scheduled_notifications_t
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

std::string stg::notifier::notification::make_title(const session &session, type type) {
    if (type == type::prepare_strategy_end ||
        type == type::strategy_end) {
        return "end of a strategy";
    }

    if (!session.activity) {
        throw std::invalid_argument("session must have an activity for this type of notification");
    }

    return session.activity->name()
           + " ("
           + stg::time_utils::human_time_for_minutes(session.duration())
           + ")";
}

stg::notifier::seconds stg::notifier::notification::make_delivery_time(const session &session, type type) {
    switch (type) {
        case type::prepare_start:
            return prepare_delivery_seconds(session.begin_time());
        case type::start:
            return immediate_delivery_seconds(session.begin_time());
        case type::prepare_end:
        case type::prepare_strategy_end:
            return prepare_delivery_seconds(session.end_time());
        case type::end:
        case type::strategy_end:
            return immediate_delivery_seconds(session.end_time());
    }
}

std::string stg::notifier::notification::make_sub_title(const session &session, type type) {
    switch (type) {
        case type::prepare_start:
            return "coming up in "
                   + stg::time_utils::human_time_for_minutes(prepare_seconds_interval / 60);
        case type::start:
            return "starts right now";
        case type::prepare_end:
            return "ends in "
                   + stg::time_utils::human_time_for_minutes(prepare_seconds_interval / 60);
        case type::end:
            return "ends right now";
        case type::prepare_strategy_end:
            return "strategy ends in "
                   + stg::time_utils::human_time_for_minutes(prepare_seconds_interval / 60);
        case type::strategy_end:
            return "strategy ends right now";
    }
}

stg::notifier::notification::notification(const session &session, type type) :
        title(make_title(session, type)),
        sub_title(make_sub_title(session, type)),
        delivery_time(make_delivery_time(session, type)) {}
