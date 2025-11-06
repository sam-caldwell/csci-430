// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathSqrt.SqrtComparisonArgumentErrors
 * Purpose: Validate that SQRT rejects a boolean/comparison result as its argument.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain/type checks)
 * Expected Behavior: Compilation throws SemanticError for SQRT(1<2).
 */
/*
Test: SemanticsMathSqrt.SqrtComparisonArgumentErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathSqrt, SqrtComparisonArgumentErrors) {
    const auto src =
        "10 PRINT SQRT(1<2)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
