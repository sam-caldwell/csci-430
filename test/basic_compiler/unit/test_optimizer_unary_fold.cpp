// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: Optimizer Unary
 * Purpose: Validate folding of unary plus/minus to minimize IR.
 * Components Under Test: AstOptimizer::optExpr.
 * Expected Behavior: +1 becomes 1.0; -(2) becomes -2.0; no fsub for +1.
 */
TEST(OptimizerExpr, UnaryPlusMinusFolding) {
    auto ir = Compiler::compileStringOptimized("10 PRINT +1\n20 END\n");
    EXPECT_NE(ir.find(", double 1.0)"), std::string::npos);
    EXPECT_EQ(ir.find(" = fsub double 0.0"), std::string::npos);

    ir = Compiler::compileStringOptimized("10 PRINT -(2)\n20 END\n");
    EXPECT_NE(ir.find(", double -2.0)"), std::string::npos);
}

