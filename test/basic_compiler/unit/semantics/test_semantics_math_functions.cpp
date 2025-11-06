// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMath.SqrtRequiresOneNumericArg
 * Purpose: Validate that SQR enforces exactly one numeric argument.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic arity/type checks)
 * Expected Behavior: Throws SemanticError for zero args and for a string argument.
 */
/*
Test: SemanticsMath.SqrtRequiresOneNumericArg
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMath, SqrtRequiresOneNumericArg) {
    // Too few args
    const auto src1 =
        "10 PRINT SQR()\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    // Non-numeric arg
    const auto src2 =
        "10 PRINT SQR(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}
