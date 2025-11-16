// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Codegen_Mixed_Array_Types_IR
 * Purpose: Demonstrate correct lowering and conversions across % (i32), & (i64), ! (float), # (double) arrays.
 * Components Under Test: CodeGenerator array typing and store/load conversions.
 * Expected Behavior: IR shows typed allocas, fptosi/fptrunc/store per type, and corresponding load/casts.
 */
TEST(Integration, Codegen_Mixed_Array_Types_IR) {
    const char* src =
        "10 DIM A%(3)\n"
        "11 DIM B&(3)\n"
        "12 DIM C!(3)\n"
        "13 DIM D#(3)\n"
        "20 A%(1)=1.5\n"
        "30 B&(1)=1.5\n"
        "40 C!(1)=1.5\n"
        "50 D#(1)=1.5\n"
        "60 PRINT A%(1),B&(1),C!(1),D#(1)\n"
        "70 END\n";
    const std::string ir = Compiler::compileString(src);
    // Typed allocations
    ASSERT_NE(ir.find("alloca [4 x i32]"), std::string::npos);   // A%
    ASSERT_NE(ir.find("alloca [4 x i64]"), std::string::npos);   // B&
    ASSERT_NE(ir.find("alloca [4 x float]"), std::string::npos); // C!
    ASSERT_NE(ir.find("alloca [4 x double]"), std::string::npos);// D#
    // Store conversions
    ASSERT_NE(ir.find("fptosi double"), std::string::npos); // integer paths
    ASSERT_NE(ir.find("fptrunc double"), std::string::npos); // single path
    ASSERT_NE(ir.find("store double"), std::string::npos); // double path
    // Load conversions
    ASSERT_NE(ir.find("load i32, ptr"), std::string::npos);
    ASSERT_NE(ir.find("load i64, ptr"), std::string::npos);
    ASSERT_NE(ir.find("load float, ptr"), std::string::npos);
    ASSERT_NE(ir.find("load double, ptr"), std::string::npos);
    ASSERT_NE(ir.find("sitofp i32"), std::string::npos);
    ASSERT_NE(ir.find("sitofp i64"), std::string::npos);
    ASSERT_NE(ir.find("fpext float"), std::string::npos);
}
