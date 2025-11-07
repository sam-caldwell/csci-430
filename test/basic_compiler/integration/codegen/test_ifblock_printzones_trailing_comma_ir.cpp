// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_PrintZones_TrailingComma_IR
 * Purpose: Ensure trailing comma in PRINT uses zone padding inside IF blocks when OPTION PRINTZONES ON.
 * Components Under Test: Codegen (PRINT trailing comma in IF block).
 * Expected Behavior: IR references @.fmt_pad and @.spaces_14.
 */
TEST(Integration, IfBlock_PrintZones_TrailingComma_IR) {
    const char* src =
        "10 OPTION PRINTZONES ON\n"
        "20 IF 1=1 THEN\n"
        "30   PRINT ,\n"
        "40 END IF\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_pad"), std::string::npos);
    ASSERT_NE(ir.find("@.spaces_14"), std::string::npos);
}
