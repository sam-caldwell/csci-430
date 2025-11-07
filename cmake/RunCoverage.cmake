cmake_minimum_required(VERSION 3.21)

# Inputs via -D flags
#  - OUT_DIR: coverage artifacts directory
#  - BINS_CSV: comma-separated list of test/CLI binaries
#  - LLVM_COV: path to llvm-cov
#  - PROF_DATA: path to merged coverage.profdata
#  - MIN: minimum coverage percent to pass
#  - METRIC: lines|regions|both
#  - INCLUDE_FILE: file containing include regex (may be empty)
#  - EXCLUDE_FILE: file containing exclude regex (may be empty)
#  - SCOPE_FILE: file to write scope summary (overwritten)

foreach(_qv OUT_DIR BINS_CSV LLVM_COV PROF_DATA MIN METRIC INCLUDE_FILE EXCLUDE_FILE SCOPE_FILE)
  if(DEFINED ${_qv})
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" ${_qv} "${${_qv}}")
  endif()
endforeach()

if(NOT DEFINED OUT_DIR OR NOT DEFINED BINS_CSV OR NOT DEFINED PROF_DATA)
  message(FATAL_ERROR "RunCoverage.cmake requires OUT_DIR, BINS_CSV, and PROF_DATA")
endif()

if(NOT DEFINED LLVM_COV)
  set(LLVM_COV llvm-cov)
endif()

# Read regex filters (if files exist)
set(INCLUDE_RE "")
set(EXCLUDE_RE "")
if(DEFINED INCLUDE_FILE AND EXISTS "${INCLUDE_FILE}")
  file(READ "${INCLUDE_FILE}" INCLUDE_RE)
  string(STRIP "${INCLUDE_RE}" INCLUDE_RE)
endif()
if(DEFINED EXCLUDE_FILE AND EXISTS "${EXCLUDE_FILE}")
  file(READ "${EXCLUDE_FILE}" EXCLUDE_RE)
  string(STRIP "${EXCLUDE_RE}" EXCLUDE_RE)
endif()

message(STATUS "[coverage] Generating report...")

# Filter to only existing bins
string(REPLACE "," ";" _bins "${BINS_CSV}")
set(_existing_bins)
foreach(b IN LISTS _bins)
  if(EXISTS "${b}")
    list(APPEND _existing_bins "${b}")
  endif()
endforeach()

if(_existing_bins STREQUAL "")
  message(FATAL_ERROR "No existing binaries to report coverage from")
endif()

# Produce human-readable report and LCOV export
execute_process(
  COMMAND "${LLVM_COV}" report ${_existing_bins} -instr-profile=${PROF_DATA} -use-color=false --no-warn
  OUTPUT_FILE "${OUT_DIR}/report.txt"
  RESULT_VARIABLE _report_rv
)
if(NOT _report_rv EQUAL 0)
  message(WARNING "llvm-cov report returned ${_report_rv}")
endif()

message(STATUS "[coverage] Exporting LCOV to ${OUT_DIR}/lcov.info...")
execute_process(
  COMMAND "${LLVM_COV}" export -format=lcov ${_existing_bins} -instr-profile=${PROF_DATA} --no-warn
  OUTPUT_FILE "${OUT_DIR}/lcov.info"
  RESULT_VARIABLE _lcov_rv
)
if(NOT _lcov_rv EQUAL 0)
  message(WARNING "llvm-cov export returned ${_lcov_rv}")
endif()

if(NOT INCLUDE_RE STREQUAL "")
  message(STATUS "[coverage] Including: ${INCLUDE_RE}")
endif()
if(NOT EXCLUDE_RE STREQUAL "")
  message(STATUS "[coverage] Excluding: ${EXCLUDE_RE}")
endif()

# Parse report.txt and apply include/exclude filters to accumulate totals
file(READ "${OUT_DIR}/report.txt" _report)
string(REPLACE "\n" ";" _lines "${_report}")

set(_reg_total 0)
set(_reg_missed 0)
set(_line_total 0)
set(_line_missed 0)

foreach(line IN LISTS _lines)
  string(STRIP "${line}" line)
  if(line STREQUAL "")
    continue()
  endif()
  if(line MATCHES "^Filename[ ]")
    continue()
  endif()
  if(line MATCHES "^[-]+$")
    continue()
  endif()

  # Normalize multiple spaces to single, strip percent signs
  string(REGEX REPLACE "[ ]+" " " norm "${line}")
  string(REPLACE "%" "" norm "${norm}")

  # Split columns
  string(REPLACE " " ";" cols "${norm}")
  list(LENGTH cols _ncols)
  if(_ncols LESS 13)
    # Unexpected format; skip
    continue()
  endif()

  list(GET cols 0 filename)

  # Apply include/exclude filters on filename
  set(_take TRUE)
  if(NOT INCLUDE_RE STREQUAL "")
    if(NOT filename MATCHES "${INCLUDE_RE}")
      set(_take FALSE)
    endif()
  endif()
  if(_take AND NOT EXCLUDE_RE STREQUAL "")
    if(filename MATCHES "${EXCLUDE_RE}")
      set(_take FALSE)
    endif()
  endif()
  if(NOT _take)
    continue()
  endif()

  # Extract numeric columns
  list(GET cols 1 reg_total)
  list(GET cols 2 reg_missed)
  list(GET cols 7 line_total)
  list(GET cols 8 line_missed)

  # Accumulate
  math(EXPR _reg_total "${_reg_total} + ${reg_total}")
  math(EXPR _reg_missed "${_reg_missed} + ${reg_missed}")
  math(EXPR _line_total "${_line_total} + ${line_total}")
  math(EXPR _line_missed "${_line_missed} + ${line_missed}")
endforeach()

# Compute summary stats
set(_scope_summary "")
set(_lines_pct "")
set(_regs_pct "")
if(_reg_total GREATER 0)
  math(EXPR _reg_hits "${_reg_total} - ${_reg_missed}")
  math(EXPR _regs_pct "(${_reg_hits} * 100) / ${_reg_total}")
  string(APPEND _scope_summary "Regions ${_reg_hits}/${_reg_total} ${_regs_pct}\n")
endif()
if(_line_total GREATER 0)
  math(EXPR _line_hits "${_line_total} - ${_line_missed}")
  math(EXPR _lines_pct "(${_line_hits} * 100) / ${_line_total}")
  string(APPEND _scope_summary "Lines ${_line_hits}/${_line_total} ${_lines_pct}\n")
endif()

if(NOT DEFINED SCOPE_FILE)
  set(SCOPE_FILE "${OUT_DIR}/scope.txt")
endif()
file(WRITE "${SCOPE_FILE}" "${_scope_summary}")

message(STATUS "[coverage] Scope summary:")
foreach(_l IN LISTS _lines)
  # This loops all original lines; we'll instead echo the summary we just built
endforeach()
file(READ "${SCOPE_FILE}" _scope_out)
string(REPLACE "\n" "\n  " _scope_out "  ${_scope_out}")
message(STATUS "${_scope_out}")

# Enforce threshold
if(NOT DEFINED MIN)
  set(MIN 95)
endif()
if(NOT DEFINED METRIC)
  set(METRIC lines)
endif()

set(_val "")
if(METRIC STREQUAL "lines")
  set(_val "${_lines_pct}")
elseif(METRIC STREQUAL "regions")
  set(_val "${_regs_pct}")
elseif(METRIC STREQUAL "both")
  set(_val "${_lines_pct}")
else()
  set(_val "${_lines_pct}")
endif()

set(_fail FALSE)
if(_val STREQUAL "")
  message(FATAL_ERROR "Could not parse coverage values for scope.")
endif()
if(_val LESS MIN)
  set(_fail TRUE)
endif()
if(METRIC STREQUAL "both" AND NOT _regs_pct STREQUAL "")
  if(_regs_pct LESS MIN)
    set(_fail TRUE)
  endif()
endif()

if(_fail)
  message(FATAL_ERROR "Coverage below threshold ${MIN}% (lines=${_lines_pct}, regions=${_regs_pct}).")
else()
  message(STATUS "[coverage] OK. Detailed report: ${OUT_DIR}/report.txt (lcov: ${OUT_DIR}/lcov.info)")
endif()
