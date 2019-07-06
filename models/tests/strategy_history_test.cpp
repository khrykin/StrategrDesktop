//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <catch2/catch.hpp>
#include "strategy.h"

TEST_CASE("Strategy history", "[strategy][history]") {
    auto strategy = Strategy();
    auto initialActivity = Activity("Some 1");

    strategy.addActivity(initialActivity);

    SECTION("activity addition") {
        strategy.addActivity(Activity("Some 2"));

        SECTION("undo") {
            strategy.undo();
            REQUIRE(strategy.activities().size() == 1);
        }

        SECTION("redo") {
            strategy.addActivity(Activity("Some 3"));
            strategy.undo();
            strategy.undo();
            strategy.redo();

            REQUIRE(strategy.activities().size() == 2);
        }
    }

    SECTION("activity editing") {
        const auto editedActivity = Activity("Some 1 Edited");
        strategy.editActivityAtIndex(0, editedActivity);

        SECTION("undo") {
            strategy.undo();
            REQUIRE(strategy.activities()[0] == initialActivity);
        }

        SECTION("redo") {

        }
    }

    SECTION("activity removal") {
        const auto editedActivity = Activity("Some 1 Edited");
        strategy.addActivity(Activity("Some 2"));
        strategy.removeActivityAtIndex(0);

        SECTION("undo") {
            strategy.undo();
            REQUIRE(strategy.activities().size() == 2);
        }

        SECTION("redo") {
            strategy.undo();
            strategy.undo();
            strategy.redo();

            REQUIRE(strategy.activities().size() == 2);
        }
    }


}