# cmake/tools/check_one_function_per_file.cmake
# -*- cmake -*-
#
# Static check: ensure each .cpp file under src/basic_compiler/
# defines at most one function/method. Approximates function definitions
# by matching common signature forms that end with '{' on the same line.
#
# Usage:
#   cmake -DPROJECT_ROOT=/path/to/repo -P cmake/tools/check_one_function_per_file.cmake
#

if(NOT PROJECT_ROOT)
    if(DEFINED CMAKE_SOURCE_DIR)
        set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
    else()
        file(TO_CMAKE_PATH "$ENV{PWD}" PROJECT_ROOT)
    endif()
endif()

set(SRC_DIR "${PROJECT_ROOT}/src/basic_compiler")
if(NOT EXISTS "${SRC_DIR}")
    message(STATUS "Source directory not found for check: ${SRC_DIR}. Skipping.")
    return()
endif()

# Match start-of-line using (^|\r|\n) to emulate multiline anchors.
# Use a bracket-quoted string to avoid CMake treating backslashes as escapes (e.g., "\{").
set(FUNC_RE [[(\r|\n|^)[ \t]*(template[ \t]*<[^>]+>[ \t]*)?(inline[ \t]+)?(static[ \t]+)?(constexpr[ \t]+)?(void|bool|int|double|float|size_t|uint64_t|int64_t|std::[A-Za-z_][A-Za-z0-9_]*|[A-Za-z_][A-Za-z0-9_]*::[A-Za-z_][A-Za-z0-9_]*)[ \t]+[*& \t]*~?[A-Za-z_][A-Za-z0-9_:<>]*[ \t]*\([^;{}\n]*\)[ \t]*(const)?[ \t]*(noexcept)?[ \t]*\{]])

file(GLOB_RECURSE _cpp_files RELATIVE "${PROJECT_ROOT}" "${SRC_DIR}/*.cpp")

set(_violations)
foreach(_rel IN LISTS _cpp_files)
    set(_abs "${PROJECT_ROOT}/${_rel}")
    file(READ "${_abs}" _text)
    string(REGEX MATCHALL "${FUNC_RE}" _matches "${_text}")
    list(LENGTH _matches _n)
    if(_n GREATER 1)
        list(APPEND _violations "${_abs}:${_n}")
    endif()
endforeach()

if(_violations)
    message("Error: found files with more than one function/method definition:")
    foreach(v IN LISTS _violations)
        message("  ${v}")
    endforeach()
    message("Hint: project convention is one function/method per .cpp file under src/basic_compiler/.")
    message(FATAL_ERROR "one-function-per-file check failed.")
else()
    message(STATUS "One-function-per-file check: OK")
endif()
