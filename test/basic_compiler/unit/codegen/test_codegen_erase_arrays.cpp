// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenLineBlock.ERASE_Arrays_AllKinds
 * Purpose: Exercise ERASE handling in emit_line_block: numeric arrays of all
 *          kinds and a string array get reset appropriately.
 */
TEST(CodeGenLineBlock, ERASE_Arrays_AllKinds) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM A%(2,2)\n"
        "21 DIM B&(2)\n"
        "22 DIM C!(2)\n"
        "23 DIM D#(2)\n"
        "24 DIM S$(2)\n"
        "30 ERASE A%, B&, C!, D#, S$\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    // Evidence of array element addressing and stores for each numeric kind
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    ASSERT_NE(ir.find("store i32 0, ptr"), std::string::npos);
    ASSERT_NE(ir.find("store i64 0, ptr"), std::string::npos);
    ASSERT_NE(ir.find("store float 0.0, ptr"), std::string::npos);
    ASSERT_NE(ir.find("store double 0.0, ptr"), std::string::npos);
    // String array elements cleared to null
    ASSERT_NE(ir.find("store ptr null, ptr"), std::string::npos);
}
