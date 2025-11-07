// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringFuncs.MoreArityChecks
 * Purpose: Strengthen arity guardrails for STR$, SPACE$, STRING$, LTRIM$, RTRIM$.
 * Components Under Test: SemanticAnalyzer::analyzeExpr arity checks.
 * Expected Behavior: SemanticError for wrong argument counts on these string intrinsics.
 */
TEST(SemanticsStringFuncs, MoreArityChecks) {
    // STR$ requires 1
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STR$()\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STR$(1,2)\n20 END\n"); (void)ir; }, SemanticError);
    // SPACE$ requires 1
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT SPACE$()\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT SPACE$(1,2)\n20 END\n"); (void)ir; }, SemanticError);
    // STRING$ requires exactly 2
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STRING$(3)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT STRING$(3,65,0)\n20 END\n"); (void)ir; }, SemanticError);
    // LTRIM$/RTRIM$ require 1
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LTRIM$()\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LTRIM$(\"A\",1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RTRIM$()\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RTRIM$(\"A\",1)\n20 END\n"); (void)ir; }, SemanticError);
}

