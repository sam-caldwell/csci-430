// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.IfBlock_Kitchen_Sink_IR
 * Purpose: Exercise a wide set of THEN/ELSE body statements to increase emit_if_block coverage
 *          (PRINT stdout/channel, USING, zones, WRITE #n variants, INPUT, LINE INPUT, ON GOTO/GOSUB,
 *           GOSUB/RETURN, WHILE, STOP/SYSTEM).
 */
TEST(Integration, IfBlock_Kitchen_Sink_IR) {
    const char* src =
        "10 OPTION PRINTZONES ON\n"
        "20 OPEN \"ks_if.txt\" FOR OUTPUT AS #1\n"
        "30 IF 1 THEN\n"
        // stdout PRINT variants
        "40   PRINT \"A\";\n"
        "50   PRINT \"B\",\n"
        "60   PRINT USING(\"%d\"), 42\n"
        // channel PRINT variants
        "70   PRINT #1, \"C\";\n"
        "80   PRINT #1, \"D\",\n"
        "90   PRINT #1, USING(\"%f\"), 3.14\n"
        // WRITE #n variants
        "100  WRITE #1, 5, 3.14, \"S\", \"\"\n"
        // INPUT and LINE INPUT to exercise those bodies
        "110  INPUT \"Q:\"; N\n"
        "120  LINE INPUT ; P$\n"
        // ON GOTO/GOSUB
        "130  ON 1 GOTO 200,300\n"
        "140  ON 1 GOSUB 400,500\n"
        // WHILE false body to generate structure
        "150  WHILE 0=1\n"
        "160    PRINT \"W\"\n"
        "170  WEND\n"
        // Termination paths
        "180  STOP\n"
        "190  SYSTEM\n"
        "195 ELSE\n"
        // Mirror some ELSE paths
        "200  PRINT \"E\",\n"
        "210  PRINT #1, USING(\"%d\"), 9\n"
        "220  WRITE #1, \"T\"\n"
        "230  GOSUB 500\n"
        "240 END IF\n"
        // Targets for ON branches and GOSUB
        "250 PRINT \"T1\"\n"
        "260 END\n"
        "300 PRINT \"T2\"\n"
        "310 END\n"
        "400 PRINT \"SG1\"\n"
        "410 RETURN\n"
        "500 PRINT \"SG2\"\n"
        "510 RETURN\n";
    std::string ir = Compiler::compileString(src);
    // A few smoke checks for key constructs (not exhaustive)
    EXPECT_NE(ir.find("@snprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fwrite"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@fprintf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@scanf"), std::string::npos) << ir;
    EXPECT_NE(ir.find("_while_cond"), std::string::npos) << ir;
    EXPECT_NE(ir.find("switch i32"), std::string::npos) << ir;
}

