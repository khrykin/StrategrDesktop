cmake_minimum_required(VERSION 3.14)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "Minimum macOS deployment version" FORCE)

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/cmake/modules
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# Get version from git tag
include(GetVersionFromGit)

project(StrategrDesktop VERSION ${VERSION_SHORT})

set(CMAKE_CXX_STANDARD 17)

# Include core library

add_subdirectory(../core ${CMAKE_CURRENT_BINARY_DIR}/core)

# Configure files

include(ConfigureDesktop)

# Compile options

if (MSVC)
    add_compile_options(/W4 /WX /wd4068)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unknown-pragmas)
endif ()

# Dependencies

find_package(Qt5 5.14 COMPONENTS Widgets Test REQUIRED)
find_package(Catch2 2.9 REQUIRED)

if (APPLE)
    find_package(Qt5 COMPONENTS MacExtras REQUIRED)
    find_package(Sparkle 1.22 REQUIRED)
endif ()

if (WIN32)
    find_package(Qt5 COMPONENTS WinExtras REQUIRED)
    find_package(WinSparkle REQUIRED)
endif ()

# Resources

qt5_add_resources(QT_RESOURCES resources/fonts.qrc)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

set(DESKTOP_SOURCES
        ${DESKTOP_VERSION_FILE}
        src/mainwindow.cpp
        src/mainwindow.h
        src/windowgeometrymanager.cpp
        src/windowgeometrymanager.h
        src/applicationmenu.cpp
        src/applicationmenu.h
        src/mainscene.cpp
        src/mainscene.cpp
        src/mainscene.h
        src/third-party/slidingstackedwidget.cpp
        src/third-party/slidingstackedwidget.h
        src/sessionsmainwidget.cpp
        src/sessionsmainwidget.h
        src/activitylistwidget.cpp
        src/activitylistwidget.h
        src/activitywidget.cpp
        src/activitywidget.h
        src/strategysettingswidget.cpp
        src/strategysettingswidget.h
        src/slotboardwidget.cpp
        src/slotboardwidget.h
        src/slidinganimator.cpp
        src/slidinganimator.h
        src/slotswidget.cpp
        src/slotboardwidget.h
        src/slotruler.cpp
        src/slotruler.h
        src/sessionwidget.cpp
        src/sessionwidget.h
        src/selectionwidget.cpp
        src/selectionwidget.h
        src/navbar.cpp
        src/navbar.h
        src/colorpicker.cpp
        src/colorpicker.h
        src/colorpickeritem.cpp
        src/colorpicker.h
        src/enterpresseater.cpp
        src/mainscene.h
        src/activityeditormenu.cpp
        src/activityeditormenu.h
        src/currenttimemarkerwidget.cpp
        src/currenttimemarkerwidget.h
        src/reactivelist.hpp
        src/currentsessionwidget.cpp
        src/currentsessionwidget.h
        src/abstractspinboxdecorator.cpp
        src/abstractspinboxdecorator.h
        src/alert.cpp
        src/alert.h
        src/application.cpp
        src/application.h
        src/rowwidget.cpp
        src/rowwidget.h
        src/coloredlabel.cpp
        src/coloredlabel.h
        src/colorprovider.cpp
        src/colorprovider.h
        src/macoswindow.h
        src/cursorprovider.cpp
        src/cursorprovider.h
        src/separator.cpp
        src/separator.h
        src/aboutwindow.cpp
        src/aboutwindow.h
        src/applicationicon.cpp
        src/applicationicon.h
        src/overviewwidget.cpp
        src/overviewwidget.h
        src/searchboxwidget.cpp
        src/searchboxwidget.h
        src/dynamicpalette.cpp
        src/dynamicpalette.h
        src/iconwidget.cpp
        src/iconwidget.h
        src/slotboardscrollarea.cpp
        src/slotboardscrollarea.h
        src/dataproviderwidget.cpp
        src/dataproviderwidget.h
        src/drawingutils.h
        src/filesystemiomanager.cpp
        src/filesystemiomanager.h
        src/applicationsettings.h
        src/utils.cpp
        src/utils.h
        src/notifierbackend.h
        src/notifierbackend.cpp
        src/fontutils.cpp
        src/fontutils.h
        src/backends.h)

# MacOS Specifics

if (APPLE)
    set(PLATFORM_RESOURCES
            deployment/Strategr.icns
            deployment/Strategy.icns)

    set(PLATFORM_LIBRARIES
            "-framework AppKit"
            "-framework UserNotifications"
            Sparkle::Sparkle
            Qt5::MacExtras)

    list(APPEND DESKTOP_SOURCES
            ${PLATFORM_RESOURCES}
            src/application.mm
            src/alert.mm
            src/macoswindow.mm
            src/applicationicon.mm
            src/notifierbackend.mm
            src/cocoautils.h
            src/cocoautils.mm
            src/colorprovider.mm
            src/macoscalendarexporter.mm
            src/macoscalendarexporter.h
            src/third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.m
            src/third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h
            src/third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.m
            src/third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.h
            src/cocoa/STGCalendarExportViewController.m
            src/cocoa/STGCalendarExportViewController.h
            src/cocoa/STGCalendarExportProgressWindow.h
            src/cocoa/STGCalendarExportProgressWindow.m
            src/cocoa/STGCalendarExportView.h
            src/cocoa/STGCalendarExportView.m
            src/cocoa/STGProgressWindow.h
            src/cocoa/STGProgressWindow.m
            src/cocoa/STGCalendarImportExportView.h
            src/cocoa/STGCalendarImportExportViewSubclass.h
            src/cocoa/STGCalendarImportExportView.m
            src/cocoa/STGCalendarImportView.h
            src/cocoa/STGCalendarImportView.m
            src/cocoa/STGColoredCheckBoxView.h
            src/cocoa/STGColoredCheckBoxView.m
            src/cocoa/STGCalendarImportViewController.m
            src/cocoa/STGCalendarImportViewController.h
            src/cocoa/STGToolbar.mm
            src/cocoa/STGToolbar.h)

endif ()

if (WIN32)
    list(APPEND PLATFORM_LIBRARIES
            WinSparkle::WinSparkle)
endif ()

# Main Executable

add_executable(Strategr MACOSX_BUNDLE WIN32
        ${QT_RESOURCES}
        ${DESKTOP_SOURCES}
        src/main.cpp)

target_include_directories(Strategr PUBLIC src)

target_link_libraries(Strategr
        PRIVATE
        ${PLATFORM_LIBRARIES}
        Qt5::Widgets
        libstrategr)

# UI Tests Executable

add_executable(uitests
        ${QT_RESOURCES}
        ${DESKTOP_SOURCES}
        tests/qt_tests_main.cpp
        tests/main_window_test.cpp)

target_compile_definitions(uitests PUBLIC STG_UI_TEST)

target_link_libraries(uitests
        PRIVATE
        ${PLATFORM_LIBRARIES}
        Qt5::Widgets
        libstrategr
        Catch2::Catch2
        Qt5::Test)

target_include_directories(uitests PUBLIC src)

enable_testing()

include(Catch)

catch_discover_tests(uitests)

# MacOS Specifics

if (APPLE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)

    # Enable ARC
    target_compile_options(Strategr PRIVATE -fobjc-arc)
    target_compile_options(uitests PRIVATE -fobjc-arc)

    # Disable ARC for given files:
    set_source_files_properties(src/application.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)
    set_source_files_properties(src/macoswindow.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)

    set_target_properties(Strategr PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/deployment/Info.plist
            MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_SHORT}
            MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_SHORT}
            RESOURCE "${PLATFORM_RESOURCES}"
            XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_SOURCE_DIR}/deployment/Strategr.entitlements)

    set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "$ENV{CODE_SIGN_IDENTITY}")
    set(CMAKE_XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "--deep --timestamp --options runtime --keychain \"$ENV{CODE_SIGN_KEYCHAIN}\"")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_INJECT_BASE_ENTITLEMENTS NO)
    set(CMAKE_XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING ON)
    set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET 10.13)
endif ()


# Windows Specifics

if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr)

    # Configure resource file

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr.rc"
            "${CMAKE_CURRENT_BINARY_DIR}/Strategr.rc")

    # Copy icons

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr.ico"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategy.ico"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif ()

# Deployment

include(Deployment)

#if (CMAKE_BUILD_TYPE MATCHES Rel)
#    if (WIN32)
#        set_target_properties(Strategr PROPERTIES WIN32_EXECUTABLE ON)
#
#        # Configure installer scripts
#
#        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.template.iss"
#                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.iss")
#
#        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.template.iss"
#                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.iss ")
#
#        # Deploy Qt
#
#        add_custom_command(TARGET Strategr
#                POST_BUILD
#                COMMAND ${Qt5_DIR}/../../../bin/windeployqt.exe --no-translations ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#    endif ()
#endif ()


message(STATUS "CMAKE_OSX_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET}")