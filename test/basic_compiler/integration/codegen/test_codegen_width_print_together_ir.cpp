// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.WIDTH_PRINT_Together_IR
 * Purpose: Ensure WIDTH sets screen cols and subsequent PRINTs emit gwb_screen_write; verifies they work together.
 */
TEST(Integration, WIDTH_PRINT_Together_IR) {
    const char* src =
        "10 WIDTH 40\n"
        "20 PRINT \"HELLO\";\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_screen_cols"), std::string::npos) << ir; // WIDTH target
    EXPECT_NE(ir.find("@gwb_screen_write"), std::string::npos) << ir; // PRINT mirroring
}

