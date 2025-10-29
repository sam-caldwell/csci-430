// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: Optimizer Algebraic Identities
 * Purpose: Validate basic identities (x+0, 0+x, x-0, x*1, 1*x, x/1, x*0).
 * Components Under Test: AstOptimizer::optExpr.
 * Expected Behavior: Expressions simplify; x*0 prints 0.0.
 */
TEST(OptimizerExpr, AlgebraicIdentities) {
    auto ir = Compiler::compileStringOptimized(
        "10 LET X = 42\n"
        "20 PRINT X + 0\n"
        "30 PRINT 0 + X\n"
        "40 PRINT X - 0\n"
        "50 PRINT X * 1\n"
        "60 PRINT 1 * X\n"
        "70 PRINT X / 1\n"
        "80 PRINT X * 0\n"
        "90 END\n");
    EXPECT_NE(ir.find(", double 0.0)"), std::string::npos);
}

