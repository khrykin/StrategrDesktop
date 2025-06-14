cmake_minimum_required(VERSION 3.14)

# Setup modules path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(GetVersionFromGit)

project(StrategrCore VERSION ${VERSION_SHORT})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)

# Configure Files
include(StrategrCoreConfigure)

# Compile options
if (MSVC)
    add_compile_options(/W4 /WX /wd4068 /wd26812)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unknown-pragmas)
endif ()

# Dependencies
find_package(Boost REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-utf8proc CONFIG REQUIRED)

# Core Library
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
        include/strategr/file_bookmark.h
      )

set_target_properties(libstrategr
        PROPERTIES
        OUTPUT_NAME strategr)

target_include_directories(libstrategr
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/strategr
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(libstrategr
        PRIVATE
        utf8proc
        nlohmann_json::nlohmann_json
        Boost::headers
      )

add_executable(tests
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

target_include_directories(tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include/strategr)

target_link_libraries(tests
        PRIVATE
        nlohmann_json::nlohmann_json
        libstrategr
        Catch2::Catch2
        Catch2::Catch2WithMain
      )

if (APPLE)
    target_compile_options(libstrategr PRIVATE -fobjc-arc)

    # Enable std::filesystem for tests
    target_compile_options(tests PRIVATE -mmacosx-version-min=10.15)
    target_link_options(tests PRIVATE -mmacosx-version-min=10.15)

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

if (WIN32)
    target_link_libraries(libstrategr
            PRIVATE
            bcrypt)
endif ()

# Tests
enable_testing()
include(Catch)
catch_discover_tests(tests)

