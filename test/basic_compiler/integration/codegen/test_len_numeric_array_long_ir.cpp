// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.LEN_Numeric_Array_Long_IR
 * Inputs: Program DIMs a LONG (&) array and prints LEN of an element
 * Code under test: Code generation for LEN on LONG array element
 * Expected behavior: No strlen call; IR contains constant 4.0
 */
TEST(Integration, LEN_Numeric_Array_Long_IR) {
    auto const src =
        "10 DIM D&(3)\n"
        "20 PRINT LEN(D&(1))\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_EQ(ir.find("call i64 @strlen(ptr"), std::string::npos);
    ASSERT_NE(ir.find("4.0"), std::string::npos);
}
