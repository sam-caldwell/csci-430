// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefSeg.ParsesWithAndWithoutExpr
 * Inputs: DEF SEG (no expr) and DEF SEG = 0
 * Code under test: Parser/Compiler handling of DEF SEG statement.
 * Expected behavior: Both forms compile successfully.
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
