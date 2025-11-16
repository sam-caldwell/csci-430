// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenChain.CommonPreservesArrays
 * Purpose: Combine CHAIN (non-ALL) with COMMON declarations and multiple array
 *          element kinds declared before the CHAIN line. Verifies that non-COMMON
 *          arrays are zeroed per kind while COMMON-named arrays are preserved.
 */
TEST(CodeGenChain, CommonPreservesArrays) {
    const auto src =
        "10 COMMON A, S$\n"
        "20 DIM A%(2)\n"
        "21 DIM B&(2)\n"
        "22 DIM C!(2)\n"
        "23 DIM D#(2)\n"
        "24 DIM S$(2)\n"
        "30 CHAIN \"P\", 100\n"
        "40 END\n"
        "100 END\n";

    const std::string ir = Compiler::compileString(src);
    auto p = ir.find("line30:");
    ASSERT_NE(p, std::string::npos);
    std::string blk = ir.substr(p, 1200);
    // Non-COMMON arrays should be zeroed per element kind
    EXPECT_NE(blk.find("store i32 0, ptr"), std::string::npos);      // Int16 elements
    EXPECT_NE(blk.find("store i64 0, ptr"), std::string::npos);      // Long32 elements
    EXPECT_NE(blk.find("store float 0.0, ptr"), std::string::npos);  // Single elements
    EXPECT_NE(blk.find("store double 0.0, ptr"), std::string::npos); // Double elements
    // COMMON S$ should preserve the string array S$(2): no nulling stores in CHAIN block
    EXPECT_EQ(blk.find("store ptr null, ptr"), std::string::npos);
}
