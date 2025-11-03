// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathMore.UnknownFunctionErrors
 * Purpose: Validate that calls to unknown/unsupported functions are rejected by semantics.
 * Components Under Test: Compiler::compileString; Semantics (function/intrinsic resolution)
 * Expected Behavior: Compilation throws SemanticError for FOO(1).
 */
TEST(SemanticsMathMore, UnknownFunctionErrors) {
    const auto src =
        "10 PRINT FOO(1)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
