// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.Else_Input_And_On
 * Purpose: Cover ELSE-body INPUT, ON GOTO/GOSUB and STOP/SYSTEM routes in emitIfBlock.
 */
TEST(CodeGenIfBlock, Else_Input_And_On) {
    const char* src =
        "10 IF 0<1 THEN\n"
        "20 PRINT \"A\"\n"
        "30 ELSE\n"
        "40 INPUT X\n"
        "50 ON 2 GOTO 200,300\n"
        "60 ON 1 GOSUB 400,500\n"
        "70 STOP\n"
        "80 SYSTEM\n"
        "90 END IF\n"
        "100 END\n"
        "200 END\n"
        "300 END\n"
        "400 RETURN\n"
        "500 RETURN\n";
    const std::string ir = Compiler::compileString(src);
    // ELSE label present
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    // INPUT lowering in ELSE
    ASSERT_NE(ir.find("@scanf"), std::string::npos);
    // ON dispatch plumbing
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
}
