// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: CodeGen OPTION BASE (string arrays)
 * Purpose: Ensure DIM allocation length for string arrays honors OPTION BASE 1.
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
