// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.LogOfZeroArithmeticConstantErrors
 * Purpose: Validate domain checking rejects LOG applied to zero from an arithmetic constant expression.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain checks)
 * Expected Behavior: Compilation throws SemanticError for LOG(1-1).
 */
TEST(SemanticsMathDomains, LogOfZeroArithmeticConstantErrors) {
    const auto src =
        "10 PRINT LOG(1-1)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
