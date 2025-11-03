#!/usr/bin/env bash
set -euo pipefail

OUT_DIR=${1:?out dir}
UNIT_BIN=${2:?unit bin}
INT_BIN=${3:?integration bin}
E2E_BIN=${4:?e2e bin}
LLVM_PROFDATA=${5:?llvm-profdata}
LLVM_COV=${6:?llvm-cov}
INCLUDE_FILE=${7:?include regex file}
EXCLUDE_FILE=${8:?exclude regex file}
SCOPE_FILE=${9:?scope file}
MIN=${10:-95}
METRIC=${11:-both}

echo "[coverage] Running tests with profiling..."
mkdir -p "${OUT_DIR}"

if [ ! -x "${UNIT_BIN}" ]; then echo "Unit tests not found: ${UNIT_BIN}"; exit 2; fi
LLVM_PROFILE_FILE="${OUT_DIR}/unit-%p.profraw" "${UNIT_BIN}" >/dev/null || true
if [ -x "${INT_BIN}" ]; then LLVM_PROFILE_FILE="${OUT_DIR}/int-%p.profraw"  "${INT_BIN}" >/dev/null || true; fi
if [ -x "${E2E_BIN}" ]; then LLVM_PROFILE_FILE="${OUT_DIR}/e2e-%p.profraw"  "${E2E_BIN}" >/dev/null || true; fi

echo "[coverage] Merging profiles..."
shopt -s nullglob
files=( "${OUT_DIR}"/*.profraw )
if [ ${#files[@]} -eq 0 ]; then echo "No .profraw files found in ${OUT_DIR}"; exit 2; fi
"${LLVM_PROFDATA}" merge -sparse "${files[@]}" -o "${OUT_DIR}/coverage.profdata"

"$(dirname "$0")/run_coverage.sh" \
  "${OUT_DIR}" \
  "${UNIT_BIN}" "${INT_BIN}" "${E2E_BIN}" \
  "${LLVM_COV}" \
  "${OUT_DIR}/coverage.profdata" \
  "${MIN}" "${METRIC}" \
  "${INCLUDE_FILE}" "${EXCLUDE_FILE}" "${SCOPE_FILE}"
