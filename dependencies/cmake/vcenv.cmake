cmake_minimum_required(VERSION 3.19.0)

# Parses `cmake -E environment` output
# and applies it to the current process environment
function(apply_env env_output)
    set(path_delimeter "__________")

    string(REPLACE ";" ${path_delimeter} env_output "${env_output}")
    string(REPLACE "\n" ";" env_output "${env_output}")

    list(LENGTH env_output env_vars_count)
    string(LENGTH "${env_output}" env_output_length)

    message(STATUS "Applying environment: ${env_vars_count} variables, content-length: ${env_output_length}")

    foreach (line ${env_output})
        string(REPLACE "=" ";" line "${line}")

        list(GET line 0 env_var_name)
        list(GET line 1 env_var_value)

        string(REPLACE "${path_delimeter}" ";" env_var_value "${env_var_value}")
        set(ENV{${env_var_name}} "${env_var_value}")
    endforeach ()
endfunction()

# Saves cmake -E environment output to a variable
function(print_env output_var)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E environment
            OUTPUT_VARIABLE out
            COMMAND_ERROR_IS_FATAL ANY)

    set(${output_var} ${out} PARENT_SCOPE)
endfunction()

# Saves cmake -E environment output to a file
function(print_env_to_file output_file)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E environment
            OUTPUT_FILE "${output_file}"
            COMMAND_ERROR_IS_FATAL ANY)
endfunction()

# Removes all environment variables
function(unset_env)
    print_env(env_output)

    set(path_delimeter "__________")

    string(REPLACE ";" ${path_delimeter} env_output "${env_output}")
    string(REPLACE "\n" ";" env_output "${env_output}")

    list(LENGTH env_output env_vars_count)
    string(LENGTH "${env_output}" env_output_length)

    message(STATUS "Unsetting environment: ${env_vars_count} variables, content-length: ${env_output_length}")

    foreach (line ${env_output})
        string(REPLACE "=" ";" line "${line}")

        list(GET line 0 env_var_name)

        unset(ENV{${env_var_name}})
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
    set(printenv_output_file "${CMAKE_CURRENT_LIST_DIR}/__printenv_${platform}")

    file(WRITE "${printenv_script}" [[
        execute_process(COMMAND "${CMAKE_COMMAND}" -E environment)
    ]])

    execute_process(COMMAND "${vcvarsall_script}" ${platform} > nul
            && "${CMAKE_COMMAND}" -P "${printenv_script}"
            OUTPUT_VARIABLE printenv_output
            COMMAND_ERROR_IS_FATAL ANY)

    apply_env("${printenv_output}")

    file(REMOVE "${printenv_script}")
endfunction()

