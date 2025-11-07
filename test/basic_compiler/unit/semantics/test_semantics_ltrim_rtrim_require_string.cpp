// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsBuiltins.Trim_RequireString
 * Purpose: Ensure LTRIM$/RTRIM$ require string argument; numeric should error.
 * Components Under Test: SemanticAnalyzer::analyzeExpr for LTRIM$/RTRIM$.
 * Expected Behavior: SemanticError thrown when argument is numeric expression.
 */
TEST(SemanticsBuiltins, Trim_RequireString) {
    const char* src1 =
        "10 PRINT LTRIM$(1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);

    const char* src2 =
        "10 PRINT RTRIM$(1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}

