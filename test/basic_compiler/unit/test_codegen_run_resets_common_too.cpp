// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../helper/ir_block_slice.h"

using namespace gwbasic;

/***
 * Test: CodeGenRun.RunResetsCommonAndBranches
 * Purpose: Validate CodeGen lowering for RUN resets even COMMON variables and branches correctly.
 * Components Under Test: Compiler; CodeGenerator (RunStmt lowering)
 * Expected Behavior: IR for the RUN line contains store 0.0 for both %A (COMMON) and %B,
 *                    and a branch to the specified target line.
 */
/*
Test: CodeGenRun.RunResetsCommonAndBranches
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenRun, RunResetsCommonAndBranches) {
    const auto src =
        "10 COMMON A\n"
        "20 LET A = 1 : LET B = 2\n"
        "30 RUN \"P\", 100\n"
        "40 END\n"
        "100 PRINT A, B\n"
        "110 END\n";
    std::string ir = Compiler::compileString(src);
    auto blk = irBlockSlice(ir, "line30");
    ASSERT_FALSE(blk.empty());
    EXPECT_NE(blk.find("store float 0.0, ptr %A"), std::string::npos);
    EXPECT_NE(blk.find("store float 0.0, ptr %B"), std::string::npos);
    EXPECT_NE(blk.find("br label %line100"), std::string::npos);
}
