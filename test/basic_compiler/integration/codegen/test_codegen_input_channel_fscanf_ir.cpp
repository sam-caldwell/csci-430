// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.INPUT_Channel_EmitsFscanf
 * Purpose: Ensure INPUT #n emits @fscanf calls for numeric reads.
 */
TEST(Integration, INPUT_Channel_EmitsFscanf) {
    const char* src =
        "10 OPEN \"in.txt\" FOR INPUT AS #1\n"
        "20 INPUT #1, A, B\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare i32 @fscanf(ptr, ptr, ...)"), std::string::npos);
    ASSERT_NE(ir.find("call i32 (ptr, ptr, ...) @fscanf"), std::string::npos);
}

