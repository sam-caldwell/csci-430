# File: cmake/projects/clang_tidy_one_func.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Define the one-function-per-file checker library and CLI, plus a validation target.

file(GLOB CLANG_TIDY_ONEFUNC_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/clang-tidy-one-func/*.cpp)

set(_onefunc_lib_sources)
foreach(_s IN LISTS CLANG_TIDY_ONEFUNC_SOURCES)
  get_filename_component(_name "${_s}" NAME)
  if(NOT _name STREQUAL "main.cpp")
    list(APPEND _onefunc_lib_sources "${_s}")
  endif()
endforeach()

add_library(clang_tidy_one_func_lib STATIC ${_onefunc_lib_sources})
target_include_directories(clang_tidy_one_func_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(clang_tidy_one_func ${PROJECT_SOURCE_DIR}/src/clang-tidy-one-func/main.cpp)
target_include_directories(clang_tidy_one_func PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(clang_tidy_one_func PRIVATE clang_tidy_one_func_lib)

# Validation target: run the checker over compiler sources
add_custom_target(validate_one_function_per_file
  COMMAND $<TARGET_FILE:clang_tidy_one_func> -d ${PROJECT_SOURCE_DIR}/src/basic_compiler
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Validating one-function-per-file convention"
  USES_TERMINAL
  VERBATIM)

add_dependencies(validate_one_function_per_file clang_tidy_one_func)

