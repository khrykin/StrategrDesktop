//
// Created by Dmitry Khrykin on 2019-07-04.
//

#ifndef MODELS_NOTIFIABLEONCHANGE_H
#define MODELS_NOTIFIABLEONCHANGE_H

#include <optional>
#include <functional>

class NotifiableOnChange {
public:
    using OnChangeCallback = std::function<void()>;

    void addOnChangeCallback(const OnChangeCallback &callback) const {
        callbacks.push_back(callback);
    }

    template<class Listener,
            typename Method = std::function<void(Listener *)>>
    void addOnChangeCallback(Listener *listener,
                             const Method &method) const {
        addOnChangeCallback(std::bind(method, listener));
    }

protected:
    mutable std::vector<OnChangeCallback> callbacks = {};

    void onChangeEvent() {
        for (const auto &callback: callbacks) {
            callback();
        }
    }
};

#endif //MODELS_NOTIFIABLEONCHANGE_H
