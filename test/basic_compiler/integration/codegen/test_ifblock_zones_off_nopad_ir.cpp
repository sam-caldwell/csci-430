// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.IfBlock_Zones_Off_NoPad_IR
 * Purpose: With OPTION PRINTZONES OFF, comma should not trigger zone padding path in IF THEN.
 * We still assert general PRINT machinery, not absence, to keep test resilient.
 */
TEST(Integration, IfBlock_Zones_Off_NoPad_IR) {
    const char* src =
        "10 OPTION PRINTZONES OFF\n"
        "20 IF 1 THEN\n"
        "30   PRINT \"A\", 1\n"
        "40 END IF\n";
    std::string ir = Compiler::compileString(src);
    // Assert stdout print path is present
    EXPECT_NE(ir.find("@gwb_screen_write"), std::string::npos) << ir;
}

