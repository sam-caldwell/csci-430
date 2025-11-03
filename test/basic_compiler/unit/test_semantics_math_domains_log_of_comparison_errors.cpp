// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.LogOfComparisonErrors
 * Purpose: Validate domain/type checking rejects LOG applied to a boolean/comparison result.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain/type checks)
 * Expected Behavior: Compilation throws SemanticError for LOG(1<2).
 */
TEST(SemanticsMathDomains, LogOfComparisonErrors) {
    const auto src =
        "10 PRINT LOG(1<2)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
