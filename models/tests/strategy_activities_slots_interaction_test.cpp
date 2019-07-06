//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <catch2/catch.hpp>
#include "strategy.h"
#include <iostream>

TEST_CASE("Strategy activities and slots interaction") {
    auto strategy = Strategy();

    strategy.addActivity(Activity("Some 1"));
    const auto &activity = strategy.activities()[0];

    strategy.putActivityInTimeSlotsAtIndices(&activity, {0, 2});

    SECTION("deleting activity should also remove it from slots") {
        strategy.removeActivityAtIndex(0);
        REQUIRE(strategy.activitySessions()[0].activity
                == Strategy::NoActivity);
    }

    SECTION("editing (replacing) activity should also edit (replace) it in slots") {
        strategy.editActivityAtIndex(0, Activity("Some Edited"));
        const auto &updatedActivity = strategy.activities()[0];

        REQUIRE(strategy.activitySessions()[0].activity
                == &updatedActivity);

        REQUIRE(strategy.activitySessions()[2].activity
                == &updatedActivity);
    }
}