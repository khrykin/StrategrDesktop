option(RELEASE_ONLY "\
Build only Release versions of vcpkg libraries. \
Be careful, as this will edit all triplet files in your vcpkg installation" ON)

option(CLEAN "Clean vcpkg folder after install" OFF)

if (NOT VCPKG_ROOT)
    set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
endif ()

if (WIN32)
    set(CMAKE_EXECUTABLE_SUFFIX .exe)
endif ()

set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg${CMAKE_EXECUTABLE_SUFFIX}")

function(vcpkg_install)
    if (NOT EXISTS "${VCPKG_ROOT}")
        message(STATUS "Vcpkg isn't installed in the expected location. Will install it now.")
        execute_process(COMMAND git clone https://github.com/microsoft/vcpkg "${VCPKG_ROOT}")
    else ()
        message(STATUS "Found vcpkg source directory (${VCPKG_ROOT})")
    endif ()

    if (VCPKG_COMMIT)
        execute_process(COMMAND git reset --hard ${VCPKG_COMMIT}
                WORKING_DIRECTORY "${VCPKG_ROOT}")
    endif ()

    if (NOT EXISTS "${VCPKG_EXECUTABLE}")
        message(STATUS "Vcpkg executable ${VCPKG_EXECUTABLE} not found")
        message(STATUS "Boostrapping vcpkg...")

        if (WIN32)
            set(bootstrap_script "${VCPKG_ROOT}/bootstrap-vcpkg.bat")
        else ()
            set(bootstrap_script "${VCPKG_ROOT}/bootstrap-vcpkg.sh")
        endif ()

        execute_process(COMMAND "${bootstrap_script}" -disableMetrics
                RESULT_VARIABLE FAILED)

        if (NOT FAILED)
            message(STATUS "vcpkg installed.")
        endif ()
    else ()
        message(STATUS "Found vcpkg executable (${VCPKG_ROOT}/vcpkg)")
    endif ()

endfunction()

function(parse_lists_file lists_file out)
    file(READ ${lists_file} contents)

    string(REGEX MATCHALL "find_package\\([^\\)]+" FIND_PACKAGE_ARGS "${contents}")
    list(TRANSFORM FIND_PACKAGE_ARGS REPLACE "find_package\\(" "")

    foreach (ARGS ${FIND_PACKAGE_ARGS})
        list(TRANSFORM ARGS REPLACE " +" ";")
        list(GET ARGS 0 PKG_name)
        string(TOLOWER ${PKG_name} PKG_name)
        string(REGEX REPLACE "_" "-" PKG_name ${PKG_name})

        list(APPEND PACKAGES ${PKG_name})
    endforeach ()

    set(${out} ${PACKAGES} PARENT_SCOPE)
endfunction()

function(install_package package_info success)
    string(REPLACE "@" ";" package_info ${package_info})

    list(GET package_info 0 package_name)

    string(REGEX REPLACE "-.+" "" package_name_base ${package_name})

    list(LENGTH package_info package_info_length)

    if (package_info_length EQUAL 2)
        list(GET package_info 1 package_sha)
    endif ()

    if (NOT package_sha)
        set(package_sha master)
    endif ()

    message(STATUS "Installing ${package_name}@${package_sha}...")

    execute_process(COMMAND
            git checkout ${package_sha} -- ports

            WORKING_DIRECTORY "${VCPKG_ROOT}"
            RESULT_VARIABLE failed)

    if (NOT failed)
        if (EXISTS vcpkg-triplets)
            set(flags --overlay-triplets=vcpkg-triplets)
        endif ()

        if (NOT TRIPLETS)
            execute_process(COMMAND
                    "${VCPKG_ROOT}/vcpkg" install
                    ${package_name}
                    ${flags}

                    RESULT_VARIABLE failed)
        else ()
            foreach (triplet ${TRIPLETS})
                if (NOT failed)
                    execute_process(COMMAND
                            "${VCPKG_ROOT}/vcpkg" install
                            ${package_name}:${triplet}
                            ${flags}

                            RESULT_VARIABLE failed)
                endif ()
            endforeach ()
        endif ()
    endif ()

    if (NOT failed)
        set(${success} 1 PARENT_SCOPE)
    else ()
        set(${success} 0 PARENT_SCOPE)
    endif ()
endfunction()

function(make_release_only_triplets triplet_files)
    message(STATUS "Adding \"set(CPKG_BUILD_TYPE release)\" to triplet files")

    foreach (file ${triplet_files})
        file(READ ${file} file_contents)

        if (NOT ${file_contents} MATCHES "VCPKG_BUILD_TYPE release")
            file(APPEND ${file} "\nset(VCPKG_BUILD_TYPE release)\n")
        endif ()
    endforeach ()
endfunction()


# Main logic

include(./Dependencies.cmake)

vcpkg_install()

if (RELEASE_ONLY)
    # Find all triplet files
    file(GLOB_RECURSE TRIPLET_FILES "${VCPKG_ROOT}/triplets/*.cmake")

    make_release_only_triplets("${TRIPLET_FILES}")
endif ()


foreach (PACKAGE_INFO ${PACKAGES})
    install_package(${PACKAGE_INFO} success)

    if (success)
        message(STATUS "Installing ${PACKAGE} done")
    else ()
        message(FATAL_ERROR "Installing ${PACKAGE} failed")
    endif ()
endforeach ()

if (CLEAN)
    file(REMOVE_RECURSE "${VCPKG_ROOT}/downloads")
    file(REMOVE_RECURSE "${VCPKG_ROOT}/packages")
endif ()


