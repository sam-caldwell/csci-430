// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.IfBlock_MidAssign_OOB_And_Using_IR
 * Purpose: Cover IF THEN MID$ array OOB error path and THEN-channel USING formatting.
 */
TEST(Integration, IfBlock_MidAssign_OOB_And_Using_IR) {
    const char* src =
        "10 OPEN \"f_if_kitchen.txt\" FOR OUTPUT AS #1\n"
        "20 DIM A$(1)\n"
        "30 IF 1 THEN\n"
        "40   MID$(A$(2),1)=\"Z\"\n" // OOB -> error dispatch path
        "50   PRINT #1, USING(\"%d\"), 7\n"
        "60 ELSE\n"
        "70   PRINT \"skip\"\n"
        "80 END IF\n"
        "90 CLOSE #1\n";
    std::string ir = Compiler::compileString(src);
    // MID$ OOB -> error dispatch code
    EXPECT_NE(ir.find("_mid_idx_err_"), std::string::npos) << ir;
    EXPECT_NE(ir.find("store i32 9, ptr @gwb_err_code"), std::string::npos) << ir;
    // THEN USING channel path
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
}

