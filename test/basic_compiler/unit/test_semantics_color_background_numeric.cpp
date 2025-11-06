// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsColor.BackgroundMustBeNumeric
 * Inputs: COLOR 1,"B"
 * Code under test: Compiler::compileString() + semantics for COLOR.
 * Expected behavior: SemanticError thrown for non-numeric background color.
 */
TEST(SemanticsColor, BackgroundMustBeNumeric) {
    const char* src =
        "10 COLOR 1,\"B\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

