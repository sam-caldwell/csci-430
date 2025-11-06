// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_block_slice.h"

using namespace gwbasic;

/*
Test: Integration.OnGotoGosub_IRPatterns
Inputs: Small programs using ON GOTO and ON GOSUB
Code under test: Full compile to IR
Expected behavior: IR contains switch-based dispatch to targets/entries
*/
TEST(Integration, OnGotoGosub_IRPatterns) {
    {
        const char* src =
            "10 ON A GOTO 100,200\n"
            "20 PRINT 0\n"
            "100 PRINT 1\n"
            "200 PRINT 2\n";
        const std::string ir = Compiler::compileString(src);
        auto blk = irBlockSlice(ir, "line10");
        ASSERT_NE(blk.find("switch i32"), std::string::npos);
        ASSERT_NE(blk.find("label %line100"), std::string::npos);
        ASSERT_NE(blk.find("label %line200"), std::string::npos);
    }
    {
        const char* src =
            "10 ON A GOSUB 100,200\n"
            "20 PRINT 0\n"
            "100 PRINT 1: RETURN\n"
            "200 PRINT 2: RETURN\n";
        const std::string ir = Compiler::compileString(src);
        auto blk = irBlockSlice(ir, "line10");
        ASSERT_NE(blk.find("switch i32"), std::string::npos);
        // Entries should be auto-generated under line10 prefix
        ASSERT_NE(ir.find("line10_on_gs_entry_"), std::string::npos);
        ASSERT_NE(ir.find("line10_on_gs_cont_"), std::string::npos);
    }
}

/*
Test: Integration.OnGotoGosub_IR_DefaultLabels
Inputs: Programs using ON GOTO and ON GOSUB
Expected behavior: switch has a default label to continuation (out-of-range)
*/
TEST(Integration, OnGotoGosub_IR_DefaultLabels) {
    {
        const char* src =
            "10 ON A GOTO 100,200\n"
            "20 PRINT 0\n"
            "100 PRINT 1\n"
            "200 PRINT 2\n";
        const std::string ir = Compiler::compileString(src);
        auto blk = irBlockSlice(ir, "line10");
        ASSERT_NE(blk.find("switch i32"), std::string::npos);
        // default label points to continuation label under line10 prefix
        ASSERT_NE(blk.find("label %line10_on_cont_"), std::string::npos);
        // label definition for continuation should exist as its own block
        size_t p = ir.find("\nline10_on_cont_");
        ASSERT_NE(p, std::string::npos);
        size_t c = ir.find(":\n", p);
        ASSERT_NE(c, std::string::npos);
    }
    {
        const char* src =
            "10 ON A GOSUB 100,200\n"
            "20 PRINT 0\n"
            "100 PRINT 1: RETURN\n"
            "200 PRINT 2: RETURN\n";
        const std::string ir = Compiler::compileString(src);
        auto blk = irBlockSlice(ir, "line10");
        ASSERT_NE(blk.find("switch i32"), std::string::npos);
        // default label points to gs continuation label under line10 prefix
        ASSERT_NE(blk.find("label %line10_on_gs_cont_"), std::string::npos);
        // label definition for gs continuation exists
        size_t p2 = ir.find("\nline10_on_gs_cont_");
        ASSERT_NE(p2, std::string::npos);
        size_t c2 = ir.find(":\n", p2);
        ASSERT_NE(c2, std::string::npos);
    }
}
