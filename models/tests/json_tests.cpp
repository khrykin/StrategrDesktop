//
// Created by Dmitry Khrykin on 2019-11-23.
//

#include <catch2/catch.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

#include "strategy.h"
#include "json.h"

std::string readTestFile(const std::string &filename) {
    using namespace boost::filesystem;
    const auto testFilePath = path{__FILE__}.parent_path() / path{filename};

    std::ifstream t(testFilePath.string());

    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
}

void testStrategyFile(const std::string &filename) {
    auto strategy = stg::strategy::from_json_string(readTestFile(filename));

    REQUIRE(strategy->time_slot_duration() == 10);
    REQUIRE(strategy->begin_time() == 370);
    REQUIRE(strategy->number_of_time_slots() == 10);
    REQUIRE(strategy->time_slots()[0].begin_time == 370);
    REQUIRE(strategy->time_slots()[0].duration == 10);
    REQUIRE(strategy->time_slots()[0].activity == stg::strategy::no_activity);
    REQUIRE(strategy->time_slots()[2].activity == strategy->activities().at(0));

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
        auto strategy = stg::strategy();
        strategy.set_begin_time(100);
        strategy.set_time_slot_duration(10);

        strategy.add_activity(stg::activity("Some 1", "#ff0000"));
        strategy.add_activity(stg::activity("Some 2", "#00ff00"));

        strategy.set_number_of_time_slots(5);

        strategy.place_activity(0, {0, 1});
        strategy.place_activity(1, {2, 3});

        auto json_string = strategy.to_json_string();
        auto json = nlohmann::json::parse(json_string);

        REQUIRE(json[stg::json::keys::start_time] == 100);
        REQUIRE(json[stg::json::keys::slot_duration] == 10);

        REQUIRE(json[stg::json::keys::activities].is_array());
        REQUIRE(json[stg::json::keys::slots].is_array());

        REQUIRE(json[stg::json::keys::slots].size() == 5);

        REQUIRE(json[stg::json::keys::activities][0]["name"] == "Some 1");
        REQUIRE(json[stg::json::keys::activities][0]["color"] == "#ff0000");

        REQUIRE(json[stg::json::keys::activities][1]["name"] == "Some 2");
        REQUIRE(json[stg::json::keys::activities][1]["color"] == "#00ff00");

        REQUIRE(json[stg::json::keys::slots][0] == 0);
        REQUIRE(json[stg::json::keys::slots][1] == 0);

        REQUIRE(json[stg::json::keys::slots][2] == 1);
        REQUIRE(json[stg::json::keys::slots][3] == 1);

        REQUIRE(json[stg::json::keys::slots][4].is_null());
    }
}