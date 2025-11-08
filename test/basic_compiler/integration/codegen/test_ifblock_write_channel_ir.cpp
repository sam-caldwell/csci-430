// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_Write_Channel_IR
 * Purpose: Ensure WRITE #n inside THEN and ELSE emits fprintf channel paths (int and float variants).
 */
TEST(Integration, IfBlock_Write_Channel_IR) {
    const char* src =
        "10 OPEN \"w_if.txt\" FOR OUTPUT AS #1\n"
        "20 IF 1=1 THEN\n"
        "30   WRITE #1, 5\n"
        "40 ELSE\n"
        "50   WRITE #1, 3.14\n"
        "60 END IF\n"
        "70 CLOSE #1\n";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@fprintf"), std::string::npos) << ir;
}

