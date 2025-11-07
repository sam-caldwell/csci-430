// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.IfMissingTargetErrors
 * Inputs: IF ... THEN 9999 where 9999 is not a program line.
 * Code under test: Compiler::compileString() semantic validation of branch targets.
 * Expected behavior: SemanticError thrown for missing label target.
 */
TEST(Semantics, IfMissingTargetErrors) {
    const auto src =
        "10 IF 1 = 1 THEN 9999\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
