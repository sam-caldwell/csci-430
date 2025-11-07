// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: Semantics_LEN.Additional_Cases
Inputs: Programs using LEN on non-variable numeric expressions (A+1), on array element
        arithmetic (A(1)+1), and on string array elements S$(i)
Code under test: Compiler::compileString; semantic analyzer LEN branches
Expected behavior: Reject LEN(A+1) and LEN(A(1)+1); accept LEN(S$(1))
*/
TEST(Semantics_LEN, Additional_Cases) {
    EXPECT_THROW({ (void)Compiler::compileString("10 A=1: PRINT LEN(A+1)\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 DIM A(2): A(1)=1: PRINT LEN(A(1)+1)\n"); }, SemanticError);
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 DIM S$(2): S$(1)=\"HI\": PRINT LEN(S$(1))\n"); });
}

