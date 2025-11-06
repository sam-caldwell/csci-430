// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenWhile.MoreBranches
 * Purpose: Hit additional emitWhile branches: variable numeric print path (int detection),
 *          STOP and SYSTEM in loop body, string array assign.
 */
TEST(CodeGenWhile, MoreBranches) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM T$(2)\n"
        "30 I=0\n"
        "40 WHILE I < 2\n"
        "50 PRINT I+0\n"   // triggers integer detection path for non-constant expression
        "55 T$(1)=\"X\"\n"
        "60 IF I=1 THEN STOP ELSE SYSTEM\n"
        "70 I=I+1\n"
        "80 WEND\n"
        "90 END\n";
    std::string ir = Compiler::compileString(src);
    // Integer detection branches produce labels
    ASSERT_NE(ir.find("_wprint_int_"), std::string::npos);
    ASSERT_NE(ir.find("_wprint_flt_"), std::string::npos);
    // STOP/SYSTEM branches present
    ASSERT_NE(ir.find(".msg_break"), std::string::npos);
}

