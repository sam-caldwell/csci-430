// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenInputChannel.UnitFscanf
 * Purpose: INPUT #n lowers to fscanf in unit test context.
 */
TEST(CodeGenInputChannel, UnitFscanf) {
    const char* src =
        "10 INPUT #2, A\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare i32 @fscanf(ptr, ptr, ...)"), std::string::npos);
    ASSERT_NE(ir.find("call i32 (ptr, ptr, ...) @fscanf"), std::string::npos);
}

