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
    auto strategy = Strategy();

    SECTION("should add activity") {
        auto activity = Activity("Some");

        strategy.addActivity(activity);

        REQUIRE(strategy.activities()[0] == activity);

        SECTION("should throw if activity already present") {
            REQUIRE_THROWS_AS(strategy.addActivity(Activity("Some")),
                              ActivityList::AlreadyPresentException);
        }
    }

    SECTION("should remove activity") {
        auto activity1 = Activity("Some 1");
        auto activity2 = Activity("Some 2");

        strategy.addActivity(activity1);
        strategy.addActivity(activity2);

        SECTION("at index") {
            strategy.removeActivityAtIndex(0);
            REQUIRE(strategy.activities()[0] == activity2);
        }
    }

    SECTION("should edit activity") {
        auto activity = Activity("Some 1");
        auto activityEdited = Activity("Some 1 Edited");

        strategy.addActivity(activity);

        SECTION("at index") {
            strategy.editActivityAtIndex(0, activityEdited);
            REQUIRE(strategy.activities()[0] == activityEdited);
        }

        SECTION("should throw if activity already present") {
            REQUIRE_THROWS_AS(strategy.editActivityAtIndex(0, Activity("Some 1")),
                              ActivityList::AlreadyPresentException);
        }
    }

    SECTION("should drag activity") {
        auto activity1 = Activity("Some 1");
        auto activity2 = Activity("Some 2");
        auto activity3 = Activity("Some 3");

        strategy.addActivity(activity1);
        strategy.addActivity(activity2);
        strategy.addActivity(activity3);

        SECTION("up") {
            auto fromIndex = 2;
            auto toIndex = 0;

            strategy.dragActivity(fromIndex, toIndex);

            REQUIRE(strategy.activities()[0] == activity3);
            REQUIRE(strategy.activities()[1] == activity1);
            REQUIRE(strategy.activities()[2] == activity2);
        }

        SECTION("down") {
            auto fromIndex = 0;
            auto toIndex = 2;

            strategy.dragActivity(fromIndex, toIndex);

            REQUIRE(strategy.activities()[0] == activity2);
            REQUIRE(strategy.activities()[1] == activity3);
            REQUIRE(strategy.activities()[2] == activity1);
        }
    }

    SECTION("should notify on change") {
        auto activity = Activity("Some 1");
        bool callbackWasCalled = false;

        strategy.activities().setOnChangeCallback([&callbackWasCalled]() {
            callbackWasCalled = true;
        });

        strategy.addActivity(activity);

        SECTION("when added") {
            REQUIRE(callbackWasCalled);
        }

        SECTION("when removed") {
            callbackWasCalled = false;
            strategy.removeActivityAtIndex(0);

            REQUIRE(callbackWasCalled);
        }

        SECTION("when edited") {
            callbackWasCalled = false;
            strategy.editActivityAtIndex(0, Activity("Edited"));

            REQUIRE(callbackWasCalled);
        }

        SECTION("when dragged") {
            auto activity2 = Activity("Some 2");
            strategy.addActivity(activity2);

            callbackWasCalled = false;
            strategy.dragActivity(0, 1);

            REQUIRE(callbackWasCalled);
        }

        SECTION("class method as callback") {
            auto activity2 = Activity("Some 2");
            ActivitiesListener listener{callbackWasCalled};

            strategy.activities().setOnChangeCallback(&listener,
                                                      &ActivitiesListener::onChange);
            callbackWasCalled = false;
            strategy.addActivity(activity2);

            REQUIRE(callbackWasCalled);
        }
    }
}


