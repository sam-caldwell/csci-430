// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_Print_TrailingTerms_IR
 * Purpose: Verify PRINT trailing semicolon and comma inside IF blocks to stdout and file channel.
 * Expected: Uses snprintf/fwrite for channel, gwb_screen_write for stdout, and @.fmt_pad for comma padding.
 */
TEST(Integration, IfBlock_Print_TrailingTerms_IR) {
    const char* src =
        "10 OPEN \"t_if_terms.txt\" FOR OUTPUT AS #1\n"
        "20 IF 1=1 THEN\n"
        "30   PRINT \"A\";\n"          // no newline to stdout
        "40   PRINT #1, \"B\",\n"       // comma pad to channel
        "50 ELSE\n"
        "60   PRINT \"C\",\n"         // comma pad to stdout
        "70   PRINT #1, \"D\";\n"       // no newline to channel
        "80 END IF\n"
        "90 CLOSE #1\n";
    const const std::string ir = Compiler::compileString(src);
    // stdout path should use gwb_screen_write
    EXPECT_NE(ir.find("@gwb_screen_write"), std::string::npos) << ir;
    // channel paths should use snprintf+fwrite
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
    // comma-based zone padding should appear
    EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos) << ir;
}

