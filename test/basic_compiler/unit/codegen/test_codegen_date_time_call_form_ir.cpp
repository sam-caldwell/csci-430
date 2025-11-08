// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenDateTime.CallFormIR
 * Purpose: Exercise DATE$() and TIME$() call forms to cover CallExpr branches.
 */
TEST(CodeGenDateTime, CallFormIR) {
    const char* src =
        "10 PRINT DATE$()\n"
        "20 PRINT TIME$()\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@strftime"), std::string::npos);
    ASSERT_NE(ir.find("@localtime"), std::string::npos);
}

