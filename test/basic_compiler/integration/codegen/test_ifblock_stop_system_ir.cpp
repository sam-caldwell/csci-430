// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_Stop_System_IR
 * Purpose: Verify STOP and SYSTEM inside IF/ELSE produce break message and exit branch.
 */
TEST(Integration, IfBlock_Stop_System_IR) {
    const char* src =
        "10 IF 1 THEN\n"
        "20   STOP\n"
        "30 ELSE\n"
        "40   SYSTEM\n"
        "50 END IF\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@.msg_break"), std::string::npos) << ir;
    EXPECT_NE(ir.find("br label %exit"), std::string::npos) << ir;
}

