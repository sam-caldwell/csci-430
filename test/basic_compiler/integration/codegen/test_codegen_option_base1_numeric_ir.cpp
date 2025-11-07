// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Codegen_DIM_With_OptionBase1_Alloc_UsesUb
 * Purpose: Ensure OPTION BASE 1 makes the extent equal to upper bound for numeric arrays.
 * Components Under Test: CodeGenerator array allocation length under OPTION BASE 1.
 * Expected Behavior: IR alloca uses [5 x float] for DIM A(5) with BASE 1.
 */
TEST(Integration, Codegen_DIM_With_OptionBase1_Alloc_UsesUb) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM A(5)\n"
        "30 A(1)=1\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Under BASE 1, extent == upper bound (5); default numeric kind is Single => float
    ASSERT_NE(ir.find("alloca [5 x float]"), std::string::npos);
}
