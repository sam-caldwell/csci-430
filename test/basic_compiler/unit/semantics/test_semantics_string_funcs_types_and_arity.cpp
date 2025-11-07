// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringFuncs.LEFT_Right_RequireStringAndNumeric
 * Inputs: Various LEFT$/RIGHT$ calls with wrong argument types.
 * Code under test: Compiler::compileString() + semantic type checks for string funcs.
 * Expected behavior: SemanticError is thrown for invalid type combinations.
 */
TEST(SemanticsStringFuncs, LEFT_Right_RequireStringAndNumeric) {
    // LEFT$ wrong types
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LEFT$(42,1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LEFT$(\"A\",\"B\")\n20 END\n"); (void)ir; }, SemanticError);
    // RIGHT$ wrong types
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RIGHT$(42,1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RIGHT$(\"A\",\"B\")\n20 END\n"); (void)ir; }, SemanticError);
}
