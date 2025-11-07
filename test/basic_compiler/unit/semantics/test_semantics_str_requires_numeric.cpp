// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsBuiltins.STR_Dollar_RequiresNumeric
 * Purpose: Validate STR$ rejects string argument during semantic analysis.
 * Components Under Test: Compiler::compileString() + SemanticAnalyzer::analyzeExpr for STR$.
 * Expected Behavior: SemanticError is thrown for STR$("A").
 */
TEST(SemanticsBuiltins, STR_Dollar_RequiresNumeric) {
    const char* src =
        "10 PRINT STR$(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

