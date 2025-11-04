// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenRND.EmitsHelperAndGlobal
 * Purpose: Validate CodeGen lowering for RND emits the helper and global state.
 * Components Under Test: Compiler; CodeGenerator (IR for RND)
 * Expected Behavior: IR defines global @gwb_last_rnd = global double 0.0;
 *                    IR defines helper @gwb_rnd(double) and calls it.
 */
/*
Test: CodeGenRND.EmitsHelperAndGlobal
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenRND, EmitsHelperAndGlobal) {
    const auto src =
        "10 PRINT RND(1)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_last_rnd = global double 0.0"), std::string::npos);
    EXPECT_NE(ir.find("define double @gwb_rnd(double"), std::string::npos);
    EXPECT_NE(ir.find("call double @gwb_rnd(double"), std::string::npos);
}
