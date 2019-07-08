cmake_minimum_required(VERSION 3.14)
project(models_tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(models)
include_directories(ui)
include_directories(utility)

if (APPLE)
    set(Qt5_DIR ~/Qt/5.13.0/clang_64/lib/cmake/Qt5)
endif ()

find_package(catch2 REQUIRED)
find_package(Qt5 COMPONENTS Widgets Test REQUIRED)

set(MODELS
        models/activity.cpp
        models/activity.h
        models/strategy.cpp
        models/strategy.h
        models/activityinvalidpropertyexception.cpp
        models/activityinvalidpropertyexception.h
        models/activitylist.cpp
        models/activitylist.h
        models/privatelist.hpp
        models/notifiableonchange.h
        models/timeslot.cpp
        models/timeslot.h
        models/timeslotsstate.cpp
        models/timeslotsstate.h
        models/activitysessionslist.cpp
        models/activitysessionslist.h
        models/streamablelist.h
        models/activitysessionscalculator.cpp
        models/activitysessionscalculator.h
        models/strategyhistory.cpp
        models/strategyhistory.h)

set(MODELS_TESTS
        models/tests/activity_tests.cpp
        models/tests/strategy_activities_test.cpp
        models/tests/strategy_settings_test.cpp
        models/tests/strategy_timeslots_test.cpp
        models/tests/strategy_activities_slots_interaction_test.cpp
        models/tests/strategy_history_test.cpp)

set(UI
        ui/mainwindow.cpp
        ui/mainwindow.h)

set(UI_TESTS
        ui/tests/main_window_test.cpp)

qt5_add_resources(RESOURCES
        resources/icons.qrc
        resources/fonts.qrc)

add_executable(models_tests
        tests_main.cpp
        ${MODELS}
        ${MODELS_TESTS})

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

add_executable(strategr
        main.cpp
        ${UI}
        ${RESOURCES})

add_executable(ui_tests
        qt_tests_main.cpp
        ${UI}
        ${UI_TESTS}
        ${RESOURCES})

target_link_libraries(models_tests Catch2::Catch2)
target_link_libraries(ui_tests Catch2::Catch2 Qt5::Widgets Qt5::Test)

target_link_libraries(strategr Qt5::Widgets)
