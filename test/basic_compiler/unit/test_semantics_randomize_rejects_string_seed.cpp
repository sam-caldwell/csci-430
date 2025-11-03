// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsRandomize.RejectsStringSeed
 * Purpose: Validate that RANDOMIZE rejects a string seed argument.
 * Components Under Test: Compiler::compileString; Semantics (argument type checking)
 * Expected Behavior: Compilation throws SemanticError for RANDOMIZE "A".
 */
TEST(SemanticsRandomize, RejectsStringSeed) {
    const auto src =
        "10 RANDOMIZE \"A\"\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
