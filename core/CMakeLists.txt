cmake_minimum_required(VERSION 3.14)
project(StrategrCore)

set(CMAKE_CXX_STANDARD 17)

# Configure Files

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(StrategrCoreConfigure)

# Compile options

if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

# Dependencies

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.71 COMPONENTS filesystem REQUIRED)
find_package(Catch2 2.9 REQUIRED)
find_package(utf8proc 2.4 REQUIRED)
find_package(nlohmann_json 3.2 REQUIRED)

# Core Library

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(libstrategr STATIC
        ${STRATEGR_CORE_VERSION_FILE}
        include/strategr/version.h
        src/activity.cpp
        include/strategr/activity.h
        src/strategy.cpp
        include/strategr/strategy.h
        src/activityinvalidpropertyexception.cpp
        include/strategr/activityinvalidpropertyexception.h
        src/activitylist.cpp
        include/strategr/activitylist.h
        include/strategr/privatelist.h
        include/strategr/notifiableonchange.h
        src/timeslot.cpp
        include/strategr/timeslot.h
        src/timeslotsstate.cpp
        include/strategr/timeslotsstate.h
        src/sessionslist.cpp
        include/strategr/sessionslist.h
        include/strategr/streamablelist.h
        src/strategyhistory.cpp
        include/strategr/strategyhistory.h
        src/session.cpp
        src/dragoperation.cpp
        include/strategr/dragoperation.h
        src/resizeoperation.cpp
        include/strategr/resizeoperation.h
        src/overview.cpp
        include/strategr/overview.h
        src/time_utils.cpp
        include/strategr/time_utils.h
        src/json.cpp
        src/json.h
        src/currenttimemarker.cpp
        include/strategr/currenttimemarker.h
        include/strategr/geometry.h
        src/notifier.cpp
        include/strategr/notifier.h
        include/strategr/utility.h
        src/mousehandler.cpp
        include/strategr/mousehandler.h
        include/strategr/mousehandleroperations.h
        src/selection.cpp
        include/strategr/selection.h
        src/event.cpp
        include/strategr/event.h
        src/color.cpp
        include/strategr/color.h
        src/theme.cpp
        include/strategr/theme.h
        src/timer.cpp
        include/strategr/timer.h
        include/strategr/action.hpp
        src/actioncenter.cpp
        include/strategr/actioncenter.h
        src/stgstring.cpp
        include/strategr/stgstring.h
        src/persistent.cpp
        include/strategr/persistent.h
        src/notifications.cpp
        include/strategr/notifications.h
        src/file_bookmark.cpp
        include/strategr/file_bookmark.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

set_target_properties(libstrategr PROPERTIES OUTPUT_NAME strategr)

target_include_directories(libstrategr
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/strategr
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(libstrategr
        PUBLIC
        nlohmann_json::nlohmann_json

        PRIVATE
        Boost::headers
        utf8proc::utf8proc)

if (APPLE)
    target_compile_options(libstrategr PRIVATE -fobjc-arc)

    target_sources(libstrategr
            PRIVATE
            src/apple/STGCalendarManager.m
            include/strategr/apple/STGCalendarManager.h
            include/strategr/apple/STGCalendarExporter.h
            src/apple/STGCalendarExporter.mm
            src/apple/STGCalendarImporter.mm
            include/strategr/apple/STGCalendarImporter.h
            include/strategr/apple/STGCalendarImportViewModel.h
            src/apple/STGCalendarImportViewModel.m
            include/strategr/apple/STGCalendarExportViewModel.h
            src/apple/STGCalendarExportViewModel.m
            src/apple/color.mm
            src/apple/stgstring.mm
            src/apple/file_bookmark.mm)

    target_include_directories(libstrategr
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include/strategr/apple)

    target_link_libraries(libstrategr
            PRIVATE
            "-framework Foundation"
            "-framework QuartzCore"
            "-framework EventKit")
endif ()

# Tests

add_executable(test
        tests/tests_main.cpp
        tests/strategy_settings_test.cpp
        tests/strategy_timeslots_test.cpp
        tests/strategy_activities_slots_interaction_test.cpp
        tests/strategy_history_test.cpp
        tests/json_tests.cpp
        tests/persistent_test.cpp
        tests/time_utils_test.cpp
        tests/notifier_immeadiate_test.cpp
        tests/notifier_scheduled_test.cpp)

target_include_directories(test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include/strategr)

target_link_libraries(test
        PRIVATE
        libstrategr
        Catch2::Catch2
        Boost::filesystem)