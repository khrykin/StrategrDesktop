//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <catch2/catch.hpp>
#include "Strategy.h"

TEST_CASE("Strategy settings", "[strategy]") {
    auto strategy = Strategy();


    SECTION("reduce number of slots") {
        strategy.setNumberOfTimeSlots(12);
        REQUIRE(strategy.sessions().size() == 12);
    }

    SECTION("increase number of slots") {
        auto newNumberOfSlots = strategy.numberOfTimeSlots() + 12;
        strategy.setNumberOfTimeSlots(newNumberOfSlots);
        REQUIRE(strategy.sessions().size() == newNumberOfSlots);
    }

    SECTION("change beginTime") {
        strategy.setBeginTime(666);
        const auto endTime = 666
                             + strategy.numberOfTimeSlots()
                               * strategy.timeSlotDuration();

        REQUIRE(strategy.sessions().first().beginTime() == 666);
        REQUIRE(strategy.sessions().last().endTime() == endTime);

    }

    SECTION("change time slot duration") {
        strategy.setTimeSlotDuration(1);

        const auto expectedEndTime = strategy.beginTime()
                                     + strategy.numberOfTimeSlots();

        REQUIRE(strategy.sessions().first().duration() == 1);
        REQUIRE(strategy.sessions().last().endTime() == expectedEndTime);

    }

}