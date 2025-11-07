// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenIfBlock.Then_Rich_Features
Purpose: Exercise THEN-body INPUT, ON GOTO/GOSUB, MID$, array assign, PRINT USING and channel, STOP and SYSTEM.
*/
TEST(CodeGenIfBlock, Then_Rich_Features) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM S$(2)\n"
        "21 DIM N(2)\n"
        "30 IF 2>1 THEN\n"
        "40 INPUT X\n"
        "50 ON 1 GOTO 200,300\n"
        "60 ON 1 GOSUB 400,500\n"
        "70 S$=\"HI\": MID$(S$,2,1)=\"Z\"\n"
        "80 N(1)=1\n"
        "90 PRINT USING(\"%d\"), 42\n"
        "95 PRINT #1, \"CH\"\n"
        "100 STOP\n"
        "105 SYSTEM\n"
        "110 END IF\n"
        "120 END\n"
        "200 END\n"
        "300 END\n"
        "400 RETURN\n"
        "500 RETURN\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("@scanf"), std::string::npos);
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
}

