# tools/check_test_docstrings.cmake
# -*- cmake -*-
#
# check_test_docstrings.cmake
#
# Ensures that under test/:
#   - Each C++ test file contains exactly one GoogleTest TEST(...) block.
#   - A docstring comment immediately above the TEST(...) contains either:
#       (Strict)  Test:, Inputs:, Code under test:, Expected behavior:
#       (Legacy)  Test Suite:  and (Purpose: or Components Under Test:)
#
# Prints a report of any violations and exits non-zero if any are found.
#
# Usage:
#   cmake -DPROJECT_ROOT=/path/to/repo -P tools/check_test_docstrings.cmake
#
# If PROJECT_ROOT is not provided, uses CMAKE_SOURCE_DIR (if available) or $PWD.

if(NOT PROJECT_ROOT)
    if(DEFINED CMAKE_SOURCE_DIR)
        set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
    else()
        file(TO_CMAKE_PATH "$ENV{PWD}" PROJECT_ROOT)
    endif()
endif()

set(TEST_DIR "${PROJECT_ROOT}/test")

# Regexes (CMake uses a POSIX/PCRE-like engine; these work multiline)
set(TEST_LINE_RE "^[ \t]*TEST\\s*\\(")         # find TEST(...) at line start
set(TEST_CAPTURE_RE "([ \t]*)TEST\\s*\\(")     # capture full matched text for indexing
set(BLOCK_RE "/\\*((.|\\n)*?)\\*/")            # non-greedy C-style block comment

# --------------------------- Helpers ----------------------------------------
#
# Check if a comment body satisfies strict or legacy fields (case-insensitive).
# Args:
#    BLOCK_BODY: string inside /* ... */
#    OUT_BOOL:   output var set to TRUE if compliant, else FALSE
function(_has_required_fields BLOCK_BODY OUT_BOOL)
    string(TOLOWER "${BLOCK_BODY}" _lower)

    # Strict fields
    set(_has_all TRUE)
    foreach(_tok "test:" "inputs:" "code under test:" "expected behavior:")
        if(NOT _lower MATCHES "([\\r\\n]|^)[ \\t]*${_tok}")
            set(_has_all FALSE)
            break()
        endif()
    endforeach()

    if(_has_all)
        set(${OUT_BOOL} TRUE PARENT_SCOPE)
        return()
    endif()

    # Legacy: Test Suite: and (Purpose: or Components Under Test:)
    set(_has_suite FALSE)
    if(_lower MATCHES "([\\r\\n]|^)[ \\t]*test suite[ \\t]*:")
        set(_has_suite TRUE)
    endif()
    set(_has_purpose FALSE)
    if(_lower MATCHES "([\\r\\n]|^)[ \\t]*purpose[ \\t]*:")
        set(_has_purpose TRUE)
    endif()
    set(_has_components FALSE)
    if(_lower MATCHES "([\\r\\n]|^)[ \\t]*components under test[ \\t]*:")
        set(_has_components TRUE)
    endif()

    if(_has_suite AND (_has_purpose OR _has_components))
        set(${OUT_BOOL} TRUE PARENT_SCOPE)
    else()
        set(${OUT_BOOL} FALSE PARENT_SCOPE)
    endif()
endfunction()

#
# Return TRUE if there exists a compliant /* ... */ block comment
# *before* TEST_POS in TEXT. Scans from nearest upwards.
#
function(_has_required_docstring_above TEXT TEST_POS OUT_BOOL)

    # Prefix up to TEST(...)
    string(SUBSTRING "${TEXT}" 0 ${TEST_POS} _prefix)

    # All block comments in prefix
    string(REGEX MATCHALL "${BLOCK_RE}" _comments "${_prefix}")
    if(NOT _comments)
        set(${OUT_BOOL} FALSE PARENT_SCOPE)
        return()
    endif()

    # Iterate from the last block upward
    list(LENGTH _comments _ncomm)
    math(EXPR _last_idx "${_ncomm}-1")
    foreach(_i RANGE ${_last_idx} 0 -1)
        list(GET _comments ${_i} _full)
        # Strip /* and */ to get body
        string(REGEX REPLACE "^/\\*|\\*/$" "" _body "${_full}")
        _has_required_fields("${_body}" _ok)
        if(_ok)
            set(${OUT_BOOL} TRUE PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${OUT_BOOL} FALSE PARENT_SCOPE)
endfunction()

# --------------------------- Main -------------------------------------------

if(NOT EXISTS "${TEST_DIR}")
    message(FATAL_ERROR "Test directory not found: ${TEST_DIR}")
endif()

set(_offenders "")   # aggregated lines "<path>: <msg>"
file(GLOB_RECURSE _cpp_files RELATIVE "${PROJECT_ROOT}" "${TEST_DIR}/*.cpp")

foreach(_rel ${_cpp_files})
    set(_abs "${PROJECT_ROOT}/${_rel}")
    file(READ "${_abs}" _text)

    # Find all TEST(...) occurrences by line
    string(REGEX MATCHALL "${TEST_LINE_RE}" _test_lines "${_text}")
    list(LENGTH _test_lines _ntests)

    if(_ntests EQUAL 0)
        continue()
    endif()

    if(NOT _ntests EQUAL 1)
        list(APPEND _offenders "- ${_abs}: contains ${_ntests} TEST(...) blocks (expected 1)")
        continue()
    endif()

    # Extract the first (and only) TEST match and its index
    string(REGEX MATCH "${TEST_CAPTURE_RE}" _first "${_text}")
    if("${_first}" STREQUAL "")
        # Shouldn't happen if _ntests == 1, but be defensive
        list(APPEND _offenders "- ${_abs}: unable to locate TEST(...) position")
        continue()
    endif()

    # TEST position for prefix slicing
    string(FIND "${_text}" "${_first}" _test_pos)
    if(_test_pos LESS 0)
        list(APPEND _offenders "- ${_abs}: unable to compute TEST(...) position")
        continue()
    endif()

    # Docstring check directly above TEST(...)
    _has_required_docstring_above("${_text}" ${_test_pos} _has_doc)
    if(NOT _has_doc)
        list(APPEND _offenders "- ${_abs}: missing required docstring immediately above TEST(...)")
    endif()
endforeach()

# Reporting & exit code
if(_offenders)
    # Build a single report string so we can fail with one fatal message.
    string(JOIN "\n" _report_line ${_offenders})
    set(_report "Docstring/structure check found issues:\n\n${_report_line}")
    message("${_report}")          # print the report
    message(FATAL_ERROR "Docstring check failed.")  # non-zero exit
else()
    message(STATUS "All tests pass docstring and single-TEST checks.")
endif()

