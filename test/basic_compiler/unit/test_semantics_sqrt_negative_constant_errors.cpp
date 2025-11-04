// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathSqrt.SqrtNegativeConstantErrors
 * Purpose: Validate domain checking rejects SQRT of a negative constant.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain checks)
 * Expected Behavior: Compilation throws SemanticError for SQRT(-1).
 */
/*
Test: SemanticsMathSqrt.SqrtNegativeConstantErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathSqrt, SqrtNegativeConstantErrors) {
    const auto src =
        "10 PRINT SQRT(-1)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
