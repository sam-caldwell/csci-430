// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.LEN_Numeric_Array_IR_NoStrlen
 * Inputs: Program that DIMs a typed numeric array and prints LEN on an element
 * Code under test: Code generation for LEN on numeric array elements
 * Expected behavior: IR does not contain a call to strlen for LEN; result comes from a constant
 */
TEST(Integration, LEN_Numeric_Array_IR_NoStrlen) {
    auto const src =
        "10 DIM A%(3)\n"
        "20 PRINT LEN(A%(1))\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Header may declare strlen; ensure we do not call it for LEN numeric
    ASSERT_EQ(ir.find("call i64 @strlen(ptr"), std::string::npos);
    // Should embed a floating constant 2.0 for i16 sized variables
    ASSERT_NE(ir.find("2.0"), std::string::npos);
}
