# Reads git tag into semantic version.

macro(get_git_tag GIT_TAG)
    execute_process(COMMAND git describe --tags OUTPUT_VARIABLE ${GIT_TAG})
    string(REGEX REPLACE "\n" "" ${GIT_TAG} ${${GIT_TAG}})
endmacro()

macro(get_git_version)
    get_git_tag(GIT_VERSION)

    # Parse the version information into pieces.
    string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" GIT_VERSION_MAJOR "${GIT_VERSION}")
    string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" GIT_VERSION_MINOR "${GIT_VERSION}")
    string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" GIT_VERSION_PATCH "${GIT_VERSION}")
    string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" GIT_VERSION_SHA1 "${GIT_VERSION}")

    # Long version
    string(REGEX REPLACE "^v" "" ${ARGV0} "${GIT_VERSION}")

    # Short version
    set(${ARGV1} "${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}")

    # Symantic versioning
    set(${ARGV2} ${GIT_VERSION_MAJOR})
    set(${ARGV3} ${GIT_VERSION_MINOR})
    set(${ARGV4} ${GIT_VERSION_PATCH})
    set(${ARGV5} ${GIT_VERSION_SHA1})

endmacro()