// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.SqrtNegativeConstantErrors
 * Purpose: Validate domain checking rejects SQR of a negative constant.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain checks)
 * Expected Behavior: Compilation throws SemanticError for SQR(-1).
 */
TEST(SemanticsMathDomains, SqrtNegativeConstantErrors) {
    const auto src =
        "10 PRINT SQR(-1)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
