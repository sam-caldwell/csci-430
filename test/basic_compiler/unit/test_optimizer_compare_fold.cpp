// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: Optimizer Comparisons
 * Purpose: Fold constant comparison expressions to numeric 0.0/1.0 results
 *          to minimize generated IR.
 * Components Under Test: AstOptimizer::optExpr via Compiler::compileStringOptimized.
 * Expected Behavior: Constant comparisons print 1.0 or 0.0 with no fcmp
 *          emitted in the final IR.
 */
TEST(OptimizerCmp, ConstantComparisonsFoldToNumbers) {
    auto ir = Compiler::compileStringOptimized("10 PRINT 1=1\n20 PRINT 1<>2\n30 PRINT 1<2\n40 PRINT 2<=2\n50 PRINT 3>2\n60 PRINT 3>=3\n70 END\n");
    EXPECT_NE(ir.find(", double 1.0)"), std::string::npos); // there should be many
    // and at least one 0.0 for a false comparison (e.g., 1<>2 is true; ensure there is at least one false too)
    auto ir2 = Compiler::compileStringOptimized("10 PRINT 1=2\n20 END\n");
    EXPECT_NE(ir2.find(", double 0.0)"), std::string::npos);
    // No fcmp for constant comparisons
    EXPECT_EQ(ir.find(" = fcmp"), std::string::npos);
    EXPECT_EQ(ir2.find(" = fcmp"), std::string::npos);
}
