#
# Make a version file containing the current version from git.
#

include(GitVersion)

get_git_version(DESKTOP_VERSION DESKTOP_VERSION_SHORT)

set(DESKTOP_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

file(REMOVE ${DESKTOP_VERSION_FILE})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in ${DESKTOP_VERSION_FILE})
