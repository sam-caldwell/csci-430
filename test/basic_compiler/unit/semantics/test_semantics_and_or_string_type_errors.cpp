// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.AndOrStringTypeErrors
Inputs: "X" AND "Y"; "X" OR "Y"
Code under test: SemanticAnalyzer::typeOf for AND/OR rejecting string operands
Expected behavior: SemanticError thrown
*/
TEST(Semantics, AndOrStringTypeErrors) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 LET A = \"X\" AND \"Y\"\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 LET A = \"X\" OR \"Y\"\n"); (void)ir; }, SemanticError);
}
