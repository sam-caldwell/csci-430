// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Codegen_LongArray_i64_StoresAndLoads
 * Purpose: Ensure & arrays lower to i64 with correct fptosi/sitofp conversions on store/load.
 * Components Under Test: CodeGenerator numeric array typing and conversions.
 * Expected Behavior: IR includes alloca/getelementptr for [N x i64], fptosi for store, and sitofp after load.
 */
TEST(Integration, Codegen_LongArray_i64_StoresAndLoads) {
    const auto src =
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
