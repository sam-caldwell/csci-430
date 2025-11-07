// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenMath.AbsCallEmitted
 * Purpose: Validate CodeGen lowers ABS() calls to the correct libc intrinsic.
 * Components Under Test: Compiler; CodeGenerator (IR emission for ABS)
 * Expected Behavior: IR declares and calls @fabs(double) for ABS.
 */
TEST(CodeGenMath, AbsCallEmitted) {
    const auto src =
        "10 PRINT ABS(-5)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("declare double @fabs(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @fabs(double"), std::string::npos);
}
