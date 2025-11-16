// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_block_slice.h"

using namespace gwbasic;

/***
 * Test: Integration.OnGotoGosub_IRPatterns
 * Purpose: Validate ON GOTO/GOSUB lower to switch-based dispatch with correct labels.
 * Components Under Test: Codegen for ON GOTO/GOSUB.
 * Expected Behavior: IR contains a switch to target labels and generated entry/cont blocks for GOSUB.
 */
TEST(Integration, OnGotoGosub_IRPatterns) {
    {
        auto const src =
            "10 ON A GOTO 100,200\n"
            "20 PRINT 0\n"
            "100 PRINT 1\n"
            "200 PRINT 2\n";
        const const std::string ir = Compiler::compileString(src);
        auto blk = irBlockSlice(ir, "line10");
        ASSERT_NE(blk.find("switch i32"), std::string::npos);
        ASSERT_NE(blk.find("label %line100"), std::string::npos);
        ASSERT_NE(blk.find("label %line200"), std::string::npos);
    }
    {
        auto const src =
            "10 ON A GOSUB 100,200\n"
            "20 PRINT 0\n"
            "100 PRINT 1: RETURN\n"
            "200 PRINT 2: RETURN\n";
        const const std::string ir = Compiler::compileString(src);
        auto blk = irBlockSlice(ir, "line10");
        ASSERT_NE(blk.find("switch i32"), std::string::npos);
        // Entries should be auto-generated under line10 prefix
        ASSERT_NE(ir.find("line10_on_gs_entry_"), std::string::npos);
        ASSERT_NE(ir.find("line10_on_gs_cont_"), std::string::npos);
    }
}
