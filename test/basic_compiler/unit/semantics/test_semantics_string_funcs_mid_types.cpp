// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsStringFuncs.MID_RequiresStringNumericAndOptionalNumeric
 * Inputs: MID$ with non-string first arg; non-numeric position/length.
 * Code under test: Compiler::compileString() semantic checks for MID$.
 * Expected behavior: SemanticError thrown for invalid type usage.
 */
TEST(SemanticsStringFuncs, MID_RequiresStringNumericAndOptionalNumeric) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(42,1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(\"A\",\"B\")\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(\"A\",1,\"C\")\n20 END\n"); (void)ir; }, SemanticError);
}

