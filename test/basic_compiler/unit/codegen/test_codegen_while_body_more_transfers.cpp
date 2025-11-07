// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.Body_MoreTransfers
 * Purpose: Exercise additional emitWhile branches: GOTO, GOSUB inline, RETURN, END,
 *          and INPUT inside the loop body.
 */
TEST(CodeGenWhile, Body_MoreTransfers) {
    const auto src =
        "10 I=0\n"
        "20 WHILE I<1\n"
        "25 GOSUB 400\n"
        "26 GOTO 900\n"
        "27 INPUT Z\n"
        "28 RETURN\n"
        "29 END\n"
        "30 I=I+1\n"
        "40 WEND\n"
        "400 RETURN\n"
        "900 END\n";
    std::string ir = Compiler::compileString(src);
    // Look for gosub inline scaffolding
    ASSERT_NE(ir.find("_gosub_entry"), std::string::npos);
    ASSERT_NE(ir.find("_gosub_cont"), std::string::npos);
    // Look for goto target label and exit branching
    ASSERT_NE(ir.find(" br label %exit"), std::string::npos);
}
