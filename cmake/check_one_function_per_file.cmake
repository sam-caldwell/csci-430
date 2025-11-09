# Usage: cmake --build . --target check_one_function_per_file
# Refactored to use the clang-tidy-style checker binary.
add_custom_target(check_one_function_per_file
        COMMAND $<TARGET_FILE:clang_tidy_one_func> -d ${CMAKE_SOURCE_DIR}/src/basic_compiler
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking one-function-per-file convention in basic_compiler sources"
        VERBATIM)
add_dependencies(check_one_function_per_file clang_tidy_one_func)
