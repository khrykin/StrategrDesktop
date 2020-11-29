# FindPackage file for static WinSparkle DLL

find_path(WinSparkle_INCLUDE_DIR winsparkle.h)
find_library(WinSparkle_LIBRARY WinSparkle)

string(REPLACE ".lib" ".dll" WinSparkle_DLL "${WinSparkle_LIBRARY}")

# Determine version:

if (NOT ${WinSparkle_INCLUDE_DIR} MATCHES NOTFOUND)
    # TODO: Obtain version

    set(WinSparkle_VERSION "1.0.0")
endif ()

# Handle find_package arguments:

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WinSparkle
        REQUIRED_VARS
        WinSparkle_LIBRARY
        WinSparkle_INCLUDE_DIR

        VERSION_VAR
        WinSparkle_VERSION)

# Setup targets:

if (WinSparkle_FOUND AND NOT TARGET WinSparkle::WinSparkle)
    add_library(WinSparkle::WinSparkle SHARED IMPORTED)
    set_target_properties(WinSparkle::WinSparkle
            PROPERTIES
            IMPORTED_LOCATION "${WinSparkle_DLL}"
            IMPORTED_IMPLIB "${WinSparkle_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${WinSparkle_INCLUDE_DIR}")
endif ()