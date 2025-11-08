// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: OptimizerPrint.ItemFolding_NoArithInIR
Inputs: PRINT with constant arithmetic expressions
Code under test: AstOptimizer folding of PRINT value/more items
Expected behavior: IR lacks fadd/fmul/fsub/fdiv for these items
*/
TEST(OptimizerPrint, ItemFolding_NoArithInIR) {
    const char* src =
        "10 PRINT 1+2, 3*4, 5-5, 8/1\n"
        "20 END\n";
    const std::string ir = Compiler::compileStringOptimized(src);
    ASSERT_NE(ir.find("printf"), std::string::npos);
    EXPECT_EQ(ir.find(" = fadd double"), std::string::npos);
    EXPECT_EQ(ir.find(" = fmul double"), std::string::npos);
    EXPECT_EQ(ir.find(" = fsub double"), std::string::npos);
    EXPECT_EQ(ir.find(" = fdiv double"), std::string::npos);
}

