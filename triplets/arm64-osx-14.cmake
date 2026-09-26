set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES arm64)
set(VCPKG_OSX_DEPLOYMENT_TARGET 14.0)

# Newer Xcode/macOS SDKs report __has_builtin(__yield) as true without
# declaring it, which trips qtbase's qyieldcpu.h under -Werror.
set(VCPKG_C_FLAGS "-Wno-error=implicit-function-declaration")
set(VCPKG_CXX_FLAGS "-Wno-error=implicit-function-declaration")
