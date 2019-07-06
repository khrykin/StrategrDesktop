//
// Created by Dmitry Khrykin on 2019-07-01.
//
#include <sstream>

#include <catch2/catch.hpp>
#include "activity.h"
#include "activityinvalidpropertyexception.h"

const auto BLACK_COLOR = "#000000";
const auto RED_COLOR = "#ff0000";

TEST_CASE("Activity default constructor", "[activity]") {
    SECTION("should set name and default color") {
        auto activity = Activity("Some");

        REQUIRE(activity.name() == "Some");
        REQUIRE(activity.color() == BLACK_COLOR);
    }

    SECTION("should set _name and custom color") {
        auto activityWithColor = Activity("Some 2", RED_COLOR);

        REQUIRE(activityWithColor.name() == "Some 2");
        REQUIRE(activityWithColor.color() == RED_COLOR);
    }

    SECTION("shouldn't allow empty name") {
        REQUIRE_THROWS_AS(Activity(""), Activity::InvalidPropertyException);
    }

    SECTION("shouldn't allow only-whitespaces name") {
        REQUIRE_THROWS_AS(Activity("  \t  "),
                          Activity::InvalidPropertyException);
    }
}

TEST_CASE("Activity immutability", "[activity]") {
    const auto intialName = "Some";
    const auto activity = Activity(intialName);

    SECTION("copy changing name") {
        REQUIRE(activity.copyChangingName("Some 2")
                == Activity("Some 2"));
        REQUIRE(activity == Activity(intialName));
    }

    SECTION("copy changing color") {
        REQUIRE(activity.copyChangingColor(RED_COLOR)
                == Activity(intialName, RED_COLOR));
        REQUIRE(activity == Activity(intialName));
    }
}

TEST_CASE("Activity equality", "[activity]") {
    auto activity1 = Activity("Some");
    auto activity2 = Activity("Some");
    auto activity3 = Activity("Some", RED_COLOR);
    auto activity4 = Activity("Some 2", RED_COLOR);

    REQUIRE(activity1 == activity2);
    REQUIRE(activity1 != activity3);
    REQUIRE(activity4 != activity1);
}

TEST_CASE("Activity ostream operator", "[activity]") {
    auto activity = Activity("Some", RED_COLOR);
    std::stringstream sstream;
    sstream << activity;

    REQUIRE(sstream.str() == "Activity(Some, #ff0000)");
}