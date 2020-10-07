# FindPackage file for static WinSparkle DLL

find_file(WinSparkle_HEADER winsparkle.h)
find_library(WinSparkle_LIBRARY WinSparkle)

# Determine version:

if (NOT ${WinSparkle_FRAMEWORK} MATCHES NOTFOUND)
    # TODO: Obtain version

    set(WinSparkle_VERSION ${CMAKE_MATCH_1})
endif ()

# Handle find_package arguments:

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WinSparkle
        REQUIRED_VARS
        WinSparkle_LIBRARY
        WinSparkle_HEADER

        VERSION_VAR
        WinSparkle_VERSION)

# Setup targets:

if (WinSparkle_FOUND AND NOT TARGET WinSparkle::WinSparkle)
    add_library(WinSparkle::WinSparkle SHARED IMPORTED)
    set_target_properties(WinSparkle::WinSparkle
            PROPERTIES
            IMPORTED_LOCATION ${WinSparkle_LIBRARY}
            PUBLIC_HEADER ${WinSparkle_HEADER})
endif ()