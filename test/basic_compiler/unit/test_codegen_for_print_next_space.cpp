// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenFor.PrintNextStartsWithSpace
 * Purpose: Exercise nextStartsWithSpace path used to choose *_ns formats.
 */
TEST(CodeGenFor, PrintNextStartsWithSpace) {
    const auto src =
        "10 FOR I=1 TO 1\n"
        "20 PRINT 2, \" X\"\n"
        "30 NEXT I\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_int_ns"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
}

