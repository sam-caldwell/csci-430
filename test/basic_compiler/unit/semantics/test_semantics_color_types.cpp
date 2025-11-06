// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsColor.ForegroundMustBeNumeric
 * Inputs: COLOR "A"
 * Code under test: Compiler::compileString() + semantics for COLOR types.
 * Expected behavior: SemanticError thrown for non-numeric foreground.
 */
TEST(SemanticsColor, ForegroundMustBeNumeric) {
    const char* src =
        "10 COLOR \"A\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
