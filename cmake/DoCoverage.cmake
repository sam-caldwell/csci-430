cmake_minimum_required(VERSION 3.21)

# Inputs (provided via -D flags)
#  - OUT_DIR: coverage artifacts directory
#  - BINS_CSV: comma-separated list of test/CLI binaries
#  - LLVM_PROFDATA: path to llvm-profdata
#  - LLVM_COV: path to llvm-cov
#  - INCLUDE_FILE: file containing include regex
#  - EXCLUDE_FILE: file containing exclude regex
#  - SCOPE_FILE: file to hold scope summary (will be overwritten)
#  - MIN: minimum coverage percent to pass
#  - METRIC: lines|regions|both

if(NOT DEFINED OUT_DIR OR NOT DEFINED BINS_CSV)
  message(FATAL_ERROR "DoCoverage.cmake requires OUT_DIR and BINS_CSV")
endif()

foreach(_qv OUT_DIR BINS_CSV LLVM_PROFDATA LLVM_COV INCLUDE_FILE EXCLUDE_FILE SCOPE_FILE MIN METRIC)
  if(DEFINED ${_qv})
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" ${_qv} "${${_qv}}")
  endif()
endforeach()

file(MAKE_DIRECTORY "${OUT_DIR}")
message(STATUS "[coverage] Running tests with profiling...")

# Split CSV into list
string(REPLACE "," ";" _bins "${BINS_CSV}")

set(_did_any FALSE)
foreach(bin IN LISTS _bins)
  if(EXISTS "${bin}")
    set(_did_any TRUE)
    get_filename_component(_base "${bin}" NAME)
    # Run the binary with LLVM_PROFILE_FILE to produce .profraw; ignore exit code
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E env
              "LLVM_PROFILE_FILE=${OUT_DIR}/${_base}-%p.profraw"
              "${bin}"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      RESULT_VARIABLE _rv
      OUTPUT_QUIET ERROR_QUIET
    )
    # Intentionally ignore ${_rv} to tolerate non-zero exits/segfaults
  endif()
endforeach()
if(NOT _did_any)
  message(FATAL_ERROR "No test binaries found to run")
endif()

message(STATUS "[coverage] Merging profiles...")
file(GLOB _profraws "${OUT_DIR}/*.profraw")
list(LENGTH _profraws _prof_count)
if(_prof_count EQUAL 0)
  message(FATAL_ERROR "No .profraw files found in ${OUT_DIR}")
endif()

if(NOT DEFINED LLVM_PROFDATA)
  set(LLVM_PROFDATA llvm-profdata)
endif()
if(NOT DEFINED LLVM_COV)
  set(LLVM_COV llvm-cov)
endif()

execute_process(
  COMMAND "${LLVM_PROFDATA}" merge -sparse ${_profraws} -o "${OUT_DIR}/coverage.profdata"
  RESULT_VARIABLE _merge_rv
)
if(NOT _merge_rv EQUAL 0)
  message(FATAL_ERROR "llvm-profdata merge failed with code ${_merge_rv}")
endif()

# Delegate to RunCoverage.cmake to generate report, export LCOV, enforce threshold
execute_process(
  COMMAND "${CMAKE_COMMAND}"
          -D OUT_DIR="${OUT_DIR}"
          -D BINS_CSV="${BINS_CSV}"
          -D LLVM_COV="${LLVM_COV}"
          -D PROF_DATA="${OUT_DIR}/coverage.profdata"
          -D MIN="${MIN}"
          -D METRIC="${METRIC}"
          -D INCLUDE_FILE="${INCLUDE_FILE}"
          -D EXCLUDE_FILE="${EXCLUDE_FILE}"
          -D SCOPE_FILE="${SCOPE_FILE}"
          -P "${CMAKE_CURRENT_LIST_DIR}/RunCoverage.cmake"
  RESULT_VARIABLE _run_rv
)
if(NOT _run_rv EQUAL 0)
  message(FATAL_ERROR "Coverage run failed with code ${_run_rv}")
endif()
