// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: Semantics.OptionBaseInvalid
Inputs: OPTION BASE 2
Code under test: SemanticAnalyzer::analyzeStmt(OptionBaseStmt)
Expected behavior: SemanticError thrown (only 0 or 1 allowed)
*/
TEST(Semantics, OptionBaseInvalid) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 OPTION BASE 2\n"); (void)ir; }, SemanticError);
}

