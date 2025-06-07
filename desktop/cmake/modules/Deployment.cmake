set(PACKAGES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/packages")

# Get the path to the Qt installation
get_filename_component(Qt5_ROOT "${Qt5_DIR}/../../../../" ABSOLUTE)

if (APPLE)
    set(MACOS_DEPLOYMENT_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos_deploy.sh")
    set(MACOS_PACKAGE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos_package.sh")
    set(MACOS_NOTARIZE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos_notarize.sh")
    set(MACOS_PACKAGE_PATH "${PACKAGES_DIR}/Strategr ${VERSION_SHORT}.dmg")

    set(BUNDLE_FRAMEWORKS "$<TARGET_BUNDLE_DIR:Strategr>/Contents/Frameworks")

    # Create Frameworks directory and copy Sparkle framework
    add_custom_command(TARGET Strategr
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${BUNDLE_FRAMEWORKS}"
            COMMAND ${CMAKE_COMMAND} -E echo "Copying Sparkle framework from ${Sparkle_FRAMEWORK} to ${BUNDLE_FRAMEWORKS}"
            COMMAND rsync
            ARGS -azh
            "${Sparkle_FRAMEWORK}"
            "${BUNDLE_FRAMEWORKS}"
            COMMAND ${CMAKE_COMMAND} -E echo "Sparkle framework copy completed")

    add_custom_command(TARGET Strategr
            POST_BUILD
            COMMENT "Deploying macOS application..."
            COMMAND "${MACOS_DEPLOYMENT_SCRIPT}"
            ARGS "$<TARGET_BUNDLE_DIR:Strategr>")

    add_custom_target(package
            DEPENDS Strategr
            COMMAND bash
            ARGS "${MACOS_PACKAGE_SCRIPT}"
            "$<TARGET_BUNDLE_DIR:Strategr>"
            "${PACKAGES_DIR}")

    add_custom_command(TARGET package
            POST_BUILD
            COMMENT "Notarizing macOS package..."
            COMMAND "${MACOS_NOTARIZE_SCRIPT}"
            ARGS com.khrykin.Strategr "${MACOS_PACKAGE_PATH}")

    set_target_properties(package
            PROPERTIES
            EXCLUDE_FROM_ALL true)
endif ()

if (WIN32)
    if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES x64)
        set(WINDOWS_64_OR_EMPTY 64)
    endif ()

    if (${CMAKE_VS_PLATFORM_NAME} MATCHES Win32)
        set(WINDOWS_PLATFORM_NAME x86)
    else ()
        set(WINDOWS_PLATFORM_NAME ${CMAKE_VS_PLATFORM_NAME})
    endif ()

    # Copy dlls

    add_custom_command(TARGET Strategr
            POST_BUILD

            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_if_different
            "${WinSparkle_DLL}"
            "$<TARGET_FILE_DIR:Strategr>")

    # Configure installer script

    configure_file("${CMAKE_SOURCE_DIR}/deployment/Strategr.in.iss"
            "${CMAKE_CURRENT_BINARY_DIR}/Strategr.in.iss")

    file(GENERATE OUTPUT "$<TARGET_FILE_DIR:Strategr>/Strategr.iss"
            INPUT "${CMAKE_CURRENT_BINARY_DIR}/Strategr.in.iss "
            CONDITION $<CONFIG:Release>)

    # Package target

    add_custom_target(package
            COMMAND iscc "$<TARGET_FILE_DIR:Strategr>/Strategr.iss"
            "/O\"$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/packages>\""
            DEPENDS Strategr)

    # Download MSVC Redist package

    add_custom_command(TARGET package
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND}
            "-DDOWNLOAD_DIR=\"$<TARGET_FILE_DIR:Strategr>\""
            "-DCMAKE_VS_PLATFORM_NAME=\"${CMAKE_VS_PLATFORM_NAME}\""
            -P "${CMAKE_SOURCE_DIR}/cmake/modules/GetMSVCRedist.cmake")

    set_target_properties(package
            PROPERTIES
            EXCLUDE_FROM_ALL true)
endif ()
