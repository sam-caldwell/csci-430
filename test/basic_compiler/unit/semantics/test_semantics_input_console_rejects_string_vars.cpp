// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.InputConsole_RejectsStringVariables
Inputs: INPUT "prompt"; S$, N (string var followed by numeric)
Code under test: SemanticAnalyzer::analyzeStmt(InputStmt)
Expected behavior: Throws SemanticError indicating string variable not allowed for INPUT (use LINE INPUT).
*/
TEST(Semantics, InputConsole_RejectsStringVariables) {
    const char* src =
        "10 INPUT \"Enter:\"; S$, N\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

