//
// Created by Dmitry Khrykin on 15.06.2020.
//

#ifndef STRATEGR_ACTION_HPP
#define STRATEGR_ACTION_HPP

#include <cassert>
#include <functional>
#include <string>
#include <utility>

namespace stg {
    class action_center;

    class action {
    public:
        enum action_type {
            standard,
            cancel,
            destructive,
            separator
        };

        static auto separator_action() -> const action & {
            static const auto separator_action = action(
                "", [] {}, action_type::separator);
            return separator_action;
        }

        const action_type type;
        const std::string name;

        action(std::string name,
               std::function<void()> performer,
               std::function<bool()> enabled,
               action_type type = action_type::standard)
            : name(std::move(name)),
              performer(std::move(performer)),
              enabled_getter(std::move(enabled)),
              type(type) {
            assert(this->performer && "action performer must not be null");
        }

        action(const std::string &name,
               std::function<void()> performer,
               action_type type = action_type::standard)
            : action(name, std::move(performer), nullptr, type) {}

        auto is_enabled() const -> bool {
            return enabled_getter ? enabled_getter() : true;
        }

        void operator()() const {
            if (is_enabled())
                return performer();
        }

    private:
        friend action_center;

        const std::function<void()> performer;
        const std::function<bool()> enabled_getter = nullptr;
    };
}

#endif//STRATEGR_ACTION_HPP
