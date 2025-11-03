// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.IfConditionStringErrors
 * Purpose: Validate that IF conditions must be numeric/boolean expressions, not strings.
 * Components Under Test: Compiler::compileString; Semantics (type checking)
 * Expected Behavior: Compilation throws SemanticError when the IF condition is a string ("A").
 */
TEST(Semantics, IfConditionStringErrors) {
    const auto src =
        "10 IF \"A\" THEN 20\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
