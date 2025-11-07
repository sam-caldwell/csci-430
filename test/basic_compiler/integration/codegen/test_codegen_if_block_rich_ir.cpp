// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.IfBlock_Rich_IR_Coverage
 * Purpose: Exercise emitIfBlock with THEN/ELSE bodies containing assorted statements: assignments,
 *          RANDOMIZE (with and without seed), ON GOTO/ON GOSUB, INPUT (literal + var prompt),
 *          WHILE loop, GOSUB, RETURN, STOP/SYSTEM paths.
 * Components Under Test: CodeGenerator::emitIfBlock (then/else)
 * Expected Behavior: IR includes branches to then/else, switch for ON*, calls to srand48/time/scanf/printf, and while labels.
 */
TEST(Integration, IfBlock_Rich_IR_Coverage) {
    const char* src =
        "10 X=1\n"
        "20 Y=2\n"
        "30 IF X<Y THEN\n"
        "40 X=X+1\n"
        "50 RANDOMIZE 5\n"
        "60 ON X GOTO 200,300\n"
        "70 GOSUB 500\n"
        "80 RETURN\n"
        "90 ELSE\n"
        "100 RANDOMIZE\n"
        "110 INPUT \"Q:\"; Z\n"
        "120 INPUT ; P$, Z\n"
        "130 ON X GOSUB 400,500\n"
        "140 WHILE X<Y\n"
        "150 X=X+1\n"
        "160 WEND\n"
        "170 STOP\n"
        "180 SYSTEM\n"
        "190 END IF\n"
        "200 PRINT \"A\"\n"
        "210 END\n"
        "300 PRINT \"B\"\n"
        "310 END\n"
        "400 PRINT \"GS1\"\n"
        "410 RETURN\n"
        "500 PRINT \"GS2\"\n"
        "510 RETURN\n";

    std::string ir = Compiler::compileString(src);
    // IF block should have then/else labels and end
    EXPECT_NE(ir.find("_if_then"), std::string::npos);
    EXPECT_NE(ir.find("_if_else"), std::string::npos);
    EXPECT_NE(ir.find("_if_end"), std::string::npos);
    // RANDOMIZE with seed and without seed
    EXPECT_NE(ir.find("@srand48"), std::string::npos);
    EXPECT_NE(ir.find("@time"), std::string::npos);
    // ON GOTO/ON GOSUB produce switch in IR
    EXPECT_NE(ir.find("switch i32"), std::string::npos);
    // INPUT uses scanf and may print prompt
    EXPECT_NE(ir.find("@scanf"), std::string::npos);
    EXPECT_NE(ir.find("@printf"), std::string::npos);
    // WHILE emits labeled blocks
    EXPECT_NE(ir.find("_while_cond"), std::string::npos);
    EXPECT_NE(ir.find("_while_body"), std::string::npos);
}
