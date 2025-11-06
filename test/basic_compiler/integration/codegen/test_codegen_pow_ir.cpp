// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Integration.Codegen_Emits_Pow_Call
Inputs: Program with exponentiation in an expression
Expected: IR declares @pow and calls it for '^'.
*/
TEST(Integration, Codegen_Emits_Pow_Call) {
    std::string src = R"(10 PRINT 2 ^ 3
)";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare double @pow(double, double)"), std::string::npos);
    ASSERT_NE(ir.find("call double @pow"), std::string::npos);
}

