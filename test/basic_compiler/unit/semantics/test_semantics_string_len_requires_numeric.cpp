// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsBuiltins.STRING_Dollar_LengthMustBeNumeric
 * Purpose: Ensure STRING$ first argument (length) must be numeric.
 * Components Under Test: SemanticAnalyzer::analyzeExpr for STRING$.
 * Expected Behavior: SemanticError thrown for STRING$("3","A").
 */
TEST(SemanticsBuiltins, STRING_Dollar_LengthMustBeNumeric) {
    const char* src =
        "10 PRINT STRING$(\"3\",\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

