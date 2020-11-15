function(git_get_repo_name _var)
    execute_process(COMMAND git config --get remote.origin.url
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT_VARIABLE out
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(REGEX REPLACE "^.+\\.com/" "" out "${out}")
    string(REGEX REPLACE ".git$" "" out "${out}")

    set(${_var} "${out}" PARENT_SCOPE)
endfunction()