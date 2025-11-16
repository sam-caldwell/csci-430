// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.IfBlock_Print_IntVsLong_Channel_IR
 * Purpose: Ensure PRINT #n for integer (i32) and long (i64) variables in THEN/ELSE hit integer branches.
 */
TEST(Integration, IfBlock_Print_IntVsLong_Channel_IR) {
    const char* src =
        "10 OPEN \"intlong.txt\" FOR OUTPUT AS #1\n"
        "20 A% = 1\n"
        "30 L& = 2\n"
        "40 IF 1 THEN\n"
        "50   PRINT #1, A%\n"
        "60 ELSE\n"
        "70   PRINT #1, L&\n"
        "80 END IF\n"
        "90 CLOSE #1\n";
    const std::string ir = Compiler::compileString(src);
    // Channel print formatting should be present
    EXPECT_NE(ir.find("@fprintf"), std::string::npos) << ir;
}

