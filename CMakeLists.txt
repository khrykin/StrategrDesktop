cmake_minimum_required(VERSION 3.14)
project(Strategr)

set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS filesystem REQUIRED)
# find_package(catch2 REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Qt5 COMPONENTS Widgets Test REQUIRED)

find_library(utf8Proc_LIBRARY_PATH libutf8proc.a utf8proc.lib utf8proc)

if (APPLE)
    find_package(Qt5 COMPONENTS MacExtras REQUIRED)
    find_library(Sparkle_FRAMEWORK_PATH Sparkle)

    add_compile_options("-fobjc-arc")
endif ()

if (WIN32)
    find_package(Qt5 COMPONENTS WinExtras REQUIRED)
    find_library(WinSparkle_PATH WinSparkle)
endif ()

if (UNIX)
    add_compile_options("-fpermissive")
endif ()

include_directories(.)
include_directories(models)
include_directories(models/apple)

include_directories(ui)
include_directories(utility)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
        ${CMAKE_MODULE_PATH})

include(GenerateVersionFile)

if (APPLE)
    set(MODELS_PLATFORM_FILES
            models/apple/NSColor+HexString.h
            models/apple/NSColor+HexString.m
            models/apple/SGCalendarManager.m
            models/apple/SGCalendarManager.h
            models/apple/SGCalendarExporter.h
            models/apple/SGCalendarExporter.mm
            models/color.mm)
endif ()

set(MODELS ${MODELS_PLATFORM_FILES}
        models/activity.cpp
        models/activity.h
        models/strategy.cpp
        models/strategy.h
        models/activityinvalidpropertyexception.cpp
        models/activityinvalidpropertyexception.h
        models/activitylist.cpp
        models/activitylist.h
        models/privatelist.h
        models/notifiableonchange.h
        models/timeslot.cpp
        models/timeslot.h
        models/timeslotsstate.cpp
        models/timeslotsstate.h
        models/sessionslist.cpp
        models/sessionslist.h
        models/streamablelist.h
        models/sessionscalculator.cpp
        models/sessionscalculator.h
        models/strategyhistory.cpp
        models/strategyhistory.h
        models/session.cpp
        models/dragoperation.cpp
        models/dragoperation.h
        models/resizeoperation.cpp
        models/resizeoperation.h
        models/overview.cpp
        models/overview.h
        models/time_utils.cpp
        models/time_utils.h
        models/json.cpp
        models/json.h
        models/currenttimemarker.cpp
        models/currenttimemarker.h
        models/geometry.h
        models/notifier.cpp
        models/notifier.h
        models/utility.h
        models/mousehandler.cpp
        models/mousehandler.h
        models/mousehandleroperations.h
        models/selection.cpp
        models/selection.h
        models/event.cpp
        models/event.h
        models/color.cpp
        models/color.h
        models/theme.cpp
        models/theme.h)

set(MODELS_TESTS
        models/tests/activity_tests.cpp
        models/tests/strategy_activities_test.cpp
        models/tests/strategy_settings_test.cpp
        models/tests/strategy_timeslots_test.cpp
        models/tests/strategy_activities_slots_interaction_test.cpp
        models/tests/strategy_history_test.cpp
        models/tests/json_tests.cpp)

set(MODELS_LIBRARIES ${utf8Proc_LIBRARY_PATH})

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
        ui/activitywidget.cpp
        ui/activitywidget.h
        ui/strategysettingswidget.cpp
        ui/strategysettingswidget.h
        ui/slotboardwidget.cpp
        ui/slotboardwidget.h
        ui/slidinganimator.cpp
        ui/slidinganimator.h
        ui/slotswidget.cpp
        ui/slotboardwidget.h
        ui/slotruler.cpp
        ui/slotruler.h
        ui/sessionwidget.cpp
        ui/sessionwidget.h
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
        ui/currenttimemarkerwidget.cpp
        ui/currenttimemarkerwidget.h
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
        ui/separator.cpp
        ui/separator.h
        ui/aboutwindow.cpp
        ui/aboutwindow.h
        ui/applicationicon.cpp
        ui/applicationicon.h
        ui/overviewwidget.cpp
        ui/overviewwidget.h
        ui/searchboxwidget.cpp
        ui/searchboxwidget.h
        ui/dynamicpalette.cpp
        ui/dynamicpalette.h
        ui/slotboardcircleswidget.cpp
        ui/slotboardcircleswidget.h
        ui/iconwidget.cpp
        ui/iconwidget.h)

set(UTILITY
        ${version_file}
        utility/filesystemiomanager.cpp
        utility/filesystemiomanager.h
        utility/applicationsettings.h
        utility/utils.cpp
        utility/utils.h
        utility/notifierbackend.h
        utility/notifierbackend.cpp
        utility/fontutils.cpp
        utility/fontutils.h)

set(UI_TESTS
        ui/tests/main_window_test.cpp
        utility/testutils.h)

qt5_add_resources(RESOURCES
        resources/fonts.qrc)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
    set(MODELS_PLATFORM_LIBRARIES
            "-framework Foundation"
            "-framework AppKit"
            "-framework EventKit")

    set(PLATFORM_LIBRARIES
            "-framework Foundation"
            "-framework AppKit"
            "-framework UserNotifications"
            ${Sparkle_FRAMEWORK_PATH}
            Qt5::MacExtras)

    set(PLATFORM_FILES
            ui/application.mm
            ui/alert.mm
            ui/macoswindow.mm
            ui/applicationicon.mm
            ui/cocoa/SGCalendarExportOptionsView.h
            ui/cocoa/SGCalendarExportOptionsView.m
            ui/cocoa/SGProgressWindow.h
            ui/cocoa/SGProgressWindow.m
            utility/notifierbackend.mm
            utility/cocoautils.h
            utility/cocoautils.mm
            utility/macoscalendarexporter.mm
            utility/macoscalendarexporter.h
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.m
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.m
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.h
            ui/cocoa/SGOptionsViewController.m
            ui/cocoa/SGOptionsViewController.h
            ui/cocoa/SGCalendarExportProgressWindow.h
            ui/cocoa/SGCalendarExportProgressWindow.m ui/colorprovider.mm)

    set(PLATFORM_RESOURCES
            deployment/Strategr.icns
            deployment/Strategy.icns)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)

    set_source_files_properties(ui/application.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)
    set_source_files_properties(ui/macoswindow.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)

endif ()

if (WIN32)
    set(PLATFORM_RESOURCES
            ${CMAKE_CURRENT_BINARY_DIR}/Strategr.rc)

    set(PLATFORM_LIBRARIES
            ${WinSparkle_PATH}
            Qt5::WinExtras)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr)
endif ()

add_executable(models_tests
        tests_main.cpp
        ${MODELS}
        ${MODELS_TESTS})

target_link_libraries(models_tests
        Catch2::Catch2
        Boost::filesystem
        ${MODELS_LIBRARIES}
        ${MODELS_PLATFORM_LIBRARIES})

option(COVERAGE "Generate code coverage" OFF)

if (COVERAGE)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES Clang)
        target_compile_options(models_tests PUBLIC -fprofile-instr-generate -fcoverage-mapping)
        target_link_options(models_tests PUBLIC -fprofile-instr-generate)
        #Uncomment in case of linker errors
        #    target_link_libraries(models_tests clang_rt.profile-x86_64)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU)
        target_compile_options(models_tests PUBLIC --coverage)
        #Uncomment in case of linker errors
        #target_link_libraries(models_tests gcov)
    endif ()
endif ()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(StrategrCore SHARED ${MODELS})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

target_compile_definitions(StrategrCore PUBLIC UTF8PROC_STATIC)

target_link_libraries(StrategrCore
        ${MODELS_LIBRARIES}
        ${MODELS_PLATFORM_LIBRARIES})

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
        ${PLATFORM_RESOURCES})

add_executable(ui_tests
        qt_tests_main.cpp
        ${PLATFORM_FILES}
        ${UI}
        ${UTILITY}
        ${UI_TESTS}
        ${RESOURCES})

target_link_libraries(ui_tests
        StrategrCore
        Catch2::Catch2
        Qt5::Widgets
        Qt5::Test
        ${PLATFORM_LIBRARIES})

target_link_libraries(Strategr
        StrategrCore
        Qt5::Widgets
        ${PLATFORM_LIBRARIES})

if (APPLE)
    if (CMAKE_BUILD_TYPE MATCHES Rel)
        file(COPY ${Sparkle_FRAMEWORK_PATH} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)

        add_custom_command(TARGET Strategr
                POST_BUILD
                COMMAND ${CMAKE_SOURCE_DIR}/scripts/macos_deploy.sh ${CMAKE_CURRENT_BINARY_DIR} ${VERSION})
    endif (CMAKE_BUILD_TYPE MATCHES Rel)
endif ()

if (WIN32)
    # Configure resource file

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr.rc"
            "${CMAKE_CURRENT_BINARY_DIR}/Strategr.rc")

    # Copy icons

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr.ico"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategy.ico"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

    # Copy non-Qt dlls

    string(REGEX REPLACE "\\.lib" ".dll" WinSparkle_DLL "${WinSparkle_PATH}")
    file(COPY "${WinSparkle_DLL}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    if (CMAKE_BUILD_TYPE MATCHES Rel)
        set_target_properties(Strategr PROPERTIES WIN32_EXECUTABLE ON)

        # Configure installer scripts

        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.template.iss"
                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.iss")

        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.template.iss"
                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.iss")

        # Deploy Qt

        add_custom_command(TARGET Strategr
                POST_BUILD
                COMMAND ${Qt5_DIR}/../../../bin/windeployqt.exe --no-translations ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    endif (CMAKE_BUILD_TYPE MATCHES Rel)
endif ()

# Set a custom plist file for the app bundle
set_target_properties(Strategr PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/deployment/Info.plist
        MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
        MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_SHORT}
        RESOURCE "${PLATFORM_RESOURCES}")


