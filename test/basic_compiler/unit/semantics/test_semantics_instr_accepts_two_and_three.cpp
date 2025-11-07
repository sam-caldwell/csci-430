// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: SemanticsINSTR.Accepts_Two_Or_Three_Args
Inputs: PRINT INSTR("ABCD","BC") and PRINT INSTR(2,"ABAB","B")
Code under test: Compiler::compileString; Semantics arity/type checks
Expected behavior: Both forms compile without SemanticError
*/
TEST(SemanticsINSTR, Accepts_Two_Or_Three_Args) {
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT INSTR(\"ABCD\",\"BC\")\n"); });
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT INSTR(2,\"ABAB\",\"B\")\n"); });
}
