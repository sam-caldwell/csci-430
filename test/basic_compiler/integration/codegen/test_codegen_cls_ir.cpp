// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGen.CLS_EmitsMemsetAndCursorReset
 * Purpose: Ensure CLS lowers to memset of @gwb_screen and resets cursor row/col to 0.
 */
TEST(CodeGen, CLS_EmitsMemsetAndCursorReset) {
    const std::string src =
        "10 PRINT \"HI\"\n"
        "20 CLS\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src.c_str());
    // Expect a memset to clear 2000 bytes and stores to cursor globals
    EXPECT_NE(ir.find("call ptr @memset(ptr @gwb_screen, i32 0, i64 2000)"), std::string::npos);
    EXPECT_NE(ir.find("store i32 0, ptr @gwb_cur_row"), std::string::npos);
    EXPECT_NE(ir.find("store i32 0, ptr @gwb_cur_col"), std::string::npos);
}

