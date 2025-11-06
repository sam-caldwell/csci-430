// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: Semantics_LEN_VAL.TypeChecks
Inputs: Programs using LEN("ABC"), VAL("123"); and LEN(1), VAL(1)
Code under test: Compiler::compileString; Semantics arity/type checks
Expected behavior: Accept string args; reject numeric args with SemanticError
*/
TEST(Semantics_LEN_VAL, TypeChecks) {
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT LEN(\"ABC\")\n"); });
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT VAL(\"123\")\n"); });

    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT LEN(1)\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT VAL(1)\n"); }, SemanticError);
}

