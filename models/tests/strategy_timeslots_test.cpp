//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include <catch2/catch.hpp>
#include "Strategy.h"

TEST_CASE("Strategy activity sessions", "[strategy][sessions]") {
    auto strategy = Strategy();

    strategy.addActivity(Activity("Some 0"));
    strategy.addActivity(Activity("Some 1"));
    strategy.addActivity(Activity("Some 2"));

    const auto &firstActivity = strategy.activities()[0];
    const auto &secondActivity = strategy.activities()[1];
    const auto &thirdActivity = strategy.activities()[2];

    SECTION("default constructor") {
        REQUIRE(strategy.numberOfTimeSlots() == Strategy::Defaults::numberOfTimeSlots);
        REQUIRE(strategy.timeSlotDuration() == Strategy::Defaults::timeSlotDuration);
        REQUIRE(strategy.beginTime() == Strategy::Defaults::beginTime);

        REQUIRE(strategy.sessions().size() == strategy.numberOfTimeSlots());
        REQUIRE(strategy.sessions().first().beginTime() == strategy.beginTime());
        REQUIRE(strategy.sessions().last().endTime() == strategy.endTime());
    }

    SECTION("put activity in single slot") {
        strategy.putActivityInTimeSlotsAtIndices(0, {0});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
    }

    SECTION("put activity in adjacent slots") {
        strategy.putActivityInTimeSlotsAtIndices(0, {0, 1});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[0].length() == 2);
    }

    SECTION("put activity in two groups of adjacent slots") {
        strategy.putActivityInTimeSlotsAtIndices(0, {0, 1, 3, 4});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[0].length() == 2);

        REQUIRE(strategy.sessions()[1].activity == Strategy::NoActivity);
        REQUIRE(strategy.sessions()[1].length() == 1);

        REQUIRE(strategy.sessions()[2].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[2].length() == 2);
    }

    SECTION("put activity in two last adjacent slots") {
        const auto penultimateIndex = strategy.numberOfTimeSlots() - 2;
        const auto lastIndex = strategy.numberOfTimeSlots() - 1;

        strategy.putActivityInTimeSlotsAtIndices(0,
                                                 {penultimateIndex, lastIndex});

        REQUIRE(strategy.sessions()[penultimateIndex].activity == &strategy.activities()[0]);
        REQUIRE(strategy.sessions()[penultimateIndex].length() == 2);
    }

    SECTION("put two activities in two adjacent groups of adjacent slots") {
        strategy.putActivityInTimeSlotsAtIndices(0, {0, 1});
        strategy.putActivityInTimeSlotsAtIndices(1, {2, 3, 4});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[0].length() == 2);

        REQUIRE(strategy.sessions()[1].activity == strategy.activities().at(1));
        REQUIRE(strategy.sessions()[1].length() == 3);
    }

    SECTION("fill timeslots") {
        strategy.putActivityInTimeSlotsAtIndices(0, {0});
        strategy.putActivityInTimeSlotsAtIndices(1, {1});

        SECTION("up") {
            strategy.fillTimeSlots(0, 0);
            REQUIRE(strategy.sessions()[0].length() == 2);
            REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(1));

            REQUIRE(strategy.sessions()[1].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == Strategy::NoActivity);
        }

        SECTION("down") {
            strategy.fillTimeSlots(0, 0);
            REQUIRE(strategy.sessions()[0].length() == 2);
            REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));

            REQUIRE(strategy.sessions()[1].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == Strategy::NoActivity);
        }

        SECTION("empty slot as a source") {
            strategy.fillTimeSlots(0, 0);

            REQUIRE(strategy.sessions()[0].length() == 1);
            REQUIRE(strategy.sessions()[0].activity == Strategy::NoActivity);

            REQUIRE(strategy.sessions()[1].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == Strategy::NoActivity);
        }

    }

    SECTION("drag activity session") {
        strategy.putActivityInTimeSlotsAtIndices(1, {1, 2});
        strategy.putActivityInTimeSlotsAtIndices(2, {3, 4, 5});

        SECTION("up") {
            strategy.beginDragging(2);
            strategy.dragActivitySession(2, -2);
            strategy.endDragging();

            REQUIRE(strategy.sessions()[0].activity == Strategy::NoActivity);
            REQUIRE(strategy.sessions()[0].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == strategy.activities().at(2));
            REQUIRE(strategy.sessions()[1].length() == 3);
            REQUIRE(strategy.sessions()[2].activity == strategy.activities().at(1));
            REQUIRE(strategy.sessions()[2].length() == 2);
            REQUIRE(strategy.sessions()[3].activity == Strategy::NoActivity);
            REQUIRE(strategy.sessions()[3].length() == 1);
        }

        SECTION("down") {
            strategy.beginDragging(1);
            strategy.dragActivitySession(1, 2);
            strategy.endDragging();

            REQUIRE(strategy.sessions()[0].activity == Strategy::NoActivity);
            REQUIRE(strategy.sessions()[0].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == strategy.activities().at(2));
            REQUIRE(strategy.sessions()[1].length() == 2);
            REQUIRE(strategy.sessions()[2].activity == strategy.activities().at(1));
            REQUIRE(strategy.sessions()[2].length() == 2);
            REQUIRE(strategy.sessions()[3].activity == strategy.activities().at(2));
            REQUIRE(strategy.sessions()[3].length() == 1);
            REQUIRE(strategy.sessions()[4].activity == Strategy::NoActivity);
            REQUIRE(strategy.sessions()[4].length() == 1);
        }

    }

    SECTION("activity session index for time slot index") {
        strategy.putActivityInTimeSlotsAtIndices(1, {1, 2});

        REQUIRE(strategy.sessions().sessionIndexForTimeSlotIndex(0) == 0);
        REQUIRE(strategy.sessions().sessionIndexForTimeSlotIndex(2) == 1);
    }
}

TEST_CASE("Strategy sessions on change notifications", "[strategy][sessions]") {
    auto strategy = Strategy();
    auto callbackWasCalled = false;

    strategy.sessions()
            .addOnChangeCallback([&callbackWasCalled]() {
                callbackWasCalled = true;
            });

    strategy.addActivity(Activity("Some"));

    SECTION("should notify on change") {
        strategy.putActivityInTimeSlotsAtIndices(0, {0});

        REQUIRE(callbackWasCalled);
    }

    SECTION("shouldn't notify when there's no change") {
        strategy.emptyTimeSlotsAtIndices({0});

        REQUIRE(!callbackWasCalled);
    }
}