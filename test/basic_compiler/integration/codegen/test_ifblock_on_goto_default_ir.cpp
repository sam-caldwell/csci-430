// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.IfBlock_OnGoto_Default_IR
 * Purpose: Trigger default (fallthrough) path of ON GOTO with index 0 inside IF/ELSE.
 */
TEST(Integration, IfBlock_OnGoto_Default_IR) {
    const char* src =
        "10 IF 1 THEN\n"
        "20   ON 0 GOTO 100,200\n"
        "30 ELSE\n"
        "40   ON 0 GOTO 300,400\n"
        "50 END IF\n"
        "60 PRINT \"X\"\n"
        "70 END\n"
        "100 PRINT \"A\"\n"
        "110 END\n"
        "200 PRINT \"B\"\n"
        "210 END\n"
        "300 PRINT \"C\"\n"
        "310 END\n"
        "400 PRINT \"D\"\n"
        "410 END\n";
    std::string ir = Compiler::compileString(src);
    // Verify ON generates switch (even though index 0 goes to default)
    EXPECT_NE(ir.find("switch i32"), std::string::npos) << ir;
}

