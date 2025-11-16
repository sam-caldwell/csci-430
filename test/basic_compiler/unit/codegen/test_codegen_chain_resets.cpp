// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_block_slice.h"

using namespace gwbasic;

/***
 * Test: CodeGenChain.ResetsNonCommonAndBranches
 * Purpose: Validate CodeGen behavior for test_codegen_chain_resets.cpp.
 * Components Under Test: Compiler; CodeGenerator (IR emission)
 * Expected Behavior: Generated IR contains expected patterns.
 */
TEST(CodeGenChain, ResetsNonCommonAndBranches) {
    const auto src =
        "10 COMMON A\n"
        "20 LET A = 1 : LET B = 2\n"
        "30 CHAIN \"P\", 100\n"
        "40 END\n"
        "100 PRINT A, B\n"
        "110 END\n";
    const std::string ir = Compiler::compileString(src);
    auto blk = irBlockSlice(ir, "line30");
    ASSERT_FALSE(blk.empty());
    // B should be reset (float), A should not
    EXPECT_NE(blk.find("store float 0.0, ptr %B"), std::string::npos);
    EXPECT_EQ(blk.find("store float 0.0, ptr %A"), std::string::npos);
    // Branch to line100
    EXPECT_NE(blk.find("br label %line100"), std::string::npos);
}
