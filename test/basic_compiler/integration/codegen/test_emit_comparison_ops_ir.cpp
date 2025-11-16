// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.Comparison_All_Ops_IR
 * Purpose: Cover emit_comparison string and numeric paths for =, <>, <, <=, >, >=.
 */
TEST(Integration, Comparison_All_Ops_IR) {
    const char* src =
        "10 A=1: B=2\n"
        "20 IF A=B THEN 90\n"
        "30 IF A<>B THEN 90\n"
        "40 IF A<B THEN 90\n"
        "50 IF A<=B THEN 90\n"
        "60 IF B>A THEN 90\n"
        "70 IF B>=A THEN 90\n"
        "80 PRINT \"NUM\"\n"
        "90 IF \"A\"=\"A\" THEN 130\n"
        "100 IF \"A\"<>\"B\" THEN 130\n"
        "110 IF \"A\"<\"B\" THEN 130\n"
        "120 IF \"A\"<=\"B\" THEN 130\n"
        "130 IF \"B\">\"A\" THEN 160\n"
        "140 IF \"B\">=\"A\" THEN 160\n"
        "150 PRINT \"STR\"\n"
        "160 END\n";
    const std::string ir = Compiler::compileString(src);
    // Numeric fcmp predicates should appear
    EXPECT_NE(ir.find("fcmp oeq double"), std::string::npos);
    EXPECT_NE(ir.find("fcmp one double"), std::string::npos);
    EXPECT_NE(ir.find("fcmp olt double"), std::string::npos);
    EXPECT_NE(ir.find("fcmp ole double"), std::string::npos);
    EXPECT_NE(ir.find("fcmp ogt double"), std::string::npos);
    EXPECT_NE(ir.find("fcmp oge double"), std::string::npos);
    // String path uses strcmp and icmp compare against 0
    EXPECT_NE(ir.find("@strcmp"), std::string::npos);
    EXPECT_NE(ir.find("icmp eq i32"), std::string::npos);
    EXPECT_NE(ir.find("icmp ne i32"), std::string::npos);
    EXPECT_NE(ir.find("icmp slt i32"), std::string::npos);
    EXPECT_NE(ir.find("icmp sle i32"), std::string::npos);
    EXPECT_NE(ir.find("icmp sgt i32"), std::string::npos);
    EXPECT_NE(ir.find("icmp sge i32"), std::string::npos);
}

