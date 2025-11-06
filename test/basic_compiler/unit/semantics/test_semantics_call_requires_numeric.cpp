// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsCALL.RequiresNumericAddress
 * Inputs: CALL "X"
 * Code under test: Compiler::compileString() + semantic validation for CALL.
 * Expected behavior: SemanticError thrown due to non-numeric call address.
 */
TEST(SemanticsCALL, RequiresNumericAddress) {
    const char* src =
        "10 CALL \"X\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
