// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.READ_UsesTypedDataTables
 * Inputs: Program with mixed DATA then READ
 * Expected: IR emits @gwb_data_isstr and @gwb_data_num and does not call @atof
 */
TEST(Integration, READ_UsesTypedDataTables) {
    const std::string src = R"(10 DATA "X", 42
20 READ S$, A
)";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@gwb_data_isstr"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_data_num"), std::string::npos);
    // No atof-based parsing in READ anymore
    ASSERT_EQ(ir.find("@atof"), std::string::npos);
}
