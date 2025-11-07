// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.CLEAR_ZerosArrays_IR
 * Purpose: Ensure CLEAR emits zeroing stores for arrays DIM'd before it.
 * Components: Parser, Semantics, Codegen
 * Expected: IR contains GEP into A_arr and a subsequent store double 0.0
 */
TEST(Integration, CLEAR_ZerosArrays_IR) {
    const auto src =
        "10 DIM A(3)\n"
        "20 A(1)=9\n"
        "30 CLEAR\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Look for array alloca name and zero store pattern
    ASSERT_NE(ir.find("%A_arr"), std::string::npos);
    // Expect a store of 0.0 targeting a gep derived from %A_arr; default arrays are float
    ASSERT_NE(ir.find("getelementptr inbounds [4 x float], ptr %A_arr, i64 0, i64"), std::string::npos);
    ASSERT_NE(ir.find("store float 0.0"), std::string::npos);
}
