#! File: cmake/Coverage.cmake
#! Purpose: Provide a CMake-driven 'coverage' target that builds tests with
#!          coverage instrumentation and generates coverage reports, enforcing
#!          a threshold. Designed to be invoked via: cmake --build ... --target coverage

include_guard(GLOBAL)

# Configuration knobs (override at configure time with -D...)
set(COVERAGE_MIN "95" CACHE STRING "Minimum percent (Lines/Regions) to pass")
set(COVERAGE_SCOPE "src/basic_compiler/" CACHE STRING "Path prefix to report/aggregate")
# Default scope: focus on lexer to enable 100% coverage goals incrementally
set(COVERAGE_INCLUDE_RE "src/basic_compiler/lexer/" CACHE STRING "egrep regex to include rows")
set(COVERAGE_EXCLUDE_RE "src/basic_compiler/lexer/log_token.cpp|src/basic_compiler/codegenerator/" CACHE STRING "egrep regex to exclude rows")
set(COVERAGE_METRIC "regions" CACHE STRING "lines|regions|both (both requires both >= min)")

# Test executables (from this project)
set(UNIT_BIN "${CMAKE_BINARY_DIR}/basic_compiler_unit_tests")
set(INT_BIN  "${CMAKE_BINARY_DIR}/basic_compiler_integration_tests")
set(E2E_BIN  "${CMAKE_BINARY_DIR}/basic_compiler_e2e_tests")

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
  DEPENDS basic_compiler_unit_tests basic_compiler_integration_tests basic_compiler_e2e_tests
  COMMAND /bin/bash "${CMAKE_SOURCE_DIR}/cmake/scripts/do_coverage.sh"
                  "${COVERAGE_OUT_DIR}"
                  "${UNIT_BIN}" "${INT_BIN}" "${E2E_BIN}"
                  "${LLVM_PROFDATA_EXECUTABLE}" "${LLVM_COV_EXECUTABLE}"
                  "${COVERAGE_OUT_DIR}/include.re" "${COVERAGE_OUT_DIR}/exclude.re" "${COVERAGE_OUT_DIR}/scope.txt" "${COVERAGE_MIN}" "${COVERAGE_METRIC}"
  USES_TERMINAL
)
