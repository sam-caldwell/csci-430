// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenFor.Body_Pad_Channel_And_EmptyPrint
 * Purpose: Drive emitFor branches for zone padding (OPTION PRINTZONES ON),
 *          channel printing with strings and numbers, and empty PRINT inside
 *          the FOR body (trailing terminator path).
 * Components Under Test: CodeGenerator::emitFor and related PRINT lowering.
 * Expected Behavior: IR contains pad helpers (@.fmt_pad, @.spaces_14),
 *          channel file array (@gwb_files) with fprintf, and fmt/empty for
 *          empty PRINT path.
 */
TEST(CodeGenFor, Body_Pad_Channel_And_EmptyPrint) {
    const std::string src =
        "10 OPTION PRINTZONES ON\n"
        // FOR with assignments and various PRINT forms in body
        "20 FOR I = 1 TO 1\n"
        "30 S$ = \"Z\"\n"
        "40 PRINT \"X\", 5\n"
        "50 PRINT #1, \"Y\", 7\n"
        "60 PRINT\n"
        "70 NEXT\n"
        "80 END\n";
    const const std::string ir = Compiler::compileString(src);
    // Zone pad helper present
    EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos);
    EXPECT_NE(ir.find("@.spaces_14"), std::string::npos);
    // Channel file printing evidence
    EXPECT_NE(ir.find("@gwb_files"), std::string::npos);
    EXPECT_NE(ir.find("@fprintf"), std::string::npos);
    // Empty PRINT uses format and empty string
    EXPECT_NE(ir.find("@.fmt_str"), std::string::npos);
    EXPECT_NE(ir.find("@.str_empty"), std::string::npos);
}

