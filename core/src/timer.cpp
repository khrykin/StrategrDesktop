//
// Created by Dmitry Khrykin on 12.06.2020.
//

#include "timer.h"

#include <cassert>
#include <iostream>

namespace stg {
    timer::backend::scheduler_t timer::backend::scheduler = nullptr;
    timer::backend::invalidator_t timer::backend::invalidator = nullptr;

    void timer::backend::set_scheduler(scheduler_t fn) {
        backend::scheduler = std::move(fn);
    }

    void timer::backend::set_invalidator(invalidator_t fn) {
        backend::invalidator = std::move(fn);
    }

    auto timer::schedule(timer::seconds duration, bool repeats, const callback_t &callback) -> std::shared_ptr<timer> {
        assert(backend::scheduler && "You must provide stg::timer::backend::scheduler");

        // Note: we don't use stg::make_unique here,
        // because we want to keep default constructor private.
        auto timer = std::shared_ptr<stg::timer>(new stg::timer());
        auto weak_timer_ptr = std::weak_ptr<stg::timer>(timer);

        timer->repeats = repeats;
        timer->implementation = backend::scheduler(duration, [repeats, callback, weak_timer_ptr](void *impl) {
            callback();

            auto timer = weak_timer_ptr.lock();

            if (!repeats) {
                backend::invalidator(impl);
                if (timer) timer->invalidated = true;
            }
        });

        return timer;
    }

    void timer::invalidate() {
        assert(backend::invalidator && "You must provide stg::timer::backend::invalidator");

        if (invalidated)
            return;

        backend::invalidator(implementation);
        invalidated = true;
    }

    auto timer::is_running() const -> bool {
        return !invalidated;
    }

    timer::~timer() {
        // If timer is non-repeating it is automatically invalidated by backend,
        // and we don't want to invalidate it before it fires.

        if (repeats)
            invalidate();
    }
}
