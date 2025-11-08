// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Print_Channel_String_And_Int_IR
 * Purpose: Ensure PRINT #n on string and integer items emits snprintf + fwrite path.
 */
TEST(Integration, Print_Channel_String_And_Int_IR) {
    const char* src =
        "10 OPEN \"ch_print.txt\" FOR OUTPUT AS #1\n"
        "20 IF 1=1 THEN\n"
        "30   PRINT #1, \"X\";\n"
        "40   PRINT #1, 42\n"
        "50 END IF\n"
        "60 CLOSE #1\n";
    const std::string ir = Compiler::compileString(src);
    // Expect both snprintf and fwrite patterns present
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
}

/***
 * Test: Integration.Print_USING_With_Channel_IR
 * Purpose: Ensure PRINT #n, USING("%04d"), 7 follows snprintf + fwrite path.
 */
TEST(Integration, Print_USING_With_Channel_IR) {
    const char* src =
        "10 OPEN \"ch_using.txt\" FOR OUTPUT AS #1\n"
        "20 PRINT #1, USING(\"%04d\"), 7\n"
        "30 CLOSE #1\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
}

/***
 * Test: Integration.Print_Zones_Channel_Trailing_Comma_IR
 * Purpose: With OPTION PRINTZONES ON, trailing comma to channel pads via snprintf+fwrite.
 */
TEST(Integration, Print_Zones_Channel_Trailing_Comma_IR) {
    const char* src =
        "10 OPEN \"ch_zone.txt\" FOR OUTPUT AS #1\n"
        "20 OPTION PRINTZONES ON\n"
        "30 PRINT #1, 1,\n"
        "40 CLOSE #1\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
}

