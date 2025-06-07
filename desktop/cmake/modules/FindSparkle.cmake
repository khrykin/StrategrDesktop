# FindPackage file for Sparkle.framework

# Look for Sparkle framework in vcpkg installation directory
find_library(Sparkle_FRAMEWORK Sparkle
    PATHS
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/Frameworks
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/Frameworks
    NO_DEFAULT_PATH
)

# Determine version:
if (NOT ${Sparkle_FRAMEWORK} MATCHES NOTFOUND)
    get_filename_component(Sparkle_DIR "${Sparkle_FRAMEWORK}" DIRECTORY)
    set(ENV{Sparkle_DIR} "${Sparkle_DIR}")

    set(Sparkle_INFO_PLIST ${Sparkle_FRAMEWORK}/Resources/Info.plist)

    file(READ ${Sparkle_INFO_PLIST} Sparkle_INFO_PLIST_CONTENTS)

    string(REGEX MATCH "<key>CFBundleVersion<\\/key>[^<]*<string>([^<]*)<\\/string>" _ ${Sparkle_INFO_PLIST_CONTENTS})
    set(Sparkle_VERSION ${CMAKE_MATCH_1})
endif ()

# Handle find_package arguments:
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sparkle
        REQUIRED_VARS
        Sparkle_FRAMEWORK

        VERSION_VAR
        Sparkle_VERSION)

# Setup targets:
if (Sparkle_FOUND AND NOT TARGET Sparkle::Sparkle)
    set(Sparkle_INCLUDE_DIR ${Sparkle_FRAMEWORK}/Headers)

    add_library(Sparkle::Sparkle SHARED IMPORTED)

    set_target_properties(Sparkle::Sparkle
            PROPERTIES
            FRAMEWORK TRUE
            INSTALL_NAME_DIR @executable_path/../Frameworks
            BUILD_WITH_INSTALL_NAME_DIR true
            IMPORTED_LOCATION ${Sparkle_FRAMEWORK}/Sparkle)

    target_include_directories(Sparkle::Sparkle
            INTERFACE
            ${Sparkle_FRAMEWORK})

endif ()
