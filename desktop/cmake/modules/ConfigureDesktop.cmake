set(DESKTOP_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

set(DESKTOP_VERSION ${VERSION})
set(DESKTOP_VERSION_SHORT ${VERSION_SHORT})

file(REMOVE ${DESKTOP_VERSION_FILE})


# Get repo name
git_get_repo_name(REPO_NAME)

# Get appcast url

string(REGEX REPLACE "^[^/]+/" "" GITHUB_REPO "${REPO_NAME}")
string(REGEX REPLACE "/[^/]+$" "" GITHUB_USER "${REPO_NAME}")
set(APPCAST_URL "https://${GITHUB_USER}.github.io/${GITHUB_REPO}/appcast.xml")

# Generate sources
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in ${DESKTOP_VERSION_FILE})
