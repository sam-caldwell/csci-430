#!/usr/bin/env python3
"""
check_test_docstrings.py

Ensures that under test/:
  - Each C++ test file contains exactly one GoogleTest TEST(...) block.
  - A docstring comment immediately above the TEST(...) contains the fields:
      Test:, Inputs:, Code under test:, Expected behavior:

Prints a report of any violations and returns non-zero if any are found.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEST_DIR = ROOT / 'test'

TEST_RE = re.compile(r'^\s*TEST\s*\(', re.M)

def find_test_positions(text: str):
    return [m.start() for m in TEST_RE.finditer(text)]

def has_required_docstring_above(text: str, test_pos: int) -> bool:
    """Find any block comment above the TEST that contains all required fields."""
    prefix = text[:test_pos]
    # Search for the last block comment before test
    comments = list(re.finditer(r'/\*([\s\S]*?)\*/', prefix))
    if not comments:
        return False
    for m in reversed(comments):
        block = m.group(1)
        # Strict pattern
        has_name = bool(re.search(r'Test\s*:', block, re.IGNORECASE))
        has_inputs = bool(re.search(r'Inputs\s*:', block, re.IGNORECASE))
        has_code = bool(re.search(r'Code under test\s*:', block, re.IGNORECASE))
        has_expected = bool(re.search(r'Expected behavior\s*:', block, re.IGNORECASE))
        if has_name and has_inputs and has_code and has_expected:
            return True
        # Legacy pattern accepted: Test Suite + Purpose (+ Components Under Test)
        has_suite = bool(re.search(r'Test Suite\s*:', block, re.IGNORECASE))
        has_purpose = bool(re.search(r'Purpose\s*:', block, re.IGNORECASE))
        has_components = bool(re.search(r'Components Under Test\s*:', block, re.IGNORECASE))
        if has_suite and (has_purpose or has_components):
            return True
    return False

def main() -> int:
    offenders = []
    for cpp in TEST_DIR.rglob('*.cpp'):
        text = cpp.read_text(errors='ignore')
        tests = find_test_positions(text)
        if not tests:
            continue
        if len(tests) != 1:
            offenders.append((cpp, f"contains {len(tests)} TEST(...) blocks (expected 1)"))
            continue
        if not has_required_docstring_above(text, tests[0]):
            offenders.append((cpp, "missing required docstring immediately above TEST(...)"))
    if offenders:
        print("Docstring/structure check found issues:\n")
        for path, msg in offenders:
            print(f"- {path}: {msg}")
        return 1
    print("All tests pass docstring and single-TEST checks.")
    return 0

if __name__ == '__main__':
    sys.exit(main())
