# Downloading & extracting sources

file(DOWNLOAD
        https://github.com/JuliaStrings/utf8proc/archive/v2.6.0.zip
        "${ARCHIVE_PATH}")

file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_PATH}" DESTINATION "${TMP_DIR}")
file(REMOVE "${ARCHIVE_PATH}")

# Setting source & build directories

file(GLOB SOURCE_DIR ${TMP_DIR}/utf8proc*)
set(BUILD_DIR ${SOURCE_DIR}/build)

# Building for each platform

function(build)
    # Resetting build directory

    file(REMOVE_RECURSE "${BUILD_DIR}")
    file(MAKE_DIRECTORY "${BUILD_DIR}")

    execute_process(COMMAND ${CMAKE_COMMAND}
            ${SHARED_CMAKE_DEFINITIONS}
            -S "${SOURCE_DIR}"
            -B "${BUILD_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)

    execute_process(COMMAND ${CMAKE_COMMAND}
            --build "${BUILD_DIR}"
            --config Release
            COMMAND_ERROR_IS_FATAL ANY)

    file(GLOB BINARY_OUTPUT
            ${BUILD_DIR}/libutf8proc.a
            ${BUILD_DIR}/Release/utf8proc_static.lib)

    file(COPY "${BINARY_OUTPUT}"
            DESTINATION "${PLATFORM_LIB_DIR}")

    file(COPY "${SOURCE_DIR}/utf8proc.h"
            DESTINATION "${PLATFORM_INCLUDE_DIR}")
endfunction()



