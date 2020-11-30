if (WIN32)
    message(STATUS "Installing vswhere...")

    if (NOT EXISTS "${TOOLS_DIR}/vswhere.exe")
        file(DOWNLOAD
                https://github.com/microsoft/vswhere/releases/download/2.8.4/vswhere.exe
                "${TOOLS_DIR}/vswhere.exe")
    endif ()

    set(VSWHERE_PROGRAM "${TOOLS_DIR}/vswhere.exe")

    message(STATUS "Done installing vswhere")
endif ()