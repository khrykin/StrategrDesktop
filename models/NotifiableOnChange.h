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

    NotifiableOnChange() = default;
    NotifiableOnChange(const NotifiableOnChange &) = delete;

    void addOnChangeCallback(const OnChangeCallback &callback) const {
        onChangeCallbacks.push_back(callback);
    }

    template<class Listener,
            typename Method = std::function<void(Listener *)>>
    void addOnChangeCallback(Listener *listener,
                             const Method &method) const {
        addOnChangeCallback(std::bind(method, listener));
    }

protected:
    mutable std::vector<OnChangeCallback> onChangeCallbacks = {};

    void onChangeEvent() {
        for (const auto &callback: onChangeCallbacks) {
            callback();
        }
    }
};

#endif //MODELS_NOTIFIABLEONCHANGE_H
