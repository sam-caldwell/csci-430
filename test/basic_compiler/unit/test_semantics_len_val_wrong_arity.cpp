// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: Semantics_LEN_VAL.Rejects_Wrong_Arity
Inputs: LEN(), LEN("A","B"), VAL(), VAL("1","2")
Code under test: Compiler::compileString; Semantics arity checks
Expected behavior: SemanticError thrown for arg counts other than 1
*/
TEST(Semantics_LEN_VAL, Rejects_Wrong_Arity) {
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT LEN()\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT LEN(\"A\",\"B\")\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT VAL()\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT VAL(\"1\",\"2\")\n"); }, SemanticError);
}

