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

    void setOnChangeCallback(const std::optional<OnChangeCallback>
                             &callback) const {
        onChangeCallback = callback;
    }

    template<class Listener,
            typename Method = std::function<void(Listener *)>>
    void setOnChangeCallback(Listener *listener,
                             Method method) const {
        onChangeCallback = std::bind(method, listener);
    }

protected:
    mutable std::optional<OnChangeCallback> onChangeCallback;

    void onChangeEvent() {
        if (onChangeCallback) {
            (*onChangeCallback)();
        }
    }
};

#endif //MODELS_NOTIFIABLEONCHANGE_H
