// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.SWAP_String_Vars_IR
 * Purpose: Ensure SWAP A$,B$ lowers to ptr loads/stores.
 * Components: Parser, Semantics, Codegen
 * Expected: IR contains load/store of ptr for both variables.
 */
/*
Test: Integration.SWAP_String_Vars_IR
Inputs: Program with SWAP A$,B$
Code under test: Full compile pipeline
Expected behavior: IR contains load ptr,ptr %A$ and store ptr to %A$/%B$
*/
TEST(Integration, SWAP_String_Vars_IR) {
    const char* src =
        "10 A$=\"X\"\n"
        "11 B$=\"Y\"\n"
        "20 SWAP A$,B$\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("load ptr, ptr %A$"), std::string::npos);
    ASSERT_NE(ir.find("load ptr, ptr %B$"), std::string::npos);
    ASSERT_NE(ir.find("store ptr %"), std::string::npos);
}

/***
 * Test: Integration.SWAP_Numeric_Array_Elements_IR
 * Purpose: Ensure SWAP A(1),B(1) on single-precision arrays emits element GEPs and stores.
 * Components: Parser, Semantics, Codegen
 * Expected: IR contains GEPs into A_arr/B_arr and typed stores.
 */
/*
Test: Integration.SWAP_Numeric_Array_Elements_IR
Inputs: DIM A(2), DIM B(2), SWAP A(1),B(1)
Code under test: Full compile pipeline
Expected behavior: IR has getelementptr into [3 x float] arrays and store float
*/
TEST(Integration, SWAP_Numeric_Array_Elements_IR) {
    const char* src =
        "10 DIM A(2)\n"
        "11 DIM B(2)\n"
        "20 SWAP A(1), B(1)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("%A_arr"), std::string::npos);
    ASSERT_NE(ir.find("%B_arr"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [3 x float], ptr %A_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [3 x float], ptr %B_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("store float"), std::string::npos);
}

