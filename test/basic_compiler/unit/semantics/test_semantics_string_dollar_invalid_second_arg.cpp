// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringFuncs.STRING_SecondArg_InvalidShapes
 * Purpose: Negative-path variants for STRING$ second arg to strengthen guardrails.
 * Components Under Test: SemanticAnalyzer::analyzeExpr type validation for STRING$.
 * Expected Behavior: SemanticError via invalid nested expressions (string arithmetic or bad inner types).
 */
TEST(SemanticsStringFuncs, STRING_SecondArg_InvalidShapes) {
    // Invalid: nested LEN with non-variable numeric argument (LEN(1) forbidden)
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STRING$(3, LEN(1))\n20 END\n"); (void)ir; }, SemanticError);
    // Invalid: nested LEFT$ with wrong arg types (first arg numeric)
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STRING$(3, LEFT$(1,2))\n20 END\n"); (void)ir; }, SemanticError);
    // Invalid: nested MID$ with wrong first arg type (numeric)
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STRING$(3, MID$(1,1))\n20 END\n"); (void)ir; }, SemanticError);
}
