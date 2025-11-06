// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: CodeGen Long Integer Arrays (i64)
 * Purpose: Ensure & arrays lower to i64 with proper fptosi/sitofp conversions.
 */
/*
Test: Integration.Codegen_LongArray_i64_StoresAndLoads
Inputs: Source compiled to IR
Code under test: Code generation (IR text)
Expected behavior: Alloca/getelementptr use i64; fptosi/sitofp present.
*/
TEST(Integration, Codegen_LongArray_i64_StoresAndLoads) {
    const char* src =
        "10 DIM B&(3)\n"
        "20 B&(2)=42.9\n"
        "30 PRINT B&(2)\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Alloca and GEP on i64
    ASSERT_NE(ir.find("alloca [4 x i64]"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [4 x i64]"), std::string::npos);
    // Store path uses fptosi to i64 then store i64
    ASSERT_NE(ir.find("fptosi double"), std::string::npos);
    ASSERT_NE(ir.find("store i64"), std::string::npos);
    // Load path uses load i64 and sitofp to double
    ASSERT_NE(ir.find("load i64, ptr"), std::string::npos);
    ASSERT_NE(ir.find("sitofp i64"), std::string::npos);
}

