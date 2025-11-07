// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenLineBlock.SWAP_Arrays_And_Scalars
Purpose: Exercise SWAP for numeric array elements and string scalar/array
         combinations to cover both numeric and string swap paths.
*/
TEST(CodeGenLineBlock, SWAP_Arrays_And_Scalars) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM A%(2)\n"
        "21 DIM B&(2)\n"
        "22 DIM S$(2)\n"
        "23 DIM C!(2)\n"  // Single-precision to trigger fpext in swap
        "30 A%(1)=1: B&(2)=2\n"
        "31 S$=\"AA\": S$(2)=\"BB\"\n"
        "40 SWAP A%(1), B&(2)\n"
        "41 SWAP S$, S$(2)\n"
        "42 SWAP C!(1), A%(1)\n"
        "50 END\n";
    std::string ir = Compiler::compileString(src);
    // String swap uses load/store ptr
    ASSERT_NE(ir.find("load ptr, ptr"), std::string::npos);
    ASSERT_NE(ir.find("store ptr"), std::string::npos);
    // Numeric swap path converts to double via sitofp (ints) and/or fpext (floats)
    ASSERT_NE(ir.find("sitofp"), std::string::npos);
    // fpext should also be present due to swapping a Single (!)
    ASSERT_NE(ir.find("fpext"), std::string::npos);
}
