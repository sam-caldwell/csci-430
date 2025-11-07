// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.DeepNesting_3D_Arrays
 * Purpose: Exercise emitWhile with non-constant condition, 3D arrays, nested WHILE and FOR,
 *          MID$ on array element without len, ON GOTO/GOSUB, and INPUT inside loop body.
 */
TEST(CodeGenWhile, DeepNesting_3D_Arrays) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM Q(2,2,2)\n"
        "21 DIM T$(2,2,2)\n"
        "30 I=0\n"
        "40 WHILE I < 1\n"
        "45 Q(2,1,2) = I+2\n"
        "46 T$(1,2,2) = \"M\"\n"
        "47 MID$(T$(2,1,1),2) = \"P\"\n"
        "49 ON 2 GOTO 200,210\n"
        "50 ON 1 GOSUB 300,310\n"
        "52 K=0\n"
        "53 WHILE K < 1\n"
        "54 PRINT T$(1,2,2), 5, 5.25\n"
        "55 K = K + 1\n"
        "56 WEND\n"
        "57 FOR N=1 TO 2 STEP 1\n"
        "58 PRINT N\n"
        "59 NEXT N\n"
        "60 INPUT Z\n"
        "61 I = I + 1\n"
        "62 WEND\n"
        "200 END\n"
        "210 END\n"
        "300 RETURN\n"
        "310 RETURN\n"
        "900 END\n";

    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_while_cond"), std::string::npos);
    ASSERT_NE(ir.find("_for_cond"), std::string::npos);
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
}
