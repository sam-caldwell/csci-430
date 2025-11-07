// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Program.h"

using namespace gwbasic::phase_log_helpers;
using gwbasic::Program;
using gwbasic::Line;

/***
 * Test: PhaseLogHelpers.GenerateIRWithLogs
 * Purpose: Confirm generateIRWithLogs produces non-empty IR for a minimal program.
 * Components Under Test: phase_log_helpers::generateIRWithLogs() (lexer→parser→semantics→codegen).
 * Expected Behavior: Returned IR string is non-empty.
 */
TEST(PhaseLogHelpers, GenerateIRWithLogs) {
    // Minimal program to exercise semantics+codegen
    Program p{}; p.lines.push_back(Line{10,{}});
    auto ir = generateIRWithLogs(p, "", "");
    ASSERT_FALSE(ir.empty());
}
