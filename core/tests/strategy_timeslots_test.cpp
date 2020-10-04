//
// Created by Dmitry Khrykin on 2019-07-05.
//

#include <catch2/catch.hpp>

#include "strategy.h"

TEST_CASE("Strategy activity sessions", "[strategy][sessions]") {
    auto strategy = stg::strategy();

    strategy.add_activity(stg::activity("Some 0"));
    strategy.add_activity(stg::activity("Some 1"));
    strategy.add_activity(stg::activity("Some 2"));

    const auto &first_activity = strategy.activities()[0];
    const auto &second_activity = strategy.activities()[1];
    const auto &third_activity = strategy.activities()[2];

    SECTION("put activity in single slot") {
        strategy.place_activity(0, {0});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
    }

    SECTION("put activity in adjacent slots") {
        strategy.place_activity(0, {0, 1});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[0].length() == 2);
    }

    SECTION("put activity in two groups of adjacent slots") {
        strategy.place_activity(0, {0, 1, 3, 4});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[0].length() == 2);

        REQUIRE(strategy.sessions()[1].activity == stg::strategy::no_activity);
        REQUIRE(strategy.sessions()[1].length() == 1);

        REQUIRE(strategy.sessions()[2].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[2].length() == 2);
    }

    SECTION("put activity in two last adjacent slots") {
        const auto last_index = strategy.number_of_time_slots() - 1;
        const auto penultimate_index = last_index - 1;

        strategy.place_activity(0, {penultimate_index, last_index});

        REQUIRE(strategy.sessions().last().activity == &strategy.activities()[0]);
        REQUIRE(strategy.sessions().last().length() == 2);
    }

    SECTION("put two activities in two adjacent groups of adjacent slots") {
        strategy.place_activity(0, {0, 1});
        strategy.place_activity(1, {2, 3, 4});

        REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));
        REQUIRE(strategy.sessions()[0].length() == 2);

        REQUIRE(strategy.sessions()[1].activity == strategy.activities().at(1));
        REQUIRE(strategy.sessions()[1].length() == 3);
    }

    SECTION("fill time slots") {
        strategy.place_activity(0, {0});
        strategy.place_activity(1, {1});

        SECTION("up") {
            strategy.begin_resizing();
            strategy.fill_time_slots(1, 0);
            strategy.end_resizing();

            REQUIRE(strategy.sessions()[0].length() == 2);
            REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(1));

            REQUIRE(strategy.sessions()[1].length() == strategy.number_of_time_slots() - 2);
            REQUIRE(strategy.sessions()[1].activity == stg::strategy::no_activity);
        }

        SECTION("down") {
            strategy.begin_resizing();
            strategy.fill_time_slots(0, 1);
            strategy.end_resizing();

            REQUIRE(strategy.sessions()[0].length() == 2);
            REQUIRE(strategy.sessions()[0].activity == strategy.activities().at(0));

            REQUIRE(strategy.sessions()[1].length() == strategy.number_of_time_slots() - 2);
            REQUIRE(strategy.sessions()[1].activity == stg::strategy::no_activity);
        }

        SECTION("empty slot as a source") {
            strategy.begin_resizing();
            strategy.fill_time_slots(2, 0);
            strategy.end_resizing();

            REQUIRE(strategy.sessions().size() == 1);
            REQUIRE(strategy.sessions()[0].length() == strategy.number_of_time_slots());
            REQUIRE(strategy.sessions()[0].activity == stg::strategy::no_activity);
        }
    }

    SECTION("drag activity session") {
        strategy.place_activity(1, {1, 2});
        strategy.place_activity(2, {3, 4, 5});

        SECTION("up") {
            strategy.begin_dragging(2);
            strategy.drag_session(2, -2);
            strategy.end_dragging();

            REQUIRE(strategy.sessions()[0].activity == stg::strategy::no_activity);
            REQUIRE(strategy.sessions()[0].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == strategy.activities().at(2));
            REQUIRE(strategy.sessions()[1].length() == 3);
            REQUIRE(strategy.sessions()[2].activity == strategy.activities().at(1));
            REQUIRE(strategy.sessions()[2].length() == 2);
            REQUIRE(strategy.sessions()[3].activity == stg::strategy::no_activity);
            REQUIRE(strategy.sessions()[3].length() == strategy.number_of_time_slots() - 6);
        }

        SECTION("down") {
            strategy.begin_dragging(1);
            strategy.drag_session(1, 2);
            strategy.end_dragging();

            REQUIRE(strategy.sessions()[0].activity == stg::strategy::no_activity);
            REQUIRE(strategy.sessions()[0].length() == 1);
            REQUIRE(strategy.sessions()[1].activity == strategy.activities().at(2));
            REQUIRE(strategy.sessions()[1].length() == 2);
            REQUIRE(strategy.sessions()[2].activity == strategy.activities().at(1));
            REQUIRE(strategy.sessions()[2].length() == 2);
            REQUIRE(strategy.sessions()[3].activity == strategy.activities().at(2));
            REQUIRE(strategy.sessions()[3].length() == 1);
            REQUIRE(strategy.sessions()[4].activity == stg::strategy::no_activity);
            REQUIRE(strategy.sessions()[4].length() == strategy.number_of_time_slots() - 6);
        }
    }

    SECTION("activity session index for time slot index") {
        strategy.place_activity(1, {1, 2});

        REQUIRE(strategy.sessions().session_index_for_time_slot_index(0) == 0);
        REQUIRE(strategy.sessions().session_index_for_time_slot_index(2) == 1);
    }
}

TEST_CASE("Strategy sessions on change notifications", "[strategy][sessions]") {
    auto strategy = stg::strategy();
    auto callbackWasCalled = false;

    strategy.sessions()
        .add_on_change_callback([&callbackWasCalled]() {
            callbackWasCalled = true;
        });

    strategy.add_activity(stg::activity("Some"));

    SECTION("should notify on change") {
        strategy.place_activity(0, {0});

        REQUIRE(callbackWasCalled);
    }

    SECTION("shouldn't notify when there's no change") {
        strategy.make_empty_at({0});

        REQUIRE(!callbackWasCalled);
    }
}