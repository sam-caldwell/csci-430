#! File: cmake/linter.cmake
#! Purpose: Define a CMake-driven 'lint' target that:
#!  - Globs all .h and .cpp files in the repo (src/ and include/)
#!  - Runs clang-tidy (configured by .clang-tidy) against each file
#!  - Executes sequentially and fails fast on the first warning/error
#! Notes:
#!  - Uses compile_commands.json from the build tree (-p ${CMAKE_BINARY_DIR}).
#!  - Treats all warnings as errors via -warnings-as-errors=*

include_guard(GLOBAL)

# Locate clang-tidy (prefer PATH). Allow override via CLANG_TIDY_EXE cache var.
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# Collect lintable sources: headers and C++ source files only, per request
file(GLOB_RECURSE LINT_HEADERS CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/include/*.h
     ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE LINT_CPPS CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(LINT_FILES ${LINT_HEADERS} ${LINT_CPPS})
list(REMOVE_DUPLICATES LINT_FILES)
list(SORT LINT_FILES)

if(NOT CLANG_TIDY_EXE)
    add_custom_target(lint
        COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found; skipping lint."
        VERBATIM)
    return()
endif()

# Emit a small CMake script that iterates files and runs clang-tidy sequentially.
set(LINT_DIR "${CMAKE_BINARY_DIR}/.lint")
file(MAKE_DIRECTORY "${LINT_DIR}")
set(LINT_SCRIPT "${LINT_DIR}/run_clang_tidy_failfast.cmake")

file(WRITE "${LINT_SCRIPT}" "# Auto-generated: run clang-tidy sequentially and fail fast\n")
file(APPEND "${LINT_SCRIPT}" "cmake_minimum_required(VERSION 3.16)\n")
file(APPEND "${LINT_SCRIPT}" "set(CLANG_TIDY_EXE \"${CLANG_TIDY_EXE}\")\n")
file(APPEND "${LINT_SCRIPT}" "set(BINARY_DIR \"${CMAKE_BINARY_DIR}\")\n")
file(APPEND "${LINT_SCRIPT}" "set(FILES\n")
foreach(_f IN LISTS LINT_FILES)
    file(APPEND "${LINT_SCRIPT}" "  \"${_f}\"\n")
endforeach()
file(APPEND "${LINT_SCRIPT}" ")\n")

file(APPEND "${LINT_SCRIPT}" [=[
foreach(f IN LISTS FILES)
  message(STATUS "clang-tidy: ${f}")
  execute_process(
    COMMAND "${CLANG_TIDY_EXE}" -p "${BINARY_DIR}" "-warnings-as-errors=*" "${f}"
    RESULT_VARIABLE rv
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE)
  if(NOT rv EQUAL 0)
    message(STATUS "clang-tidy output:
${out}
${err}")
    message(FATAL_ERROR "clang-tidy failed for: ${f}")
  endif()
endforeach()
]=])

add_custom_target(lint
    COMMAND ${CMAKE_COMMAND} -P "${LINT_SCRIPT}"
    USES_TERMINAL
    VERBATIM)
