//
// Created by Dmitry Khrykin on 2019-11-23.
//

#include <catch2/catch.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include "Strategy.h"
#include "JSON.h"

std::string readTestFile(const std::string &filename) {
    using namespace boost::filesystem;
    const auto testFilePath = path{__FILE__}.parent_path() / path{filename};

    std::ifstream t(testFilePath.string());

    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
}

void testStrategyFile(const std::string &filename) {
    auto strategy = Strategy::fromJsonString(readTestFile(filename));

    REQUIRE(strategy->timeSlotDuration() == 10);
    REQUIRE(strategy->beginTime() == 370);
    REQUIRE(strategy->numberOfTimeSlots() == 10);
    REQUIRE(strategy->timeSlots()[0].beginTime == 370);
    REQUIRE(strategy->timeSlots()[0].duration == 10);
    REQUIRE(strategy->timeSlots()[0].activity == Strategy::NoActivity);
    REQUIRE(strategy->timeSlots()[2].activity == strategy->activities().at(0));

    REQUIRE(strategy->activities().size() == 3);
    REQUIRE(strategy->activities()[0].name() == "Exercise");
    REQUIRE(strategy->activities()[0].color() == "#ff4136");
    REQUIRE(strategy->activities()[1].name() == "Meal");
    REQUIRE(strategy->activities()[1].color() == "#ffd700");
    REQUIRE(strategy->activities()[2].name() == "Commute");
    REQUIRE(strategy->activities()[2].color() == "#000000");
}

TEST_CASE("JSON Parser") {
    SECTION("parse from JSON string") {
        testStrategyFile("test.stg");

        SECTION("wrong activity index in slots") {
            testStrategyFile("test_wrong_activity_index.stg");
        }
    }

    SECTION("serialize to JSON string") {
        auto strategy = Strategy();
        strategy.setBeginTime(100);
        strategy.setTimeSlotDuration(10);

        strategy.addActivity(Activity("Some 1", "#ff0000"));
        strategy.addActivity(Activity("Some 2", "#00ff00"));

        strategy.setNumberOfTimeSlots(5);

        strategy.putActivityInTimeSlotsAtIndices(0, {0, 1});
        strategy.putActivityInTimeSlotsAtIndices(1, {2, 3});

        auto jsonString = strategy.toJsonString();

        auto json = nlohmann::json::parse(jsonString);

        REQUIRE(json[JSON::Keys::startTime] == 100);
        REQUIRE(json[JSON::Keys::slotDuration] == 10);

        REQUIRE(json[JSON::Keys::activities].is_array());
        REQUIRE(json[JSON::Keys::slots].is_array());

        REQUIRE(json[JSON::Keys::slots].size() == 5);

        REQUIRE(json[JSON::Keys::activities][0]["name"] == "Some 1");
        REQUIRE(json[JSON::Keys::activities][0]["color"] == "#ff0000");

        REQUIRE(json[JSON::Keys::activities][1]["name"] == "Some 2");
        REQUIRE(json[JSON::Keys::activities][1]["color"] == "#00ff00");

        REQUIRE(json[JSON::Keys::slots][0] == 0);
        REQUIRE(json[JSON::Keys::slots][1] == 0);

        REQUIRE(json[JSON::Keys::slots][2] == 1);
        REQUIRE(json[JSON::Keys::slots][3] == 1);

        REQUIRE(json[JSON::Keys::slots][4].is_null());
    }
}