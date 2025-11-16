// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.SWAP_Numeric_Array_Elements_IR
 * Purpose: Ensure SWAP A(1),B(1) on single-precision arrays emits GEPs and typed stores.
 * Components Under Test: Parser (SWAP array elements), Semantics (array typing), Codegen (element swap IR).
 * Expected Behavior: IR contains GEPs into arrays and store float instructions.
 */
TEST(Integration, SWAP_Numeric_Array_Elements_IR) {
    const auto src =
        "10 DIM A(2)\n"
        "11 DIM B(2)\n"
        "20 SWAP A(1), B(1)\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("%A_arr"), std::string::npos);
    ASSERT_NE(ir.find("%B_arr"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [3 x float], ptr %A_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [3 x float], ptr %B_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("store float"), std::string::npos);
}
