// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsColor.BorderMustBeNumeric
 * Inputs: COLOR 1,2,"C"
 * Code under test: Compiler::compileString() + semantics for COLOR.
 * Expected behavior: SemanticError thrown for non-numeric border color.
 */
TEST(SemanticsColor, BorderMustBeNumeric) {
    const char* src =
        "10 COLOR 1,2,\"C\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
