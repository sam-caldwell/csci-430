// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenFor.PrintUsing_Breadth
 * Purpose: Cover PRINT USING with/without channel as first, middle, and last items,
 *          and exercise nextStartsWithSpace to select *_ns formats; traverse both
 *          integer and float branches in loop body.
 */
TEST(CodeGenFor, PrintUsing_Breadth) {
    const auto src =
        "10 FOR I=1 TO 1\n"
        "20 PRINT USING(\"%d\"), I, \" X\"\n"
        "30 NEXT I\n"
        "40 FOR J=1 TO 1\n"
        "50 PRINT #1, USING(\"%f\"), 2.5, \" Y\", J+0.1\n"
        "60 NEXT J\n"
        "100 END\n";
    std::string ir = Compiler::compileString(src);
    // Using overrides and channel/stdio paths
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
    // format selection symbols exist
    ASSERT_NE(ir.find("@.fmt_int_ns"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
    // Integer detection branch labels
    ASSERT_NE(ir.find("_print_int_"), std::string::npos);
    ASSERT_NE(ir.find("_print_flt_"), std::string::npos);
}
