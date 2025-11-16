// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.PrintNextStartsWithSpace
 * Purpose: Exercise nextStartsWithSpace selection for while-body print of constant ints/floats.
 */
TEST(CodeGenWhile, PrintNextStartsWithSpace) {
    const auto src =
        "10 I=0\n"
        "20 WHILE I<1\n"
        "30 PRINT 2, \" X\"\n"
        "40 I=I+1\n"
        "50 WEND\n"
        "60 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_int_ns"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
}
