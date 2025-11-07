// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics_LEN_Array.Numeric_Array_Element_Accepts_And_Errors
Inputs: Programs using LEN(A(1)) with DIM A(...) declared; and error cases:
        LEN(A(1)) when A is not DIM'd, LEN(A(1)) when DIM dimensions mismatch,
        and LEN(A("X")) with string index.
Code under test: Compiler::compileString; Semantics analyzeExpr for LEN on arrays
Expected behavior: Accept LEN on numeric array element; reject when array missing,
                   when index arity mismatches, and when index is string-typed.
*/
TEST(Semantics_LEN_Array, Numeric_Array_Element_Accepts_And_Errors) {
    // Accept: numeric array element reference
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 DIM A(3): PRINT LEN(A(2))\n"); });

    // Reject: array not DIM'd
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT LEN(A(1))\n"); }, SemanticError);

    // Reject: DIM dimensions mismatch (expects 2 indices)
    EXPECT_THROW({ (void)Compiler::compileString("10 DIM A(2,3): PRINT LEN(A(1))\n"); }, SemanticError);

    // Reject: string index
    EXPECT_THROW({ (void)Compiler::compileString("10 DIM A(2): PRINT LEN(A(\"X\"))\n"); }, SemanticError);
}
