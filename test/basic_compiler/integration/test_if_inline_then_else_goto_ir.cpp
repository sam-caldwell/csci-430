// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../helper/ir_block_slice.h"

using namespace gwbasic;

/***
 * Test: Integration.IfInlineThenElseGoto_IRPatterns
 * Purpose: Ensure inline IF with THEN/ELSE GOTO lowers to expected branches.
 */
/*
Test: Integration.IfInlineThenElseGoto_IRPatterns
Inputs: Program with inline THEN GOTO and ELSE GOTO
Code under test: Full compile to IR (no run)
Expected behavior: IR for line10 contains branch to then/else labels and goto targets inside
*/
TEST(Integration, IfInlineThenElseGoto_IRPatterns) {
    const char* src =
        "10 IF A = 0 THEN GOTO 100 ELSE GOTO 200\n"
        "20 END\n"
        "100 PRINT 1\n"
        "200 PRINT 2\n";
    const std::string ir = Compiler::compileString(src);
    // Expect dispatcher in line10 block
    const auto top = irBlockSlice(ir, "line10");
    ASSERT_FALSE(top.empty());
    ASSERT_NE(top.find(" br i1 "), std::string::npos);
    ASSERT_NE(top.find("_if_then"), std::string::npos);
    ASSERT_NE(top.find("_if_else"), std::string::npos);
    // Then body should branch to line100 and else body to line200
    ASSERT_NE(ir.find("line10_if_then"), std::string::npos);
    ASSERT_NE(ir.find("line10_if_else"), std::string::npos);
    ASSERT_NE(ir.find("line10_if_then"), std::string::npos);
    ASSERT_NE(ir.find("br label %line100"), std::string::npos);
    ASSERT_NE(ir.find("br label %line200"), std::string::npos);
}
