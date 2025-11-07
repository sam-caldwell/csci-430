// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenClear.EmitsStringNullAndDataIdxReset
 * Purpose: Ensure CLEAR resets string vars to null and rewinds DATA index in IR.
 * Components Under Test: Compiler::compileString; Codegen for CLEAR
 * Expected: IR contains store ptr null to %S$ and store 0 to @gwb_data_idx.
 */
TEST(CodeGenClear, EmitsStringNullAndDataIdxReset) {
    const auto src =
        "10 S$=\"HI\"\n"
        "20 CLEAR\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("store ptr null, ptr %S$"), std::string::npos);
    ASSERT_NE(ir.find("store i32 0, ptr @gwb_data_idx"), std::string::npos);
}

