//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <catch2/catch.hpp>
#include "strategy.h"

TEST_CASE("Strategy settings", "[strategy]") {
    auto strategy = stg::strategy();


    SECTION("reduce number of slots") {
        strategy.set_number_of_time_slots(12);
        REQUIRE(strategy.sessions().size() == 12);
    }

    SECTION("increase number of slots") {
        auto newNumberOfSlots = strategy.number_of_time_slots() + 12;
        strategy.set_number_of_time_slots(newNumberOfSlots);
        REQUIRE(strategy.sessions().size() == newNumberOfSlots);
    }

    SECTION("change beginTime") {
        strategy.set_begin_time(666);
        const auto endTime = 666
                             + strategy.number_of_time_slots()
                               * strategy.time_slot_duration();

        REQUIRE(strategy.sessions().first().begin_time() == 666);
        REQUIRE(strategy.sessions().last().end_time() == endTime);

    }

    SECTION("change time slot duration") {
        strategy.set_time_slot_duration(1);

        const auto expectedEndTime = strategy.begin_time()
                                     + strategy.number_of_time_slots();

        REQUIRE(strategy.sessions().first().duration() == 1);
        REQUIRE(strategy.sessions().last().end_time() == expectedEndTime);

    }

}