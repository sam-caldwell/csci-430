// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenColor.EmitsSgrPrintf
 * Inputs: Program with COLOR 2,4
 * Code under test: Compiler::compileString() IR generation for COLOR.
 * Expected behavior: IR references @.fmt_sgr and @printf for SGR output.
 */
TEST(CodeGenColor, EmitsSgrPrintf) {
    const auto src =
        "10 COLOR 2,4\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_sgr"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    // The first vararg should be literal 27 (ESC)
    ASSERT_NE(ir.find("call i32 (ptr, ...) @printf(ptr %"), std::string::npos);
    ASSERT_NE(ir.find(", i32 27, i32"), std::string::npos);
}
