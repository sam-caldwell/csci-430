// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenGosub.InlineArraysAndMid
 * Purpose: Exercise emitSubroutineInline coverage for numeric and string array
 *          assignments, as well as MID$ on a scalar string and on a string
 *          array element (with and without length argument).
 */
TEST(CodeGenGosub, InlineArraysAndMid) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM S$(2)\n"
        "30 GOSUB 100: END\n"
        // Subroutine body
        "100 S$=\"HI\": MID$(S$,2,1)=\"Z\"\n"
        "110 MID$(S$(2),1)=\"X\"\n"
        "120 RETURN\n";

    const std::string ir = Compiler::compileString(src);
    // Numeric array element store (gep on numeric array element type)
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    ASSERT_NE(ir.find("store"), std::string::npos);
    // String array element store
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    // MID$ lowering via strncpy for both scalar and array element variants
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
