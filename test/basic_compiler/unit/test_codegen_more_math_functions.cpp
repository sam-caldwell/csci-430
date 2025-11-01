// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenMathMore, TrigLogExpCalls) {
    const auto src =
        "10 PRINT SIN(0)\n"
        "20 PRINT COS(0)\n"
        "30 PRINT TAN(0)\n"
        "40 PRINT ATN(1)\n"
        "50 PRINT LOG(1)\n"
        "60 PRINT EXP(1)\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("declare double @sin(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @sin(double 0.0)"), std::string::npos);
    EXPECT_NE(ir.find("declare double @cos(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @cos(double 0.0)"), std::string::npos);
    EXPECT_NE(ir.find("declare double @tan(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @tan(double 0.0)"), std::string::npos);
    EXPECT_NE(ir.find("declare double @atan(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @atan(double 1.0)"), std::string::npos);
    EXPECT_NE(ir.find("declare double @log(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @log(double 1.0)"), std::string::npos);
    EXPECT_NE(ir.find("declare double @exp(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @exp(double 1.0)"), std::string::npos);
}

TEST(CodeGenMathMore, IntFixSgn) {
    const auto src =
        "10 PRINT INT(1.9)\n"
        "20 PRINT FIX(-1.9)\n"
        "30 PRINT SGN(-2)\n"
        "40 PRINT SGN(0)\n"
        "50 PRINT SGN(2)\n"
        "60 END\n";
    std::string ir = Compiler::compileString(src);
    // INT -> floor
    EXPECT_NE(ir.find("declare double @floor(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @floor(double"), std::string::npos);
    // FIX -> fptosi/sitofp sequence
    EXPECT_NE(ir.find(" = fptosi double"), std::string::npos);
    EXPECT_NE(ir.find(" = sitofp i64"), std::string::npos);
    // SGN -> comparisons and add
    EXPECT_NE(ir.find(" = fcmp ogt double"), std::string::npos);
    EXPECT_NE(ir.find(" = fcmp olt double"), std::string::npos);
}

