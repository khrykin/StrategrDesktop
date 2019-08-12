#
# Make a version file containing the current version from git.
#
include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-d)

#parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")

set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

file(REMOVE ${version_file})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/version.cpp.in
        ${version_file})

