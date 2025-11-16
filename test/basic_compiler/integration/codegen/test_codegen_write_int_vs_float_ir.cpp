// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Integration.Write_IntVsFloat_BranchesAndChannels
Purpose: Ensure WRITE chooses integer vs float formats, for stdout and channel.
*/
TEST(Integration, Write_IntVsFloat_BranchesAndChannels) {
    // stdout int vs float
    {
        const char* src =
            "10 WRITE 7, 7/2\n"; // integral then non-integral
        const const std::string ir = Compiler::compileString(src);
        // Expect int then float selection formats present
        EXPECT_NE(ir.find("@.fmt_int_sp"), std::string::npos) << ir;
        EXPECT_NE(ir.find("@.fmt_num_sp"), std::string::npos) << ir;
        // stdout path uses @printf for WRITE
        EXPECT_NE(ir.find("@printf(ptr"), std::string::npos) << ir;
    }
    // channel int vs float
    {
        const char* src =
            "10 OPEN \"o\" FOR OUTPUT AS #1\n"
            "20 WRITE #1, 5, 3.14\n";
        const const std::string ir = Compiler::compileString(src);
        EXPECT_NE(ir.find("@.fmt_int_sp"), std::string::npos) << ir;
        EXPECT_NE(ir.find("@.fmt_num_sp"), std::string::npos) << ir;
        EXPECT_NE(ir.find("@fprintf(ptr"), std::string::npos) << ir;
    }
}

