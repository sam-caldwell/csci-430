// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../helper/ir_block_slice.h"

using namespace gwbasic;


/***
 * Test: CodeGenChain.AllPreservesAllAndBranches
 * Purpose: Validate CodeGen behavior for test_codegen_chain_all_preserves.cpp.
 * Components Under Test: Compiler; CodeGenerator (IR emission)
 * Expected Behavior: Generated IR contains expected patterns.
 */
TEST(CodeGenChain, AllPreservesAllAndBranches) {
    const auto src =
        "10 LET A = 1 : LET B = 2\n"
        "20 CHAIN \"P\", 100, ALL\n"
        "30 END\n"
        "100 PRINT A, B\n"
        "110 END\n";
    const std::string ir = Compiler::compileString(src);
    auto blk = irBlockSlice(ir, "line20");
    ASSERT_FALSE(blk.empty());
    // No resets for either variable
    EXPECT_EQ(blk.find("store double 0.0, ptr %A"), std::string::npos);
    EXPECT_EQ(blk.find("store double 0.0, ptr %B"), std::string::npos);
    EXPECT_NE(blk.find("br label %line100"), std::string::npos);
}
