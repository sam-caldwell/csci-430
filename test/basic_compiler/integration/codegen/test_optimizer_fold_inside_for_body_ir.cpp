// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: OptimizerExpr.FoldInsideForBody
 * Inputs: FOR loop with body `PRINT 1+2`
 * Code under test: AstOptimizer::optimize (body expression simplification)
 * Expected behavior: Body constant expression folds; IR prints integer 3 without fadd
 */
TEST(OptimizerExpr, FoldInsideForBody) {
    auto const src =
        "10 FOR I=1 TO 2\n"
        "20 PRINT 1+2\n"
        "30 NEXT I\n"
        "40 END\n";
    const std::string ir = Compiler::compileStringOptimized(src);
    // Look within the FOR body for printing literal 3; ensure no fadd
    EXPECT_NE(ir.find(", i64 3)"), std::string::npos);
    EXPECT_EQ(ir.find(" = fadd double 1.0, 2.0"), std::string::npos);
}
