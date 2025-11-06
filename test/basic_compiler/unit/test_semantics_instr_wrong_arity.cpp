// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: SemanticsINSTR.Rejects_Wrong_Arity
Inputs: INSTR(), INSTR("A"), INSTR("A","B","C","D")
Code under test: Compiler::compileString; Semantics arity checks
Expected behavior: SemanticError thrown for arg counts other than 2 or 3
*/
TEST(SemanticsINSTR, Rejects_Wrong_Arity) {
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR()\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR(\"A\")\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT INSTR(\"A\",\"B\",\"C\",\"D\")\n"); }, SemanticError);
}

