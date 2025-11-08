// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.WHILE_MidAssign_Array_IR
 * Purpose: Exercise WHILE body with MID$ on array element to cover bounds checks and linear index math.
 */
TEST(Integration, WHILE_MidAssign_Array_IR) {
    const char* src =
        "10 DIM A$(2)\n"
        "20 I=1\n"
        "30 WHILE I<2\n"
        "40   MID$(A$(I),1)=\"Z\"\n"
        "50   I=I+1\n"
        "60 WEND\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("getelementptr inbounds ["), std::string::npos) << ir;
    EXPECT_NE(ir.find("@gwb_err_trap_line"), std::string::npos) << ir; // error dispatch helper presence
}

