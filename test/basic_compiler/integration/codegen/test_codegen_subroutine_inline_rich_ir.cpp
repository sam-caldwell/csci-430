// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.SubroutineInline_Rich_IR_Coverage
 * Purpose: Exercise many branches in emitSubroutineInline: Assign, MID$ (var and array element),
 *          nested GOSUB, IF (comparison), FOR loop inside subroutine, INPUT with literal prompt.
 * Components Under Test: CodeGenerator::emitSubroutineInline
 * Expected Behavior: IR contains gosub entry/cont labels, strncpy/malloc/strlen for MID$, and for-loop labels.
 */
TEST(Integration, SubroutineInline_Rich_IR_Coverage) {
    const char* src =
        // Setup and call
        "10 OPTION BASE 1\n"
        "20 DIM A$(2)\n"
        "30 S$=\"ABCDE\"\n"
        "40 GOSUB 100\n"
        "50 PRINT \"DONE\"\n"
        "60 END\n"
        // Subroutine 100: covers string assign, RIGHT$/LEFT$/MID$ and array element MID$
        "100 S$=\"X\"+RIGHT$(S$,2)\n"
        "110 MID$(S$,2,2)=\"YZ\"\n"
        "130 MID$(A$(2),1)=LEFT$(\"HELLO\",3)\n"
        // Single-line IF THEN (supported in subroutine inline) and nested GOSUB
        "140 IF 1<2 THEN 150\n"
        "150 GOSUB 300\n"
        "160 X=X+1\n"
        // INPUT with literal prompt
        "180 INPUT \"P:\"; X\n"
        "190 RETURN\n"
        // Nested subroutine 300: FOR loop and PRINT
        "300 FOR I=1 TO 2\n"
        "310 X=X+1\n"
        "320 NEXT I\n"
        "330 RETURN\n";

    std::string ir = Compiler::compileString(src);
    // Subroutine inlining emits entry/cont labels and uses strncpy/malloc for MID$
    EXPECT_NE(ir.find("_gosub_entry"), std::string::npos);
    EXPECT_NE(ir.find("_gosub_cont"), std::string::npos);
    EXPECT_NE(ir.find("@strncpy"), std::string::npos);
    EXPECT_NE(ir.find("@malloc"), std::string::npos);
    EXPECT_NE(ir.find("@strlen"), std::string::npos);
    // FOR loop inside subroutine will generate labels/branches
    EXPECT_NE(ir.find("_for_"), std::string::npos);
}
