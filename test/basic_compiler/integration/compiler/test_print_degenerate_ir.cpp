// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Integration.PRINT_Degenerate_Separators_Compile
Inputs: Program with "PRINT ;" and "PRINT ," forms
Code under test: Parser and codegen for degenerate PRINT separators/terminators
Expected behavior: Compiles successfully and emits printf calls
*/
TEST(Integration, PRINT_Degenerate_Separators_Compile) {
    const auto src =
        "10 PRINT ;\n"
        "20 PRINT ,\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Should at least reference printf in the IR header or body
    ASSERT_NE(ir.find("@printf"), std::string::npos);
}
