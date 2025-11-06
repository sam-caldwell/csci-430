// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.MoreBranches
 * Purpose: Further exercise emitIfBlock paths: nested FOR, RANDOMIZE, ON GOTO/GOSUB, INPUT.
 */
TEST(CodeGenIfBlock, MoreBranches) {
    const auto src =
        "10 IF 1 < 2 THEN\n"
        "20 RANDOMIZE 5\n"
        "30 FOR J=1 TO 2\n"
        "40 PRINT J\n"
        "50 NEXT J\n"
        "60 ELSE\n"
        "70 ON 1 GOTO 200,300\n"
        "80 ON 1 GOSUB 400,500\n"
        "90 INPUT X\n"
        "100 END IF\n"
        "200 END\n"
        "300 END\n"
        "400 RETURN\n"
        "500 RETURN\n";
    std::string ir = Compiler::compileString(src);
    // Randomize uses srand48 path
    ASSERT_NE(ir.find("@srand48"), std::string::npos);
    // Nested FOR compiles
    ASSERT_NE(ir.find("_for_body"), std::string::npos);
    // ON GOTO/GOSUB present
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
    // INPUT path present
    ASSERT_NE(ir.find("@scanf"), std::string::npos);
}

