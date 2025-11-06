// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenRandomize.EmitsTimeAndSrandWhenNoArg
 * Purpose: Validate CodeGen lowering for RANDOMIZE with no argument (default seeding).
 * Components Under Test: Compiler; CodeGenerator (IR for RANDOMIZE default seed)
 * Expected Behavior: IR declares and calls time(ptr null) to obtain a seed;
 *                    IR declares @srand48(i64) for seeding.
 */
/*
Test: CodeGenRandomize.EmitsTimeAndSrandWhenNoArg
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenRandomize, EmitsTimeAndSrandWhenNoArg) {
    const auto src =
        "10 RANDOMIZE\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("declare i64 @time(ptr)"), std::string::npos);
    EXPECT_NE(ir.find("call i64 @time(ptr null)"), std::string::npos);
    EXPECT_NE(ir.find("declare void @srand48(i64)"), std::string::npos);
}
