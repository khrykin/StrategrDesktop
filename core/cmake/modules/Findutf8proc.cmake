# FindPackage file for static utf8roc

find_file(utf8proc_HEADER utf8proc.h)
find_library(utf8proc_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}utf8proc${CMAKE_STATIC_LIBRARY_SUFFIX})

# Determine version:

if (NOT ${utf8proc_HEADER} MATCHES NOTFOUND)
    file(READ ${utf8proc_HEADER} utf8proc_HEADER_CONTENTS)

    string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${utf8proc_HEADER_CONTENTS})
    set(utf8proc_VER_MAJOR ${CMAKE_MATCH_1})

    string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${utf8proc_HEADER_CONTENTS})
    set(utf8proc_VER_MINOR ${CMAKE_MATCH_1})

    string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ ${utf8proc_HEADER_CONTENTS})
    set(utf8proc_VER_PATCH ${CMAKE_MATCH_1})

    set(utf8proc_VERSION "${utf8proc_VER_MAJOR}.${utf8proc_VER_MINOR}.${utf8proc_VER_PATCH}")
endif ()

# Handle find_package arguments:

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(utf8proc
        REQUIRED_VARS
        utf8proc_LIBRARY
        utf8proc_HEADER

        VERSION_VAR
        utf8proc_VERSION)

# Setup targets:

if (utf8proc_FOUND AND NOT TARGET utf8proc::utf8proc)
    add_library(utf8proc::utf8proc STATIC IMPORTED)
    set_target_properties(utf8proc::utf8proc
            PROPERTIES
            IMPORTED_LOCATION ${utf8proc_LIBRARY}
            PUBLIC_HEADER ${utf8proc_HEADER})
endif ()