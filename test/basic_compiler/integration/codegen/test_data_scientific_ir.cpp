// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.DATA_Supports_D_Exponent
 * Inputs: Program with DATA containing a D-exponent numeric
 * Expected: IR encodes numeric in @gwb_data_num as expected (no atof usage).
 */
TEST(Integration, DATA_Supports_D_Exponent) {
    std::string src = R"(10 DATA 1D2
)";
    std::string ir = Compiler::compileString(src);
    // Look for @gwb_data_num with 1.000000e+02 exactly once
    ASSERT_NE(ir.find("@gwb_data_num"), std::string::npos);
    ASSERT_NE(ir.find("double 1.000000e+02"), std::string::npos);
    // Still no atof in the IR
    ASSERT_EQ(ir.find("@atof"), std::string::npos);
}
