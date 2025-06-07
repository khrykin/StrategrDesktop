if (NOT VERSION)
    execute_process(COMMAND git describe --tags
            OUTPUT_VARIABLE VERSION
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (NOT VERSION)
        message(WARNING "No git tags was found. Setting version to 0.0.0")
        set(VERSION "0.0.0")
    endif ()

    string(REGEX REPLACE "^v" "" VERSION ${VERSION})
    string(REGEX REPLACE "-([0-9]+)-.+" ".\\1" VERSION_SHORT ${VERSION})


    message(STATUS "Project version: ${VERSION} (short: ${VERSION_SHORT})")
endif ()
