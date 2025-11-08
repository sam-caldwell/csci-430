// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenDefSeg.EmitsStoresToSeg
 * Purpose: DEF SEG with and without value emits stores to @gwb_seg.
 */
TEST(CodeGenDefSeg, EmitsStoresToSeg) {
    const char* src =
        "10 DEF SEG = 64\n"
        "20 DEF SEG\n";
    std::string ir = Compiler::compileString(src);
    // Look for store to @gwb_seg at least twice
    ASSERT_NE(ir.find("store i32"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_seg"), std::string::npos);
}

