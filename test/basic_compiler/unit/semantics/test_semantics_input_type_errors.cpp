// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: SemanticsInput.ConsoleRejectsStringVars
Inputs: Programs using console INPUT with string variables
Code under test: SemanticAnalyzer (type checks for INPUT varlists)
Expected behavior: Throws SemanticError advising to use LINE INPUT
*/
TEST(SemanticsInput, ConsoleRejectsStringVars) {
    // Single string variable
    const char* src1 = "10 INPUT A$\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    // Mixed var-list with string
    const char* src2 = "10 INPUT N, S$\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}
