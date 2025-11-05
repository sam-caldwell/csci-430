// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenClear.EmitsVariableResets
 * Purpose: Ensure CLEAR resets scalars to 0.0 in IR after prior assignments.
 * Components Under Test: Compiler::compileString; Codegen for CLEAR
 * Expected: IR contains store of non-zero followed by store of 0.0 (typed) for vars.
 */
/*
Test: CodeGenClear.EmitsVariableResets
Inputs: Program assigning A and B then CLEAR
Code under test: Code generator IR emission
Expected behavior: IR contains 'store double 0.0, ptr %A' and for %B
*/
TEST(CodeGenClear, EmitsVariableResets) {
    const char* src =
        "10 A=5: B=9\n"
        "20 CLEAR\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("fptrunc double 5.0 to float"), std::string::npos);
    ASSERT_NE(ir.find("fptrunc double 9.0 to float"), std::string::npos);
    ASSERT_NE(ir.find("store float 0.0, ptr %A"), std::string::npos);
    ASSERT_NE(ir.find("store float 0.0, ptr %B"), std::string::npos);
}
