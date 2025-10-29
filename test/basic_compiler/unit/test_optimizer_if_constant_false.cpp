// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: Optimizer IF (false)
 * Purpose: Validate that constant-false IF is removed, preserving following code.
 * Components Under Test: AstOptimizer::optimize; CodeGenerator.
 * Expected Behavior: No branch to target; subsequent PRINT remains.
 */
TEST(OptimizerIf, ConstantFalseRemoved) {
    const char* src =
        "10 IF 2 < 1 THEN 100\n"
        "20 PRINT 5\n"
        "30 END\n";
    auto ir = Compiler::compileStringOptimized(src);
    EXPECT_EQ(ir.find("line100"), std::string::npos);
    EXPECT_NE(ir.find(", double 5.0)"), std::string::npos);
}

