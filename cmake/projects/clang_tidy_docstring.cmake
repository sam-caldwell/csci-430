# File: cmake/projects/clang_tidy_docstring.cmake
# (c) 2025 Sam Caldwell. All Rights Reserved.
# Purpose: Define the clang-tidy-docstring library and CLI, plus a validation target.

file(GLOB CLANG_TIDY_DOCSTRING_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/clang-tidy-docstring/*.cpp)

# Library (exclude main.cpp from the library; build CLI separately)
set(_lib_sources)
foreach(_s IN LISTS CLANG_TIDY_DOCSTRING_SOURCES)
  get_filename_component(_name "${_s}" NAME)
  if(NOT _name STREQUAL "main.cpp")
    list(APPEND _lib_sources "${_s}")
  endif()
endforeach()

add_library(clang_tidy_docstring_lib STATIC ${_lib_sources})
target_include_directories(clang_tidy_docstring_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(clang_tidy_docstring ${PROJECT_SOURCE_DIR}/src/clang-tidy-docstring/main.cpp)
target_include_directories(clang_tidy_docstring PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(clang_tidy_docstring PRIVATE clang_tidy_docstring_lib)

# Validation target: run the checker over compiler sources only
# Limit scope to basic_compiler to avoid linting the checker and unrelated samples.
add_custom_target(validate_docstrings
  COMMAND $<TARGET_FILE:clang_tidy_docstring>
          -d ${PROJECT_SOURCE_DIR}/src/basic_compiler/lexer
          -d ${PROJECT_SOURCE_DIR}/src/basic_compiler/parser
          -d ${PROJECT_SOURCE_DIR}/src/basic_compiler/codegen
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Validating docstrings for functions/methods (clang-tidy-docstring)"
  USES_TERMINAL
  VERBATIM)

add_dependencies(validate_docstrings clang_tidy_docstring)
