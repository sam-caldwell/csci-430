// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: CodeGen Integer Arrays (i32)
 * Purpose: Ensure % arrays lower to i32 with proper fptosi/sitofp conversions.
 */
TEST(Integration, Codegen_IntArray_i32_StoresAndLoads) {
    const auto src =
        "10 DIM A%(3)\n"
        "20 A%(1)=2.7\n"
        "30 PRINT A%(1)\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Alloca and GEP on i32
    ASSERT_NE(ir.find("alloca [4 x i32]"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [4 x i32]"), std::string::npos);
    // Store path uses fptosi to i32 then store i32
    ASSERT_NE(ir.find("fptosi double"), std::string::npos);
    ASSERT_NE(ir.find("store i32"), std::string::npos);
    // Load path uses load i32 and sitofp to double
    ASSERT_NE(ir.find("load i32, ptr"), std::string::npos);
    ASSERT_NE(ir.find("sitofp i32"), std::string::npos);
}

