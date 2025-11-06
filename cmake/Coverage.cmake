#! File: cmake/Coverage.cmake
#! Purpose: Provide a CMake-driven 'coverage' target that builds tests with
#!          coverage instrumentation and generates coverage reports, enforcing
#!          a threshold. Designed to be invoked via: cmake --build ... --target coverage

include_guard(GLOBAL)

# Configuration knobs (override at configure time with -D...)
set(COVERAGE_MIN "95" CACHE STRING "Minimum percent (Lines/Regions) to pass")
set(COVERAGE_SCOPE "src/basic_compiler/" CACHE STRING "Path prefix to report/aggregate")
# Default scope: include all compiler sources only; exclude tests from coverage requirements.
# Include CLI helpers, usage, and the CLI entrypoint (main.cpp) in coverage by default.
set(COVERAGE_INCLUDE_RE "^(src/|include/)" CACHE STRING "egrep regex to include rows")
set(COVERAGE_EXCLUDE_RE "" CACHE STRING "egrep regex to exclude rows")
set(COVERAGE_METRIC "lines" CACHE STRING "lines|regions|both (both requires both >= min)")

# Test executables (from this project)
set(UNIT_BIN "${CMAKE_BINARY_DIR}/basic_compiler_unit_tests")
set(INT_BIN  "${CMAKE_BINARY_DIR}/basic_compiler_integration_tests")
set(E2E_BIN  "${CMAKE_BINARY_DIR}/basic_compiler_e2e_tests")
# Logger test executables (include to capture src/logger/ coverage)
set(LUNIT_BIN "${CMAKE_BINARY_DIR}/logger_unit_tests")
set(LINT_BIN  "${CMAKE_BINARY_DIR}/logger_integration_tests")
set(LE2E_BIN  "${CMAKE_BINARY_DIR}/logger_e2e_tests")
# Compiler CLI binary (include for coverage mapping of main.cpp)
set(CLI_BIN   "${CMAKE_BINARY_DIR}/basic_compiler/basic_compiler")
string(JOIN "," COVERAGE_BINS ${UNIT_BIN} ${INT_BIN} ${E2E_BIN} ${LUNIT_BIN} ${LINT_BIN} ${LE2E_BIN} ${CLI_BIN})

# Coverage artifacts output directory
set(COVERAGE_OUT_DIR "${CMAKE_BINARY_DIR}/coverage")
file(MAKE_DIRECTORY "${COVERAGE_OUT_DIR}")
file(WRITE "${COVERAGE_OUT_DIR}/include.re" "${COVERAGE_INCLUDE_RE}")
file(WRITE "${COVERAGE_OUT_DIR}/exclude.re" "${COVERAGE_EXCLUDE_RE}")
file(WRITE "${COVERAGE_OUT_DIR}/scope.txt" "${COVERAGE_SCOPE}")

# Discover llvm-cov / llvm-profdata (allow override by env LLVM_PREFIX)
set(_llvm_hint "$ENV{LLVM_PREFIX}")
if(_llvm_hint)
  list(APPEND _llvm_bin_hints "${_llvm_hint}/bin")
endif()
find_program(LLVM_PROFDATA_EXECUTABLE NAMES llvm-profdata HINTS ${_llvm_bin_hints})
find_program(LLVM_COV_EXECUTABLE      NAMES llvm-cov      HINTS ${_llvm_bin_hints})
if(NOT LLVM_PROFDATA_EXECUTABLE)
  set(LLVM_PROFDATA_EXECUTABLE llvm-profdata)
endif()
if(NOT LLVM_COV_EXECUTABLE)
  set(LLVM_COV_EXECUTABLE llvm-cov)
endif()

# Ensure tests exist before attempting to run
add_custom_target(coverage
  DEPENDS basic_compiler_unit_tests basic_compiler_integration_tests basic_compiler_e2e_tests logger_unit_tests logger_integration_tests logger_e2e_tests basic_compiler
  COMMAND ${CMAKE_COMMAND}
          -D OUT_DIR="${COVERAGE_OUT_DIR}"
          -D BINS_CSV="${COVERAGE_BINS}"
          -D LLVM_PROFDATA="${LLVM_PROFDATA_EXECUTABLE}"
          -D LLVM_COV="${LLVM_COV_EXECUTABLE}"
          -D INCLUDE_FILE="${COVERAGE_OUT_DIR}/include.re"
          -D EXCLUDE_FILE="${COVERAGE_OUT_DIR}/exclude.re"
          -D SCOPE_FILE="${COVERAGE_OUT_DIR}/scope.txt"
          -D MIN="${COVERAGE_MIN}"
          -D METRIC="${COVERAGE_METRIC}"
          -P "${CMAKE_SOURCE_DIR}/cmake/DoCoverage.cmake"
  USES_TERMINAL
)
