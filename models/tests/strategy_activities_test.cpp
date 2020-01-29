//
// Created by Dmitry Khrykin on 2019-07-03.
//

#include <catch2/catch.hpp>
#include "strategy.h"

class ActivitiesListener {
public:
    bool &_callbackWasCalled;

    void onChange() {
        _callbackWasCalled = true;
    }
};

TEST_CASE("Strategy activities", "[strategy][activities]") {
    auto strategy = stg::strategy();

    SECTION("should add activity") {
        auto activity = stg::activity("Some");

        strategy.add_activity(activity);

        REQUIRE(strategy.activities()[0] == activity);

        SECTION("should throw if activity already present") {
            REQUIRE_THROWS_AS(strategy.add_activity(stg::activity("Some")),
                              stg::activity_list::already_present_exception);
        }
    }

    SECTION("should remove activity") {
        auto activity1 = stg::activity("Some 1");
        auto activity2 = stg::activity("Some 2");

        strategy.add_activity(activity1);
        strategy.add_activity(activity2);

        SECTION("at index") {
            strategy.delete_activity(0);
            REQUIRE(strategy.activities()[0] == activity2);
            REQUIRE(strategy.activities().size() == 1);
        }
    }

    SECTION("should edit activity") {
        auto activity = stg::activity("Some 1");
        auto activityEdited = stg::activity("Some 1 Edited");

        strategy.add_activity(activity);

        SECTION("at index") {
            strategy.edit_activity(0, activityEdited);
            REQUIRE(strategy.activities()[0] == activityEdited);
        }

        SECTION("should throw if activity already present") {
            REQUIRE_THROWS_AS(strategy.edit_activity(0, stg::activity("Some 1")),
                              stg::activity_list::already_present_exception);
        }
    }

    SECTION("should drag activity") {
        auto activity1 = stg::activity("Some 1");
        auto activity2 = stg::activity("Some 2");
        auto activity3 = stg::activity("Some 3");

        strategy.add_activity(activity1);
        strategy.add_activity(activity2);
        strategy.add_activity(activity3);

        SECTION("up") {
            auto fromIndex = 2;
            auto toIndex = 0;

            strategy.drag_activity(fromIndex, toIndex);

            REQUIRE(strategy.activities()[0] == activity3);
            REQUIRE(strategy.activities()[1] == activity1);
            REQUIRE(strategy.activities()[2] == activity2);
        }

        SECTION("down") {
            auto fromIndex = 0;
            auto toIndex = 2;

            strategy.drag_activity(fromIndex, toIndex);

            REQUIRE(strategy.activities()[0] == activity2);
            REQUIRE(strategy.activities()[1] == activity3);
            REQUIRE(strategy.activities()[2] == activity1);
        }
    }

    SECTION("should notify on change") {
        auto activity = stg::activity("Some 1");
        bool callbackWasCalled = false;

        strategy.activities().add_on_change_callback([&callbackWasCalled]() {
            callbackWasCalled = true;
        });

        strategy.add_activity(activity);

        SECTION("when added") {
            REQUIRE(callbackWasCalled);
        }

        SECTION("when removed") {
            callbackWasCalled = false;
            strategy.delete_activity(0);

            REQUIRE(callbackWasCalled);
        }

        SECTION("when edited") {
            callbackWasCalled = false;
            strategy.edit_activity(0, stg::activity("Edited"));

            REQUIRE(callbackWasCalled);
        }

        SECTION("when dragged") {
            auto activity2 = stg::activity("Some 2");
            strategy.add_activity(activity2);

            callbackWasCalled = false;
            strategy.drag_activity(0, 1);

            REQUIRE(callbackWasCalled);
        }

        SECTION("class method as callback") {
            auto activity2 = stg::activity("Some 2");
            ActivitiesListener listener{callbackWasCalled};

            strategy.activities().add_on_change_callback(&listener,
                                                         &ActivitiesListener::onChange);
            callbackWasCalled = false;
            strategy.add_activity(activity2);

            REQUIRE(callbackWasCalled);
        }
    }
}


