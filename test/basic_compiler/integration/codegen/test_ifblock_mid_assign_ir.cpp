// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_MidAssign_IR
 * Purpose: Ensure MID$ assignments (scalar and array element) inside THEN emit strncpy and pointer math.
 */
TEST(Integration, IfBlock_MidAssign_IR) {
    const char* src =
        "10 A$=\"HELLO\"\n"
        "20 DIM B$(2)\n"
        "30 IF 1 THEN\n"
        "40   MID$(A$,2)=\"Z\"\n"
        "50   MID$(B$(1),3,1)=\"Q\"\n"
        "60 END IF\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@strncpy"), std::string::npos) << ir;
    EXPECT_NE(ir.find("getelementptr inbounds i8, ptr"), std::string::npos) << ir;
}

