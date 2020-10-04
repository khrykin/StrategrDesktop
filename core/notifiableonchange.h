//
// Created by Dmitry Khrykin on 2019-07-04.
//

#ifndef MODELS_NOTIFIABLEONCHANGE_H
#define MODELS_NOTIFIABLEONCHANGE_H

#include <functional>
#include <optional>

namespace stg {
    class notifiable_on_change {
    public:
        using callback_t = std::function<void()>;

        notifiable_on_change() = default;
        notifiable_on_change(const notifiable_on_change &) = delete;

        void add_on_change_callback(const callback_t &callback) const {
            on_change_callbacks.push_back(callback);
        }

        template<class Listener, typename Method = std::function<void(Listener *)>>
        void add_on_change_callback(Listener *listener, const Method &method) const {
            add_on_change_callback(std::bind(method, listener));
        }

    protected:
        mutable std::vector<callback_t> on_change_callbacks = {};

        virtual void on_change_event() const {
            for (const auto &callback : on_change_callbacks) {
                callback();
            }
        }
    };
};

#endif//MODELS_NOTIFIABLEONCHANGE_H