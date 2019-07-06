cmake_minimum_required(VERSION 3.14)
project(models_tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(tests)

find_package(catch2 REQUIRED)

add_executable(models_tests
        activity.cpp
        activity.h
        #        activitygroup.cpp
        #        activitygroup.h
        strategy.cpp
        strategy.h
        tests/tests_main.cpp
        tests/activity_tests.cpp
        activityinvalidpropertyexception.cpp
        activityinvalidpropertyexception.h
        tests/strategy_activities_tests.cpp
        activitylist.cpp
        activitylist.h
        privatelist.hpp
        notifiableonchange.h
        timeslot.cpp
        timeslot.h
        timeslotsstate.cpp
        timeslotsstate.h
        tests/strategy_timeslots_test.cpp
        activitysessionslist.cpp
        activitysessionslist.h
        streamablelist.h
        activitysessionscalculator.cpp
        activitysessionscalculator.h
        tests/strategy_settings_test.cpp
        tests/strategy_activities_slots_interaction_test.cpp
        strategyhistory.cpp strategyhistory.h
        tests/strategy_history_test.cpp)

target_link_libraries(models_tests Catch2::Catch2)
