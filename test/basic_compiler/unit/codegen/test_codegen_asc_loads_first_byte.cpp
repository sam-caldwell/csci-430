// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenBuiltins.ASC_LoadsFirstByte
 * Inputs: PRINT ASC("C")
 * Code under test: Compiler::compileString() codegen for ASC.
 * Expected behavior: IR contains a load of the first byte of the string.
 */
TEST(CodeGenBuiltins, ASC_LoadsFirstByte) {
    const char* src =
        "10 PRINT ASC(\"C\")\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("load i8, ptr"), std::string::npos);
}
