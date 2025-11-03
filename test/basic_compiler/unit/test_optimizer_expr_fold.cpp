// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: OptimizerExpr.ConstantFoldingArithmetic
 * Purpose: Validate constant folding and elimination of redundant arithmetic ops in IR.
 * Components Under Test: Compiler::compileStringOptimized; AstOptimizer (optExpr)
 * Expected Behavior: Expression 1 + 2*3 folds to 7.0; IR still prints via printf;
 *                    no "= fmul double 2.0, 3.0" nor generic "= fadd double" appear.
 */
TEST(OptimizerExpr, ConstantFoldingArithmetic) {
    // 1 + 2 * 3 => 7.0, no fmul/fadd for constants
    const auto src = "10 PRINT 1 + 2 * 3\n20 END\n";
    auto ir = Compiler::compileStringOptimized(src);
    EXPECT_NE(ir.find("call i32 (ptr, ...) @printf"), std::string::npos);
    EXPECT_NE(ir.find(", double 7.0)"), std::string::npos);
    EXPECT_EQ(ir.find(" = fmul double 2.0, 3.0"), std::string::npos);
    EXPECT_EQ(ir.find(" = fadd double"), std::string::npos);
}
