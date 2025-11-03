// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathMore.ArityErrorsReported
 * Purpose: Validate that recognized functions with incorrect arity are rejected.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic arity checks)
 * Expected Behavior: Compilation throws SemanticError for TAN called with two arguments.
 */
TEST(SemanticsMathMore, ArityErrorsReported) {
    // recognized function but wrong arity
    const auto src =
        "10 PRINT TAN(1,2)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
