// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsBuiltins.SPACE_Dollar_RequiresNumeric
 * Purpose: Validate SPACE$ rejects string argument during semantics.
 * Components Under Test: SemanticAnalyzer::analyzeExpr for SPACE$.
 * Expected Behavior: SemanticError is thrown for SPACE$("A").
 */
TEST(SemanticsBuiltins, SPACE_Dollar_RequiresNumeric) {
    const char* src =
        "10 PRINT SPACE$(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

