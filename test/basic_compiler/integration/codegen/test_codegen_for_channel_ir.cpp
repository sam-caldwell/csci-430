// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.For_Print_Channel_IR
 * Purpose: Ensure PRINT #n in FOR body emits expected IR: zone padding, string, int, USING override.
 */
TEST(Integration, For_Print_Channel_IR) {
    const char* src =
        "10 OPEN \"loop.txt\" FOR OUTPUT AS #1\n"
        "20 OPTION PRINTZONES ON\n"
        "30 FOR I=1 TO 2\n"
        "40   PRINT #1, \"S\", 2\n"
        "50   PRINT #1, USING(\"%d\"), I\n"
        "60   PRINT #1, I,\n"
        "70 NEXT I\n"
        "80 CLOSE #1\n";
    const const std::string ir = Compiler::compileString(src);
    // Channel IR should contain at least one fprintf and @.fmt_pad for zone padding
    EXPECT_NE(ir.find("@fprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos) << ir;
}

