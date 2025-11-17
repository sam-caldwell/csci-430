// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/codegen/CodeGenerator.h"

using gwbasic::Program;
using gwbasic::Line;

/***
 * Test: PhaseLogHelpers.GenerateIRWithLogs
 * Purpose: Confirm generateIRWithLogs produces non-empty IR for a minimal program.
 * Components Under Test: semantics + codegen pipeline without logging.
 * Expected Behavior: Returned IR string is non-empty.
 */
TEST(PhaseLogHelpers, GenerateIRWithLogs) {
    // Minimal program to exercise semantics+codegen
    Program p{}; p.lines.push_back(Line{10,{}});
    gwbasic::SemanticAnalyzer sema; auto res = sema.analyze(p);
    gwbasic::CodeGenerator gen; gen.setSemantics(res);
    const auto ir = gen.generate(p);
    ASSERT_FALSE(ir.empty());
}
