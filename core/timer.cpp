//
// Created by Dmitry Khrykin on 12.06.2020.
//

#include "timer.h"

#include <iostream>
#include <cassert>

namespace stg {
    timer::backend::scheduler_t timer::backend::scheduler = nullptr;
    timer::backend::invalidator_t timer::backend::invalidator = nullptr;

    void timer::backend::set_scheduler(scheduler_t fn) {
        backend::scheduler = std::move(fn);
    }

    void timer::backend::set_invalidator(invalidator_t fn) {
        backend::invalidator = std::move(fn);
    }

    auto timer::schedule(timer::seconds duration, bool repeats, const callback_t &callback) -> std::unique_ptr<timer> {
        assert(backend::scheduler && "You must provide stg::timer::backend::scheduler");

        // Note: we don't use stg::make_unique here,
        // because we want to keep default constructor private.
        auto timer = std::unique_ptr<stg::timer>(new stg::timer());
        timer->repeats = repeats;
        timer->implementation = backend::scheduler(duration, repeats, [=](void *impl) {
            callback();

            if (!repeats)
                backend::invalidator(impl);
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

    timer::~timer() {
        // If timer is non-repeating it is automatically invalidated,
        // and we don't want to invalidate it before it fires.

        if (repeats)
            invalidate();
    }
}