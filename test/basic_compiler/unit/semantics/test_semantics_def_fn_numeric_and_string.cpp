// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsDefFn.NumericAndStringFunctionsCompile
 * Inputs: DEF FNSQ(X)=X*X; DEF FNB$(S$)=S$+"!"; PRINT calls
 * Code under test: Compiler::compileString() DEF FN for numeric and string functions.
 * Expected behavior: Program compiles and emits IR.
 */
TEST(SemanticsDefFn, NumericAndStringFunctionsCompile) {
    const char* src =
        "10 DEF FNSQ(X) = X*X\n"
        "20 DEF FNB$(S$) = S$ + \"!\"\n"
        "30 PRINT FNSQ(2)\n"
        "40 PRINT FNB$(\"ok\")\n"
        "50 END\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
