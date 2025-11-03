// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.ForEndMustBeNumeric
 * Purpose: Validate that the FOR loop's end bound must be numeric.
 * Components Under Test: Compiler::compileString; Semantics (type checking)
 * Expected Behavior: Compilation throws SemanticError when the TO bound is a string (e.g., "Z").
 */
TEST(Semantics, ForEndMustBeNumeric) {
    const auto src =
        "10 FOR I=1 TO \"Z\":NEXT I\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
