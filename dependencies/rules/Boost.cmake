# Downloading & extracting sources

file(DOWNLOAD
        https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.zip
        "${ARCHIVE_PATH}")

file(ARCHIVE_EXTRACT INPUT "${ARCHIVE_PATH}" DESTINATION "${TMP_DIR}")
file(REMOVE "${ARCHIVE_PATH}")

# Setting source & build directories

file(GLOB SOURCE_DIR ${TMP_DIR}/boost*)

# Building for each platform

macro(build)
    #    if (ALREADY_BUILT_AT)
    #        # Optimize installation size:
    #        # Symlink first built platform prefix
    #
    #        message(STATUS "Symlinking header-only installation from \"${ALREADY_BUILT_AT}\"...")
    #
    #        file(REMOVE_RECURSE "${PLATFORM_PREFIX}")
    #        file(CREATE_LINK "${ALREADY_BUILT_AT}" "${PLATFORM_PREFIX}" COPY_ON_ERROR SYMBOLIC)
    #    else ()
    if (WIN32)
        set(BOOTSTRAP_SCRIPT "${SOURCE_DIR}/bootstrap.bat")
    else ()
        set(BOOTSTRAP_SCRIPT "${SOURCE_DIR}/bootstrap.sh")
    endif ()

    execute_process(COMMAND "${BOOTSTRAP_SCRIPT}"
            WORKING_DIRECTORY "${SOURCE_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)

    execute_process(COMMAND ./b2
            "--prefix=${PLATFORM_PREFIX}"
            --with-headers

            install
            COMMAND_ECHO STDOUT
            WORKING_DIRECTORY "${SOURCE_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)

    set(ALREADY_BUILT_AT "${PLATFORM_PREFIX}")
    #    endif ()
endmacro()




