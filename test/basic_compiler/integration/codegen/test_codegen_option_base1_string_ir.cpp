// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.StringArray_With_OptionBase1_IR_UsesUb
 * Purpose: Ensure OPTION BASE 1 sets the string array extent equal to upper bound.
 * Components Under Test: CodeGenerator string array allocation under OPTION BASE 1.
 * Expected Behavior: IR alloca uses [5 x ptr] for DIM S$(5) with BASE 1.
 */
TEST(Integration, StringArray_With_OptionBase1_IR_UsesUb) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM S$(5)\n"
        "30 S$(1)=\"Z\"\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Under BASE 1, extent == upper bound (5)
    ASSERT_NE(ir.find("alloca [5 x ptr]"), std::string::npos);
}
