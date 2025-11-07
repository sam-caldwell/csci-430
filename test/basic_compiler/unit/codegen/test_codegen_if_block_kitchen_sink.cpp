// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.KitchenSink
 * Purpose: Aggressively exercise emitIfBlock code paths with multidim arrays (string/numeric),
 *          MID$ (scalar and array element), PRINT (plain and USING), RANDOMIZE (time path),
 *          nested FOR, ON GOTO/GOSUB, INPUT, GOTO, STOP, SYSTEM.
 */
TEST(CodeGenIfBlock, KitchenSink) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM N(2,2)\n"
        "21 DIM S$(2,2)\n"
        "30 IF 1 < 2 THEN\n"
        "40 N(2,1) = 3\n"
        "41 S$(1,2) = \"Z\"\n"
        "42 MID$(S$(2,2),1) = \"X\"\n"
        "43 PRINT \"A\", 2, \" B\"\n"
        "44 RANDOMIZE\n"
        "45 FOR K=1 TO 2\n"
        "46 PRINT K\n"
        "47 NEXT K\n"
        "48 ON 1 GOTO 200,300\n"
        "49 ON 1 GOSUB 400,500\n"
        "50 INPUT Y\n"
        "51 GOTO 900\n"
        "60 ELSE\n"
        "61 N(1,1) = 1\n"
        "62 S$(2,1) = \"Y\"\n"
        "63 MID$(S$,1,1) = \"Q\"\n"
        "64 PRINT USING(\"X=#\"), 7\n"
        "65 STOP\n"
        "66 SYSTEM\n"
        "67 GOTO 910\n"
        "70 END IF\n"
        "80 END\n"
        "200 END\n"
        "300 END\n"
        "400 RETURN\n"
        "500 RETURN\n"
        "900 END\n"
        "910 END\n";
    std::string ir = Compiler::compileString(src);
    // Spot check a few signatures that indicate most branches were reached
    ASSERT_NE(ir.find("@snprintf"), std::string::npos) << "screen write via snprintf present";
    ASSERT_NE(ir.find("@srand48"), std::string::npos);
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
