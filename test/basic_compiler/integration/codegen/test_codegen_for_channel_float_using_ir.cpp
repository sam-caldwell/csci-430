// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.For_Channel_Float_Using_IR
 * Purpose: Verify PRINT #n with USING("%f") on a float inside FOR body emits channel float path.
 */
TEST(Integration, For_Channel_Float_Using_IR) {
    const char* src =
        "10 OPEN \"f_for.txt\" FOR OUTPUT AS #1\n"
        "20 FOR I=1 TO 1\n"
        "30   PRINT #1, USING(\"%f\"), 3.14\n"
        "40 NEXT I\n"
        "50 CLOSE #1\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@fprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("double 3.140000"), std::string::npos) << ir; // baked literal path
}

