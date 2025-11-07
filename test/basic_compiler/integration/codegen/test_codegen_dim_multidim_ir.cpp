// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Codegen_DIM_MultiDim_Alloc_UsesProduct
 * Purpose: Ensure DIM A(2,3) allocates total elements as the product of extents.
 * Components Under Test: Compiler::compileString; CodeGenerator multidimensional array allocation.
 * Expected Behavior: IR contains an alloca with [12 x <elem type>] for A(2,3) under OPTION BASE 0.
 */
TEST(Integration, Codegen_DIM_MultiDim_Alloc_UsesProduct) {
    // Default OPTION BASE 0 => extents are (3,4) for DIM A(2,3) => total 12
    const auto src =
        "10 DIM A(2,3)\n"
        "20 A(1,2)=5\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Default numeric kind is Single => float arrays
    ASSERT_NE(ir.find("alloca [12 x float]"), std::string::npos);
}
