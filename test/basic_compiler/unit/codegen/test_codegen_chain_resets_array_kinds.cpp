// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenChain.ResetsArrayElementKinds
Purpose: Ensure CHAIN without ALL zeros arrays of all numeric kinds
         (Int16 %, Long32 &, Single !, Double #) in emit_line_block.
*/
TEST(CodeGenChain, ResetsArrayElementKinds) {
    const char* src =
        "10 DIM A%(2), B&(2), C!(2), D#(2)\n"
        "20 CHAIN \"P\", 100\n"
        "30 END\n"
        "100 END\n";

    std::string ir = Compiler::compileString(src);
    // Look for per-kind zero stores in the CHAIN block
    auto pos = ir.find("line20:");
    ASSERT_NE(pos, std::string::npos);
    const auto slice = ir.substr(pos, 800); // local slice of the CHAIN block
    EXPECT_NE(slice.find("store i32 0, ptr"), std::string::npos);   // Int16 array elements
    EXPECT_NE(slice.find("store i64 0, ptr"), std::string::npos);   // Long32 array elements
    EXPECT_NE(slice.find("store float 0.0, ptr"), std::string::npos); // Single array elements
    EXPECT_NE(slice.find("store double 0.0, ptr"), std::string::npos); // Double array elements
}

