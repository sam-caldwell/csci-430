// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Integration.LEN_Numeric_Array_Single_IR
Inputs: Program DIMs a SINGLE (!) array and prints LEN of an element
Code under test: Code generation for LEN on SINGLE array element
Expected behavior: No strlen call; IR contains constant 4.0
*/
TEST(Integration, LEN_Numeric_Array_Single_IR) {
    const char* src =
        "10 DIM B!(3)\n"
        "20 PRINT LEN(B!(1))\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_EQ(ir.find("call i64 @strlen(ptr"), std::string::npos);
    ASSERT_NE(ir.find("4.0"), std::string::npos);
}

