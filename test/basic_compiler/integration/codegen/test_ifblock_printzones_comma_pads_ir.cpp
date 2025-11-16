// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_PrintZones_CommaPads_IR
 * Purpose: Verify OPTION PRINTZONES ON induces zone padding after comma inside IF blocks.
 * Components Under Test: Codegen (PRINT zones in IF block code path).
 * Expected Behavior: IR references @.fmt_pad and @.spaces_14.
 */
TEST(Integration, IfBlock_PrintZones_CommaPads_IR) {
    const char* src =
        "10 OPTION PRINTZONES ON\n"
        "20 IF 1=1 THEN\n"
        "30   PRINT \"A\", 1\n"
        "40 END IF\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_pad"), std::string::npos);
    ASSERT_NE(ir.find("@.spaces_14"), std::string::npos);
}
