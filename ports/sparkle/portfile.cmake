vcpkg_download_distfile(
    ARCHIVE
    URLS "https://github.com/sparkle-project/Sparkle/releases/download/1.24.0/Sparkle-1.24.0.tar.xz"
    FILENAME "Sparkle-1.24.0.tar.xz"
    SHA512 "3c42f420da7861d2719ab320c151066a0695ae93da38256fd8709b08827a46ee2272958bab5854e94383eb69e4535b7ef9ef746d81fc1cb601ad7edb54f1aafb"
)

vcpkg_extract_source_archive(
    SOURCE_PATH
    ARCHIVE "${ARCHIVE}"
    NO_REMOVE_ONE_LEVEL
)

# Install the framework
file(INSTALL "${SOURCE_PATH}/Sparkle.framework" DESTINATION "${CURRENT_PACKAGES_DIR}/Frameworks")
file(INSTALL "${SOURCE_PATH}/Sparkle.framework" DESTINATION "${CURRENT_PACKAGES_DIR}/debug/Frameworks")

# Install headers
file(INSTALL "${SOURCE_PATH}/Sparkle.framework/Headers" DESTINATION "${CURRENT_PACKAGES_DIR}/include/Sparkle")
file(INSTALL "${SOURCE_PATH}/Sparkle.framework/Headers" DESTINATION "${CURRENT_PACKAGES_DIR}/debug/include/Sparkle")

# Install license
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/sparkle" RENAME copyright)

# Configure CMake config
set(Sparkle_FRAMEWORK "${CURRENT_PACKAGES_DIR}/Frameworks/Sparkle.framework")
set(Sparkle_INCLUDE_DIR "${CURRENT_PACKAGES_DIR}/include")
configure_file("${CMAKE_CURRENT_LIST_DIR}/sparkle-config.cmake.in" "${CURRENT_PACKAGES_DIR}/share/sparkle/sparkle-config.cmake" @ONLY)
