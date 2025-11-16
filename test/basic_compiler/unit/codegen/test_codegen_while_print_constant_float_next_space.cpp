// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.Print_ConstantFloat_NextSpace
 * Purpose: Hit emitWhile constant-float PRINT path and nextStartsWithSpace selection.
 */
TEST(CodeGenWhile, Print_ConstantFloat_NextSpace) {
    const auto src =
        "10 I=0\n"
        "20 WHILE I<1\n"
        "30 PRINT 4.25, \" X\"\n"
        "40 I=I+1\n"
        "50 WEND\n"
        "60 END\n";
    const std::string ir = Compiler::compileString(src);
    // Expect printf on constant double literal
    ASSERT_NE(ir.find("call i32 (ptr, ...) @printf(ptr"), std::string::npos);
}
