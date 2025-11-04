// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenScreen.EmitsGlobalsAndHelper
 * Inputs: Program that prints "AB" and reads SCREEN(1,1)
 * Code under test: Compiler::compileString() IR emission for virtual screen.
 * Expected behavior: IR contains @gwb_screen global, @snprintf decl and @gwb_screen_write.
 */
TEST(CodeGenScreen, EmitsGlobalsAndHelper) {
    const char* src =
        "10 PRINT \"AB\"\n"
        "20 X=SCREEN(1,1)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@gwb_screen"), std::string::npos);
    ASSERT_NE(ir.find("declare i32 @snprintf"), std::string::npos);
    ASSERT_NE(ir.find("define void @gwb_screen_write"), std::string::npos);
}
