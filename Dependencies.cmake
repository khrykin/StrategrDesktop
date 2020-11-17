set(CMAKE_PROJECT_NAME Strategr)

set(PACKAGES
        qt5
        boost-uuid
        nlohmann-json
        catch2
        utf8proc)

if (WIN32)
    set(TRIPLETS
            x64-windows-static
            x86-windows-static)
endif ()
