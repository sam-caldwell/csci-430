// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenMath, SqrtCallEmitted) {
    const auto src =
        "10 PRINT SQR(9)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    // Should contain a call to @sqrt
    EXPECT_NE(ir.find("declare double @sqrt(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @sqrt(double 9.0)"), std::string::npos);
}

TEST(CodeGenMath, AbsCallEmitted) {
    const auto src =
        "10 PRINT ABS(-5)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("declare double @fabs(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @fabs(double"), std::string::npos);
}

