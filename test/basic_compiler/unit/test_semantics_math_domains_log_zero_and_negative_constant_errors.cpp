// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.LogZeroAndNegativeConstantErrors
 * Purpose: Validate domain checking rejects LOG applied to zero and negative constants.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain checks)
 * Expected Behavior: Compilation throws SemanticError for both LOG(0) and LOG(-1).
 */
TEST(SemanticsMathDomains, LogZeroAndNegativeConstantErrors) {
    const auto src1 =
        "10 PRINT LOG(0)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    const auto src2 =
        "10 PRINT LOG(-1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}
