// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(SemanticsRandomize, AcceptsNoArgOrNumeric) {
    const auto src1 =
        "10 RANDOMIZE\n"
        "20 END\n";
    std::string ir1 = Compiler::compileString(src1);
    ASSERT_NE(ir1.size(), 0u);
    const auto src2 =
        "10 RANDOMIZE 123\n"
        "20 END\n";
    std::string ir2 = Compiler::compileString(src2);
    ASSERT_NE(ir2.size(), 0u);
}

