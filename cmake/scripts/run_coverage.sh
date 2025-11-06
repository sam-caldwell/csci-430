#!/usr/bin/env bash
set -euo pipefail

OUT_DIR=${1:?out dir}
UNIT_BIN=${2:?unit bin}
INT_BIN=${3:?integration bin}
E2E_BIN=${4:?e2e bin}
LLVM_COV=${5:?llvm-cov path}
PROF_DATA=${6:?coverage.profdata}
MIN=${7:-95}
METRIC=${8:-both}
INCLUDE_RE=${9:-}
EXCLUDE_RE=${10:-}
SCOPE=${11:-}

# If args 9-11 are files, read their contents (allows special regex chars without shell parsing issues)
if [ -f "${INCLUDE_RE}" ]; then INCLUDE_RE="$(cat "${INCLUDE_RE}")"; fi
if [ -f "${EXCLUDE_RE}" ]; then EXCLUDE_RE="$(cat "${EXCLUDE_RE}")"; fi
if [ -f "${SCOPE}" ]; then SCOPE="$(cat "${SCOPE}")"; fi

echo "[coverage] Generating report..."
# Build list of binaries to include in the report (some may not exist in certain builds)
report_bins=()
if [ -x "${UNIT_BIN}" ]; then report_bins+=("${UNIT_BIN}"); fi
if [ -x "${INT_BIN}" ]; then report_bins+=("${INT_BIN}"); fi
if [ -x "${E2E_BIN}" ]; then report_bins+=("${E2E_BIN}"); fi
"${LLVM_COV}" report "${report_bins[@]}" -instr-profile="${PROF_DATA}" -use-color=false > "${OUT_DIR}/report.txt"

echo "[coverage] Exporting LCOV to ${OUT_DIR}/lcov.info..."
"${LLVM_COV}" export -format=lcov "${report_bins[@]}" -instr-profile="${PROF_DATA}" > "${OUT_DIR}/lcov.info"

awk_input="${OUT_DIR}/report.txt"
if [ -n "${INCLUDE_RE}" ]; then
  echo "[coverage] Including: ${INCLUDE_RE}"
  egrep "${INCLUDE_RE}" "${OUT_DIR}/report.txt" > "${OUT_DIR}/included.txt" || true
  awk_input="${OUT_DIR}/included.txt"
fi
if [ -n "${EXCLUDE_RE}" ]; then
  echo "[coverage] Excluding: ${EXCLUDE_RE}"
  egrep -v "${EXCLUDE_RE}" "${awk_input}" > "${OUT_DIR}/filtered.txt" || true
  awk_input="${OUT_DIR}/filtered.txt"
fi

awk '{ reg+=$2; miss+=$3; line+=$8; lmiss+=$9 } END { \
      if (reg>0) printf("Regions %d/%d %.0f\n", reg-miss, reg, (reg-miss)*100/reg); \
      if (line>0) printf("Lines %d/%d %.0f\n", line-lmiss, line, (line-lmiss)*100/line); }' "${awk_input}" > "${OUT_DIR}/scope.txt"

echo "[coverage] Scope summary:"
sed 's/^/  /' "${OUT_DIR}/scope.txt"

LINES=$(awk '/^Lines/{print $3}' "${OUT_DIR}/scope.txt")
REGS=$(awk '/^Regions/{print $3}' "${OUT_DIR}/scope.txt")
FAIL=0
case "${METRIC}" in
  lines)   VAL="${LINES}" ;;
  regions) VAL="${REGS}" ;;
  both)    VAL="${LINES}"; if [ -n "${REGS}" ] && [ "${REGS}" -lt "${MIN}" ]; then FAIL=1; fi ;;
  *)       VAL="${LINES}" ;;
esac

if [ -z "${VAL}" ]; then echo "Could not parse coverage values for scope ${SCOPE}."; exit 4; fi
if [ "${VAL}" -lt "${MIN}" ]; then FAIL=1; fi
if [ ${FAIL} -ne 0 ]; then echo "Coverage below threshold ${MIN}% (lines=${LINES}, regions=${REGS})."; exit 3; fi

echo "[coverage] OK. Detailed report: ${OUT_DIR}/report.txt (lcov: ${OUT_DIR}/lcov.info)"
