// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefSeg.ParsesWithAndWithoutExpr
 */
TEST(SemanticsDefSeg, ParsesWithAndWithoutExpr) {
    const char* src1 =
        "10 DEF SEG\n"
        "20 PRINT \"OK\"\n"
        "30 END\n";
    ASSERT_FALSE(Compiler::compileString(src1).empty());

    const char* src2 =
        "10 DEF SEG = 0\n"
        "20 END\n";
    ASSERT_FALSE(Compiler::compileString(src2).empty());
}

/*
 * Test: SemanticsDefSeg.StringExprErrors
 */
TEST(SemanticsDefSeg, StringExprErrors) {
    const char* src =
        "10 DEF SEG = \"X\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

