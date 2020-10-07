cmake_minimum_required(VERSION 3.14)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)

project(StrategrDesktop)

set(CMAKE_CXX_STANDARD 17)


# Include core library

add_subdirectory(../core ${CMAKE_CURRENT_BINARY_DIR}/core)

# Configure files

list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/cmake/modules
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(ConfigureDesktop)

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

# Shared UI Library

add_library(StrategrDesktop STATIC
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

target_include_directories(StrategrDesktop PUBLIC src)

target_link_libraries(StrategrDesktop
        PUBLIC
        Qt5::Widgets
        libstrategr)

# Main Executable

add_executable(Strategr MACOSX_BUNDLE WIN32
        ${QT_RESOURCES}
        src/main.cpp)

target_link_libraries(Strategr
        PRIVATE
        StrategrDesktop)

# UI Tests Executable

add_executable(uitest
        tests/qt_tests_main.cpp
        tests/main_window_test.cpp)

target_link_libraries(uitest
        PRIVATE
        StrategrDesktop
        Catch2::Catch2
        Qt5::Test)

# MacOS Specifics

if (APPLE)
    set(PLATFORM_RESOURCES
            deployment/Strategr.icns
            deployment/Strategy.icns)

    target_link_libraries(StrategrDesktop
            PRIVATE
            "-framework AppKit"
            "-framework UserNotifications"
            Sparkle::Sparkle
            Qt5::MacExtras)

    target_sources(StrategrDesktop
            PRIVATE
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

    # Enable ARC
    target_compile_options(StrategrDesktop PRIVATE -fobjc-arc)

    # Disable ARC for given files:
    set_source_files_properties(src/application.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)
    set_source_files_properties(src/macoswindow.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)     #

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)

    # Set a custom plist file for the app bundle
    set_target_properties(Strategr PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/deployment/Info.plist
            MACOSX_BUNDLE_BUNDLE_VERSION ${DESKTOP_VERSION}
            MACOSX_BUNDLE_LONG_VERSION_STRING ${DESKTOP_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${DESKTOP_VERSION_SHORT}
            RESOURCE "${PLATFORM_RESOURCES}")
endif ()

# Windows Specifics

if (WIN32)
    target_link_libraries(StrategrDesktop
            PRIVATE
            WinSparkle::WinSparkle)

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

    #    if (CMAKE_BUILD_TYPE MATCHES Rel)
    #        set_target_properties(Strategr PROPERTIES WIN32_EXECUTABLE ON)
    #
    #        # Configure installer scripts
    #
    #        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.template.iss"
    #                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.iss")
    #
    #        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.template.iss"
    #                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.iss")
    #
    #        # Deploy Qt
    #
    #        add_custom_command(TARGET Strategr
    #                POST_BUILD
    #                COMMAND ${Qt5_DIR}/../../../bin/windeployqt.exe --no-translations ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    #
    #    endif (CMAKE_BUILD_TYPE MATCHES Rel)
endif ()


#    set(CORE_PLATFORM_LIBRARIES
#
#            "-framework EventKit")
#
#    set(PLATFORM_LIBRARIES
#            "-framework Foundation"
#            "-framework AppKit"
#            "-framework UserNotifications"
#            "-framework CoreImage"
#            "-framework QuartzCore"
#            ${Sparkle_FRAMEWORK_PATH}
#            Qt5::MacExtras)
#
#    set(PLATFORM_FILES
#            ui/application.mm
#            ui/alert.mm
#            ui/macoswindow.mm
#            ui/applicationicon.mm
#            utility/notifierbackend.mm
#            utility/cocoautils.h
#            utility/cocoautils.mm
#            utility/macoscalendarexporter.mm
#            utility/macoscalendarexporter.h
#            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.m
#            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItem.h
#            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.m
#            third-party/TAAdaptiveSpaceItem/TAAdaptiveSpaceItemView.h
#            ui/cocoa/STGCalendarExportViewController.m
#            ui/cocoa/STGCalendarExportViewController.h
#            ui/cocoa/STGCalendarExportProgressWindow.h
#            ui/cocoa/STGCalendarExportProgressWindow.m
#            ui/cocoa/STGCalendarExportView.h
#            ui/cocoa/STGCalendarExportView.m
#            ui/cocoa/STGProgressWindow.h
#            ui/cocoa/STGProgressWindow.m
#            ui/cocoa/STGCalendarImportExportView.h
#            ui/cocoa/STGCalendarImportExportViewSubclass.h
#            ui/cocoa/STGCalendarImportExportView.m
#            ui/cocoa/STGCalendarImportView.h
#            ui/cocoa/STGCalendarImportView.m
#            ui/cocoa/STGColoredCheckBoxView.h
#            ui/cocoa/STGColoredCheckBoxView.m
#            ui/colorprovider.mm
#            ui/cocoa/STGCalendarImportViewController.m
#            ui/cocoa/STGCalendarImportViewController.h
#            ui/cocoa/STGToolbar.mm
#            ui/cocoa/STGToolbar.h)
#
#    set(PLATFORM_RESOURCES
#            )
#
#    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)
#
#    set_source_files_properties(ui/application.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)
#    set_source_files_properties(ui/macoswindow.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)
#
#endif ()
#
#if (WIN32)
#    set(PLATFORM_RESOURCES
#            ${CMAKE_CURRENT_BINARY_DIR}/Strategr.rc)
#
#    set(PLATFORM_LIBRARIES
#            ${WinSparkle_PATH}
#            Qt5::WinExtras)
#
#    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr)
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Strategr)
#endif ()
#
##add_executable(core_tests
##        tests_main.cpp
##        ${CORE}
##        ${CORE_TESTS})
##
##target_link_libraries(core_tests
##        Catch2::Catch2
##        Boost::filesystem
##        ${CORE_LIBRARIES}
##        ${CORE_PLATFORM_LIBRARIES})
#
#option(COVERAGE "Generate code coverage" OFF)
#
#if (COVERAGE)
#    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES Clang)
#        target_compile_options(core_tests PUBLIC -fprofile-instr-generate -fcoverage-mapping)
#        target_link_options(core_tests PUBLIC -fprofile-instr-generate)
#        #Uncomment in case of linker errors
#        #    target_link_libraries(core_tests clang_rt.profile-x86_64)
#    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU)
#        target_compile_options(core_tests PUBLIC --coverage)
#        #Uncomment in case of linker errors
#        #target_link_libraries(core_tests gcov)
#    endif ()
#endif ()
#
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#
#add_library(StrategrCore SHARED ${CORE})
#
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
#
#target_include_directories(StrategrCore PUBLIC core)
#
#target_link_libraries(StrategrCore
#        PRIVATE
#        utf8proc::utf8proc
#        nlohmann_json::nlohmann_json
#        ${CORE_PLATFORM_LIBRARIES})
#
#set_target_properties(StrategrCore PROPERTIES
#        FRAMEWORK TRUE
#        PUBLIC_HEADER core/strategy.h
#        VERSION ${VERSION_SHORT}
#        MACOSX_FRAMEWORK_LONG_VERSION_STRING ${VERSION}
#        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION_SHORT}
#        MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION}
#        XCODE_ATTRIBUTE_INSTALL_PATH @executable_path/../Frameworks/)
#
## Instruct CMake to run moc automatically when needed
#set(CMAKE_AUTOMOC ON)
#
#add_executable(Strategr
#        MACOSX_BUNDLE
#        main.cpp
#        ${PLATFORM_FILES}
#        ${UI}
#        ${UTILITY}
#        ${RESOURCES}
#        ${PLATFORM_RESOURCES})
#
#add_executable(ui_tests
#        qt_tests_main.cpp
#        ${PLATFORM_FILES}
#        ${UI}
#        ${UTILITY}
#        ${UI_TESTS}
#        ${RESOURCES})
#
#target_compile_definitions(ui_tests PRIVATE STG_UI_TEST)
#
#target_link_libraries(ui_tests
#        StrategrCore
#        Catch2::Catch2
#        Qt5::Widgets
#        Qt5::Test
#        ${PLATFORM_LIBRARIES})
#
#target_link_libraries(Strategr
#        StrategrCore
#        Qt5::Widgets
#        ${PLATFORM_LIBRARIES})
#
#if (APPLE)
#    if (CMAKE_BUILD_TYPE MATCHES Rel)
#        file(COPY ${Sparkle_FRAMEWORK_PATH} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Strategr.app/Contents/Frameworks)
#
#        add_custom_command(TARGET Strategr
#                POST_BUILD
#                COMMAND ${CMAKE_SOURCE_DIR}/scripts/macos_deploy.sh ${CMAKE_CURRENT_BINARY_DIR} ${VERSION})
#    endif (CMAKE_BUILD_TYPE MATCHES Rel)
#endif ()
#
#if (WIN32)
#    # Configure resource file
#
#    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr.rc"
#            "${CMAKE_CURRENT_BINARY_DIR}/Strategr.rc")
#
#    # Copy icons
#
#    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr.ico"
#            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategy.ico"
#            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#
#    # Copy non-Qt dlls
#
#    string(REGEX REPLACE "\\.lib" ".dll" WinSparkle_DLL "${WinSparkle_PATH}")
#    file(COPY "${WinSparkle_DLL}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
#
#    if (CMAKE_BUILD_TYPE MATCHES Rel)
#        set_target_properties(Strategr PROPERTIES WIN32_EXECUTABLE ON)
#
#        # Configure installer scripts
#
#        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.template.iss"
#                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x86.iss")
#
#        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.template.iss"
#                "${CMAKE_CURRENT_SOURCE_DIR}/deployment/Strategr_x64.iss")
#
#        # Deploy Qt
#
#        add_custom_command(TARGET Strategr
#                POST_BUILD
#                COMMAND ${Qt5_DIR}/../../../bin/windeployqt.exe --no-translations ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#
#    endif (CMAKE_BUILD_TYPE MATCHES Rel)
#endif ()
#


