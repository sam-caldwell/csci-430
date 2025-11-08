// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.OnIndex_StringErrors
Inputs: ON "X" GOTO 10; ON "Y" GOSUB 20
Expected: SemanticError for string index in both.
*/
TEST(Semantics, OnIndex_StringErrors) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 ON \"X\" GOTO 10\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 ON \"Y\" GOSUB 20\n"); (void)ir; }, SemanticError);
}

