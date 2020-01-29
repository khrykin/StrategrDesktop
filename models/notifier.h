//
// Created by Dmitry Khrykin on 2019-11-26.
//

#ifndef STRATEGR_NOTIFIER_H
#define STRATEGR_NOTIFIER_H

#include <thread>
#include <stdexcept>

#include "strategy.h"
#include "timer.h"

namespace stg {
    class strategy;

    class notifier {
    public:
        using seconds = unsigned int;
        using minutes = unsigned int;

        struct notification {
            enum class type {
                prepare_start,
                start,
                prepare_end,
                end,
                prepare_strategy_end,
                strategy_end
            };

            notification(const session &session, type type);

            const std::string identifier = make_string_uuid();

            const std::string title;
            const std::string sub_title;

            const seconds delivery_time;

        private:
            static std::string make_title(const session &session, type type);
            static std::string make_sub_title(const session &session, type type);
            static seconds make_delivery_time(const session &session, type type);
        };

        using scheduled_notifications_t = std::vector<notification>;
        using scheduler_t = std::function<void(const scheduled_notifications_t &)>;
        using resetter_t = std::function<void()>;
        using sender_t = std::function<void(const notification &)>;

        explicit notifier(const strategy *strategy);
        ~notifier();

        void set_strategy(const strategy *strategy);

        void start_watching();
        void stop_watching();

        void on_send_notification(const sender_t &callback);
        void on_schedule_notifications(const scheduler_t &callback);
        void on_reset_notifications(const resetter_t &callback);

        const scheduled_notifications_t &scheduled_notifications() const;
        std::vector<std::string> scheduled_identifiers() const;

        static const seconds prepare_seconds_interval = 5 * 60;
        static const seconds immediate_seconds_interval = 20;

    private:
        const strategy *strategy;

        bool is_watching = false;

        scheduler_t scheduler;
        resetter_t resetter;
        sender_t sender;

        scheduled_notifications_t _scheduled_notifications;

        void schedule_notifications();
        void reset_notifications();

        static seconds immediate_delivery_seconds(minutes minutes_time);
        static seconds prepare_delivery_seconds(minutes minutes_time);

        static std::string make_string_uuid();
    };
}

#endif //STRATEGR_NOTIFIER_H
