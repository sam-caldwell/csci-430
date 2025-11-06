# Usage: cmake --build . --target check_one_function_per_file
add_custom_target(check_one_function_per_file
        COMMAND ${CMAKE_COMMAND} -Wdev -Werror=dev
            -DPROJECT_ROOT=${CMAKE_SOURCE_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/tools/check_one_function_per_file.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking one-function-per-file convention in basic_compiler sources"
        VERBATIM)
