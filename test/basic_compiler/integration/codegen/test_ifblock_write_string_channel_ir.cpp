// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.IfBlock_Write_String_Channel_IR
 * Purpose: Hit WRITE #n string paths in THEN and ELSE inside IF block.
 */
TEST(Integration, IfBlock_Write_String_Channel_IR) {
    const char* src =
        "10 OPEN \"w_if_str.txt\" FOR OUTPUT AS #1\n"
        "20 IF 1=1 THEN\n"
        "30   WRITE #1, \"HELLO\"\n"
        "40 ELSE\n"
        "50   WRITE #1, \"WORLD\"\n"
        "60 END IF\n"
        "70 CLOSE #1\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@fprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
}

