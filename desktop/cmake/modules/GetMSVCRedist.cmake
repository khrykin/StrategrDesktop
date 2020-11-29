if ("${CMAKE_VS_PLATFORM_NAME}" MATCHES x64)
    set(MSVC_REDIST_NAME vc_redist.x64.exe)
elseif ("${CMAKE_VS_PLATFORM_NAME}" MATCHES Win32)
    set(MSVC_REDIST_NAME vc_redist.x86.exe)
elseif ("${CMAKE_VS_PLATFORM_NAME}" MATCHES ARM64)
    set(MSVC_REDIST_NAME vc_redist.arm64.exe)
endif ()

set(MSVC_REDIST_URL https://aka.ms/vs/16/release/${MSVC_REDIST_NAME})
set(MSVC_REDIST_DOWNLOAD_PATH "${DOWNLOAD_DIR}/${MSVC_REDIST_NAME}")

message(STATUS "Downloading MSVC Redistributable package for architecture: ${CMAKE_VS_PLATFORM_NAME}...")

if (NOT EXISTS "${MSVC_REDIST_DOWNLOAD_PATH}")
    file(DOWNLOAD ${MSVC_REDIST_URL} "${MSVC_REDIST_DOWNLOAD_PATH}" SHOW_PROGRESS)
else ()
    message(STATUS "MSVC Redistributable package already downloaded")
endif ()