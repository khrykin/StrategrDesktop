//
// Created by Dmitry Khrykin on 2020-01-29.
//

#ifndef STRATEGR_MOUSEHANDLEROPERATIONS_H
#define STRATEGR_MOUSEHANDLEROPERATIONS_H

#include "mousehandler.h"

namespace stg {

    struct mousehandler::operation {
        explicit operation(mousehandler *handler) : handler(handler) {}

        virtual ~operation() = default;

        virtual operation_type type() = 0;

        virtual void init(const mouse_event &event) = 0;

        virtual void perform(const mouse_event &event) = 0;

        virtual void teardown(const mouse_event &event) = 0;

//        friend bool operator==(operation *lhs, operation rhs) {
//            return lhs->type() == rhs.type();
//        }

//        friend bool operator!=(operation *lhs, operation rhs) {
//            return lhs->type() != rhs.type();
//        }

    protected:
        mousehandler *handler;
    };

    struct mousehandler::none_operation : public operation {
        using operation::operation;

        operation_type type() override {
            return none;
        }

        void init(const mouse_event &event) override {};

        void perform(const mouse_event &event) override {};

        void teardown(const mouse_event &event) override {};
    };


    struct mousehandler::drag_operation : public operation {
        using operation::operation;

        operation_type type() override {
            return drag;
        }

        void init(const mouse_event &event) override {};

        void perform(const mouse_event &event) override {};

        void teardown(const mouse_event &event) override {};
    };

    struct mousehandler::stretch_top_operation : public operation {
        using operation::operation;

        operation_type type() override {
            return stretch_top;
        }

        void init(const mouse_event &event) override {};

        void perform(const mouse_event &event) override {};

        void teardown(const mouse_event &event) override {};
    };

    struct mousehandler::stretch_bottom_operation : public operation {
        using operation::operation;

        operation_type type() override {
            return stretch_bottom;
        }

        void init(const mouse_event &event) override {};

        void perform(const mouse_event &event) override {};

        void teardown(const mouse_event &event) override {};
    };

    struct mousehandler::select_operation : public operation {
        using operation::operation;

        operation_type type() override {
            return select;
        }

        void init(const mouse_event &event) override {};

        void perform(const mouse_event &event) override {};

        void teardown(const mouse_event &event) override {};
    };
}

#endif //STRATEGR_MOUSEHANDLEROPERATIONS_H
