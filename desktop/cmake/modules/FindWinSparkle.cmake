# FindPackage file for WinSparkle.
#
# vcpkg's winsparkle port installs a raw header/lib/dll (no CMake or
# pkg-config config file at all), so it needs a MODULE-mode find script,
# same idea as FindSparkle.cmake does for Sparkle.framework on macOS.

find_path(WinSparkle_INCLUDE_DIR
    NAMES winsparkle.h
    PATHS
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include/winsparkle
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/include/winsparkle
    NO_DEFAULT_PATH
)

find_library(WinSparkle_LIBRARY
    NAMES WinSparkle
    PATHS
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/lib
    NO_DEFAULT_PATH
)

find_file(WinSparkle_DLL
    NAMES WinSparkle.dll
    PATHS
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/debug/bin
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WinSparkle
        REQUIRED_VARS
        WinSparkle_LIBRARY
        WinSparkle_INCLUDE_DIR)

if (WinSparkle_FOUND AND NOT TARGET WinSparkle::WinSparkle)
    add_library(WinSparkle::WinSparkle SHARED IMPORTED)

    set_target_properties(WinSparkle::WinSparkle
            PROPERTIES
            IMPORTED_IMPLIB ${WinSparkle_LIBRARY}
            IMPORTED_LOCATION ${WinSparkle_DLL}
            INTERFACE_INCLUDE_DIRECTORIES ${WinSparkle_INCLUDE_DIR})
endif ()
