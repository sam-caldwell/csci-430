// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.ERASE_ZerosArrays_IR
 * Purpose: Ensure ERASE emits zeroing stores for the specified arrays.
 * Components: Parser, Semantics, Codegen
 * Expected: IR contains GEPs into A_arr and a subsequent store 0.0/null
 */
/*
Test: Integration.ERASE_ZerosArrays_IR
Inputs: Program with DIM A(3), DIM S$(2), ERASE A,S$
Code under test: Full compile pipeline
Expected behavior: IR contains store float 0.0 to %A_arr and store ptr null to %S$_arr
*/
TEST(Integration, ERASE_ZerosArrays_IR) {
    const auto src =
        "10 DIM A(3)\n"
        "11 DIM S$(2)\n"
        "20 ERASE A,S$\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    // Look for array allocas
    ASSERT_NE(ir.find("%A_arr"), std::string::npos);
    ASSERT_NE(ir.find("%S$_arr"), std::string::npos);
    // Expect store to zero float elements for A
    ASSERT_NE(ir.find("getelementptr inbounds [4 x float], ptr %A_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("store float 0.0"), std::string::npos);
    // Expect store of null for string array S$
    ASSERT_NE(ir.find("getelementptr inbounds [3 x ptr], ptr %S$_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("store ptr null"), std::string::npos);
}

