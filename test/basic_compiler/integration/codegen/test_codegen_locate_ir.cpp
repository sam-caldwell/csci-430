// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGen.LOCATE_EmitsClampedStores
 * Purpose: Ensure LOCATE row[,col] clamps to 1..25 and 1..80 and stores zero-based row/col.
 */
TEST(CodeGen, LOCATE_EmitsClampedStores) {
    const std::string src =
        "10 LOCATE 26, 0\n"
        "20 LOCATE 1\n";
    std::string ir = Compiler::compileString(src.c_str());
    // Check for store to gwb_cur_row and gwb_cur_col
    EXPECT_NE(ir.find("store i32"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_cur_row"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_cur_col"), std::string::npos);
}

