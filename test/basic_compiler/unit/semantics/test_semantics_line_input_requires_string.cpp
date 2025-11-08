// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.LineInput_RequiresStringVar
Inputs: LINE INPUT A (numeric variable)
Code under test: SemanticAnalyzer::analyzeStmt(LineInputStmt)
Expected behavior: Throws SemanticError requiring string variable
*/
TEST(Semantics, LineInput_RequiresStringVar) {
    const char* src = "10 LINE INPUT A\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

