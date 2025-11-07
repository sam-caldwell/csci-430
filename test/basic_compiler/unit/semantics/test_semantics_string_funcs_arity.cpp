// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringFuncs.ArityChecks
 * Inputs: LEFT$/RIGHT$/MID$ with too few/many args.
 * Code under test: Compiler::compileString() arg count validation for string funcs.
 * Expected behavior: SemanticError thrown for wrong arity.
 */
TEST(SemanticsStringFuncs, ArityChecks) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LEFT$(\"A\")\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RIGHT$(\"A\",1,2)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(\"A\")\n20 END\n"); (void)ir; }, SemanticError);
}
