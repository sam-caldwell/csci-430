// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenScreenStmt.EmitsGraphicsInit
 * Purpose: Ensure SCREEN statement triggers stub graphics initializer in IR.
 * Components Under Test: Compiler::compileString; CodeGenerator for SCREEN stmt
 * Expected Behavior: IR defines @gwb_graphics_init and calls it with mode.
 */
/*
Test: CodeGenScreenStmt.EmitsGraphicsInit
Inputs: Small BASIC program using SCREEN 1 and END
Code under test: Code generator IR emission for SCREEN statement
Expected behavior: IR contains 'define void @gwb_graphics_init' and a call
*/
TEST(CodeGenScreenStmt, EmitsGraphicsInit) {
    const char* src =
        "10 SCREEN 1\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("define void @gwb_graphics_init"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_graphics_init(i32"), std::string::npos);
}

