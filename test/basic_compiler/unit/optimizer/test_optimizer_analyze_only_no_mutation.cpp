// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: OptimizerAnalyzeOnly.NoMutation
 * Inputs: Set gMetrics->analyze_only=true; compile optimized
 * Code under test: AstOptimizer transform guards with isAnalyzeOnly()
 * Expected behavior: No constant folding/mutation; IR still contains fmul/fadd for 1+2*3
 */
TEST(OptimizerAnalyzeOnly, NoMutation) {
    Metrics m; m.setAnalyzeOnly(true);
    auto* prev = gMetrics; gMetrics = &m;
    const std::string ir = Compiler::compileStringOptimized("10 PRINT 1+2*3\n20 END\n");
    // Expect raw operations remain since optimizer is analysis-only
    EXPECT_NE(ir.find(" = fmul double 2.0, 3.0"), std::string::npos);
    EXPECT_NE(ir.find(" = fadd double"), std::string::npos);
    gMetrics = prev;
}
