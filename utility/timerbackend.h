//
// Created by Dmitry Khrykin on 12.06.2020.
//

#ifndef STRATEGR_TIMERBACKEND_H
#define STRATEGR_TIMERBACKEND_H

#include "timer.h"

#include <QTimer>

inline void setupTimerBackend() {
    using namespace stg;

    timer::backend::set_scheduler([](timer::seconds duration, bool repeats, auto callback) {
        auto *qTimer = new QTimer();
        void *implementation = reinterpret_cast<void *>(qTimer);
        qTimer->setSingleShot(!repeats);
        QObject::connect(qTimer, &QTimer::timeout, [callback, implementation] {
            callback(implementation);
        });

        qTimer->start(1000 * duration);

        return implementation;
    });

    timer::backend::set_invalidator([](void *timerImpl) {
        auto *qTimer = reinterpret_cast<QTimer *>(timerImpl);
        qTimer->stop();
        qTimer->deleteLater();
    });
}

#endif //STRATEGR_TIMERBACKEND_H
