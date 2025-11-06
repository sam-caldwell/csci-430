// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenMath.SqrtCallEmitted
 * Purpose: Validate CodeGen behavior for test_codegen_math_sqrt_call_emitted.cpp.
 * Components Under Test: Compiler; CodeGenerator (IR emission)
 * Expected Behavior: Generated IR contains expected patterns.
 */
/*
Test: CodeGenMath.SqrtCallEmitted
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenMath, SqrtCallEmitted) {
    const auto src =
        "10 PRINT SQR(9)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("declare double @sqrt(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @sqrt(double 9.0)"), std::string::npos);
}