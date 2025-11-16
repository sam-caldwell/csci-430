// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenLIST.EmitsStdoutAndScreen
 * Purpose: Ensure LIST emits @printf and @gwb_screen_write paths.
 */
TEST(CodeGenLIST, EmitsStdoutAndScreen) {
    const char* src =
        "10 PRINT \"X\"\n"
        "20 LIST\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_screen_write"), std::string::npos);
}

