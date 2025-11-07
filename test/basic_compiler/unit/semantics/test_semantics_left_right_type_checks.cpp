// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsBuiltins.LeftRight_TypeChecks
 * Purpose: Validate LEFT$/RIGHT$ require string first arg and numeric length.
 * Components Under Test: SemanticAnalyzer::analyzeExpr for LEFT$/RIGHT$.
 * Expected Behavior: Errors when types are wrong for either parameter.
 */
TEST(SemanticsBuiltins, LeftRight_TypeChecks) {
    // First arg must be string
    const char* src1 =
        "10 PRINT LEFT$(1,2)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    // Second arg must be numeric
    const char* src2 =
        "10 PRINT RIGHT$(\"HELLO\",\"2\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}

