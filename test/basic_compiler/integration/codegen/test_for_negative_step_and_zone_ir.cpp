// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.For_Negative_Step_And_Zones_IR
 * Purpose: Exercise computeForCond() negative-step branch and PRINT zone padding inside FOR.
 */
TEST(Integration, For_Negative_Step_And_Zones_IR) {
    const char* src =
        "10 FOR I=3 TO 1 STEP -1\n"
        "20   PRINT I,\n"
        "30 NEXT I\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Negative step check in condition
    EXPECT_NE(ir.find("fcmp olt double"), std::string::npos) << ir; // step < 0
    // PRINT comma padding for zones
    EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos) << ir;
}

