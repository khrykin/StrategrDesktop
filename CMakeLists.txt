cmake_minimum_required(VERSION 3.14)
project(Strategr)

set(CMAKE_CXX_STANDARD 17)

find_package(catch2 REQUIRED)
find_package(Qt5 COMPONENTS Widgets Test MacExtras REQUIRED)

include_directories(.)
include_directories(models)
include_directories(ui)
include_directories(utility)

set(CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
        ${CMAKE_MODULE_PATH})

include(GenerateVersionFile)

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
        models/strategyhistory.h
        models/currentactivitysession.cpp
        models/currentactivitysession.h
        models/activitysession.cpp
        models/dragoperation.cpp
        models/dragoperation.h
        models/resizeoperation.cpp
        models/resizeoperation.h)

set(MODELS_TESTS
        models/tests/activity_tests.cpp
        models/tests/strategy_activities_test.cpp
        models/tests/strategy_settings_test.cpp
        models/tests/strategy_timeslots_test.cpp
        models/tests/strategy_activities_slots_interaction_test.cpp
        models/tests/strategy_history_test.cpp)

set(UI
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/windowgeometrymanager.cpp
        ui/windowgeometrymanager.h
        ui/applicationmenu.cpp
        ui/applicationmenu.h
        ui/mainscene.cpp
        ui/mainscene.cpp
        ui/mainscene.h
        third-party/slidingstackedwidget.cpp
        third-party/slidingstackedwidget.h
        third-party/stacklayout.cpp
        third-party/stacklayout.h
        ui/sessionsmainwidget.cpp
        ui/sessionsmainwidget.h
        ui/activitylistwidget.cpp
        ui/activitylistwidget.h
        ui/activitylistitemwidget.cpp
        ui/activitylistitemwidget.h
        ui/strategysettingswidget.cpp
        ui/strategysettingswidget.h
        ui/slotboard.cpp
        ui/slotboard.h
        ui/slidinganimator.cpp
        ui/slidinganimator.h
        ui/slotswidget.cpp
        ui/slotboard.h
        ui/slotruler.cpp
        ui/slotruler.h
        ui/activitysessionwidget.cpp
        ui/activitysessionwidget.h
        ui/slotsmousehandler.cpp
        ui/slotsmousehandler.h
        ui/selectionwidget.cpp
        ui/selectionwidget.h
        ui/navbar.cpp
        ui/navbar.h
        ui/colorpicker.cpp
        ui/colorpicker.h
        ui/colorpickeritem.cpp
        ui/colorpicker.h
        ui/enterpresseater.cpp
        ui/mainscene.h
        ui/activityeditormenu.cpp
        ui/activityeditormenu.h
        ui/currenttimemarker.cpp
        ui/currenttimemarker.h
        ui/parentwindowaccessible.cpp
        ui/parentwindowaccessible.h
        ui/reactivelist.hpp
        ui/currentsessionwidget.cpp
        ui/currentsessionwidget.h
        ui/abstractspinboxdecorator.cpp
        ui/abstractspinboxdecorator.h
        ui/timelabel.h
        ui/alert.cpp
        ui/alert.h
        ui/application.cpp
        ui/application.h
        ui/rowwidget.cpp
        ui/rowwidget.h
        ui/coloredlabel.cpp
        ui/coloredlabel.h
        ui/colorprovider.cpp
        ui/colorprovider.h
        ui/macoswindow.h
        ui/cursorprovider.cpp
        ui/cursorprovider.h
        ui/separator.cpp ui/separator.h
        ui/visualeffectwidget.mm
        ui/visualeffectwidget.h
        ui/aboutwindow.cpp
        ui/aboutwindow.h
        ui/applicationicon.cpp
        ui/applicationicon.h)

set(UTILITY
        utility/jsonserializer.cpp
        utility/jsonserializer.h
        utility/filesystemiomanager.cpp
        utility/filesystemiomanager.h
        utility/applicationsettings.h
        utility/utils.cpp
        utility/utils.h
        utility/notifier.cpp
        utility/notifier.h
        utility/notifierbackend.h
        utility/fontutils.cpp
        utility/fontutils.h)

set(UI_TESTS
        ui/tests/main_window_test.cpp
        utility/testutils.h)

qt5_add_resources(RESOURCES
        resources/icons.qrc
        resources/fonts.qrc)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)

    set(PLATFORM_LIBRARIES
            "-framework Foundation"
            "-framework AppKit"
            "-framework UserNotifications"
            Qt5::MacExtras)

    set(PLATFORM_FILES
            ui/alert.mm
            ui/macoswindow.mm
            ui/applicationicon.mm
            utility/notifierbackend.mm
            utility/cocoautils.h
            utility/cocoautils.mm
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.m
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.m
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.h)

    list(APPEND RESOURCES
            deployment/Strategr.icns
            deployment/Strategy.icns)

    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)
endif ()

add_executable(models_tests
        tests_main.cpp
        ${MODELS}
        ${MODELS_TESTS})

add_library(StrategrCore SHARED ${MODELS})
set_target_properties(StrategrCore PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER models/strategy.h
        VERSION ${VERSION_SHORT}
        MACOSX_FRAMEWORK_LONG_VERSION_STRING ${VERSION}
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION_SHORT}
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION}
        XCODE_ATTRIBUTE_INSTALL_PATH @executable_path/../Frameworks/)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

add_executable(Strategr
        MACOSX_BUNDLE
        main.cpp
        ${PLATFORM_FILES}
        ${UI}
        ${UTILITY}
        ${RESOURCES}
        ${version_file})

add_executable(ui_tests
        qt_tests_main.cpp
        ${PLATFORM_FILES}
        ${UI}
        ${UTILITY}
        ${UI_TESTS}
        ${RESOURCES})

target_link_libraries(models_tests Catch2::Catch2)
target_link_libraries(ui_tests
        StrategrCore
        Catch2::Catch2
        Qt5::Widgets
        Qt5::Test)

target_link_libraries(Strategr
        StrategrCore
        Qt5::Widgets
        ${PLATFORM_LIBRARIES})

if (APPLE)
    if (CMAKE_BUILD_TYPE MATCHES Release)
        add_custom_command(TARGET Strategr
                POST_BUILD
                COMMAND ${CMAKE_SOURCE_DIR}/scripts/macos_deploy.sh ${CMAKE_CURRENT_BINARY_DIR})
    endif (CMAKE_BUILD_TYPE MATCHES Release)
endif ()

# Set a custom plist file for the app bundle
set_target_properties(Strategr PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/deployment/Info.plist
        MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_SHORT}
        MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_SHORT}
        RESOURCE "${RESOURCES}")


