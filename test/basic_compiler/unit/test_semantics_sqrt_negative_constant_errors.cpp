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
TEST(SemanticsMathSqrt, SqrtNegativeConstantErrors) {
    const auto src =
        "10 PRINT SQRT(-1)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
