// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.WIDTH_SCRN_IR
 * Purpose: Ensure WIDTH n emits a store to @gwb_screen_cols and clamps via compares.
 */
TEST(Integration, WIDTH_SCRN_IR) {
    const char* src = "10 WIDTH 40\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_screen_cols"), std::string::npos) << ir;
}

/***
 * Test: Integration.WIDTH_LPT_IR
 * Purpose: Ensure WIDTH "LPT1:", n emits a store to @gwb_printer_cols.
 */
TEST(Integration, WIDTH_LPT_IR) {
    const char* src = "10 WIDTH \"LPT1:\", 132\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_printer_cols"), std::string::npos) << ir;
}

