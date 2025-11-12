#! File: cmake/run_clang_tidy_failfast.cmake
#! Purpose: Run clang-tidy sequentially (fail-fast) over a set of files,
#!          optionally filtered to a subdirectory relative to the repo root.

cmake_minimum_required(VERSION 3.16)

# Inputs (pass via -DVAR=...):
#  - CLANG_TIDY_EXE: optional explicit path to clang-tidy
#  - ROOT_DIR: repository root (defaults to parent of this script's dir)
#  - BINARY_DIR: build dir containing compile_commands.json (required)
#  - FILTER_DIR: optional path under ROOT_DIR to restrict files

if(NOT DEFINED CLANG_TIDY_EXE OR CLANG_TIDY_EXE STREQUAL "")
  find_program(CLANG_TIDY_EXE NAMES clang-tidy)
endif()
if(NOT CLANG_TIDY_EXE)
  message(FATAL_ERROR "clang-tidy not found; set CLANG_TIDY_EXE or add to PATH")
endif()

# Default ROOT_DIR to repo root (parent of this script's directory)
if(NOT DEFINED ROOT_DIR OR ROOT_DIR STREQUAL "")
  get_filename_component(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
endif()

if(NOT DEFINED BINARY_DIR OR BINARY_DIR STREQUAL "")
  message(FATAL_ERROR "BINARY_DIR not set (expected compile_commands.json in build dir)")
endif()

# Resolve sysroot on macOS so libc headers resolve under clang-tidy
set(_LINT_SYSROOT "")
if(APPLE)
  execute_process(
    COMMAND xcrun --show-sdk-path
    OUTPUT_VARIABLE _LINT_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
endif()

# Collect candidate files (headers and C++ sources)
file(GLOB_RECURSE _hdrs CONFIGURE_DEPENDS
     "${ROOT_DIR}/include/*.h"
     "${ROOT_DIR}/src/*.h")
file(GLOB_RECURSE _cpps CONFIGURE_DEPENDS
     "${ROOT_DIR}/src/*.cpp")
set(LINT_FILES ${_hdrs} ${_cpps})
list(REMOVE_DUPLICATES LINT_FILES)
list(SORT LINT_FILES)

# Optional prefix filter: keep only files under FILTER_DIR
if(DEFINED FILTER_DIR AND NOT FILTER_DIR STREQUAL "")
  if(IS_ABSOLUTE "${FILTER_DIR}")
    set(_prefix "${FILTER_DIR}")
  else()
    set(_prefix "${ROOT_DIR}/${FILTER_DIR}")
  endif()
  # Normalize prefix (remove trailing slashes)
  string(REGEX REPLACE "/+$" "" _prefix "${_prefix}")
  set(_filtered)
  foreach(f IN LISTS LINT_FILES)
    string(LENGTH "${_prefix}" _plen)
    if(_plen GREATER 0)
      string(SUBSTRING "${f}" 0 ${_plen} _head)
      if(_head STREQUAL "${_prefix}")
        list(APPEND _filtered "${f}")
      endif()
    endif()
  endforeach()
  set(LINT_FILES ${_filtered})
endif()

if(NOT LINT_FILES)
  message(STATUS "No files to lint (filter='${FILTER_DIR}')")
  return()
endif()

# Extra arguments for clang-tidy invocation
set(_EXTRA_ARGS
  -p "${BINARY_DIR}"
  -warnings-as-errors=*
)
if(NOT "${_LINT_SYSROOT}" STREQUAL "")
  list(APPEND _EXTRA_ARGS --extra-arg=-isysroot --extra-arg="${_LINT_SYSROOT}")
endif()

foreach(f IN LISTS LINT_FILES)
  message(STATUS "clang-tidy: ${f}")
  execute_process(
    COMMAND "${CLANG_TIDY_EXE}" ${_EXTRA_ARGS} "${f}"
    RESULT_VARIABLE rv
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE)
  if(NOT rv EQUAL 0)
    message(STATUS "clang-tidy output:\n${out}\n${err}")
    message(FATAL_ERROR "clang-tidy failed for: ${f}")
  endif()
endforeach()

