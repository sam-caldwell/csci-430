// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.Arrays_And_DynamicPrint
 * Purpose: Exercise array element reads in expressions inside WHILE, and the
 *          dynamic integer detection split for non-constant numeric prints.
 */
TEST(CodeGenWhile, Arrays_And_DynamicPrint) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM N(2,2,2)\n"
        "21 DIM S$(2,2,2)\n"
        "30 I=1\n"
        "40 WHILE I<=1\n"
        "50 N(1,1,1)=I+N(1,1,1)\n"
        "51 S$(1,1,1)=\"P\"\n"
        "52 PRINT N(1,1,1), \" Q\", S$(1,1,1)\n"
        "53 PRINT I+0, I+0.5\n"
        "54 I=I+1\n"
        "60 WEND\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    // Array element access triggers gep computations
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    // Dynamic integer detection branches present
    ASSERT_NE(ir.find("_wprint_int_"), std::string::npos);
    ASSERT_NE(ir.find("_wprint_flt_"), std::string::npos);
}
