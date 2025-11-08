// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.WIDTH_then_CLS_Smoke_IR
 * Purpose: Ensure CLS still emits after WIDTH changes (no regression in sequencing).
 */
TEST(Integration, WIDTH_then_CLS_Smoke_IR) {
    const char* src =
        "10 WIDTH 40\n"
        "20 CLS\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_screen_cols"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@gwb_screen"), std::string::npos) << ir; // memset target
}

