cmake_minimum_required(VERSION 3.19.0)

# Parses cmake -E environment output
# and applies it to the current process's environment
function(apply_env env_output)
    set(path_delimeter "__________")

    string(REPLACE ";" ${path_delimeter} env_output "${env_output}")
    string(REPLACE "\n" ";" env_output "${env_output}")

    foreach (line ${env_output})
        string(REPLACE "=" ";" line "${line}")

        list(GET line 0 env_var_name)
        list(GET line 1 env_var_value)

        string(REPLACE "${path_delimeter}" ";" env_var_value "${env_var_value}")
        set(ENV{${env_var_name}} "${env_var_value}")
    endforeach ()
endfunction()

# Applies MSVC dev environment to the current CMake process for given platform
# Example:
# - for x86: set_mscvc_env(Win32)
# - for x64: set_mscvc_env(x64)

function(set_msvc_env platform)
    if (${platform} STREQUAL Win32)
        set(platform x86)
    elseif (${platform} STREQUAL x64)
        set(platform x86_x64)
    endif ()

    if (NOT VSWHERE_PROGRAM)
        message(FATAL_ERROR "VSWHERE_PROGRAM not set")
    endif ()

    execute_process(COMMAND "${VSWHERE_PROGRAM}"
            -property installationPath

            OUTPUT_VARIABLE vs_installation_path
            OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY)

    string(REPLACE "\\" "/" vs_installation_path "${vs_installation_path}")
    set(vcvarsall_script "${vs_installation_path}/VC/Auxiliary/Build/vcvarsall.bat")

    set(printenv_script "${CMAKE_CURRENT_LIST_DIR}/__printenv.cmake")
    file(WRITE "${printenv_script}" "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E environment)")

    execute_process(
            COMMAND "${vcvarsall_script}" ${platform}>nul
            && cmake -P "${printenv_script}"

            OUTPUT_VARIABLE env_output
            OUTPUT_STRIP_TRAILING_WHITESPACE
            COMMAND_ERROR_IS_FATAL ANY)

    apply_env("${env_output}")

    file(REMOVE "${printenv_script}")
endfunction()

