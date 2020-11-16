set(PACKAGES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/packages")

# Get the path to the Qt installation
get_filename_component(Qt5_ROOT "${Qt5_DIR}/../../../../" ABSOLUTE)

if (APPLE)
    set(MACOS_DEPLOYMENT_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos_deploy.sh")
    set(MACOS_PACKAGE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos_package.sh")
    set(MACOS_NOTARIZE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos_notarize.sh")
    set(MACOS_PACKAGE_PATH "${PACKAGES_DIR}/Strategr ${VERSION_SHORT}.dmg")

    set(BUNDLE_FRAMEWORKS "$<TARGET_BUNDLE_DIR:Strategr>/Contents/Frameworks")

    add_custom_command(TARGET Strategr
            POST_BUILD

            COMMAND rsync
            ARGS -avzh
            "${Sparkle_FRAMEWORK}"
            "${BUNDLE_FRAMEWORKS}")

    add_custom_command(TARGET Strategr
            POST_BUILD
            COMMENT "Deploying macOS application..."
            COMMAND "${MACOS_DEPLOYMENT_SCRIPT}"
            ARGS "${Qt5_ROOT}" "$<TARGET_BUNDLE_DIR:Strategr>")

    add_custom_target(package
            DEPENDS Strategr
            COMMAND "${MACOS_PACKAGE_SCRIPT}"
            "$<TARGET_BUNDLE_DIR:Strategr>"
            "${PACKAGES_DIR}")

    add_custom_command(TARGET package
            POST_BUILD
            COMMENT "Notarizing macOS package..."
            COMMAND "${MACOS_NOTARIZE_SCRIPT}"
            ARGS com.khrykin.Strategr "${MACOS_PACKAGE_PATH}")
endif ()

if (WIN32)
    # Copy non-Qt dlls

    string(REGEX REPLACE " \\.lib" ".dll" WinSparkle_DLL "${WinSparkle_PATH}")
    install(FILE "${WinSparkle_DLL}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR} ")
endif ()