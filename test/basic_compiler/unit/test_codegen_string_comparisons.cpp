// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: CodeGen String Comparisons
 * Purpose: Cover strcmp/icmp paths in emit_comparison
 */
TEST(CodeGenStrings, StringComparisonsUseStrcmp) {
    const auto src =
        "10 IF \"A\" = \"A\" THEN 20\n"
        "15 IF \"A\" < \"B\" THEN 30\n"
        "20 PRINT 1\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@.fmt_num"), std::string::npos);
    EXPECT_NE(ir.find("@strcmp"), std::string::npos);
}

