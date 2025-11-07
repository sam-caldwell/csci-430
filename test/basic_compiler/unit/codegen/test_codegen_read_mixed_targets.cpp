// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenLineBlock.READ_MixedTargets
 * Purpose: Exercise READ codegen for string and numeric targets, including array
 *          elements and scalars, to traverse both string and numeric paths.
 */
TEST(CodeGenLineBlock, READ_MixedTargets) {
    const char* src =
        "10 DATA \"A\", 1, \"B\", 2\n"
        "20 OPTION BASE 1\n"
        "30 DIM A(2)\n"
        "31 DIM S$(2)\n"
        "40 READ S$(1), A(2), S$, A\n"
        "50 END\n";
    std::string ir = Compiler::compileString(src);
    // Evidence of DATA backing tables and typed stores
    ASSERT_NE(ir.find("@gwb_data"), std::string::npos);          // pointer table to string literals
    ASSERT_NE(ir.find("@gwb_data_isstr"), std::string::npos);    // is-string flags
    ASSERT_NE(ir.find("@gwb_data_num"), std::string::npos);      // numeric value table
    ASSERT_NE(ir.find("store ptr"), std::string::npos);
    bool hasNumStore = ir.find("store i16 ") != std::string::npos ||
                       ir.find("store i32 ") != std::string::npos ||
                       ir.find("store float ") != std::string::npos ||
                       ir.find("store double ") != std::string::npos;
    ASSERT_TRUE(hasNumStore);
}
