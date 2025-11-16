// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.WIDTH_SCRN_Clamp_IR
 * Purpose: Verify WIDTH "SCRN:",n clamps current cursor col if beyond new width.
 * Expected: IR touches @gwb_cur_col and compares against width-1 with icmp sgt/select store.
 */
TEST(Integration, WIDTH_SCRN_Clamp_IR) {
    const char* src =
        "10 LOCATE 1,80\n"
        "20 WIDTH \"SCRN:\", 40\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_screen_cols"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@gwb_cur_col"), std::string::npos) << ir;
    EXPECT_NE(ir.find("icmp sgt i32"), std::string::npos) << ir;
}

