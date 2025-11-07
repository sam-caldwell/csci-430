// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.ColorAndChdirErrors
Inputs: COLOR with string args; CHDIR with numeric path
Code under test: SemanticAnalyzer::analyzeStmt(ColorStmt/ChdirStmt)
Expected behavior: Compilation throws SemanticError
*/
TEST(Semantics, ColorAndChdirErrors) {
    // COLOR fg string
    EXPECT_THROW({ auto ir = Compiler::compileString("10 COLOR \"X\"\n"); (void)ir; }, SemanticError);
    // COLOR bg string
    EXPECT_THROW({ auto ir = Compiler::compileString("10 COLOR 1, \"X\"\n"); (void)ir; }, SemanticError);
    // COLOR border string
    EXPECT_THROW({ auto ir = Compiler::compileString("10 COLOR 1,2, \"X\"\n"); (void)ir; }, SemanticError);
    // CHDIR with non-string
    EXPECT_THROW({ auto ir = Compiler::compileString("10 CHDIR 3\n"); (void)ir; }, SemanticError);
}
