//
// Created by Dmitry Khrykin on 12.06.2020.
//

#ifndef STRATEGR_BACKENDS_H
#define STRATEGR_BACKENDS_H

#include <QTimer>
#include <utility>

#include "timer.h"
#include "persistent.h"
#include "application.h"
#include "notifications.h"

#include "application.h"

inline void setupBackends() {
    using namespace stg;

    timer::backend::set_scheduler([](timer::seconds duration,
                                     const timer::backend::scheduler_callback_t &callback) -> void * {
        auto *qTimer = new QTimer();
        void *implementation = static_cast<void *>(qTimer);

        QObject::connect(qTimer, &QTimer::timeout, [=] {
            callback(implementation);
        });

        qTimer->start(1000 * duration);

        return implementation;
    });

    timer::backend::set_invalidator([](void *implementation) {
        auto *qTimer = static_cast<QTimer *>(implementation);

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

    user_notifications::backend::set_immediate_sender([](const user_notifications::notification &notification) {
        std::cout << "want to sent notification: " << notification << "\n";
        Application::notifierBackend().sendMessage(notification.title.c_str(),
                                                   notification.message.c_str());
    });
}

#endif //STRATEGR_BACKENDS_H
