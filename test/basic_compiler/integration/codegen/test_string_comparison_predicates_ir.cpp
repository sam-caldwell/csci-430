// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.StringComparisonPredicatesICmp
 * Inputs: Program comparing strings with < and >=, then PRINT to keep codegen paths alive
 * Expected: IR uses @strcmp and icmp with correct signed predicates for strings.
 */
TEST(Integration, StringComparisonPredicatesICmp) {
    const auto src =
        "10 IF \"A\" < \"B\" THEN 20\n"
        "15 IF \"A\" >= \"A\" THEN 30\n"
        "20 PRINT 1\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // strcmp present
    ASSERT_NE(ir.find("@strcmp"), std::string::npos);
    // icmp slt for < and icmp sge for >= somewhere in IR
    ASSERT_NE(ir.find("icmp slt i32"), std::string::npos);
    ASSERT_NE(ir.find("icmp sge i32"), std::string::npos);
}

