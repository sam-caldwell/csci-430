// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.NonConst_Nested_3D_Arrays
 * Purpose: Exercise emitIfBlock with a non-constant condition, deep nesting (WHILE, FOR, nested IF),
 *          multi-dimensional array indexing (numeric and string, 3D), MID$ with and without len on
 *          array elements, and ON GOTO/GOSUB in both THEN and ELSE bodies. Place early-terminating
 *          statements at the end to avoid short-circuiting generation.
 * Components Under Test: CodeGenerator emitIfBlock/emitWhile/emitFor/emitExpr array paths
 * Expected Behavior: IR contains if_then/if_else/if_end labels, nested while/for labels, array
 *                    bounds plumbing, MID$/strncpy, and ON ... switch tables for both THEN/ELSE.
 */
TEST(CodeGenIfBlock, NonConst_Nested_3D_Arrays) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM A(2,2,2)\n"
        "21 DIM T$(2,2,2)\n"
        "30 X=1: Y=2\n"
        "40 IF X < Y THEN\n"
        // THEN: multidim numeric and string array assigns; MID$ on array element (no len)
        "50 A(1,2,2) = A(1,1,1) + 3\n"
        "55 T$(2,1,2) = \"S\"\n"
        "57 MID$(T$(1,1,1), 2) = \"K\"\n"
        // prints: string then numeric to use _sp variants on non-final items
        "60 PRINT T$(1,1,1), A(1,1,1)+0.5, \" X\"\n"
        // RANDOMIZE with explicit seed path
        "65 RANDOMIZE 123\n"
        // Nested WHILE to ensure deeper nesting
        "70 I=0\n"
        "75 WHILE I < 1\n"
        "80 PRINT I, I+0.25\n"
        "85 I=I+1\n"
        "90 WEND\n"
        // Nested FOR
        "95 FOR J=1 TO 2\n"
        "96 PRINT J\n"
        "97 NEXT J\n"
        // Nested IF under THEN (non-constant)
        "98 IF X<>0 THEN PRINT \"NEST\" ELSE PRINT \"ALT\"\n"
        // ON ... in THEN
        "99 ON 1 GOTO 200,300\n"
        "100 ON 2 GOSUB 400,500\n"
        // Input into variable to cover InputStmt path
        "105 INPUT Z\n"
        // End THEN with a GOTO to mark termination after other statements are emitted
        "110 GOTO 900\n"
        "120 ELSE\n"
        // ELSE: different multidim indices; MID$ on scalar string with len
        "130 A(2,2,2) = 5\n"
        "135 T$(2,2,1) = \"Q\"\n"
        "140 MID$(S$,1,1)=\"Z\"\n"
        // prints in ELSE (string and numeric)
        "145 PRINT \"ELSE\", 7\n"
        // ON ... in ELSE
        "150 ON 0 GOTO 210,220\n"
        "155 ON 1 GOSUB 230,240\n"
        // Place terminating statements at the end to avoid hiding codegen above
        "160 STOP\n"
        "165 SYSTEM\n"
        "170 END IF\n"
        // Targets for ON and GOSUB/GOTO
        "200 END\n"
        "210 END\n"
        "220 END\n"
        "230 RETURN\n"
        "240 RETURN\n"
        "300 END\n"
        "400 RETURN\n"
        "500 RETURN\n"
        "900 END\n";

    std::string ir = Compiler::compileString(src);
    // IF structure
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    ASSERT_NE(ir.find("_if_end"), std::string::npos);
    // Nested loop labels
    ASSERT_NE(ir.find("_while_cond"), std::string::npos);
    ASSERT_NE(ir.find("_for_cond"), std::string::npos);
    // MID$ and array element addressing present
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    // ON GOTO/GOSUB switch dispatch plumbing for THEN and ELSE
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
    // RANDOMIZE with seed
    ASSERT_NE(ir.find("@srand48"), std::string::npos);
}
