include(GitVersion)

get_git_version(_ STRATEGR_CORE_VERSION)

set(STRATEGR_CORE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/core_version.cpp")

file(REMOVE ${STRATEGR_CORE_VERSION_FILE})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in ${STRATEGR_CORE_VERSION_FILE})
