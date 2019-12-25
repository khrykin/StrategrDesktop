//
// Created by Dmitry Khrykin on 2019-11-26.
//

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>
#include <algorithm>

#include "Notifier.h"
#include "Strategy.h"
#include "TimeUtils.h"

void Notifier::setStrategy(const Strategy *newStrategy) {
    strategy = newStrategy;
    strategy->addOnChangeCallback(this,
                                  &Notifier::scheduleNotifications);
}

Notifier::Notifier(const Strategy *strategy) : strategy(strategy) {
    strategy->addOnChangeCallback(this,
                                  &Notifier::scheduleNotifications);
}

void Notifier::startWatching() {
    isWatching = true;

    // reset() -> schedule();
}

void Notifier::stopWatching() {
    isWatching = false;

    // reset();
}

void Notifier::onScheduleNotifications(const Scheduler &callback) {
    scheduler = callback;
}

void Notifier::onResetNotifications(const Resetter &callback) {
    resetter = callback;
}

Notifier::~Notifier() {
}

void Notifier::scheduleNotifications() {
    std::cout << "schedule notifications" << std::endl;

    ScheduledNotifications notifications;
    for (auto it = strategy->sessions().begin();
         it != strategy->sessions().end();
         ++it) {
        const auto &session = *it;
        auto nextIt = std::next(it);

        if (nextIt == strategy->sessions().end()) {
            notifications.push_back(Notification(session,
                                                 Notification::Type::PrepareStrategyEnd));
            notifications.push_back(Notification(session,
                                                 Notification::Type::StrategyEnd));
        }

        if (!session.activity) {
            return;
        }

        notifications.push_back(Notification(session,
                                             Notification::Type::PrepareStart));
        notifications.push_back(Notification(session,
                                             Notification::Type::Start));


        if (nextIt != strategy->sessions().end() && !nextIt->activity) {
            notifications.push_back(Notification(session,
                                                 Notification::Type::PrepareEnd));
            notifications.push_back(Notification(session,
                                                 Notification::Type::End));
        }
    }

}

void Notifier::resetNotifications() {
    if (resetter) {
        resetter();
    }
}

void Notifier::onSendNotification(const Notifier::Sender &callback) {
    sender = callback;
}

std::string Notifier::makeStringUUID() {
    auto uuid = boost::uuids::random_generator()();

    std::stringstream sstream;
    sstream << uuid;

    return sstream.str();
}

const Notifier::ScheduledNotifications
&Notifier::scheduledNotifications() const {
    return _scheduledNotifications;
}

std::vector<std::string> Notifier::scheduledIdentifiers() const {
    std::vector<std::string> result;
    std::transform(_scheduledNotifications.begin(),
                   _scheduledNotifications.end(),
                   std::back_inserter(result),
                   [](const auto &notification) {
                       return notification.identifier;
                   });

    return result;
}

Notifier::Seconds Notifier::immediateDeliverySeconds(Notifier::Minutes minutesTime) {
    return minutesTime * 60 - immediateSecondsInterval;
}

Notifier::Seconds Notifier::prepareDeliverySeconds(Notifier::Minutes minutesTime) {
    return minutesTime * 60 - prepareSecondsInterval;
}

std::string Notifier::Notification::makeTitle(const Session &session, Type type) {
    if (type == Type::PrepareStrategyEnd ||
        type == Type::StrategyEnd) {
        return "End Of A Strategy";
    }

    if (!session.activity) {
        throw std::invalid_argument("Session must have an activity for this type of notification");
    }

    return session.activity->name()
           + " ("
           + TimeUtils::humanTimeForMinutes(session.duration())
           + ")";
}

Notifier::Seconds Notifier::Notification::makeDeliveryTime(const Session &session, Type type) {
    switch (type) {
        case Type::PrepareStart:
            return prepareDeliverySeconds(session.beginTime());
        case Type::Start:
            return immediateDeliverySeconds(session.beginTime());
        case Type::PrepareEnd:
        case Type::PrepareStrategyEnd:
            return prepareDeliverySeconds(session.endTime());
        case Type::End:
        case Type::StrategyEnd:
            return immediateDeliverySeconds(session.endTime());
    }
}

std::string Notifier::Notification::makeSubTitle(const Session &session, Type type) {
    switch (type) {
        case Type::PrepareStart:
            return "Coming up in "
                   + TimeUtils::humanTimeForMinutes(prepareSecondsInterval / 60);
        case Type::Start:
            return "Starts right now";
        case Type::PrepareEnd:
            return "Ends in "
                   + TimeUtils::humanTimeForMinutes(prepareSecondsInterval / 60);
        case Type::End:
            return "Ends right now";
        case Type::PrepareStrategyEnd:
            return "Strategy ends in "
                   + TimeUtils::humanTimeForMinutes(prepareSecondsInterval / 60);
        case Type::StrategyEnd:
            return "Strategy ends right now";
    }
}

Notifier::Notification::Notification(const Session &session, Type type) :
        title(makeTitle(session, type)),
        subTitle(makeSubTitle(session, type)),
        deliveryTime(makeDeliveryTime(session, type)) {}
