#!/usr/bin/env bash
#
# check_one_function_per_file.sh
# (c) 2022 Asymmetric Effort, LLC. <scaldwell@asymmetric-effort.com>
# Simple static check: ensure each .cpp file under src/ defines at most one function/method.
# We approximate function definitions with a regex that matches common C++ signature forms ending with '{'.
#
#
set -euo pipefail


shopt -s nullglob
ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT_DIR"

violations=()

# Portable file list without readarray/mapfile (works on macOS Bash 3.x)
files=$(find src/ -type f -name "*.cpp" | sort)

# Regex notes:
# - optional qualifiers: template<...>, inline, static, constexpr
# - return type (identifier with optional :: / <>), optional pointer/ref
# - name (identifier or qualified ::)
# - parameter list (...)
# - optional const/noexcept, then '{' on same line
pattern='^[[:space:]]*(template[[:space:]]*<[^>]+>[[:space:]]*)?(inline[[:space:]]+)?(static[[:space:]]+)?(constexpr[[:space:]]+)?[A-Za-z_][A-Za-z0-9_:<>]*[[:space:]]+[*& ]*[~]?[A-Za-z_][A-Za-z0-9_:<>]*[[:space:]]*\([^;{}]*\)[[:space:]]*(const)?[[:space:]]*(noexcept)?[[:space:]]*\{'

for f in $files; do
  # Skip generated or special cases if any
  count=$(grep -E "$pattern" "$f" | wc -l | tr -d '[:space:]')
  # Allow 0 (e.g., TU with only helper lambdas or declarations) or 1; flag >1
  if [[ "$count" =~ ^[0-9]+$ ]] && (( count > 1 )); then
    violations+=("$f:$count")
  fi
done

if ((${#violations[@]} > 0)); then
  echo "Error: found files with more than one function/method definition:" >&2
  for v in "${violations[@]}"; do echo "  $v" >&2; done
  echo "Hint: project convention is one function/method per .cpp file under src/basic_compiler/." >&2
  exit 1
fi

echo "One-function-per-file check: OK"
