// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsBuiltins.Mid_FunctionTypeChecks
 * Purpose: Validate MID$ function requires string first arg, numeric start/len.
 * Components Under Test: SemanticAnalyzer::analyzeExpr for MID$ (function form).
 * Expected Behavior: SemanticError thrown for wrong-typed arguments.
 */
TEST(SemanticsBuiltins, Mid_FunctionTypeChecks) {
    // Non-string first argument
    const char* src1 =
        "10 PRINT MID$(1,2)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    // String start argument
    const char* src2 =
        "10 PRINT MID$(\"HELLO\",\"2\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
    // String len argument
    const char* src3 =
        "10 PRINT MID$(\"HELLO\",2,\"3\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src3); (void)ir; }, SemanticError);
}

