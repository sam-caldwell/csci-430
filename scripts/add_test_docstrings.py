#!/usr/bin/env python3
"""
add_test_docstrings.py

Scans C++ tests under test/ and, for any file missing a compliant
docstring block immediately before the GoogleTest TEST(...) block,
inserts a standard docstring with the required fields:

  Test:, Inputs:, Code under test:, Expected behavior:

Heuristics are used based on the path to produce reasonable values.
Existing files that already contain a compliant block are left
unchanged. Files with multiple TEST(...) blocks are reported but not
modified.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEST_DIR = ROOT / 'test'

TEST_RE = re.compile(r'^(\s*)TEST\s*\(\s*([^,\s]+)\s*,\s*([^\)]+)\)', re.M)


def find_test_match(text: str):
    return TEST_RE.search(text)


def last_block_comment_before(text: str, pos: int) -> tuple[int, int, str] | None:
    """Return (start, end, body) of the last /* ... */ before pos, if any."""
    prefix = text[:pos]
    matches = list(re.finditer(r'/\*([\s\S]*?)\*/', prefix))
    if not matches:
        return None
    m = matches[-1]
    return (m.start(), m.end(), m.group(1))


def has_required_fields(block_body: str) -> bool:
    return (
        re.search(r'\bTest\s*:', block_body, re.IGNORECASE)
        and re.search(r'\bInputs\s*:', block_body, re.IGNORECASE)
        and re.search(r'\bCode\s+under\s+test\s*:', block_body, re.IGNORECASE)
        and re.search(r'\bExpected\s+behavior\s*:', block_body, re.IGNORECASE)
    )


def classify(path: Path) -> tuple[str, str, str]:
    """Return (inputs, code_under_test, expected) based on path."""
    p = str(path)
    if '/basic_compiler/e2e/' in p:
        return (
            'BASIC program(s) executed end-to-end (runtime output)',
            'Full compiler pipeline (lexer → parser → semantics → codegen → runtime)',
            'Program compiles and runs; output/behavior matches expectations',
        )
    if '/basic_compiler/integration/' in p:
        return (
            'BASIC snippet compiled through multiple stages',
            'Parser + Semantics + Codegen integration',
            'Emitted IR/state contains expected constructs and values',
        )
    if '/basic_compiler/unit/' in p:
        if 'test_lexer' in path.name or '/lexer' in p:
            return (
                'Raw source text and helper inputs',
                'Lexer/tokenization and helpers',
                'Tokens/escapes match expectations; errors are reported appropriately',
            )
        if 'test_parser' in path.name or '/parser' in p:
            return (
                'BASIC source snippet',
                'Parser (BASIC → AST)',
                'AST structure or parse errors match expectations',
            )
        if 'test_semantics' in path.name or '/semantics' in p:
            return (
                'Parsed AST (from BASIC snippet) and default environment',
                'Semantics analyzer (type/arity/domain checks)',
                'Valid programs accepted; invalid ones produce expected semantic errors',
            )
        if 'test_codegen' in path.name or '/codegen' in p:
            return (
                'AST (and semantic info) from BASIC snippet',
                'LLVM IR code generator',
                'Emits expected IR calls/ops; unsupported cases are reported',
            )
    if '/logger/' in p:
        return (
            'Filesystem paths, log messages, toggles',
            'Logger component',
            'Creates directories, writes/appends as expected, handles errors',
        )
    if '/hello_world/' in p:
        return (
            'None',
            'hello_world function',
            'Returns expected string',
        )
    # Fallback
    return (
        'See test body',
        'Relevant module(s) under test',
        'Asserts expected results/behavior described in test',
    )


def build_docstring(suite: str, name: str, path: Path) -> str:
    inputs, code_under_test, expected = classify(path)
    return (
        "/*\n"
        f"Test: {suite}.{name}\n"
        f"Inputs: {inputs}\n"
        f"Code under test: {code_under_test}\n"
        f"Expected behavior: {expected}\n"
        "*/\n"
    )


def process_file(path: Path) -> bool:
    """Insert a docstring if missing. Return True if modified."""
    text = path.read_text(errors='ignore')
    m = find_test_match(text)
    if not m:
        return False
    indent, suite, name = m.group(1), m.group(2), m.group(3)
    test_pos = m.start()

    last = last_block_comment_before(text, test_pos)
    if last and has_required_fields(last[2]):
        return False  # already compliant

    # Insert new block directly above TEST(...)
    doc = build_docstring(suite.strip(), name.strip(), path)
    new_text = text[:test_pos] + doc + text[test_pos:]
    path.write_text(new_text)
    return True


def main() -> int:
    modified = 0
    multi_test = []
    for cpp in TEST_DIR.rglob('*.cpp'):
        text = cpp.read_text(errors='ignore')
        tests = list(TEST_RE.finditer(text))
        if not tests:
            continue
        if len(tests) != 1:
            multi_test.append((cpp, len(tests)))
            continue
        if process_file(cpp):
            modified += 1
    if multi_test:
        print('Warning: files with multiple TEST(...) (not modified):')
        for p, n in multi_test:
            print(f'  - {p} ({n})')
    print(f'Added/updated docstrings in {modified} file(s).')
    return 0


if __name__ == '__main__':
    sys.exit(main())

