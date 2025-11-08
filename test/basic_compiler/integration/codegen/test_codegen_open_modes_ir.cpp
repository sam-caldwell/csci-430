// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Integration.Open_Modes_IR_ContainsModeStrings
Inputs: OPEN "r.txt" FOR INPUT AS #1 / OPEN "w.txt" FOR OUTPUT AS #2
Code under test: CodeGenerator (emit_line_block OPEN handling) + emit_globals
Expected behavior: IR references @.mode_r and @.mode_w via getelementptr and stores into @gwb_files slots.
*/
TEST(Integration, Open_Modes_IR_ContainsModeStrings) {
    const char* src =
        "10 OPEN \"r.txt\" FOR INPUT AS #1\n"
        "20 OPEN \"w.txt\" FOR OUTPUT AS #2\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@.mode_r"), std::string::npos);
    EXPECT_NE(ir.find("@.mode_w"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_files"), std::string::npos);
}

