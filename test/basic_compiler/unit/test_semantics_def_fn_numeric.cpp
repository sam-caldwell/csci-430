// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefFn.NumericFunctionCompiles
 * Purpose: Ensure DEF FNSQ(X)=X*X compiles and can be called.
 */
TEST(SemanticsDefFn, NumericFunctionCompiles) {
    const char* src =
        "10 DEF FNSQ(X) = X*X\n"
        "20 PRINT FNSQ(3)\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}

TEST(SemanticsDefFn, NumericAndStringFunctionsCompile) {
    const char* src =
        "10 DEF FNSQ(X) = X*X\n"
        "20 DEF FNB$(S$) = S$ + \"!\"\n"
        "30 PRINT FNSQ(2)\n"
        "40 PRINT FNB$(\"ok\")\n"
        "50 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
