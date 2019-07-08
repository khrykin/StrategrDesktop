//
// Created by Dmitry Khrykin on 2019-07-06.
//

#include <catch2/catch.hpp>
#include "strategy.h"

TEST_CASE("Strategy settings", "[strategy]") {
    auto strategy = Strategy();


    SECTION("reduce number of slots") {
        strategy.setNumberOfTimeSlots(12);
        REQUIRE(strategy.activitySessions().size() == 12);
    }

    SECTION("increase number of slots") {
        auto newNumberOfSlots = strategy.numberOfTimeSlots() + 12;
        strategy.setNumberOfTimeSlots(newNumberOfSlots);
        REQUIRE(strategy.activitySessions().size() == newNumberOfSlots);
    }

    SECTION("change beginTime") {
        strategy.setBeginTime(666);
        const auto endTime = 666
                             + strategy.numberOfTimeSlots()
                               * strategy.timeSlotDuration();

        REQUIRE(strategy.activitySessions().first().beginTime() == 666);
        REQUIRE(strategy.activitySessions().last().endTime() == endTime);

    }

    SECTION("change time slot duration") {
        strategy.setTimeSlotDuration(1);

        const auto expectedEndTime = strategy.beginTime()
                                     + strategy.numberOfTimeSlots();

        REQUIRE(strategy.activitySessions().first().duration() == 1);
        REQUIRE(strategy.activitySessions().last().endTime() == expectedEndTime);

    }

}