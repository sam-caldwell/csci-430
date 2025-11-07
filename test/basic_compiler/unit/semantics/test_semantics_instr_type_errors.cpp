// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: SemanticsINSTR.TypeErrors
Inputs: INSTR(1,1), INSTR(1,2,3), INSTR("A",1), INSTR(1,"A",1)
Code under test: Compiler::compileString; Semantics type checks
Expected behavior: SemanticError thrown for non-string haystack/needle or non-numeric start
*/
TEST(SemanticsINSTR, TypeErrors) {
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR(1,1)\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR(1,2,3)\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR(\"A\",1)\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR(1,\"A\",1)\n"); }, SemanticError);
}
