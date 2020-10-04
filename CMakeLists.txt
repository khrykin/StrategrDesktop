cmake_minimum_required(VERSION 3.14)
project(Strategr)

set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS filesystem REQUIRED)
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

include_directories(.)
include_directories(core)
include_directories(core/apple)

include_directories(ui)
include_directories(utility)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
        ${CMAKE_MODULE_PATH})

include(GenerateVersionFile)

if (APPLE)
    set(CORE_PLATFORM_FILES
            core/apple/STGCalendarManager.m
            core/apple/STGCalendarManager.h
            core/apple/STGCalendarExporter.h
            core/apple/STGCalendarExporter.mm
            core/apple/STGCalendarImporter.mm
            core/apple/STGCalendarImporter.h
            core/apple/STGCalendarImportViewModel.h
            core/apple/STGCalendarImportViewModel.m
            core/apple/STGCalendarExportViewModel.h
            core/apple/STGCalendarExportViewModel.m
            core/apple/color.mm
            core/apple/stgstring.mm
            core/apple/file_bookmark.mm)
endif ()

set(CORE ${CORE_PLATFORM_FILES}
        core/version.h
        ${CORE_VERSION_FILE}
        core/activity.cpp
        core/activity.h
        core/strategy.cpp
        core/strategy.h
        core/activityinvalidpropertyexception.cpp
        core/activityinvalidpropertyexception.h
        core/activitylist.cpp
        core/activitylist.h
        core/privatelist.h
        core/notifiableonchange.h
        core/timeslot.cpp
        core/timeslot.h
        core/timeslotsstate.cpp
        core/timeslotsstate.h
        core/sessionslist.cpp
        core/sessionslist.h
        core/streamablelist.h
        core/strategyhistory.cpp
        core/strategyhistory.h
        core/session.cpp
        core/dragoperation.cpp
        core/dragoperation.h
        core/resizeoperation.cpp
        core/resizeoperation.h
        core/overview.cpp
        core/overview.h
        core/time_utils.cpp
        core/time_utils.h
        core/json.cpp
        core/json.h
        core/currenttimemarker.cpp
        core/currenttimemarker.h
        core/geometry.h
        core/notifier.cpp
        core/notifier.h
        core/utility.h
        core/mousehandler.cpp
        core/mousehandler.h
        core/mousehandleroperations.h
        core/selection.cpp
        core/selection.h
        core/event.cpp
        core/event.h
        core/color.cpp
        core/color.h
        core/theme.cpp
        core/theme.h
        core/timer.cpp
        core/timer.h
        core/action.hpp
        core/actioncenter.cpp
        core/actioncenter.h
        core/stgstring.cpp
        core/stgstring.h
        core/persistent.cpp
        core/persistent.h
        core/notifications.cpp
        core/notifications.h
        core/file_bookmark.cpp
        core/file_bookmark.h)

set(CORE_TESTS
        core/tests/strategy_settings_test.cpp
        core/tests/strategy_timeslots_test.cpp
        core/tests/strategy_activities_slots_interaction_test.cpp
        core/tests/strategy_history_test.cpp
        core/tests/json_tests.cpp
        core/tests/persistent_test.cpp
        core/tests/time_utils_test.cpp
        core/tests/notifier_immeadiate_test.cpp
        core/tests/notifier_scheduled_test.cpp)

set(CORE_LIBRARIES ${utf8Proc_LIBRARY_PATH})

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
        ui/iconwidget.cpp
        ui/iconwidget.h
        ui/slotboardscrollarea.cpp
        ui/slotboardscrollarea.h
        ui/dataproviderwidget.cpp
        ui/dataproviderwidget.h
        ui/drawingutils.h)

set(UTILITY
        ${VERSION_FILE}
        utility/filesystemiomanager.cpp
        utility/filesystemiomanager.h
        utility/applicationsettings.h
        utility/utils.cpp
        utility/utils.h
        utility/notifierbackend.h
        utility/notifierbackend.cpp
        utility/fontutils.cpp
        utility/fontutils.h
        utility/backends.h)

set(UI_TESTS
        ui/tests/main_window_test.cpp
        utility/testutils.h)

qt5_add_resources(RESOURCES
        resources/fonts.qrc)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
    set(CORE_PLATFORM_LIBRARIES
            "-framework Foundation"
            "-framework AppKit"
            "-framework EventKit")

    set(PLATFORM_LIBRARIES
            "-framework Foundation"
            "-framework AppKit"
            "-framework UserNotifications"
            "-framework CoreImage"
            "-framework QuartzCore"
            ${Sparkle_FRAMEWORK_PATH}
            Qt5::MacExtras)

    set(PLATFORM_FILES
            ui/application.mm
            ui/alert.mm
            ui/macoswindow.mm
            ui/applicationicon.mm
            utility/notifierbackend.mm
            utility/cocoautils.h
            utility/cocoautils.mm
            utility/macoscalendarexporter.mm
            utility/macoscalendarexporter.h
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.m
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.m
            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.h
            ui/cocoa/STGCalendarExportViewController.m
            ui/cocoa/STGCalendarExportViewController.h
            ui/cocoa/STGCalendarExportProgressWindow.h
            ui/cocoa/STGCalendarExportProgressWindow.m
            ui/cocoa/STGCalendarExportView.h
            ui/cocoa/STGCalendarExportView.m
            ui/cocoa/STGProgressWindow.h
            ui/cocoa/STGProgressWindow.m
            ui/cocoa/STGCalendarImportExportView.h
            ui/cocoa/STGCalendarImportExportViewSubclass.h
            ui/cocoa/STGCalendarImportExportView.m
            ui/cocoa/STGCalendarImportView.h
            ui/cocoa/STGCalendarImportView.m
            ui/cocoa/STGColoredCheckBoxView.h
            ui/cocoa/STGColoredCheckBoxView.m
            ui/colorprovider.mm
            ui/cocoa/STGCalendarImportViewController.m
            ui/cocoa/STGCalendarImportViewController.h
            ui/cocoa/STGToolbar.mm
            ui/cocoa/STGToolbar.h)

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

add_executable(core_tests
        tests_main.cpp
        ${CORE}
        ${CORE_TESTS})

target_link_libraries(core_tests
        Catch2::Catch2
        Boost::filesystem
        ${CORE_LIBRARIES}
        ${CORE_PLATFORM_LIBRARIES})

option(COVERAGE "Generate code coverage" OFF)

if (COVERAGE)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES Clang)
        target_compile_options(core_tests PUBLIC -fprofile-instr-generate -fcoverage-mapping)
        target_link_options(core_tests PUBLIC -fprofile-instr-generate)
        #Uncomment in case of linker errors
        #    target_link_libraries(core_tests clang_rt.profile-x86_64)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU)
        target_compile_options(core_tests PUBLIC --coverage)
        #Uncomment in case of linker errors
        #target_link_libraries(core_tests gcov)
    endif ()
endif ()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(StrategrCore SHARED ${CORE})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

target_compile_definitions(StrategrCore PUBLIC UTF8PROC_STATIC)

target_link_libraries(StrategrCore
        ${CORE_LIBRARIES}
        ${CORE_PLATFORM_LIBRARIES})

set_target_properties(StrategrCore PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER core/strategy.h
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

target_compile_definitions(ui_tests PRIVATE STG_UI_TEST)

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


