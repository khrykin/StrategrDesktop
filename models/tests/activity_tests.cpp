//
// Created by Dmitry Khrykin on 2019-07-01.
//
#include <sstream>

#include <catch2/catch.hpp>
#include "activity.h"
#include "activityinvalidpropertyexception.h"

const auto BLACK_COLOR = "#000000";
const auto RED_COLOR = "#ff0000";

TEST_CASE("stg::activity default constructor", "[activity]") {
    SECTION("should set name and default color") {
        auto activity = stg::activity("Some");

        REQUIRE(activity.name() == "Some");
        REQUIRE(activity.color() == BLACK_COLOR);
    }

    SECTION("should set _name and custom color") {
        auto activityWithColor = stg::activity("Some 2", RED_COLOR);

        REQUIRE(activityWithColor.name() == "Some 2");
        REQUIRE(activityWithColor.color() == RED_COLOR);
    }

    SECTION("shouldn't allow empty name") {
        REQUIRE_THROWS_AS(stg::activity(""), stg::activity::invalid_property_exception);
    }

    SECTION("shouldn't allow only-whitespaces name") {
        REQUIRE_THROWS_AS(stg::activity("  \t  "),
                          stg::activity::invalid_property_exception);
    }
}

TEST_CASE("stg::activity immutability", "[activity]") {
    const auto intial_name = "Some";
    const auto activity = stg::activity(intial_name);

    SECTION("copy changing name") {
        REQUIRE(activity.copy_changing_name("Some 2")
                == stg::activity("Some 2"));
        REQUIRE(activity == stg::activity(intial_name));
    }

    SECTION("copy changing color") {
        REQUIRE(activity.copy_changing_color(RED_COLOR)
                == stg::activity(intial_name, RED_COLOR));
        REQUIRE(activity == stg::activity(intial_name));
    }
}

TEST_CASE("stg::activity equality", "[activity]") {
    auto activity1 = stg::activity("Some");
    auto activity2 = stg::activity("Some");
    auto activity3 = stg::activity("Some", RED_COLOR);
    auto activity4 = stg::activity("Some 2", RED_COLOR);

    REQUIRE(activity1 == activity2);
    REQUIRE(activity1 != activity3);
    REQUIRE(activity4 != activity1);
}

TEST_CASE("stg::activity ostream operator", "[activity]") {
    auto activity = stg::activity("Some", RED_COLOR);
    std::stringstream sstream;
    sstream << activity;

    REQUIRE(sstream.str() == "activity(Some, #ff0000)");
}