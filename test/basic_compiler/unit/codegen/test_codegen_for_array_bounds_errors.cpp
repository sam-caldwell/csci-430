// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenFor.ArrayBoundsErrors
 * Purpose: Drive emitFor array assign error branches for both numeric and string arrays.
 */
TEST(CodeGenFor, ArrayBoundsErrors) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM A(2,2,2)\n"
        "21 DIM S$(2,2,2)\n"
        "30 FOR I=1 TO 1\n"
        "40 A(3,1,1)=7\n"    // upper OOB on first dim
        "50 S$(0,2,2)=\"X\"\n" // lower OOB on first dim (OPTION BASE 1)
        "60 NEXT I\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_for_arr_err_"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_err_code"), std::string::npos);
}
