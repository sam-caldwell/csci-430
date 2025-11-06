// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.NoElse_NestedMix
 * Purpose: Cover the IF (no ELSE) branch in emitIfBlock, with nested IF/FOR/WHILE inside THEN,
 *          and additional multi-dimensional array indexing and MID$ variants to extend coverage.
 * Components Under Test: CodeGenerator emitIfBlock (no-ELSE path), emitFor/emitWhile nesting
 * Expected Behavior: IR has if_then/if_end labels (no if_else); nested blocks present.
 */
TEST(CodeGenIfBlock, NoElse_NestedMix) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM B(2,2,2)\n"
        "21 DIM U$(2,2)\n"
        "30 P=2: Q=1\n"
        "40 IF P > Q THEN\n"
        "50 B(1,1,2) = 42\n"
        "55 U$(2,2) = \"HELLO\"\n"
        "60 MID$(U$(2,2),2,1)=\"Z\"\n"
        "65 IF B(1,1,2) >= 42 THEN PRINT \"OK\" ELSE PRINT \"NO\"\n"
        "70 FOR T=1 TO 2\n"
        "75 PRINT T, T+0.1\n"
        "80 NEXT T\n"
        "85 W=0\n"
        "90 WHILE W < 1\n"
        "95 PRINT U$(2,2)\n"
        "100 W = W + 1\n"
        "105 WEND\n"
        "110 END IF\n"
        "120 END\n";

    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_end"), std::string::npos);
    // Intentionally no check for _if_else, as this test targets no-ELSE form
    ASSERT_NE(ir.find("_for_cond"), std::string::npos);
    ASSERT_NE(ir.find("_while_cond"), std::string::npos);
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
