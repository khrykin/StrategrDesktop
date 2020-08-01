//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <iostream>

#include <catch2/catch.hpp>

#include "strategy.h"

TEST_CASE("Strategy activities and slots interaction") {
    auto strategy = stg::strategy();

    strategy.add_activity(stg::activity("Some 1"));
    const auto &activity = strategy.activities()[0];

    strategy.place_activity(0, {0, 2});

    SECTION("deleting activity should also remove it from slots") {
        strategy.delete_activity(0);

        REQUIRE(strategy.sessions()[0].activity == stg::strategy::no_activity);
    }

    SECTION("editing (replacing) activity should also edit (replace) it in slots") {
        strategy.edit_activity(0, stg::activity("Some Edited"));
        const auto &updatedActivity = strategy.activities()[0];

        REQUIRE(strategy.sessions()[0].activity == &updatedActivity);
        REQUIRE(strategy.sessions()[2].activity == &updatedActivity);
    }
}