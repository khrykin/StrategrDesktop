# Downloading & extracting sources

execute_process(COMMAND git clone git://code.qt.io/qt/qt5.git "${TMP_DIR}"
        COMMAND_ERROR_IS_FATAL ANY)

execute_process(COMMAND git checkout 5.15
        WORKING_DIRECTORY "${TMP_DIR}"
        COMMAND_ERROR_IS_FATAL ANY)

# Initializing repository

if (WIN32)
    set(EXTRA_MODULES ,qtwinextras)
elseif (APPLE)
    set(EXTRA_MODULES ,qtmacextras)
endif ()

execute_process(COMMAND perl init-repository
        --module-subset=qtbase${EXTRA_MODULES}

        WORKING_DIRECTORY "${TMP_DIR}"
        COMMAND_ERROR_IS_FATAL ANY)

set(MAKE_COMMAND make)
if (WIN32)
    set(MAKE_COMMAND "${TOOLS_DIR}/jom.exe")
endif ()

if (WIN32 AND NOT EXISTS "${MAKE_COMMAND}")
    # Downloading jom

    message(STATUS "Installing jom...")

    file(DOWNLOAD http://download.qt.io/official_releases/jom/jom.zip
            ${TMP_DIR}/jom.zip)

    file(ARCHIVE_EXTRACT INPUT "${TMP_DIR}/jom.zip"
            DESTINATION "${TMP_DIR}")

    file(RENAME "${TMP_DIR}/jom.exe" "${TOOLS_DIR}/jom.exe")
endif ()

# Setting source & build directories

set(SOURCE_DIR ${TMP_DIR})
set(BUILD_DIR ${SOURCE_DIR}/build)

# Building for each platform

function(build)
    message(STATUS "make command: ${MAKE_COMMAND}")
    set(CONFIGURE_ARGS
            -opensource
            -confirm-license
            -release
            -silent

            -no-dbus

            -nomake tools
            -nomake tests
            -nomake examples)

    if (WIN32)
        set(CONFIGURE_SCRIPT ${SOURCE_DIR}/configure.bat)
        list(APPEND CONFIGURE_ARGS
                -platform win32-msvc)
    else ()
        set(CONFIGURE_SCRIPT ${SOURCE_DIR}/configure)
    endif ()

    if (APPLE)
        list(APPEND CONFIGURE_ARGS
                #                QMAKE_MAC_SDK=macosx11.0
                -appstore-compliant)
    endif ()

    set(INSTALL_PREFIX "${PLATFORM_PREFIX}")

    if (NOT "${Qt5_BUILD_STATIC_LIBS_ONLY}")
        # Building shared version
        message(STATUS "Building shared version...")

        file(REMOVE_RECURSE "${BUILD_DIR}")
        file(MAKE_DIRECTORY "${BUILD_DIR}")

        set(SHARED_SWITCH -shared)
        if (APPLE)
            set(SHARED_SWITCH -framework)
        endif ()

        message("CONFIGURE_SCRIPT ${CONFIGURE_SCRIPT}")
        message("CONFIGURE_ARGS ${CONFIGURE_ARGS}")

        execute_process(COMMAND "${CONFIGURE_SCRIPT}"
                ${CONFIGURE_ARGS}
                -prefix "${INSTALL_PREFIX}"
                ${SHARED_SWITCH}

                WORKING_DIRECTORY "${BUILD_DIR}"
                COMMAND_ERROR_IS_FATAL ANY)

        execute_process(COMMAND "${MAKE_COMMAND}" -j8
                WORKING_DIRECTORY "${BUILD_DIR}"
                COMMAND_ERROR_IS_FATAL ANY)

        execute_process(COMMAND "${MAKE_COMMAND}" install
                WORKING_DIRECTORY "${BUILD_DIR}"
                COMMAND_ERROR_IS_FATAL ANY)
    endif ()

    # Building static version
    message(STATUS "Building static version...")

    file(REMOVE_RECURSE "${BUILD_DIR}")
    file(MAKE_DIRECTORY "${BUILD_DIR}")

    if (NOT "${Qt5_BUILD_STATIC_LIBS_ONLY}")
        set(INSTALL_PREFIX "${INSTALL_DIR}_static/${PLATFORM}")
    endif ()

    execute_process(COMMAND "${CONFIGURE_SCRIPT}"
            ${CONFIGURE_ARGS}
            -prefix "${INSTALL_PREFIX}"
            -static

            WORKING_DIRECTORY "${BUILD_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)

    execute_process(COMMAND "${MAKE_COMMAND}" -j8
            WORKING_DIRECTORY "${BUILD_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)

    execute_process(COMMAND "${MAKE_COMMAND}" install
            WORKING_DIRECTORY "${BUILD_DIR}"
            COMMAND_ERROR_IS_FATAL ANY)
endfunction()


