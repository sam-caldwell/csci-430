// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Integration.PrintZones_NumberFollowedByString_SpaceVsNoSpace
Purpose: When OPTION PRINTZONES ON, numeric item chooses fmt _ns if the next item begins with a space; otherwise _sp.
*/
TEST(Integration, PrintZones_NumberFollowedByString_SpaceVsNoSpace) {
    // Case 1: next item begins with space -> use @.fmt_num_ns and @.fmt_int_ns
    {
        const char* src =
            "10 OPTION PRINTZONES ON\n"
            "20 PRINT 1, \" X\"\n"; // next string starts with space
        const std::string ir = Compiler::compileString(src);
        EXPECT_NE(ir.find("@.fmt_num_ns"), std::string::npos) << ir;
        EXPECT_NE(ir.find("@.fmt_int_ns"), std::string::npos) << ir;
    }
    // Case 2: next item does not start with space -> use _sp variants
    {
        const char* src =
            "10 OPTION PRINTZONES ON\n"
            "20 PRINT 1, \"X\"\n";
        const std::string ir = Compiler::compileString(src);
        EXPECT_NE(ir.find("@.fmt_num_sp"), std::string::npos) << ir;
        EXPECT_NE(ir.find("@.fmt_int_sp"), std::string::npos) << ir;
    }
}

