#! File: cmake/linter.cmake
#! Purpose: Define a CMake-driven 'lint' target that runs clang-tidy
#!          over project sources in parallel via the build system (Ninja).
#! Notes:
#!  - Uses compile_commands.json from the build tree (-p ${CMAKE_BINARY_DIR}).
#!  - Falls back gracefully if clang-tidy is not installed.

include_guard(GLOBAL)

# Locate clang-tidy (prefer PATH). Allow override via CLANG_TIDY_EXE cache var.
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# Collect lintable sources (project sources only; exclude tests)
file(GLOB_RECURSE LINT_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/*.cc
  ${PROJECT_SOURCE_DIR}/src/*.cxx
  ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_DUPLICATES LINT_SOURCES)

# Default checks are controlled by the repository .clang-tidy
# (no command-line override here; lint uses project policy as-is)

if(NOT CLANG_TIDY_EXE)
  # clang-tidy missing: create a dummy 'lint' that prints a message and succeeds
  add_custom_target(lint
    COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found; skipping lint."
    VERBATIM)
  # Ensure the docstring checker is built before any linting activity
  if(TARGET clang_tidy_docstring)
    add_dependencies(lint clang_tidy_docstring)
  endif()
  if(TARGET validate_docstrings)
    add_dependencies(lint validate_docstrings)
  endif()
  return()
endif()

# Directory to hold per-file stamp outputs
set(LINT_OUT_DIR "${CMAKE_BINARY_DIR}/.lint")

set(_lint_stamps)
foreach(_src IN LISTS LINT_SOURCES)
  get_filename_component(_abs "${_src}" ABSOLUTE)
  string(MD5 _hash "${_abs}")
  set(_stamp "${LINT_OUT_DIR}/${_hash}.ok")

  add_custom_command(
    OUTPUT "${_stamp}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LINT_OUT_DIR}"
    COMMAND ${CLANG_TIDY_EXE}
            -p "${CMAKE_BINARY_DIR}"
            -quiet
            "${_abs}"
    COMMAND ${CMAKE_COMMAND} -E touch "${_stamp}"
    DEPENDS "${_abs}"
    COMMENT "clang-tidy ${_src}"
    VERBATIM)

  list(APPEND _lint_stamps "${_stamp}")
endforeach()

add_custom_target(lint DEPENDS ${_lint_stamps})

# Ensure the docstring checker binary is available prior to linting
if(TARGET clang_tidy_docstring)
  add_dependencies(lint clang_tidy_docstring)
endif()
if(TARGET validate_docstrings)
  add_dependencies(lint validate_docstrings)
endif()
