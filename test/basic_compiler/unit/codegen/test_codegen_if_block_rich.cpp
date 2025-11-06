// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.RichThenElseBodies
 * Purpose: Exercise emitIfBlock: THEN/ELSE bodies with string/numeric assigns,
 *          MID$, PRINT with format override and channel, and control flow.
 * Components Under Test: CodeGenerator emitIfBlock
 * Expected Behavior: IR contains if_then/if_else labels and corresponding prints/strncpy.
 */
TEST(CodeGenIfBlock, RichThenElseBodies) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM S$(2)\n"
        "21 DIM N(2)\n"
        "30 S$=\"HI\"\n"
        "40 IF 2 > 1 THEN\n"
        "50 S$=\"BYE\"\n"
        "55 MID$(S$,2,1)=\"Z\"\n"
        "60 PRINT USING(\"X=#\"), 42\n"
        "70 ELSE\n"
        "80 N(1)=1\n"
        "90 PRINT #1, \"ALT\"\n"
        "95 SYSTEM\n"
        "100 END IF\n"
        "110 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    // Format override path and channel printing
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    // MID$ lowering present
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
