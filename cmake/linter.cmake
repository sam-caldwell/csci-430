#! File: cmake/linter.cmake
#! Purpose: Define a single aggregate 'lint' target that:
#!   - Validates docstrings and test docstrings
#!   - Runs static organization checks
#!   - Runs clang-tidy over project sources (if available)
#! Notes:
#!   - Uses compile_commands.json from the build tree (-p ${CMAKE_BINARY_DIR}).
#!   - Falls back gracefully if clang-tidy is not installed.

include_guard(GLOBAL)

# Locate clang-tidy (prefer PATH). Allow override via CLANG_TIDY_EXE cache var.
# Prefer matching clang-tidy to the compiler toolchain (LLVM 17), fall back to PATH
find_program(CLANG_TIDY_EXE NAMES clang-tidy HINTS /opt/homebrew/opt/llvm@17/bin)
if(EXISTS "/opt/homebrew/opt/llvm@17/bin/clang-tidy")
  set(CLANG_TIDY_EXE "/opt/homebrew/opt/llvm@17/bin/clang-tidy" CACHE FILEPATH "clang-tidy executable" FORCE)
endif()

# Subtarget: run clang-tidy across sources (project-only)
if(CLANG_TIDY_EXE)
  # Collect lintable sources (exclude internal tooling and tests)
  file(GLOB_RECURSE LINT_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cc
    ${PROJECT_SOURCE_DIR}/src/*.cxx
    ${PROJECT_SOURCE_DIR}/src/*.cpp)
  list(REMOVE_DUPLICATES LINT_SOURCES)
  list(FILTER LINT_SOURCES EXCLUDE REGEX "/src/clang-tidy-.*")
  list(FILTER LINT_SOURCES EXCLUDE REGEX "/test/.*")

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

  add_custom_target(lint_clang_tidy DEPENDS ${_lint_stamps})
else()
  # No clang-tidy: create a noop subtarget to keep aggregate flow simple
  add_custom_target(lint_clang_tidy
    COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found; skipping clang-tidy checks."
    VERBATIM)
endif()

# Helper to add serialized lint steps (fail-fast ordering)
function(lint_add_step)
  set(options)
  set(oneValueArgs NAME)
  set(multiValueArgs DEPS)
  cmake_parse_arguments(LA "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(NOT LA_NAME)
    message(FATAL_ERROR "lint_add_step requires NAME")
  endif()
  add_custom_target(${LA_NAME})
  # Chain to previous step to serialize execution
  get_property(_prev GLOBAL PROPERTY LINT_LAST_STEP)
  if(_prev)
    add_dependencies(${LA_NAME} ${_prev})
  endif()
  foreach(_d IN LISTS LA_DEPS)
    if(TARGET ${_d})
      add_dependencies(${LA_NAME} ${_d})
    endif()
  endforeach()
  set_property(GLOBAL PROPERTY LINT_LAST_STEP ${LA_NAME})
endfunction()

# Define ordered steps: docstrings -> test docstrings -> one-func-per-file -> clang-tidy
lint_add_step(NAME lint_step_validate DEPS validate_docstrings)
lint_add_step(NAME lint_step_testdocs DEPS check_test_docstrings)
lint_add_step(NAME lint_step_onefunc DEPS check_one_function_per_file)
lint_add_step(NAME lint_step_clangtidy DEPS lint_clang_tidy)

# Final aggregate lint target depends only on the last step to ensure fail-fast
add_custom_target(lint)
get_property(_lint_last GLOBAL PROPERTY LINT_LAST_STEP)
if(_lint_last)
  add_dependencies(lint ${_lint_last})
endif()
