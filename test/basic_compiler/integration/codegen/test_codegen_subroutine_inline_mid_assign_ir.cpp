// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.SubroutineInline_MidAssign_IR
 * Purpose: GOSUB inline body contains MID$ to cover emit_subroutine_inline code path.
 */
TEST(Integration, SubroutineInline_MidAssign_IR) {
    const char* src =
        "10 DIM B$(2)\n"
        "20 GOSUB 100\n"
        "30 END\n"
        "100 MID$(B$(1),1)=\"X\"\n"
        "110 RETURN\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("_gosub_entry"), std::string::npos) << ir;
    EXPECT_NE(ir.find("@strncpy"), std::string::npos) << ir;
}

