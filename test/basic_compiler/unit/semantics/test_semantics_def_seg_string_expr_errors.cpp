// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsDefSeg.StringExprErrors
 * Inputs: DEF SEG = "X"
 * Code under test: Compiler::compileString() + semantic checks for DEF SEG expr type.
 * Expected behavior: SemanticError thrown due to string expression.
 */
TEST(SemanticsDefSeg, StringExprErrors) {
    const char* src =
        "10 DEF SEG = \"X\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
