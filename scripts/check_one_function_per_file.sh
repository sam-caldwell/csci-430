#!/usr/bin/env bash
set -euo pipefail

# Simple static check: ensure each .cpp file under src/basic_compiler/
# defines at most one function/method. Approximate function definitions
# by matching common C++ signature forms ending with '{' on the same line.

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT_DIR"

command -v shellcheck || {
  echo 'missing dependency: shellcheck'
  exit 1
}

violations=()

files=$(find src/basic_compiler -type f -name "*.cpp" | sort)

# Regex: optional qualifiers, return type, name, params, optional const/noexcept, then '{'
pattern='^[[:space:]]*(template[[:space:]]*<[^>]+>[[:space:]]*)?(inline[[:space:]]+)?(static[[:space:]]+)?(constexpr[[:space:]]+)?(void|bool|int|double|float|size_t|uint64_t|int64_t|std::[A-Za-z_][A-Za-z0-9_]*|[A-Za-z_][A-Za-z0-9_]*::[A-Za-z_][A-Za-z0-9_]*)[[:space:]]+[*& ]*[~]?[A-Za-z_][A-Za-z0-9_:<>]*[[:space:]]*\([^;{}]*\)[[:space:]]*(const)?[[:space:]]*(noexcept)?[[:space:]]*\{'

for f in $files; do
  # shellcheck disable=SC2126
  count=$(grep -E "$pattern" "$f" | wc -l | tr -d '[:space:]') || true
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
