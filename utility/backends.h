//
// Created by Dmitry Khrykin on 12.06.2020.
//

#ifndef STRATEGR_BACKENDS_H
#define STRATEGR_BACKENDS_H

#include <QTimer>

#include "timer.h"
#include "persistent.h"
#include "application.h"

inline void setupBackends() {
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

    persistent_storage::backend::set_setter([](const std::string &key, const void *data, size_t size) {
        Application::currentSettings().setValue(key.c_str(), QByteArray((const char *) data, size));
    });

    persistent_storage::backend::set_getter([](const std::string &key, const auto &result_callback) {
        auto value = Application::currentSettings().value(key.c_str());

        if (value.isNull())
            return result_callback(nullptr);

        // Special handling for QString for backwards compatibility.
        if (value.type() == QVariant::String) {
            auto stringValue = value.toString().toStdString();
            auto byteArray = stg::serialize(stringValue);

            return result_callback(byteArray.data());
        }

        result_callback(value.toByteArray().data());
    });
}

#endif //STRATEGR_BACKENDS_H
