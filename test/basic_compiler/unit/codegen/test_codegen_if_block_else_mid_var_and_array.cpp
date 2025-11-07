// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.Else_MidScalarAndArray
 * Purpose: Exercise ELSE-body MID$ on scalar string and array element (with and
 *          without length argument) to cover ELSE MID$ paths.
 */
TEST(CodeGenIfBlock, Else_MidScalarAndArray) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM S$(2)\n"
        "30 S$=\"AB\"\n"
        "40 IF 1<2 THEN\n"
        "50 PRINT \"X\"\n"
        "60 ELSE\n"
        "70 MID$(S$,2,1)=\"Z\"\n"
        "80 MID$(S$(2),1)=\"Q\"\n"
        "90 END IF\n"
        "100 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
