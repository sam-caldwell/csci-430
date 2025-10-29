// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: Optimizer IF Folding
 * Purpose: Fold IF statements whose conditions are constant after expression
 *          simplification into a GOTO (true) or a no-op (false).
 * Components Under Test: AstOptimizer::optimize, CodeGenerator emitLineBlock.
 * Expected Behavior: True conditions emit an unconditional branch to target
 *          without fcmp; false conditions remove the branch entirely.
 */
TEST(OptimizerIf, ConstantTrueBecomesGoto) {
    const char* src =
        "10 IF 1 < 2 THEN 100\n"
        "20 PRINT 0\n"
        "100 END\n";
    auto ir = Compiler::compileStringOptimized(src);
    // Expect unconditional branch to line100 from line10
    EXPECT_NE(ir.find("line10:"), std::string::npos);
    EXPECT_NE(ir.find("  br label %line100"), std::string::npos);
    // No compare emitted for the IF
    EXPECT_EQ(ir.find(" = fcmp"), std::string::npos);
}

// Single-test-per-file policy: additional case moved to dedicated file.
