// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefFn.NumericFunctionCompiles
 * Inputs: DEF FNSQ(X)=X*X; PRINT FNSQ(3)
 * Code under test: Compiler::compileString() DEF FN numeric function handling.
 * Expected behavior: Program compiles and emits IR.
 */
TEST(SemanticsDefFn, NumericFunctionCompiles) {
    const char* src =
        "10 DEF FNSQ(X) = X*X\n"
        "20 PRINT FNSQ(3)\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
