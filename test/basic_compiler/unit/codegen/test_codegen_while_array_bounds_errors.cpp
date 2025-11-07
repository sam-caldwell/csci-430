// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.ArrayBoundsErrors
 * Purpose: Drive emitWhile array assign error branches for both numeric and string arrays.
 */
TEST(CodeGenWhile, ArrayBoundsErrors) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM B(2,2,2)\n"
        "21 DIM T$(2,2,2)\n"
        "30 X=0\n"
        "40 WHILE X<1\n"
        "50 B(2,3,2)=1\n"    // upper OOB on second dim
        "55 T$(1,1,0)=\"Q\"\n" // lower OOB on third dim
        "60 X=X+1\n"
        "70 WEND\n"
        "80 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_while_arr_err_"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_err_code"), std::string::npos);
}
