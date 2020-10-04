//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <catch2/catch.hpp>

#include "strategy.h"

TEST_CASE("Strategy settings", "[strategy][settings]") {
    auto strategy = stg::strategy();

    SECTION("change begin time") {
        strategy.set_begin_time(666);

        const auto end_time = 666 + strategy.number_of_time_slots() * strategy.time_slot_duration();

        REQUIRE(strategy.sessions().first().begin_time() == 666);
        REQUIRE(strategy.sessions().last().end_time() == end_time);
    }

    SECTION("change time slot duration") {
        const auto expected_end_time = strategy.end_time();

        strategy.set_time_slot_duration(1);

        REQUIRE(strategy.time_slots().first().duration == 1);
        REQUIRE(strategy.time_slots().last().end_time() == expected_end_time);
    }

    SECTION("change end time") {
        const auto expected_number_of_slots = strategy.number_of_time_slots() + 1;

        strategy.set_end_time(strategy.end_time() + strategy.time_slot_duration());

        REQUIRE(strategy.number_of_time_slots() == expected_number_of_slots);
    }
}