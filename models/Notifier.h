//
// Created by Dmitry Khrykin on 2019-11-26.
//

#ifndef STRATEGR_NOTIFIER_H
#define STRATEGR_NOTIFIER_H

#include <thread>
#include <stdexcept>

#include "Strategy.h"
#include "Timer.h"

class Strategy;

class Notifier {
public:
    using Seconds = unsigned int;
    using Minutes = unsigned int;

    struct Notification {
        enum class Type {
            PrepareStart,
            Start,
            PrepareEnd,
            End,
            PrepareStrategyEnd,
            StrategyEnd
        };

        Notification(const Session &session, Type type);

        const std::string identifier = makeStringUUID();

        const std::string title;
        const std::string subTitle;

        const Seconds deliveryTime;

    private:
        static std::string makeTitle(const Session &session, Type type);
        static std::string makeSubTitle(const Session &session, Type type);
        static Seconds makeDeliveryTime(const Session &session, Type type);
    };

    using ScheduledNotifications = std::vector<Notification>;
    using Scheduler = std::function<void(const ScheduledNotifications &)>;
    using Resetter = std::function<void()>;
    using Sender = std::function<void(const Notification &)>;

    explicit Notifier(const Strategy *strategy);
    ~Notifier();

    void setStrategy(const Strategy *strategy);

    void startWatching();
    void stopWatching();

    void onSendNotification(const Sender &callback);
    void onScheduleNotifications(const Scheduler &callback);
    void onResetNotifications(const Resetter &callback);

    const ScheduledNotifications &scheduledNotifications() const;
    std::vector<std::string> scheduledIdentifiers() const;

    static const Seconds prepareSecondsInterval = 5 * 60;
    static const Seconds immediateSecondsInterval = 20;

private:
    const Strategy *strategy;

    bool isWatching = false;

    Scheduler scheduler;
    Resetter resetter;
    Sender sender;

    ScheduledNotifications _scheduledNotifications;

    void scheduleNotifications();
    void resetNotifications();

    static Seconds immediateDeliverySeconds(Minutes minutesTime);
    static Seconds prepareDeliverySeconds(Minutes minutesTime);

    static std::string makeStringUUID();
};


#endif //STRATEGR_NOTIFIER_H
