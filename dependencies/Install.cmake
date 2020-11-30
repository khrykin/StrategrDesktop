cmake_minimum_required(VERSION 3.19.1)

# Options

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)

option(OSX_UNIVERSAL_BINARIES "Build universal binaries for macOS" ON)

if (NOT PLATFORMS)
    if (APPLE)
        set(PLATFORMS x86_64)
    elseif (WIN32)
        set(PLATFORMS Win32 x64)
    elseif (UNIX)
        set(PLATFORMS x64)
    endif ()
endif ()

# Special directories

include("${CMAKE_CURRENT_LIST_DIR}/cmake/vcenv.cmake")

set(DEPENDENCIES_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(DEPENDENCIES_TMP_DIR "${DEPENDENCIES_DIR}/tmp")
set(DEPENDENCIES_TOOLS_DIR "${DEPENDENCIES_DIR}/tools")

if (NOT DEPENDENCIES_INSTALL_DIR)
    set(DEPENDENCIES_INSTALL_DIR "${DEPENDENCIES_DIR}/installed")
endif ()

file(GLOB DEPENDENCIES_BUILD_RULES "${DEPENDENCIES_DIR}/rules/*.cmake")
file(GLOB DEPENDENCIES_TOOLS_RULES "${DEPENDENCIES_DIR}/rules/tools/*.cmake")

message(STATUS "Installing dependecies...")

# Install build tools

set(TOOLS_DIR "${DEPENDENCIES_TOOLS_DIR}")
foreach (TOOL_BUILD_FILE ${DEPENDENCIES_TOOLS_RULES})
    get_filename_component(TOOL_NAME "${TOOL_BUILD_FILE}" NAME_WLE)

    # Create tools directory
    if (NOT "${DEPENDENCIES_TOOLS_DIR}")
        file(MAKE_DIRECTORY "${DEPENDENCIES_TOOLS_DIR}")
    endif ()

    include("${TOOL_BUILD_FILE}")
endforeach ()

# Build dependecies

foreach (DEPENDECY_BUILD_FILE ${DEPENDENCIES_BUILD_RULES})
    get_filename_component(DEPENDENCY_NAME "${DEPENDECY_BUILD_FILE}" NAME_WLE)

    # Determine what to build according to ONLY variable
    list(FIND ONLY ${DEPENDENCY_NAME} INDEX_IN_ONLY)
    if (NOT ONLY OR INDEX_IN_ONLY GREATER -1)
        message(STATUS "Installing ${DEPENDENCY_NAME}...")

        # Reset temporary directory

        # TODO:
        file(REMOVE_RECURSE "${DEPENDENCIES_TMP_DIR}")

        # Set dependency install directory

        set(DEPENDENCY_INSTALL_DIR "${DEPENDENCIES_INSTALL_DIR}/${DEPENDENCY_NAME}")

        if (NOT ONLY AND EXISTS "${DEPENDENCY_INSTALL_DIR}")
            message(STATUS
                    "Install directory for ${DEPENDENCY_NAME} "
                    "already exists at:\n\t\"${DEPENDENCY_INSTALL_DIR}\""
                    "\n\tRemove it if you want to reinstall ${DEPENDENCY_NAME}")
            message(STATUS "Skipping...")
        else ()
            file(MAKE_DIRECTORY "${DEPENDENCY_INSTALL_DIR}")

            # Execute buildfile

            set(TMP_DIR "${DEPENDENCIES_TMP_DIR}")
            set(ARCHIVE_PATH "${DEPENDENCIES_TMP_DIR}/${DEPENDENCY_NAME}.zip")
            set(INSTALL_DIR "${DEPENDENCY_INSTALL_DIR}")
            set(TOOLS_DIR "${DEPENDENCIES_TOOLS_DIR}")
            set(SKIP_BUILD false)

            function(build)
                # Default build function: noop
            endfunction()

            function(post_build_all)
                # Default post build function: noop
            endfunction()

            include("${DEPENDECY_BUILD_FILE}")

            if (${SKIP_BUILD})
                message(STATUS "Skipping installing ${DEPENDENCY_NAME}...\n")
                file(REMOVE_RECURSE "${DEPENDENCY_INSTALL_DIR}")
            else ()
                foreach (PLATFORM ${PLATFORMS})
                    message(STATUS "Installing for ${PLATFORM}...")

                    # Setting shared CMake flags...

                    set(SHARED_CMAKE_DEFINITIONS
                            -DCMAKE_BUILD_TYPE=Release)

                    if (APPLE)
                        list(APPEND SHARED_CMAKE_DEFINITIONS
                                -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
                    endif ()

                    # Setting platform flags...

                    if (APPLE)
                        if (${PLATFORM} STREQUAL universal)
                            set(CMAKE_OSX_ARCHITECTURES x86_64 arm64)
                            list(APPEND SHARED_CMAKE_DEFINITIONS
                                    -DCMAKE_OSX_ARCHITECTURES=x86_64\\\;arm64)
                        else ()
                            set(CMAKE_OSX_ARCHITECTURES ${PLATFORM})
                            list(APPEND SHARED_CMAKE_DEFINITIONS
                                    "-DCMAKE_OSX_ARCHITECTURES=${PLATFORM}")
                        endif ()

                    elseif (WIN32)
                        list(APPEND SHARED_CMAKE_DEFINITIONS
                                -A ${PLATFORM})
                        set_msvc_env(${PLATFORM})
                    endif ()

                    # Setting install directories

                    set(PLATFORM_PREFIX "${INSTALL_DIR}/${PLATFORM}")
                    set(PLATFORM_LIB_DIR "${PLATFORM_PREFIX}/lib")
                    set(PLATFORM_INCLUDE_DIR "${PLATFORM_PREFIX}/include")

                    set(CMAKE_INSTALL_PREFIX "${PLATFORM_PREFIX}")
                    list(APPEND SHARED_CMAKE_DEFINITIONS
                            "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

                    # Call build function from buildfile
                    build()
                endforeach ()

                # Cleanup
                file(REMOVE_RECURSE "${DEPENDENCIES_TMP_DIR}")

                foreach (PLATFORM ${PLATFORMS})
                    file(REMOVE_RECURSE "${DEPENDENCIES_INSTALL_DIR}/${PLATFORM}/doc")
                endforeach ()

                message(STATUS "Done installing ${DEPENDENCY_NAME}\n")
            endif ()
        endif ()
    endif ()
endforeach ()

message("")
message(STATUS "Done installing dependencies")


