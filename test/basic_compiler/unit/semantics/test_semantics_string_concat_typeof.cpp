// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.StringConcatTypeOf
Inputs: LET S$ = "A" + "B"
Code under test: SemanticAnalyzer::typeOf for BinaryOp::Add with two strings
Expected behavior: Program compiles (concatenation allowed, result is string)
*/
TEST(Semantics, StringConcatTypeOf) {
    const char* src =
        "10 LET S$ = \"A\" + \"B\"\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
