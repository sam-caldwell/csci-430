// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/***
Test: CodeGenCHDIR.EmitsChdirCallAndDecl
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenCHDIR, EmitsChdirCallAndDecl) {
    const auto src =
        "10 CHDIR \"/tmp\"\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare i32 @chdir(ptr)"), std::string::npos);
    ASSERT_NE(ir.find("call i32 @chdir(ptr"), std::string::npos);
}
