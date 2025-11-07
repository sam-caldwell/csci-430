// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Integration.LEN_Numeric_Var_IR_NoStrlen
Inputs: Program that assigns a numeric variable and prints LEN(var)
Code under test: Code generation for LEN on numeric variables
Expected behavior: IR does not contain a call to strlen for LEN; result is a constant literal
*/
TEST(Integration, LEN_Numeric_Var_IR_NoStrlen) {
    const char* src =
        "10 A%=1\n"
        "20 PRINT LEN(A%)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Ensure LEN(var) does not call strlen
    ASSERT_EQ(ir.find("call i64 @strlen(ptr"), std::string::npos);
    // Should embed a floating constant 2.0 for i16 sized variables
    ASSERT_NE(ir.find("2.0"), std::string::npos);
}

