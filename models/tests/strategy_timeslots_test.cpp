//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include <catch2/catch.hpp>
#include "strategy.h"


TEST_CASE("Strategy activity sessions", "[strategy][sessions]") {
    auto strategy = Strategy();

    strategy.addActivity(Activity("Some 1"));
    strategy.addActivity(Activity("Some 2"));
    strategy.addActivity(Activity("Some 3"));

    const auto &firstActivity = strategy.activities()[0];
    const auto &secondActivity = strategy.activities()[1];
    const auto &thirdActivity = strategy.activities()[2];

    SECTION("default constructor") {
        REQUIRE(strategy.numberOfTimeSlots() == Strategy::Defaults::numberOfTimeSlots);
        REQUIRE(strategy.timeSlotDuration() == Strategy::Defaults::timeSlotDuration);
        REQUIRE(strategy.beginTime() == Strategy::Defaults::beginTime);

        REQUIRE(strategy.activitySessions().size() == strategy.numberOfTimeSlots());
        REQUIRE(strategy.activitySessions().first().beginTime() == strategy.beginTime());
        REQUIRE(strategy.activitySessions().last().endTime() == strategy.endTime());
    }

    SECTION("put activity in single slot") {
        strategy.putActivityInTimeSlotsAtIndices(&firstActivity, {0});

        REQUIRE(strategy.activitySessions()[0].activity == &strategy.activities()[0]);
    }

    SECTION("put activity in adjacent slots") {
        strategy.putActivityInTimeSlotsAtIndices(&firstActivity, {0, 1});

        REQUIRE(strategy.activitySessions()[0].activity == &strategy.activities()[0]);
        REQUIRE(strategy.activitySessions()[0].length() == 2);
    }

    SECTION("put activity in two groups of adjacent slots") {
        strategy.putActivityInTimeSlotsAtIndices(&firstActivity, {0, 1, 3, 4});

        REQUIRE(strategy.activitySessions()[0].activity == &strategy.activities()[0]);
        REQUIRE(strategy.activitySessions()[0].length() == 2);

        REQUIRE(strategy.activitySessions()[1].activity == Strategy::NoActivity);
        REQUIRE(strategy.activitySessions()[1].length() == 1);

        REQUIRE(strategy.activitySessions()[2].activity == &strategy.activities()[0]);
        REQUIRE(strategy.activitySessions()[2].length() == 2);
    }

    SECTION("put activity in two last adjacent slots") {
        const auto penultimateIndex = strategy.numberOfTimeSlots() - 2;
        const auto lastIndex = strategy.numberOfTimeSlots() - 1;

        strategy.putActivityInTimeSlotsAtIndices(&firstActivity,
                                                 {penultimateIndex, lastIndex});

        REQUIRE(strategy.activitySessions()[penultimateIndex].activity == &strategy.activities()[0]);
        REQUIRE(strategy.activitySessions()[penultimateIndex].length() == 2);
    }

    SECTION("put two activities in two adjacent groups of adjacent slots") {
        strategy.putActivityInTimeSlotsAtIndices(&firstActivity, {0, 1});
        strategy.putActivityInTimeSlotsAtIndices(&secondActivity, {2, 3, 4});

        REQUIRE(strategy.activitySessions()[0].activity == &strategy.activities()[0]);
        REQUIRE(strategy.activitySessions()[0].length() == 2);

        REQUIRE(strategy.activitySessions()[1].activity == &strategy.activities()[1]);
        REQUIRE(strategy.activitySessions()[1].length() == 3);
    }
}

TEST_CASE("Strategy sessions on change notifications", "[strategy][sessions]") {
    auto strategy = Strategy();
    auto callbackWasCalled = false;

    strategy.activitySessions()
            .setOnChangeCallback([&callbackWasCalled]() {
                callbackWasCalled = true;
            });

    auto activity = Activity("Some");

    SECTION("should notify on change") {
        strategy.putActivityInTimeSlotsAtIndices(&activity, {0});

        REQUIRE(callbackWasCalled);
    }

    SECTION("shouldn't notify when there's no change") {
        strategy.putActivityInTimeSlotsAtIndices(Strategy::NoActivity, {0});

        REQUIRE(!callbackWasCalled);
    }
}