// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.SubroutineInline_Kitchen_Sink_IR
 * Purpose: Drive multiple branches in emit_subroutine_inline: Assign, IF, GOTO, FOR, NEXT, RETURN.
 */
TEST(Integration, SubroutineInline_Kitchen_Sink_IR) {
    const char* src =
        "10 X=0\n"
        "20 GOSUB 100\n"
        "30 END\n"
        "100 I=1\n"
        "110 IF I<2 THEN 130\n"
        "120 GOTO 150\n"
        "130 FOR J=1 TO 2\n"
        "140 I=I+1\n"
        "145 NEXT J\n"
        "150 RETURN\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("_gosub_entry"), std::string::npos) << ir;
    EXPECT_NE(ir.find("_gosub_cont"), std::string::npos) << ir;
    EXPECT_NE(ir.find("IfStmt ->"), std::string::npos) << ir; // log marker typically present
    EXPECT_NE(ir.find("For var="), std::string::npos) << ir;   // semantics/log marker for FOR
}

