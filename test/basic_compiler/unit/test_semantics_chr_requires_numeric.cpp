// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsBuiltins.CHR_Dollar_RequiresNumericArg
 * Inputs: CHR$("A")
 * Code under test: Compiler::compileString() + semantics for CHR$.
 * Expected behavior: SemanticError thrown for non-numeric argument.
 */
TEST(SemanticsBuiltins, CHR_Dollar_RequiresNumericArg) {
    const char* src =
        "10 PRINT CHR$(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

