if (NOT WIN32)
    set(SKIP_BUILD true)
    return()
endif ()

# Downloading & extracting sources

file(DOWNLOAD
        https://github.com/vslavik/winsparkle/releases/download/v0.7.0/WinSparkle-0.7.0.zip
        "${ARCHIVE_PATH}")

file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_PATH}" DESTINATION "${TMP_DIR}")
file(REMOVE "${ARCHIVE_PATH}")

# Setting source & build directories

file(GLOB SOURCE_DIR ${TMP_DIR}/WinSparkle*)

# Building for each platform

function(build)
    if (${PLATFORM} MATCHES x64)
        set(BUILD_DIR "${SOURCE_DIR}/x64/Release")
    else ()
        set(BUILD_DIR "${SOURCE_DIR}/Release")
    endif ()

    file(GLOB BINARY_OUTPUTS
            "${BUILD_DIR}/*.lib"
            "${BUILD_DIR}/*.dll")

    message("BINARY_OUTPUTS ${BINARY_OUTPUTS}")

    file(COPY ${BINARY_OUTPUTS}
            DESTINATION "${PLATFORM_LIB_DIR}")

    file(GLOB HEADERS "${SOURCE_DIR}/include/*.h")

    message("HEADERS ${HEADERS}")

    file(COPY ${HEADERS}
            DESTINATION "${PLATFORM_INCLUDE_DIR}")
endfunction()

