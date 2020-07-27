//
// Created by Dmitry Khrykin on 12.06.2020.
//

#ifndef STRATEGR_TIMER_H
#define STRATEGR_TIMER_H

#include <functional>
#include <memory>

namespace stg {
    class timer {
    public:
        using seconds = double;
        using callback_t = std::function<void()>;

        class backend {
        public:
            using scheduler_callback_t = std::function<void(void *implementation)>;
            using scheduler_t = std::function<void *(seconds duration, bool repeats, scheduler_callback_t)>;
            using invalidator_t = std::function<void(void *timer_impl_ptr)>;

            static void set_scheduler(scheduler_t fn);
            static void set_invalidator(invalidator_t fn);

        private:
            friend timer;

            static scheduler_t scheduler;
            static invalidator_t invalidator;
        };

        static auto schedule(seconds duration, bool repeats, const callback_t &callback) -> std::unique_ptr<timer>;

        ~timer();

        void invalidate();

    private:
        timer() = default;

        void *implementation = nullptr;
        bool invalidated = false;
        bool repeats = false;
    };
}


#endif //STRATEGR_TIMER_H
