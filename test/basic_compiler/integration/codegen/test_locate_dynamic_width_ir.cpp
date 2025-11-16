// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.LOCATE_Uses_Dynamic_Width_IR
 * Purpose: Ensure LOCATE col clamp loads @gwb_screen_cols (dynamic width) and clamps accordingly.
 */
TEST(Integration, LOCATE_Uses_Dynamic_Width_IR) {
    const char* src =
        "10 WIDTH 40\n"
        "20 LOCATE 1, 100\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("load i32, ptr @gwb_screen_cols"), std::string::npos) << ir;
}

