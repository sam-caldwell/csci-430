// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.StringArray_IR_UsesPtrElements
 * Purpose: Ensure DIM of string arrays allocates pointer elements and accesses use pointer stores/loads.
 * Components Under Test: CodeGenerator string array allocation and GEP addressing.
 * Expected Behavior: IR shows [extent x ptr] alloca, GEP into ptr array, and store ptr instructions.
 */
TEST(Integration, StringArray_IR_UsesPtrElements) {
    const char* src =
        "10 DIM A$(5)\n"
        "20 A$(2)=\"HI\"\n"
        "30 PRINT A$(2)\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    // Expect element type as ptr in gep and store ptr
    // Default OPTION BASE 0 -> upper bound 5 yields extent 6
    ASSERT_NE(ir.find("alloca [6 x ptr]"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [6 x ptr]"), std::string::npos);
    ASSERT_NE(ir.find("store ptr"), std::string::npos);
}
