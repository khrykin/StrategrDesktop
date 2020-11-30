# Downloading & extracting sources

file(DOWNLOAD
        https://github.com/catchorg/Catch2/archive/v2.13.3.zip
        "${ARCHIVE_PATH}")

file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_PATH}" DESTINATION "${TMP_DIR}")
file(REMOVE "${ARCHIVE_PATH}")

# Setting source & build directories

file(GLOB SOURCE_DIR ${TMP_DIR}/Catch2*)
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
            --install "${BUILD_DIR}"
            --config Release
            COMMAND_ERROR_IS_FATAL ANY)
endfunction()


