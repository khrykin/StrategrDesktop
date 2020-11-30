if (NOT APPLE)
    set(SKIP_BUILD true)
    return()
endif ()

# Downloading & extracting sources

set(ARCHIVE_PATH "${TMP_DIR}/Sparkle.tar.xz")

file(DOWNLOAD
        https://github.com/sparkle-project/Sparkle/releases/download/1.24.0/Sparkle-1.24.0.tar.xz
        "${ARCHIVE_PATH}")

file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_PATH}" DESTINATION "${TMP_DIR}")
file(REMOVE "${ARCHIVE_PATH}")

# Setting source & build directories

set(SOURCE_DIR ${TMP_DIR})

# Building for each platform

function(build)
    # Setting install directories

    file(COPY "${SOURCE_DIR}/Sparkle.framework"
            DESTINATION "${PLATFORM_LIB_DIR}")
endfunction()



