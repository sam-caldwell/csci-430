// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_Print_Channels_Else_IR
 * Purpose: Exercise ELSE branch printing to a file channel, including USING override and trailing comma pad.
 */
TEST(Integration, IfBlock_Print_Channels_Else_IR) {
    const char* src =
        "10 OPEN \"elsech.txt\" FOR OUTPUT AS #1\n"
        "20 OPTION PRINTZONES ON\n"
        "30 IF 1=2 THEN\n"
        "40   PRINT #1, \"SKIP\"\n"
        "50 ELSE\n"
        "60   PRINT #1, USING(\"%d\"), 9\n"
        "70   PRINT #1, 3,\n"
        "80 END IF\n"
        "90 CLOSE #1\n";
    const std::string ir = Compiler::compileString(src);
    // Expect snprintf + fwrite and padding usage
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos) << ir;
}

