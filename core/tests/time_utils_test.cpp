//
// Created by Dmitry Khrykin on 28.07.2020.
//
#include <thread>

#include <catch2/catch.hpp>

#include "time_utils.h"

TEST_CASE("Time Utilities", "[time_utils]") {
    using namespace std::chrono_literals;

    SECTION("default time source") {
        auto sec1 = stg::time_utils::current_seconds();
        std::this_thread::sleep_for(1s);
        auto sec2 = stg::time_utils::current_seconds();

        // >= rather than == 1: current_seconds() is whole-second-floored, so
        // if sec1 was sampled near the end of its second, a 1s sleep (plus
        // scheduler jitter, more pronounced on loaded CI runners) can cross
        // two second boundaries rather than one.
        REQUIRE(sec2 - sec1 >= 1);
    }

    SECTION("custom time source") {
        stg::time_utils::set_time_source([] { return 60; });

        REQUIRE(stg::time_utils::current_seconds() == 60);
        REQUIRE(stg::time_utils::current_minutes() == 1);

        stg::time_utils::set_time_source(nullptr);
    }
}