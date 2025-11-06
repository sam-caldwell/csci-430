# tools/add_test_docstrings.cmake
# -*- cmake -*-
#
# add_test_docstrings.cmake
#
# Scans C++ tests under test/ and, for any file missing a compliant
# docstring block immediately before the GoogleTest TEST(...) block,
# inserts a standard docstring with the required fields:
#
#   Test:, Inputs:, Code under test:, Expected behavior:
#
# Existing files that already contain a compliant block are left unchanged.
# Files with multiple TEST(...) blocks are reported but not modified.
#
# Usage:
#   cmake -DPROJECT_ROOT=/path/to/repo -P tools/add_test_docstrings.cmake
#
# If PROJECT_ROOT is not provided, this script uses CMAKE_SOURCE_DIR (when
# invoked from a CMakeLists.txt) or the current working directory.

if(NOT PROJECT_ROOT)
    if(DEFINED CMAKE_SOURCE_DIR)
        set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
    else()
        file(TO_CMAKE_PATH "$ENV{PWD}" PROJECT_ROOT)
    endif()
endif()

set(TEST_DIR "${PROJECT_ROOT}/test")

# Regexes
set(TEST_RE "([ \\t]*)TEST\\s*\\(\\s*([^, \\t\\n]+)\\s*,\\s*([^\\)\\n]+)\\)")
set(BLOCK_RE "/\\*((.|\\n)*?)\\*/")

# --------------------------- Helpers ----------------------------------------
#Return heuristics tuple (inputs, code_under_test, expected) based on file path.
#
#Args:
#PATH: Absolute or relative file path to the test .cpp
#OUT_INPUTS, OUT_CODE, OUT_EXPECTED: Output variable names to receive strings
function(_classify PATH OUT_INPUTS OUT_CODE OUT_EXPECTED)
    set(p "${PATH}")

    if(p MATCHES "/basic_compiler/e2e/")
        set(inputs "BASIC program(s) executed end-to-end (runtime output)")
        set(code   "Full compiler pipeline (lexer → parser → semantics → codegen → runtime)")
        set(exp    "Program compiles and runs; output/behavior matches expectations")
    elseif(p MATCHES "/basic_compiler/integration/")
        set(inputs "BASIC snippet compiled through multiple stages")
        set(code   "Parser + Semantics + Codegen integration")
        set(exp    "Emitted IR/state contains expected constructs and values")
    elseif(p MATCHES "/basic_compiler/unit/")
        get_filename_component(fname "${PATH}" NAME)
        if(fname MATCHES "test_lexer" OR p MATCHES "/lexer/")
            set(inputs "Raw source text and helper inputs")
            set(code   "Lexer/tokenization and helpers")
            set(exp    "Tokens/escapes match expectations; errors are reported appropriately")
        elseif(fname MATCHES "test_parser" OR p MATCHES "/parser/")
            set(inputs "BASIC source snippet")
            set(code   "Parser (BASIC → AST)")
            set(exp    "AST structure or parse errors match expectations")
        elseif(fname MATCHES "test_semantics" OR p MATCHES "/semantics/")
            set(inputs "Parsed AST (from BASIC snippet) and default environment")
            set(code   "Semantics analyzer (type/arity/domain checks)")
            set(exp    "Valid programs accepted; invalid ones produce expected semantic errors")
        elseif(fname MATCHES "test_codegen" OR p MATCHES "/codegen/")
            set(inputs "AST (and semantic info) from BASIC snippet")
            set(code   "LLVM IR code generator")
            set(exp    "Emits expected IR calls/ops; unsupported cases are reported")
        else()
            set(inputs "See test body")
            set(code   "Relevant module(s) under test")
            set(exp    "Asserts expected results/behavior described in test")
        endif()
    elseif(p MATCHES "/logger/")
        set(inputs "Filesystem paths, log messages, toggles")
        set(code   "Logger component")
        set(exp    "Creates directories, writes/appends as expected, handles errors")
    elseif(p MATCHES "/hello_world/")
        set(inputs "None")
        set(code   "hello_world function")
        set(exp    "Returns expected string")
    else()
        set(inputs "See test body")
        set(code   "Relevant module(s) under test")
        set(exp    "Asserts expected results/behavior described in test")
    endif()

    set(${OUT_INPUTS}  "${inputs}" PARENT_SCOPE)
    set(${OUT_CODE}    "${code}"   PARENT_SCOPE)
    set(${OUT_EXPECTED} "${exp}"   PARENT_SCOPE)
endfunction()

function(_has_required_fields BLOCK_BODY OUT_BOOL)
    # Case-insensitive check for required fields inside a C-style block comment.
    #
    # Args:
    #     BLOCK_BODY: Text between /* and */
    #     OUT_BOOL: Output var name set to TRUE or FALSE
    string(TOLOWER "${BLOCK_BODY}" lower)
    set(ok TRUE)
    foreach(token "test:" "inputs:" "code under test:" "expected behavior:")
        if(NOT lower MATCHES "[\\n\\r^][ \\t]*${token}")
            set(ok FALSE)
            break()
        endif()
    endforeach()
    set(${OUT_BOOL} ${ok} PARENT_SCOPE)
endfunction()

# Build the standardized docstring given TEST suite and name and file path.
#
# Args:
#   SUITE: GoogleTest suite name
#   NAME:  GoogleTest test name
#   PATH:  File path for heuristic classification
#   OUT_DOC: Output var name receiving the final docstring (with trailing newline)
function(_build_docstring SUITE NAME PATH OUT_DOC)
    _classify("${PATH}" inputs code expected)
    set(doc "/*\nTest: ${SUITE}.${NAME}\nInputs: ${inputs}\nCode under test: ${code}\nExpected behavior: ${expected}\n*/\n")
    set(${OUT_DOC} "${doc}" PARENT_SCOPE)
endfunction()

function(_process_file FILEPATH OUT_MODIFIED)
    # Insert a docstring if missing for a file containing exactly one TEST(...) block.
    #
    # Behavior:
    #    - If the last block comment immediately before TEST(...) already contains
    #    the required fields, leave file unchanged.
    #    - Otherwise, insert the generated block above the TEST(...) line.
    #
    # Args:
    #    FILEPATH: Path to a .cpp file
    #    OUT_MODIFIED: Output var name set to 1 if file changed, else 0
    set(mod 0)
    file(READ "${FILEPATH}" text)

    # Count TEST blocks
    string(REGEX MATCHALL "${TEST_RE}" matches "${text}")
    list(LENGTH matches ntests)
    if(ntests EQUAL 0)
        set(${OUT_MODIFIED} 0 PARENT_SCOPE)
        return()
    endif()
    if(NOT ntests EQUAL 1)
        # Caller records multi-TEST files for reporting.
        set(${OUT_MODIFIED} 0 PARENT_SCOPE)
        return()
    endif()

    # Extract first TEST match and its position.
    string(REGEX MATCH "${TEST_RE}" first_match "${text}")
    string(FIND "${text}" "${first_match}" test_pos)

    # Everything before TEST(...)
    string(SUBSTRING "${text}" 0 ${test_pos} prefix)

    # Find the last block comment in prefix.
    string(REGEX MATCHALL "${BLOCK_RE}" comments "${prefix}")
    set(has_compliant FALSE)
    if(comments)
        list(LENGTH comments ncomm)
        math(EXPR last_idx "${ncomm}-1")
        list(GET comments ${last_idx} last_full)
        # Strip /* and */ to get body
        string(REGEX REPLACE "^/\\*|\\*/$" "" last_body "${last_full}")
        _has_required_fields("${last_body}" has_compliant)
    endif()

    if(has_compliant)
        set(${OUT_MODIFIED} 0 PARENT_SCOPE)
        return()
    endif()

    # Build docstring and insert above TEST(...)
    # Capture groups from first TEST
    # CMAKE_MATCH_0..3 are populated by last REGEX MATCH
    set(indent "${CMAKE_MATCH_1}")
    set(suite  "${CMAKE_MATCH_2}")
    set(name   "${CMAKE_MATCH_3}")

    _build_docstring("${suite}" "${name}" "${FILEPATH}" doc)

    # Insert without disturbing indentation; doc itself is left-aligned as in Python version.
    string(SUBSTRING "${text}" ${test_pos} -1 suffix)
    set(new_text "${prefix}${doc}${suffix}")
    file(WRITE "${FILEPATH}" "${new_text}")
    set(${OUT_MODIFIED} 1 PARENT_SCOPE)
endfunction()

# --------------------------- Main -------------------------------------------

if(NOT EXISTS "${TEST_DIR}")
    message(FATAL_ERROR "Test directory not found: ${TEST_DIR}")
endif()

set(modified 0)
set(multi_test_files "")
file(GLOB_RECURSE cpp_files RELATIVE "${PROJECT_ROOT}" "${TEST_DIR}/*.cpp")

foreach(rel ${cpp_files})
    set(abs "${PROJECT_ROOT}/${rel}")
    file(READ "${abs}" _content)
    # Count TEST occurrences first
    string(REGEX MATCHALL "${TEST_RE}" _tests "${_content}")
    list(LENGTH _tests _ntests)
    if(_ntests EQUAL 0)
        continue()
    elseif(NOT _ntests EQUAL 1)
        list(APPEND multi_test_files "${abs} (${_ntests})")
        continue()
    endif()

    _process_file("${abs}" changed)
    if(changed)
        math(EXPR modified "${modified}+1")
    endif()
endforeach()

if(multi_test_files)
    message(WARNING "Files with multiple TEST(...) (not modified):")
    foreach(x IN LISTS multi_test_files)
        message(WARNING "  - ${x}")
    endforeach()
endif()

message(STATUS "Added/updated docstrings in ${modified} file(s).")
